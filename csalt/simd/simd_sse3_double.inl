#ifndef SIMD_SSE3_DOUBLE_INL
#define SIMD_SSE3_DOUBLE_INL

/**
 * @file simd_sse3_double.inl
 * @brief SSE3 implementation of tolerance-aware double search.
 *
 * Processes 2 doubles per iteration using 128-bit XMM registers.
 *
 * Exact mode: _mm_cmpeq_epi32 with AND of high/low halves (emulates 64-bit compare)
 * Tolerance mode: fabs + fmax/fmul/cmple, same formula as scalar.
 *
 * Requires: -msse2
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <pmmintrin.h   /* SSE3 */>

static inline bool _biteq_double(double a, double b) {
    uint64_t ua, ub;
    memcpy(&ua, &a, sizeof ua);
    memcpy(&ub, &b, sizeof ub);
    return ua == ub;
}


// ================================================================================
// Internal helpers
// ================================================================================

static inline __m128d _sse3_fabs_pd(__m128d v) {
    /* Clear sign bit of each 64-bit lane. */
    __m128i mask = _mm_set_epi32(0x7FFFFFFF, 0xFFFFFFFF,
                                  0x7FFFFFFF, 0xFFFFFFFF);
    return _mm_and_pd(v, _mm_castsi128_pd(mask));
}

/* SSE2/3: emulate 64-bit integer compare using two 32-bit comparisons. */
static inline int _sse3_exact_mask_d(__m128d chunk, __m128d vn) {
    __m128i ci  = _mm_castpd_si128(chunk);
    __m128i ni  = _mm_castpd_si128(vn);
    /* Compare all 32-bit lanes and AND adjacent pairs for 64-bit equality */
    __m128i cmp32 = _mm_cmpeq_epi32(ci, ni);
    /* Swap halves of each 64-bit lane and AND */
    __m128i swap  = _mm_shuffle_epi32(cmp32, _MM_SHUFFLE(2, 3, 0, 1));
    __m128i cmp64 = _mm_and_si128(cmp32, swap);
    return _mm_movemask_pd(_mm_castsi128_pd(cmp64));
}

static inline int _sse3_tol_mask_d(__m128d chunk, __m128d vn,
                                        __m128d vabs_tol, __m128d vrel_tol,
                                        __m128d vabs_needle) {
    __m128d diff       = _sse3_fabs_pd(_mm_sub_pd(chunk, vn));
    __m128d abs_chunk  = _sse3_fabs_pd(chunk);
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
            __m128d chunk = _mm_castsi128_pd(_mm_lddqu_si128((__m128i*)(data + i)));
            int     mask  = _sse3_exact_mask_d(chunk, vn);
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
            __m128d chunk = _mm_castsi128_pd(_mm_lddqu_si128((__m128i*)(data + i)));
            int     mask  = _sse3_tol_mask_d(chunk, vn, vabs_tol, vrel_tol,
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
        __m128d chunk = _mm_castsi128_pd(_mm_lddqu_si128((__m128i*)(data + base)));
        int     mask  = _sse3_tol_mask_d(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
        if (mask == 0x3) { best = base; L = base; continue; }
        /* Scan high→low so a miss stops the walk without skipping higher matches. */
        for (int _e = (int)2u - 1; _e >= 0; _e--) {
            if ((mask >> _e) & 1) { best = base + (size_t)_e; }
            else                  { L = base + (size_t)_e + 1u; goto left_done_sse3_d; }
        }
        L = base;
    }
    left_done_sse3_d:
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
            int     mask  = _sse3_tol_mask_d(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
            if (mask == 0) break;
            for (size_t e = 0; e < 2u && R + e < len; e++) {
                if ((mask >> (int)e) & 1) { if (best == SIZE_MAX) best = R + e; }
                else                      { goto right_done_sse3_d; }
            }
            R += 2u;
        }
        right_done_sse3_d:
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
 
            __m128d o0 = _mm_unpacklo_pd(r0, r1);
            __m128d o1 = _mm_unpackhi_pd(r0, r1);
 
            _mm_storeu_pd(dst + (j + 0u) * src_rows + i, o0);
            _mm_storeu_pd(dst + (j + 1u) * src_rows + i, o1);
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
 
static inline bool simd_double_arrays_equal(const double* a,
                                            const double* b,
                                            size_t        count) {
    if (a == NULL || b == NULL) return false;

    size_t i = 0u;
    size_t vec_end = (count / 2u) * 2u;

    for (; i < vec_end; i += 2u) {
        __m128d av = _mm_loadu_pd(a + i);
        __m128d bv = _mm_loadu_pd(b + i);
        __m128d cv = _mm_cmpeq_pd(av, bv);

        if (_mm_movemask_pd(cv) != 0x3) {
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
 
static inline size_t simd_count_nonzero_double(const double* data,
                                               size_t        count) {
    size_t nnz = 0u;
    size_t i = 0u;
    __m128d zero = _mm_setzero_pd();
 
    size_t body_end = (count / 8u) * 8u;
    for (; i < body_end; i += 8u) {
        int m0 = ~_mm_movemask_pd(_mm_cmpeq_pd(_mm_loadu_pd(data + i),       zero)) & 0x3;
        int m1 = ~_mm_movemask_pd(_mm_cmpeq_pd(_mm_loadu_pd(data + i + 2u),  zero)) & 0x3;
        int m2 = ~_mm_movemask_pd(_mm_cmpeq_pd(_mm_loadu_pd(data + i + 4u),  zero)) & 0x3;
        int m3 = ~_mm_movemask_pd(_mm_cmpeq_pd(_mm_loadu_pd(data + i + 6u),  zero)) & 0x3;
 
        nnz += (size_t)__builtin_popcount(
            (unsigned)(m0 | (m1 << 2) | (m2 << 4) | (m3 << 6)));
    }
 
    size_t vec_end = i + ((count - i) / 2u) * 2u;
    for (; i < vec_end; i += 2u) {
        int m = ~_mm_movemask_pd(_mm_cmpeq_pd(_mm_loadu_pd(data + i), zero)) & 0x3;
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
        int m0 = _mm_movemask_pd(_mm_cmpeq_pd(_mm_loadu_pd(data + i),       zero));
        int m1 = _mm_movemask_pd(_mm_cmpeq_pd(_mm_loadu_pd(data + i + 2u),  zero));
        int m2 = _mm_movemask_pd(_mm_cmpeq_pd(_mm_loadu_pd(data + i + 4u),  zero));
        int m3 = _mm_movemask_pd(_mm_cmpeq_pd(_mm_loadu_pd(data + i + 6u),  zero));
 
        if ((m0 & m1 & m2 & m3) != 0x3) return false;
    }
 
    size_t vec_end = i + ((count - i) / 2u) * 2u;
    for (; i < vec_end; i += 2u) {
        if (_mm_movemask_pd(_mm_cmpeq_pd(
                _mm_loadu_pd(data + i), zero)) != 0x3) return false;
    }
 
    for (; i < count; ++i) {
        if (data[i] != 0.0) return false;
    }
 
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_SSE3_DOUBLE_INL */
