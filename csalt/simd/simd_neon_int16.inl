#include <arm_neon.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static inline void simd_fill_int16(int16_t* data, size_t count, int16_t value) {
    if (data == NULL) return;

    size_t i = 0u;
    int16x8_t vv = vdupq_n_s16(value);
    size_t vec_end = (count / 8u) * 8u;

    for (; i < vec_end; i += 8u) {
        vst1q_s16(data + i, vv);
    }

    for (; i < count; ++i) {
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

    size_t i = 0u;
    size_t vec_end = (count / 8u) * 8u;

    for (; i < vec_end; i += 8u) {
        int16x8_t av = vld1q_s16(a + i);
        int16x8_t bv = vld1q_s16(b + i);
        uint16x8_t cmp = vceqq_s16(av, bv);

#if defined(__aarch64__)
        if (vminvq_u16(cmp) != 0xFFFFu) {
            return false;
        }
#else
        uint16_t lanes[8];
        vst1q_u16(lanes, cmp);
        for (int j = 0; j < 8; ++j) {
            if (lanes[j] != 0xFFFFu) return false;
        }
#endif
    }

    for (; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }

    return true;
}

// --------------------------------------------------------------------------------

static inline size_t simd_count_nonzero_int16(const int16_t* data,
                                              size_t         count) {
    if (data == NULL) return 0u;

    size_t nnz = 0u;
    size_t i = 0u;
    size_t vec_end = (count / 8u) * 8u;
    int16x8_t zero = vdupq_n_s16(0);

    for (; i < vec_end; i += 8u) {
        int16x8_t v = vld1q_s16(data + i);
        uint16x8_t eq = vceqq_s16(v, zero);

        uint16_t lanes[8];
        vst1q_u16(lanes, eq);
        for (int j = 0; j < 8; ++j) {
            if (lanes[j] != 0xFFFFu) ++nnz;
        }
    }

    for (; i < count; ++i) {
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

    size_t i = 0u;
    size_t vec_end = (count / 8u) * 8u;
    int16x8_t zero = vdupq_n_s16(0);

    for (; i < vec_end; i += 8u) {
        int16x8_t v = vld1q_s16(data + i);
        uint16x8_t eq = vceqq_s16(v, zero);

#if defined(__aarch64__)
        if (vminvq_u16(eq) != 0xFFFFu) {
            return false;
        }
#else
        uint16_t lanes[8];
        vst1q_u16(lanes, eq);
        for (int j = 0; j < 8; ++j) {
            if (lanes[j] != 0xFFFFu) return false;
        }
#endif
    }

    for (; i < count; ++i) {
        if (data[i] != 0) return false;
    }

    return true;
}
