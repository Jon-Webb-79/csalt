
#ifndef CSALT_SIMD_SSE41_UINT8_INL
#define CSALT_SIMD_SSE41_UINT8_INL

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
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SSE41_UINT8_INL */

