
#ifndef CSALT_SIMD_SSE3_UINT8_INL
#define CSALT_SIMD_SSE3_UINT8_INL

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <tmmintrin.h>   /* SSSE3 */
// ================================================================================ 
// ================================================================================ 

/**
 * Reverse element order within a 128-bit SSSE3 register without disturbing
 * the internal byte order of each element.
 *
 * Each mask treats groups of data_size bytes as atomic units and places
 * them in reversed order within the 16-byte register.
 */
static inline __m128i _ssse3_reverse_elems(__m128i v, size_t data_size) {
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
 
    /* SSSE3 path: valid for element sizes that evenly divide 16 bytes,
     * have a defined shuffle mask above, and have enough elements to
     * fill at least two registers. */
    if (data_size <= 8u &&
        (16u % data_size == 0u) &&
        data_size != 3u &&
        data_size != 5u &&
        data_size != 6u &&
        data_size != 7u) {
 
        size_t elems_per_reg = 16u / data_size;
 
        if (len >= 2u * elems_per_reg) {
            /* lo and hi point to the start of each register-width chunk.
             * hi starts at the first element of the last full chunk. */
            size_t lo = 0u;
            size_t hi = len - elems_per_reg;
 
            while (lo + elems_per_reg <= hi) {
                uint8_t* lo_ptr = data + lo * data_size;
                uint8_t* hi_ptr = data + hi * data_size;
 
                __m128i vlo = _mm_loadu_si128((__m128i*)lo_ptr);
                __m128i vhi = _mm_loadu_si128((__m128i*)hi_ptr);
                vlo = _ssse3_reverse_elems(vlo, data_size);
                vhi = _ssse3_reverse_elems(vhi, data_size);
                _mm_storeu_si128((__m128i*)hi_ptr, vlo);
                _mm_storeu_si128((__m128i*)lo_ptr, vhi);
 
                lo += elems_per_reg;
                hi -= elems_per_reg;
            }
 
            /* Odd number of chunks — reverse the middle chunk in place */
            if (lo == hi) {
                uint8_t* mid_ptr = data + lo * data_size;
                __m128i  vmid    = _mm_loadu_si128((__m128i*)mid_ptr);
                vmid = _ssse3_reverse_elems(vmid, data_size);
                _mm_storeu_si128((__m128i*)mid_ptr, vmid);
                lo += elems_per_reg;
            }
 
            /* Scalar remainder for any elements between the converging
             * chunk boundaries that did not fill a complete register pair */
            size_t scalar_lo = lo;
            size_t scalar_hi = (hi >= elems_per_reg) ? hi - 1u : 0u;
            uint8_t tmp[16];
            while (scalar_lo < scalar_hi) {
                uint8_t* slo_ptr = data + scalar_lo * data_size;
                uint8_t* shi_ptr = data + scalar_hi * data_size;
                memcpy(tmp,     slo_ptr, data_size);
                memcpy(slo_ptr, shi_ptr, data_size);
                memcpy(shi_ptr, tmp,     data_size);
                scalar_lo++;
                scalar_hi--;
            }
            return;
        }
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
#endif /* CSALT_SIMD_SSE3_UINT8_INL */

