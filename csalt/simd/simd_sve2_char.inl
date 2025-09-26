/* simd_sve2_char.inl
   SVE2 helpers for last-occurrence search in bytes (VL-agnostic).
   Requires: <arm_sve.h>
*/
#ifndef CSALT_SIMD_SVE2_CHAR_INL
#define CSALT_SIMD_SVE2_CHAR_INL

#include <stddef.h>
#include <stdint.h>
#include <arm_sve.h>

static inline size_t simd_last_index_u8_sve2(const unsigned char* s, size_t n, unsigned char c) {
    size_t i = 0, last = SIZE_MAX;

    while (i < n) {
        svbool_t pg = svwhilelt_b8((uint64_t)i, (uint64_t)n);
        svuint8_t v = svld1_u8(pg, (const uint8_t*)(s + i));
        svbool_t  m = svcmpeq_n_u8(pg, v, c);

        if (svptest_any(svptrue_b8(), m)) {
            svuint8_t idx0 = svindex_u8(0, 1);
            svuint8_t idxc = svcompact_u8(m, idx0);
            uint8_t rel = svlasta_u8(svptrue_b8(), idxc);
            last = i + (size_t)rel;
        }

        i += svcntb();
    }
    return last;
}

#define CSALT_LAST_U8_INDEX(s, n, c) simd_last_index_u8_sve2((s), (n), (c))

#endif /* CSALT_SIMD_SVE2_CHAR_INL */

