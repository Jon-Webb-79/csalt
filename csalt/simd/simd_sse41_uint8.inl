
#ifndef CSALT_SIMD_SSE41_UINT8_INL
#define CSALT_SIMD_SSE41_UINT8_INL

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <smmintrin.h>
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Internal: reverse bytes within a 128-bit register using SSE2 only
//
// Strategy: treat the 16 bytes as 8 x uint16_t, shuffle the 16-bit words
// into reverse order, then byte-swap each 16-bit word.
// ================================================================================

static inline __m128i _sse41_reverse_bytes(__m128i v) {
    const __m128i mask = _mm_set_epi8(
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    );
    return _mm_shuffle_epi8(v, mask);
}

// ================================================================================
// Public interface
// ================================================================================

static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;

    if (data_size < 16u && (16u % data_size == 0u)) {
        size_t lo = 0u;
        size_t hi = len - 1u;
        size_t elems_per_reg = 16u / data_size;

        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;

            if ((hi - lo + 1u) >= 2u * elems_per_reg) {
                __m128i vlo = _mm_loadu_si128((__m128i*)lo_ptr);
                __m128i vhi = _mm_loadu_si128((__m128i*)hi_ptr);

                vlo = _sse41_reverse_bytes(vlo);
                vhi = _sse41_reverse_bytes(vhi);

                _mm_storeu_si128((__m128i*)hi_ptr, vlo);
                _mm_storeu_si128((__m128i*)lo_ptr, vhi);

                lo += elems_per_reg;
                hi -= elems_per_reg;
                continue;
            }

            uint8_t tmp[16];
            memcpy(tmp,    lo_ptr, data_size);
            memcpy(lo_ptr, hi_ptr, data_size);
            memcpy(hi_ptr, tmp,    data_size);
            lo++;
            hi--;
        }
        return;
    }

    /* Scalar fallback */
    size_t lo = 0u;
    size_t hi = len - 1u;
    uint8_t tmp[256];

    if (data_size <= sizeof(tmp)) {
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
            memcpy(tmp,    lo_ptr, data_size);
            memcpy(lo_ptr, hi_ptr, data_size);
            memcpy(hi_ptr, tmp,    data_size);
            lo++; hi--;
        }
    } else {
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
            for (size_t b = 0u; b < data_size; b++) {
                uint8_t byte = lo_ptr[b];
                lo_ptr[b]    = hi_ptr[b];
                hi_ptr[b]    = byte;
            }
            lo++; hi--;
        }
    }
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Public interface
// ================================================================================

static size_t simd_contains_uint8(const uint8_t* data,
                                   size_t         start,
                                   size_t         end,
                                   size_t         data_size,
                                   const uint8_t* needle) {
    size_t i = start;

    if (data_size == 1u) {
        /* Broadcast single byte across all 16 lanes */
        __m128i vneedle = _mm_set1_epi8((char)*needle);
        while (i + 16u <= end) {
            __m128i chunk = _mm_loadu_si128((__m128i*)(data + i));
            int     mask  = _mm_movemask_epi8(_mm_cmpeq_epi8(chunk, vneedle));
            if (mask != 0) {
                /* __builtin_ctz gives position of lowest set bit */
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
            i += 16u;
        }
    } else if (data_size == 2u) {
        __m128i vneedle = _mm_set1_epi16(*(const int16_t*)needle);
        while (i + 8u <= end) {
            __m128i chunk = _mm_loadu_si128((__m128i*)(data + i * 2u));
            /* cmpeq_epi16 sets each 16-bit lane to 0xFFFF on match */
            __m128i cmp   = _mm_cmpeq_epi16(chunk, vneedle);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                /* Two bits set per matching element; find first pair */
                for (size_t e = 0; e < 8u && i + e < end; e++) {
                    if (((mask >> (int)(e * 2u)) & 0x3) == 0x3) return i + e;
                }
            }
            i += 8u;
        }
    } else if (data_size == 4u) {
        __m128i vneedle = _mm_set1_epi32(*(const int32_t*)needle);
        while (i + 4u <= end) {
            __m128i chunk = _mm_loadu_si128((__m128i*)(data + i * 4u));
            __m128i cmp   = _mm_cmpeq_epi32(chunk, vneedle);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                /* Four bits per element */
                for (size_t e = 0; e < 4u && i + e < end; e++) {
                    if (((mask >> (int)(e * 4u)) & 0xF) == 0xF) return i + e;
                }
            }
            i += 4u;
        }
    } else if (data_size == 8u) {
        /* _mm_cmpeq_epi64 is SSE4.1 */
        __m128i vneedle = _mm_set1_epi64x(*(const int64_t*)needle);
        while (i + 2u <= end) {
            __m128i chunk = _mm_loadu_si128((__m128i*)(data + i * 8u));
            __m128i cmp   = _mm_cmpeq_epi64(chunk, vneedle);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                if ((mask & 0x00FF) == 0x00FF && i     < end) return i;
                if ((mask & 0xFF00) == 0xFF00 && i + 1 < end) return i + 1u;
            }
            i += 2u;
        }
    }

    /* Scalar remainder for all sizes */
    for (; i < end; i++) {
        if (memcmp(data + i * data_size, needle, data_size) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline uint8_t _sse41_hmin_u8(__m128i v) {
    __m128i lo8  = _mm_and_si128(v, _mm_set1_epi16(0x00FF));
    __m128i hi8  = _mm_srli_epi16(v, 8);
    __m128i min8 = _mm_min_epu8(lo8, hi8);
    __m128i pos  = _mm_minpos_epu16(min8);
    return (uint8_t)_mm_cvtsi128_si32(pos);
}

static inline uint8_t _sse41_hmax_u8(__m128i v) {
    __m128i inv = _mm_xor_si128(v, _mm_set1_epi8((char)0xFF));
    return (uint8_t)(0xFF ^ _sse41_hmin_u8(inv));
}

static size_t simd_min_uint8(const uint8_t* data,
                              size_t         len,
                              size_t         data_size,
                              int          (*cmp)(const void*, const void*)) {
    if (data_size == 1u) {
        __m128i vmin = _mm_set1_epi8((char)0xFF);
        size_t  i    = 0u;
        while (i + 16u <= len) {
            vmin = _mm_min_epu8(vmin, _mm_loadu_si128((__m128i*)(data + i)));
            i += 16u;
        }
        for (; i < len; i++) {
            __m128i v = _mm_set1_epi8((char)data[i]);
            vmin = _mm_min_epu8(vmin, v);
        }
        uint8_t min_val = _sse41_hmin_u8(vmin);
        for (size_t j = 0u; j < len; j++)
            if (data[j] == min_val) return j;
    }
    size_t best = 0u;
    for (size_t i = 1u; i < len; i++)
        if (cmp(data + i * data_size, data + best * data_size) < 0) best = i;
    return best;
}
// -------------------------------------------------------------------------------- 

static size_t simd_max_uint8(const uint8_t* data,
                              size_t         len,
                              size_t         data_size,
                              int          (*cmp)(const void*, const void*)) {
    if (data_size == 1u) {
        __m128i vmax = _mm_setzero_si128();
        size_t  i    = 0u;
        while (i + 16u <= len) {
            vmax = _mm_max_epu8(vmax, _mm_loadu_si128((__m128i*)(data + i)));
            i += 16u;
        }
        for (; i < len; i++) {
            __m128i v = _mm_set1_epi8((char)data[i]);
            vmax = _mm_max_epu8(vmax, v);
        }
        uint8_t max_val = _sse41_hmax_u8(vmax);
        for (size_t j = 0u; j < len; j++)
            if (data[j] == max_val) return j;
    }
    size_t best = 0u;
    for (size_t i = 1u; i < len; i++)
        if (cmp(data + i * data_size, data + best * data_size) > 0) best = i;
    return best;
}
// -------------------------------------------------------------------------------- 

static void simd_sum_uint8(const uint8_t* data,
                            size_t         len,
                            size_t         data_size,
                            void*          accum,
                            void         (*add)(void* accum, const void* element)) {
    size_t i = 0u;
    if (data_size == 1u) {
        while (i + 16u <= len) {
            __m128i chunk = _mm_loadu_si128((__m128i*)(data + i));
            uint8_t v;
            v=(uint8_t)_mm_extract_epi8(chunk, 0);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk, 1);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk, 2);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk, 3);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk, 4);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk, 5);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk, 6);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk, 7);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk, 8);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk, 9);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk,10);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk,11);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk,12);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk,13);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk,14);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(chunk,15);add(accum,&v);
            i += 16u;
        }
    } else if (data_size == 2u) {
        while (i + 8u <= len) {
            __m128i chunk = _mm_loadu_si128((__m128i*)(data + i * 2u));
            uint16_t v;
            v=(uint16_t)_mm_extract_epi16(chunk,0);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(chunk,1);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(chunk,2);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(chunk,3);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(chunk,4);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(chunk,5);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(chunk,6);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(chunk,7);add(accum,&v);
            i += 8u;
        }
    } else if (data_size == 4u) {
        while (i + 4u <= len) {
            __m128i chunk = _mm_loadu_si128((__m128i*)(data + i * 4u));
            uint32_t v;
            v=(uint32_t)_mm_extract_epi32(chunk,0);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(chunk,1);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(chunk,2);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(chunk,3);add(accum,&v);
            i += 4u;
        }
    } else if (data_size == 8u) {
        while (i + 2u <= len) {
            __m128i chunk = _mm_loadu_si128((__m128i*)(data + i * 8u));
            uint64_t v;
            v=(uint64_t)_mm_extract_epi64(chunk,0);add(accum,&v);
            v=(uint64_t)_mm_extract_epi64(chunk,1);add(accum,&v);
            i += 2u;
        }
    }
    for (; i < len; i++) add(accum, data + i * data_size);
}
// ================================================================================ 
// ================================================================================ 

static inline void simd_fill_uint8(uint8_t* data, size_t count, uint8_t value) {
    memset(data, value, count);
}
// -------------------------------------------------------------------------------- 
 
static inline void simd_transpose_uint8(const uint8_t* src,
                                        uint8_t*       dst,
                                        size_t         src_rows,
                                        size_t         src_cols) {
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
 
static inline bool simd_uint8_arrays_equal(const uint8_t* a,
                                           const uint8_t* b,
                                           size_t         count) {
    if (a == NULL || b == NULL) return false;

    size_t i = 0u;
    size_t vec_end = (count / 16u) * 16u;

    for (; i < vec_end; i += 16u) {
        __m128i av = _mm_loadu_si128((const __m128i*)(a + i));
        __m128i bv = _mm_loadu_si128((const __m128i*)(b + i));

        __m128i cmp = _mm_cmpeq_epi8(av, bv);

        if (_mm_movemask_epi8(cmp) != 0xFFFF) {
            return false;
        }
    }

    for (; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }

    return true;
}
// -------------------------------------------------------------------------------- 
 
static inline size_t simd_count_nonzero_uint8(const uint8_t* data,
                                              size_t         count) {
    size_t nnz = 0u;
    size_t i = 0u;
    __m128i zero = _mm_setzero_si128();
 
    size_t body_end = (count / 64u) * 64u;
    for (; i < body_end; i += 64u) {
        __m128i v0 = _mm_loadu_si128((const __m128i*)(data + i));
        __m128i v1 = _mm_loadu_si128((const __m128i*)(data + i + 16u));
        __m128i v2 = _mm_loadu_si128((const __m128i*)(data + i + 32u));
        __m128i v3 = _mm_loadu_si128((const __m128i*)(data + i + 48u));
 
        /* Fast all-zero skip */
        __m128i any = _mm_or_si128(_mm_or_si128(v0, v1),
                                   _mm_or_si128(v2, v3));
        if (_mm_testz_si128(any, any)) continue;
 
        int m0 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(v0, zero)) & 0xFFFF;
        int m1 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(v1, zero)) & 0xFFFF;
        int m2 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(v2, zero)) & 0xFFFF;
        int m3 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(v3, zero)) & 0xFFFF;
 
        nnz += (size_t)(__builtin_popcount((unsigned)m0) +
                        __builtin_popcount((unsigned)m1) +
                        __builtin_popcount((unsigned)m2) +
                        __builtin_popcount((unsigned)m3));
    }
 
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
        if (_mm_testz_si128(v, v)) continue;
 
        int m = ~_mm_movemask_epi8(_mm_cmpeq_epi8(v, zero)) & 0xFFFF;
        nnz += (size_t)__builtin_popcount((unsigned)m);
    }
 
    for (; i < count; ++i) {
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
    size_t i = 0u;
 
    size_t body_end = (count / 64u) * 64u;
    for (; i < body_end; i += 64u) {
        __m128i any = _mm_or_si128(
            _mm_or_si128(
                _mm_loadu_si128((const __m128i*)(data + i)),
                _mm_loadu_si128((const __m128i*)(data + i + 16u))),
            _mm_or_si128(
                _mm_loadu_si128((const __m128i*)(data + i + 32u)),
                _mm_loadu_si128((const __m128i*)(data + i + 48u))));
 
        if (!_mm_testz_si128(any, any)) return false;
    }
 
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
        if (!_mm_testz_si128(v, v)) return false;
    }
 
    for (; i < count; ++i) {
        if (data[i] != 0u) return false;
    }
 
    return true;
}
// -------------------------------------------------------------------------------- 

static inline void simd_add_scalar_uint8(uint8_t*      data,
                                         size_t        len,
                                         size_t        data_size,
                                         const void*   scalar,
                                         void        (*add_scalar)(void* element,
                                                                   const void* scalar)) {
    size_t i = 0u;

    switch (data_size) {
        case 1u: {
            const __m128i s = _mm_set1_epi8((char)(*(const uint8_t*)scalar));
            size_t vec_end = (len / 16u) * 16u;
            for (; i < vec_end; i += 16u) {
                __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
                v = _mm_add_epi8(v, s);
                _mm_storeu_si128((__m128i*)(data + i), v);
            }
            for (; i < len; ++i) ((uint8_t*)data)[i] += *(const uint8_t*)scalar;
            return;
        }

        case 2u: {
            uint16_t* d = (uint16_t*)data;
            const __m128i s = _mm_set1_epi16((short)(*(const uint16_t*)scalar));
            size_t vec_end = (len / 8u) * 8u;
            for (; i < vec_end; i += 8u) {
                __m128i v = _mm_loadu_si128((const __m128i*)(d + i));
                v = _mm_add_epi16(v, s);
                _mm_storeu_si128((__m128i*)(d + i), v);
            }
            for (; i < len; ++i) d[i] += *(const uint16_t*)scalar;
            return;
        }

        case 4u: {
            uint32_t* d = (uint32_t*)data;
            const __m128i s = _mm_set1_epi32((int)(*(const uint32_t*)scalar));
            size_t vec_end = (len / 4u) * 4u;
            for (; i < vec_end; i += 4u) {
                __m128i v = _mm_loadu_si128((const __m128i*)(d + i));
                v = _mm_add_epi32(v, s);
                _mm_storeu_si128((__m128i*)(d + i), v);
            }
            for (; i < len; ++i) d[i] += *(const uint32_t*)scalar;
            return;
        }

        case 8u: {
            uint64_t* d = (uint64_t*)data;
            const __m128i s = _mm_set1_epi64x((long long)(*(const uint64_t*)scalar));
            size_t vec_end = (len / 2u) * 2u;
            for (; i < vec_end; i += 2u) {
                __m128i v = _mm_loadu_si128((const __m128i*)(d + i));
                v = _mm_add_epi64(v, s);
                _mm_storeu_si128((__m128i*)(d + i), v);
            }
            for (; i < len; ++i) d[i] += *(const uint64_t*)scalar;
            return;
        }

        default:
            for (i = 0u; i < len; ++i) add_scalar(data + i * data_size, scalar);
            return;
    }
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SSE41_UINT8_INL */

