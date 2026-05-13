// ================================================================================
// ================================================================================
// - File:    simd_avx_lsearch_uint16.inl
// - Purpose: AVX (non-AVX2) implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements.
//
//            Plain AVX provides 256-bit floating-point registers but
//            256-bit integer operations are not available until AVX2.
//            This implementation therefore uses two 128-bit SSE2 passes
//            per 256-byte-wide loop iteration, processing 16 uint16_t
//            elements per iteration (2 x 8) via the lower 128-bit lanes
//            of the AVX registers.  This avoids the AVX-SSE transition
//            penalty while still amortising the loop overhead over more
//            elements than the pure SSE2 version.
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
#ifndef CSALT_SIMD_AVX2_FLOAT_INL
#define CSALT_SIMD_AVX2_FLOAT_INL

#include <immintrin.h>   /* AVX2 (includes AVX) */
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
 
    const size_t elems_per_reg = 8u;
 
#if defined(__AVX2__)
    if (len >= elems_per_reg) {
        __m256 target   = _mm256_set1_ps(value);
        __m256 tol      = _mm256_set1_ps(tolerance);
        __m256 abs_mask = _mm256_castsi256_ps(
                              _mm256_set1_epi32(0x7FFFFFFF));
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m256 chunk   = _mm256_loadu_ps(data + i);
            __m256 diff    = _mm256_sub_ps(chunk, target);
            __m256 absdiff = _mm256_and_ps(diff, abs_mask);
            __m256 cmp     = _mm256_cmp_ps(absdiff, tol, _CMP_LE_OQ);
            int    mask    = _mm256_movemask_ps(cmp);
            if (mask != 0) {
                _mm256_zeroupper();
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
        /* Handle remaining full SSE2 chunks (4 elements) */
        __m128 target128   = _mm_set1_ps(value);
        __m128 tol128      = _mm_set1_ps(tolerance);
        __m128 abs_mask128 = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
 
        for (; i + 4u <= len; i += 4u) {
            __m128 chunk   = _mm_loadu_ps(data + i);
            __m128 diff    = _mm_sub_ps(chunk, target128);
            __m128 absdiff = _mm_and_ps(diff, abs_mask128);
            __m128 cmp     = _mm_cmple_ps(absdiff, tol128);
            int    mask    = _mm_movemask_ps(cmp);
            if (mask != 0) {
                _mm256_zeroupper();
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
        _mm256_zeroupper();
 
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
 
#if defined(__AVX2__)
    __m256 tol256 = _mm256_set1_ps(tolerance);
    __m256 abs256 = _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF));
    __m128 tol128 = _mm_set1_ps(tolerance);
    __m128 abs128 = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
 
    size_t i = 0u;
    for (; i + 8u <= len; i += 8u) {
        __m256 va      = _mm256_loadu_ps(a + i);
        __m256 vb      = _mm256_loadu_ps(b + i);
        __m256 diff    = _mm256_sub_ps(va, vb);
        __m256 absdiff = _mm256_and_ps(diff, abs256);
        __m256 cmp     = _mm256_cmp_ps(absdiff, tol256, _CMP_LE_OQ);
        if (_mm256_movemask_ps(cmp) != 0xFF) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    for (; i + 4u <= len; i += 4u) {
        __m128 va      = _mm_loadu_ps(a + i);
        __m128 vb      = _mm_loadu_ps(b + i);
        __m128 diff    = _mm_sub_ps(va, vb);
        __m128 absdiff = _mm_and_ps(diff, abs128);
        __m128 cmp     = _mm_cmple_ps(absdiff, tol128);
        if (_mm_movemask_ps(cmp) != 0xF) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    _mm256_zeroupper();
 
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
#endif /* CSALT_SIMD_AVX2_FLOAT_INL */

