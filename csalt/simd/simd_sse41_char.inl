/* simd_sse41_char.inl
   SSE4.1 path uses SSE2 integer ops for this routine.
   Requires: <smmintrin.h> and <emmintrin.h>
*/
#ifndef CSALT_SIMD_SSE41_CHAR_INL
#define CSALT_SIMD_SSE41_CHAR_INL

#include <stddef.h>
#include <stdint.h>
#include <smmintrin.h>
#include <emmintrin.h>

static inline int csalt_highbit_u32(uint32_t m) { return 31 - __builtin_clz(m); }

static inline size_t simd_last_index_u8_sse41(const unsigned char* s, size_t n, unsigned char c) {
    size_t i = 0, last = SIZE_MAX;
    const __m128i needle = _mm_set1_epi8((char)c);

    while (i + 16 <= n) {
        __m128i v  = _mm_loadu_si128((const __m128i*)(s + i));
        __m128i eq = _mm_cmpeq_epi8(v, needle);
        uint32_t mask = (uint32_t)_mm_movemask_epi8(eq);
        if (mask) last = i + (size_t)csalt_highbit_u32(mask);
        i += 16;
    }
    for (size_t j = n; j-- > i; ) if (s[j] == c) return j;
    return last;
}

static inline size_t simd_first_substr_index_sse41(const unsigned char* s, size_t n,
                                                   const unsigned char* pat, size_t m) {
    if (m == 0) return 0;
    if (m == 1) {
        const void* p = memchr(s, pat[0], n);
        return p ? (size_t)((const unsigned char*)p - s) : SIZE_MAX;
    }
    size_t i = 0;
    const __m128i needle0 = _mm_set1_epi8((char)pat[0]);

    while (i + 16 <= n) {
        __m128i v  = _mm_loadu_si128((const __m128i*)(s + i));
        __m128i eq = _mm_cmpeq_epi8(v, needle0);
        uint32_t mask = (uint32_t)_mm_movemask_epi8(eq);
        while (mask) {
            int pos = __builtin_ctz(mask);
            size_t cand = i + (size_t)pos;
            if (cand + m <= n && memcmp(s + cand, pat, m) == 0) return cand;
            mask &= (mask - 1);
        }
        i += 16;
    }
    for (; i + m <= n; ++i)
        if (s[i] == pat[0] && memcmp(s + i, pat, m) == 0) return i;
    return SIZE_MAX;
}
static inline int csalt_hi_bit32_sse41(uint32_t m) { return 31 - __builtin_clz(m); }

static inline size_t simd_last_substr_index_sse41(const unsigned char* s, size_t n,
                                                  const unsigned char* pat, size_t m) {
    if (m == 0) return n;
    if (m == 1) { for (size_t i = n; i-- > 0; ) if (s[i] == pat[0]) return i; return SIZE_MAX; }

    size_t i = 0, last = SIZE_MAX;
    const __m128i needle0 = _mm_set1_epi8((char)pat[0]);

    while (i + 16 <= n) {
        __m128i v  = _mm_loadu_si128((const __m128i*)(s + i));
        __m128i eq = _mm_cmpeq_epi8(v, needle0);
        uint32_t mask = (uint32_t)_mm_movemask_epi8(eq);

        while (mask) {
            int pos = csalt_hi_bit32_sse41(mask);
            size_t cand = i + (size_t)pos;
            if (cand + m <= n && memcmp(s + cand, pat, m) == 0) { last = cand; break; }
            mask &= (1u << pos) - 1;
        }
        i += 16;
    }
    for (size_t j = (n >= m ? n - m : 0); j + 1 > i; --j)
        if (s[j] == pat[0] && memcmp(s + j, pat, m) == 0) return j;

    return last;
}

//static inline int csalt_hi_bit32_sse41(uint32_t m) { return 31 - __builtin_clz(m); }

// static inline size_t simd_last_substr_index_sse41(const unsigned char* s, size_t n,
//                                                   const unsigned char* pat, size_t m) {
//     if (m == 0) return n;
//     if (m == 1) { for (size_t i=n; i-- > 0; ) if (s[i]==pat[0]) return i; return SIZE_MAX; }
//     if (n < m) return SIZE_MAX;
//
//     size_t i = 0, last = SIZE_MAX;
//     const __m128i b0 = _mm_set1_epi8((char)pat[0]);
//
//     while (i + 16 <= n) {
//         __m128i v  = _mm_loadu_si128((const __m128i*)(s + i));
//         __m128i eq = _mm_cmpeq_epi8(v, b0);
//         uint32_t mask = (uint32_t)_mm_movemask_epi8(eq);
//         while (mask) {
//             int pos = csalt_hi_bit32_sse41(mask);
//             size_t cand = i + (size_t)pos;
//             if (cand + m <= n && memcmp(s + cand, pat, m) == 0) { last = cand; break; }
//             mask &= (1u << pos) - 1u;
//         }
//         i += 16;
//     }
//
//     for (size_t j = (n - m); j + 1 > i; --j)
//         if (s[j] == pat[0] && memcmp(s + j, pat, m) == 0) return j;
//
//     return last;
// }

#endif /* CSALT_SIMD_SSE41_CHAR_INL */

