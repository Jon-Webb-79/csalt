#ifndef SIMD_SVE2_DOUBLE_INL
#define SIMD_SVE2_DOUBLE_INL

/**
 * @file simd_sve2_double.inl
 * @brief ARM SVE2 implementation of tolerance-aware double search.
 *
 * The vector length is queried at runtime via svcntd() (number of 64-bit lanes).
 * SVE2 adds no instructions that improve on the SVE double path for single-needle
 * search. This file uses the identical SVE algorithm and exists as a separate
 * dispatch target for future SVE2-specific refinements.
 * Exact mode: svreinterpret_u64_f64 + svcmpeq_u64 — bitwise equality.
 * Tolerance mode: svabs_f64_z, svsub_f64_z, svmax_f64_z, svmul_f64_z, svcmple_f64.
 *
 * Requires: -march=armv9-a or -march=armv8-a+sve2
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <arm_sve.h>

static inline bool _biteq_double(double a, double b) {
    uint64_t ua, ub;
    memcpy(&ua, &a, sizeof ua);
    memcpy(&ub, &b, sizeof ub);
    return ua == ub;
}


static size_t simd_contains_double(const double* data,
                                    size_t        start,
                                    size_t        end,
                                    double        needle,
                                    double        abs_tol,
                                    double        rel_tol) {
    size_t vl = svcntd();
    size_t i  = start;

    if (abs_tol == 0.0 && rel_tol == 0.0) {
        union { double d; uint64_t u; } nu; nu.d = needle;
        svuint64_t vn = svdup_n_u64(nu.u);
        while (i + vl <= end) {
            svbool_t    pg    = svptrue_b64();
            svfloat64_t fchunk = svld1_f64(pg, data + i);
            svuint64_t  chunk  = svreinterpret_u64_f64(fchunk);
            svbool_t    cmp    = svcmpeq_u64(pg, chunk, vn);
            if (svptest_any(pg, cmp))
                return i + (size_t)svcntp_b64(pg, svbrkb_z(pg, cmp));
            i += vl;
        }
    } else {
        svfloat64_t vn          = svdup_n_f64(needle);
        svfloat64_t vabs_tol    = svdup_n_f64(abs_tol);
        svfloat64_t vrel_tol    = svdup_n_f64(rel_tol);
        svfloat64_t vabs_needle = svdup_n_f64(fabs(needle));
        while (i + vl <= end) {
            svbool_t    pg         = svptrue_b64();
            svfloat64_t chunk      = svld1_f64(pg, data + i);
            svfloat64_t diff       = svabs_f64_z(pg, svsub_f64_z(pg, chunk, vn));
            svfloat64_t abs_chunk  = svabs_f64_z(pg, chunk);
            svfloat64_t rel_base   = svmax_f64_z(pg, abs_chunk, vabs_needle);
            svfloat64_t rel_thresh = svmul_f64_z(pg, vrel_tol, rel_base);
            svfloat64_t threshold  = svmax_f64_z(pg, vabs_tol, rel_thresh);
            svbool_t    cmp        = svcmple_f64(pg, diff, threshold);
            if (svptest_any(pg, cmp))
                return i + (size_t)svcntp_b64(pg, svbrkb_z(pg, cmp));
            i += vl;
        }
    }

    /* Scalar remainder */
    if (abs_tol == 0.0 && rel_tol == 0.0) {
        union { double d; uint64_t u; } n; n.d = needle;
        for (; i < end; i++) {
            union { double d; uint64_t u; } v; v.d = data[i];
            if (v.u == n.u) return i;
        }
    } else {
        for (; i < end; i++) {
            double diff      = fabs(data[i] - needle);
            double threshold = fmax(abs_tol,
                                    rel_tol * fmax(fabs(data[i]), fabs(needle)));
            if (diff <= threshold) return i;
        }
    }
    return SIZE_MAX;
}

// ================================================================================
// simd_binary_search_double
// ================================================================================

static size_t simd_binary_search_double(const double* data,
                                         size_t        len,
                                         double        needle,
                                         double        abs_tol,
                                         double        rel_tol,
                                         int         (*cmp)(const void*,
                                                            const void*)) {
    if (len == 0u) return SIZE_MAX;

    /* ── Bisection (exact comparator, sort contract unchanged) ── */
    size_t lo = 0u, hi = len - 1u, mid = 0u;
    while (lo < hi) {
        mid = lo + (hi - lo) / 2u;
        int c = cmp(data + mid, &needle);
        if      (c < 0) lo = mid + 1u;
        else if (c > 0) { if (mid == 0u) { lo = hi + 1u; break; } hi = mid; }
        else            { lo = hi = mid; break; }
    }
    if (lo > hi) return SIZE_MAX;
    mid = lo;

    size_t vl        = svcntd();
    size_t best      = SIZE_MAX;
    double abs_needle = fabs(needle);
    svfloat64_t vn       = svdup_n_f64(needle);
    svfloat64_t vabs_tol = svdup_n_f64(abs_tol);
    svfloat64_t vrel_tol = svdup_n_f64(rel_tol);
    svfloat64_t vabs_ndl = svdup_n_f64(abs_needle);

    /* Exact-mode bitwise helper (valid for the whole function scope). */

    if (abs_tol == 0.0 && rel_tol == 0.0) {
        if (_biteq_double(data[mid], needle)) best = mid;
    } else {
        double diff = fabs(data[mid] - needle);
        double thr  = fmax(abs_tol, rel_tol * fmax(fabs(data[mid]), abs_needle));
        if (diff <= thr) best = mid;
    }

    size_t L = mid;
    while (L >= vl) {
        size_t      base  = L - vl;
        svbool_t    pg    = svptrue_b64();
        svfloat64_t chunk = svld1_f64(pg, data + base);
        svfloat64_t diff  = svabs_f64_z(pg, svsub_f64_z(pg, chunk, vn));
        svfloat64_t abs_c = svabs_f64_z(pg, chunk);
        svfloat64_t thr   = svmax_f64_z(pg, vabs_tol,
                                svmul_f64_z(pg, vrel_tol,
                                    svmax_f64_z(pg, abs_c, vabs_ndl)));
        svbool_t    match   = svcmple_f64(pg, diff, thr);
        svbool_t    nomatch = svnot_b_z(pg, match);
        if (!svptest_any(pg, nomatch)) {
            best = base; L = base; continue;
        }
        size_t n_match = (size_t)svcntp_b64(pg, svbrkb_z(pg, nomatch));
        for (size_t e = 0; e < n_match && base + e < L; e++)
            best = base + e;
        L = base + n_match;
        break;
    }
    if (abs_tol == 0.0 && rel_tol == 0.0) {
        while (L > 0u && _biteq_double(data[L - 1u], needle)) { best = --L; }
    } else {
        while (L > 0u) {
            double diff = fabs(data[L - 1u] - needle);
            double thr  = fmax(abs_tol, rel_tol * fmax(fabs(data[L - 1u]), abs_needle));
            if (diff <= thr) { best = --L; } else { break; }
        }
    }

    if (best == SIZE_MAX) {
        size_t R = mid + 1u;
        while (R + vl <= len) {
            svbool_t    pg    = svptrue_b64();
            svfloat64_t chunk = svld1_f64(pg, data + R);
            svfloat64_t diff  = svabs_f64_z(pg, svsub_f64_z(pg, chunk, vn));
            svfloat64_t abs_c = svabs_f64_z(pg, chunk);
            svfloat64_t thr   = svmax_f64_z(pg, vabs_tol,
                                    svmul_f64_z(pg, vrel_tol,
                                        svmax_f64_z(pg, abs_c, vabs_ndl)));
            svbool_t match = svcmple_f64(pg, diff, thr);
            if (!svptest_any(pg, match)) break;
            if (best == SIZE_MAX) {
                size_t first = (size_t)svcntp_b64(pg, svbrkb_z(pg, match));
                if (R + first < len) best = R + first;
            }
            if (svptest_any(pg, svnot_b_z(pg, match))) break;
            R += vl;
        }
        if (abs_tol == 0.0 && rel_tol == 0.0) {
            while (R < len && best == SIZE_MAX && _biteq_double(data[R], needle)) { best = R++; }
        } else {
            while (R < len && best == SIZE_MAX) {
                double diff = fabs(data[R] - needle);
                double thr  = fmax(abs_tol, rel_tol * fmax(fabs(data[R]), abs_needle));
                if (diff <= thr) { best = R++; } else { break; }
            }
        }
    }
    return best;
}
// ================================================================================ 
// ================================================================================ 

static inline void simd_fill_double(double* data, size_t count, double value) {
    svfloat64_t v = svdup_n_f64(value);
    size_t i = 0u;
    size_t vl = svcntd();
 
    size_t body_end = (count / vl) * vl;
    for (; i < body_end; i += vl) {
        svst1_f64(svptrue_b64(), data + i, v);
    }
 
    if (i < count) {
        svbool_t mask = svwhilelt_b64((uint64_t)i, (uint64_t)count);
        svst1_f64(mask, data + i, v);
    }
}
// -------------------------------------------------------------------------------- 
 
static inline void simd_transpose_double(const double* src,
                                         double*       dst,
                                         size_t        src_rows,
                                         size_t        src_cols) {
    const size_t vl = svcntd();
    const size_t TILE_H = 8u;
    const size_t TILE_W = vl;
 
    size_t i = 0u;
    size_t j = 0u;
 
    size_t row_body = (src_rows / TILE_H) * TILE_H;
    size_t col_body = (src_cols / TILE_W) * TILE_W;
 
    for (i = 0u; i < row_body; i += TILE_H) {
        for (j = 0u; j < col_body; j += TILE_W) {
            for (size_t jj = 0u; jj < TILE_W; ++jj) {
                for (size_t ii = 0u; ii < TILE_H; ++ii) {
                    dst[(j + jj) * src_rows + (i + ii)] =
                        src[(i + ii) * src_cols + (j + jj)];
                }
            }
        }
    }
 
    for (i = 0u; i < row_body; i += TILE_H) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t ii = 0u; ii < TILE_H; ++ii) {
                dst[j * src_rows + (i + ii)] =
                    src[(i + ii) * src_cols + j];
            }
        }
    }
 
    for (i = row_body; i < src_rows; ++i) {
        for (j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
}
// -------------------------------------------------------------------------------- 
 
static inline bool simd_double_arrays_equal(const double* a,
                                            const double* b,
                                            size_t        count) {
    if (a == NULL || b == NULL) return false;

    size_t i = 0u;
    while (i < count) {
        svbool_t pg = svwhilelt_b64(i, count);

        svfloat64_t av = svld1(pg, a + i);
        svfloat64_t bv = svld1(pg, b + i);

        svbool_t cmp = svcmpeq_f64(pg, av, bv);

        if (!svptest_all(pg, cmp)) {
            return false;
        }

        i += svcntd();
    }

    return true;
}
// -------------------------------------------------------------------------------- 
 
static inline size_t simd_count_nonzero_double(const double* data,
                                               size_t        count) {
    size_t nnz = 0u;
    size_t i = 0u;
    size_t vl = svcntd();
 
    size_t body_end = (count / vl) * vl;
    for (; i < body_end; i += vl) {
        svfloat64_t v = svld1_f64(svptrue_b64(), data + i);
 
        svbool_t is_zero    = svcmpeq_n_f64(svptrue_b64(), v, 0.0);
        svbool_t is_nonzero = svnot_b_z(svptrue_b64(), is_zero);
 
        nnz += (size_t)svcntp_b64(svptrue_b64(), is_nonzero);
    }
 
    if (i < count) {
        svbool_t mask = svwhilelt_b64((uint64_t)i, (uint64_t)count);
        svfloat64_t v = svld1_f64(mask, data + i);
 
        svbool_t is_zero    = svcmpeq_n_f64(mask, v, 0.0);
        svbool_t is_nonzero = svnot_b_z(mask, is_zero);
 
        nnz += (size_t)svcntp_b64(mask, is_nonzero);
    }
 
    return nnz;
}
// -------------------------------------------------------------------------------- 
 
static inline size_t simd_scatter_csr_row_double(const double* row_data,
                                                 size_t        cols,
                                                 size_t        col_offset,
                                                 size_t*       col_idx,
                                                 double*       values,
                                                 size_t        k) {
    size_t j = 0u;
    size_t vl = svcntd();
    uint64_t col_tmp[32];
 
    size_t vec_end = (cols / vl) * vl;
    for (; j < vec_end; j += vl) {
        svfloat64_t v = svld1_f64(svptrue_b64(), row_data + j);
 
        svbool_t is_zero    = svcmpeq_n_f64(svptrue_b64(), v, 0.0);
        svbool_t is_nonzero = svnot_b_z(svptrue_b64(), is_zero);
 
        uint64_t popcnt = (uint64_t)svcntp_b64(svptrue_b64(), is_nonzero);
        if (popcnt == 0u) continue;
 
        svfloat64_t packed_vals = svcompact_f64(is_nonzero, v);
        svbool_t store_mask = svwhilelt_b64(0ull, popcnt);
        svst1_f64(store_mask, values + k, packed_vals);
 
        svuint64_t col_vec = svindex_u64((uint64_t)(col_offset + j), 1ull);
        svuint64_t packed_cols = svcompact_u64(is_nonzero, col_vec);
        svst1_u64(store_mask, col_tmp, packed_cols);
 
        for (uint64_t n = 0u; n < popcnt; ++n) {
            col_idx[k + n] = (size_t)col_tmp[n];
        }
 
        k += (size_t)popcnt;
    }
 
    for (; j < cols; ++j) {
        if (row_data[j] != 0.0) {
            col_idx[k] = col_offset + j;
            values[k]  = row_data[j];
            ++k;
        }
    }
 
    return k;
}
// -------------------------------------------------------------------------------- 
 
static inline bool simd_is_all_zero_double(const double* data, size_t count) {
    size_t i = 0u;
    size_t vl = svcntd();
 
    size_t body_end = (count / vl) * vl;
    for (; i < body_end; i += vl) {
        svfloat64_t v = svld1_f64(svptrue_b64(), data + i);
 
        svbool_t is_zero    = svcmpeq_n_f64(svptrue_b64(), v, 0.0);
        svbool_t is_nonzero = svnot_b_z(svptrue_b64(), is_zero);
 
        if (svptest_any(svptrue_b64(), is_nonzero)) return false;
    }
 
    if (i < count) {
        svbool_t mask = svwhilelt_b64((uint64_t)i, (uint64_t)count);
        svfloat64_t v = svld1_f64(mask, data + i);
 
        svbool_t is_zero    = svcmpeq_n_f64(mask, v, 0.0);
        svbool_t is_nonzero = svnot_b_z(mask, is_zero);
 
        if (svptest_any(mask, is_nonzero)) return false;
    }
 
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_SVE2_DOUBLE_INL */
