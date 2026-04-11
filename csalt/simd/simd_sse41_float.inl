#ifndef SIMD_SSE41_FLOAT_INL
#define SIMD_SSE41_FLOAT_INL
// ================================================================================ 
// ================================================================================ 

/**
 * @file simd_sse41_float.inl
 * @brief SSE4.1 implementation of tolerance-aware float search.
 *
 * SSE4.1 adds _mm_blendv_ps which is not needed here, but more usefully
 * provides _mm_cmpeq_epi32 (also in SSE2, so no change for exact mode)
 * and _mm_round_ps.  The primary benefit over SSE2 for this operation is
 * _mm_dp_ps (dot product) which is irrelevant here, so the tolerance math
 * is the same as SSE2.  The file exists as a distinct dispatch target so
 * future SSE4.1-specific refinements can be added without restructuring
 * the dispatch chain.
 *
 * Exact mode uses _mm_cmpeq_epi32 via integer reinterpretation — bitwise
 * equality, NaN != NaN, -0.0f != +0.0f.
 *
 * Requires: -msse4.1
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <smmintrin.h>   /* SSE4.1 */

// ================================================================================
// Internal helpers
// ================================================================================

static inline __m128 _sse41_fabs_ps(__m128 v) {
    __m128i mask = _mm_set1_epi32(0x7FFFFFFF);
    return _mm_and_ps(v, _mm_castsi128_ps(mask));
}

static inline int _sse41_exact_mask(__m128 chunk, __m128 vn) {
    __m128i ci  = _mm_castps_si128(chunk);
    __m128i ni  = _mm_castps_si128(vn);
    __m128i cmp = _mm_cmpeq_epi32(ci, ni);
    return _mm_movemask_ps(_mm_castsi128_ps(cmp));
}

static inline int _sse41_tol_mask(__m128 chunk, __m128 vn,
                                   __m128 vabs_tol, __m128 vrel_tol,
                                   __m128 vabs_needle) {
    __m128 diff       = _sse41_fabs_ps(_mm_sub_ps(chunk, vn));
    __m128 abs_chunk  = _sse41_fabs_ps(chunk);
    __m128 rel_base   = _mm_max_ps(abs_chunk, vabs_needle);
    __m128 rel_thresh = _mm_mul_ps(vrel_tol, rel_base);
    __m128 threshold  = _mm_max_ps(vabs_tol, rel_thresh);
    return _mm_movemask_ps(_mm_cmple_ps(diff, threshold));
}

// ================================================================================
// Public interface
// ================================================================================

static size_t simd_contains_float(const float* data,
                                   size_t       start,
                                   size_t       end,
                                   float        needle,
                                   float        abs_tol,
                                   float        rel_tol) {
    size_t i  = start;
    __m128 vn = _mm_set1_ps(needle);

    if (abs_tol == 0.0f && rel_tol == 0.0f) {
        while (i + 4u <= end) {
            __m128 chunk = _mm_loadu_ps(data + i);
            int    mask  = _sse41_exact_mask(chunk, vn);
            if (mask != 0) {
                for (size_t e = 0; e < 4u && i + e < end; e++) {
                    if ((mask >> (int)e) & 1) return i + e;
                }
            }
            i += 4u;
        }
    } else {
        __m128 vabs_tol    = _mm_set1_ps(abs_tol);
        __m128 vrel_tol    = _mm_set1_ps(rel_tol);
        __m128 vabs_needle = _mm_set1_ps(fabsf(needle));

        while (i + 4u <= end) {
            __m128 chunk = _mm_loadu_ps(data + i);
            int    mask  = _sse41_tol_mask(chunk, vn, vabs_tol, vrel_tol,
                                           vabs_needle);
            if (mask != 0) {
                for (size_t e = 0; e < 4u && i + e < end; e++) {
                    if ((mask >> (int)e) & 1) return i + e;
                }
            }
            i += 4u;
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
    size_t best = SIZE_MAX;
    float  abs_needle = fabsf(needle);
    __m128 vn         = _mm_set1_ps(needle);
    __m128 vabs_tol   = _mm_set1_ps(abs_tol);
    __m128 vrel_tol   = _mm_set1_ps(rel_tol);
    __m128 vabs_ndl   = _mm_set1_ps(abs_needle);

    /* Check landing */
    {
        float diff = fabsf(data[mid] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[mid]), abs_needle));
        if (diff <= thr) best = mid;
    }

    /* SIMD left walk — 4 floats per step */
    size_t L = mid;
    while (L >= 4u) {
        size_t base  = L - 4u;
        __m128 chunk = _mm_loadu_ps(data + base);
        int    mask  = _sse41_tol_mask(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
        /* All 4 match — the entire block is within tolerance */
        if (mask == 0xF) { best = base; L = base; continue; }
        /* Partial match — find the rightmost matching lane from low end */
        for (size_t e = 0; e < 4u && base + e < L; e++) {
            if ((mask >> (int)e) & 1) { best = base + e; }
            else                      { L = base + e; goto left_done_sse41; }
        }
        L = base;
    }
    left_done_sse41:
    /* Scalar left mop-up */
    while (L > 0u) {
        float diff = fabsf(data[L - 1u] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[L - 1u]), abs_needle));
        if (diff <= thr) { best = --L; } else { break; }
    }

    /* SIMD right walk — only if landing didn't match */
    if (best == SIZE_MAX) {
        size_t R = mid + 1u;
        while (R + 4u <= len) {
            __m128 chunk = _mm_loadu_ps(data + R);
            int    mask  = _sse41_tol_mask(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
            if (mask == 0) break;   /* none match — tolerance region ended */
            for (size_t e = 0; e < 4u && R + e < len; e++) {
                if ((mask >> (int)e) & 1) { if (best == SIZE_MAX) best = R + e; }
                else                      { goto right_done_sse41; }
            }
            R += 4u;
        }
        right_done_sse41:
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
    __m128 v = _mm_set1_ps(value);
 
    size_t i = 0u;
 
    /* ---- Unaligned head ---- */
    while (i < count && ((uintptr_t)(data + i) & 15u) != 0u) {
        data[i] = value;
        ++i;
    }
 
    /* ---- Aligned body: 16 floats per iteration ---- */
    size_t aligned_end = i + ((count - i) / 16u) * 16u;
    for (; i < aligned_end; i += 16u) {
        _mm_store_ps(data + i,        v);
        _mm_store_ps(data + i + 4u,   v);
        _mm_store_ps(data + i + 8u,   v);
        _mm_store_ps(data + i + 12u,  v);
    }
 
    /* ---- Single-vector passes ---- */
    aligned_end = i + ((count - i) / 4u) * 4u;
    for (; i < aligned_end; i += 4u) {
        _mm_store_ps(data + i, v);
    }
 
    /* ---- Scalar tail ---- */
    for (; i < count; ++i) {
        data[i] = value;
    }
}
// -------------------------------------------------------------------------------- 

static inline void _sse41_transpose_4x4(__m128 r0, __m128 r1,
                                         __m128 r2, __m128 r3,
                                         __m128* o0, __m128* o1,
                                         __m128* o2, __m128* o3) {
    __m128 t0 = _mm_unpacklo_ps(r0, r1);
    __m128 t1 = _mm_unpackhi_ps(r0, r1);
    __m128 t2 = _mm_unpacklo_ps(r2, r3);
    __m128 t3 = _mm_unpackhi_ps(r2, r3);
 
    *o0 = _mm_movelh_ps(t0, t2);
    *o1 = _mm_movehl_ps(t2, t0);
    *o2 = _mm_movelh_ps(t1, t3);
    *o3 = _mm_movehl_ps(t3, t1);
}
 
static inline void simd_transpose_float(const float* src,
                                        float*       dst,
                                        size_t       src_rows,
                                        size_t       src_cols) {
    size_t i = 0u;
    size_t j = 0u;
 
    size_t row_body = (src_rows / 4u) * 4u;
    size_t col_body = (src_cols / 4u) * 4u;
 
    /* ---- Tiled 4×4 body ---- */
    for (i = 0u; i < row_body; i += 4u) {
        for (j = 0u; j < col_body; j += 4u) {
            __m128 r0 = _mm_loadu_ps(src + (i + 0u) * src_cols + j);
            __m128 r1 = _mm_loadu_ps(src + (i + 1u) * src_cols + j);
            __m128 r2 = _mm_loadu_ps(src + (i + 2u) * src_cols + j);
            __m128 r3 = _mm_loadu_ps(src + (i + 3u) * src_cols + j);
 
            __m128 o0, o1, o2, o3;
            _sse41_transpose_4x4(r0, r1, r2, r3, &o0, &o1, &o2, &o3);
 
            _mm_storeu_ps(dst + (j + 0u) * src_rows + i, o0);
            _mm_storeu_ps(dst + (j + 1u) * src_rows + i, o1);
            _mm_storeu_ps(dst + (j + 2u) * src_rows + i, o2);
            _mm_storeu_ps(dst + (j + 3u) * src_rows + i, o3);
        }
    }
 
    /* ---- Remainder columns ---- */
    for (i = 0u; i < row_body; i += 4u) {
        for (j = col_body; j < src_cols; ++j) {
            dst[j * src_rows + (i + 0u)] = src[(i + 0u) * src_cols + j];
            dst[j * src_rows + (i + 1u)] = src[(i + 1u) * src_cols + j];
            dst[j * src_rows + (i + 2u)] = src[(i + 2u) * src_cols + j];
            dst[j * src_rows + (i + 3u)] = src[(i + 3u) * src_cols + j];
        }
    }
 
    /* ---- Remainder rows ---- */
    for (i = row_body; i < src_rows; ++i) {
        for (j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
}
// -------------------------------------------------------------------------------- 

static inline bool simd_float_arrays_equal(const float* a,
                                           const float* b,
                                           size_t       count) {
    if (a == NULL || b == NULL) return false;

    size_t i = 0u;
    size_t vec_end = (count / 4u) * 4u;

    for (; i < vec_end; i += 4u) {
        __m128 av = _mm_loadu_ps(a + i);
        __m128 bv = _mm_loadu_ps(b + i);
        __m128 cv = _mm_cmpeq_ps(av, bv);

        if (_mm_movemask_ps(cv) != 0xF) {
            return false;
        }
    }

    for (; i < count; ++i) {
        if (!(a[i] == b[i])) {
            return false;
        }
    }

    return true;
}
// -------------------------------------------------------------------------------- 

static inline size_t simd_count_nonzero_float(const float* data,
                                              size_t       count) {
    size_t nnz = 0u;
    size_t i = 0u;
 
    __m128 zero = _mm_setzero_ps();
 
    /* ---- Body: 16 floats per iteration with fast all-zero skip ---- */
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        /* Quick check: OR all four vectors — if result is all-zero
           the entire 16-element block is zero and we skip it. */
        __m128 v0 = _mm_loadu_ps(data + i);
        __m128 v1 = _mm_loadu_ps(data + i + 4u);
        __m128 v2 = _mm_loadu_ps(data + i + 8u);
        __m128 v3 = _mm_loadu_ps(data + i + 12u);
 
        __m128 any = _mm_or_ps(_mm_or_ps(v0, v1), _mm_or_ps(v2, v3));
        __m128i any_i = _mm_castps_si128(any);
 
        if (_mm_testz_si128(any_i, any_i)) continue;   /* all 16 are zero */
 
        /* At least one nonzero — count per vector */
        int m0 = ~_mm_movemask_ps(_mm_cmpeq_ps(v0, zero)) & 0xF;
        int m1 = ~_mm_movemask_ps(_mm_cmpeq_ps(v1, zero)) & 0xF;
        int m2 = ~_mm_movemask_ps(_mm_cmpeq_ps(v2, zero)) & 0xF;
        int m3 = ~_mm_movemask_ps(_mm_cmpeq_ps(v3, zero)) & 0xF;
 
        nnz += (size_t)__builtin_popcount(
            (unsigned)(m0 | (m1 << 4) | (m2 << 8) | (m3 << 12)));
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        __m128 v = _mm_loadu_ps(data + i);
        __m128i vi = _mm_castps_si128(v);
 
        if (_mm_testz_si128(vi, vi)) continue;
 
        int m = ~_mm_movemask_ps(_mm_cmpeq_ps(v, _mm_setzero_ps())) & 0xF;
        nnz += (size_t)__builtin_popcount((unsigned)m);
    }
 
    /* ---- Scalar tail ---- */
    for (; i < count; ++i) {
        if (data[i] != 0.0f) ++nnz;
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
    for (size_t j = 0u; j < cols; ++j) {
        if (row_data[j] != 0.0f) {
            col_idx[k] = col_offset + j;
            values[k]  = row_data[j];
            ++k;
        }
    }
    return k;
}
// -------------------------------------------------------------------------------- 

static inline bool simd_is_all_zero_float(const float* data, size_t count) {
    size_t i = 0u;
 
    __m128 zero = _mm_setzero_ps();
 
    /* ---- Body: 16 floats per iteration ---- */
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        __m128 eq0 = _mm_cmpeq_ps(_mm_loadu_ps(data + i),       zero);
        __m128 eq1 = _mm_cmpeq_ps(_mm_loadu_ps(data + i + 4u),  zero);
        __m128 eq2 = _mm_cmpeq_ps(_mm_loadu_ps(data + i + 8u),  zero);
        __m128 eq3 = _mm_cmpeq_ps(_mm_loadu_ps(data + i + 12u), zero);
 
        /* AND all equality masks: all-ones iff every lane is zero */
        __m128 all_eq = _mm_and_ps(_mm_and_ps(eq0, eq1),
                                   _mm_and_ps(eq2, eq3));
 
        /* Invert: if any lane was nonzero, inverted mask has set bits.
           testz returns 1 iff (a & b) == 0.  We test ~all_eq against
           itself — if all_eq was all-ones, ~all_eq is all-zeros → testz=1.
           If any lane was nonzero, ~all_eq has bits set → testz=0. */
        __m128i inv = _mm_andnot_si128(_mm_castps_si128(all_eq),
                                       _mm_set1_epi32(-1));
        if (!_mm_testz_si128(inv, inv)) return false;
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        __m128 eq = _mm_cmpeq_ps(_mm_loadu_ps(data + i), zero);
        __m128i inv = _mm_andnot_si128(_mm_castps_si128(eq),
                                       _mm_set1_epi32(-1));
        if (!_mm_testz_si128(inv, inv)) return false;
    }
 
    /* ---- Scalar tail ---- */
    for (; i < count; ++i) {
        if (data[i] != 0.0f) return false;
    }
 
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_SSE41_FLOAT_INL */ 
// ================================================================================ 
// ================================================================================ 
// eof
