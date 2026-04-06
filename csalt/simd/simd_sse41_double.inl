#ifndef SIMD_SSE41_DOUBLE_INL
#define SIMD_SSE41_DOUBLE_INL

/**
 * @file simd_sse41_double.inl
 * @brief SSE41 implementation of tolerance-aware double search.
 *
 * Processes 2 doubles per iteration using 128-bit XMM registers.
 *
 * Exact mode: _mm_cmpeq_epi64 (native SSE4.1)
 * Tolerance mode: fabs + fmax/fmul/cmple, same formula as scalar.
 *
 * Requires: -msse4.1
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <smmintrin.h>

static inline bool _biteq_double(double a, double b) {
    uint64_t ua, ub;
    memcpy(&ua, &a, sizeof ua);
    memcpy(&ub, &b, sizeof ub);
    return ua == ub;
}


// ================================================================================
// Internal helpers
// ================================================================================

static inline __m128d _sse41_fabs_pd(__m128d v) {
    /* Clear sign bit of each 64-bit lane using a 64-bit mask. */
    __m128i mask = _mm_set_epi64x(0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL);
    return _mm_and_pd(v, _mm_castsi128_pd(mask));
}

/* Exact bitwise match: reinterpret as int64, compare, return 2-bit mask. */
static inline int _sse41_exact_mask_d(__m128d chunk, __m128d vn) {
    __m128i ci  = _mm_castpd_si128(chunk);
    __m128i ni  = _mm_castpd_si128(vn);
    __m128i cmp = _mm_cmpeq_epi64(ci, ni);   /* SSE4.1 on sse41, emulated on sse2/3 */
    return _mm_movemask_pd(_mm_castsi128_pd(cmp));
}

static inline int _sse41_tol_mask_d(__m128d chunk, __m128d vn,
                                        __m128d vabs_tol, __m128d vrel_tol,
                                        __m128d vabs_needle) {
    __m128d diff       = _sse41_fabs_pd(_mm_sub_pd(chunk, vn));
    __m128d abs_chunk  = _sse41_fabs_pd(chunk);
    __m128d rel_base   = _mm_max_pd(abs_chunk, vabs_needle);
    __m128d rel_thresh = _mm_mul_pd(vrel_tol, rel_base);
    __m128d threshold  = _mm_max_pd(vabs_tol, rel_thresh);
    return _mm_movemask_pd(_mm_cmple_pd(diff, threshold));
}

// ================================================================================
// simd_contains_double
// ================================================================================

static size_t simd_contains_double(const double* data,
                                    size_t        start,
                                    size_t        end,
                                    double        needle,
                                    double        abs_tol,
                                    double        rel_tol) {
    size_t  i  = start;
    __m128d vn = _mm_set1_pd(needle);

    if (abs_tol == 0.0 && rel_tol == 0.0) {
        while (i + 2u <= end) {
            __m128d chunk = _mm_loadu_pd(data + i);
            int     mask  = _sse41_exact_mask_d(chunk, vn);
            if (mask != 0) {
                for (size_t e = 0; e < 2u && i + e < end; e++)
                    if ((mask >> (int)e) & 1) return i + e;
            }
            i += 2u;
        }
    } else {
        __m128d vabs_tol    = _mm_set1_pd(abs_tol);
        __m128d vrel_tol    = _mm_set1_pd(rel_tol);
        __m128d vabs_needle = _mm_set1_pd(fabs(needle));
        while (i + 2u <= end) {
            __m128d chunk = _mm_loadu_pd(data + i);
            int     mask  = _sse41_tol_mask_d(chunk, vn, vabs_tol, vrel_tol,
                                                  vabs_needle);
            if (mask != 0) {
                for (size_t e = 0; e < 2u && i + e < end; e++)
                    if ((mask >> (int)e) & 1) return i + e;
            }
            i += 2u;
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

    /* ── Tolerance fan-out ── */
    size_t  best       = SIZE_MAX;
    double  abs_needle = fabs(needle);
    __m128d vn         = _mm_set1_pd(needle);
    __m128d vabs_tol   = _mm_set1_pd(abs_tol);
    __m128d vrel_tol   = _mm_set1_pd(rel_tol);
    __m128d vabs_ndl   = _mm_set1_pd(abs_needle);

    /* Check landing */
    if (abs_tol == 0.0 && rel_tol == 0.0) {
        /* Exact mode: bitwise equality avoids NaN from (inf - inf). */
        if (_biteq_double(data[mid], needle)) best = mid;
    } else {
    {
        double diff = fabs(data[mid] - needle);
        double thr  = fmax(abs_tol, rel_tol * fmax(fabs(data[mid]), abs_needle));
        if (diff <= thr) best = mid;
    }
    }

    /* SIMD left walk — 2 doubles per step */
    size_t L = mid;
    while (L >= 2u) {
        size_t  base  = L - 2u;
        __m128d chunk = _mm_loadu_pd(data + base);
        int     mask  = _sse41_tol_mask_d(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
        if (mask == 0x3) { best = base; L = base; continue; }
        /* Scan high→low so a miss stops the walk without skipping higher matches. */
        for (int _e = (int)2u - 1; _e >= 0; _e--) {
            if ((mask >> _e) & 1) { best = base + (size_t)_e; }
            else                  { L = base + (size_t)_e + 1u; goto left_done_sse41_d; }
        }
        L = base;
    }
    left_done_sse41_d:
    if (abs_tol == 0.0 && rel_tol == 0.0) {
        while (L > 0u && _biteq_double(data[L - 1u], needle)) { best = --L; }
    } else {
    while (L > 0u) {
        double diff = fabs(data[L - 1u] - needle);
        double thr  = fmax(abs_tol, rel_tol * fmax(fabs(data[L - 1u]), abs_needle));
        if (diff <= thr) { best = --L; } else { break; }
    }
    }

    /* SIMD right walk */
    if (best == SIZE_MAX) {
        size_t R = mid + 1u;
        while (R + 2u <= len) {
            __m128d chunk = _mm_loadu_pd(data + R);
            int     mask  = _sse41_tol_mask_d(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
            if (mask == 0) break;
            for (size_t e = 0; e < 2u && R + e < len; e++) {
                if ((mask >> (int)e) & 1) { if (best == SIZE_MAX) best = R + e; }
                else                      { goto right_done_sse41_d; }
            }
            R += 2u;
        }
        right_done_sse41_d:
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
    __m128d v = _mm_set1_pd(value);
    size_t i = 0u;
 
    while (i < count && ((uintptr_t)(data + i) & 15u) != 0u) {
        data[i] = value;
        ++i;
    }
 
    size_t aligned_end = i + ((count - i) / 8u) * 8u;
    for (; i < aligned_end; i += 8u) {
        _mm_store_pd(data + i,       v);
        _mm_store_pd(data + i + 2u,  v);
        _mm_store_pd(data + i + 4u,  v);
        _mm_store_pd(data + i + 6u,  v);
    }
 
    aligned_end = i + ((count - i) / 2u) * 2u;
    for (; i < aligned_end; i += 2u) {
        _mm_store_pd(data + i, v);
    }
 
    for (; i < count; ++i) {
        data[i] = value;
    }
}
// -------------------------------------------------------------------------------- 
 
static inline void simd_transpose_double(const double* src,
                                         double*       dst,
                                         size_t        src_rows,
                                         size_t        src_cols) {
    size_t i = 0u;
    size_t j = 0u;
 
    size_t row_body = (src_rows / 2u) * 2u;
    size_t col_body = (src_cols / 2u) * 2u;
 
    for (i = 0u; i < row_body; i += 2u) {
        for (j = 0u; j < col_body; j += 2u) {
            __m128d r0 = _mm_loadu_pd(src + (i + 0u) * src_cols + j);
            __m128d r1 = _mm_loadu_pd(src + (i + 1u) * src_cols + j);
 
            _mm_storeu_pd(dst + (j + 0u) * src_rows + i,
                          _mm_unpacklo_pd(r0, r1));
            _mm_storeu_pd(dst + (j + 1u) * src_rows + i,
                          _mm_unpackhi_pd(r0, r1));
        }
    }
 
    for (i = 0u; i < row_body; i += 2u) {
        for (j = col_body; j < src_cols; ++j) {
            dst[j * src_rows + (i + 0u)] = src[(i + 0u) * src_cols + j];
            dst[j * src_rows + (i + 1u)] = src[(i + 1u) * src_cols + j];
        }
    }
 
    for (i = row_body; i < src_rows; ++i) {
        for (j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
}
// -------------------------------------------------------------------------------- 
 
static inline bool simd_equal_double(const double* a,
                                     const double* b,
                                     size_t        count) {
    size_t i = 0u;
 
    size_t body_end = (count / 8u) * 8u;
    for (; i < body_end; i += 8u) {
        __m128i x0 = _mm_xor_si128(
            _mm_loadu_si128((const __m128i*)(a + i)),
            _mm_loadu_si128((const __m128i*)(b + i)));
        __m128i x1 = _mm_xor_si128(
            _mm_loadu_si128((const __m128i*)(a + i + 2u)),
            _mm_loadu_si128((const __m128i*)(b + i + 2u)));
        __m128i x2 = _mm_xor_si128(
            _mm_loadu_si128((const __m128i*)(a + i + 4u)),
            _mm_loadu_si128((const __m128i*)(b + i + 4u)));
        __m128i x3 = _mm_xor_si128(
            _mm_loadu_si128((const __m128i*)(a + i + 6u)),
            _mm_loadu_si128((const __m128i*)(b + i + 6u)));
 
        __m128i any = _mm_or_si128(_mm_or_si128(x0, x1),
                                   _mm_or_si128(x2, x3));
 
        if (!_mm_testz_si128(any, any)) return false;
    }
 
    size_t vec_end = i + ((count - i) / 2u) * 2u;
    for (; i < vec_end; i += 2u) {
        __m128i x = _mm_xor_si128(
            _mm_loadu_si128((const __m128i*)(a + i)),
            _mm_loadu_si128((const __m128i*)(b + i)));
 
        if (!_mm_testz_si128(x, x)) return false;
    }
 
    for (; i < count; ++i) {
        uint64_t va, vb;
        memcpy(&va, a + i, sizeof(uint64_t));
        memcpy(&vb, b + i, sizeof(uint64_t));
        if (va != vb) return false;
    }
 
    return true;
}
// -------------------------------------------------------------------------------- 
 
static inline size_t simd_count_nonzero_double(const double* data,
                                               size_t        count) {
    size_t nnz = 0u;
    size_t i = 0u;
    __m128d zero = _mm_setzero_pd();
 
    size_t body_end = (count / 8u) * 8u;
    for (; i < body_end; i += 8u) {
        __m128d v0 = _mm_loadu_pd(data + i);
        __m128d v1 = _mm_loadu_pd(data + i + 2u);
        __m128d v2 = _mm_loadu_pd(data + i + 4u);
        __m128d v3 = _mm_loadu_pd(data + i + 6u);
 
        __m128d any = _mm_or_pd(_mm_or_pd(v0, v1), _mm_or_pd(v2, v3));
        __m128i any_i = _mm_castpd_si128(any);
        if (_mm_testz_si128(any_i, any_i)) continue;
 
        int m0 = ~_mm_movemask_pd(_mm_cmpeq_pd(v0, zero)) & 0x3;
        int m1 = ~_mm_movemask_pd(_mm_cmpeq_pd(v1, zero)) & 0x3;
        int m2 = ~_mm_movemask_pd(_mm_cmpeq_pd(v2, zero)) & 0x3;
        int m3 = ~_mm_movemask_pd(_mm_cmpeq_pd(v3, zero)) & 0x3;
 
        nnz += (size_t)__builtin_popcount(
            (unsigned)(m0 | (m1 << 2) | (m2 << 4) | (m3 << 6)));
    }
 
    size_t vec_end = i + ((count - i) / 2u) * 2u;
    for (; i < vec_end; i += 2u) {
        __m128d v = _mm_loadu_pd(data + i);
        __m128i vi = _mm_castpd_si128(v);
        if (_mm_testz_si128(vi, vi)) continue;
 
        int m = ~_mm_movemask_pd(_mm_cmpeq_pd(v, zero)) & 0x3;
        nnz += (size_t)__builtin_popcount((unsigned)m);
    }
 
    for (; i < count; ++i) {
        if (data[i] != 0.0) ++nnz;
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
    for (size_t j = 0u; j < cols; ++j) {
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
    __m128d zero = _mm_setzero_pd();
 
    size_t body_end = (count / 8u) * 8u;
    for (; i < body_end; i += 8u) {
        __m128d eq0 = _mm_cmpeq_pd(_mm_loadu_pd(data + i),       zero);
        __m128d eq1 = _mm_cmpeq_pd(_mm_loadu_pd(data + i + 2u),  zero);
        __m128d eq2 = _mm_cmpeq_pd(_mm_loadu_pd(data + i + 4u),  zero);
        __m128d eq3 = _mm_cmpeq_pd(_mm_loadu_pd(data + i + 6u),  zero);
 
        __m128d all_eq = _mm_and_pd(_mm_and_pd(eq0, eq1),
                                    _mm_and_pd(eq2, eq3));
 
        __m128i inv = _mm_andnot_si128(_mm_castpd_si128(all_eq),
                                       _mm_set1_epi64x(-1LL));
        if (!_mm_testz_si128(inv, inv)) return false;
    }
 
    size_t vec_end = i + ((count - i) / 2u) * 2u;
    for (; i < vec_end; i += 2u) {
        __m128d eq = _mm_cmpeq_pd(_mm_loadu_pd(data + i), zero);
        __m128i inv = _mm_andnot_si128(_mm_castpd_si128(eq),
                                       _mm_set1_epi64x(-1LL));
        if (!_mm_testz_si128(inv, inv)) return false;
    }
 
    for (; i < count; ++i) {
        if (data[i] != 0.0) return false;
    }
 
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_SSE41_DOUBLE_INL */
