#ifndef SIMD_NEON_FLOAT_INL
#define SIMD_NEON_FLOAT_INL
// ================================================================================ 
// ================================================================================ 

/**
 * @file simd_neon_float.inl
 * @brief ARM NEON implementation of tolerance-aware float search.
 *
 * Processes 4 floats per iteration using 128-bit NEON Q registers.
 *
 * Exact mode (both tolerances == 0.0f):
 *   vceqq_u32 on the reinterpreted float register provides bitwise
 *   equality: NaN != NaN, -0.0f (0x80000000) != +0.0f.  vmaxvq_u32
 *   reduces the 4-lane comparison result to a scalar to check for any
 *   hit before scanning lanes with vgetq_lane_u32.
 *
 * Tolerance mode:
 *   vabsq_f32 computes |element|, vsubq_f32 + vabsq_f32 computes the
 *   absolute difference, vmaxq_f32 + vmulq_f32 builds the per-lane
 *   threshold, and vcleq_f32 (compare less-than-or-equal) produces the
 *   match predicate.  vmaxvq_u32 on the cast result detects any hit.
 *
 * Requires: -mfpu=neon (ARMv7) or implicit on AArch64
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <arm_neon.h>

// ================================================================================
// Public interface
// ================================================================================

static size_t simd_contains_float(const float* data,
                                   size_t       start,
                                   size_t       end,
                                   float        needle,
                                   float        abs_tol,
                                   float        rel_tol) {
    size_t i = start;

    if (abs_tol == 0.0f && rel_tol == 0.0f) {
        /*
         * Exact bitwise match. Reinterpret as uint32 so that
         * -0.0f and NaN are handled correctly.
         */
        uint32x4_t vn = vdupq_n_u32(*(const uint32_t*)(const void*)&needle);

        while (i + 4u <= end) {
            uint32x4_t chunk = vld1q_u32((const uint32_t*)(data + i));
            uint32x4_t cmp   = vceqq_u32(chunk, vn);
            if (vmaxvq_u32(cmp) != 0u) {
                uint32_t tmp[4];
                vst1q_u32(tmp, cmp);
                for (size_t e = 0; e < 4u && i + e < end; e++) {
                    if (tmp[e] != 0u) return i + e;
                }
            }
            i += 4u;
        }
    } else {
        float32x4_t vn          = vdupq_n_f32(needle);
        float32x4_t vabs_tol    = vdupq_n_f32(abs_tol);
        float32x4_t vrel_tol    = vdupq_n_f32(rel_tol);
        float32x4_t vabs_needle = vdupq_n_f32(fabsf(needle));

        while (i + 4u <= end) {
            float32x4_t chunk     = vld1q_f32(data + i);
            float32x4_t diff      = vabsq_f32(vsubq_f32(chunk, vn));
            float32x4_t abs_chunk = vabsq_f32(chunk);
            float32x4_t rel_base  = vmaxq_f32(abs_chunk, vabs_needle);
            float32x4_t rel_thresh = vmulq_f32(vrel_tol, rel_base);
            float32x4_t threshold  = vmaxq_f32(vabs_tol, rel_thresh);
            /* vcleq_f32: returns 0xFFFFFFFF per lane where diff <= threshold */
            uint32x4_t  cmp        = vcleq_f32(diff, threshold);
            if (vmaxvq_u32(cmp) != 0u) {
                uint32_t tmp[4];
                vst1q_u32(tmp, cmp);
                for (size_t e = 0; e < 4u && i + e < end; e++) {
                    if (tmp[e] != 0u) return i + e;
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
    size_t best      = SIZE_MAX;
    float  abs_needle = fabsf(needle);
    float32x4_t vn        = vdupq_n_f32(needle);
    float32x4_t vabs_tol  = vdupq_n_f32(abs_tol);
    float32x4_t vrel_tol  = vdupq_n_f32(rel_tol);
    float32x4_t vabs_ndl  = vdupq_n_f32(abs_needle);

    /* Check landing */
    {
        float diff = fabsf(data[mid] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[mid]), abs_needle));
        if (diff <= thr) best = mid;
    }

    /* SIMD left walk — 4 floats per step */
    size_t L = mid;
    while (L >= 4u) {
        size_t      base  = L - 4u;
        float32x4_t chunk = vld1q_f32(data + base);
        float32x4_t diff  = vabsq_f32(vsubq_f32(chunk, vn));
        float32x4_t abs_c = vabsq_f32(chunk);
        float32x4_t thr   = vmaxq_f32(vabs_tol, vmulq_f32(vrel_tol,
                                vmaxq_f32(abs_c, vabs_ndl)));
        uint32x4_t  cmp   = vcleq_f32(diff, thr);
        uint32_t    tmp[4];
        vst1q_u32(tmp, cmp);
        if (tmp[0] && tmp[1] && tmp[2] && tmp[3]) {
            best = base; L = base; continue;
        }
        for (size_t e = 0; e < 4u && base + e < L; e++) {
            if (tmp[e]) { best = base + e; }
            else        { L = base + e; goto left_done_neon; }
        }
        L = base;
    }
    left_done_neon:
    while (L > 0u) {
        float diff = fabsf(data[L - 1u] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[L - 1u]), abs_needle));
        if (diff <= thr) { best = --L; } else { break; }
    }

    /* SIMD right walk */
    if (best == SIZE_MAX) {
        size_t R = mid + 1u;
        while (R + 4u <= len) {
            float32x4_t chunk = vld1q_f32(data + R);
            float32x4_t diff  = vabsq_f32(vsubq_f32(chunk, vn));
            float32x4_t abs_c = vabsq_f32(chunk);
            float32x4_t thr   = vmaxq_f32(vabs_tol, vmulq_f32(vrel_tol,
                                    vmaxq_f32(abs_c, vabs_ndl)));
            uint32x4_t  cmp   = vcleq_f32(diff, thr);
            uint32_t    tmp[4];
            vst1q_u32(tmp, cmp);
            if (!tmp[0]) break;
            for (size_t e = 0; e < 4u && R + e < len; e++) {
                if (tmp[e]) { if (best == SIZE_MAX) best = R + e; }
                else        { goto right_done_neon; }
            }
            R += 4u;
        }
        right_done_neon:
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
    float32x4_t v = vdupq_n_f32(value);
 
    size_t i = 0u;
 
    /* ---- Unaligned head: scalar until 16-byte aligned ---- */
    while (i < count && ((uintptr_t)(data + i) & 15u) != 0u) {
        data[i] = value;
        ++i;
    }
 
    /* ---- Aligned NEON body: 16 floats (4 × 128-bit) per iteration ---- */
    size_t aligned_end = i + ((count - i) / 16u) * 16u;
    for (; i < aligned_end; i += 16u) {
        vst1q_f32(data + i,        v);
        vst1q_f32(data + i + 4u,   v);
        vst1q_f32(data + i + 8u,   v);
        vst1q_f32(data + i + 12u,  v);
    }
 
    /* ---- Single-vector passes ---- */
    aligned_end = i + ((count - i) / 4u) * 4u;
    for (; i < aligned_end; i += 4u) {
        vst1q_f32(data + i, v);
    }
 
    /* ---- Scalar tail ---- */
    for (; i < count; ++i) {
        data[i] = value;
    }
}
// -------------------------------------------------------------------------------- 

static inline void _neon_transpose_4x4(float32x4_t r0, float32x4_t r1,
                                       float32x4_t r2, float32x4_t r3,
                                       float32x4_t* o0, float32x4_t* o1,
                                       float32x4_t* o2, float32x4_t* o3) {
    /*
     * vtrnq interleaves even/odd elements between two registers:
     *   vtrnq(r0, r1) =>  val[0] = {r0[0],r1[0],r0[2],r1[2]}
     *                     val[1] = {r0[1],r1[1],r0[3],r1[3]}
     */
    float32x4x2_t trn01 = vtrnq_f32(r0, r1);
    float32x4x2_t trn23 = vtrnq_f32(r2, r3);
 
    /*
     * Now recombine the 64-bit halves.  We reinterpret as float64x2
     * to use vcombine on the low/high 64-bit chunks, which swaps
     * the second and third rows of the 4×4 block.
     *
     * trn01.val[0] = [a0 b0 | a2 b2]   trn23.val[0] = [c0 d0 | c2 d2]
     * trn01.val[1] = [a1 b1 | a3 b3]   trn23.val[1] = [c1 d1 | c3 d3]
     *
     * We need:
     *   o0 = [a0 b0 c0 d0]  =>  low(trn01[0])  + low(trn23[0])
     *   o1 = [a1 b1 c1 d1]  =>  low(trn01[1])  + low(trn23[1])
     *   o2 = [a2 b2 c2 d2]  =>  high(trn01[0]) + high(trn23[0])
     *   o3 = [a3 b3 c3 d3]  =>  high(trn01[1]) + high(trn23[1])
     */
    float64x2_t a0 = vreinterpretq_f64_f32(trn01.val[0]);
    float64x2_t a1 = vreinterpretq_f64_f32(trn01.val[1]);
    float64x2_t b0 = vreinterpretq_f64_f32(trn23.val[0]);
    float64x2_t b1 = vreinterpretq_f64_f32(trn23.val[1]);
 
    /* vcombine_f64 joins two f64x1 scalars into an f64x2 */
    *o0 = vreinterpretq_f32_f64(
        vcombine_f64(vget_low_f64(a0),  vget_low_f64(b0)));
    *o1 = vreinterpretq_f32_f64(
        vcombine_f64(vget_low_f64(a1),  vget_low_f64(b1)));
    *o2 = vreinterpretq_f32_f64(
        vcombine_f64(vget_high_f64(a0), vget_high_f64(b0)));
    *o3 = vreinterpretq_f32_f64(
        vcombine_f64(vget_high_f64(a1), vget_high_f64(b1)));
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
            float32x4_t r0 = vld1q_f32(src + (i + 0u) * src_cols + j);
            float32x4_t r1 = vld1q_f32(src + (i + 1u) * src_cols + j);
            float32x4_t r2 = vld1q_f32(src + (i + 2u) * src_cols + j);
            float32x4_t r3 = vld1q_f32(src + (i + 3u) * src_cols + j);
 
            float32x4_t o0, o1, o2, o3;
            _neon_transpose_4x4(r0, r1, r2, r3, &o0, &o1, &o2, &o3);
 
            vst1q_f32(dst + (j + 0u) * src_rows + i, o0);
            vst1q_f32(dst + (j + 1u) * src_rows + i, o1);
            vst1q_f32(dst + (j + 2u) * src_rows + i, o2);
            vst1q_f32(dst + (j + 3u) * src_rows + i, o3);
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

static inline bool simd_equal_float(const float* a,
                                    const float* b,
                                    size_t       count) {
    size_t i = 0u;
 
    /* ---- Body: 16 floats (4 × 128-bit) per iteration ---- */
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        uint32x4_t x0 = veorq_u32(
            vreinterpretq_u32_f32(vld1q_f32(a + i)),
            vreinterpretq_u32_f32(vld1q_f32(b + i)));
        uint32x4_t x1 = veorq_u32(
            vreinterpretq_u32_f32(vld1q_f32(a + i + 4u)),
            vreinterpretq_u32_f32(vld1q_f32(b + i + 4u)));
        uint32x4_t x2 = veorq_u32(
            vreinterpretq_u32_f32(vld1q_f32(a + i + 8u)),
            vreinterpretq_u32_f32(vld1q_f32(b + i + 8u)));
        uint32x4_t x3 = veorq_u32(
            vreinterpretq_u32_f32(vld1q_f32(a + i + 12u)),
            vreinterpretq_u32_f32(vld1q_f32(b + i + 12u)));
 
        /* OR all XOR results */
        uint32x4_t any = vorrq_u32(vorrq_u32(x0, x1),
                                   vorrq_u32(x2, x3));
 
        /* vmaxvq_u32: horizontal max — nonzero means mismatch */
        if (vmaxvq_u32(any) != 0u) return false;
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        uint32x4_t x = veorq_u32(
            vreinterpretq_u32_f32(vld1q_f32(a + i)),
            vreinterpretq_u32_f32(vld1q_f32(b + i)));
 
        if (vmaxvq_u32(x) != 0u) return false;
    }
 
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
 
    float32x4_t zero = vdupq_n_f32(0.0f);
 
    /* ---- Body: 16 floats (4 × 128-bit) per iteration ---- */
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        float32x4_t v0 = vld1q_f32(data + i);
        float32x4_t v1 = vld1q_f32(data + i + 4u);
        float32x4_t v2 = vld1q_f32(data + i + 8u);
        float32x4_t v3 = vld1q_f32(data + i + 12u);
 
        /* Fast all-zero check */
        uint32x4_t any = vorrq_u32(
            vorrq_u32(vreinterpretq_u32_f32(v0), vreinterpretq_u32_f32(v1)),
            vorrq_u32(vreinterpretq_u32_f32(v2), vreinterpretq_u32_f32(v3)));
 
        if (vmaxvq_u32(any) == 0u) continue;
 
        /* vceqq_f32: 0xFFFFFFFF where equal to zero.
           vmvnq_u32: invert — 0xFFFFFFFF where nonzero.
           Shift right by 31 to get 0 or 1 per lane, then horizontal add. */
        uint32x4_t nz0 = vshrq_n_u32(vmvnq_u32(vceqq_f32(v0, zero)), 31);
        uint32x4_t nz1 = vshrq_n_u32(vmvnq_u32(vceqq_f32(v1, zero)), 31);
        uint32x4_t nz2 = vshrq_n_u32(vmvnq_u32(vceqq_f32(v2, zero)), 31);
        uint32x4_t nz3 = vshrq_n_u32(vmvnq_u32(vceqq_f32(v3, zero)), 31);
 
        /* Sum all 16 lanes */
        uint32x4_t sum = vaddq_u32(vaddq_u32(nz0, nz1),
                                   vaddq_u32(nz2, nz3));
        nnz += (size_t)vaddvq_u32(sum);
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        float32x4_t v = vld1q_f32(data + i);
        uint32x4_t nz = vshrq_n_u32(vmvnq_u32(vceqq_f32(v, vdupq_n_f32(0.0f))), 31);
        nnz += (size_t)vaddvq_u32(nz);
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
 
    float32x4_t zero = vdupq_n_f32(0.0f);
 
    /* ---- Body: 16 floats (4 × 128-bit) per iteration ---- */
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        /* Invert equality mask: 0xFFFFFFFF where nonzero */
        uint32x4_t nz0 = vmvnq_u32(vceqq_f32(vld1q_f32(data + i),       zero));
        uint32x4_t nz1 = vmvnq_u32(vceqq_f32(vld1q_f32(data + i + 4u),  zero));
        uint32x4_t nz2 = vmvnq_u32(vceqq_f32(vld1q_f32(data + i + 8u),  zero));
        uint32x4_t nz3 = vmvnq_u32(vceqq_f32(vld1q_f32(data + i + 12u), zero));
 
        /* OR all masks — any set bit means nonzero element exists */
        uint32x4_t any = vorrq_u32(vorrq_u32(nz0, nz1),
                                   vorrq_u32(nz2, nz3));
 
        if (vmaxvq_u32(any) != 0u) return false;
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        uint32x4_t nz = vmvnq_u32(vceqq_f32(vld1q_f32(data + i), zero));
        if (vmaxvq_u32(nz) != 0u) return false;
    }
 
    /* ---- Scalar tail ---- */
    for (; i < count; ++i) {
        if (data[i] != 0.0f) return false;
    }
 
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_NEON_FLOAT_INL */ 
// ================================================================================ 
// ================================================================================ 
// eof
