#ifndef SIMD_SVE_FLOAT_INL
#define SIMD_SVE_FLOAT_INL
// ================================================================================ 
// ================================================================================ 

/**
 * @file simd_sve_float.inl
 * @brief ARM SVE implementation of tolerance-aware float search.
 *
 * The vector length is queried at runtime via svcntw() so this code runs
 * correctly on any SVE-capable core regardless of its vector width.
 *
 * Exact mode (both tolerances == 0.0f):
 *   The float vector is reinterpreted as uint32 via svreinterpret_u32_f32
 *   and compared with svcmpeq_u32.  This preserves bitwise semantics:
 *   NaN != NaN, -0.0f != +0.0f.  svptest_any detects a hit and
 *   svcntp_b32(svbrkb_z) counts active lanes before the first match to
 *   compute its index.
 *
 * Tolerance mode:
 *   svabs_f32_z computes |element|, svsub_f32_z + svabs_f32_z gives the
 *   absolute difference, svmax_f32_z + svmul_f32_z builds the threshold,
 *   and svcmple_f32 returns the match predicate directly.
 *
 * Requires: -march=armv8-a+sve or -march=armv9-a
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <arm_sve.h>

// ================================================================================
// Public interface
// ================================================================================

static size_t simd_contains_float(const float* data,
                                   size_t       start,
                                   size_t       end,
                                   float        needle,
                                   float        abs_tol,
                                   float        rel_tol) {
    size_t vl = svcntw();   /* number of 32-bit lanes in the hardware vector */
    size_t i  = start;

    if (abs_tol == 0.0f && rel_tol == 0.0f) {
        /*
         * Reinterpret as uint32 for bitwise equality.
         * needle reinterpreted via union to get its bit pattern.
         */
        union { float f; uint32_t u; } nu; nu.f = needle;
        svuint32_t vn = svdup_n_u32(nu.u);

        while (i + vl <= end) {
            svbool_t   pg    = svptrue_b32();
            svfloat32_t fchunk = svld1_f32(pg, data + i);
            svuint32_t  chunk  = svreinterpret_u32_f32(fchunk);
            svbool_t    cmp    = svcmpeq_u32(pg, chunk, vn);
            if (svptest_any(pg, cmp)) {
                /* Count active lanes before the first match */
                return i + (size_t)svcntp_b32(pg, svbrkb_z(pg, cmp));
            }
            i += vl;
        }
    } else {
        svfloat32_t vn          = svdup_n_f32(needle);
        svfloat32_t vabs_tol    = svdup_n_f32(abs_tol);
        svfloat32_t vrel_tol    = svdup_n_f32(rel_tol);
        svfloat32_t vabs_needle = svdup_n_f32(fabsf(needle));

        while (i + vl <= end) {
            svbool_t    pg        = svptrue_b32();
            svfloat32_t chunk     = svld1_f32(pg, data + i);
            svfloat32_t diff      = svabs_f32_z(pg, svsub_f32_z(pg, chunk, vn));
            svfloat32_t abs_chunk = svabs_f32_z(pg, chunk);
            svfloat32_t rel_base  = svmax_f32_z(pg, abs_chunk, vabs_needle);
            svfloat32_t rel_thresh = svmul_f32_z(pg, vrel_tol, rel_base);
            svfloat32_t threshold  = svmax_f32_z(pg, vabs_tol, rel_thresh);
            svbool_t    cmp        = svcmple_f32(pg, diff, threshold);
            if (svptest_any(pg, cmp)) {
                return i + (size_t)svcntp_b32(pg, svbrkb_z(pg, cmp));
            }
            i += vl;
        }
    }

    /* Scalar remainder */
    if (abs_tol == 0.0f && rel_tol == 0.0f) {
        union { float f; uint32_t u; } n; n.f = needle;
        for (; i < end; i++) {
            union { float f; uint32_t u; } v; v.f = data[i];
            if (v.u == n.u) return i;
        }
    } else {
        for (; i < end; i++) {
            float diff      = fabsf(data[i] - needle);
            float threshold = fmaxf(abs_tol,
                                    rel_tol * fmaxf(fabsf(data[i]), fabsf(needle)));
            if (diff <= threshold) return i;
        }
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static size_t simd_binary_search_float(const float* data,
                                        size_t       len,
                                        float        needle,
                                        float        abs_tol,
                                        float        rel_tol,
                                        int        (*cmp)(const void*, const void*)) {
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
    if (lo > hi) return SIZE_MAX;   /* empty range after underflow guard */
    mid = lo;

    /* ── Tolerance fan-out ── */
    size_t vl        = svcntw();
    size_t best      = SIZE_MAX;
    float  abs_needle = fabsf(needle);
    svfloat32_t vn       = svdup_n_f32(needle);
    svfloat32_t vabs_tol = svdup_n_f32(abs_tol);
    svfloat32_t vrel_tol = svdup_n_f32(rel_tol);
    svfloat32_t vabs_ndl = svdup_n_f32(abs_needle);

    /* Check landing */
    {
        float diff = fabsf(data[mid] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[mid]), abs_needle));
        if (diff <= thr) best = mid;
    }

    /* SVE left walk */
    size_t L = mid;
    while (L >= vl) {
        size_t      base  = L - vl;
        svbool_t    pg    = svptrue_b32();
        svfloat32_t chunk = svld1_f32(pg, data + base);
        svfloat32_t diff  = svabs_f32_z(pg, svsub_f32_z(pg, chunk, vn));
        svfloat32_t abs_c = svabs_f32_z(pg, chunk);
        svfloat32_t thr   = svmax_f32_z(pg, vabs_tol,
                                svmul_f32_z(pg, vrel_tol,
                                    svmax_f32_z(pg, abs_c, vabs_ndl)));
        svbool_t    match = svcmple_f32(pg, diff, thr);
        svbool_t    nomatch = svnot_b_z(pg, match);
        if (!svptest_any(pg, nomatch)) {
            /* All lanes match */
            best = base; L = base; continue;
        }
        /* Find first non-matching lane from the low end */
        size_t n_match = (size_t)svcntp_b32(pg, svbrkb_z(pg, nomatch));
        for (size_t e = 0; e < n_match && base + e < L; e++)
            best = base + e;
        L = base + n_match;
        break;
    }
    /* Scalar left mop-up */
    while (L > 0u) {
        float diff = fabsf(data[L - 1u] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[L - 1u]), abs_needle));
        if (diff <= thr) { best = --L; } else { break; }
    }

    /* SVE right walk */
    if (best == SIZE_MAX) {
        size_t R = mid + 1u;
        while (R + vl <= len) {
            svbool_t    pg    = svptrue_b32();
            svfloat32_t chunk = svld1_f32(pg, data + R);
            svfloat32_t diff  = svabs_f32_z(pg, svsub_f32_z(pg, chunk, vn));
            svfloat32_t abs_c = svabs_f32_z(pg, chunk);
            svfloat32_t thr   = svmax_f32_z(pg, vabs_tol,
                                    svmul_f32_z(pg, vrel_tol,
                                        svmax_f32_z(pg, abs_c, vabs_ndl)));
            svbool_t    match = svcmple_f32(pg, diff, thr);
            if (!svptest_any(pg, match)) break;
            /* Record only the first match in this block */
            if (best == SIZE_MAX) {
                size_t first = (size_t)svcntp_b32(pg, svbrkb_z(pg, match));
                if (R + first < len) best = R + first;
            }
            /* Stop when a non-matching lane is found */
            svbool_t nomatch = svnot_b_z(pg, match);
            if (svptest_any(pg, nomatch)) break;
            R += vl;
        }
        /* Scalar right mop-up */
        while (R < len && best == SIZE_MAX) {
            float diff = fabsf(data[R] - needle);
            float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[R]), abs_needle));
            if (diff <= thr) { best = R++; } else { break; }
        }
    }
    return best;
}
// ================================================================================ 
// ================================================================================ 
// MATRIX METHODS

static inline void simd_fill_float(float* data, size_t count, float value) {
    svfloat32_t v = svdup_n_f32(value);
 
    size_t i = 0u;
    size_t vl = svcntw();   /* floats per vector — known at runtime */
 
    /* ---- Main body: full-vector stores ---- */
    size_t body_end = (count / vl) * vl;
    for (; i < body_end; i += vl) {
        svst1_f32(svptrue_b32(), data + i, v);
    }
 
    /* ---- Predicated tail ---- */
    if (i < count) {
        svbool_t mask = svwhilelt_b32((uint32_t)i, (uint32_t)count);
        svst1_f32(mask, data + i, v);
    }
}
// -------------------------------------------------------------------------------- 

static inline void simd_transpose_float(const float* src,
                                        float*       dst,
                                        size_t       src_rows,
                                        size_t       src_cols) {
    /*
     * Use a fixed tile height of 8 rows.  The tile width equals the
     * SVE vector length in 32-bit elements.  This keeps the tile small
     * enough for L1 while still benefiting from predicated loads.
     */
    const size_t vl = svcntw();
    const size_t TILE_H = 8u;
    const size_t TILE_W = vl;
 
    size_t i = 0u;
    size_t j = 0u;
 
    size_t row_body = (src_rows / TILE_H) * TILE_H;
    size_t col_body = (src_cols / TILE_W) * TILE_W;
 
    /* ---- Tiled body: TILE_H × TILE_W blocks ---- */
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
 
    /* ---- Remainder columns (right strip) ---- */
    for (i = 0u; i < row_body; i += TILE_H) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t ii = 0u; ii < TILE_H; ++ii) {
                dst[j * src_rows + (i + ii)] =
                    src[(i + ii) * src_cols + j];
            }
        }
    }
 
    /* ---- Remainder rows (bottom strip, including corner) ---- */
    for (i = row_body; i < src_rows; ++i) {
        for (j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
}
// -------------------------------------------------------------------------------- 

static inline bool simd_equal_float(const float* a,
                                    const float* b,
                                    size_t       count) {
    size_t i = 0u;
    size_t vl = svcntw();   /* floats per vector */
 
    /* ---- Main body: full-vector checks ---- */
    size_t body_end = (count / vl) * vl;
    for (; i < body_end; i += vl) {
        svuint32_t va = svld1_u32(svptrue_b32(),
                                  (const uint32_t*)(a + i));
        svuint32_t vb = svld1_u32(svptrue_b32(),
                                  (const uint32_t*)(b + i));
 
        svuint32_t x = sveor_u32_z(svptrue_b32(), va, vb);
 
        /* svptest_any returns true if ANY active lane in the predicate
           result is set.  svcmpne checks which lanes of x are nonzero. */
        svbool_t neq = svcmpne_n_u32(svptrue_b32(), x, 0u);
        if (svptest_any(svptrue_b32(), neq)) return false;
    }
 
    /* ---- Predicated tail ---- */
    if (i < count) {
        svbool_t mask = svwhilelt_b32((uint32_t)i, (uint32_t)count);
 
        svuint32_t va = svld1_u32(mask, (const uint32_t*)(a + i));
        svuint32_t vb = svld1_u32(mask, (const uint32_t*)(b + i));
 
        svuint32_t x = sveor_u32_z(mask, va, vb);
 
        svbool_t neq = svcmpne_n_u32(mask, x, 0u);
        if (svptest_any(mask, neq)) return false;
    }
 
    return true;
}
// -------------------------------------------------------------------------------- 

static inline size_t simd_count_nonzero_float(const float* data,
                                              size_t       count) {
    size_t nnz = 0u;
    size_t i = 0u;
    size_t vl = svcntw();
 
    /* ---- Main body ---- */
    size_t body_end = (count / vl) * vl;
    for (; i < body_end; i += vl) {
        svfloat32_t v = svld1_f32(svptrue_b32(), data + i);
 
        svbool_t is_zero    = svcmpeq_n_f32(svptrue_b32(), v, 0.0f);
        svbool_t is_nonzero = svnot_b_z(svptrue_b32(), is_zero);
 
        nnz += (size_t)svcntp_b32(svptrue_b32(), is_nonzero);
    }
 
    /* ---- Predicated tail ---- */
    if (i < count) {
        svbool_t mask = svwhilelt_b32((uint32_t)i, (uint32_t)count);
        svfloat32_t v = svld1_f32(mask, data + i);
 
        svbool_t is_zero    = svcmpeq_n_f32(mask, v, 0.0f);
        svbool_t is_nonzero = svnot_b_z(mask, is_zero);
 
        nnz += (size_t)svcntp_b32(mask, is_nonzero);
    }
 
    return nnz;
}
 
// --------------------------------------------------------------------------------
 
static inline size_t simd_scatter_csr_row_float(const float* row_data,
                                                size_t       cols,
                                                size_t       col_offset,
                                                size_t*      col_idx,
                                                float*       values,
                                                size_t       k) {
    size_t j = 0u;
    size_t vl = svcntw();
 
    /*
     * Stack buffer for spilling compacted column indices.
     * SVE vectors are at most 2048 bits = 64 x 32-bit lanes.
     */
    uint32_t col_tmp[64];
 
    /* ---- Main body ---- */
    size_t vec_end = (cols / vl) * vl;
    for (; j < vec_end; j += vl) {
        svfloat32_t v = svld1_f32(svptrue_b32(), row_data + j);
 
        svbool_t is_zero    = svcmpeq_n_f32(svptrue_b32(), v, 0.0f);
        svbool_t is_nonzero = svnot_b_z(svptrue_b32(), is_zero);
 
        uint32_t popcnt = (uint32_t)svcntp_b32(svptrue_b32(), is_nonzero);
        if (popcnt == 0u) continue;
 
        /* Compact nonzero values and store */
        svfloat32_t packed_vals = svcompact_f32(is_nonzero, v);
        svbool_t store_mask = svwhilelt_b32(0u, popcnt);
        svst1_f32(store_mask, values + k, packed_vals);
 
        /* Compact column indices and spill to stack */
        svuint32_t col_vec = svindex_u32((uint32_t)(col_offset + j), 1u);
        svuint32_t packed_cols = svcompact_u32(is_nonzero, col_vec);
        svst1_u32(store_mask, col_tmp, packed_cols);
 
        /* Widen uint32 -> size_t */
        for (uint32_t n = 0u; n < popcnt; ++n) {
            col_idx[k + n] = (size_t)col_tmp[n];
        }
 
        k += (size_t)popcnt;
    }
 
    /* ---- Scalar tail ---- */
    for (; j < cols; ++j) {
        if (row_data[j] != 0.0f) {
            col_idx[k] = col_offset + j;
            values[k]  = row_data[j];
            ++k;
        }
    }
 
    return k;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_SVE_FLOAT_INL */ 
// ================================================================================ 
// ================================================================================ 
// eof
