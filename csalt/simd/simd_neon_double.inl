#ifndef SIMD_NEON_DOUBLE_INL
#define SIMD_NEON_DOUBLE_INL

/**
 * @file simd_neon_double.inl
 * @brief ARM NEON implementation of tolerance-aware double search.
 *
 * Processes 2 doubles per iteration using 128-bit NEON Q registers.
 * AArch64 only — double-precision NEON requires the 64-bit ARM ISA.
 *
 * Exact mode: vceqq_u64 on vreinterpretq_u64_f64 for bitwise equality.
 *   NaN != NaN, -0.0 != +0.0.
 * Tolerance mode: vabsq_f64, vsubq_f64, vcleq_f64, vmaxq_f64, vmulq_f64.
 *
 * Requires: AArch64 (implicit double-precision NEON)
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <arm_neon.h>

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
    size_t i = start;

    if (abs_tol == 0.0 && rel_tol == 0.0) {
        uint64_t nbits;
        memcpy(&nbits, &needle, 8);
        uint64x2_t vn = vdupq_n_u64(nbits);
        while (i + 2u <= end) {
            uint64x2_t chunk = vld1q_u64((const uint64_t*)(data + i));
            uint64x2_t cmp   = vceqq_u64(chunk, vn);
            uint64_t lo = vgetq_lane_u64(cmp, 0);
            uint64_t hi = vgetq_lane_u64(cmp, 1);
            if (lo && i     < end) return i;
            if (hi && i + 1 < end) return i + 1;
            i += 2u;
        }
    } else {
        float64x2_t vn          = vdupq_n_f64(needle);
        float64x2_t vabs_tol    = vdupq_n_f64(abs_tol);
        float64x2_t vrel_tol    = vdupq_n_f64(rel_tol);
        float64x2_t vabs_needle = vdupq_n_f64(fabs(needle));
        while (i + 2u <= end) {
            float64x2_t chunk     = vld1q_f64(data + i);
            float64x2_t diff      = vabsq_f64(vsubq_f64(chunk, vn));
            float64x2_t abs_chunk = vabsq_f64(chunk);
            float64x2_t rel_base  = vmaxq_f64(abs_chunk, vabs_needle);
            float64x2_t rel_thr   = vmulq_f64(vrel_tol, rel_base);
            float64x2_t threshold = vmaxq_f64(vabs_tol, rel_thr);
            uint64x2_t  cmp       = vcleq_f64(diff, threshold);
            uint64_t lo = vgetq_lane_u64(cmp, 0);
            uint64_t hi = vgetq_lane_u64(cmp, 1);
            if (lo && i     < end) return i;
            if (hi && i + 1 < end) return i + 1;
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

    size_t best      = SIZE_MAX;
    double abs_needle = fabs(needle);
    float64x2_t vn        = vdupq_n_f64(needle);
    float64x2_t vabs_tol  = vdupq_n_f64(abs_tol);
    float64x2_t vrel_tol  = vdupq_n_f64(rel_tol);
    float64x2_t vabs_ndl  = vdupq_n_f64(abs_needle);
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
    while (L >= 2u) {
        size_t      base  = L - 2u;
        float64x2_t chunk = vld1q_f64(data + base);
        float64x2_t diff  = vabsq_f64(vsubq_f64(chunk, vn));
        float64x2_t abs_c = vabsq_f64(chunk);
        float64x2_t thr   = vmaxq_f64(vabs_tol,
                                vmulq_f64(vrel_tol, vmaxq_f64(abs_c, vabs_ndl)));
        uint64x2_t  cmp   = vcleq_f64(diff, thr);
        uint64_t    lo    = vgetq_lane_u64(cmp, 0);
        uint64_t    hi    = vgetq_lane_u64(cmp, 1);
        /* All match? */
        if (lo && hi) { best = base; L = base; continue; }
        /* Scan high→low: lane 1 (hi) is index base+1, closer to mid. */
        if (hi) { best = base + 1u; }  /* lane 1 matches */
        if (!hi) { L = base + 1u; goto left_done_neon_d; } /* lane 1 misses → stop above it */
        /* hi matched; lo (lane 0) misses → stop at base+1 */
        L = base + 1u; goto left_done_neon_d;
        L = base;
    }
    left_done_neon_d:
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
        while (R + 2u <= len) {
            float64x2_t chunk = vld1q_f64(data + R);
            float64x2_t diff  = vabsq_f64(vsubq_f64(chunk, vn));
            float64x2_t abs_c = vabsq_f64(chunk);
            float64x2_t thr   = vmaxq_f64(vabs_tol,
                                    vmulq_f64(vrel_tol, vmaxq_f64(abs_c, vabs_ndl)));
            uint64x2_t  cmp   = vcleq_f64(diff, thr);
            uint64_t    lo    = vgetq_lane_u64(cmp, 0);
            uint64_t    hi    = vgetq_lane_u64(cmp, 1);
            if (!lo) break;
            if (best == SIZE_MAX) best = R;
            if (!hi) goto right_done_neon_d;
            R += 2u;
        }
        right_done_neon_d:
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
    float64x2_t v = vdupq_n_f64(value);
    size_t i = 0u;
 
    while (i < count && ((uintptr_t)(data + i) & 15u) != 0u) {
        data[i] = value;
        ++i;
    }
 
    size_t aligned_end = i + ((count - i) / 8u) * 8u;
    for (; i < aligned_end; i += 8u) {
        vst1q_f64(data + i,       v);
        vst1q_f64(data + i + 2u,  v);
        vst1q_f64(data + i + 4u,  v);
        vst1q_f64(data + i + 6u,  v);
    }
 
    aligned_end = i + ((count - i) / 2u) * 2u;
    for (; i < aligned_end; i += 2u) {
        vst1q_f64(data + i, v);
    }
 
    for (; i < count; ++i) {
        data[i] = value;
    }
}
// -------------------------------------------------------------------------------- 
 
/*
 * 2×2 double transpose using NEON: just unzip the two pairs.
 * r0 = [a0 a1],  r1 = [b0 b1]
 * o0 = [a0 b0],  o1 = [a1 b1]
 */
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
            float64x2_t r0 = vld1q_f64(src + (i + 0u) * src_cols + j);
            float64x2_t r1 = vld1q_f64(src + (i + 1u) * src_cols + j);
 
            /* vtrn for f64 produces the 2×2 transpose directly */
            float64x2_t o0 = vzip1q_f64(r0, r1);   /* [a0 b0] */
            float64x2_t o1 = vzip2q_f64(r0, r1);   /* [a1 b1] */
 
            vst1q_f64(dst + (j + 0u) * src_rows + i, o0);
            vst1q_f64(dst + (j + 1u) * src_rows + i, o1);
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
        uint64x2_t x0 = veorq_u64(
            vreinterpretq_u64_f64(vld1q_f64(a + i)),
            vreinterpretq_u64_f64(vld1q_f64(b + i)));
        uint64x2_t x1 = veorq_u64(
            vreinterpretq_u64_f64(vld1q_f64(a + i + 2u)),
            vreinterpretq_u64_f64(vld1q_f64(b + i + 2u)));
        uint64x2_t x2 = veorq_u64(
            vreinterpretq_u64_f64(vld1q_f64(a + i + 4u)),
            vreinterpretq_u64_f64(vld1q_f64(b + i + 4u)));
        uint64x2_t x3 = veorq_u64(
            vreinterpretq_u64_f64(vld1q_f64(a + i + 6u)),
            vreinterpretq_u64_f64(vld1q_f64(b + i + 6u)));
 
        uint64x2_t any = vorrq_u64(vorrq_u64(x0, x1),
                                   vorrq_u64(x2, x3));
 
        if (vmaxvq_u32(vreinterpretq_u32_u64(any)) != 0u) return false;
    }
 
    size_t vec_end = i + ((count - i) / 2u) * 2u;
    for (; i < vec_end; i += 2u) {
        uint64x2_t x = veorq_u64(
            vreinterpretq_u64_f64(vld1q_f64(a + i)),
            vreinterpretq_u64_f64(vld1q_f64(b + i)));
 
        if (vmaxvq_u32(vreinterpretq_u32_u64(x)) != 0u) return false;
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
    float64x2_t zero = vdupq_n_f64(0.0);
 
    size_t body_end = (count / 8u) * 8u;
    for (; i < body_end; i += 8u) {
        float64x2_t v0 = vld1q_f64(data + i);
        float64x2_t v1 = vld1q_f64(data + i + 2u);
        float64x2_t v2 = vld1q_f64(data + i + 4u);
        float64x2_t v3 = vld1q_f64(data + i + 6u);
 
        uint64x2_t any = vorrq_u64(
            vorrq_u64(vreinterpretq_u64_f64(v0), vreinterpretq_u64_f64(v1)),
            vorrq_u64(vreinterpretq_u64_f64(v2), vreinterpretq_u64_f64(v3)));
 
        if (vmaxvq_u32(vreinterpretq_u32_u64(any)) == 0u) continue;
 
        /* Count nonzero lanes: invert equality mask, shift to 0/1, sum */
        uint64x2_t nz0 = vshrq_n_u64(vmvnq_u8(vreinterpretq_u8_u64(vceqq_f64(v0, zero))), 63);
        uint64x2_t nz1 = vshrq_n_u64(vmvnq_u8(vreinterpretq_u8_u64(vceqq_f64(v1, zero))), 63);
        uint64x2_t nz2 = vshrq_n_u64(vmvnq_u8(vreinterpretq_u8_u64(vceqq_f64(v2, zero))), 63);
        uint64x2_t nz3 = vshrq_n_u64(vmvnq_u8(vreinterpretq_u8_u64(vceqq_f64(v3, zero))), 63);
 
        uint64x2_t sum = vaddq_u64(vaddq_u64(nz0, nz1),
                                   vaddq_u64(nz2, nz3));
        nnz += (size_t)(vgetq_lane_u64(sum, 0) + vgetq_lane_u64(sum, 1));
    }
 
    size_t vec_end = i + ((count - i) / 2u) * 2u;
    for (; i < vec_end; i += 2u) {
        float64x2_t v = vld1q_f64(data + i);
        uint64x2_t nz = vshrq_n_u64(
            vmvnq_u8(vreinterpretq_u8_u64(vceqq_f64(v, zero))), 63);
        nnz += (size_t)(vgetq_lane_u64(nz, 0) + vgetq_lane_u64(nz, 1));
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
    float64x2_t zero = vdupq_n_f64(0.0);
 
    size_t body_end = (count / 8u) * 8u;
    for (; i < body_end; i += 8u) {
        uint64x2_t nz0 = vmvnq_u8(vreinterpretq_u8_u64(vceqq_f64(vld1q_f64(data + i),       zero)));
        uint64x2_t nz1 = vmvnq_u8(vreinterpretq_u8_u64(vceqq_f64(vld1q_f64(data + i + 2u),  zero)));
        uint64x2_t nz2 = vmvnq_u8(vreinterpretq_u8_u64(vceqq_f64(vld1q_f64(data + i + 4u),  zero)));
        uint64x2_t nz3 = vmvnq_u8(vreinterpretq_u8_u64(vceqq_f64(vld1q_f64(data + i + 6u),  zero)));
 
        uint64x2_t any = vorrq_u64(vorrq_u64(
            vreinterpretq_u64_u8(nz0), vreinterpretq_u64_u8(nz1)),
            vorrq_u64(vreinterpretq_u64_u8(nz2), vreinterpretq_u64_u8(nz3)));
 
        if (vmaxvq_u32(vreinterpretq_u32_u64(any)) != 0u) return false;
    }
 
    size_t vec_end = i + ((count - i) / 2u) * 2u;
    for (; i < vec_end; i += 2u) {
        uint64x2_t nz = vmvnq_u8(vreinterpretq_u8_u64(
            vceqq_f64(vld1q_f64(data + i), zero)));
 
        if (vmaxvq_u32(vreinterpretq_u32_u64(
            vreinterpretq_u64_u8(nz))) != 0u) return false;
    }
 
    for (; i < count; ++i) {
        if (data[i] != 0.0) return false;
    }
 
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_NEON_DOUBLE_INL */
