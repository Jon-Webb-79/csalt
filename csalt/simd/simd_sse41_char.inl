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

#define CSALT_LAST_U8_INDEX(s, n, c) simd_last_index_u8_sse41((s), (n), (c))

#endif /* CSALT_SIMD_SSE41_CHAR_INL */

