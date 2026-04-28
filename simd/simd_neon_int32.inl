#include <arm_neon.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static inline void simd_fill_int32(int32_t* data, size_t count, int32_t value) {
    if (data == NULL) return;

    size_t i = 0u;
    int32x4_t vv = vdupq_n_s32(value);
    size_t vec_end = (count / 4u) * 4u;

    for (; i < vec_end; i += 4u) {
        vst1q_s32(data + i, vv);
    }

    for (; i < count; ++i) {
        data[i] = value;
    }
}

// --------------------------------------------------------------------------------

static inline void simd_transpose_int32(const int32_t* src,
                                        int32_t*       dst,
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

static inline bool simd_int32_arrays_equal(const int32_t* a,
                                           const int32_t* b,
                                           size_t         count) {
    if (a == NULL || b == NULL) return false;

    size_t i = 0u;
    size_t vec_end = (count / 4u) * 4u;

    for (; i < vec_end; i += 4u) {
        int32x4_t av = vld1q_s32(a + i);
        int32x4_t bv = vld1q_s32(b + i);
        uint32x4_t cmp = vceqq_s32(av, bv);

#if defined(__aarch64__)
        if (vminvq_u32(cmp) != 0xFFFFFFFFu) {
            return false;
        }
#else
        uint32_t lanes[4];
        vst1q_u32(lanes, cmp);
        for (int j = 0; j < 4; ++j) {
            if (lanes[j] != 0xFFFFFFFFu) return false;
        }
#endif
    }

    for (; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }

    return true;
}

// --------------------------------------------------------------------------------

static inline size_t simd_count_nonzero_int32(const int32_t* data,
                                              size_t         count) {
    if (data == NULL) return 0u;

    size_t nnz = 0u;
    size_t i = 0u;
    size_t vec_end = (count / 4u) * 4u;
    int32x4_t zero = vdupq_n_s32(0);

    for (; i < vec_end; i += 4u) {
        int32x4_t v = vld1q_s32(data + i);
        uint32x4_t eq = vceqq_s32(v, zero);

        uint32_t lanes[4];
        vst1q_u32(lanes, eq);
        for (int j = 0; j < 4; ++j) {
            if (lanes[j] != 0xFFFFFFFFu) ++nnz;
        }
    }

    for (; i < count; ++i) {
        if (data[i] != 0) ++nnz;
    }

    return nnz;
}

// --------------------------------------------------------------------------------

static inline size_t simd_scatter_csr_row_int32(const int32_t* row_data,
                                                size_t         cols,
                                                size_t         col_offset,
                                                size_t*        col_idx,
                                                int32_t*       values,
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

static inline bool simd_is_all_zero_int32(const int32_t* data, size_t count) {
    if (data == NULL) return true;

    size_t i = 0u;
    size_t vec_end = (count / 4u) * 4u;
    int32x4_t zero = vdupq_n_s32(0);

    for (; i < vec_end; i += 4u) {
        int32x4_t v = vld1q_s32(data + i);
        uint32x4_t eq = vceqq_s32(v, zero);

#if defined(__aarch64__)
        if (vminvq_u32(eq) != 0xFFFFFFFFu) {
            return false;
        }
#else
        uint32_t lanes[4];
        vst1q_u32(lanes, eq);
        for (int j = 0; j < 4; ++j) {
            if (lanes[j] != 0xFFFFFFFFu) return false;
        }
#endif
    }

    for (; i < count; ++i) {
        if (data[i] != 0) return false;
    }

    return true;
}
