#include <arm_sve.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static inline void simd_fill_uint16(uint16_t* data, size_t count, uint16_t value) {
    if (data == NULL) return;

    size_t i = 0u;
    while (i < count) {
        svbool_t pg = svwhilelt_b16(i, count);
        svuint16_t vv = svdup_n_u16(value);
        svst1_u16(pg, data + i, vv);
        i += svcnth();
    }
}

// --------------------------------------------------------------------------------

static inline void simd_transpose_uint16(const uint16_t* src,
                                         uint16_t*       dst,
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

static inline bool simd_uint16_arrays_equal(const uint16_t* a,
                                            const uint16_t* b,
                                            size_t          count) {
    if (a == NULL || b == NULL) return false;

    size_t i = 0u;
    while (i < count) {
        svbool_t pg = svwhilelt_b16(i, count);
        svuint16_t av = svld1_u16(pg, a + i);
        svuint16_t bv = svld1_u16(pg, b + i);
        svbool_t cmp = svcmpeq_u16(pg, av, bv);

        if (!svptest_all(pg, cmp)) {
            return false;
        }

        i += svcnth();
    }

    return true;
}

// --------------------------------------------------------------------------------

static inline size_t simd_count_nonzero_uint16(const uint16_t* data,
                                               size_t          count) {
    if (data == NULL) return 0u;

    size_t nnz = 0u;
    size_t i = 0u;

    while (i < count) {
        svbool_t pg = svwhilelt_b16(i, count);
        svuint16_t v = svld1_u16(pg, data + i);
        svbool_t nz = svcmpne_n_u16(pg, v, 0u);

        nnz += (size_t)svcntp_b16(pg, nz);
        i += svcnth();
    }

    return nnz;
}

// --------------------------------------------------------------------------------

static inline size_t simd_scatter_csr_row_uint16(const uint16_t* row_data,
                                                 size_t          cols,
                                                 size_t          col_offset,
                                                 size_t*         col_idx,
                                                 uint16_t*       values,
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

static inline bool simd_is_all_zero_uint16(const uint16_t* data, size_t count) {
    if (data == NULL) return true;

    size_t i = 0u;
    while (i < count) {
        svbool_t pg = svwhilelt_b16(i, count);
        svuint16_t v = svld1_u16(pg, data + i);
        svbool_t nz = svcmpne_n_u16(pg, v, 0u);

        if (svptest_any(pg, nz)) {
            return false;
        }

        i += svcnth();
    }

    return true;
}
