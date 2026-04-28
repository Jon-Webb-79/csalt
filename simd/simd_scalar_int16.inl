#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static inline void simd_fill_int16(int16_t* data, size_t count, int16_t value) {
    if (data == NULL) return;
    for (size_t i = 0u; i < count; ++i) {
        data[i] = value;
    }
}

// --------------------------------------------------------------------------------

static inline void simd_transpose_int16(const int16_t* src,
                                        int16_t*       dst,
                                        size_t         src_rows,
                                        size_t         src_cols) {
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

static inline bool simd_int16_arrays_equal(const int16_t* a,
                                           const int16_t* b,
                                           size_t         count) {
    if (a == NULL || b == NULL) return false;

    for (size_t i = 0u; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// --------------------------------------------------------------------------------

static inline size_t simd_count_nonzero_int16(const int16_t* data,
                                              size_t         count) {
    if (data == NULL) return 0u;

    size_t nnz = 0u;
    for (size_t i = 0u; i < count; ++i) {
        if (data[i] != 0) ++nnz;
    }
    return nnz;
}

// --------------------------------------------------------------------------------

static inline size_t simd_scatter_csr_row_int16(const int16_t* row_data,
                                                size_t         cols,
                                                size_t         col_offset,
                                                size_t*        col_idx,
                                                int16_t*       values,
                                                size_t         k) {
    if (row_data == NULL || col_idx == NULL || values == NULL) return k;

    for (size_t j = 0u; j < cols; ++j) {
        if (row_data[j] != 0) {
            col_idx[k] = col_offset + j;
            values[k]  = row_data[j];
            ++k;
        }
    }
    return k;
}

// --------------------------------------------------------------------------------

static inline bool simd_is_all_zero_int16(const int16_t* data, size_t count) {
    if (data == NULL) return true;

    for (size_t i = 0u; i < count; ++i) {
        if (data[i] != 0) return false;
    }
    return true;
}
