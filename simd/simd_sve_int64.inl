#include <arm_sve.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static inline void simd_fill_int64(int64_t* data, size_t count, int64_t value) {
    if (data == NULL) return;

    size_t i = 0u;
    while (i < count) {
        svbool_t pg = svwhilelt_b64(i, count);
        svint64_t vv = svdup_n_s64(value);
        svst1_s64(pg, data + i, vv);
        i += svcntd();
    }
}

// --------------------------------------------------------------------------------

static inline void simd_transpose_int64(const int64_t* src,
                                        int64_t*       dst,
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

static inline bool simd_int64_arrays_equal(const int64_t* a,
                                           const int64_t* b,
                                           size_t         count) {
    if (a == NULL || b == NULL) return false;

    size_t i = 0u;
    while (i < count) {
        svbool_t pg = svwhilelt_b64(i, count);
        svint64_t av = svld1_s64(pg, a + i);
        svint64_t bv = svld1_s64(pg, b + i);
        svbool_t eq = svcmpeq_s64(pg, av, bv);

        if (!svptest_all(pg, eq)) {
            return false;
        }

        i += svcntd();
    }

    return true;
}

// --------------------------------------------------------------------------------

static inline size_t simd_count_nonzero_int64(const int64_t* data,
                                              size_t         count) {
    if (data == NULL) return 0u;

    size_t nnz = 0u;
    size_t i = 0u;

    while (i < count) {
        svbool_t pg = svwhilelt_b64(i, count);
        svint64_t v = svld1_s64(pg, data + i);
        svbool_t nz = svcmpne_n_s64(pg, v, 0);

        nnz += (size_t)svcntp_b64(pg, nz);
        i += svcntd();
    }

    return nnz;
}

// --------------------------------------------------------------------------------

static inline size_t simd_scatter_csr_row_int64(const int64_t* row_data,
                                                size_t         cols,
                                                size_t         col_offset,
                                                size_t*        col_idx,
                                                int64_t*       values,
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

static inline bool simd_is_all_zero_int64(const int64_t* data, size_t count) {
    if (data == NULL) return true;

    size_t i = 0u;
    while (i < count) {
        svbool_t pg = svwhilelt_b64(i, count);
        svint64_t v = svld1_s64(pg, data + i);
        svbool_t nz = svcmpne_n_s64(pg, v, 0);

        if (svptest_any(pg, nz)) {
            return false;
        }

        i += svcntd();
    }

    return true;
}
