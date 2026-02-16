// ================================================================================
// ================================================================================
// - File:    c_string.c
// - purpose: this file contains the implementation for functions used in the 
//            c_string library
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 30, 2024
// - Version: 0.1
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <string.h>
#include "c_string.h"

/* SIMD backend selection for byte/char ops */

#if defined(__AVX512BW__) && defined(__AVX512VL__)
  #include <immintrin.h>
  #include "simd_avx512_char.inl"

#elif defined(__AVX2__)
  #include <immintrin.h>
  #include "simd_avx2_char.inl"

#elif defined(__AVX__)
  #include <immintrin.h>
  #include "simd_avx_char.inl"

#elif defined(__SSE4_1__)
  #include <immintrin.h>
  #include "simd_sse41_char.inl"

#elif defined(__SSE3__)
  #include <immintrin.h>
  #include "simd_sse3_char.inl"

#elif defined(__SSE2__)
  #include <immintrin.h>
  #include "simd_sse2_char.inl"

#elif defined(__ARM_FEATURE_SVE2)
  #include <arm_sve.h>
  #include "simd_sve2_char.inl"

#elif defined(__ARM_FEATURE_SVE)
  #include <arm_sve.h>
  #include "simd_sve_char.inl"

#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
  #include <arm_neon.h>
  #include "simd_neon_char.inl"

#else
  #include "simd_scalar_char.inl"
#endif

// ================================================================================ 
// ================================================================================ 

static inline string_expect_t string_error(error_code_t e) {
    string_expect_t out;
    out.has_value = false;
    out.u.error = e;
    return out;
}
// -------------------------------------------------------------------------------- 

string_expect_t init_string(const char* cstr,
                            size_t capacity_bytes,          // payload capacity (chars), excludes NUL
                            allocator_vtable_t allocator) {
    if (!cstr) return string_error(NULL_POINTER);
    if (!allocator.allocate || !allocator.return_element) return string_error(INVALID_ARG);

    size_t const src_len = strlen(cstr);                  // payload chars (no NUL)

    if (capacity_bytes == 0u) {
        capacity_bytes = src_len;                         // default: fit entire string
    }

    size_t const buf_bytes = capacity_bytes + 1u;         // always reserve NUL
    size_t const copy_len  = (src_len < capacity_bytes) ? src_len : capacity_bytes;

    // Allocate string object
    void_ptr_expect_t r = allocator.allocate(allocator.ctx, sizeof(string_t), true);
    if (!r.has_value) return string_error(r.u.error);

    string_t* s = (string_t*)r.u.value;

    // Allocate buffer
    r = allocator.allocate(allocator.ctx, buf_bytes, false);
    if (!r.has_value) {
        allocator.return_element(allocator.ctx, s);
        return string_error(r.u.error);
    }

    s->str       = (char*)r.u.value;
    s->len       = copy_len;
    s->alloc     = buf_bytes;        // store actual allocated bytes (recommended)
    s->allocator = allocator;

    if (copy_len > 0u) {
        memcpy(s->str, cstr, copy_len);
    }
    s->str[copy_len] = '\0';         // REQUIRED for truncation cases

    string_expect_t out;
    out.has_value = true;
    out.u.value = s;
    return out;
}
// -------------------------------------------------------------------------------- 

void return_string(string_t* s) {
    if (!s) return;

    // local copy avoids problems if someone overwrote s->allocator before free
    allocator_vtable_t a = s->allocator;

    if (s->str) {
        a.return_element(a.ctx, s->str);
        s->str = NULL;
    }

    a.return_element(a.ctx, s);
}
// -------------------------------------------------------------------------------- 

bool str_concat(string_t* s, const char* str) {
    if (!s || !s->str || !str) return false;

    allocator_vtable_t* a = &s->allocator;

    // We need at least allocate + return_element; reallocate is optional (fallback below).
    if (!a->allocate || !a->return_element) return false;

    size_t const len1 = s->len;
    size_t const len2 = strlen(str);

    if (len2 == 0u) return true;

    // overflow guard: len1 + len2 + 1
    if (len2 > (SIZE_MAX - 1u - len1)) return false;

    size_t const needed = len1 + len2 + 1u; // bytes including NUL

    // Check whether source pointer aliases current string buffer (including NUL)
    const char* const s_begin = s->str;
    const char* const s_end   = s->str + (len1 + 1u);
    bool const overlaps = (str >= s_begin && str < s_end);

    const char* src = str;
    char* temp = NULL;

    // If we must grow and src aliases s->str, we must copy src aside before growing.
    if (needed > s->alloc && overlaps) {
        void_ptr_expect_t tr = a->allocate(a->ctx, len2, false);
        if (!tr.has_value) return false;

        temp = (char*)tr.u.value;
        memcpy(temp, str, len2);
        src = temp;
    }

    // Ensure capacity
    if (needed > s->alloc) {
        if (a->reallocate) {
            // Preferred: reallocate in-place or move
            void_ptr_expect_t rr =
                a->reallocate(a->ctx, s->str, s->alloc, needed, false);
            if (!rr.has_value) {
                if (temp) a->return_element(a->ctx, temp);
                return false;
            }
            s->str = (char*)rr.u.value;
            s->alloc = needed;
        } else {
            // Fallback: allocate new buffer + copy old + append + (attempt) return old buffer
            void_ptr_expect_t nr = a->allocate(a->ctx, needed, false);
            if (!nr.has_value) {
                if (temp) a->return_element(a->ctx, temp);
                return false;
            }

            char* newbuf = (char*)nr.u.value;

            // Copy old content including NUL
            memcpy(newbuf, s->str, len1 + 1u);

            // Best-effort return old buffer (no-op for arenas)
            a->return_element(a->ctx, s->str);

            s->str = newbuf;
            s->alloc = needed;
        }
    }

    // Append safely (memmove handles overlap)
    memmove(s->str + len1, src, len2);
    s->str[len1 + len2] = '\0';
    s->len = len1 + len2;

    if (temp) a->return_element(a->ctx, temp);
    return true;
}
// -------------------------------------------------------------------------------- 

bool string_concat(string_t* s, const string_t* str) {
    if (!str) return false;
    return str_concat(s, const_string(str));
}
// -------------------------------------------------------------------------------- 

int8_t str_compare(const string_t* s, const char* str) {
    if ((s == NULL) || (s->str == NULL) || (str == NULL)) {
        return ((int8_t)-128);
    }

    /* Compare up to s->len characters, but stop early if str ends. */
    for (size_t i = 0u; i < s->len; ++i) {
        uint8_t const a = (uint8_t)(unsigned char)s->str[i];
        uint8_t const b = (uint8_t)(unsigned char)str[i];

        /* If C string ended before s did, then s is greater (unless a is also '\0'). */
        if (b == (uint8_t)0u) {
            /* If s has '\0' here too, they match through s->len (treat as equal). */
            return (a == (uint8_t)0u) ? (int8_t)0 : (int8_t)1;
        }

        if (a < b) { return (int8_t)-1; }
        if (a > b) { return (int8_t) 1; }
    }

    /* All s->len characters matched. If str has more chars, s is shorter => less. */
    return (str[s->len] == '\0') ? (int8_t)0 : (int8_t)-1;
}
// --------------------------------------------------------------------------------

int8_t string_compare(const string_t* a, const string_t* b) {
    if ((a == NULL) || (b == NULL) || (a->str == NULL) || (b->str == NULL)) {
        return (int8_t)-128; /* STRCMP_ERROR */
    }

    size_t const n = (a->len < b->len) ? a->len : b->len;

    size_t const k = simd_first_diff_u8((const uint8_t*)a->str,
                                         (const uint8_t*)b->str,
                                         n);

    if (k < n) {
        uint8_t const x = (uint8_t)(unsigned char)a->str[k];
        uint8_t const y = (uint8_t)(unsigned char)b->str[k];
        return (x < y) ? (int8_t)-1 : (int8_t)1;
    }

    if (a->len < b->len) return (int8_t)-1;
    if (a->len > b->len) return (int8_t)1;
    return (int8_t)0;
}
// -------------------------------------------------------------------------------- 

string_expect_t copy_string(const string_t* s, allocator_vtable_t allocator) {
    if (!s || !s->str) return string_error(NULL_POINTER);
    return init_string(const_string(s), s->len, allocator); 
}
// -------------------------------------------------------------------------------- 

bool is_string_ptr(const string_t* s, const void* ptr) {
    if ((s == NULL) || (s->str == NULL) || (ptr == NULL)) {
        return false;
    }

    uintptr_t const begin = (uintptr_t)(const void*)s->str;
    uintptr_t const end   = begin + s->alloc;   /* one-past-end */
    uintptr_t const addr  = (uintptr_t)ptr;

    /* overflow-safe containment check */
    if (addr < begin) {
        return false;
    }

    if (addr >= end) {
        return false;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

bool is_string_ptr_sized(const string_t* s, const void* ptr, size_t bytes) {
    if ((s == NULL) || (s->str == NULL) || (ptr == NULL) || (bytes == 0u)) {
        return false;
    }

    uintptr_t const begin = (uintptr_t)(const void*)s->str;
    uintptr_t const end   = begin + s->alloc; /* one-past-end */
    uintptr_t const p     = (uintptr_t)ptr;

    /* ptr must be within [begin, end) */
    if ((p < begin) || (p >= end)) {
        return false;
    }

    /* Overflow-safe: require bytes <= end - p  (equiv to p+bytes <= end) */
    if (bytes > (size_t)(end - p)) {
        return false;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

static inline bool _range_within_alloc_(const string_t* s,
                                        const uint8_t* begin,
                                        const uint8_t* end) {
    if ((s == NULL) || (s->str == NULL) || (begin == NULL) || (end == NULL)) {
        return false;
    }

    uintptr_t const base = (uintptr_t)(const void*)s->str;
    uintptr_t const lim  = base + s->alloc;          /* one-past-end of allocation */
    uintptr_t const b    = (uintptr_t)(const void*)begin;
    uintptr_t const e    = (uintptr_t)(const void*)end;

    /* monotonic and inside allocation */
    if ((b < base) || (e < b) || (e > lim)) {
        return false;
    }
    return true;
}

size_t find_substr(const string_t* haystack,
                   const string_t* needle,
                   const uint8_t*  begin,
                   const uint8_t*  end,
                   direction_t     dir) {
    if ((haystack == NULL) || (needle == NULL) ||
        (haystack->str == NULL) || (needle->str == NULL))
    {
        return SIZE_MAX;
    }

    const uint8_t* const hs_base     = (const uint8_t*)(const void*)haystack->str;
    const uint8_t* const hs_used_end = hs_base + haystack->len; /* exclude terminator */

    /* Defaults if begin/end omitted */
    if (begin == NULL) { begin = hs_base; }
    if (end   == NULL) { end   = hs_used_end; }

    /* begin/end must be inside allocation */
    if (!_range_within_alloc_(haystack, begin, end)) {
        return SIZE_MAX;
    }

    /* Clamp to used region (typical string-search semantics) */
    if (begin > hs_used_end) { return SIZE_MAX; }
    if (end   > hs_used_end) { end = hs_used_end; }
    if (begin > end)         { return SIZE_MAX; }

    size_t const region_len = (size_t)(end - begin);
    size_t const nlen       = needle->len;

    /* Empty needle: define as found at start of region */
    if (nlen == 0u) {
        return 0u;
    }
    if (nlen > region_len) {
        return SIZE_MAX;
    }

    /* Delegate to SIMD/scalar implementation */
    return simd_find_substr_u8(begin, region_len,
                               (const uint8_t*)(const void*)needle->str, nlen,
                               dir);
}
// ================================================================================
// ================================================================================
// eof
