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
    const uint8_t* const hs_used_end = hs_base + haystack->len;
    
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
    size_t offset_from_search_start = simd_find_substr_u8(begin, region_len,
                                                          (const uint8_t*)(const void*)needle->str, nlen,
                                                          dir);
    
    /* Convert to offset from string start */
    if (offset_from_search_start == SIZE_MAX) {
        return SIZE_MAX;
    }
    
    return (size_t)(begin - hs_base) + offset_from_search_start;
}
// -------------------------------------------------------------------------------- 

size_t find_substr_lit(const string_t* haystack,
                       const char*     needle_lit,
                       const uint8_t*  begin,
                       const uint8_t*  end,
                       direction_t     dir) {
    if ((haystack == NULL) || (haystack->str == NULL) || (needle_lit == NULL)) {
        return SIZE_MAX;
    }

    const uint8_t* const hs_base     = (const uint8_t*)(const void*)haystack->str;
    const uint8_t* const hs_used_end = hs_base + haystack->len;

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

    /* NUL-terminated literal length */
    size_t const nlen = strlen(needle_lit);

    /* Empty needle: define as found at start of region */
    if (nlen == 0u) {
        return (size_t)(begin - hs_base);
    }
    if (nlen > region_len) {
        return SIZE_MAX;
    }

    /* Delegate to SIMD/scalar implementation */
    size_t offset_from_search_start =
        simd_find_substr_u8(begin, region_len,
                            (const uint8_t*)(const void*)needle_lit, nlen,
                            dir);

    if (offset_from_search_start == SIZE_MAX) {
        return SIZE_MAX;
    }

    /* Convert to offset from string start */
    return (size_t)(begin - hs_base) + offset_from_search_start;
}
// -------------------------------------------------------------------------------- 

size_t word_count(const string_t* s,
                  const string_t* word,
                  const uint8_t*  start,
                  const uint8_t*  end) {
    if ((s == NULL) || (s->str == NULL))       return 0;
    if ((word == NULL) || (word->str == NULL)) return 0;

    /* Prevent infinite loop:
       find_substr() returns 0 for empty needle */
    if (word->len == 0u) return 0;

    const uint8_t* const base = (const uint8_t*)(const void*)s->str;

    /* Initialize cursor */
    const uint8_t* cur = (start != NULL) ? start : base;

    size_t count = 0;

    for (;;) {
        size_t pos = find_substr(s, word, cur, end, FORWARD);
        if (pos == SIZE_MAX) break;

        ++count;

        /* Advance past this match (non-overlapping semantics) */
        cur = base + pos + word->len;

        /* Stop if we've reached or exceeded the end window */
        if ((end != NULL) && (cur >= end)) break;
    }

    return count;
}
// -------------------------------------------------------------------------------- 

size_t word_count_lit(const string_t* s,
                      const char*     word,
                      const uint8_t*  start,
                      const uint8_t*  end) {
    if ((s == NULL) || (s->str == NULL)) return 0;
    if (word == NULL) return 0;

    const size_t wlen = strlen(word);
    if (wlen == 0u) return 0;

    /* Create a non-owning needle view */
    string_t needle;
    needle.str = (char*)(const void*)word;  /* adjust if string_t uses uint8_t* */
    needle.len = wlen;

    const uint8_t* const base = (const uint8_t*)(const void*)s->str;

    const uint8_t* cur = (start != NULL) ? start : base;

    size_t count = 0;

    for (;;) {
        size_t pos = find_substr(s, &needle, cur, end, FORWARD);
        if (pos == SIZE_MAX) break;

        ++count;

        /* Non-overlapping advancement */
        cur = base + pos + wlen;

        if ((end != NULL) && (cur >= end)) break;
    }

    return count;
}
// -------------------------------------------------------------------------------- 

size_t token_count_lit(const string_t* s,
                       const char*     delim,
                       const uint8_t*  begin,
                       const uint8_t*  end) {
    if ((s == NULL) || (s->str == NULL) || (delim == NULL)) {
        return SIZE_MAX;
    }

    const uint8_t* const base     = (const uint8_t*)(const void*)s->str;
    const uint8_t* const used_end = base + s->len;

    /* Defaults if begin/end omitted */
    if (begin == NULL) { begin = base; }
    if (end   == NULL) { end   = used_end; }

    /* begin/end must be inside allocation */
    if (!_range_within_alloc_(s, begin, end)) {
        return SIZE_MAX;
    }

    /* Clamp to used region */
    if (begin > used_end) { return SIZE_MAX; }
    if (end   > used_end) { end = used_end; }
    if (begin >= end)     { return 0u; }

    const size_t n    = (size_t)(end - begin);
    const size_t dlen = strlen(delim);

    /* No delimiters => entire window is 1 token if non-empty */
    if (dlen == 0u) {
        return (n == 0u) ? 0u : 1u;
    }

    return simd_token_count_u8(begin, n, delim, dlen);
}
// -------------------------------------------------------------------------------- 

size_t token_count(const string_t* s,
                   const string_t* delim,
                   const uint8_t*  begin,
                   const uint8_t*  end) {
    if ((s == NULL) || (s->str == NULL) ||
        (delim == NULL) || (delim->str == NULL))
    {
        return SIZE_MAX;
    }

    const uint8_t* const base     = (const uint8_t*)(const void*)s->str;
    const uint8_t* const used_end = base + s->len;

    /* Defaults if begin/end omitted */
    if (begin == NULL) { begin = base; }
    if (end   == NULL) { end   = used_end; }

    /* begin/end must be inside allocation */
    if (!_range_within_alloc_(s, begin, end)) {
        return SIZE_MAX;
    }

    /* Clamp to used region */
    if (begin > used_end) { return SIZE_MAX; }
    if (end   > used_end) { end = used_end; }
    if (begin >= end)     { return 0u; }

    const size_t n    = (size_t)(end - begin);
    const size_t dlen = delim->len;

    /* No delimiters => entire window is 1 token if non-empty */
    if (dlen == 0u) {
        return (n == 0u) ? 0u : 1u;
    }

    return simd_token_count_u8(begin, n,
                              (const char*)(const void*)delim->str, dlen);
}
// -------------------------------------------------------------------------------- 

void to_uppercase(string_t* s, uint8_t* start, uint8_t* end) {
    if ((s == NULL) || (s->str == NULL)) {
        return;
    }

    uint8_t* const base     = (uint8_t*)(void*)s->str;
    uint8_t* const used_end = base + s->len;

    if (start == NULL) { start = base; }
    if (end   == NULL) { end   = used_end; }

    /* Must be within allocation */
    if (!_range_within_alloc_(s, start, end)) {
        return;
    }

    /* Clamp to used region */
    if (start > used_end) { return; }
    if (end   > used_end) { end = used_end; }
    if (start >= end)     { return; }

    simd_ascii_upper_u8(start, (size_t)(end - start));
}
// -------------------------------------------------------------------------------- 

void to_lowercase(string_t* s, uint8_t* start, uint8_t* end) {
    if ((s == NULL) || (s->str == NULL)) {
        return;
    }

    uint8_t* const base     = (uint8_t*)(void*)s->str;
    uint8_t* const used_end = base + s->len;

    if (start == NULL) { start = base; }
    if (end   == NULL) { end   = used_end; }

    if (!_range_within_alloc_(s, start, end)) {
        return;
    }

    if (start > used_end) { return; }
    if (end   > used_end) { end = used_end; }
    if (start >= end)     { return; }

    simd_ascii_lower_u8(start, (size_t)(end - start));
}
// -------------------------------------------------------------------------------- 

void drop_substr_lit(string_t* s,
                     const char* substring,
                     uint8_t* begin,
                     uint8_t* end) {
    if ((s == NULL) || (s->str == NULL) || (substring == NULL)) return;

    uint8_t* const base     = (uint8_t*)(void*)s->str;
    uint8_t* used_end = base + s->len;         /* end exclusive */

    size_t const n = strlen(substring);
    if (n == 0u) return;                             /* define empty needle as no-op */

    /* defaults */
    if (begin == NULL) begin = base;
    if (end   == NULL) end   = used_end;

    /* validate against allocation, then clamp to used region */
    if (!_range_within_alloc_(s, begin, end)) return;

    if (begin > used_end) return;
    if (end   > used_end) end = used_end;
    if (begin >= end) return;

    /* We search in REVERSE order to minimize memmove sizes. */
    while (true) {
        size_t region_len = (size_t)(end - begin);
        if (region_len < n) break;

        size_t hit_off = find_substr_lit(s, substring, begin, end, REVERSE);
        if (hit_off == SIZE_MAX) break;

        /* hit pointer (still valid at this moment) */
        uint8_t* hit = base + hit_off;

        /* Ensure the match is fully inside current used content */
        if ((hit_off + n) > s->len) break; /* should not happen if find_substr_lit is correct */

        size_t drop_len = n;

        /* Optional: also drop ONE trailing space after the match */
        if ((hit_off + drop_len) < s->len) {
            if (base[hit_off + drop_len] == (uint8_t)' ') {
                drop_len += 1u;
            }
        }

        /* Move suffix INCLUDING terminating NUL (assuming s->str is NUL-terminated) */
        size_t const src_off = hit_off + drop_len;
        size_t const bytes_to_move = (s->len + 1u) - src_off; /* +1 includes '\0' */
        memmove(hit, base + src_off, bytes_to_move);

        s->len -= drop_len;

        /* Adjust window end to remain within new used_end */
        used_end = base + s->len; /* (recompute conceptually; base unchanged) */

        if (end > used_end) end = used_end;

        /* Ensure end is still >= begin */
        if (begin > end) break;

        /* Continue searching reverse within [begin,end) */
    }
}
// -------------------------------------------------------------------------------- 

void drop_substr(string_t* s,
                 const string_t* substring,
                 uint8_t* begin,
                 uint8_t* end) {
    /* ---------- basic validation ---------- */
    if ((s == NULL) || (s->str == NULL) ||
        (substring == NULL) || (substring->str == NULL))
    {
        return;
    }

    uint8_t* const base     = (uint8_t*)(void*)s->str;
    uint8_t*       used_end = base + s->len;   /* end-exclusive */

    size_t const n = substring->len;
    if (n == 0u) return;                       /* empty needle = no-op */

    /* ---------- default window ---------- */
    if (begin == NULL) begin = base;
    if (end   == NULL) end   = used_end;

    /* ---------- validate allocation range ---------- */
    if (!_range_within_alloc_(s, begin, end)) return;

    /* ---------- clamp to used region ---------- */
    if (begin > used_end) return;
    if (end   > used_end) end = used_end;
    if (begin >= end) return;

    /* ---------- reverse-search removal loop ---------- */
    for (;;) {
        size_t const region_len = (size_t)(end - begin);
        if (region_len < n) break;

        size_t hit_off = find_substr(s, substring, begin, end, REVERSE);
        if (hit_off == SIZE_MAX) break;

        /* pointer to hit */
        uint8_t* hit = base + hit_off;

        /* safety: match must be fully inside used length */
        if ((hit_off + n) > s->len) break;

        size_t drop_len = n;

        /* ---------- optional: drop ONE trailing ASCII space ---------- */
        if ((hit_off + drop_len) < s->len) {
            if (base[hit_off + drop_len] == (uint8_t)' ') {
                drop_len += 1u;
            }
        }

        /* ---------- shift suffix left (include terminating NUL) ---------- */
        size_t const src_off        = hit_off + drop_len;
        size_t const bytes_to_move  = (s->len + 1u) - src_off;

        memmove(hit, base + src_off, bytes_to_move);

        /* ---------- shrink logical length ---------- */
        s->len -= drop_len;
        used_end = base + s->len;

        /* ---------- clamp window end to new used length ---------- */
        if (end > used_end) end = used_end;
        if (begin > end) break;
    }
}
// -------------------------------------------------------------------------------- 

static bool _string_reserve_(string_t* s, size_t new_used_len) {
    /* new_used_len excludes NUL; alloc must be >= new_used_len+1 */
    size_t need = new_used_len + 1u;
    if (need <= s->alloc) return true;
    //if (!s->allocator) return false;


    void_ptr_expect_t p = s->allocator.reallocate(s->allocator.ctx, 
                                                  s->str, 
                                                  s->alloc, 
                                                  need, 
                                                  false);
    if (!p.has_value) return false;
    s->str = (char*)p.u.value;
    s->alloc = need;
    return true;
}

/**
 * @brief Replace all non-overlapping occurrences of a literal substring in-place.
 *
 * Replaces every **case-sensitive, non-overlapping** occurrence of the
 * NUL-terminated C string @p pattern with @p replace_string inside the
 * byte window `[min_ptr, max_ptr)` of @p string.
 *
 * The replacement is performed **in-place** using allocator-aware resizing:
 *
 * 1. The number of matches is determined using @ref word_count_lit.
 * 2. The final string length is computed before modification.
 * 3. If necessary, the string buffer is **reallocated once** via the
 *    associated allocator.
 * 4. Matches are processed using **reverse search** (@ref find_substr_lit with
 *    `REVERSE`) to minimize the total amount of `memmove` shifting.
 *
 * @param string
 * Pointer to the destination @ref string_t to modify.
 *
 * @param pattern
 * NUL-terminated substring to search for.
 *
 * @param replace_string
 * NUL-terminated replacement substring.
 *
 * @param min_ptr
 * Optional pointer to the first byte of the replacement window within
 * `string->str`.  
 * If `NULL`, the window begins at the start of the used string.
 *
 * @param max_ptr
 * Optional pointer to one-past-the-last byte of the replacement window.  
 * If `NULL`, the window extends to the end of the used string.
 *
 * @return
 * `true` if the operation completed successfully or no replacements were
 * required.  
 * `false` if:
 * - any argument is invalid
 * - the window lies outside the string allocation
 * - memory reallocation fails
 *
 * @note
 * - Matching is **case-sensitive** and **substring-based**.
 * - Replacements are **non-overlapping**.
 * - The window is interpreted as `[min_ptr, max_ptr)` (end exclusive).
 * - The terminating NUL byte is always preserved.
 * - On failure, the original string contents remain unchanged.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("red green red blue", 0u, a);
 * if (!r.has_value) {
 *     // handle allocation failure
 * }
 *
 * string_t* s = r.u.value;
 *
 * replace_substr_lit(s, "red", "yellow", NULL, NULL);
 * // Result: "yellow green yellow blue"
 *
 * return_string(s);
 * @endcode
 *
 * @see replace_substr
 * @see find_substr_lit
 * @see word_count_lit
 */
bool replace_substr_lit(string_t* s,
                        const char* pattern,
                        const char* replace_string,
                        uint8_t* begin,
                        uint8_t* end) {
    if ((s == NULL) || (s->str == NULL) || (pattern == NULL) || (replace_string == NULL)) {
        return false;
    }

    uint8_t* base     = (uint8_t*)(void*)s->str;
    uint8_t* used_end = base + s->len;          /* end exclusive */

    /* defaults */
    if (begin == NULL) begin = base;
    if (end   == NULL) end   = used_end;

    /* validate allocation bounds; then clamp to used region */
    if (!_range_within_alloc_(s, begin, end)) return false;

    if (begin > used_end) return false;
    if (end   > used_end) end = used_end;
    if (begin >= end)     return true;          /* empty window => nothing to do */

    size_t pat_len = strlen(pattern);
    if (pat_len == 0u) return true;             /* define empty pattern as no-op */

    size_t rep_len = strlen(replace_string);

    /* 1) count matches in window */
    size_t k = word_count_lit(s, pattern, begin, end);
    if (k == 0u) return true;

    /* 2) compute new length after replacement */
    size_t window_len = (size_t)(end - begin);
    /* guaranteed: pat_len <= window_len if k>0 */
    size_t new_window_len = window_len + k * (rep_len - pat_len);
    size_t prefix_len = (size_t)(begin - base);
    size_t suffix_len = (size_t)(used_end - end);

    size_t new_len = prefix_len + new_window_len + suffix_len;

    /* 3) ensure capacity (single grow) */
    if (!_string_reserve_(s, new_len)) return false;

    /* IMPORTANT: base pointers may have moved after realloc */
    base     = (uint8_t*)(void*)s->str;
    begin    = base + prefix_len;
    end      = begin + window_len;              /* old window end in original content */
    used_end = base + s->len;

    /* 4) do replacements from right-to-left in the ORIGINAL window content */
    /* We'll update s->len as we go; compute the final used_end after edits. */
    size_t cur_len = s->len;
    //size_t delta   = rep_len - pat_len;         /* may be negative via wrap; handle separately */

    /* We keep a moving "search_end" that corresponds to the original content window. */
    uint8_t* search_end = end;

    for (size_t t = 0; t < k; ++t) {
        size_t hit_off = find_substr_lit(s, pattern, begin, search_end, REVERSE);
        if (hit_off == SIZE_MAX) {
            /* Should not happen if word_count_lit and find_substr_lit agree */
            break;
        }

        uint8_t* hit = base + hit_off;

        /* Position immediately after the match in current buffer */
        size_t after_match_off = hit_off + pat_len;

        /* Shift tail to make room (or close gap), including NUL. */
        if (rep_len != pat_len) {
            /* Tail begins after the matched pattern */
            size_t bytes_tail = (cur_len + 1u) - after_match_off; /* +1 includes NUL */

            if (rep_len > pat_len) {
                size_t grow = rep_len - pat_len;
                memmove(hit + rep_len, base + after_match_off, bytes_tail);
                cur_len += grow;
            } else {
                size_t shrink = pat_len - rep_len;
                memmove(hit + rep_len, base + after_match_off, bytes_tail);
                cur_len -= shrink;
            }
        }

        /* Copy replacement bytes into the gap */
        if (rep_len != 0u) {
            memcpy(hit, replace_string, rep_len);
        }

        /* Next reverse search should not re-find this same region.
           Set search_end to hit (end-exclusive), so next search is left of this hit. */
        search_end = hit;

        if (search_end <= begin) break;
    }

    s->len = cur_len;
    s->str[s->len] = '\0';
    return true;
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Replace all non-overlapping occurrences of a substring in-place.
 *
 * Replaces every **case-sensitive, non-overlapping** occurrence of
 * @p pattern with @p replace_string inside the byte window
 * `[min_ptr, max_ptr)` of @p string.
 *
 * This function is the @ref string_t-based counterpart to
 * @ref replace_substr_lit and follows the same allocator-aware algorithm:
 *
 * 1. Match count determined using @ref word_count.
 * 2. Final length computed before modification.
 * 3. Buffer resized **once** via the string’s allocator if required.
 * 4. Replacement performed using **reverse search** (@ref find_substr with
 *    `REVERSE`) to minimize memory movement.
 *
 * @param string
 * Pointer to the destination @ref string_t to modify.
 *
 * @param pattern
 * Substring to search for.
 *
 * @param replace_string
 * Replacement substring.
 *
 * @param min_ptr
 * Optional pointer to the first byte of the replacement window within
 * `string->str`.  
 * If `NULL`, the window begins at the start of the used string.
 *
 * @param max_ptr
 * Optional pointer to one-past-the-last byte of the replacement window.  
 * If `NULL`, the window extends to the end of the used string.
 *
 * @return
 * `true` if the operation completed successfully or no replacements were
 * required.  
 * `false` if:
 * - any argument is invalid
 * - the window lies outside the string allocation
 * - memory reallocation fails
 *
 * @note
 * - Matching is **case-sensitive** and **substring-based**.
 * - Replacements are **non-overlapping**.
 * - The window is interpreted as `[min_ptr, max_ptr)` (end exclusive).
 * - The terminating NUL byte is preserved.
 * - On failure, the original string contents remain unchanged.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r1 = init_string("one two two three", 0u, a);
 * string_expect_t r2 = init_string("two", 0u, a);
 * string_expect_t r3 = init_string("four", 0u, a);
 *
 * if (r1.has_value && r2.has_value && r3.has_value) {
 *     string_t* s   = r1.u.value;
 *     string_t* pat = r2.u.value;
 *     string_t* rep = r3.u.value;
 *
 *     replace_substr(s, pat, rep, NULL, NULL);
 *     // Result: "one four four three"
 *
 *     return_string(rep);
 *     return_string(pat);
 *     return_string(s);
 * }
 * @endcode
 *
 * @see replace_substr_lit
 * @see find_substr
 * @see word_count
 */
bool replace_substr(string_t* s,
                    const string_t* pattern,
                    const string_t* replace_string,
                    char* min_ptr,
                    char* max_ptr) {
    if ((s == NULL) || (s->str == NULL) ||
        (pattern == NULL) || (pattern->str == NULL) ||
        (replace_string == NULL) || (replace_string->str == NULL))
    {
        return false;
    }

    /* Define empty pattern as no-op */
    size_t const pat_len = pattern->len;
    if (pat_len == 0u) return true;

    size_t const rep_len = replace_string->len;

    uint8_t* base_u     = (uint8_t*)(void*)s->str;
    uint8_t* used_end_u = base_u + s->len;                 /* end-exclusive */

    /* defaults */
    if (min_ptr == NULL) min_ptr = s->str;
    if (max_ptr == NULL) max_ptr = s->str + s->len;        /* end-exclusive */

    uint8_t* begin_u = (uint8_t*)(void*)min_ptr;
    uint8_t* end_u   = (uint8_t*)(void*)max_ptr;

    /* Validate begin/end are within allocation, then clamp to used */
    if (!_range_within_alloc_(s, begin_u, end_u)) return false;

    if (begin_u > used_end_u) return false;
    if (end_u   > used_end_u) end_u = used_end_u;
    if (begin_u >= end_u)     return true;                 /* empty window => nothing */

    size_t const window_len = (size_t)(end_u - begin_u);
    if (window_len < pat_len) return true;

    /* 1) count matches */
    size_t const k = word_count(s, pattern, begin_u, end_u);
    if (k == 0u) return true;

    /* 2) compute final new length */
    size_t const prefix_len = (size_t)(begin_u - base_u);
    size_t const suffix_len = (size_t)(used_end_u - end_u);

    /* new_window_len = window_len + k*(rep_len - pat_len) */
    size_t new_window_len;
    if (rep_len >= pat_len) {
        new_window_len = window_len + k * (rep_len - pat_len);
    } else {
        new_window_len = window_len - k * (pat_len - rep_len);
    }

    size_t const new_len = prefix_len + new_window_len + suffix_len;

    /* 3) reserve/grow once if needed */
    if (!_string_reserve_(s, new_len)) return false;

    /* Rebase pointers after possible realloc */
    base_u     = (uint8_t*)(void*)s->str;
    begin_u    = base_u + prefix_len;
    end_u      = begin_u + window_len;     /* end of original content window */
    used_end_u = base_u + s->len;

    /* 4) replace right-to-left to minimize memmove in worst cases */
    size_t cur_len = s->len;
    uint8_t* search_end = end_u;           /* end-exclusive */

    for (size_t t = 0; t < k; ++t) {
        size_t hit_off = find_substr(s, pattern, begin_u, search_end, REVERSE);
        if (hit_off == SIZE_MAX) break; /* should not happen if count and find agree */

        uint8_t* hit_u = base_u + hit_off;

        /* offset just after the matched pattern in current buffer */
        size_t after_match_off = hit_off + pat_len;

        /* shift tail (including NUL) if sizes differ */
        if (rep_len != pat_len) {
            size_t const bytes_tail = (cur_len + 1u) - after_match_off;

            if (rep_len > pat_len) {
                size_t const grow = rep_len - pat_len;
                memmove(hit_u + rep_len, base_u + after_match_off, bytes_tail);
                cur_len += grow;
            } else {
                size_t const shrink = pat_len - rep_len;
                memmove(hit_u + rep_len, base_u + after_match_off, bytes_tail);
                cur_len -= shrink;
            }
        }

        /* copy replacement bytes */
        if (rep_len != 0u) {
            memcpy(hit_u, replace_string->str, rep_len);
        }

        /* Next reverse search stays strictly left of this hit */
        search_end = hit_u;
        if (search_end <= begin_u) break;
    }

    s->len = cur_len;
    s->str[s->len] = '\0';

    return true;
}
// ================================================================================
// ================================================================================
// eof
