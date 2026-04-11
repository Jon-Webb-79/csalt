
#ifndef CSALT_SIMD_SCALAR_UINT8_INL
#define CSALT_SIMD_SCALAR_UINT8_INL

#include <stdint.h>
#include <stddef.h>
#include <string.h>
// ================================================================================ 
// ================================================================================ 

static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;

    size_t lo = 0u;
    size_t hi = len - 1u;

    /* Stack buffer for the swap. Sized to cover all built-in types and
       reasonably sized user structs without heap allocation. Falls back
       to a byte-by-byte swap for anything larger. */
    uint8_t tmp[256];

    if (data_size <= sizeof(tmp)) {
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;

            memcpy(tmp,     lo_ptr,  data_size);
            memcpy(lo_ptr,  hi_ptr,  data_size);
            memcpy(hi_ptr,  tmp,     data_size);

            lo++;
            hi--;
        }
    } else {
        /* data_size > 256: byte-by-byte swap to avoid VLAs */
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;

            for (size_t b = 0u; b < data_size; b++) {
                uint8_t byte   = lo_ptr[b];
                lo_ptr[b]      = hi_ptr[b];
                hi_ptr[b]      = byte;
            }

            lo++;
            hi--;
        }
    }
}
// -------------------------------------------------------------------------------- 

static size_t simd_contains_uint8(const uint8_t* data,
                                   size_t         start,
                                   size_t         end,
                                   size_t         data_size,
                                   const uint8_t* needle) {
    for (size_t i = start; i < end; i++) {
        if (memcmp(data + i * data_size, needle, data_size) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static size_t simd_min_uint8(const uint8_t* data,
                              size_t         len,
                              size_t         data_size,
                              int          (*cmp)(const void*, const void*)) {
    size_t best = 0u;
    for (size_t i = 1u; i < len; i++) {
        if (cmp(data + i * data_size, data + best * data_size) < 0)
            best = i;
    }
    return best;
}
// -------------------------------------------------------------------------------- 

static size_t simd_max_uint8(const uint8_t* data,
                              size_t         len,
                              size_t         data_size,
                              int          (*cmp)(const void*, const void*)) {
    size_t best = 0u;
    for (size_t i = 1u; i < len; i++) {
        if (cmp(data + i * data_size, data + best * data_size) > 0)
            best = i;
    }
    return best;
}
// -------------------------------------------------------------------------------- 

static void simd_sum_uint8(const uint8_t* data,
                            size_t         len,
                            size_t         data_size,
                            void*          accum,
                            void         (*add)(void* accum, const void* element)) {
    for (size_t i = 0u; i < len; i++) {
        add(accum, data + i * data_size);
    }
}
// ================================================================================ 
// ================================================================================ 

#define TRANSPOSE_TILE_SCALAR 16u
 
static inline void simd_fill_uint8(uint8_t* data, size_t count, uint8_t value) {
    memset(data, value, count);
}
// -------------------------------------------------------------------------------- 
 
static inline void simd_transpose_uint8(const uint8_t* src,
                                        uint8_t*       dst,
                                        size_t         src_rows,
                                        size_t         src_cols) {
    size_t i = 0u;
    size_t j = 0u;
 
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
 
    for (i = 0u; i < row_body; i += TRANSPOSE_TILE_SCALAR) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t ii = i; ii < i + TRANSPOSE_TILE_SCALAR; ++ii) {
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
 
#undef TRANSPOSE_TILE_SCALAR
// -------------------------------------------------------------------------------- 
 
static inline bool simd_uint8_arrays_equal(const uint8_t* a,
                                           const uint8_t* b,
                                           size_t         count) {
    if (a == NULL || b == NULL) return false;

    for (size_t i = 0u; i < count; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}
// -------------------------------------------------------------------------------- 
 
static inline size_t simd_count_nonzero_uint8(const uint8_t* data,
                                              size_t         count) {
    size_t nnz = 0u;
    for (size_t i = 0u; i < count; ++i) {
        if (data[i] != 0u) ++nnz;
    }
    return nnz;
}
// -------------------------------------------------------------------------------- 
 
static inline size_t simd_scatter_csr_row_uint8(const uint8_t* row_data,
                                                size_t         cols,
                                                size_t         col_offset,
                                                size_t*        col_idx,
                                                uint8_t*       values,
                                                size_t         k) {
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
 
static inline bool simd_is_all_zero_uint8(const uint8_t* data, size_t count) {
    for (size_t i = 0u; i < count; ++i) {
        if (data[i] != 0u) return false;
    }
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SCALAR_UINT8_INL */

