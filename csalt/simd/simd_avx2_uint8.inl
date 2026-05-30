
#ifndef CSALT_SIMD_AVX2_UINT8_INL
#define CSALT_SIMD_AVX2_UINT8_INL

#include "c_error.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <immintrin.h>   /* AVX2 */
// ================================================================================ 
// ================================================================================ 

/**
 * Reverse element order within a 256-bit AVX2 register without disturbing
 * the internal byte order of each element.
 *
 * _mm256_shuffle_epi8 operates independently on each 128-bit lane, so the
 * mask reverses element order within each lane, and then vperm2x128 swaps
 * the two lanes to complete the full 32-byte reversal.
 */
static inline __m256i _avx2_reverse_elems(__m256i v, size_t data_size) {
    __m256i mask;
    switch (data_size) {
        case 1u:
            /* 32 bytes, fully reversed across both lanes */
            mask = _mm256_set_epi8(
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15,
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15);
            break;
        case 2u:
            /* 16 uint16_t elements, reversed — bytes within each pair preserved */
            mask = _mm256_set_epi8(
                 1,  0,  3,  2,  5,  4,  7,  6,
                 9,  8, 11, 10, 13, 12, 15, 14,
                 1,  0,  3,  2,  5,  4,  7,  6,
                 9,  8, 11, 10, 13, 12, 15, 14);
            break;
        case 4u:
            /* 8 uint32_t / float elements, reversed */
            mask = _mm256_set_epi8(
                 3,  2,  1,  0,  7,  6,  5,  4,
                11, 10,  9,  8, 15, 14, 13, 12,
                 3,  2,  1,  0,  7,  6,  5,  4,
                11, 10,  9,  8, 15, 14, 13, 12);
            break;
        case 8u:
            /* 4 uint64_t / double elements, reversed */
            mask = _mm256_set_epi8(
                 7,  6,  5,  4,  3,  2,  1,  0,
                15, 14, 13, 12, 11, 10,  9,  8,
                 7,  6,  5,  4,  3,  2,  1,  0,
                15, 14, 13, 12, 11, 10,  9,  8);
            break;
        default:
            return v;   /* caller must not reach here */
    }
 
    /* Reverse elements within each 128-bit lane */
    v = _mm256_shuffle_epi8(v, mask);
 
    /* Swap the two 128-bit lanes: 0x01 swaps lane 0 and lane 1 */
    return _mm256_permute2x128_si256(v, v, 0x01);
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;
 
    /* AVX2 path: valid for element sizes that evenly divide 32 bytes
     * and have a defined shuffle mask above. */
    if (data_size <= 8u &&
        (32u % data_size == 0u) &&
        data_size != 3u &&
        data_size != 5u &&
        data_size != 6u &&
        data_size != 7u) {
 
        size_t lo            = 0u;
        size_t hi            = len - 1u;
        size_t elems_per_reg = 32u / data_size;
        bool   used_avx      = false;
 
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
 
            if ((hi - lo + 1u) >= 2u * elems_per_reg) {
                __m256i vlo = _mm256_loadu_si256((__m256i*)lo_ptr);
                __m256i vhi = _mm256_loadu_si256((__m256i*)hi_ptr);
                vlo = _avx2_reverse_elems(vlo, data_size);
                vhi = _avx2_reverse_elems(vhi, data_size);
                _mm256_storeu_si256((__m256i*)hi_ptr, vlo);
                _mm256_storeu_si256((__m256i*)lo_ptr, vhi);
                lo += elems_per_reg;
                hi -= elems_per_reg;
                used_avx = true;
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
 
        if (used_avx) _mm256_zeroupper();
        return;
    }
 
    /* Scalar fallback — correct for any data_size */
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

static inline error_code_t simd_min_uint8(const uint8_t* data,
                                          size_t         len,
                                          uint8_t*       out) {
    uint8_t cur_min = *out;
    size_t  i       = 0u;
 
    if (len >= 32u) {
        __m256i vmin = _mm256_set1_epi8((char)0xFF);
 
        for (; i + 32u <= len; i += 32u) {
            __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
            vmin = _mm256_min_epu8(vmin, v);
        }
 
        /* Fold 256 → 128 */
        __m128i lo  = _mm256_castsi256_si128(vmin);
        __m128i hi  = _mm256_extracti128_si256(vmin, 1);
        __m128i v16 = _mm_min_epu8(lo, hi);
 
        /* Horizontal reduction: 16 → 8 → 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(v16, 8);
        v16 = _mm_min_epu8(v16, shifted);
        shifted = _mm_srli_si128(v16, 4);
        v16 = _mm_min_epu8(v16, shifted);
        shifted = _mm_srli_si128(v16, 2);
        v16 = _mm_min_epu8(v16, shifted);
        shifted = _mm_srli_si128(v16, 1);
        v16 = _mm_min_epu8(v16, shifted);
 
        uint8_t lane_min = (uint8_t)_mm_extract_epi16(v16, 0) & 0xFFu;
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Handle 16-byte chunk if remaining tail >= 16 */
    if (i + 16u <= len) {
        __m128i vmin = _mm_set1_epi8((char)cur_min);
        __m128i v    = _mm_loadu_si128((const __m128i*)(data + i));
        vmin = _mm_min_epu8(vmin, v);
        i += 16u;
 
        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _mm_min_epu8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _mm_min_epu8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 2);
        vmin = _mm_min_epu8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 1);
        vmin = _mm_min_epu8(vmin, shifted);
 
        uint8_t lane_min = (uint8_t)_mm_extract_epi16(vmin, 0) & 0xFFu;
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Scalar tail */
    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
        }
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_AVX2_UINT8_INL */

