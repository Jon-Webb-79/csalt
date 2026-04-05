#ifndef SIMD_AVX512_FLOAT_INL
#define SIMD_AVX512_FLOAT_INL
// ================================================================================ 
// ================================================================================ 

/**
 * @file simd_avx512_float.inl
 * @brief AVX-512 implementation of tolerance-aware float search.
 *
 * Processes 16 floats per iteration using 512-bit ZMM registers.
 *
 * Exact mode (both tolerances == 0.0f):
 *   _mm512_cmpeq_epi32_mask reinterprets the ZMM register as 16 x int32
 *   and returns a 16-bit mask directly — no movemask needed.  Semantics
 *   are bitwise: NaN != NaN, -0.0f != +0.0f.
 *
 * Tolerance mode:
 *   _mm512_cmp_ps_mask(_CMP_LE_OQ) returns a 16-bit mask directly.
 *   _mm512_andnot_ps clears the sign bit for fabsf lane-wise.
 *   __builtin_ctz on the 16-bit mask gives the first matching lane.
 *
 * Requires: -mavx512f -mavx512bw
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <immintrin.h>   /* AVX-512 */

// ================================================================================
// Internal helpers
// ================================================================================

static inline __m512 _avx512_fabs_ps(__m512 v) {
    __m512 sign_mask = _mm512_set1_ps(-0.0f);
    return _mm512_andnot_ps(sign_mask, v);
}

/* Exact bitwise match: cast to int32 lanes, compare, return 16-bit mask. */
static inline __mmask16 _avx512_exact_mask(__m512 chunk, __m512 vn) {
    __m512i ci = _mm512_castps_si512(chunk);
    __m512i ni = _mm512_castps_si512(vn);
    return _mm512_cmpeq_epi32_mask(ci, ni);
}

static inline __mmask16 _avx512_tol_mask(__m512 chunk, __m512 vn,
                                          __m512 vabs_tol, __m512 vrel_tol,
                                          __m512 vabs_needle) {
    __m512 diff       = _avx512_fabs_ps(_mm512_sub_ps(chunk, vn));
    __m512 abs_chunk  = _avx512_fabs_ps(chunk);
    __m512 rel_base   = _mm512_max_ps(abs_chunk, vabs_needle);
    __m512 rel_thresh = _mm512_mul_ps(vrel_tol, rel_base);
    __m512 threshold  = _mm512_max_ps(vabs_tol, rel_thresh);
    return _mm512_cmp_ps_mask(diff, threshold, _CMP_LE_OQ);
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
    __m512 vn = _mm512_set1_ps(needle);

    if (abs_tol == 0.0f && rel_tol == 0.0f) {
        while (i + 16u <= end) {
            __m512    chunk = _mm512_loadu_ps(data + i);
            __mmask16 mask  = _avx512_exact_mask(chunk, vn);
            if (mask != 0) {
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
            i += 16u;
        }
    } else {
        __m512 vabs_tol    = _mm512_set1_ps(abs_tol);
        __m512 vrel_tol    = _mm512_set1_ps(rel_tol);
        __m512 vabs_needle = _mm512_set1_ps(fabsf(needle));

        while (i + 16u <= end) {
            __m512    chunk = _mm512_loadu_ps(data + i);
            __mmask16 mask  = _avx512_tol_mask(chunk, vn, vabs_tol, vrel_tol,
                                               vabs_needle);
            if (mask != 0) {
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
            i += 16u;
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
    __m512 vn        = _mm512_set1_ps(needle);
    __m512 vabs_tol  = _mm512_set1_ps(abs_tol);
    __m512 vrel_tol  = _mm512_set1_ps(rel_tol);
    __m512 vabs_ndl  = _mm512_set1_ps(abs_needle);

    /* Check landing */
    {
        float diff = fabsf(data[mid] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[mid]), abs_needle));
        if (diff <= thr) best = mid;
    }

    /* SIMD left walk — 16 floats per step */
    size_t L = mid;
    while (L >= 16u) {
        size_t    base  = L - 16u;
        __m512    chunk = _mm512_loadu_ps(data + base);
        __mmask16 mask  = _avx512_tol_mask(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
        if (mask == 0xFFFFu) { best = base; L = base; continue; }
        for (size_t e = 0; e < 16u && base + e < L; e++) {
            if ((mask >> (unsigned)e) & 1u) { best = base + e; }
            else                            { L = base + e; goto left_done_avx512; }
        }
        L = base;
    }
    left_done_avx512:
    while (L > 0u) {
        float diff = fabsf(data[L - 1u] - needle);
        float thr  = fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[L - 1u]), abs_needle));
        if (diff <= thr) { best = --L; } else { break; }
    }

    /* SIMD right walk */
    if (best == SIZE_MAX) {
        size_t R = mid + 1u;
        while (R + 16u <= len) {
            __m512    chunk = _mm512_loadu_ps(data + R);
            __mmask16 mask  = _avx512_tol_mask(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
            if (mask == 0) break;
            for (size_t e = 0; e < 16u && R + e < len; e++) {
                if ((mask >> (unsigned)e) & 1u) { if (best == SIZE_MAX) best = R + e; }
                else                            { goto right_done_avx512; }
            }
            R += 16u;
        }
        right_done_avx512:
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
    __m512 v = _mm512_set1_ps(value);
 
    size_t i = 0u;
 
    /* ---- Unaligned head: scalar until 64-byte aligned ---- */
    while (i < count && ((uintptr_t)(data + i) & 63u) != 0u) {
        data[i] = value;
        ++i;
    }
 
    /* ---- Aligned AVX-512 body: 64 floats (4 × 512-bit) per iteration ---- */
    size_t aligned_end = i + ((count - i) / 64u) * 64u;
    for (; i < aligned_end; i += 64u) {
        _mm512_store_ps(data + i,        v);
        _mm512_store_ps(data + i + 16u,  v);
        _mm512_store_ps(data + i + 32u,  v);
        _mm512_store_ps(data + i + 48u,  v);
    }
 
    /* ---- Single-vector passes ---- */
    aligned_end = i + ((count - i) / 16u) * 16u;
    for (; i < aligned_end; i += 16u) {
        _mm512_store_ps(data + i, v);
    }
 
    /* ---- Scalar tail (up to 15 elements) ---- */
    if (i < count) {
        __mmask16 mask = (__mmask16)((1u << (count - i)) - 1u);
        _mm512_mask_storeu_ps(data + i, mask, v);
    }
}
// -------------------------------------------------------------------------------- 

static inline void _avx512_transpose_16x16(const __m512 r[16], __m512 o[16]) {
    /* ---- Phase 1: interleave pairs (within 128-bit lanes) ---- */
    __m512 t0  = _mm512_unpacklo_ps(r[0],  r[1]);
    __m512 t1  = _mm512_unpackhi_ps(r[0],  r[1]);
    __m512 t2  = _mm512_unpacklo_ps(r[2],  r[3]);
    __m512 t3  = _mm512_unpackhi_ps(r[2],  r[3]);
    __m512 t4  = _mm512_unpacklo_ps(r[4],  r[5]);
    __m512 t5  = _mm512_unpackhi_ps(r[4],  r[5]);
    __m512 t6  = _mm512_unpacklo_ps(r[6],  r[7]);
    __m512 t7  = _mm512_unpackhi_ps(r[6],  r[7]);
    __m512 t8  = _mm512_unpacklo_ps(r[8],  r[9]);
    __m512 t9  = _mm512_unpackhi_ps(r[8],  r[9]);
    __m512 t10 = _mm512_unpacklo_ps(r[10], r[11]);
    __m512 t11 = _mm512_unpackhi_ps(r[10], r[11]);
    __m512 t12 = _mm512_unpacklo_ps(r[12], r[13]);
    __m512 t13 = _mm512_unpackhi_ps(r[12], r[13]);
    __m512 t14 = _mm512_unpacklo_ps(r[14], r[15]);
    __m512 t15 = _mm512_unpackhi_ps(r[14], r[15]);
 
    /* ---- Phase 2: 4-wide shuffle within 128-bit lanes ---- */
    __m512 s0  = _mm512_shuffle_ps(t0,  t2,  0x44);
    __m512 s1  = _mm512_shuffle_ps(t0,  t2,  0xEE);
    __m512 s2  = _mm512_shuffle_ps(t1,  t3,  0x44);
    __m512 s3  = _mm512_shuffle_ps(t1,  t3,  0xEE);
    __m512 s4  = _mm512_shuffle_ps(t4,  t6,  0x44);
    __m512 s5  = _mm512_shuffle_ps(t4,  t6,  0xEE);
    __m512 s6  = _mm512_shuffle_ps(t5,  t7,  0x44);
    __m512 s7  = _mm512_shuffle_ps(t5,  t7,  0xEE);
    __m512 s8  = _mm512_shuffle_ps(t8,  t10, 0x44);
    __m512 s9  = _mm512_shuffle_ps(t8,  t10, 0xEE);
    __m512 s10 = _mm512_shuffle_ps(t9,  t11, 0x44);
    __m512 s11 = _mm512_shuffle_ps(t9,  t11, 0xEE);
    __m512 s12 = _mm512_shuffle_ps(t12, t14, 0x44);
    __m512 s13 = _mm512_shuffle_ps(t12, t14, 0xEE);
    __m512 s14 = _mm512_shuffle_ps(t13, t15, 0x44);
    __m512 s15 = _mm512_shuffle_ps(t13, t15, 0xEE);
 
    /* ---- Phase 3: cross-lane 128-bit shuffles ----
     *
     * _mm512_shuffle_f32x4(a, b, imm8) selects four 128-bit lanes from
     * {a[0..3], b[0..3]} by the 2-bit fields of imm8:
     *   result[lane k] = (imm8 >> (k*2)) & 3  selects from a (if < 4)
     *                     or from b (bit2 comes from the b/a split).
     *
     * We need to gather lane 0 from rows 0,4,8,12 into output row 0, etc.
     *
     * Step 3a: combine groups of 4 rows that share the same within-lane
     *          column data, swapping 128-bit quarters.
     * Step 3b: final cross-quarter assembly.
     */
 
    /* 3a: pair up groups at 4-row stride */
    __m512 u0  = _mm512_shuffle_f32x4(s0,  s4,  0x88);  /* lanes: s0[0] s0[2] s4[0] s4[2] */
    __m512 u1  = _mm512_shuffle_f32x4(s1,  s5,  0x88);
    __m512 u2  = _mm512_shuffle_f32x4(s2,  s6,  0x88);
    __m512 u3  = _mm512_shuffle_f32x4(s3,  s7,  0x88);
    __m512 u4  = _mm512_shuffle_f32x4(s0,  s4,  0xDD);  /* lanes: s0[1] s0[3] s4[1] s4[3] */
    __m512 u5  = _mm512_shuffle_f32x4(s1,  s5,  0xDD);
    __m512 u6  = _mm512_shuffle_f32x4(s2,  s6,  0xDD);
    __m512 u7  = _mm512_shuffle_f32x4(s3,  s7,  0xDD);
    __m512 u8  = _mm512_shuffle_f32x4(s8,  s12, 0x88);
    __m512 u9  = _mm512_shuffle_f32x4(s9,  s13, 0x88);
    __m512 u10 = _mm512_shuffle_f32x4(s10, s14, 0x88);
    __m512 u11 = _mm512_shuffle_f32x4(s11, s15, 0x88);
    __m512 u12 = _mm512_shuffle_f32x4(s8,  s12, 0xDD);
    __m512 u13 = _mm512_shuffle_f32x4(s9,  s13, 0xDD);
    __m512 u14 = _mm512_shuffle_f32x4(s10, s14, 0xDD);
    __m512 u15 = _mm512_shuffle_f32x4(s11, s15, 0xDD);
 
    /* 3b: final assembly — each output row draws from two u-vectors */
    o[0]  = _mm512_shuffle_f32x4(u0,  u8,  0x88);
    o[1]  = _mm512_shuffle_f32x4(u1,  u9,  0x88);
    o[2]  = _mm512_shuffle_f32x4(u2,  u10, 0x88);
    o[3]  = _mm512_shuffle_f32x4(u3,  u11, 0x88);
    o[4]  = _mm512_shuffle_f32x4(u4,  u12, 0x88);
    o[5]  = _mm512_shuffle_f32x4(u5,  u13, 0x88);
    o[6]  = _mm512_shuffle_f32x4(u6,  u14, 0x88);
    o[7]  = _mm512_shuffle_f32x4(u7,  u15, 0x88);
    o[8]  = _mm512_shuffle_f32x4(u0,  u8,  0xDD);
    o[9]  = _mm512_shuffle_f32x4(u1,  u9,  0xDD);
    o[10] = _mm512_shuffle_f32x4(u2,  u10, 0xDD);
    o[11] = _mm512_shuffle_f32x4(u3,  u11, 0xDD);
    o[12] = _mm512_shuffle_f32x4(u4,  u12, 0xDD);
    o[13] = _mm512_shuffle_f32x4(u5,  u13, 0xDD);
    o[14] = _mm512_shuffle_f32x4(u6,  u14, 0xDD);
    o[15] = _mm512_shuffle_f32x4(u7,  u15, 0xDD);
}
 
static inline void simd_transpose_float(const float* src,
                                        float*       dst,
                                        size_t       src_rows,
                                        size_t       src_cols) {
    size_t i = 0u;
    size_t j = 0u;
 
    size_t row_body = (src_rows / 16u) * 16u;
    size_t col_body = (src_cols / 16u) * 16u;
 
    /* ---- Tiled 16×16 body ---- */
    for (i = 0u; i < row_body; i += 16u) {
        for (j = 0u; j < col_body; j += 16u) {
            __m512 r[16], o[16];
 
            for (size_t k = 0u; k < 16u; ++k) {
                r[k] = _mm512_loadu_ps(src + (i + k) * src_cols + j);
            }
 
            _avx512_transpose_16x16(r, o);
 
            for (size_t k = 0u; k < 16u; ++k) {
                _mm512_storeu_ps(dst + (j + k) * src_rows + i, o[k]);
            }
        }
    }
 
    /* ---- Remainder columns ---- */
    for (i = 0u; i < row_body; i += 16u) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t k = 0u; k < 16u; ++k) {
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
}
// -------------------------------------------------------------------------------- 

static inline bool simd_equal_float(const float* a,
                                    const float* b,
                                    size_t       count) {
    size_t i = 0u;
 
    /* ---- Body: 64 floats (4 × 512-bit) per iteration ---- */
    size_t body_end = (count / 64u) * 64u;
    for (; i < body_end; i += 64u) {
        __m512i x0 = _mm512_xor_si512(
            _mm512_loadu_si512((const __m512i*)(a + i)),
            _mm512_loadu_si512((const __m512i*)(b + i)));
        __m512i x1 = _mm512_xor_si512(
            _mm512_loadu_si512((const __m512i*)(a + i + 16u)),
            _mm512_loadu_si512((const __m512i*)(b + i + 16u)));
        __m512i x2 = _mm512_xor_si512(
            _mm512_loadu_si512((const __m512i*)(a + i + 32u)),
            _mm512_loadu_si512((const __m512i*)(b + i + 32u)));
        __m512i x3 = _mm512_xor_si512(
            _mm512_loadu_si512((const __m512i*)(a + i + 48u)),
            _mm512_loadu_si512((const __m512i*)(b + i + 48u)));
 
        /* OR all XOR results */
        __m512i any = _mm512_or_si512(_mm512_or_si512(x0, x1),
                                      _mm512_or_si512(x2, x3));
 
        /* _mm512_test_epi32_mask(v, v): mask bit set where lane != 0 */
        if (_mm512_test_epi32_mask(any, any) != 0u) return false;
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        __m512i x = _mm512_xor_si512(
            _mm512_loadu_si512((const __m512i*)(a + i)),
            _mm512_loadu_si512((const __m512i*)(b + i)));
 
        if (_mm512_test_epi32_mask(x, x) != 0u) return false;
    }
 
    /* ---- Masked tail (up to 15 elements) ---- */
    if (i < count) {
        __mmask16 mask = (__mmask16)((1u << (count - i)) - 1u);
 
        __m512i va = _mm512_maskz_loadu_epi32(mask, (const __m512i*)(a + i));
        __m512i vb = _mm512_maskz_loadu_epi32(mask, (const __m512i*)(b + i));
        __m512i x  = _mm512_xor_si512(va, vb);
 
        /* Only test lanes within the mask */
        if ((_mm512_test_epi32_mask(x, x) & mask) != 0u) return false;
    }
 
    return true;
}
// -------------------------------------------------------------------------------- 

static inline size_t simd_scatter_csr_row_float(const float* row_data,
                                                size_t       cols,
                                                size_t       col_offset,
                                                size_t*      col_idx,
                                                float*       values,
                                                size_t       k) {
    size_t j = 0u;
 
    __m512 zero = _mm512_setzero_ps();
 
    /* Column index base: [0, 1, 2, ..., 15] added to (col_offset + j) */
    __m512i idx_base = _mm512_set_epi32(15, 14, 13, 12, 11, 10, 9, 8,
                                         7,  6,  5,  4,  3,  2, 1, 0);
 
    /* ---- Body: 16 floats per iteration ---- */
    size_t vec_end = (cols / 16u) * 16u;
    for (; j < vec_end; j += 16u) {
        __m512 v = _mm512_loadu_ps(row_data + j);
        __mmask16 mask = _mm512_cmp_ps_mask(v, zero, _CMP_NEQ_UQ);
 
        if (mask == 0u) continue;   /* entire vector is zero */
 
        int popcnt = __builtin_popcount((unsigned)mask);
 
        /* Compress-store nonzero values */
        _mm512_mask_compressstoreu_ps(values + k, mask, v);
 
        /* Build and compress-store column indices.
         *
         * col_idx is size_t (64-bit) but our indices fit in 32 bits.
         * We compress into a temporary 32-bit buffer and widen.
         *
         * For simplicity and portability (size_t may be 32-bit on
         * some platforms), we fall back to a small scalar loop for
         * the column indices.
         */
        __m512i col_vec = _mm512_add_epi32(
            idx_base, _mm512_set1_epi32((int)(col_offset + j)));
 
        /* Compress column indices into a stack buffer */
        int32_t col_tmp[16];
        _mm512_mask_compressstoreu_epi32(col_tmp, mask, col_vec);
 
        /* Widen to size_t and write to col_idx */
        for (int n = 0; n < popcnt; ++n) {
            col_idx[k + (size_t)n] = (size_t)col_tmp[n];
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
#endif /* SIMD_AVX512_FLOAT_INL */ 
// ================================================================================ 
// ================================================================================ 
// eof
