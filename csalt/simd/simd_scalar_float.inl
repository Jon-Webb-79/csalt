#ifndef SIMD_SCALAR_FLOAT_INL
#define SIMD_SCALAR_FLOAT_INL
// ================================================================================ 
// ================================================================================ 

/**
 * @file simd_scalar_float.inl
 * @brief Scalar fallback implementation of tolerance-aware float search.
 *
 * Performs a linear scan over elements in [start, end). When both abs_tol
 * and rel_tol are 0.0f the comparison reduces to bitwise equality via a
 * union reinterpretation, matching the exact bit-pattern semantics of the
 * integer SIMD paths (NaN != NaN, -0.0f != +0.0f). When either tolerance
 * is non-zero the combined test
 *
 *     fabsf(element - needle) <= fmaxf(abs_tol, rel_tol * fmaxf(|element|, |needle|))
 *
 * is applied.  Passing abs_tol = 0.0f selects pure relative mode; passing
 * rel_tol = 0.0f selects pure absolute mode.
 *
 * This is the reference implementation against which all SIMD paths are
 * verified and the target compiled when no SIMD flag is set.
 */

#include <stdint.h>
#include <stddef.h>
#include <math.h>

// ================================================================================
// Scalar contains
//
// Returns the index of the first element in [start, end) that matches
// needle within the supplied tolerances, or SIZE_MAX if no match is found.
// ================================================================================

static size_t simd_contains_float(const float* data,
                                   size_t       start,
                                   size_t       end,
                                   float        needle,
                                   float        abs_tol,
                                   float        rel_tol) {
    if (abs_tol == 0.0f && rel_tol == 0.0f) {
        /*
         * Exact bit-pattern match. Use a union to reinterpret both values as
         * uint32_t so that the comparison is bitwise: NaN != NaN,
         * -0.0f (0x80000000) != +0.0f (0x00000000).
         */
        union { float f; uint32_t u; } n;
        n.f = needle;
        for (size_t i = start; i < end; i++) {
            union { float f; uint32_t u; } v;
            v.f = data[i];
            if (v.u == n.u) return i;
        }
        return SIZE_MAX;
    }

    for (size_t i = start; i < end; i++) {
        float diff      = fabsf(data[i] - needle);
        float threshold = fmaxf(abs_tol,
                                rel_tol * fmaxf(fabsf(data[i]), fabsf(needle)));
        if (diff <= threshold) return i;
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

    /* ── Tolerance fan-out ──────────────────────────────────────────────────
     * The bisection left lo == hi at the landing position closest to needle.
     * Because the array is sorted, all elements within tolerance form a
     * contiguous run.  We check the landing position, then walk left and
     * right, stopping as soon as an element falls outside tolerance.  The
     * lowest matching index is returned.
     *
     * When both tolerances are 0.0f the fan-out degenerates to an exact
     * match check plus the original leftward scan for first occurrence.
     */
    size_t best = SIZE_MAX;

    /* Inline tolerance test */
#define _IN_TOL(idx) \
    (fabsf(data[(idx)] - needle) <= \
         fmaxf(abs_tol, rel_tol * fmaxf(fabsf(data[(idx)]), fabsf(needle))))

    /* Check landing position */
    if (_IN_TOL(mid)) best = mid;

    /* Walk left */
    size_t L = mid;
    while (L > 0u && _IN_TOL(L - 1u)) { L--; best = L; }

    /* Walk right — only improves result if landing itself didn't match */
    if (best == SIZE_MAX) {
        size_t R = mid + 1u;
        while (R < len && _IN_TOL(R)) { best = (best == SIZE_MAX) ? R : best; R++; }
    }

#undef _IN_TOL
    return best;
}
// ================================================================================ 
// ================================================================================ 
// MATRIX implementation

static inline void simd_fill_float(float* data, size_t count, float value) {
    for (size_t i = 0u; i < count; ++i) {
        data[i] = value;
    }
}
// -------------------------------------------------------------------------------- 

#define TRANSPOSE_TILE_SCALAR 16u
 
static inline void simd_transpose_float(const float* src,
                                        float*       dst,
                                        size_t       src_rows,
                                        size_t       src_cols) {
    size_t i = 0u;
    size_t j = 0u;
 
    /* ---- Tiled body ---- */
    size_t row_body = (src_rows / TRANSPOSE_TILE_SCALAR) * TRANSPOSE_TILE_SCALAR;
    size_t col_body = (src_cols / TRANSPOSE_TILE_SCALAR) * TRANSPOSE_TILE_SCALAR;
 
    for (i = 0u; i < row_body; i += TRANSPOSE_TILE_SCALAR) {
        for (j = 0u; j < col_body; j += TRANSPOSE_TILE_SCALAR) {
            for (size_t ii = i; ii < i + TRANSPOSE_TILE_SCALAR; ++ii) {
                for (size_t jj = j; jj < j + TRANSPOSE_TILE_SCALAR; ++jj) {
                    dst[jj * src_rows + ii] = src[ii * src_cols + jj];
                }
            }
        }
    }
 
    /* ---- Remainder columns (right strip) ---- */
    for (i = 0u; i < row_body; i += TRANSPOSE_TILE_SCALAR) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t ii = i; ii < i + TRANSPOSE_TILE_SCALAR; ++ii) {
                dst[j * src_rows + ii] = src[ii * src_cols + j];
            }
        }
    }
 
    /* ---- Remainder rows (bottom strip, including corner) ---- */
    for (i = row_body; i < src_rows; ++i) {
        for (j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
}
 
#undef TRANSPOSE_TILE_SCALAR
// -------------------------------------------------------------------------------- 

static inline bool simd_float_arrays_equal(const float* a,
                                           const float* b,
                                           size_t       count) {
    if (a == NULL || b == NULL) return false;

    for (size_t i = 0u; i < count; ++i) {
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
    for (size_t i = 0u; i < count; ++i) {
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
    for (size_t i = 0u; i < count; ++i) {
        if (data[i] != 0.0f) return false;
    }
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* SIMD_SCALAR_FLOAT_INL */
// ================================================================================ 
// ================================================================================ 
// eof
