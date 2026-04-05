#ifndef SIMD_SSE2_FLOAT_INL
#define SIMD_SSE2_FLOAT_INL
// ================================================================================ 
// ================================================================================ 

/**
 * @file simd_sse2_float.inl
 * @brief SSE2 implementation of tolerance-aware float search.
 *
 * Processes 4 floats per iteration using 128-bit XMM registers.
 *
 * Exact mode (both tolerances == 0.0f):
 *   Reinterprets the float register as integer with _mm_castps_si128 and
 *   uses _mm_cmpeq_epi32 for bitwise equality — identical semantics to the
 *   uint32 path in contains_sse2_uint8.inl.  NaN != NaN, -0.0f != +0.0f.
 *
 * Tolerance mode (either tolerance > 0.0f):
 *   Computes |element - needle| <= max(abs_tol, rel_tol * max(|element|, |needle|))
 *   per lane using _mm_cmpps/subps/andps/maxps.  The sign bit is cleared
 *   with _mm_andps against a mask of 0x7FFFFFFF to implement fabsf.
 *
 * Requires: -msse2 (implicit on x86-64)
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <emmintrin.h>   /* SSE2 */

// ================================================================================
// Internal helpers
// ================================================================================

/* Absolute value mask: clears the sign bit of each 32-bit lane. */
static inline __m128 _sse2_fabs_ps(__m128 v) {
    __m128i mask = _mm_set1_epi32(0x7FFFFFFF);
    return _mm_and_ps(v, _mm_castsi128_ps(mask));
}

/*
 * For exact mode: return a 4-bit mask (one bit per lane) where bit k is set
 * if lane k matches needle exactly (bitwise).
 */
static inline int _sse2_exact_mask(__m128 chunk, __m128 vn) {
    __m128i ci = _mm_castps_si128(chunk);
    __m128i ni = _mm_castps_si128(vn);
    __m128i cmp = _mm_cmpeq_epi32(ci, ni);
    /* movemask_ps gives one bit per 32-bit lane based on sign bit */
    return _mm_movemask_ps(_mm_castsi128_ps(cmp));
}

/*
 * For tolerance mode: return a 4-bit mask where bit k is set if lane k
 * satisfies |chunk[k] - needle| <= max(abs_tol, rel_tol * max(|chunk[k]|, |needle|)).
 */
static inline int _sse2_tol_mask(__m128 chunk, __m128 vn,
                                  __m128 vabs_tol, __m128 vrel_tol,
                                  __m128 vabs_needle) {
    __m128 diff      = _sse2_fabs_ps(_mm_sub_ps(chunk, vn));
    __m128 abs_chunk = _sse2_fabs_ps(chunk);
    __m128 rel_base  = _mm_max_ps(abs_chunk, vabs_needle);
    __m128 rel_thresh = _mm_mul_ps(vrel_tol, rel_base);
    __m128 threshold  = _mm_max_ps(vabs_tol, rel_thresh);
    /* diff <= threshold: _mm_cmple_ps sets all bits in lane if true */
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
    size_t  i  = start;
    __m128  vn = _mm_set1_ps(needle);

    if (abs_tol == 0.0f && rel_tol == 0.0f) {
        /* Exact bitwise match — 4 floats per iteration */
        while (i + 4u <= end) {
            __m128 chunk = _mm_loadu_ps(data + i);
            int    mask  = _sse2_exact_mask(chunk, vn);
            if (mask != 0) {
                /* Find the first set bit (each lane is one bit in movemask_ps) */
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
            int    mask  = _sse2_tol_mask(chunk, vn, vabs_tol, vrel_tol,
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
        int    mask  = _sse2_tol_mask(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
        /* All 4 match — the entire block is within tolerance */
        if (mask == 0xF) { best = base; L = base; continue; }
        /* Partial match — find the rightmost matching lane from low end */
        for (size_t e = 0; e < 4u && base + e < L; e++) {
            if ((mask >> (int)e) & 1) { best = base + e; }
            else                      { L = base + e; goto left_done_sse2; }
        }
        L = base;
    }
    left_done_sse2:
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
            int    mask  = _sse2_tol_mask(chunk, vn, vabs_tol, vrel_tol, vabs_ndl);
            if (mask == 0) break;   /* none match — tolerance region ended */
            for (size_t e = 0; e < 4u && R + e < len; e++) {
                if ((mask >> (int)e) & 1) { if (best == SIZE_MAX) best = R + e; }
                else                      { goto right_done_sse2; }
            }
            R += 4u;
        }
        right_done_sse2:
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
 
    /* ---- Unaligned head: scalar until data is 16-byte aligned ---- */
    while (i < count && ((uintptr_t)(data + i) & 15u) != 0u) {
        data[i] = value;
        ++i;
    }
 
    /* ---- Aligned SSE2 body: 16 floats (4 × 128-bit) per iteration ---- */
    size_t aligned_end = i + ((count - i) / 16u) * 16u;
    for (; i < aligned_end; i += 16u) {
        _mm_store_ps(data + i,        v);
        _mm_store_ps(data + i + 4u,   v);
        _mm_store_ps(data + i + 8u,   v);
        _mm_store_ps(data + i + 12u,  v);
    }
 
    /* ---- Aligned single-vector passes ---- */
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

#define TRANSPOSE_TILE_SSE 16u
 
/*
 * Transpose a 4×4 block in registers using SSE unpack instructions.
 *
 *   Input (row-major):        Output (row-major of transposed):
 *   r0 = [a0 a1 a2 a3]       r0 = [a0 b0 c0 d0]
 *   r1 = [b0 b1 b2 b3]       r1 = [a1 b1 c1 d1]
 *   r2 = [c0 c1 c2 c3]       r2 = [a2 b2 c2 d2]
 *   r3 = [d0 d1 d2 d3]       r3 = [a3 b3 c3 d3]
 */
static inline void _sse2_transpose_4x4(__m128 r0, __m128 r1,
                                       __m128 r2, __m128 r3,
                                       __m128* o0, __m128* o1,
                                       __m128* o2, __m128* o3) {
    /* Interleave low/high pairs of two rows at a time */
    __m128 t0 = _mm_unpacklo_ps(r0, r1);   /* [a0 b0 a1 b1] */
    __m128 t1 = _mm_unpackhi_ps(r0, r1);   /* [a2 b2 a3 b3] */
    __m128 t2 = _mm_unpacklo_ps(r2, r3);   /* [c0 d0 c1 d1] */
    __m128 t3 = _mm_unpackhi_ps(r2, r3);   /* [c2 d2 c3 d3] */
 
    /* Move high/low halves to complete the transpose */
    *o0 = _mm_movelh_ps(t0, t2);           /* [a0 b0 c0 d0] */
    *o1 = _mm_movehl_ps(t2, t0);           /* [a1 b1 c1 d1] */
    *o2 = _mm_movelh_ps(t1, t3);           /* [a2 b2 c2 d2] */
    *o3 = _mm_movehl_ps(t3, t1);           /* [a3 b3 c3 d3] */
}
 
static inline void simd_transpose_float(const float* src,
                                        float*       dst,
                                        size_t       src_rows,
                                        size_t       src_cols) {
    size_t i = 0u;
    size_t j = 0u;
 
    /* ---- Tiled 4×4 body ---- */
    size_t row_body = (src_rows / 4u) * 4u;
    size_t col_body = (src_cols / 4u) * 4u;
 
    for (i = 0u; i < row_body; i += 4u) {
        for (j = 0u; j < col_body; j += 4u) {
            /* Load four source rows of 4 floats each */
            __m128 r0 = _mm_loadu_ps(src + (i + 0u) * src_cols + j);
            __m128 r1 = _mm_loadu_ps(src + (i + 1u) * src_cols + j);
            __m128 r2 = _mm_loadu_ps(src + (i + 2u) * src_cols + j);
            __m128 r3 = _mm_loadu_ps(src + (i + 3u) * src_cols + j);
 
            __m128 o0, o1, o2, o3;
            _sse2_transpose_4x4(r0, r1, r2, r3, &o0, &o1, &o2, &o3);
 
            /* Store four destination rows of 4 floats each */
            _mm_storeu_ps(dst + (j + 0u) * src_rows + i, o0);
            _mm_storeu_ps(dst + (j + 1u) * src_rows + i, o1);
            _mm_storeu_ps(dst + (j + 2u) * src_rows + i, o2);
            _mm_storeu_ps(dst + (j + 3u) * src_rows + i, o3);
        }
    }
 
    /* ---- Remainder columns (right strip, full 4-row blocks) ---- */
    for (i = 0u; i < row_body; i += 4u) {
        for (j = col_body; j < src_cols; ++j) {
            dst[j * src_rows + (i + 0u)] = src[(i + 0u) * src_cols + j];
            dst[j * src_rows + (i + 1u)] = src[(i + 1u) * src_cols + j];
            dst[j * src_rows + (i + 2u)] = src[(i + 2u) * src_cols + j];
            dst[j * src_rows + (i + 3u)] = src[(i + 3u) * src_cols + j];
        }
    }
 
    /* ---- Remainder rows (bottom strip, including corner) ---- */
    for (i = row_body; i < src_rows; ++i) {
        for (j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
}
 
#undef TRANSPOSE_TILE_SSE
// -------------------------------------------------------------------------------- 

static inline bool simd_equal_float(const float* a,
                                    const float* b,
                                    size_t       count) {
    size_t i = 0u;
 
    /* ---- SSE2 body: 16 floats (4 × 128-bit) per iteration ---- */
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        __m128i a0 = _mm_loadu_si128((const __m128i*)(a + i));
        __m128i b0 = _mm_loadu_si128((const __m128i*)(b + i));
        __m128i a1 = _mm_loadu_si128((const __m128i*)(a + i + 4u));
        __m128i b1 = _mm_loadu_si128((const __m128i*)(b + i + 4u));
        __m128i a2 = _mm_loadu_si128((const __m128i*)(a + i + 8u));
        __m128i b2 = _mm_loadu_si128((const __m128i*)(b + i + 8u));
        __m128i a3 = _mm_loadu_si128((const __m128i*)(a + i + 12u));
        __m128i b3 = _mm_loadu_si128((const __m128i*)(b + i + 12u));
 
        __m128i eq0 = _mm_cmpeq_epi32(a0, b0);
        __m128i eq1 = _mm_cmpeq_epi32(a1, b1);
        __m128i eq2 = _mm_cmpeq_epi32(a2, b2);
        __m128i eq3 = _mm_cmpeq_epi32(a3, b3);
 
        /* AND all four comparison results together */
        __m128i all = _mm_and_si128(_mm_and_si128(eq0, eq1),
                                    _mm_and_si128(eq2, eq3));
 
        /* movemask: 0xFFFF means all 16 bytes matched */
        if (_mm_movemask_epi8(all) != 0xFFFF) return false;
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        __m128i va = _mm_loadu_si128((const __m128i*)(a + i));
        __m128i vb = _mm_loadu_si128((const __m128i*)(b + i));
        __m128i eq = _mm_cmpeq_epi32(va, vb);
 
        if (_mm_movemask_epi8(eq) != 0xFFFF) return false;
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
 
    __m128 zero = _mm_setzero_ps();
 
    /* ---- Body: 16 floats (4 × 128-bit) per iteration ---- */
    size_t body_end = (count / 16u) * 16u;
    for (; i < body_end; i += 16u) {
        /* _mm_cmpeq_ps: 0xFFFFFFFF where equal to zero, 0 where nonzero.
           Invert: nonzero lanes produce 1-bits in the movemask. */
        int m0 = ~_mm_movemask_ps(_mm_cmpeq_ps(_mm_loadu_ps(data + i),       zero)) & 0xF;
        int m1 = ~_mm_movemask_ps(_mm_cmpeq_ps(_mm_loadu_ps(data + i + 4u),  zero)) & 0xF;
        int m2 = ~_mm_movemask_ps(_mm_cmpeq_ps(_mm_loadu_ps(data + i + 8u),  zero)) & 0xF;
        int m3 = ~_mm_movemask_ps(_mm_cmpeq_ps(_mm_loadu_ps(data + i + 12u), zero)) & 0xF;
 
        /* __builtin_popcount works on the combined 16-bit mask */
        nnz += (size_t)__builtin_popcount((unsigned)(m0 | (m1 << 4) | (m2 << 8) | (m3 << 12)));
    }
 
    /* ---- Single-vector passes ---- */
    size_t vec_end = i + ((count - i) / 4u) * 4u;
    for (; i < vec_end; i += 4u) {
        int m = ~_mm_movemask_ps(_mm_cmpeq_ps(_mm_loadu_ps(data + i), zero)) & 0xF;
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
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_SSE2_FLOAT_INL */ 
// ================================================================================ 
// ================================================================================ 
// eof
