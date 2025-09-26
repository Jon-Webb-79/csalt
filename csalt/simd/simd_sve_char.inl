/* simd_sve_char.inl
   SVE helpers for last-occurrence search in bytes (VL-agnostic).
   Requires: <arm_sve.h>
*/
#ifndef CSALT_SIMD_SVE_CHAR_INL
#define CSALT_SIMD_SVE_CHAR_INL

#include <stddef.h>
#include <stdint.h>
#include <arm_sve.h>

static inline size_t simd_last_index_u8_sve(const unsigned char* s, size_t n, unsigned char c) {
    size_t i = 0, last = SIZE_MAX;

    while (i < n) {
        svbool_t pg = svwhilelt_b8((uint64_t)i, (uint64_t)n);
        svuint8_t v = svld1_u8(pg, (const uint8_t*)(s + i));
        svbool_t  m = svcmpeq_n_u8(pg, v, c);

        if (svptest_any(svptrue_b8(), m)) {
            /* Build lane indices [0..VL-1] then compact under match predicate. */
            svuint8_t idx0 = svindex_u8(0, 1);
            svuint8_t idxc = svcompact_u8(m, idx0);
            /* Number of matches in this vector */
            uint64_t count = svcntp_b8(svptrue_b8(), m);
            /* Last packed index (relative to this chunk) */
            uint8_t rel = svlasta_u8(svptrue_b8(), idxc);
            /* If multiple matches exist, rel will hold the highest matched lane index */
            (void)count; /* rel already represents last match after compact */
            last = i + (size_t)rel;
        }

        i += svcntb();
    }
    return last;
}

#define CSALT_LAST_U8_INDEX(s, n, c) simd_last_index_u8_sve((s), (n), (c))

#endif /* CSALT_SIMD_SVE_CHAR_INL */

