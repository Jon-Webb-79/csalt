
// ================================================================================
// - File:    simd_sse2_lsearch_uint16.inl
// - Purpose: SSE2 implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements using SSE2 128-bit integer comparisons.
//
//            SSE2 provides _mm_cmpeq_epi16 which compares 8 uint16_t
//            lanes simultaneously.  _mm_movemask_epi8 extracts a 16-bit
//            byte mask from the comparison result; the position of the
//            first set bit divided by 2 gives the element index within
//            the current chunk.
//
//            Returns the zero-based index of the first match, or
//            SIZE_MAX if the value is not found.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 27, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
#ifndef CSALT_SIMD_SSE2_UINT16_INL
#define CSALT_SIMD_SSE2_UINT16_INL
// ================================================================================ 
// ================================================================================ 

#include <emmintrin.h>   /* SSE2 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint16(const uint16_t* data,
                                  size_t          len,
                                  uint16_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 8u;   /* 128 bits / 16 bits per uint16_t */
 
#if defined(__SSE2__)
    if (len >= elems_per_reg) {
        __m128i target = _mm_set1_epi16((int16_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _mm_cmpeq_epi16(chunk, target);
 
            /* movemask produces one bit per byte — 16 bits for 16 bytes.
             * Each uint16_t match sets two consecutive bits. */
            int mask = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                /* __builtin_ctz finds lowest set bit (byte offset).
                 * Divide by 2 to convert byte offset to element offset. */
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 2u);
            }
        }
 
        /* Scalar remainder for the last < 8 elements */
        for (; i < len; i++) {
            if (data[i] == value) return i;
        }
        return SIZE_MAX;
    }
#endif
 
    /* Scalar fallback for short arrays or when SSE2 is unavailable */
    for (size_t i = 0u; i < len; i++) {
        if (data[i] == value) return i;
    }
    return SIZE_MAX;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SSE2_UINT16_INL */

