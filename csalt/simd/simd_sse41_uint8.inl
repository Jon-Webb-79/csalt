
#ifndef CSALT_SIMD_SSE41_UINT8_INL
#define CSALT_SIMD_SSE41_UINT8_INL

#include "c_error.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <smmintrin.h>
// ================================================================================ 
// ================================================================================ 

/**
 * Reverse element order within a 128-bit register without disturbing the
 * internal byte order of each element.
 */
static inline __m128i _sse41_reverse_elems(__m128i v, size_t data_size) {
    __m128i mask;
    switch (data_size) {
        case 1u:
            mask = _mm_set_epi8(
                 0,  1,  2,  3,  4,  5,  6,  7,
                 8,  9, 10, 11, 12, 13, 14, 15);
            break;
        case 2u:
            mask = _mm_set_epi8(
                 1,  0,  3,  2,  5,  4,  7,  6,
                 9,  8, 11, 10, 13, 12, 15, 14);
            break;
        case 4u:
            mask = _mm_set_epi8(
                 3,  2,  1,  0,  7,  6,  5,  4,
                11, 10,  9,  8, 15, 14, 13, 12);
            break;
        case 8u:
            mask = _mm_set_epi8(
                 7,  6,  5,  4,  3,  2,  1,  0,
                15, 14, 13, 12, 11, 10,  9,  8);
            break;
        default:
            return v;
    }
    return _mm_shuffle_epi8(v, mask);
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;

    if (data_size <= 8u &&
        (16u % data_size == 0u) &&
        data_size != 3u &&
        data_size != 5u &&
        data_size != 6u &&
        data_size != 7u) {

        size_t elems_per_reg = 16u / data_size;

        /* lo and hi are both register-chunk indices, not element indices.
         * Each advances by elems_per_reg per SIMD iteration. */
        if (len >= 2u * elems_per_reg) {
            size_t lo = 0u;
            size_t hi = len - elems_per_reg;

            while (lo + elems_per_reg <= hi) {
                uint8_t* lo_ptr = data + lo * data_size;
                uint8_t* hi_ptr = data + hi * data_size;

                __m128i vlo = _mm_loadu_si128((__m128i*)lo_ptr);
                __m128i vhi = _mm_loadu_si128((__m128i*)hi_ptr);
                vlo = _sse41_reverse_elems(vlo, data_size);
                vhi = _sse41_reverse_elems(vhi, data_size);
                _mm_storeu_si128((__m128i*)hi_ptr, vlo);
                _mm_storeu_si128((__m128i*)lo_ptr, vhi);

                lo += elems_per_reg;
                hi -= elems_per_reg;
            }

            /* If an odd number of chunks remain, handle the middle chunk */
            if (lo == hi) {
                uint8_t* mid_ptr = data + lo * data_size;
                __m128i  vmid    = _mm_loadu_si128((__m128i*)mid_ptr);
                vmid = _sse41_reverse_elems(vmid, data_size);
                _mm_storeu_si128((__m128i*)mid_ptr, vmid);
            }
            return;
        }

        /* Fewer elements than one register — fall through to scalar */
    }

    /* Scalar fallback — correct for any data_size */
    size_t  lo = 0u;
    size_t  hi = len - 1u;
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
 
    if (len >= 16u) {
        __m128i vmin = _mm_set1_epi8((char)0xFF);
 
        for (; i + 16u <= len; i += 16u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _mm_min_epu8(vmin, v);
        }
 
        /* Fold 16 × uint8 → 8 × uint16 by taking pairwise min of adjacent
         * bytes, then use PHMINPOSUW to find the minimum uint16 lane.
         * The minimum byte value is preserved because min(a, b) <= a and
         * min(a, b) <= b, so the smallest byte survives both stages. */
        __m128i lo = vmin;
        __m128i hi = _mm_srli_epi16(vmin, 8);
        __m128i folded = _mm_min_epu8(lo, hi);         /* 8 relevant bytes in low byte of each u16 lane */
        __m128i masked = _mm_and_si128(folded, _mm_set1_epi16(0x00FF));
        __m128i minpos = _mm_minpos_epu16(masked);     /* result in bits [15:0] */
 
        uint8_t lane_min = (uint8_t)(_mm_extract_epi16(minpos, 0) & 0xFFu);
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
#endif /* CSALT_SIMD_SSE41_UINT8_INL */

