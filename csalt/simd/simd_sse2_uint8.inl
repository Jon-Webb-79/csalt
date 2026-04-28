
#ifndef CSALT_SIMD_SSE2_UINT8_INL
#define CSALT_SIMD_SSE2_UINT8_INL

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <emmintrin.h>   /* SSE2 */
// ================================================================================ 
// ================================================================================ 

/**
 * Reverse 16 individual bytes within a 128-bit SSE2 register.
 * Only valid when data_size == 1.
 * SSE2 has no pshufb so the reversal is performed by unpacking
 * bytes into 16-bit lanes, shuffling the lanes, and repacking.
 */
static inline __m128i _sse2_reverse_bytes_1(__m128i v) {
    __m128i zero = _mm_setzero_si128();
    __m128i lo16 = _mm_unpacklo_epi8(v, zero);
    __m128i hi16 = _mm_unpackhi_epi8(v, zero);
 
    lo16 = _mm_shufflelo_epi16(lo16, _MM_SHUFFLE(0, 1, 2, 3));
    lo16 = _mm_shufflehi_epi16(lo16, _MM_SHUFFLE(0, 1, 2, 3));
    hi16 = _mm_shufflelo_epi16(hi16, _MM_SHUFFLE(0, 1, 2, 3));
    hi16 = _mm_shufflehi_epi16(hi16, _MM_SHUFFLE(0, 1, 2, 3));
 
    __m128i swapped_lo = _mm_unpacklo_epi64(hi16, lo16);
    __m128i swapped_hi = _mm_unpackhi_epi64(hi16, lo16);
 
    return _mm_packus_epi16(swapped_lo, swapped_hi);
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;
 
    /* SSE2 SIMD path: only valid for data_size == 1 with enough elements
     * to fill at least two registers.  All other element sizes and smaller
     * arrays fall through to the scalar path which is always correct. */
    if (data_size == 1u && len >= 32u) {
        const size_t elems_per_reg = 16u;
 
        /* lo and hi point to the start of each register-width chunk.
         * hi starts at the first element of the last full chunk. */
        size_t lo = 0u;
        size_t hi = len - elems_per_reg;
 
        while (lo + elems_per_reg <= hi) {
            uint8_t* lo_ptr = data + lo;
            uint8_t* hi_ptr = data + hi;
 
            __m128i vlo = _mm_loadu_si128((__m128i*)lo_ptr);
            __m128i vhi = _mm_loadu_si128((__m128i*)hi_ptr);
            vlo = _sse2_reverse_bytes_1(vlo);
            vhi = _sse2_reverse_bytes_1(vhi);
            _mm_storeu_si128((__m128i*)hi_ptr, vlo);
            _mm_storeu_si128((__m128i*)lo_ptr, vhi);
 
            lo += elems_per_reg;
            hi -= elems_per_reg;
        }
 
        /* Odd number of chunks — reverse the middle chunk in place */
        if (lo == hi) {
            uint8_t* mid_ptr = data + lo;
            __m128i  vmid    = _mm_loadu_si128((__m128i*)mid_ptr);
            vmid = _sse2_reverse_bytes_1(vmid);
            _mm_storeu_si128((__m128i*)mid_ptr, vmid);
            lo += elems_per_reg;
        }
 
        /* Scalar remainder for any elements between the converging
         * chunk boundaries that did not fill a complete register pair */
        size_t scalar_lo = lo;
        size_t scalar_hi = (hi >= elems_per_reg) ? hi - 1u : 0u;
        while (scalar_lo < scalar_hi) {
            uint8_t tmp      = data[scalar_lo];
            data[scalar_lo]  = data[scalar_hi];
            data[scalar_hi]  = tmp;
            scalar_lo++;
            scalar_hi--;
        }
        return;
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
#endif /* CSALT_SIMD_SSE2_UINT8_INL */

