#ifndef SIMD_AVX2_FLOAT_INL
#define SIMD_AVX2_FLOAT_INL
// ================================================================================ 
// ================================================================================ 

/**
 * @file simd_avx2_float.inl
 * @brief AVX2 implementation of tolerance-aware float search.
 *
 * Processes 8 floats per iteration using 256-bit YMM registers.
 *
 * Exact mode (both tolerances == 0.0f):
 *   AVX2 provides _mm256_cmpeq_epi32 for native 256-bit integer comparison.
 *   The float register is reinterpreted with _mm256_castps_si256 so the
 *   comparison is bitwise: NaN != NaN, -0.0f (0x80000000) != +0.0f.
 *   _mm256_movemask_ps on the cast result gives an 8-bit lane mask directly.
 *
 * Tolerance mode:
 *   Uses _mm256_cmp_ps(_CMP_LE_OQ) on the absolute difference against the
 *   per-lane threshold.  _mm256_andnot_ps clears the sign bit for fabsf.
 *
 * Requires: -mavx2
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <immintrin.h>   /* AVX2 */

// ================================================================================
// Internal helpers
// ================================================================================

static inline __m256 _avx2_fabs_ps(__m256 v) {
    __m256 sign_mask = _mm256_set1_ps(-0.0f);
    return _mm256_andnot_ps(sign_mask, v);
}

/* Exact bitwise match using AVX2 native 256-bit integer compare. */
static inline int _avx2_exact_mask(__m256 chunk, __m256 vn) {
    __m256i ci  = _mm256_castps_si256(chunk);
    __m256i ni  = _mm256_castps_si256(vn);
    __m256i cmp = _mm256_cmpeq_epi32(ci, ni);
    return _mm256_movemask_ps(_mm256_castsi256_ps(cmp));
}

static inline int _avx2_tol_mask(__m256 chunk, __m256 vn,
                                  __m256 vabs_tol, __m256 vrel_tol,
                                  __m256 vabs_needle) {
    __m256 diff       = _avx2_fabs_ps(_mm256_sub_ps(chunk, vn));
    __m256 abs_chunk  = _avx2_fabs_ps(chunk);
    __m256 rel_base   = _mm256_max_ps(abs_chunk, vabs_needle);
    __m256 rel_thresh = _mm256_mul_ps(vrel_tol, rel_base);
    __m256 threshold  = _mm256_max_ps(vabs_tol, rel_thresh);
    return _mm256_movemask_ps(_mm256_cmp_ps(diff, threshold, _CMP_LE_OQ));
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
    __m256 vn = _mm256_set1_ps(needle);

    if (abs_tol == 0.0f && rel_tol == 0.0f) {
        while (i + 8u <= end) {
            __m256 chunk = _mm256_loadu_ps(data + i);
            int    mask  = _avx2_exact_mask(chunk, vn);
            if (mask != 0) {
                /* __builtin_ctz gives index of lowest set bit — one bit per lane */
                _mm256_zeroupper();
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
            i += 8u;
        }
    } else {
        __m256 vabs_tol    = _mm256_set1_ps(abs_tol);
        __m256 vrel_tol    = _mm256_set1_ps(rel_tol);
        __m256 vabs_needle = _mm256_set1_ps(fabsf(needle));

        while (i + 8u <= end) {
            __m256 chunk = _mm256_loadu_ps(data + i);
            int    mask  = _avx2_tol_mask(chunk, vn, vabs_tol, vrel_tol,
                                          vabs_needle);
            if (mask != 0) {
                _mm256_zeroupper();
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
            i += 8u;
        }
    }

    _mm256_zeroupper();

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
    size_t best      = SIZE_MAX;
    float  abs_needle = fabsf(needle);
    __m256 vn        = _mm256_set1_ps(needle);
    __m256 vabs_tol  = _mm256_set1_ps(abs_tol);
    __m256 vrel_tol  = _mm256_set1_ps(rel_tol);
    __m256 vabs_ndl  = _mm256_set1_ps(abs_needle);

    /* Check landing */
    {
        float diff = fabsf(data[mid] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[mid]), abs_needle));
        if (diff <= thr) best = mid;
    }

    /* SIMD left walk — 8 floats per step */
    size_t L = mid;
    while (L >= 8u) {
        size_t base  = L - 8u;
        __m256 chunk = _mm256_loadu_ps(data + base);
        int    mask  = _avx2_tol_mask(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
        if (mask == 0xFF) { best = base; L = base; continue; }
        for (size_t e = 0; e < 8u && base + e < L; e++) {
            if ((mask >> (int)e) & 1) { best = base + e; }
            else                      { L = base + e; goto left_done_avx2; }
        }
        L = base;
    }
    left_done_avx2:
    _mm256_zeroupper();
    while (L > 0u) {
        float diff = fabsf(data[L - 1u] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[L - 1u]), abs_needle));
        if (diff <= thr) { best = --L; } else { break; }
    }

    /* SIMD right walk */
    if (best == SIZE_MAX) {
        size_t R = mid + 1u;
        while (R + 8u <= len) {
            __m256 chunk = _mm256_loadu_ps(data + R);
            int    mask  = _avx2_tol_mask(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
            if (mask == 0) break;
            for (size_t e = 0; e < 8u && R + e < len; e++) {
                if ((mask >> (int)e) & 1) { if (best == SIZE_MAX) best = R + e; }
                else                      { goto right_done_avx2; }
            }
            R += 8u;
        }
        right_done_avx2:
        _mm256_zeroupper();
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
    __m256 v = _mm256_set1_ps(value);
 
    size_t i = 0u;
 
    /* ---- Unaligned head: scalar until 32-byte aligned ---- */
    while (i < count && ((uintptr_t)(data + i) & 31u) != 0u) {
        data[i] = value;
        ++i;
    }
 
    /* ---- Aligned body: 32 floats (4 × 256-bit) per iteration ---- */
    size_t aligned_end = i + ((count - i) / 32u) * 32u;
    for (; i < aligned_end; i += 32u) {
        _mm256_store_ps(data + i,        v);
        _mm256_store_ps(data + i + 8u,   v);
        _mm256_store_ps(data + i + 16u,  v);
        _mm256_store_ps(data + i + 24u,  v);
    }
 
    /* ---- Single-vector passes ---- */
    aligned_end = i + ((count - i) / 8u) * 8u;
    for (; i < aligned_end; i += 8u) {
        _mm256_store_ps(data + i, v);
    }
 
    /* ---- Scalar tail ---- */
    for (; i < count; ++i) {
        data[i] = value;
    }
 
    _mm256_zeroupper();
}
// -------------------------------------------------------------------------------- 

static inline void _avx2_transpose_8x8(const __m256 r[8], __m256 o[8]) {
    /* Phase 1: 2×2 interleave within 128-bit lanes */
    __m256 t0 = _mm256_unpacklo_ps(r[0], r[1]);
    __m256 t1 = _mm256_unpackhi_ps(r[0], r[1]);
    __m256 t2 = _mm256_unpacklo_ps(r[2], r[3]);
    __m256 t3 = _mm256_unpackhi_ps(r[2], r[3]);
    __m256 t4 = _mm256_unpacklo_ps(r[4], r[5]);
    __m256 t5 = _mm256_unpackhi_ps(r[4], r[5]);
    __m256 t6 = _mm256_unpacklo_ps(r[6], r[7]);
    __m256 t7 = _mm256_unpackhi_ps(r[6], r[7]);
 
    /* Phase 2: 4×4 shuffle within 128-bit lanes */
    __m256 s0 = _mm256_shuffle_ps(t0, t2, 0x44);
    __m256 s1 = _mm256_shuffle_ps(t0, t2, 0xEE);
    __m256 s2 = _mm256_shuffle_ps(t1, t3, 0x44);
    __m256 s3 = _mm256_shuffle_ps(t1, t3, 0xEE);
    __m256 s4 = _mm256_shuffle_ps(t4, t6, 0x44);
    __m256 s5 = _mm256_shuffle_ps(t4, t6, 0xEE);
    __m256 s6 = _mm256_shuffle_ps(t5, t7, 0x44);
    __m256 s7 = _mm256_shuffle_ps(t5, t7, 0xEE);
 
    /* Phase 3: cross-lane permute */
    o[0] = _mm256_permute2f128_ps(s0, s4, 0x20);
    o[1] = _mm256_permute2f128_ps(s1, s5, 0x20);
    o[2] = _mm256_permute2f128_ps(s2, s6, 0x20);
    o[3] = _mm256_permute2f128_ps(s3, s7, 0x20);
    o[4] = _mm256_permute2f128_ps(s0, s4, 0x31);
    o[5] = _mm256_permute2f128_ps(s1, s5, 0x31);
    o[6] = _mm256_permute2f128_ps(s2, s6, 0x31);
    o[7] = _mm256_permute2f128_ps(s3, s7, 0x31);
}
 
static inline void simd_transpose_float(const float* src,
                                        float*       dst,
                                        size_t       src_rows,
                                        size_t       src_cols) {
    size_t i = 0u;
    size_t j = 0u;
 
    size_t row_body = (src_rows / 8u) * 8u;
    size_t col_body = (src_cols / 8u) * 8u;
 
    /* ---- Tiled 8×8 body ---- */
    for (i = 0u; i < row_body; i += 8u) {
        for (j = 0u; j < col_body; j += 8u) {
            __m256 r[8], o[8];
 
            for (size_t k = 0u; k < 8u; ++k) {
                r[k] = _mm256_loadu_ps(src + (i + k) * src_cols + j);
            }
 
            _avx2_transpose_8x8(r, o);
 
            for (size_t k = 0u; k < 8u; ++k) {
                _mm256_storeu_ps(dst + (j + k) * src_rows + i, o[k]);
            }
        }
    }
 
    /* ---- Remainder columns ---- */
    for (i = 0u; i < row_body; i += 8u) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t k = 0u; k < 8u; ++k) {
                dst[j * src_rows + (i + k)] = src[(i + k) * src_cols + j];
            }
        }
    }
 
    /* ---- Remainder rows ---- */
    for (i = row_body; i < src_rows; ++i) {
        for (j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
 
    _mm256_zeroupper();
}
// -------------------------------------------------------------------------------- 

static inline bool simd_equal_float(const float* a,
                                    const float* b,
                                    size_t       count) {
    size_t i = 0u;
 
    /* ---- Body: 32 floats (4 × 256-bit) per iteration ---- */
    size_t body_end = (count / 32u) * 32u;
    for (; i < body_end; i += 32u) {
        __m256i x0 = _mm256_xor_si256(
            _mm256_loadu_si256((const __m256i*)(a + i)),
            _mm256_loadu_si256((const __m256i*)(b + i)));
        __m256i x1 = _mm256_xor_si256(
            _mm256_loadu_si256((const __m256i*)(a + i + 8u)),
            _mm256_loadu_si256((const __m256i*)(b + i + 8u)));
        __m256i x2 = _mm256_xor_si256(
            _mm256_loadu_si256((const __m256i*)(a + i + 16u)),
            _mm256_loadu_si256((const __m256i*)(b + i + 16u)));
        __m256i x3 = _mm256_xor_si256(
            _mm256_loadu_si256((const __m256i*)(a + i + 24u)),
            _mm256_loadu_si256((const __m256i*)(b + i + 24u)));
 
        /* OR all XOR results — any nonzero bit means mismatch */
        __m256i any = _mm256_or_si256(_mm256_or_si256(x0, x1),
                                      _mm256_or_si256(x2, x3));
 
        /* _mm256_testz_si256(v, v) returns 1 iff v is all-zero */
        if (!_mm256_testz_si256(any, any)) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 8u) * 8u;
    for (; i < vec_end; i += 8u) {
        __m256i x = _mm256_xor_si256(
            _mm256_loadu_si256((const __m256i*)(a + i)),
            _mm256_loadu_si256((const __m256i*)(b + i)));
 
        if (!_mm256_testz_si256(x, x)) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    _mm256_zeroupper();
 
    /* ---- Scalar tail ---- */
    for (; i < count; ++i) {
        uint32_t va, vb;
        memcpy(&va, a + i, sizeof(uint32_t));
        memcpy(&vb, b + i, sizeof(uint32_t));
        if (va != vb) return false;
    }
 
    return true;
}
// -------------------------------------------------------------------------------- 

static inline size_t simd_count_nonzero_float(const float* data,
                                              size_t       count) {
    size_t nnz = 0u;
    size_t i = 0u;
 
    __m256 zero = _mm256_setzero_ps();
 
    /* ---- Body: 32 floats per iteration ---- */
    size_t body_end = (count / 32u) * 32u;
    for (; i < body_end; i += 32u) {
        __m256 v0 = _mm256_loadu_ps(data + i);
        __m256 v1 = _mm256_loadu_ps(data + i + 8u);
        __m256 v2 = _mm256_loadu_ps(data + i + 16u);
        __m256 v3 = _mm256_loadu_ps(data + i + 24u);
 
        /* Fast all-zero check with full 256-bit integer testz */
        __m256i any = _mm256_or_si256(
            _mm256_or_si256(_mm256_castps_si256(v0),
                            _mm256_castps_si256(v1)),
            _mm256_or_si256(_mm256_castps_si256(v2),
                            _mm256_castps_si256(v3)));
 
        if (_mm256_testz_si256(any, any)) continue;
 
        int m0 = _mm256_movemask_ps(_mm256_cmp_ps(v0, zero, _CMP_NEQ_UQ));
        int m1 = _mm256_movemask_ps(_mm256_cmp_ps(v1, zero, _CMP_NEQ_UQ));
        int m2 = _mm256_movemask_ps(_mm256_cmp_ps(v2, zero, _CMP_NEQ_UQ));
        int m3 = _mm256_movemask_ps(_mm256_cmp_ps(v3, zero, _CMP_NEQ_UQ));
 
        nnz += (size_t)__builtin_popcount(
            (unsigned)(m0 | (m1 << 8) | (m2 << 16) | (m3 << 24)));
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 8u) * 8u;
    for (; i < vec_end; i += 8u) {
        __m256 v = _mm256_loadu_ps(data + i);
 
        int m = _mm256_movemask_ps(_mm256_cmp_ps(v, zero, _CMP_NEQ_UQ));
        nnz += (size_t)__builtin_popcount((unsigned)m);
    }
 
    _mm256_zeroupper();
 
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
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_AVX2_FLOAT_INL */ 
// ================================================================================ 
// ================================================================================ 
// eof
