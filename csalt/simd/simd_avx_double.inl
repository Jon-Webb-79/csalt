#ifndef SIMD_AVX_DOUBLE_INL
#define SIMD_AVX_DOUBLE_INL

/**
 * @file simd_avx_double.inl
 * @brief AVX implementation of tolerance-aware double search.
 *
 * Processes 4 doubles per iteration using 256-bit YMM registers.
 *
 * Exact mode: AVX has no 256-bit integer compare, so the register is split
 * into two 128-bit halves and each half compared with _mm_cmpeq_epi32
 * using the SSE2 emulated 64-bit compare approach.
 *
 * Tolerance mode: _mm256_cmp_pd(_CMP_LE_OQ).
 *
 * Requires: -mavx
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <immintrin.h>

static inline bool _biteq_double(double a, double b) {
    uint64_t ua, ub;
    memcpy(&ua, &a, sizeof ua);
    memcpy(&ub, &b, sizeof ub);
    return ua == ub;
}


static inline __m256d _avx_fabs_pd(__m256d v) {
    __m256d sign_mask = _mm256_set1_pd(-0.0);
    return _mm256_andnot_pd(sign_mask, v);
}

static inline int _avx_exact_mask_d(__m256d chunk, __m256d vn) {
    /*
     * Split into 128-bit halves, emulate 64-bit cmpeq with 32-bit pairs.
     * Returns a 4-bit mask (one bit per double lane).
     */
    __m128i lo_c  = _mm_castpd_si128(_mm256_castpd256_pd128(chunk));
    __m128i hi_c  = _mm_castpd_si128(_mm256_extractf128_pd(chunk, 1));
    __m128i lo_n  = _mm_castpd_si128(_mm256_castpd256_pd128(vn));
    __m128i hi_n  = _mm_castpd_si128(_mm256_extractf128_pd(vn, 1));

    /* Emulate 64-bit cmpeq using 32-bit cmpeq + AND of adjacent halves */
    __m128i clo32 = _mm_cmpeq_epi32(lo_c, lo_n);
    __m128i chi32 = _mm_cmpeq_epi32(hi_c, hi_n);
    __m128i slo   = _mm_shuffle_epi32(clo32, _MM_SHUFFLE(2, 3, 0, 1));
    __m128i shi   = _mm_shuffle_epi32(chi32, _MM_SHUFFLE(2, 3, 0, 1));
    __m128i clo64 = _mm_and_si128(clo32, slo);
    __m128i chi64 = _mm_and_si128(chi32, shi);

    int mlo = _mm_movemask_pd(_mm_castsi128_pd(clo64));   /* 2-bit */
    int mhi = _mm_movemask_pd(_mm_castsi128_pd(chi64));   /* 2-bit */
    return mlo | (mhi << 2);                               /* 4-bit */
}

static inline int _avx_tol_mask_d(__m256d chunk, __m256d vn,
                                   __m256d vabs_tol, __m256d vrel_tol,
                                   __m256d vabs_needle) {
    __m256d diff       = _avx_fabs_pd(_mm256_sub_pd(chunk, vn));
    __m256d abs_chunk  = _avx_fabs_pd(chunk);
    __m256d rel_base   = _mm256_max_pd(abs_chunk, vabs_needle);
    __m256d rel_thresh = _mm256_mul_pd(vrel_tol, rel_base);
    __m256d threshold  = _mm256_max_pd(vabs_tol, rel_thresh);
    return _mm256_movemask_pd(_mm256_cmp_pd(diff, threshold, _CMP_LE_OQ));
}

static size_t simd_contains_double(const double* data,
                                    size_t        start,
                                    size_t        end,
                                    double        needle,
                                    double        abs_tol,
                                    double        rel_tol) {
    size_t  i  = start;
    __m256d vn = _mm256_set1_pd(needle);

    if (abs_tol == 0.0 && rel_tol == 0.0) {
        while (i + 4u <= end) {
            __m256d chunk = _mm256_loadu_pd(data + i);
            int     mask  = _avx_exact_mask_d(chunk, vn);
            if (mask != 0) {
                for (size_t e = 0; e < 4u && i + e < end; e++)
                    if ((mask >> (int)e) & 1) { _mm256_zeroupper(); return i + e; }
            }
            i += 4u;
        }
    } else {
        __m256d vabs_tol    = _mm256_set1_pd(abs_tol);
        __m256d vrel_tol    = _mm256_set1_pd(rel_tol);
        __m256d vabs_needle = _mm256_set1_pd(fabs(needle));
        while (i + 4u <= end) {
            __m256d chunk = _mm256_loadu_pd(data + i);
            int     mask  = _avx_tol_mask_d(chunk, vn, vabs_tol, vrel_tol,
                                             vabs_needle);
            if (mask != 0) {
                for (size_t e = 0; e < 4u && i + e < end; e++)
                    if ((mask >> (int)e) & 1) { _mm256_zeroupper(); return i + e; }
            }
            i += 4u;
        }
    }
    _mm256_zeroupper();

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

    size_t  best       = SIZE_MAX;
    double  abs_needle = fabs(needle);
    __m256d vn         = _mm256_set1_pd(needle);
    __m256d vabs_tol   = _mm256_set1_pd(abs_tol);
    __m256d vrel_tol   = _mm256_set1_pd(rel_tol);
    __m256d vabs_ndl   = _mm256_set1_pd(abs_needle);
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

    size_t L = mid;
    while (L >= 4u) {
        size_t  base  = L - 4u;
        __m256d chunk = _mm256_loadu_pd(data + base);
        int     mask  = _avx_tol_mask_d(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
        if (mask == 0xF) { best = base; L = base; continue; }
        /* Scan high→low so a miss stops the walk without skipping higher matches. */
        for (int _e = (int)4u - 1; _e >= 0; _e--) {
            if ((mask >> _e) & 1) { best = base + (size_t)_e; }
            else                  { L = base + (size_t)_e + 1u; goto left_done_avx_d; }
        }
        L = base;
    }
    left_done_avx_d:
    _mm256_zeroupper();
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
        while (R + 4u <= len) {
            __m256d chunk = _mm256_loadu_pd(data + R);
            int     mask  = _avx_tol_mask_d(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
            if (mask == 0) break;
            for (size_t e = 0; e < 4u && R + e < len; e++) {
                if ((mask >> (int)e) & 1) { if (best == SIZE_MAX) best = R + e; }
                else                      { goto right_done_avx_d; }
            }
            R += 4u;
        }
        right_done_avx_d:
        _mm256_zeroupper();
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
    __m256d v = _mm256_set1_pd(value);
    size_t i = 0u;
 
    while (i < count && ((uintptr_t)(data + i) & 31u) != 0u) {
        data[i] = value;
        ++i;
    }
 
    size_t aligned_end = i + ((count - i) / 16u) * 16u;
    for (; i < aligned_end; i += 16u) {
        _mm256_store_pd(data + i,        v);
        _mm256_store_pd(data + i + 4u,   v);
        _mm256_store_pd(data + i + 8u,   v);
        _mm256_store_pd(data + i + 12u,  v);
    }
 
    aligned_end = i + ((count - i) / 4u) * 4u;
    for (; i < aligned_end; i += 4u) {
        _mm256_store_pd(data + i, v);
    }
 
    for (; i < count; ++i) {
        data[i] = value;
    }
 
    _mm256_zeroupper();
}
// -------------------------------------------------------------------------------- 
 
/*
 * 4×4 double transpose using AVX.
 *
 * Each __m256d holds 4 doubles as two 128-bit lanes [lo | hi].
 * unpacklo/hi interleave within lanes, then permute2f128 swaps lanes.
 */
static inline void _avx_transpose_4x4d(const __m256d r[4], __m256d o[4]) {
    __m256d t0 = _mm256_unpacklo_pd(r[0], r[1]);   /* [a0 b0 | a2 b2] */
    __m256d t1 = _mm256_unpackhi_pd(r[0], r[1]);   /* [a1 b1 | a3 b3] */
    __m256d t2 = _mm256_unpacklo_pd(r[2], r[3]);   /* [c0 d0 | c2 d2] */
    __m256d t3 = _mm256_unpackhi_pd(r[2], r[3]);   /* [c1 d1 | c3 d3] */
 
    o[0] = _mm256_permute2f128_pd(t0, t2, 0x20);   /* [a0 b0 c0 d0] */
    o[1] = _mm256_permute2f128_pd(t1, t3, 0x20);   /* [a1 b1 c1 d1] */
    o[2] = _mm256_permute2f128_pd(t0, t2, 0x31);   /* [a2 b2 c2 d2] */
    o[3] = _mm256_permute2f128_pd(t1, t3, 0x31);   /* [a3 b3 c3 d3] */
}
 
static inline void simd_transpose_double(const double* src,
                                         double*       dst,
                                         size_t        src_rows,
                                         size_t        src_cols) {
    size_t i = 0u;
    size_t j = 0u;
 
    size_t row_body = (src_rows / 4u) * 4u;
    size_t col_body = (src_cols / 4u) * 4u;
 
    for (i = 0u; i < row_body; i += 4u) {
        for (j = 0u; j < col_body; j += 4u) {
            __m256d r[4], o[4];
            for (size_t k = 0u; k < 4u; ++k) {
                r[k] = _mm256_loadu_pd(src + (i + k) * src_cols + j);
            }
            _avx_transpose_4x4d(r, o);
            for (size_t k = 0u; k < 4u; ++k) {
                _mm256_storeu_pd(dst + (j + k) * src_rows + i, o[k]);
            }
        }
    }
 
    for (i = 0u; i < row_body; i += 4u) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t k = 0u; k < 4u; ++k) {
                dst[j * src_rows + (i + k)] = src[(i + k) * src_cols + j];
            }
        }
    }
 
    for (i = row_body; i < src_rows; ++i) {
        for (j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
 
    _mm256_zeroupper();
}
// -------------------------------------------------------------------------------- 
 
static inline bool simd_equal_double(const double* a,
                                     const double* b,
                                     size_t        count) {
    size_t i = 0u;
 
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        __m256d x0 = _mm256_xor_pd(_mm256_loadu_pd(a + i),
                                    _mm256_loadu_pd(b + i));
        __m256d x1 = _mm256_xor_pd(_mm256_loadu_pd(a + i + 4u),
                                    _mm256_loadu_pd(b + i + 4u));
        __m256d x2 = _mm256_xor_pd(_mm256_loadu_pd(a + i + 8u),
                                    _mm256_loadu_pd(b + i + 8u));
        __m256d x3 = _mm256_xor_pd(_mm256_loadu_pd(a + i + 12u),
                                    _mm256_loadu_pd(b + i + 12u));
 
        __m256d any = _mm256_or_pd(_mm256_or_pd(x0, x1),
                                   _mm256_or_pd(x2, x3));
 
        __m128i lo = _mm_castpd_si128(_mm256_castpd256_pd128(any));
        __m128i hi = _mm_castpd_si128(_mm256_extractf128_pd(any, 1));
        __m128i combined = _mm_or_si128(lo, hi);
 
        if (_mm_movemask_epi8(
                _mm_cmpeq_epi8(combined, _mm_setzero_si128())) != 0xFFFF) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        __m256d x = _mm256_xor_pd(_mm256_loadu_pd(a + i),
                                   _mm256_loadu_pd(b + i));
 
        __m128i lo = _mm_castpd_si128(_mm256_castpd256_pd128(x));
        __m128i hi = _mm_castpd_si128(_mm256_extractf128_pd(x, 1));
        __m128i combined = _mm_or_si128(lo, hi);
 
        if (_mm_movemask_epi8(
                _mm_cmpeq_epi8(combined, _mm_setzero_si128())) != 0xFFFF) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    _mm256_zeroupper();
 
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
    __m256d zero = _mm256_setzero_pd();
 
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        int m0 = _mm256_movemask_pd(_mm256_cmp_pd(_mm256_loadu_pd(data + i),        zero, _CMP_NEQ_UQ));
        int m1 = _mm256_movemask_pd(_mm256_cmp_pd(_mm256_loadu_pd(data + i + 4u),   zero, _CMP_NEQ_UQ));
        int m2 = _mm256_movemask_pd(_mm256_cmp_pd(_mm256_loadu_pd(data + i + 8u),   zero, _CMP_NEQ_UQ));
        int m3 = _mm256_movemask_pd(_mm256_cmp_pd(_mm256_loadu_pd(data + i + 12u),  zero, _CMP_NEQ_UQ));
 
        nnz += (size_t)__builtin_popcount(
            (unsigned)(m0 | (m1 << 4) | (m2 << 8) | (m3 << 12)));
    }
 
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        int m = _mm256_movemask_pd(_mm256_cmp_pd(_mm256_loadu_pd(data + i), zero, _CMP_NEQ_UQ));
        nnz += (size_t)__builtin_popcount((unsigned)m);
    }
 
    _mm256_zeroupper();
 
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
    __m256d zero = _mm256_setzero_pd();
 
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        int m0 = _mm256_movemask_pd(_mm256_cmp_pd(_mm256_loadu_pd(data + i),        zero, _CMP_NEQ_UQ));
        int m1 = _mm256_movemask_pd(_mm256_cmp_pd(_mm256_loadu_pd(data + i + 4u),   zero, _CMP_NEQ_UQ));
        int m2 = _mm256_movemask_pd(_mm256_cmp_pd(_mm256_loadu_pd(data + i + 8u),   zero, _CMP_NEQ_UQ));
        int m3 = _mm256_movemask_pd(_mm256_cmp_pd(_mm256_loadu_pd(data + i + 12u),  zero, _CMP_NEQ_UQ));
 
        if ((m0 | m1 | m2 | m3) != 0) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        if (_mm256_movemask_pd(_mm256_cmp_pd(
                _mm256_loadu_pd(data + i), zero, _CMP_NEQ_UQ)) != 0) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    _mm256_zeroupper();
 
    for (; i < count; ++i) {
        if (data[i] != 0.0) return false;
    }
 
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_AVX_DOUBLE_INL */
