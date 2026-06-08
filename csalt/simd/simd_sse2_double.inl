
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
#include <stdbool.h>
#include <float.h>
#include <math.h>
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
// -------------------------------------------------------------------------------- 

static bool simd_doubles_equal(const double* a,
                                const double* b,
                                size_t        len,
                                double        tolerance) {
    if (a == NULL || b == NULL) return false;
    if (len == 0u)              return true;
    if (a == b)                 return true;
 
#if defined(__SSE2__)
    __m128d tol      = _mm_set1_pd(tolerance);
    __m128d abs_mask = _mm_castsi128_pd(
                           _mm_set_epi32(0x7FFFFFFF, 0xFFFFFFFF,
                                         0x7FFFFFFF, 0xFFFFFFFF));
 
    size_t i = 0u;
    for (; i + 2u <= len; i += 2u) {
        __m128d va      = _mm_loadu_pd(a + i);
        __m128d vb      = _mm_loadu_pd(b + i);
        __m128d diff    = _mm_sub_pd(va, vb);
        __m128d absdiff = _mm_and_pd(diff, abs_mask);
        __m128d cmp     = _mm_cmple_pd(absdiff, tol);
        if (_mm_movemask_pd(cmp) != 0x3) return false;
    }
 
    for (; i < len; i++) {
        double diff = a[i] - b[i];
        if (diff != diff)     return false;
        if (diff < 0.0)       diff = -diff;
        if (diff > tolerance) return false;
    }
    return true;
#endif
 
    for (size_t i = 0u; i < len; i++) {
        double diff = a[i] - b[i];
        if (diff != diff)     return false;
        if (diff < 0.0)       diff = -diff;
        if (diff > tolerance) return false;
    }
    return true;
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_double(const double* data,
                                           size_t        len,
                                           double*       out) {
    double cur_min = *out;
    size_t i       = 0u;
 
    if (len >= 2u) {
        __m128d vmin    = _mm_set1_pd(*out);
        __m128d has_nan = _mm_setzero_pd();
 
        for (; i + 2u <= len; i += 2u) {
            __m128d v = _mm_loadu_pd(data + i);
            has_nan = _mm_or_pd(has_nan, _mm_cmpunord_pd(v, v));
            vmin = _mm_min_pd(vmin, v);
            if (_mm_movemask_pd(has_nan)) { *out = NAN; return NO_ERROR; }
        }
 
        /* Horizontal reduction: 2 → 1 */
        __m128d hi = _mm_unpackhi_pd(vmin, vmin);
        vmin = _mm_min_pd(vmin, hi);
        _mm_store_sd(&cur_min, vmin);
 
        if (isinf(cur_min) && cur_min < 0.0) { *out = -INFINITY; return NO_ERROR; }
    }
 
    /* Scalar tail (at most 1 element) */
    for (; i < len; i++) {
        if (isnan(data[i])) { *out = NAN; return NO_ERROR; }
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (isinf(cur_min) && cur_min < 0.0) { *out = -INFINITY; return NO_ERROR; }
        }
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SSE2_DOUBLE_INL */

