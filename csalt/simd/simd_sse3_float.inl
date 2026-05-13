// ================================================================================
// ================================================================================
// - File:    simd_ssse3_lsearch_uint16.inl
// - Purpose: SSSE3 implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements.
//
//            SSSE3 adds no new integer comparison instructions beyond
//            SSE2 for this use case.  The implementation is identical
//            to the SSE2 version but compiled under a SSSE3 target,
//            which allows the compiler to use additional addressing
//            modes and scheduling improvements available in SSSE3
//            processors.  The file is provided as a distinct compilation
//            unit so the build system can select it when SSSE3 is the
//            highest available feature level.
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
#ifndef CSALT_SIMD_SSE3_FLOAT_INL
#define CSALT_SIMD_SSE3_FLOAT_INL

#include <tmmintrin.h>   /* SSSE3 (includes SSE2, SSE3) */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_float(const float* data,
                                 size_t       len,
                                 float        value,
                                 float        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 4u;
 
#if defined(__SSSE3__)
    if (len >= elems_per_reg) {
        __m128 target   = _mm_set1_ps(value);
        __m128 tol      = _mm_set1_ps(tolerance);
        __m128 abs_mask = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128 chunk   = _mm_loadu_ps(data + i);
            __m128 diff    = _mm_sub_ps(chunk, target);
            __m128 absdiff = _mm_and_ps(diff, abs_mask);
            __m128 cmp     = _mm_cmple_ps(absdiff, tol);
            int    mask    = _mm_movemask_ps(cmp);
            if (mask != 0) {
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
        for (; i < len; i++) {
            float diff = data[i] - value;
            if (diff < 0.0f) diff = -diff;
            if (diff <= tolerance) return i;
        }
        return SIZE_MAX;
    }
#endif
 
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
 
#if defined(__SSSE3__)
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
        if (diff != diff)     return false;
        if (diff < 0.0f)      diff = -diff;
        if (diff > tolerance) return false;
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
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SSE3_FLOAT_INL */

