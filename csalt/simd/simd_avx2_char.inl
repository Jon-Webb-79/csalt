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

static inline size_t simd_first_substr_index_avx2(const unsigned char* s, size_t n,
                                                  const unsigned char* pat, size_t m) {
    if (m == 0) return 0;
    if (m == 1) {
        const void* p = memchr(s, pat[0], n);
        return p ? (size_t)((const unsigned char*)p - s) : SIZE_MAX;
    }

    size_t i = 0;
    const __m256i needle0 = _mm256_set1_epi8((char)pat[0]);

    while (i + 32 <= n) {
        __m256i v  = _mm256_loadu_si256((const __m256i*)(s + i));
        __m256i eq = _mm256_cmpeq_epi8(v, needle0);
        uint32_t mask = (uint32_t)_mm256_movemask_epi8(eq);

        while (mask) {
            int pos = __builtin_ctz(mask);         // lowest set bit
            size_t cand = i + (size_t)pos;
            if (cand + m <= n && memcmp(s + cand, pat, m) == 0) return cand;
            mask &= mask - 1;                      // clear lowest set bit
        }
        i += 32;
    }

    /* tail (covers [i, n-m]) */
    for (; i + m <= n; ++i) {
        if (s[i] == pat[0] && memcmp(s + i, pat, m) == 0) return i;
    }
    return SIZE_MAX;
}
static inline size_t simd_last_substr_index_avx2(const unsigned char* s, size_t n,
                                                   const unsigned char* pat, size_t m) {
      if (m == 0) return n;
      if (m == 1) {
          for (size_t i = n; i-- > 0; ) if (s[i] == pat[0]) return i;
          return SIZE_MAX;
      }
      size_t i = 0, last = SIZE_MAX;
      const __m256i needle0 = _mm256_set1_epi8((char)pat[0]);

      while (i + 32 <= n) {
          __m256i v  = _mm256_loadu_si256((const __m256i*)(s + i));
          __m256i eq = _mm256_cmpeq_epi8(v, needle0);
          uint32_t mask = (uint32_t)_mm256_movemask_epi8(eq);
          while (mask) {
              int pos = hi_bit32(mask);                         /* highest set bit */
              size_t cand = i + (size_t)pos;
              if (cand + m <= n && memcmp(s + cand, pat, m) == 0) { last = cand; break; }
              mask &= (1u << pos) - 1;                          /* keep lower bits only */
          }
          i += 32;
      }
      for (size_t j = (n >= m ? n - m : 0); j + 1 > i; --j)
          if (s[j] == pat[0] && memcmp(s + j, pat, m) == 0) return j;
      return last;
}

static inline int csalt_hi_bit32_avx2(uint32_t m) { return 31 - __builtin_clz(m); }

static inline size_t simd_last_substr_index_avx2(const unsigned char* s, size_t n,
                                                 const unsigned char* pat, size_t m) {
    if (m == 0) return n;
    if (m == 1) { for (size_t i=n; i-- > 0; ) if (s[i]==pat[0]) return i; return SIZE_MAX; }
    if (n < m) return SIZE_MAX;

    size_t i = 0, last = SIZE_MAX;
    const __m256i b0 = _mm256_set1_epi8((char)pat[0]);

    while (i + 32 <= n) {
        __m256i v  = _mm256_loadu_si256((const __m256i*)(s + i));
        __m256i eq = _mm256_cmpeq_epi8(v, b0);
        uint32_t mask = (uint32_t)_mm256_movemask_epi8(eq);
        while (mask) {
            int pos = csalt_hi_bit32_avx2(mask);
            size_t cand = i + (size_t)pos;
            if (cand + m <= n && memcmp(s + cand, pat, m) == 0) { last = cand; break; }
            mask &= (1u << pos) - 1u;
        }
        i += 32;
    }

    for (size_t j = (n - m); j + 1 > i; --j)
        if (s[j] == pat[0] && memcmp(s + j, pat, m) == 0) return j;

    return last;
}

#endif /* CSALT_SIMD_AVX2_CHAR_INL */

