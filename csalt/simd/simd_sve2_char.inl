/* simd_sve2_char.inl
   SVE2 helpers for last-occurrence search in bytes (VL-agnostic).
   Requires: <arm_sve.h>
*/
#ifndef CSALT_SIMD_SVE2_CHAR_INL
#define CSALT_SIMD_SVE2_CHAR_INL

#include <stddef.h>
#include <stdint.h>
#include <arm_sve.h>
#include <string.h>

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

static inline size_t simd_first_substr_index_sve2(const unsigned char* s, size_t n,
                                                  const unsigned char* pat, size_t m) {
    if (m == 0) return 0;
    if (m == 1) {
        const void* p = memchr(s, pat[0], n);
        return p ? (size_t)((const unsigned char*)p - s) : SIZE_MAX;
    }

    size_t i = 0;
    while (i < n) {
        svbool_t pg = svwhilelt_b8((uint64_t)i, (uint64_t)n);
        svuint8_t v = svld1_u8(pg, (const uint8_t*)(s + i));
        svbool_t  msk = svcmpeq_n_u8(pg, v, pat[0]);

        if (svptest_any(svptrue_b8(), msk)) {
            uint64_t vl = svcntb();
            uint64_t end = i + vl;
            if (end > n) end = n;
            for (size_t j = i; j < end; ++j) {
                if (s[j] == pat[0]) {
                    if (j + m <= n && memcmp(s + j, pat, m) == 0) return j;
                }
            }
        }
        i += svcntb();
    }
    return SIZE_MAX;
}
static inline size_t simd_last_substr_index_sve2(const unsigned char* s, size_t n,
                                                 const unsigned char* pat, size_t m) {
    if (m == 0) return n;
    if (m == 1) { for (size_t i = n; i-- > 0; ) if (s[i] == pat[0]) return i; return SIZE_MAX; }

    size_t i = 0, last = SIZE_MAX;
    while (i < n) {
        svbool_t pg = svwhilelt_b8((uint64_t)i, (uint64_t)n);
        svuint8_t v = svld1_u8(pg, (const uint8_t*)(s + i));
        svbool_t  msk = svcmpeq_n_u8(pg, v, pat[0]);

        if (svptest_any(svptrue_b8(), msk)) {
            uint64_t vl = svcntb();
            uint64_t end = i + vl; if (end > n) end = n;
            for (size_t j = end; j-- > i; ) {
                if (s[j] == pat[0]) {
                    if (j + m <= n && memcmp(s + j, pat, m) == 0) { last = j; break; }
                }
            }
        }
        i += svcntb();
    }
    return last;
}

static inline size_t simd_last_substr_index_sve2(const unsigned char* s, size_t n,
                                                 const unsigned char* pat, size_t m) {
    if (m == 0) return n;
    if (m == 1) { for (size_t i=n; i-- > 0; ) if (s[i]==pat[0]) return i; return SIZE_MAX; }
    if (n < m) return SIZE_MAX;

    size_t i = 0, last = SIZE_MAX;
    while (i < n) {
        svbool_t pg = svwhilelt_b8((uint64_t)i, (uint64_t)n);
        svuint8_t v = svld1_u8(pg, (const uint8_t*)(s + i));
        svbool_t  msk = svcmpeq_n_u8(pg, v, pat[0]);

        if (svptest_any(svptrue_b8(), msk)) {
            uint64_t vl = svcntb();
            uint64_t end = i + vl; if (end > n) end = n;
            for (size_t j = end; j-- > i; ) {
                if (s[j] == pat[0]) {
                    if (j + m <= n && memcmp(s + j, pat, m) == 0) { last = j; break; }
                }
            }
        }
        i += svcntb();
    }
    return last;
}
static inline size_t simd_token_count_sve2(const char* s, size_t n,
                                          const char* delim, size_t dlen)
{
    size_t i = 0, count = 0;
    /* byte mask 0xFF (delim) / 0x00 (non) for "previous" before start */
    uint8_t prev_last = 0xFF;

    while (i < n) {
        svbool_t pg = svwhilelt_b8(i, n);
        svuint8_t v = svld1_u8(pg, (const uint8_t*)(s + i));

        /* build byte mask 0xFF/0x00 for delimiters */
        svuint8_t dm = svdup_u8(0);
        for (size_t j = 0; j < dlen; ++j) {
            svbool_t eq = svcmpeq_u8(pg, v, svdup_u8((uint8_t)delim[j]));
            dm = svorr_u8_x(pg, dm, svdup_u8_z(eq, 0xFF));
        }
        svuint8_t non = svnot_u8_z(pg, dm);

        /* prev per position: [prev_last, dm[0], dm[1], ...] */
        svuint8_t prev_fill = svdup_u8(prev_last);
        svuint8_t prev = svext_u8(prev_fill, dm, 1);

        svuint8_t starts = svand_u8_x(pg, non, prev);

        /* Count bytes != 0 via population count of 1-bit per byte.
           Shift 0xFF->1, 0x00->0, then sum lanes. */
        svuint8_t ones = svlsr_n_u8_x(pg, starts, 7);
        /* widen and accumulate */
        svuint16_t s16 = svuaddlb_u8(ones, svdup_u8(0));   /* pairwise add low */
        svuint16_t s16h= svuaddlt_u8(ones, svdup_u8(0));   /* pairwise add high */
        uint64_t block = (uint64_t)svaddv_u16(pg, s16) + (uint64_t)svaddv_u16(pg, s16h);
        count += (size_t)block;

        /* carry last-lane flag: extract the last active lane byte of dm */
        uint64_t active = (uint64_t)svcntp_b8(svptrue_b8(), pg); /* active lanes */
        uint8_t last = svlastb_u8(pg, dm); /* last active element */
        prev_last = (last != 0) ? 0xFF : 0x00;

        i += svcntb();
    }
    return count;
}


#endif /* CSALT_SIMD_SVE2_CHAR_INL */

