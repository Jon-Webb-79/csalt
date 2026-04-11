#include <immintrin.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static inline void simd_fill_uint16(uint16_t* data, size_t count, uint16_t value) {
    if (data == NULL) return;

    size_t i = 0u;
    __m512i vv = _mm512_set1_epi16((short)value);
    size_t vec_end = (count / 32u) * 32u;

    for (; i < vec_end; i += 32u) {
        _mm512_storeu_si512((void*)(data + i), vv);
    }

    for (; i < count; ++i) {
        data[i] = value;
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
    size_t vec_end = (count / 32u) * 32u;

    for (; i < vec_end; i += 32u) {
        __m512i av = _mm512_loadu_si512((const void*)(a + i));
        __m512i bv = _mm512_loadu_si512((const void*)(b + i));
        __mmask32 mask = _mm512_cmpeq_epi16_mask(av, bv);

        if (mask != 0xFFFFFFFFu) {
            return false;
        }
    }

    for (; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }

    return true;
}

// --------------------------------------------------------------------------------

static inline size_t simd_count_nonzero_uint16(const uint16_t* data,
                                               size_t          count) {
    if (data == NULL) return 0u;

    size_t nnz = 0u;
    size_t i = 0u;
    __m512i zero = _mm512_setzero_si512();
    size_t vec_end = (count / 32u) * 32u;

    for (; i < vec_end; i += 32u) {
        __m512i v = _mm512_loadu_si512((const void*)(data + i));
        __mmask32 eq = _mm512_cmpeq_epi16_mask(v, zero);
        __mmask32 nz = ~eq;
        nnz += (size_t)__builtin_popcount((unsigned)nz);
    }

    for (; i < count; ++i) {
        if (data[i] != 0u) ++nnz;
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
    __m512i zero = _mm512_setzero_si512();
    size_t vec_end = (count / 32u) * 32u;

    for (; i < vec_end; i += 32u) {
        __m512i v = _mm512_loadu_si512((const void*)(data + i));
        __mmask32 eq = _mm512_cmpeq_epi16_mask(v, zero);

        if (eq != 0xFFFFFFFFu) {
            return false;
        }
    }

    for (; i < count; ++i) {
        if (data[i] != 0u) return false;
    }

    return true;
}
