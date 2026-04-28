
#ifndef CSALT_SIMD_AVX512_UINT8_INL
#define CSALT_SIMD_AVX512_UINT8_INL

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <immintrin.h>   /* AVX-512 */

// ================================================================================ 
// ================================================================================ 

/**
 * Reverse element order within a 512-bit AVX-512 register without disturbing
 * the internal byte order of each element.
 *
 * _mm512_shuffle_epi8 operates on each of the four 128-bit lanes
 * independently.  The shuffle mask reverses element order within each lane,
 * then _mm512_permutexvar_epi64 reorders the four 128-bit lanes (treated as
 * 8 x 64-bit chunks) to complete the full reversal.
 */
static inline __m512i _avx512_reverse_elems(__m512i v, size_t data_size) {
    __m512i mask;
    switch (data_size) {
        case 1u:
            /* 64 bytes, reversed — same 128-bit pattern broadcast to all lanes */
            mask = _mm512_set_epi8(
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15,
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15,
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15,
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15);
            break;
        case 2u:
            mask = _mm512_set_epi8(
                 1,  0,  3,  2,  5,  4,  7,  6,
                 9,  8, 11, 10, 13, 12, 15, 14,
                 1,  0,  3,  2,  5,  4,  7,  6,
                 9,  8, 11, 10, 13, 12, 15, 14,
                 1,  0,  3,  2,  5,  4,  7,  6,
                 9,  8, 11, 10, 13, 12, 15, 14,
                 1,  0,  3,  2,  5,  4,  7,  6,
                 9,  8, 11, 10, 13, 12, 15, 14);
            break;
        case 4u:
            mask = _mm512_set_epi8(
                 3,  2,  1,  0,  7,  6,  5,  4,
                11, 10,  9,  8, 15, 14, 13, 12,
                 3,  2,  1,  0,  7,  6,  5,  4,
                11, 10,  9,  8, 15, 14, 13, 12,
                 3,  2,  1,  0,  7,  6,  5,  4,
                11, 10,  9,  8, 15, 14, 13, 12,
                 3,  2,  1,  0,  7,  6,  5,  4,
                11, 10,  9,  8, 15, 14, 13, 12);
            break;
        case 8u:
            mask = _mm512_set_epi8(
                 7,  6,  5,  4,  3,  2,  1,  0,
                15, 14, 13, 12, 11, 10,  9,  8,
                 7,  6,  5,  4,  3,  2,  1,  0,
                15, 14, 13, 12, 11, 10,  9,  8,
                 7,  6,  5,  4,  3,  2,  1,  0,
                15, 14, 13, 12, 11, 10,  9,  8,
                 7,  6,  5,  4,  3,  2,  1,  0,
                15, 14, 13, 12, 11, 10,  9,  8);
            break;
        case 16u:
            /* 4 x 128-bit elements — swap bytes within each element then
             * swap the four lanes.  No shuffle needed for 16-byte elements
             * since the lane-swap alone suffices if we treat the lanes as
             * atomic; but for correctness we still reverse within each lane
             * so that bytes 0-15 map to positions 15-0. */
            mask = _mm512_set_epi8(
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15,
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15,
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15,
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15);
            break;
        default:
            return v;
    }
 
    /* Reverse elements within each 128-bit lane */
    v = _mm512_shuffle_epi8(v, mask);
 
    /* Reorder the four 128-bit lanes (as 8 x 64-bit words) to swap lanes
     * 0↔3 and 1↔2: lane indices in 64-bit word order are 7,6,5,4,3,2,1,0 */
    __m512i lane_idx = _mm512_set_epi64(0, 1, 2, 3, 4, 5, 6, 7);
    return _mm512_permutexvar_epi64(lane_idx, v);
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;
 
    /* AVX-512 path: 64-byte registers, element sizes with defined masks */
    if (data_size <= 16u &&
        (64u % data_size == 0u) &&
        data_size != 3u  &&
        data_size != 5u  &&
        data_size != 6u  &&
        data_size != 7u  &&
        data_size != 9u  &&
        data_size != 10u &&
        data_size != 11u &&
        data_size != 12u &&
        data_size != 13u &&
        data_size != 14u &&
        data_size != 15u) {
 
        size_t lo            = 0u;
        size_t hi            = len - 1u;
        size_t elems_per_reg = 64u / data_size;
 
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
 
            if ((hi - lo + 1u) >= 2u * elems_per_reg) {
                __m512i vlo = _mm512_loadu_si512((__m512i*)lo_ptr);
                __m512i vhi = _mm512_loadu_si512((__m512i*)hi_ptr);
                vlo = _avx512_reverse_elems(vlo, data_size);
                vhi = _avx512_reverse_elems(vhi, data_size);
                _mm512_storeu_si512((__m512i*)hi_ptr, vlo);
                _mm512_storeu_si512((__m512i*)lo_ptr, vhi);
                lo += elems_per_reg;
                hi -= elems_per_reg;
                continue;
            }
 
            /* Scalar remainder */
            uint8_t tmp[64];
            memcpy(tmp,    lo_ptr, data_size);
            memcpy(lo_ptr, hi_ptr, data_size);
            memcpy(hi_ptr, tmp,    data_size);
            lo++;
            hi--;
        }
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
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_AVX512_UINT8_INL */

