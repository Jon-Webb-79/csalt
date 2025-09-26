/* simd_avx512_char.inl
   AVX-512BW helpers for last-occurrence search in unsigned bytes.
   Requires: <immintrin.h>, compiled with -mavx512bw
*/
#ifndef CSALT_SIMD_AVX512_CHAR_INL
#define CSALT_SIMD_AVX512_CHAR_INL

#include <stddef.h>
#include <stdint.h>

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

/* Convenience macro so callers can use one name across ISAs */
#define CSALT_LAST_U8_INDEX(s, n, c) simd_last_index_u8_avx512bw((s), (n), (c))

#endif /* CSALT_SIMD_AVX512_CHAR_INL */

