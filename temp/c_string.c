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
// -------------------------------------------------------------------------------- 

string_expect_t pop_str_token(string_t* s,
                              const string_t* token,
                              allocator_vtable_t allocator) {
    if ((s == NULL) || (s->str == NULL)) {
        return string_error(NULL_POINTER);
    }
    if ((token == NULL) || (token->str == NULL)) {
        return string_error(NULL_POINTER);
    }
    if (token->len == 0u) {
        return string_error(INVALID_ARG);
    }
    if (s->len == 0u) {
        return string_error(PRECONDITION_FAIL);
    }

    size_t const tlen = token->len;

    /* Find last occurrence within used region */
    size_t const pos = find_substr(s, token, NULL, NULL, REVERSE);
    if (pos == SIZE_MAX) {
        return string_error(PRECONDITION_FAIL); /* token not found */
    }

    /* Compute RHS (to the right of token) */
    size_t const rhs_start = pos + tlen;
    if (rhs_start > s->len) {
        return string_error(PRECONDITION_FAIL); /* defensive */
    }

    size_t const rhs_len = s->len - rhs_start;
    const char* rhs_ptr  = s->str + rhs_start;

    /* Copy RHS into a new string (using provided allocator) */
    string_expect_t out = init_string(rhs_ptr, rhs_len, allocator);
    if (!out.has_value) {
        return out; /* propagate allocation failure */
    }

    /* Shrink original string to exclude token and RHS */
    s->len = pos;
    s->str[s->len] = '\0';

    return out;
}
// -------------------------------------------------------------------------------- 

string_expect_t pop_str_token_lit(string_t* s,
                                 const char* token,
                                 allocator_vtable_t allocator) {
    if ((s == NULL) || (s->str == NULL)) {
        return string_error(NULL_POINTER);
    }
    if ((token == NULL) || (token[0] == '\0')) {
        return string_error(INVALID_ARG);
    }
    if (s->len == 0u) {
        return string_error(PRECONDITION_FAIL);
    }

    size_t const tlen = strlen(token);
    if (tlen == 0u) {
        return string_error(INVALID_ARG);
    }

    /* Find last occurrence within used region */
    size_t const pos = find_substr_lit(s, token, NULL, NULL, REVERSE);
    if (pos == SIZE_MAX) {
        return string_error(PRECONDITION_FAIL); /* token not found */
    }

    /* Compute RHS (to the right of token) */
    size_t const rhs_start = pos + tlen;
    if (rhs_start > s->len) {
        return string_error(PRECONDITION_FAIL); /* defensive */
    }

    size_t const rhs_len = s->len - rhs_start;
    const char* rhs_ptr  = s->str + rhs_start;

    /* Copy RHS into a new string (using provided allocator) */
    string_expect_t out = init_string(rhs_ptr, rhs_len, allocator);
    if (!out.has_value) {
        return out; /* propagate allocation failure */
    }

    /* Shrink original string to exclude token and RHS */
    s->len = pos;
    s->str[s->len] = '\0';

    return out;
}
// -------------------------------------------------------------------------------- 

void print_string(const string_t* s, FILE* stream) {
    if (s == NULL || stream == NULL || s->str == NULL) {
        return;
    }

    size_t col = 0u;
    size_t i   = 0u;

    /* Opening quote */
    if (fputc('"', stream) == EOF) {
        return;
    }
    col = 1u;

    while (i < s->len) {
        size_t remaining = s->len - i;
        size_t chunk     = remaining;

        if (col >= 70u) {
            if (fputc('\n', stream) == EOF) {
                return;
            }
            col = 0u;
        }

        if ((col + chunk) > 70u) {
            chunk = 70u - col;
        }

        if (chunk == 0u) {
            if (fputc('\n', stream) == EOF) {
                return;
            }
            col = 0u;
            continue;
        }

        if (fwrite(s->str + i, 1u, chunk, stream) != chunk) {
            return;
        }

        i   += chunk;
        col += chunk;

        if ((col == 70u) && (i < s->len)) {
            if (fputc('\n', stream) == EOF) {
                return;
            }
            col = 0u;
        }
    }

    /* Closing quote */
    if (col + 1u > 70u) {
        if (fputc('\n', stream) == EOF) {
            return;
        }
    }

    fputc('"', stream);
}
// ================================================================================ 
// ================================================================================ 

/*
 * Read the string_t* stored in slot index of the backing buffer.
 * The backing buffer holds sizeof(string_t*) bytes per slot.
 * No bounds checking — callers must validate index before calling.
 */
static inline string_t* _read_slot(const str_array_t* array, size_t index) {
    string_t* ptr;
    memcpy(&ptr,
           array->base.data + index * sizeof(string_t*),
           sizeof(string_t*));
    return ptr;
}
 
/*
 * Write a string_t* into slot index of the backing buffer.
 */
static inline void _write_slot(str_array_t* array, size_t index, string_t* ptr) {
    memcpy(array->base.data + index * sizeof(string_t*),
           &ptr,
           sizeof(string_t*));
}
 
/*
 * qsort-compatible comparator for string_t* slots.
 * a and b point to string_t* values stored in the buffer.
 * Returns {-1, 0, 1} via str_compare, or 0 on NULL.
 */
static int _cmp_slot(const void* a, const void* b) {
    string_t* sa;
    string_t* sb;
    memcpy(&sa, a, sizeof(string_t*));
    memcpy(&sb, b, sizeof(string_t*));
    if (sa == NULL && sb == NULL) return  0;
    if (sa == NULL)               return -1;
    if (sb == NULL)               return  1;
    return (int)str_compare(sa, const_string(sb));
}
 
// ================================================================================
// Initialization and teardown
// ================================================================================

// str_array_expect_t init_str_array(size_t             capacity,
//                                   bool               growth,
//                                   allocator_vtable_t alloc_v) {
//     if (alloc_v.allocate == NULL || alloc_v.return_element == NULL)
//         return (str_array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
//     if (capacity == 0u)
//         return (str_array_expect_t){ .has_value = false, .u.error = INVALID_ARG };
//     void_ptr_expect_t sa_r = alloc_v.allocate(alloc_v.ctx,
//                                                sizeof(str_array_t), false);
//     if (!sa_r.has_value)
//         return (str_array_expect_t){ .has_value = false,
//                                      .u.error   = OUT_OF_MEMORY };
//     str_array_t* sa = (str_array_t*)sa_r.u.value;
//  
//     void_ptr_expect_t buf_r = alloc_v.allocate(alloc_v.ctx,
//                                                 capacity * sizeof(string_t*),
//                                                 false);
//     if (!buf_r.has_value) {
//         alloc_v.return_element(alloc_v.ctx, sa);
//         return (str_array_expect_t){ .has_value = false,
//                                      .u.error   = OUT_OF_MEMORY };
//     }
//  
//     sa->base.data      = (uint8_t*)buf_r.u.value;
//     sa->base.len       = 0u;
//     sa->base.alloc     = capacity;
//     sa->base.data_size = sizeof(string_t*);
//     sa->base.dtype     = STRING_TYPE;
//     sa->base.growth    = growth;
//     sa->base.alloc_v   = alloc_v;
//     sa->alloc          = alloc_v;
//  
//     return (str_array_expect_t){ .has_value = true, .u.value = sa };
// }
//  
// // --------------------------------------------------------------------------------
//  
// void return_str_array(str_array_t* array) {
//     if (array == NULL) return;
//  
//     /* Release every owned string_t. */
//     for (size_t i = 0u; i < array->base.len; i++) {
//         return_string(_read_slot(array, i));
//     }
//  
//     /* Free the backing buffer, then the wrapper struct. */
//     array->base.alloc_v.return_element(array->base.alloc_v.ctx,
//                                         array->base.data);
//     array->base.alloc_v.return_element(array->base.alloc_v.ctx, array);
// }
//  
// // ================================================================================
// // Internal growth helper
// // ================================================================================
//  
// /*
//  * Grow the backing buffer by doubling capacity when it is full and
//  * growth is enabled.  Returns NO_ERROR if growth was not needed or
//  * succeeded, CAPACITY_OVERFLOW if growth is disabled and the array is full.
//  */
// static error_code_t _ensure_capacity(str_array_t* array) {
//     if (array->base.len < array->base.alloc) return NO_ERROR;
//     if (!array->base.growth)                 return CAPACITY_OVERFLOW;
//  
//     size_t new_cap = array->base.alloc * 2u;
//     if (new_cap < array->base.alloc) return LENGTH_OVERFLOW;   /* overflow */
//  
//     void_ptr_expect_t r = array->base.alloc_v.allocate(array->base.alloc_v.ctx,
//                                                          new_cap * sizeof(string_t*),
//                                                          false);
//     if (!r.has_value) return OUT_OF_MEMORY;
//     void* new_buf = r.u.value;
//  
//     memcpy(new_buf, array->base.data,
//            array->base.len * sizeof(string_t*));
//  
//     array->base.alloc_v.return_element(array->base.alloc_v.ctx,
//                                         array->base.data);
//     array->base.data  = new_buf;
//     array->base.alloc = new_cap;
//     return NO_ERROR;
// }
//  
// // ================================================================================
// // Internal push shared path
// // ================================================================================
//  
// /*
//  * Validate string_expect_t, grow if needed, then write ptr into the slot
//  * chosen by the position tag (0 = back, 1 = front, 2 = at index).
//  *
//  * On failure the newly allocated string_t is released so nothing leaks.
//  */
// typedef enum { _POS_BACK = 0, _POS_FRONT = 1, _POS_AT = 2 } _push_pos_t;
//  
// static error_code_t _do_push(str_array_t*    array,
//                               string_expect_t se,
//                               _push_pos_t     pos,
//                               size_t          index) {
//     if (!se.has_value) return se.u.error;
//  
//     string_t* new_ptr = se.u.value;
//  
//     error_code_t err = _ensure_capacity(array);
//     if (err != NO_ERROR) {
//         return_string(new_ptr);
//         return err;
//     }
//  
//     size_t len = array->base.len;
//  
//     if (pos == _POS_BACK) {
//         _write_slot(array, len, new_ptr);
//     } else if (pos == _POS_FRONT) {
//         if (len > 0u) {
//             memmove(array->base.data + sizeof(string_t*),
//                     array->base.data,
//                     len * sizeof(string_t*));
//         }
//         _write_slot(array, 0u, new_ptr);
//     } else {
//         /* _POS_AT — index already validated by caller */
//         if (index > len) {
//             return_string(new_ptr);
//             return OUT_OF_BOUNDS;
//         }
//         if (index < len) {
//             memmove(array->base.data + (index + 1u) * sizeof(string_t*),
//                     array->base.data + index        * sizeof(string_t*),
//                     (len - index) * sizeof(string_t*));
//         }
//         _write_slot(array, index, new_ptr);
//     }
//  
//     array->base.len++;
//     return NO_ERROR;
// }
//  
// // ================================================================================
// // Push — string_t* source
// // ================================================================================
//  
// error_code_t push_back_str(str_array_t* array, const string_t* src) {
//     if (array == NULL || src == NULL) return NULL_POINTER;
//     return _do_push(array, copy_string(src, array->alloc), _POS_BACK, 0u);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t push_front_str(str_array_t* array, const string_t* src) {
//     if (array == NULL || src == NULL) return NULL_POINTER;
//     return _do_push(array, copy_string(src, array->alloc), _POS_FRONT, 0u);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t push_at_str(str_array_t* array, size_t index, const string_t* src) {
//     if (array == NULL || src == NULL) return NULL_POINTER;
//     if (index > array->base.len)      return OUT_OF_BOUNDS;
//     return _do_push(array, copy_string(src, array->alloc), _POS_AT, index);
// }
//  
// // ================================================================================
// // Push — C-string literal source
// // ================================================================================
//  
// error_code_t push_back_lit(str_array_t* array, const char* lit) {
//     if (array == NULL || lit == NULL) return NULL_POINTER;
//     return _do_push(array, init_string(lit, 0u, array->alloc), _POS_BACK, 0u);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t push_front_lit(str_array_t* array, const char* lit) {
//     if (array == NULL || lit == NULL) return NULL_POINTER;
//     return _do_push(array, init_string(lit, 0u, array->alloc), _POS_FRONT, 0u);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t push_at_lit(str_array_t* array, size_t index, const char* lit) {
//     if (array == NULL || lit == NULL) return NULL_POINTER;
//     if (index > array->base.len)      return OUT_OF_BOUNDS;
//     return _do_push(array, init_string(lit, 0u, array->alloc), _POS_AT, index);
// }
//  
// // ================================================================================
// // Get
// // ================================================================================
//
//
// string_expect_t get_str_array_index(const str_array_t* array, size_t index) {
//     if (array == NULL)
//         return (string_expect_t){ .has_value = false, .u.error = NULL_POINTER };
//     if (index >= array->base.len)
//         return (string_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };
//     return copy_string(_read_slot(array, index), array->alloc);
// }
//  
// // --------------------------------------------------------------------------------
//  
// string_expect_t get_str_array_ptr(const str_array_t* array, size_t index) {
//     if (array == NULL)
//         return (string_expect_t){ .has_value = false, .u.error = NULL_POINTER };
//     if (index >= array->base.len)
//         return (string_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };
//     return (string_expect_t){ .has_value = true, .u.value = _read_slot(array, index) };
// }
// // ================================================================================
// // Pop — shared implementation
// // ================================================================================
//  
// static error_code_t _pop_at(str_array_t* array, size_t index) {
//     if (array == NULL)            return NULL_POINTER;
//     if (array->base.len == 0u)    return EMPTY;
//     if (index >= array->base.len) return OUT_OF_BOUNDS;
//  
//     /* Release the owned string_t. */
//     return_string(_read_slot(array, index));
//  
//     /* Compact: shift slots above index down by one. */
//     size_t tail = array->base.len - index - 1u;
//     if (tail > 0u) {
//         memmove(array->base.data + index        * sizeof(string_t*),
//                 array->base.data + (index + 1u) * sizeof(string_t*),
//                 tail * sizeof(string_t*));
//     }
//     array->base.len--;
//     return NO_ERROR;
// }
//  
// error_code_t pop_back_str_array(str_array_t* array) {
//     if (array == NULL)          return NULL_POINTER;
//     if (array->base.len == 0u)  return EMPTY;
//     return _pop_at(array, array->base.len - 1u);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t pop_front_str_array(str_array_t* array) {
//     if (array == NULL)          return NULL_POINTER;
//     if (array->base.len == 0u)  return EMPTY;
//     return _pop_at(array, 0u);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t pop_any_str_array(str_array_t* array, size_t index) {
//     return _pop_at(array, index);
// }
//  
// // ================================================================================
// // Utility operations
// // ================================================================================
//  
// error_code_t clear_str_array(str_array_t* array) {
//     if (array == NULL) return NULL_POINTER;
//     for (size_t i = 0u; i < array->base.len; i++) {
//         return_string(_read_slot(array, i));
//     }
//     array->base.len = 0u;
//     return NO_ERROR;
// }
//  
// // --------------------------------------------------------------------------------
//  
// /*
//  * Shared set-slot path: validates bounds, releases the old pointer,
//  * then writes the new one.  On failure from init/copy the new pointer
//  * is released so nothing leaks.
//  */
// static error_code_t _set_slot(str_array_t* array, size_t index,
//                                string_expect_t se) {
//     if (!se.has_value) return se.u.error;
//     if (array == NULL) {
//         return_string(se.u.value);
//         return NULL_POINTER;
//     }
//     if (index >= array->base.len) {
//         return_string(se.u.value);
//         return OUT_OF_BOUNDS;
//     }
//  
//     /* Release the displaced pointer. */
//     return_string(_read_slot(array, index));
//  
//     /* Write the new one. */
//     _write_slot(array, index, se.u.value);
//     return NO_ERROR;
// }
//  
// error_code_t set_str_array_index_str(str_array_t*    array,
//                                      size_t          index,
//                                      const string_t* src) {
//     if (array == NULL || src == NULL) return NULL_POINTER;
//     return _set_slot(array, index, copy_string(src, array->alloc));
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t set_str_array_index_lit(str_array_t* array,
//                                      size_t       index,
//                                      const char*  lit) {
//     if (array == NULL || lit == NULL) return NULL_POINTER;
//     return _set_slot(array, index, init_string(lit, 0u, array->alloc));
// }
//  
// // --------------------------------------------------------------------------------
//  
// str_array_expect_t copy_str_array(const str_array_t* src,
//                                   allocator_vtable_t alloc_v) {
//     if (src == NULL)
//         return (str_array_expect_t){ .has_value = false,
//                                      .u.error   = NULL_POINTER };
//  
//     size_t cap = src->base.len > 0u ? src->base.len : 1u;
//     str_array_expect_t dr = init_str_array(cap, src->base.growth, alloc_v);
//     if (!dr.has_value) return dr;
//  
//     str_array_t* dst = dr.u.value;
//     for (size_t i = 0u; i < src->base.len; i++) {
//         error_code_t err = push_back_str(dst, _read_slot(src, i));
//         if (err != NO_ERROR) {
//             return_str_array(dst);
//             return (str_array_expect_t){ .has_value = false, .u.error = err };
//         }
//     }
//     return (str_array_expect_t){ .has_value = true, .u.value = dst };
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t reverse_str_array(str_array_t* array) {
//     if (array == NULL) return NULL_POINTER;
//     if (array->base.len < 2u) return NO_ERROR;
//  
//     size_t lo = 0u;
//     size_t hi = array->base.len - 1u;
//     string_t* tmp;
//  
//     while (lo < hi) {
//         tmp = _read_slot(array, lo);
//         _write_slot(array, lo, _read_slot(array, hi));
//         _write_slot(array, hi, tmp);
//         lo++;
//         hi--;
//     }
//     return NO_ERROR;
// }
//  
// // ================================================================================
// // Sort
// // ================================================================================
//
// /* Swap two elements of data_size bytes using a stack buffer. */
// static inline void _swap_elements(uint8_t* a, uint8_t* b, size_t data_size) {
//     uint8_t tmp[256];
//
//     if (data_size <= sizeof(tmp)) {
//         memcpy(tmp, a,   data_size);
//         memcpy(a,   b,   data_size);
//         memcpy(b,   tmp, data_size);
//     } else {
//         /* data_size > 256: byte-by-byte swap to avoid VLA */
//         for (size_t i = 0u; i < data_size; i++) {
//             uint8_t byte = a[i];
//             a[i] = b[i];
//             b[i] = byte;
//         }
//     }
// }
//
// // --------------------------------------------------------------------------------
//
// /* Apply direction to a raw comparator result. */
// static inline int _apply_dir(int cmp_result, direction_t dir) {
//     return (dir == FORWARD) ? cmp_result : -cmp_result;
// }
//
// // --------------------------------------------------------------------------------
//
// /* Median-of-three pivot selection. Returns pointer to the chosen element. */
// static uint8_t* _median_of_three(uint8_t*  a,
//                                   uint8_t*  b,
//                                   uint8_t*  c,
//                                   int     (*cmp)(const void*, const void*),
//                                   direction_t  dir) {
//     int ab = _apply_dir(cmp(a, b), dir);
//     int bc = _apply_dir(cmp(b, c), dir);
//     int ac = _apply_dir(cmp(a, c), dir);
//
//     /* Return the median of the three */
//     if (ab <= 0) {
//         /* a <= b */
//         if (bc <= 0) return b;   /* a <= b <= c -> b is median */
//         if (ac <= 0) return c;   /* a <= c < b  -> c is median */
//         return a;                /* c < a <= b  -> a is median */
//     }
//     /* a > b */
//     if (ab >= 0 && ac <= 0) return a; /* b < a <= c -> a is median */
//     if (bc <= 0) return c;            /* b <= c < a -> c is median */
//     return b;                         /* c < b < a  -> b is median */
// }
//
// // --------------------------------------------------------------------------------
//
// /* Insertion sort for small partitions. Operates on element indices [lo, hi]. */
// static void _insertion_sort(uint8_t* data,
//                              size_t   lo,
//                              size_t   hi,
//                              size_t   data_size,
//                              int    (*cmp)(const void*, const void*),
//                              direction_t dir) {
//     uint8_t tmp[256];
//
//     for (size_t i = lo + 1u; i <= hi; i++) {
//         uint8_t* key_ptr = data + i * data_size;
//
//         /* Copy key element into tmp buffer */
//         if (data_size <= sizeof(tmp)) {
//             memcpy(tmp, key_ptr, data_size);
//         } else {
//             /* For oversized elements fall back to swap-based insertion */
//             size_t j = i;
//             while (j > lo &&
//                    _apply_dir(cmp(data + (j - 1u) * data_size,
//                                   data + j * data_size), dir) > 0) {
//                 _swap_elements(data + (j - 1u) * data_size,
//                                data + j * data_size,
//                                data_size);
//                 j--;
//             }
//             continue;
//         }
//
//         size_t j = i;
//         while (j > lo &&
//                _apply_dir(cmp(data + (j - 1u) * data_size, tmp), dir) > 0) {
//             /* Shift element at j-1 forward to position j */
//             memcpy(data + j * data_size,
//                    data + (j - 1u) * data_size,
//                    data_size);
//             j--;
//         }
//         memcpy(data + j * data_size, tmp, data_size);
//     }
// }
//
// // --------------------------------------------------------------------------------
//
// /*
//  * Partition the sub-array data[lo..hi] (inclusive indices) around a pivot
//  * chosen by median-of-three. Pivot is moved to data[hi] before partitioning.
//  * Returns the final index of the pivot.
//  */
// static size_t _partition(uint8_t* data,
//                           size_t   lo,
//                           size_t   hi,
//                           size_t   data_size,
//                           int    (*cmp)(const void*, const void*),
//                           direction_t dir) {
//     size_t   mid      = lo + (hi - lo) / 2u;
//     uint8_t* pivot_ptr = _median_of_three(data + lo  * data_size,
//                                            data + mid * data_size,
//                                            data + hi  * data_size,
//                                            cmp, dir);
//
//     /* Move pivot to end so it is out of the way during partitioning */
//     if (pivot_ptr != data + hi * data_size)
//         _swap_elements(pivot_ptr, data + hi * data_size, data_size);
//
//     uint8_t* pivot = data + hi * data_size;
//     size_t   i     = lo;   /* index of last element known to be < pivot */
//
//     for (size_t j = lo; j < hi; j++) {
//         if (_apply_dir(cmp(data + j * data_size, pivot), dir) < 0) {
//             _swap_elements(data + i * data_size,
//                            data + j * data_size,
//                            data_size);
//             i++;
//         }
//     }
//
//     /* Place pivot in its final position */
//     _swap_elements(data + i * data_size, data + hi * data_size, data_size);
//     return i;
// }
//
// // --------------------------------------------------------------------------------
//
// /*
//  * Iterative quicksort with median-of-three pivot, insertion sort fallback
//  * for small partitions, and tail-call optimisation to keep stack depth O(log n).
//  */
// static void _quicksort(uint8_t* data,
//                        size_t   lo,
//                        size_t   hi,
//                        size_t   data_size,
//                        int    (*cmp)(const void*, const void*),
//                        direction_t dir) {
// #define INSERTION_THRESHOLD 10u
//
//     while (lo < hi) {
//         if (hi - lo < INSERTION_THRESHOLD) {
//             _insertion_sort(data, lo, hi, data_size, cmp, dir);
//             break;
//         }
//
//         size_t pi = _partition(data, lo, hi, data_size, cmp, dir);
//
//         /*
//          * Recurse into the smaller partition, iterate into the larger.
//          * This keeps worst-case stack depth at O(log n).
//          */
//         if (pi > lo && pi - lo <= hi - pi) {
//             _quicksort(data, lo, pi - 1u, data_size, cmp, dir);
//             lo = pi + 1u;
//         } else {
//             if (pi + 1u < hi)
//                 _quicksort(data, pi + 1u, hi, data_size, cmp, dir);
//             if (pi == 0u) break;
//             hi = pi - 1u;
//         }
//     }
//
// #undef INSERTION_THRESHOLD
// }
//
// // --------------------------------------------------------------------------------
//
// error_code_t _sort_array(array_t* array,
//                          int    (*cmp)(const void*, const void*),
//                          direction_t dir) {
//     if (array == NULL || cmp == NULL) return NULL_POINTER;
//     if (array->len < 2u)              return EMPTY;
//
//     _quicksort(array->data, 0u, array->len - 1u, array->data_size, cmp, dir);
//     return NO_ERROR;
// }
//
// error_code_t sort_str_array(str_array_t* array, direction_t dir) {
//     if (array == NULL)          return NULL_POINTER;
//     if (array->base.len == 0u)  return EMPTY;
//     return _sort_array(&array->base, _cmp_slot, dir);
// }
//  
// // ================================================================================
// // Search — shared binary-search core
// // ================================================================================
//  
// static size_expect_t _bsearch(str_array_t* array,
//                                const char*  needle,
//                                bool         do_sort) {
//     if (array == NULL || needle == NULL)
//         return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
//     if (array->base.len == 0u)
//         return (size_expect_t){ .has_value = false, .u.error = EMPTY };
//  
//     if (do_sort)
//         _sort_array(&array->base, _cmp_slot, FORWARD);
//  
//     size_t lo = 0u;
//     size_t hi = array->base.len - 1u;
//  
//     while (lo < hi) {
//         size_t mid = lo + (hi - lo) / 2u;
//         string_t* s = _read_slot(array, mid);
//         int c = (s == NULL) ? -1 : (int)str_compare(s, needle);
//         if      (c < 0) { lo = mid + 1u; }
//         else if (c > 0) { if (mid == 0u) { lo = hi + 1u; break; } hi = mid; }
//         else            { lo = hi = mid; break; }
//     }
//  
//     if (lo > hi)
//         return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
//  
//     /* Confirm the landing position is actually a match. */
//     string_t* landed = _read_slot(array, lo);
//     if (landed == NULL || str_compare(landed, needle) != 0)
//         return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
//  
//     /* Fan left to find the lowest-index match. */
//     size_t best = lo;
//     while (best > 0u) {
//         string_t* prev = _read_slot(array, best - 1u);
//         if (prev != NULL && str_compare(prev, needle) == 0) {
//             best--;
//         } else {
//             break;
//         }
//     }
//     return (size_expect_t){ .has_value = true, .u.value = best };
// }
//  
// // ================================================================================
// // Search — contains
// // ================================================================================
//  
// size_expect_t str_array_contains_str(const str_array_t* array,
//                                      const string_t*    value,
//                                      size_t             start,
//                                      size_t             end) {
//     if (array == NULL || value == NULL)
//         return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
//     if (start >= end)
//         return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
//     if (end > array->base.len)
//         return (size_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };
//  
//     const char* needle = const_string(value);
//     if (needle == NULL)
//         return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
//  
//     for (size_t i = start; i < end; i++) {
//         string_t* s = _read_slot(array, i);
//         if (s != NULL && str_compare(s, needle) == 0)
//             return (size_expect_t){ .has_value = true, .u.value = i };
//     }
//     return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
// }
//  
// // --------------------------------------------------------------------------------
//  
// size_expect_t str_array_contains_lit(const str_array_t* array,
//                                      const char*        lit,
//                                      size_t             start,
//                                      size_t             end) {
//     if (array == NULL || lit == NULL)
//         return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
//     if (start >= end)
//         return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
//     if (end > array->base.len)
//         return (size_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };
//  
//     for (size_t i = start; i < end; i++) {
//         string_t* s = _read_slot(array, i);
//         if (s != NULL && str_compare(s, lit) == 0)
//             return (size_expect_t){ .has_value = true, .u.value = i };
//     }
//     return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
// }
//  
// // ================================================================================
// // Search — binary search
// // ================================================================================
//  
// size_expect_t str_array_binary_search_str(str_array_t*    array,
//                                           const string_t* value,
//                                           bool            sort) {
//     if (value == NULL)
//         return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
//     return _bsearch(array, const_string(value), sort);
// }
//  
// // --------------------------------------------------------------------------------
//  
// size_expect_t str_array_binary_search_lit(str_array_t* array,
//                                           const char*  lit,
//                                           bool         sort) {
//     return _bsearch(array, lit, sort);
// }
//  
// // ================================================================================
// // Introspection
// // ================================================================================
//  
// size_t str_array_size(const str_array_t* array) {
//     return (array == NULL) ? 0u : array->base.len;
// }
//  
// // --------------------------------------------------------------------------------
//  
// size_t str_array_alloc(const str_array_t* array) {
//     return (array == NULL) ? 0u : array->base.alloc;
// }
//  
// // --------------------------------------------------------------------------------
//  
// size_t str_array_data_size(const str_array_t* array) {
//     return (array == NULL) ? 0u : array->base.data_size;
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool is_str_array_empty(const str_array_t* array) {
//     return (array == NULL) || (array->base.len == 0u);
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool is_str_array_full(const str_array_t* array) {
//     return (array == NULL) || (array->base.len >= array->base.alloc);
// }
// // ================================================================================ 
// // ================================================================================ 
//
// // ================================================================================
// // Tokenization — helpers
// // ================================================================================
//  
// /*
//  * Resolve and validate a [begin, end) window against a string_t's buffer.
//  * - NULL begin  -> s->str
//  * - NULL end    -> s->str + s->len
//  * Both pointers must lie within [s->str, s->str + s->len].
//  * Returns true on success and writes the resolved pointers to out_begin, out_end.
//  */
// static bool _resolve_window(const string_t*   s,
//                              const uint8_t*    begin,
//                              const uint8_t*    end,
//                              const uint8_t**   out_begin,
//                              const uint8_t**   out_end) {
//     const uint8_t* base  = (const uint8_t*)s->str;
//     const uint8_t* limit = base + s->len;
//  
//     const uint8_t* b = (begin != NULL) ? begin : base;
//     const uint8_t* e = (end   != NULL) ? end   : limit;
//  
//     if (b < base || b > limit) return false;
//     if (e < base || e > limit) return false;
//     if (b > e)                 return false;
//  
//     *out_begin = b;
//     *out_end   = e;
//     return true;
// }
//  
// // ================================================================================
// // ================================================================================
// // Tokenization — character-set delimiter
// // ================================================================================
//  
// /*
//  * Returns true if byte `c` is present in the null-terminated set `delim_set`.
//  */
// static bool _in_delim_set(uint8_t c, const char* delim_set) {
//     for (const char* p = delim_set; *p != '\0'; ++p) {
//         if ((uint8_t)*p == c) return true;
//     }
//     return false;
// }
//  
// /*
//  * Core character-set tokenizer.
//  *
//  * Walks [win_begin, win_end) byte by byte.  Any byte present in delim_set
//  * acts as a single-character separator.  Every inter-separator segment
//  * (including empty ones produced by consecutive separators) is pushed into
//  * the output array as an owned deep copy.
//  *
//  * Pre-sizing: token_count_lit counts how many bytes in the window belong to
//  * the delimiter set, giving us (count + 1) segments.
//  */
// static str_array_expect_t _delim_tokenize(const char*        delim_set,
//                                           const uint8_t*     win_begin,
//                                           const uint8_t*     win_end,
//                                           allocator_vtable_t alloc_v) {
//     /* Use a modest initial capacity and let growth=true handle resizing.
//      * Avoids depending on token_count_lit's counting semantics matching
//      * _in_delim_set's character-set semantics exactly. */
//     str_array_expect_t ar = init_str_array(8u, true, alloc_v);
//     if (!ar.has_value) return ar;
//     str_array_t* dst = ar.u.value;
//  
//     const uint8_t* seg_start = win_begin;
//  
//     for (const uint8_t* p = win_begin; p <= win_end; ++p) {
//         bool is_delim = (p < win_end) && _in_delim_set(*p, delim_set);
//         bool is_end   = (p == win_end);
//  
//         if (is_delim || is_end) {
//             size_t seg_len = (size_t)(p - seg_start);
//  
//             /* init_string may use strlen on the source pointer, so we must
//              * provide a properly null-terminated copy of the segment rather
//              * than pointing into the middle of the original buffer. */
//             void_ptr_expect_t tmp_r = alloc_v.allocate(alloc_v.ctx,
//                                                         seg_len + 1u, false);
//             if (!tmp_r.has_value) {
//                 return_str_array(dst);
//                 return (str_array_expect_t){ .has_value = false,
//                                              .u.error   = OUT_OF_MEMORY };
//             }
//             char* tmp = (char*)tmp_r.u.value;
//             memcpy(tmp, seg_start, seg_len);
//             tmp[seg_len] = '\0';
//  
//             string_expect_t se = init_string(tmp, seg_len, alloc_v);
//             alloc_v.return_element(alloc_v.ctx, tmp);
//             if (!se.has_value) {
//                 return_str_array(dst);
//                 return (str_array_expect_t){ .has_value = false,
//                                              .u.error   = OUT_OF_MEMORY };
//             }
//  
//             error_code_t err = _do_push(dst, se, _POS_BACK, 0u);
//             if (err != NO_ERROR) {
//                 return_str_array(dst);
//                 return (str_array_expect_t){ .has_value = false,
//                                              .u.error   = err };
//             }
//  
//             seg_start = p + 1u;
//         }
//     }
//  
//     return (str_array_expect_t){ .has_value = true, .u.value = dst };
// }
//  
// // --------------------------------------------------------------------------------
//  
// str_array_expect_t string_delim_array_lit(const string_t*    s,
//                                           const char*        delim_set,
//                                           const uint8_t*     begin,
//                                           const uint8_t*     end,
//                                           allocator_vtable_t alloc_v) {
//     if (s == NULL || s->str == NULL || delim_set == NULL)
//         return (str_array_expect_t){ .has_value = false,
//                                      .u.error   = NULL_POINTER };
//     if (delim_set[0] == '\0')
//         return (str_array_expect_t){ .has_value = false,
//                                      .u.error   = INVALID_ARG };
//  
//     const uint8_t* wb;
//     const uint8_t* we;
//     if (!_resolve_window(s, begin, end, &wb, &we))
//         return (str_array_expect_t){ .has_value = false,
//                                      .u.error   = INVALID_ARG };
//  
//     return _delim_tokenize(delim_set, wb, we, alloc_v);
// }
//  
// // --------------------------------------------------------------------------------
//  
// str_array_expect_t string_delim_array_str(const string_t*    s,
//                                           const string_t*    delim_set,
//                                           const uint8_t*     begin,
//                                           const uint8_t*     end,
//                                           allocator_vtable_t alloc_v) {
//     if (s == NULL || s->str == NULL || delim_set == NULL || delim_set->str == NULL)
//         return (str_array_expect_t){ .has_value = false,
//                                      .u.error   = NULL_POINTER };
//     if (delim_set->len == 0u)
//         return (str_array_expect_t){ .has_value = false,
//                                      .u.error   = INVALID_ARG };
//  
//     const uint8_t* wb;
//     const uint8_t* we;
//     if (!_resolve_window(s, begin, end, &wb, &we))
//         return (str_array_expect_t){ .has_value = false,
//                                      .u.error   = INVALID_ARG };
//  
//     return _delim_tokenize(delim_set->str, wb, we, alloc_v);
// }
// // -------------------------------------------------------------------------------- 
//
// error_code_t print_string_array(const str_array_t* array, FILE* stream) {
//     if (array == NULL || stream == NULL) {
//         return NULL_POINTER;
//     }
//
//     size_t col = 0u;
//     size_t len = str_array_size(array);
//
//     /* Opening bracket */
//     if (fputs("[ ", stream) == EOF) {
//         return ILLEGAL_STATE;
//     }
//     col = 2u;
//
//     if (len == 0u) {
//         if (fputs("]", stream) == EOF) {
//             return ILLEGAL_STATE;
//         }
//         return NO_ERROR;
//     }
//
//     for (size_t i = 0u; i < len; ++i) {
//         string_expect_t r = get_str_array_ptr(array, i);
//         const string_t* s = NULL;
//         const char* text = "";
//         size_t text_len = 0u;
//
//         if (!r.has_value) {
//             return r.u.error;
//         }
//
//         s = r.u.value;
//         if (s != NULL && s->str != NULL) {
//             text = s->str;
//             text_len = s->len;
//         }
//
//         /* Separator before every element except the first */
//         if (i > 0u) {
//             if ((col + 2u) > 70u) {
//                 if (fputc('\n', stream) == EOF) {
//                     return ILLEGAL_STATE;
//                 }
//                 col = 0u;
//             } else {
//                 if (fputs(", ", stream) == EOF) {
//                     return ILLEGAL_STATE;
//                 }
//                 col += 2u;
//             }
//         }
//
//         /* Opening quote for this element */
//         if ((col + 1u) > 70u) {
//             if (fputc('\n', stream) == EOF) {
//                 return ILLEGAL_STATE;
//             }
//             col = 0u;
//         }
//         if (fputc('"', stream) == EOF) {
//             return ILLEGAL_STATE;
//         }
//         col += 1u;
//
//         /* Element contents, wrapping mid-string if needed */
//         size_t j = 0u;
//         while (j < text_len) {
//             size_t remaining = text_len - j;
//             size_t chunk = remaining;
//
//             if (col >= 70u) {
//                 if (fputc('\n', stream) == EOF) {
//                     return ILLEGAL_STATE;
//                 }
//                 col = 0u;
//             }
//
//             if ((col + chunk) > 70u) {
//                 chunk = 70u - col;
//             }
//
//             if (chunk == 0u) {
//                 if (fputc('\n', stream) == EOF) {
//                     return ILLEGAL_STATE;
//                 }
//                 col = 0u;
//                 continue;
//             }
//
//             if (fwrite(text + j, 1u, chunk, stream) != chunk) {
//                 return ILLEGAL_STATE;
//             }
//
//             j += chunk;
//             col += chunk;
//
//             if ((col == 70u) && (j < text_len)) {
//                 if (fputc('\n', stream) == EOF) {
//                     return ILLEGAL_STATE;
//                 }
//                 col = 0u;
//             }
//         }
//
//         /* Closing quote for this element */
//         if ((col + 1u) > 70u) {
//             if (fputc('\n', stream) == EOF) {
//                 return ILLEGAL_STATE;
//             }
//             col = 0u;
//         }
//         if (fputc('"', stream) == EOF) {
//             return ILLEGAL_STATE;
//         }
//         col += 1u;
//     }
//
//     /* Closing bracket */
//     if ((col + 2u) > 70u) {
//         if (fputc('\n', stream) == EOF) {
//             return ILLEGAL_STATE;
//         }
//         col = 0u;
//     }
//
//     if (fputs(" ]", stream) == EOF) {
//         return ILLEGAL_STATE;
//     }
//
//     return NO_ERROR;
// }
// ================================================================================
// ================================================================================
// eof
