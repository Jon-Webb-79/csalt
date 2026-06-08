// ================================================================================
// ================================================================================
// - File:    simd_sse41_lsearch_uint16.inl
// - Purpose: SSE4.1 implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements.
//
//            SSE4.1 adds _mm_test_all_zeros which can short-circuit the
//            mask check more efficiently on some microarchitectures, and
//            _mm_blendv_epi8 for branchless selection.  For a simple
//            linear search _mm_cmpeq_epi16 + _mm_movemask_epi8 from
//            SSE2 remains the primary mechanism.  The file is provided
//            as a distinct compilation unit for build system dispatch
//            when SSE4.1 is the highest available feature level.
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
#ifndef CSALT_SIMD_SSE41_DOUBLE_INL
#define CSALT_SIMD_SSE41_DOUBLE_INL

#include <smmintrin.h>   /* SSE4.1 (includes SSSE3, SSE3, SSE2) */
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
 
    const size_t elems_per_reg = 2u;
 
#if defined(__SSE4_1__)
    if (len >= elems_per_reg) {
        __m128d target   = _mm_set1_pd(value);
        __m128d tol      = _mm_set1_pd(tolerance);
        __m128d abs_mask = _mm_castsi128_pd(
                               _mm_set_epi32(0x7FFFFFFF, 0xFFFFFFFF,
                                             0x7FFFFFFF, 0xFFFFFFFF));
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128d chunk   = _mm_loadu_pd(data + i);
            __m128d diff    = _mm_sub_pd(chunk, target);
            __m128d absdiff = _mm_and_pd(diff, abs_mask);
            __m128d cmp     = _mm_cmple_pd(absdiff, tol);
            int     mask    = _mm_movemask_pd(cmp);
            if (mask != 0) {
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
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
 
#if defined(__SSE4_1__)
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
 
        __m128d hi = _mm_unpackhi_pd(vmin, vmin);
        vmin = _mm_min_pd(vmin, hi);
        _mm_store_sd(&cur_min, vmin);
 
        if (isinf(cur_min) && cur_min < 0.0) { *out = -INFINITY; return NO_ERROR; }
    }
 
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
#endif /* CSALT_SIMD_SSE41_DOUBLE_INL */

