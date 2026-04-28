#include <arm_neon.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static inline void simd_fill_uint64(uint64_t* data, size_t count, uint64_t value) {
    if (data == NULL) return;

#if defined(__aarch64__)
    size_t i = 0u;
    uint64x2_t vv = vdupq_n_u64(value);
    size_t vec_end = (count / 2u) * 2u;

    for (; i < vec_end; i += 2u) {
        vst1q_u64(data + i, vv);
    }

    for (; i < count; ++i) {
        data[i] = value;
    }
#else
    for (size_t i = 0u; i < count; ++i) {
        data[i] = value;
    }
#endif
}

// --------------------------------------------------------------------------------

static inline void simd_transpose_uint64(const uint64_t* src,
                                         uint64_t*       dst,
                                         size_t          src_rows,
                                         size_t          src_cols) {
    const size_t TILE = 16u;

    size_t row_body = (src_rows / TILE) * TILE;
    size_t col_body = (src_cols / TILE) * TILE;

    for (size_t i = 0u; i < row_body; i += TILE) {
        for (size_t j = 0u; j < col_body; j += TILE) {
            for (size_t ii = i; ii < i + TILE; ++ii) {
                for (size_t jj = j; jj < j + TILE; ++jj) {
                    dst[jj * src_rows + ii] = src[ii * src_cols + jj];
                }
            }
        }
    }

    for (size_t i = 0u; i < row_body; i += TILE) {
        for (size_t j = col_body; j < src_cols; ++j) {
            for (size_t ii = i; ii < i + TILE; ++ii) {
                dst[j * src_rows + ii] = src[ii * src_cols + j];
            }
        }
    }

    for (size_t i = row_body; i < src_rows; ++i) {
        for (size_t j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
}

// --------------------------------------------------------------------------------

static inline bool simd_uint64_arrays_equal(const uint64_t* a,
                                            const uint64_t* b,
                                            size_t          count) {
    if (a == NULL || b == NULL) return false;

#if defined(__aarch64__)
    size_t i = 0u;
    size_t vec_end = (count / 2u) * 2u;

    for (; i < vec_end; i += 2u) {
        uint64x2_t av = vld1q_u64(a + i);
        uint64x2_t bv = vld1q_u64(b + i);
        uint64x2_t eq = vceqq_u64(av, bv);

        if (vminvq_u64(eq) != UINT64_MAX) {
            return false;
        }
    }

    for (; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }

    return true;
#else
    for (size_t i = 0u; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
#endif
}

// --------------------------------------------------------------------------------

static inline size_t simd_count_nonzero_uint64(const uint64_t* data,
                                               size_t          count) {
    if (data == NULL) return 0u;

#if defined(__aarch64__)
    size_t nnz = 0u;
    size_t i = 0u;
    size_t vec_end = (count / 2u) * 2u;
    uint64x2_t zero = vdupq_n_u64(0u);

    for (; i < vec_end; i += 2u) {
        uint64x2_t v = vld1q_u64(data + i);
        uint64x2_t eq = vceqq_u64(v, zero);

        uint64_t lanes[2];
        vst1q_u64(lanes, eq);
        for (int j = 0; j < 2; ++j) {
            if (lanes[j] != UINT64_MAX) ++nnz;
        }
    }

    for (; i < count; ++i) {
        if (data[i] != 0u) ++nnz;
    }

    return nnz;
#else
    size_t nnz = 0u;
    for (size_t i = 0u; i < count; ++i) {
        if (data[i] != 0u) ++nnz;
    }
    return nnz;
#endif
}

// --------------------------------------------------------------------------------

static inline size_t simd_scatter_csr_row_uint64(const uint64_t* row_data,
                                                 size_t          cols,
                                                 size_t          col_offset,
                                                 size_t*         col_idx,
                                                 uint64_t*       values,
                                                 size_t          k) {
    if (row_data == NULL || col_idx == NULL || values == NULL) return k;

    for (size_t j = 0u; j < cols; ++j) {
        if (row_data[j] != 0u) {
            col_idx[k] = col_offset + j;
            values[k]  = row_data[j];
            ++k;
        }
    }

    return k;
}

// --------------------------------------------------------------------------------

static inline bool simd_is_all_zero_uint64(const uint64_t* data, size_t count) {
    if (data == NULL) return true;

#if defined(__aarch64__)
    size_t i = 0u;
    size_t vec_end = (count / 2u) * 2u;
    uint64x2_t zero = vdupq_n_u64(0u);

    for (; i < vec_end; i += 2u) {
        uint64x2_t v = vld1q_u64(data + i);
        uint64x2_t eq = vceqq_u64(v, zero);

        if (vminvq_u64(eq) != UINT64_MAX) {
            return false;
        }
    }

    for (; i < count; ++i) {
        if (data[i] != 0u) return false;
    }

    return true;
#else
    for (size_t i = 0u; i < count; ++i) {
        if (data[i] != 0u) return false;
    }
    return true;
#endif
}
