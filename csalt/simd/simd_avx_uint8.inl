
#ifndef CSALT_SIMD_AVX_UINT8_INL
#define CSALT_SIMD_AVX_UINT8_INL

#include "c_error.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <immintrin.h>   /* AVX */
// ================================================================================ 
// ================================================================================ 

/**
 * Reverse element order within a 256-bit AVX register by:
 *   1. Applying a 128-bit per-lane pshufb to reverse elements within each lane.
 *   2. Swapping the two 128-bit lanes with vperm2f128.
 *
 * The shuffle mask is the same 128-bit pattern applied to both lanes.
 * Internal byte order of each element is preserved.
 */
static inline __m256i _avx_reverse_elems(__m256i v, size_t data_size) {
    __m128i mask128;
    switch (data_size) {
        case 1u:
            mask128 = _mm_set_epi8(
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15);
            break;
        case 2u:
            mask128 = _mm_set_epi8(
                 1,  0,  3,  2,  5,  4,  7,  6,
                 9,  8, 11, 10, 13, 12, 15, 14);
            break;
        case 4u:
            mask128 = _mm_set_epi8(
                 3,  2,  1,  0,  7,  6,  5,  4,
                11, 10,  9,  8, 15, 14, 13, 12);
            break;
        case 8u:
            mask128 = _mm_set_epi8(
                 7,  6,  5,  4,  3,  2,  1,  0,
                15, 14, 13, 12, 11, 10,  9,  8);
            break;
        default:
            return v;
    }
 
    /* Broadcast 128-bit mask to both lanes of a 256-bit mask register */
    __m256i mask256 = _mm256_broadcastsi128_si256(mask128);
 
    /* Apply pshufb per 128-bit lane via the 256-bit cast */
    /* AVX does not have _mm256_shuffle_epi8 — use the float register to
     * perform the lane operations via 128-bit extracts and inserts.     */
    __m128i lo_lane = _mm256_extractf128_si256(v, 0);
    __m128i hi_lane = _mm256_extractf128_si256(v, 1);
 
    lo_lane = _mm_shuffle_epi8(lo_lane, mask128);
    hi_lane = _mm_shuffle_epi8(hi_lane, mask128);
 
    /* Recombine with lanes swapped: old hi becomes new lo, old lo becomes hi */
    __m256i result = _mm256_castsi128_si256(hi_lane);
    result = _mm256_insertf128_si256(result, lo_lane, 1);
 
    (void)mask256;   /* suppress unused warning — used conceptually above */
    return result;
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;
 
    /* AVX path: 32-byte registers, element sizes with defined masks only */
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
                vlo = _avx_reverse_elems(vlo, data_size);
                vhi = _avx_reverse_elems(vhi, data_size);
                _mm256_storeu_si256((__m256i*)hi_ptr, vlo);
                _mm256_storeu_si256((__m256i*)lo_ptr, vhi);
                lo += elems_per_reg;
                hi -= elems_per_reg;
                used_avx = true;
                continue;
            }
 
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

static inline error_code_t simd_min_uint8(const uint8_t* data,
                                          size_t         len,
                                          uint8_t*       out) {
    uint8_t cur_min = *out;
    size_t  i       = 0u;
 
    /* AVX has no 256-bit integer min, fall back to 128-bit SSE2 path */
    if (len >= 16u) {
        __m128i vmin = _mm_set1_epi8((char)0xFF);
 
        for (; i + 16u <= len; i += 16u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _mm_min_epu8(vmin, v);
        }
 
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
#endif /* CSALT_SIMD_AVX_UINT8_INL */

