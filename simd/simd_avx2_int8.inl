#include <immintrin.h>
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

    size_t body_end = (count / 128u) * 128u;
    for (; i < body_end; i += 128u) {
        __m256i a0 = _mm256_loadu_si256((const __m256i*)(a + i));
        __m256i b0 = _mm256_loadu_si256((const __m256i*)(b + i));
        __m256i a1 = _mm256_loadu_si256((const __m256i*)(a + i + 32));
        __m256i b1 = _mm256_loadu_si256((const __m256i*)(b + i + 32));
        __m256i a2 = _mm256_loadu_si256((const __m256i*)(a + i + 64));
        __m256i b2 = _mm256_loadu_si256((const __m256i*)(b + i + 64));
        __m256i a3 = _mm256_loadu_si256((const __m256i*)(a + i + 96));
        __m256i b3 = _mm256_loadu_si256((const __m256i*)(b + i + 96));

        __m256i c0 = _mm256_cmpeq_epi8(a0, b0);
        __m256i c1 = _mm256_cmpeq_epi8(a1, b1);
        __m256i c2 = _mm256_cmpeq_epi8(a2, b2);
        __m256i c3 = _mm256_cmpeq_epi8(a3, b3);

        if (_mm256_movemask_epi8(c0) != -1 ||
            _mm256_movemask_epi8(c1) != -1 ||
            _mm256_movemask_epi8(c2) != -1 ||
            _mm256_movemask_epi8(c3) != -1) {
            _mm256_zeroupper();
            return false;
        }
    }

    size_t vec_end = i + ((count - i) / 32u) * 32u;
    for (; i < vec_end; i += 32u) {
        __m256i av = _mm256_loadu_si256((const __m256i*)(a + i));
        __m256i bv = _mm256_loadu_si256((const __m256i*)(b + i));

        __m256i cmp = _mm256_cmpeq_epi8(av, bv);

        if (_mm256_movemask_epi8(cmp) != -1) {
            _mm256_zeroupper();
            return false;
        }
    }

    _mm256_zeroupper();

    for (; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }

    return true;
}

// --------------------------------------------------------------------------------

static inline size_t simd_count_nonzero_int8(const int8_t* data,
                                             size_t       count) {
    size_t nnz = 0u;
    size_t i = 0u;

    __m256i zero = _mm256_setzero_si256();

    size_t body_end = (count / 128u) * 128u;
    for (; i < body_end; i += 128u) {
        __m256i v0 = _mm256_loadu_si256((const __m256i*)(data + i));
        __m256i v1 = _mm256_loadu_si256((const __m256i*)(data + i + 32u));
        __m256i v2 = _mm256_loadu_si256((const __m256i*)(data + i + 64u));
        __m256i v3 = _mm256_loadu_si256((const __m256i*)(data + i + 96u));

        __m256i any = _mm256_or_si256(_mm256_or_si256(v0, v1),
                                      _mm256_or_si256(v2, v3));
        if (_mm256_testz_si256(any, any)) continue;

        unsigned m0 = (unsigned)~_mm256_movemask_epi8(_mm256_cmpeq_epi8(v0, zero));
        unsigned m1 = (unsigned)~_mm256_movemask_epi8(_mm256_cmpeq_epi8(v1, zero));
        unsigned m2 = (unsigned)~_mm256_movemask_epi8(_mm256_cmpeq_epi8(v2, zero));
        unsigned m3 = (unsigned)~_mm256_movemask_epi8(_mm256_cmpeq_epi8(v3, zero));

        nnz += (size_t)(__builtin_popcount(m0) +
                        __builtin_popcount(m1) +
                        __builtin_popcount(m2) +
                        __builtin_popcount(m3));
    }

    size_t vec_end = i + ((count - i) / 32u) * 32u;
    for (; i < vec_end; i += 32u) {
        __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
        if (_mm256_testz_si256(v, v)) continue;

        unsigned m = (unsigned)~_mm256_movemask_epi8(_mm256_cmpeq_epi8(v, zero));
        nnz += (size_t)__builtin_popcount(m);
    }

    _mm256_zeroupper();

    for (; i < count; ++i) {
        if (data[i] != 0) ++nnz;
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

    size_t body_end = (count / 128u) * 128u;
    for (; i < body_end; i += 128u) {
        __m256i any = _mm256_or_si256(
            _mm256_or_si256(
                _mm256_loadu_si256((const __m256i*)(data + i)),
                _mm256_loadu_si256((const __m256i*)(data + i + 32u))),
            _mm256_or_si256(
                _mm256_loadu_si256((const __m256i*)(data + i + 64u)),
                _mm256_loadu_si256((const __m256i*)(data + i + 96u))));

        if (!_mm256_testz_si256(any, any)) {
            _mm256_zeroupper();
            return false;
        }
    }

    size_t vec_end = i + ((count - i) / 32u) * 32u;
    for (; i < vec_end; i += 32u) {
        __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
        if (!_mm256_testz_si256(v, v)) {
            _mm256_zeroupper();
            return false;
        }
    }

    _mm256_zeroupper();

    for (; i < count; ++i) {
        if (data[i] != 0) return false;
    }

    return true;
}
