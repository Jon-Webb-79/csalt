
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
#ifndef CSALT_SIMD_SSE2_FLOAT_INL
#define CSALT_SIMD_SSE2_FLOAT_INL

#include "c_error.h"

#include <emmintrin.h>   /* SSE2 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_float(const float* data,
                                 size_t       len,
                                 float        value,
                                 float        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 4u;   /* 128 bits / 32 bits per float */
 
#if defined(__SSE2__)
    if (len >= elems_per_reg) {
        __m128 target = _mm_set1_ps(value);
        __m128 tol    = _mm_set1_ps(tolerance);
        /* Mask to clear the sign bit and compute absolute value */
        __m128 abs_mask = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128 chunk   = _mm_loadu_ps(data + i);
            __m128 diff    = _mm_sub_ps(chunk, target);
            __m128 absdiff = _mm_and_ps(diff, abs_mask);
            __m128 cmp     = _mm_cmple_ps(absdiff, tol);
 
            /* _mm_movemask_ps: one bit per lane, no divide needed */
            int mask = _mm_movemask_ps(cmp);
            if (mask != 0) {
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
        /* Scalar remainder */
        for (; i < len; i++) {
            float diff = data[i] - value;
            if (diff < 0.0f) diff = -diff;
            if (diff <= tolerance) return i;
        }
        return SIZE_MAX;
    }
#endif
 
    /* Scalar fallback */
    for (size_t i = 0u; i < len; i++) {
        float diff = data[i] - value;
        if (diff < 0.0f) diff = -diff;
        if (diff <= tolerance) return i;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static bool simd_floats_equal(const float* a,
                               const float* b,
                               size_t       len,
                               float        tolerance) {
    if (a == NULL || b == NULL) return false;
    if (len == 0u)              return true;
    if (a == b)                 return true;
 
#if defined(__SSE2__)
    __m128 tol      = _mm_set1_ps(tolerance);
    __m128 abs_mask = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
 
    size_t i = 0u;
    for (; i + 4u <= len; i += 4u) {
        __m128 va      = _mm_loadu_ps(a + i);
        __m128 vb      = _mm_loadu_ps(b + i);
        __m128 diff    = _mm_sub_ps(va, vb);
        __m128 absdiff = _mm_and_ps(diff, abs_mask);
        __m128 cmp     = _mm_cmple_ps(absdiff, tol);
        if (_mm_movemask_ps(cmp) != 0xF) return false;
    }
 
    for (; i < len; i++) {
        float diff = a[i] - b[i];
        if (diff != diff)        return false;   /* NaN check */
        if (diff < 0.0f)         diff = -diff;
        if (diff > tolerance)    return false;
    }
    return true;
#endif
 
    for (size_t i = 0u; i < len; i++) {
        float diff = a[i] - b[i];
        if (diff != diff)     return false;
        if (diff < 0.0f)      diff = -diff;
        if (diff > tolerance) return false;
    }
    return true;
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_float(const float* data,
                                          size_t       len,
                                          float*       out) {
    float  cur_min = *out;               /* caller seeds with INFINITY */
    size_t i       = 0u;
 
    if (len >= 4u) {
        __m128 vmin    = _mm_set1_ps(*out);
        __m128 has_nan = _mm_setzero_ps();
 
        for (; i + 4u <= len; i += 4u) {
            __m128 v = _mm_loadu_ps(data + i);
 
            /* Track NaN: cmpunord(v, v) is -1 for NaN lanes */
            has_nan = _mm_or_ps(has_nan, _mm_cmpunord_ps(v, v));
 
            /* MINPS: skips NaN (returns non-NaN operand), but that is
             * correct for the min accumulation — NaN is caught above */
            vmin = _mm_min_ps(vmin, v);
 
            /* Early-exit on NaN detection */
            if (_mm_movemask_ps(has_nan)) {
                *out = NAN;
                return NO_ERROR;
            }
        }
 
        /* Horizontal reduction: 4 → 2 → 1 */
        __m128 shuf = _mm_movehl_ps(vmin, vmin);       /* [2,3,2,3] */
        vmin = _mm_min_ps(vmin, shuf);                  /* [min(0,2), min(1,3), ...] */
        shuf = _mm_shuffle_ps(vmin, vmin, _MM_SHUFFLE(1,1,1,1));
        vmin = _mm_min_ps(vmin, shuf);                  /* [min, ..., ..., ...] */
 
        _mm_store_ss(&cur_min, vmin);
 
        /* Check for -INFINITY after reduction */
        if (isinf(cur_min) && cur_min < 0.0f) {
            *out = -INFINITY;
            return NO_ERROR;
        }
    }
 
    /* Scalar tail */
    for (; i < len; i++) {
        if (isnan(data[i])) { *out = NAN; return NO_ERROR; }
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (isinf(cur_min) && cur_min < 0.0f) {
                *out = -INFINITY;
                return NO_ERROR;
            }
        }
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SSE2_FLOAT_INL */

