#include <arm_sve.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

static inline void simd_fill_int8(int8_t* data, size_t count, int8_t value) {
    memset(data, (unsigned char)value, count);
}

// --------------------------------------------------------------------------------

static inline void simd_transpose_int8(const int8_t* src,
                                       int8_t*       dst,
                                       size_t        src_rows,
                                       size_t        src_cols) {
    const size_t TILE = 16u;
    size_t i = 0u;
    size_t j = 0u;

    size_t row_body = (src_rows / TILE) * TILE;
    size_t col_body = (src_cols / TILE) * TILE;

    for (i = 0u; i < row_body; i += TILE) {
        for (j = 0u; j < col_body; j += TILE) {
            for (size_t ii = i; ii < i + TILE; ++ii) {
                for (size_t jj = j; jj < j + TILE; ++jj) {
                    dst[jj * src_rows + ii] = src[ii * src_cols + jj];
                }
            }
        }
    }

    for (i = 0u; i < row_body; i += TILE) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t ii = i; ii < i + TILE; ++ii) {
                dst[j * src_rows + ii] = src[ii * src_cols + j];
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

static inline bool simd_int8_arrays_equal(const int8_t* a,
                                          const int8_t* b,
                                          size_t        count) {
    if (a == NULL || b == NULL) return false;

    size_t i = 0u;
    while (i < count) {
        svbool_t pg = svwhilelt_b8(i, count);
        svint8_t av = svld1_s8(pg, a + i);
        svint8_t bv = svld1_s8(pg, b + i);
        svbool_t cmp = svcmpeq_s8(pg, av, bv);

        if (!svptest_all(pg, cmp)) {
            return false;
        }

        i += svcntb();
    }

    return true;
}

// --------------------------------------------------------------------------------

static inline size_t simd_count_nonzero_int8(const int8_t* data,
                                             size_t       count) {
    size_t nnz = 0u;
    size_t i = 0u;

    while (i < count) {
        svbool_t pg = svwhilelt_b8(i, count);
        svint8_t v = svld1_s8(pg, data + i);
        svbool_t nz = svcmpne_n_s8(pg, v, 0);

        nnz += (size_t)svcntp_b8(pg, nz);
        i += svcntb();
    }

    return nnz;
}

// --------------------------------------------------------------------------------

static inline size_t simd_scatter_csr_row_int8(const int8_t* row_data,
                                               size_t        cols,
                                               size_t        col_offset,
                                               size_t*       col_idx,
                                               int8_t*       values,
                                               size_t        k) {
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

static inline bool simd_is_all_zero_int8(const int8_t* data, size_t count) {
    size_t i = 0u;

    while (i < count) {
        svbool_t pg = svwhilelt_b8(i, count);
        svint8_t v = svld1_s8(pg, data + i);
        svbool_t nz = svcmpne_n_s8(pg, v, 0);

        if (svptest_any(pg, nz)) {
            return false;
        }

        i += svcntb();
    }

    return true;
}
