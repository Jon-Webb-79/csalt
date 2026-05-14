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
#ifndef CSALT_SIMD_AVX2_DOUBLE_INL
#define CSALT_SIMD_AVX2_DOUBLE_INL

#include <immintrin.h>   /* AVX2 (includes AVX) */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_double(const double* data,
                                  size_t        len,
                                  double        value,
                                  double        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 4u;
 
#if defined(__AVX2__)
    if (len >= elems_per_reg) {
        __m256d target   = _mm256_set1_pd(value);
        __m256d tol      = _mm256_set1_pd(tolerance);
        __m256d abs_mask = _mm256_castsi256_pd(
                               _mm256_set_epi32(
                                   0x7FFFFFFF, 0xFFFFFFFF,
                                   0x7FFFFFFF, 0xFFFFFFFF,
                                   0x7FFFFFFF, 0xFFFFFFFF,
                                   0x7FFFFFFF, 0xFFFFFFFF));
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m256d chunk   = _mm256_loadu_pd(data + i);
            __m256d diff    = _mm256_sub_pd(chunk, target);
            __m256d absdiff = _mm256_and_pd(diff, abs_mask);
            __m256d cmp     = _mm256_cmp_pd(absdiff, tol, _CMP_LE_OQ);
            int     mask    = _mm256_movemask_pd(cmp);
            if (mask != 0) {
                _mm256_zeroupper();
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
        /* Handle remaining full SSE2 chunks (2 elements) */
        __m128d target128   = _mm_set1_pd(value);
        __m128d tol128      = _mm_set1_pd(tolerance);
        __m128d abs_mask128 = _mm_castsi128_pd(
                                  _mm_set_epi32(0x7FFFFFFF, 0xFFFFFFFF,
                                                0x7FFFFFFF, 0xFFFFFFFF));
        for (; i + 2u <= len; i += 2u) {
            __m128d chunk   = _mm_loadu_pd(data + i);
            __m128d diff    = _mm_sub_pd(chunk, target128);
            __m128d absdiff = _mm_and_pd(diff, abs_mask128);
            __m128d cmp     = _mm_cmple_pd(absdiff, tol128);
            int     mask    = _mm_movemask_pd(cmp);
            if (mask != 0) {
                _mm256_zeroupper();
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
        _mm256_zeroupper();
 
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
}
// -------------------------------------------------------------------------------- 

static bool simd_doubles_equal(const double* a,
                                const double* b,
                                size_t        len,
                                double        tolerance) {
    if (a == NULL || b == NULL) return false;
    if (len == 0u)              return true;
    if (a == b)                 return true;
 
#if defined(__AVX2__)
    __m256d tol256 = _mm256_set1_pd(tolerance);
    __m256d abs256 = _mm256_castsi256_pd(
                         _mm256_set_epi32(
                             0x7FFFFFFF, 0xFFFFFFFF,
                             0x7FFFFFFF, 0xFFFFFFFF,
                             0x7FFFFFFF, 0xFFFFFFFF,
                             0x7FFFFFFF, 0xFFFFFFFF));
    __m128d tol128 = _mm_set1_pd(tolerance);
    __m128d abs128 = _mm_castsi128_pd(
                         _mm_set_epi32(0x7FFFFFFF, 0xFFFFFFFF,
                                       0x7FFFFFFF, 0xFFFFFFFF));
 
    size_t i = 0u;
    for (; i + 4u <= len; i += 4u) {
        __m256d va      = _mm256_loadu_pd(a + i);
        __m256d vb      = _mm256_loadu_pd(b + i);
        __m256d diff    = _mm256_sub_pd(va, vb);
        __m256d absdiff = _mm256_and_pd(diff, abs256);
        __m256d cmp     = _mm256_cmp_pd(absdiff, tol256, _CMP_LE_OQ);
        if (_mm256_movemask_pd(cmp) != 0xF) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    for (; i + 2u <= len; i += 2u) {
        __m128d va      = _mm_loadu_pd(a + i);
        __m128d vb      = _mm_loadu_pd(b + i);
        __m128d diff    = _mm_sub_pd(va, vb);
        __m128d absdiff = _mm_and_pd(diff, abs128);
        __m128d cmp     = _mm_cmple_pd(absdiff, tol128);
        if (_mm_movemask_pd(cmp) != 0x3) {
            _mm256_zeroupper();
            return false;
        }
    }
 
    _mm256_zeroupper();
 
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
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_AVX2_DOUBLE_INL */

