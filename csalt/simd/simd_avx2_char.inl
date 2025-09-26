/* simd_avx2_char.inl
   AVX2 helpers for last-occurrence search in unsigned bytes.
   Requires: <immintrin.h>, compiled with -mavx2
*/
#ifndef CSALT_SIMD_AVX2_CHAR_INL
#define CSALT_SIMD_AVX2_CHAR_INL

#include <stddef.h>
#include <stdint.h>
#include <immintrin.h>

static inline int csalt_highbit_u32(uint32_t m) { return 31 - __builtin_clz(m); }

/* Returns index of last occurrence of c in s[0..n), or SIZE_MAX if not found. */
static inline size_t simd_last_index_u8_avx2(const unsigned char* s, size_t n, unsigned char c) {
    size_t i = 0, last = SIZE_MAX;
    const __m256i needle = _mm256_set1_epi8((char)c);

    while (i + 32 <= n) {
        __m256i v  = _mm256_loadu_si256((const __m256i*)(s + i));
        __m256i eq = _mm256_cmpeq_epi8(v, needle);
        uint32_t mask = (uint32_t)_mm256_movemask_epi8(eq);
        if (mask) last = i + (size_t)csalt_highbit_u32(mask);
        i += 32;
    }
    /* tail */
    for (size_t j = n; j-- > i; ) {
        if (s[j] == c) return j;
    }
    return last;
}

/* Convenience macro so callers can use one name across ISAs */
#define CSALT_LAST_U8_INDEX(s, n, c) simd_last_index_u8_avx2((s), (n), (c))

#endif /* CSALT_SIMD_AVX2_CHAR_INL */

