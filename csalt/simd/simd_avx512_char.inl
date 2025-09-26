/* simd_avx512_char.inl
   AVX-512BW helpers for last-occurrence search in unsigned bytes.
   Requires: <immintrin.h>, compiled with -mavx512bw
*/
#ifndef CSALT_SIMD_AVX512_CHAR_INL
#define CSALT_SIMD_AVX512_CHAR_INL

#include <stddef.h>
#include <stdint.h>
#include <immintrin.h>
#include <string.h>

static inline int csalt_highbit_u64(uint64_t m) { return 63 - __builtin_clzll(m); }

/* Returns index of last occurrence of c in s[0..n), or SIZE_MAX if not found. */
static inline size_t simd_last_index_u8_avx512bw(const unsigned char* s, size_t n, unsigned char c) {
    size_t i = 0, last = SIZE_MAX;
    const __m512i needle = _mm512_set1_epi8((char)c);

    while (i + 64 <= n) {
        __m512i v = _mm512_loadu_si512((const void*)(s + i));
        __mmask64 k = _mm512_cmpeq_epi8_mask(v, needle);
        if (k) last = i + (size_t)csalt_highbit_u64((uint64_t)k);
        i += 64;
    }
    /* tail */
    for (size_t j = n; j-- > i; ) {
        if (s[j] == c) return j;
    }
    return last;
}

static inline size_t simd_first_substr_index_avx512bw(const unsigned char* s, size_t n,
                                                      const unsigned char* pat, size_t m) {
    if (m == 0) return 0;
    if (m == 1) {
        const void* p = memchr(s, pat[0], n);
        return p ? (size_t)((const unsigned char*)p - s) : SIZE_MAX;
    }
    size_t i = 0;
    const __m512i needle0 = _mm512_set1_epi8((char)pat[0]);

    while (i + 64 <= n) {
        __m512i v = _mm512_loadu_si512((const void*)(s + i));
        __mmask64 k = _mm512_cmpeq_epi8_mask(v, needle0);
        while (k) {
            unsigned long pos = __builtin_ctzll((uint64_t)k);
            size_t cand = i + (size_t)pos;
            if (cand + m <= n && memcmp(s + cand, pat, m) == 0) return cand;
            k &= (k - 1);  /* clear lowest set bit */
        }
        i += 64;
    }
    for (; i + m <= n; ++i)
        if (s[i] == pat[0] && memcmp(s + i, pat, m) == 0) return i;
    return SIZE_MAX;
}
#endif /* CSALT_SIMD_AVX512_CHAR_INL */

