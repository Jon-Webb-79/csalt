
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
#ifndef CSALT_SIMD_SSE2_DOUBLE_INL
#define CSALT_SIMD_SSE2_DOUBLE_INL

#include <emmintrin.h>   /* SSE2 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_double(const double* data,
                                  size_t        len,
                                  double        value,
                                  double        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 2u;   /* 128 bits / 64 bits per double */
 
#if defined(__SSE2__)
    if (len >= elems_per_reg) {
        __m128d target   = _mm_set1_pd(value);
        __m128d tol      = _mm_set1_pd(tolerance);
        /* Mask to clear the IEEE 754 sign bit of a 64-bit double */
        __m128d abs_mask = _mm_castsi128_pd(
                               _mm_set_epi32(0x7FFFFFFF, 0xFFFFFFFF,
                                             0x7FFFFFFF, 0xFFFFFFFF));
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128d chunk   = _mm_loadu_pd(data + i);
            __m128d diff    = _mm_sub_pd(chunk, target);
            __m128d absdiff = _mm_and_pd(diff, abs_mask);
            __m128d cmp     = _mm_cmple_pd(absdiff, tol);
 
            /* _mm_movemask_pd: one bit per lane, no divide needed */
            int mask = _mm_movemask_pd(cmp);
            if (mask != 0) {
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
        /* Scalar remainder for the last < 2 elements */
        for (; i < len; i++) {
            double diff = data[i] - value;
            if (diff < 0.0) diff = -diff;
            if (diff <= tolerance) return i;
        }
        return SIZE_MAX;
    }
#endif
 
    for (size_t i = 0u; i < len; i++) {
        double diff = data[i] - value;
        if (diff < 0.0) diff = -diff;
        if (diff <= tolerance) return i;
    }
    return SIZE_MAX;
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SSE2_DOUBLE_INL */

