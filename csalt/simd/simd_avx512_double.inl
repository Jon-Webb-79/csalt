#ifndef SIMD_AVX512_DOUBLE_INL
#define SIMD_AVX512_DOUBLE_INL

/**
 * @file simd_avx512_double.inl
 * @brief AVX-512 implementation of tolerance-aware double search.
 *
 * Processes 8 doubles per iteration using 512-bit ZMM registers.
 *
 * Exact mode: _mm512_cmpeq_epi64_mask returns an 8-bit mask directly.
 * Tolerance mode: _mm512_cmp_pd_mask(_CMP_LE_OQ) returns an 8-bit mask.
 *
 * Requires: -mavx512f -mavx512bw
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


static inline __m512d _avx512_fabs_pd(__m512d v) {
    __m512d sign_mask = _mm512_set1_pd(-0.0);
    return _mm512_andnot_pd(sign_mask, v);
}

static inline __mmask8 _avx512_exact_mask_d(__m512d chunk, __m512d vn) {
    __m512i ci = _mm512_castpd_si512(chunk);
    __m512i ni = _mm512_castpd_si512(vn);
    return _mm512_cmpeq_epi64_mask(ci, ni);
}

static inline __mmask8 _avx512_tol_mask_d(__m512d chunk, __m512d vn,
                                            __m512d vabs_tol, __m512d vrel_tol,
                                            __m512d vabs_needle) {
    __m512d diff       = _avx512_fabs_pd(_mm512_sub_pd(chunk, vn));
    __m512d abs_chunk  = _avx512_fabs_pd(chunk);
    __m512d rel_base   = _mm512_max_pd(abs_chunk, vabs_needle);
    __m512d rel_thresh = _mm512_mul_pd(vrel_tol, rel_base);
    __m512d threshold  = _mm512_max_pd(vabs_tol, rel_thresh);
    return _mm512_cmp_pd_mask(diff, threshold, _CMP_LE_OQ);
}

static size_t simd_contains_double(const double* data,
                                    size_t        start,
                                    size_t        end,
                                    double        needle,
                                    double        abs_tol,
                                    double        rel_tol) {
    size_t  i  = start;
    __m512d vn = _mm512_set1_pd(needle);

    if (abs_tol == 0.0 && rel_tol == 0.0) {
        while (i + 8u <= end) {
            __m512d  chunk = _mm512_loadu_pd(data + i);
            __mmask8 mask  = _avx512_exact_mask_d(chunk, vn);
            if (mask != 0) return i + (size_t)__builtin_ctz((unsigned)mask);
            i += 8u;
        }
    } else {
        __m512d vabs_tol    = _mm512_set1_pd(abs_tol);
        __m512d vrel_tol    = _mm512_set1_pd(rel_tol);
        __m512d vabs_needle = _mm512_set1_pd(fabs(needle));
        while (i + 8u <= end) {
            __m512d  chunk = _mm512_loadu_pd(data + i);
            __mmask8 mask  = _avx512_tol_mask_d(chunk, vn, vabs_tol, vrel_tol,
                                                 vabs_needle);
            if (mask != 0) return i + (size_t)__builtin_ctz((unsigned)mask);
            i += 8u;
        }
    }

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
    __m512d vn         = _mm512_set1_pd(needle);
    __m512d vabs_tol   = _mm512_set1_pd(abs_tol);
    __m512d vrel_tol   = _mm512_set1_pd(rel_tol);
    __m512d vabs_ndl   = _mm512_set1_pd(abs_needle);
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
    while (L >= 8u) {
        size_t   base  = L - 8u;
        __m512d  chunk = _mm512_loadu_pd(data + base);
        __mmask8 mask  = _avx512_tol_mask_d(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
        if (mask == 0xFFu) { best = base; L = base; continue; }
        /* Scan high→low so a miss stops the walk without skipping higher matches. */
        for (int _e = 7; _e >= 0; _e--) {
            if ((mask >> (unsigned)_e) & 1u) { best = base + (size_t)_e; }
            else                             { L = base + (size_t)_e + 1u; goto left_done_avx512_d; }
        }
        L = base;
    }
    left_done_avx512_d:
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
        while (R + 8u <= len) {
            __m512d  chunk = _mm512_loadu_pd(data + R);
            __mmask8 mask  = _avx512_tol_mask_d(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
            if (mask == 0) break;
            for (size_t e = 0; e < 8u && R + e < len; e++) {
                if ((mask >> (unsigned)e) & 1u) { if (best == SIZE_MAX) best = R + e; }
                else                            { goto right_done_avx512_d; }
            }
            R += 8u;
        }
        right_done_avx512_d:
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
    __m512d v = _mm512_set1_pd(value);
    size_t i = 0u;
 
    while (i < count && ((uintptr_t)(data + i) & 63u) != 0u) {
        data[i] = value;
        ++i;
    }
 
    size_t aligned_end = i + ((count - i) / 32u) * 32u;
    for (; i < aligned_end; i += 32u) {
        _mm512_store_pd(data + i,        v);
        _mm512_store_pd(data + i + 8u,   v);
        _mm512_store_pd(data + i + 16u,  v);
        _mm512_store_pd(data + i + 24u,  v);
    }
 
    aligned_end = i + ((count - i) / 8u) * 8u;
    for (; i < aligned_end; i += 8u) {
        _mm512_store_pd(data + i, v);
    }
 
    if (i < count) {
        __mmask8 mask = (__mmask8)((1u << (count - i)) - 1u);
        _mm512_mask_storeu_pd(data + i, mask, v);
    }
}
// -------------------------------------------------------------------------------- 
 
/*
 * 8×8 double transpose using AVX-512.
 *
 * Each __m512d holds 8 doubles in four 128-bit lanes.
 * Phase 1: unpacklo/hi interleave pairs within lanes.
 * Phase 2: shuffle_f64x2 to rearrange 128-bit lane pairs.
 * Phase 3: shuffle_f64x2 for final cross-lane assembly.
 */
static inline void _avx512_transpose_8x8d(const __m512d r[8], __m512d o[8]) {
    /* Phase 1: interleave pairs within 128-bit lanes */
    __m512d t0 = _mm512_unpacklo_pd(r[0], r[1]);
    __m512d t1 = _mm512_unpackhi_pd(r[0], r[1]);
    __m512d t2 = _mm512_unpacklo_pd(r[2], r[3]);
    __m512d t3 = _mm512_unpackhi_pd(r[2], r[3]);
    __m512d t4 = _mm512_unpacklo_pd(r[4], r[5]);
    __m512d t5 = _mm512_unpackhi_pd(r[4], r[5]);
    __m512d t6 = _mm512_unpacklo_pd(r[6], r[7]);
    __m512d t7 = _mm512_unpackhi_pd(r[6], r[7]);
 
    /* Phase 2: shuffle 128-bit lane pairs */
    __m512d u0 = _mm512_shuffle_f64x2(t0, t2, 0x88);
    __m512d u1 = _mm512_shuffle_f64x2(t1, t3, 0x88);
    __m512d u2 = _mm512_shuffle_f64x2(t0, t2, 0xDD);
    __m512d u3 = _mm512_shuffle_f64x2(t1, t3, 0xDD);
    __m512d u4 = _mm512_shuffle_f64x2(t4, t6, 0x88);
    __m512d u5 = _mm512_shuffle_f64x2(t5, t7, 0x88);
    __m512d u6 = _mm512_shuffle_f64x2(t4, t6, 0xDD);
    __m512d u7 = _mm512_shuffle_f64x2(t5, t7, 0xDD);
 
    /* Phase 3: final cross-quarter assembly */
    o[0] = _mm512_shuffle_f64x2(u0, u4, 0x88);
    o[1] = _mm512_shuffle_f64x2(u1, u5, 0x88);
    o[2] = _mm512_shuffle_f64x2(u2, u6, 0x88);
    o[3] = _mm512_shuffle_f64x2(u3, u7, 0x88);
    o[4] = _mm512_shuffle_f64x2(u0, u4, 0xDD);
    o[5] = _mm512_shuffle_f64x2(u1, u5, 0xDD);
    o[6] = _mm512_shuffle_f64x2(u2, u6, 0xDD);
    o[7] = _mm512_shuffle_f64x2(u3, u7, 0xDD);
}
 
static inline void simd_transpose_double(const double* src,
                                         double*       dst,
                                         size_t        src_rows,
                                         size_t        src_cols) {
    size_t i = 0u;
    size_t j = 0u;
 
    size_t row_body = (src_rows / 8u) * 8u;
    size_t col_body = (src_cols / 8u) * 8u;
 
    for (i = 0u; i < row_body; i += 8u) {
        for (j = 0u; j < col_body; j += 8u) {
            __m512d r[8], o[8];
            for (size_t k = 0u; k < 8u; ++k) {
                r[k] = _mm512_loadu_pd(src + (i + k) * src_cols + j);
            }
            _avx512_transpose_8x8d(r, o);
            for (size_t k = 0u; k < 8u; ++k) {
                _mm512_storeu_pd(dst + (j + k) * src_rows + i, o[k]);
            }
        }
    }
 
    for (i = 0u; i < row_body; i += 8u) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t k = 0u; k < 8u; ++k) {
                dst[j * src_rows + (i + k)] = src[(i + k) * src_cols + j];
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
    size_t vec_end = (count / 8u) * 8u;

    for (; i < vec_end; i += 8u) {
        __m512d av = _mm512_loadu_pd(a + i);
        __m512d bv = _mm512_loadu_pd(b + i);

        __mmask8 mask = _mm512_cmp_pd_mask(av, bv, _CMP_EQ_OQ);
        if (mask != 0xFFu) {
            return false;
        }
    }

    for (; i < count; ++i) {
        if (!(a[i] == b[i])) {
            return false;
        }
    }

    return true;
}// -------------------------------------------------------------------------------- 
 
static inline size_t simd_count_nonzero_double(const double* data,
                                               size_t        count) {
    size_t nnz = 0u;
    size_t i = 0u;
    __m512d zero = _mm512_setzero_pd();
 
    size_t body_end = (count / 32u) * 32u;
    for (; i < body_end; i += 32u) {
        __mmask8 m0 = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i),        zero, _CMP_NEQ_UQ);
        __mmask8 m1 = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i + 8u),   zero, _CMP_NEQ_UQ);
        __mmask8 m2 = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i + 16u),  zero, _CMP_NEQ_UQ);
        __mmask8 m3 = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i + 24u),  zero, _CMP_NEQ_UQ);
 
        nnz += (size_t)(__builtin_popcount((unsigned)m0) +
                        __builtin_popcount((unsigned)m1) +
                        __builtin_popcount((unsigned)m2) +
                        __builtin_popcount((unsigned)m3));
    }
 
    size_t vec_end = i + ((count - i) / 8u) * 8u;
    for (; i < vec_end; i += 8u) {
        __mmask8 m = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i), zero, _CMP_NEQ_UQ);
        nnz += (size_t)__builtin_popcount((unsigned)m);
    }
 
    if (i < count) {
        __mmask8 tail = (__mmask8)((1u << (count - i)) - 1u);
        __m512d v = _mm512_maskz_loadu_pd(tail, data + i);
        __mmask8 m = _mm512_cmp_pd_mask(v, zero, _CMP_NEQ_UQ);
        nnz += (size_t)__builtin_popcount((unsigned)(m & tail));
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
    __m512d zero = _mm512_setzero_pd();
 
    /* Column index base: [0, 1, 2, ..., 7] */
    __m256i idx_base = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
 
    size_t vec_end = (cols / 8u) * 8u;
    for (; j < vec_end; j += 8u) {
        __m512d v = _mm512_loadu_pd(row_data + j);
        __mmask8 mask = _mm512_cmp_pd_mask(v, zero, _CMP_NEQ_UQ);
 
        if (mask == 0u) continue;
 
        int popcnt = __builtin_popcount((unsigned)mask);
 
        /* Native compress-store for double values */
        _mm512_mask_compressstoreu_pd(values + k, mask, v);
 
        /* Build column indices as 32-bit, compress, widen to size_t */
        __m256i col_vec = _mm256_add_epi32(
            idx_base, _mm256_set1_epi32((int)(col_offset + j)));
 
        int32_t col_tmp[8];
        _mm256_maskstore_epi32(col_tmp,
            _mm256_set1_epi32(-1),
            col_vec);
 
        /* Compress column indices manually — AVX-512 compress is for
           512-bit, but our index vector is 256-bit.  Scalar widen. */
        int written = 0;
        for (int b = 0; b < 8; ++b) {
            if (mask & (1u << b)) {
                col_idx[k + written] = (size_t)col_tmp[b];
                ++written;
            }
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
    __m512d zero = _mm512_setzero_pd();
 
    size_t body_end = (count / 32u) * 32u;
    for (; i < body_end; i += 32u) {
        __mmask8 m0 = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i),        zero, _CMP_NEQ_UQ);
        __mmask8 m1 = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i + 8u),   zero, _CMP_NEQ_UQ);
        __mmask8 m2 = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i + 16u),  zero, _CMP_NEQ_UQ);
        __mmask8 m3 = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i + 24u),  zero, _CMP_NEQ_UQ);
 
        if ((m0 | m1 | m2 | m3) != 0u) return false;
    }
 
    size_t vec_end = i + ((count - i) / 8u) * 8u;
    for (; i < vec_end; i += 8u) {
        __mmask8 m = _mm512_cmp_pd_mask(_mm512_loadu_pd(data + i), zero, _CMP_NEQ_UQ);
        if (m != 0u) return false;
    }
 
    if (i < count) {
        __mmask8 tail = (__mmask8)((1u << (count - i)) - 1u);
        __m512d v = _mm512_maskz_loadu_pd(tail, data + i);
        __mmask8 m = _mm512_cmp_pd_mask(v, zero, _CMP_NEQ_UQ);
        if ((m & tail) != 0u) return false;
    }
 
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_AVX512_DOUBLE_INL */
