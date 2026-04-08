
#ifndef CSALT_SIMD_AVX_UINT8_INL
#define CSALT_SIMD_AVX_UINT8_INL

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <immintrin.h>   /* AVX */
// ================================================================================ 
// ================================================================================ 

static inline __m256i _avx_reverse_bytes(__m256i v) {
    const __m128i byte_mask = _mm_set_epi8(
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    );

    /* Extract and reverse each 128-bit lane independently */
    __m128i lo128 = _mm256_extractf128_si256(v, 0);
    __m128i hi128 = _mm256_extractf128_si256(v, 1);

    lo128 = _mm_shuffle_epi8(lo128, byte_mask);
    hi128 = _mm_shuffle_epi8(hi128, byte_mask);

    /* Reassemble with lanes swapped: old-hi becomes new-lo, old-lo becomes new-hi */
    __m256i result = _mm256_castsi128_si256(hi128);
    result = _mm256_insertf128_si256(result, lo128, 1);
    return result;
}

// ================================================================================
// Public interface
// ================================================================================

static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;

    if (data_size < 32u && (32u % data_size == 0u)) {
        size_t lo = 0u;
        size_t hi = len - 1u;
        size_t elems_per_reg = 32u / data_size;

        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;

            if ((hi - lo + 1u) >= 2u * elems_per_reg) {
                __m256i vlo = _mm256_loadu_si256((__m256i*)lo_ptr);
                __m256i vhi = _mm256_loadu_si256((__m256i*)hi_ptr);

                vlo = _avx_reverse_bytes(vlo);
                vhi = _avx_reverse_bytes(vhi);

                _mm256_storeu_si256((__m256i*)hi_ptr, vlo);
                _mm256_storeu_si256((__m256i*)lo_ptr, vhi);

                lo += elems_per_reg;
                hi -= elems_per_reg;
                continue;
            }

            /* Scalar remainder */
            uint8_t tmp[32];
            memcpy(tmp,    lo_ptr, data_size);
            memcpy(lo_ptr, hi_ptr, data_size);
            memcpy(hi_ptr, tmp,    data_size);
            lo++;
            hi--;
        }

        _mm256_zeroupper();
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
    _mm256_zeroupper();
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Internal: broadcast needle pattern into a 128-bit register
// ================================================================================

static inline __m128i _avx_broadcast128(const uint8_t* needle, size_t data_size) {
    uint8_t buf[16] = {0};
    for (size_t i = 0; i < 16; i++) {
        buf[i] = needle[i % data_size];
    }
    return _mm_loadu_si128((__m128i*)buf);
}

// ================================================================================
// Internal: find first matching element within a 32-byte chunk
// mask is a 32-bit value combining two 16-bit movemask results.
// ================================================================================

static inline size_t _avx_first_match32(int    mask,
                                         size_t data_size,
                                         size_t elem_base,
                                         size_t end) {
    int    elem_mask     = (data_size >= 16) ? -1 : (1 << (int)data_size) - 1;
    size_t elems_per_reg = 32u / data_size;

    for (size_t e = 0; e < elems_per_reg; e++) {
        int    shift = (int)(e * data_size);
        int    got   = (mask >> shift) & elem_mask;
        size_t idx   = elem_base + e;
        if (idx >= end) break;
        if (got == elem_mask) return idx;
    }
    return SIZE_MAX;
}

// ================================================================================
// Public interface
// ================================================================================

static size_t simd_contains_uint8(const uint8_t* data,
                                   size_t         start,
                                   size_t         end,
                                   size_t         data_size,
                                   const uint8_t* needle) {
    if (data_size <= 16u && (16u % data_size == 0u)) {
        size_t  elems_per_reg = 32u / data_size;
        __m128i vn128         = _avx_broadcast128(needle, data_size);
        size_t  i             = start;

        while (i + elems_per_reg <= end) {
            const uint8_t* ptr = data + i * data_size;

            /* Load as two 128-bit halves — AVX has no 256-bit integer cmpeq */
            __m128i lo   = _mm_loadu_si128((__m128i*)ptr);
            __m128i hi   = _mm_loadu_si128((__m128i*)(ptr + 16));
            int     mlo  = _mm_movemask_epi8(_mm_cmpeq_epi8(lo, vn128));
            int     mhi  = _mm_movemask_epi8(_mm_cmpeq_epi8(hi, vn128));
            int     mask = mlo | (mhi << 16);

            if (mask != 0) {
                size_t found = _avx_first_match32(mask, data_size, i, end);
                if (found != SIZE_MAX) {
                    _mm256_zeroupper();
                    return found;
                }
            }
            i += elems_per_reg;
        }

        /* Scalar remainder */
        _mm256_zeroupper();
        for (; i < end; i++) {
            if (memcmp(data + i * data_size, needle, data_size) == 0) {
                return i;
            }
        }
        return SIZE_MAX;
    }

    /* Scalar fallback */
    for (size_t i = start; i < end; i++) {
        if (memcmp(data + i * data_size, needle, data_size) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline uint8_t _avx_hmin128_u8(__m128i v) {
    v = _mm_min_epu8(v, _mm_srli_si128(v, 8));
    v = _mm_min_epu8(v, _mm_srli_si128(v, 4));
    v = _mm_min_epu8(v, _mm_srli_si128(v, 2));
    v = _mm_min_epu8(v, _mm_srli_si128(v, 1));
    return (uint8_t)_mm_cvtsi128_si32(v);
}

static inline uint8_t _avx_hmax128_u8(__m128i v) {
    v = _mm_max_epu8(v, _mm_srli_si128(v, 8));
    v = _mm_max_epu8(v, _mm_srli_si128(v, 4));
    v = _mm_max_epu8(v, _mm_srli_si128(v, 2));
    v = _mm_max_epu8(v, _mm_srli_si128(v, 1));
    return (uint8_t)_mm_cvtsi128_si32(v);
}

static size_t simd_min_uint8(const uint8_t* data,
                              size_t         len,
                              size_t         data_size,
                              int          (*cmp)(const void*, const void*)) {
    if (data_size == 1u) {
        __m128i vmin = _mm_set1_epi8((char)0xFF);
        size_t  i    = 0u;
        while (i + 32u <= len) {
            __m256i chunk = _mm256_loadu_si256((__m256i*)(data + i));
            __m128i lo    = _mm256_castsi256_si128(chunk);
            __m128i hi    = _mm256_extractf128_si256(chunk, 1);
            vmin = _mm_min_epu8(vmin, _mm_min_epu8(lo, hi));
            i += 32u;
        }
        if (i + 16u <= len) {
            vmin = _mm_min_epu8(vmin, _mm_loadu_si128((__m128i*)(data + i)));
            i += 16u;
        }
        for (; i < len; i++) {
            __m128i v = _mm_set1_epi8((char)data[i]);
            vmin = _mm_min_epu8(vmin, v);
        }
        uint8_t min_val = _avx_hmin128_u8(vmin);
        _mm256_zeroupper();
        for (size_t j = 0u; j < len; j++)
            if (data[j] == min_val) return j;
    }
    _mm256_zeroupper();
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
        while (i + 32u <= len) {
            __m256i chunk = _mm256_loadu_si256((__m256i*)(data + i));
            __m128i lo    = _mm256_castsi256_si128(chunk);
            __m128i hi    = _mm256_extractf128_si256(chunk, 1);
            vmax = _mm_max_epu8(vmax, _mm_max_epu8(lo, hi));
            i += 32u;
        }
        if (i + 16u <= len) {
            vmax = _mm_max_epu8(vmax, _mm_loadu_si128((__m128i*)(data + i)));
            i += 16u;
        }
        for (; i < len; i++) {
            __m128i v = _mm_set1_epi8((char)data[i]);
            vmax = _mm_max_epu8(vmax, v);
        }
        uint8_t max_val = _avx_hmax128_u8(vmax);
        _mm256_zeroupper();
        for (size_t j = 0u; j < len; j++)
            if (data[j] == max_val) return j;
    }
    _mm256_zeroupper();
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
        while (i + 32u <= len) {
            __m256i chunk = _mm256_loadu_si256((__m256i*)(data + i));
            __m128i lo    = _mm256_castsi256_si128(chunk);
            __m128i hi    = _mm256_extractf128_si256(chunk, 1);
            uint8_t v;
            v=(uint8_t)_mm_extract_epi8(lo, 0);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo, 1);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo, 2);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo, 3);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo, 4);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo, 5);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo, 6);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo, 7);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo, 8);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo, 9);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo,10);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo,11);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo,12);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo,13);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo,14);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(lo,15);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 0);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 1);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 2);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 3);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 4);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 5);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 6);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 7);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 8);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi, 9);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi,10);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi,11);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi,12);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi,13);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi,14);add(accum,&v);
            v=(uint8_t)_mm_extract_epi8(hi,15);add(accum,&v);
            i += 32u;
        }
    } else if (data_size == 2u) {
        while (i + 16u <= len) {
            __m256i chunk = _mm256_loadu_si256((__m256i*)(data + i * 2u));
            __m128i lo    = _mm256_castsi256_si128(chunk);
            __m128i hi    = _mm256_extractf128_si256(chunk, 1);
            uint16_t v;
            v=(uint16_t)_mm_extract_epi16(lo,0);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(lo,1);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(lo,2);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(lo,3);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(lo,4);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(lo,5);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(lo,6);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(lo,7);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(hi,0);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(hi,1);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(hi,2);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(hi,3);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(hi,4);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(hi,5);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(hi,6);add(accum,&v);
            v=(uint16_t)_mm_extract_epi16(hi,7);add(accum,&v);
            i += 16u;
        }
    } else if (data_size == 4u) {
        while (i + 8u <= len) {
            __m256i chunk = _mm256_loadu_si256((__m256i*)(data + i * 4u));
            __m128i lo    = _mm256_castsi256_si128(chunk);
            __m128i hi    = _mm256_extractf128_si256(chunk, 1);
            uint32_t v;
            v=(uint32_t)_mm_extract_epi32(lo,0);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(lo,1);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(lo,2);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(lo,3);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(hi,0);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(hi,1);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(hi,2);add(accum,&v);
            v=(uint32_t)_mm_extract_epi32(hi,3);add(accum,&v);
            i += 8u;
        }
    } else if (data_size == 8u) {
        while (i + 4u <= len) {
            __m256i chunk = _mm256_loadu_si256((__m256i*)(data + i * 8u));
            __m128i lo    = _mm256_castsi256_si128(chunk);
            __m128i hi    = _mm256_extractf128_si256(chunk, 1);
            uint64_t v;
            v=(uint64_t)_mm_extract_epi64(lo,0);add(accum,&v);
            v=(uint64_t)_mm_extract_epi64(lo,1);add(accum,&v);
            v=(uint64_t)_mm_extract_epi64(hi,0);add(accum,&v);
            v=(uint64_t)_mm_extract_epi64(hi,1);add(accum,&v);
            i += 4u;
        }
    }
    _mm256_zeroupper();
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
 
static inline bool simd_equal_uint8(const uint8_t* a,
                                    const uint8_t* b,
                                    size_t         count) {
    size_t i = 0u;
 
    /* Load 32 bytes as float256, XOR, extract halves, test with SSE2 */
    size_t body_end = (count / 128u) * 128u;
    for (; i < body_end; i += 128u) {
        __m256 x0 = _mm256_xor_ps(
            _mm256_loadu_ps((const float*)(a + i)),
            _mm256_loadu_ps((const float*)(b + i)));
        __m256 x1 = _mm256_xor_ps(
            _mm256_loadu_ps((const float*)(a + i + 32u)),
            _mm256_loadu_ps((const float*)(b + i + 32u)));
        __m256 x2 = _mm256_xor_ps(
            _mm256_loadu_ps((const float*)(a + i + 64u)),
            _mm256_loadu_ps((const float*)(b + i + 64u)));
        __m256 x3 = _mm256_xor_ps(
            _mm256_loadu_ps((const float*)(a + i + 96u)),
            _mm256_loadu_ps((const float*)(b + i + 96u)));
 
        __m256 any = _mm256_or_ps(_mm256_or_ps(x0, x1),
                                  _mm256_or_ps(x2, x3));
 
        __m128i lo = _mm_castps_si128(_mm256_castps256_ps128(any));
        __m128i hi = _mm_castps_si128(_mm256_extractf128_ps(any, 1));
        __m128i combined = _mm_or_si128(lo, hi);
 
        if (_mm_movemask_epi8(
                _mm_cmpeq_epi8(combined, _mm_setzero_si128())) != 0xFFFF) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    _mm256_zeroupper();
 
    /* SSE2 tail for remaining 16-byte chunks */
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        __m128i x = _mm_xor_si128(
            _mm_loadu_si128((const __m128i*)(a + i)),
            _mm_loadu_si128((const __m128i*)(b + i)));
 
        if (_mm_movemask_epi8(
                _mm_cmpeq_epi8(x, _mm_setzero_si128())) != 0xFFFF)
            return false;
    }
 
    for (; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }
 
    return true;
}
// -------------------------------------------------------------------------------- 
 
static inline size_t simd_count_nonzero_uint8(const uint8_t* data,
                                              size_t         count) {
    /* No 256-bit integer cmpeq in AVX — use SSE2 path */
    size_t nnz = 0u;
    size_t i = 0u;
    __m128i zero = _mm_setzero_si128();
 
    size_t body_end = (count / 64u) * 64u;
    for (; i < body_end; i += 64u) {
        int m0 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i)), zero)) & 0xFFFF;
        int m1 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i + 16u)), zero)) & 0xFFFF;
        int m2 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i + 32u)), zero)) & 0xFFFF;
        int m3 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i + 48u)), zero)) & 0xFFFF;
 
        nnz += (size_t)(__builtin_popcount((unsigned)m0) +
                        __builtin_popcount((unsigned)m1) +
                        __builtin_popcount((unsigned)m2) +
                        __builtin_popcount((unsigned)m3));
    }
 
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        int m = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i)), zero)) & 0xFFFF;
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
 
    /* Load as float256, extract to 128, test with SSE2 */
    size_t body_end = (count / 128u) * 128u;
    for (; i < body_end; i += 128u) {
        __m256 v0 = _mm256_loadu_ps((const float*)(data + i));
        __m256 v1 = _mm256_loadu_ps((const float*)(data + i + 32u));
        __m256 v2 = _mm256_loadu_ps((const float*)(data + i + 64u));
        __m256 v3 = _mm256_loadu_ps((const float*)(data + i + 96u));
 
        __m256 any = _mm256_or_ps(_mm256_or_ps(v0, v1),
                                  _mm256_or_ps(v2, v3));
 
        __m128i lo = _mm_castps_si128(_mm256_castps256_ps128(any));
        __m128i hi = _mm_castps_si128(_mm256_extractf128_ps(any, 1));
        __m128i combined = _mm_or_si128(lo, hi);
 
        if (_mm_movemask_epi8(
                _mm_cmpeq_epi8(combined, _mm_setzero_si128())) != 0xFFFF) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    _mm256_zeroupper();
 
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(
                _mm_loadu_si128((const __m128i*)(data + i)),
                _mm_setzero_si128())) != 0xFFFF)
            return false;
    }
 
    for (; i < count; ++i) {
        if (data[i] != 0u) return false;
    }
 
    return true;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_AVX_UINT8_INL */

