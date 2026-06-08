// ================================================================================
// ================================================================================
// - File:    simd_avx512_lsearch_uint16.inl
// - Purpose: AVX-512 implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements using AVX-512BW 512-bit integer comparisons.
//
//            AVX-512BW provides _mm512_cmpeq_epi16_mask which compares
//            32 uint16_t lanes simultaneously and writes the result
//            directly into a 32-bit k-register mask, eliminating the
//            movemask step required by AVX2.  _tzcnt_u32 on the mask
//            gives the element index of the first match directly without
//            the divide-by-2 needed for byte masks.
//
//            Requires: AVX-512F + AVX-512BW
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
#ifndef CSALT_SIMD_AVX512_DOUBLE_INL
#define CSALT_SIMD_AVX512_DOUBLE_INL

#include <immintrin.h>   /* AVX-512 */
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
 
    const size_t elems_per_reg = 8u;   /* 512 bits / 64 bits per double */
 
#if defined(__AVX512F__)
    if (len >= elems_per_reg) {
        __m512d target    = _mm512_set1_pd(value);
        __m512d tol       = _mm512_set1_pd(tolerance);
        __m512i sign_mask = _mm512_set1_epi64((int64_t)0x7FFFFFFFFFFFFFFF);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m512d chunk   = _mm512_loadu_pd(data + i);
            __m512d diff    = _mm512_sub_pd(chunk, target);
            /* Absolute value via sign-bit clear using integer AND */
            __m512d absdiff = _mm512_castsi512_pd(
                                  _mm512_and_epi64(
                                      _mm512_castpd_si512(diff),
                                      sign_mask));
            /* k-register: bit N set if |data[i+N] - value| <= tol */
            __mmask8 mask   = _mm512_cmp_pd_mask(absdiff, tol, _CMP_LE_OQ);
            if (mask != 0u) {
                return i + (size_t)_tzcnt_u32((uint32_t)mask);
            }
        }
 
        /* Handle remaining full AVX chunks (4 elements) */
        __m256d target256   = _mm256_set1_pd(value);
        __m256d tol256      = _mm256_set1_pd(tolerance);
        __m256d abs256      = _mm256_castsi256_pd(
                                  _mm256_set_epi32(
                                      0x7FFFFFFF, 0xFFFFFFFF,
                                      0x7FFFFFFF, 0xFFFFFFFF,
                                      0x7FFFFFFF, 0xFFFFFFFF,
                                      0x7FFFFFFF, 0xFFFFFFFF));
        for (; i + 4u <= len; i += 4u) {
            __m256d chunk   = _mm256_loadu_pd(data + i);
            __m256d diff    = _mm256_sub_pd(chunk, target256);
            __m256d absdiff = _mm256_and_pd(diff, abs256);
            __m256d cmp     = _mm256_cmp_pd(absdiff, tol256, _CMP_LE_OQ);
            int     mask    = _mm256_movemask_pd(cmp);
            if (mask != 0) {
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
        /* Handle remaining full SSE2 chunks (2 elements) */
        __m128d target128   = _mm_set1_pd(value);
        __m128d tol128      = _mm_set1_pd(tolerance);
        __m128d abs128      = _mm_castsi128_pd(
                                  _mm_set_epi32(0x7FFFFFFF, 0xFFFFFFFF,
                                                0x7FFFFFFF, 0xFFFFFFFF));
        for (; i + 2u <= len; i += 2u) {
            __m128d chunk   = _mm_loadu_pd(data + i);
            __m128d diff    = _mm_sub_pd(chunk, target128);
            __m128d absdiff = _mm_and_pd(diff, abs128);
            __m128d cmp     = _mm_cmple_pd(absdiff, tol128);
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
 
#if defined(__AVX512F__)
    __m512d tol512  = _mm512_set1_pd(tolerance);
    __m512i sign512 = _mm512_set1_epi64((int64_t)0x7FFFFFFFFFFFFFFF);
    __m256d tol256  = _mm256_set1_pd(tolerance);
    __m256d abs256  = _mm256_castsi256_pd(
                          _mm256_set_epi32(
                              0x7FFFFFFF, 0xFFFFFFFF,
                              0x7FFFFFFF, 0xFFFFFFFF,
                              0x7FFFFFFF, 0xFFFFFFFF,
                              0x7FFFFFFF, 0xFFFFFFFF));
    __m128d tol128  = _mm_set1_pd(tolerance);
    __m128d abs128  = _mm_castsi128_pd(
                          _mm_set_epi32(0x7FFFFFFF, 0xFFFFFFFF,
                                        0x7FFFFFFF, 0xFFFFFFFF));
 
    size_t i = 0u;
    for (; i + 8u <= len; i += 8u) {
        __m512d  va      = _mm512_loadu_pd(a + i);
        __m512d  vb      = _mm512_loadu_pd(b + i);
        __m512d  diff    = _mm512_sub_pd(va, vb);
        __m512d  absdiff = _mm512_castsi512_pd(
                               _mm512_and_epi64(
                                   _mm512_castpd_si512(diff), sign512));
        __mmask8 mask    = _mm512_cmp_pd_mask(absdiff, tol512, _CMP_LE_OQ);
        if (mask != (uint8_t)0xFF) return false;
    }
 
    for (; i + 4u <= len; i += 4u) {
        __m256d va      = _mm256_loadu_pd(a + i);
        __m256d vb      = _mm256_loadu_pd(b + i);
        __m256d diff    = _mm256_sub_pd(va, vb);
        __m256d absdiff = _mm256_and_pd(diff, abs256);
        __m256d cmp     = _mm256_cmp_pd(absdiff, tol256, _CMP_LE_OQ);
        if (_mm256_movemask_pd(cmp) != 0xF) return false;
    }
 
    for (; i + 2u <= len; i += 2u) {
        __m128d va      = _mm_loadu_pd(a + i);
        __m128d vb      = _mm_loadu_pd(b + i);
        __m128d diff    = _mm_sub_pd(va, vb);
        __m128d absdiff = _mm_and_pd(diff, abs128);
        __m128d cmp     = _mm_cmple_pd(absdiff, tol128);
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
 
    if (len >= 8u) {
        __m512d  vmin    = _mm512_set1_pd(*out);
        __mmask8 nan_acc = 0u;
 
        for (; i + 8u <= len; i += 8u) {
            __m512d v = _mm512_loadu_pd(data + i);
            nan_acc |= _mm512_cmp_pd_mask(v, v, _CMP_UNORD_Q);
            vmin = _mm512_min_pd(vmin, v);
            if (nan_acc) { *out = NAN; return NO_ERROR; }
        }
 
        cur_min = _mm512_reduce_min_pd(vmin);
        if (isinf(cur_min) && cur_min < 0.0) { *out = -INFINITY; return NO_ERROR; }
    }
 
    /* Handle 4-element AVX chunk */
    if (i + 4u <= len) {
        __m256d vmin    = _mm256_set1_pd(cur_min);
        __m256d v       = _mm256_loadu_pd(data + i);
        __m256d nan_chk = _mm256_cmp_pd(v, v, _CMP_UNORD_Q);
        if (_mm256_movemask_pd(nan_chk)) { *out = NAN; return NO_ERROR; }
        vmin = _mm256_min_pd(vmin, v);
        i += 4u;
 
        __m128d lo = _mm256_castpd256_pd128(vmin);
        __m128d hi = _mm256_extractf128_pd(vmin, 1);
        __m128d v2 = _mm_min_pd(lo, hi);
        __m128d upper = _mm_unpackhi_pd(v2, v2);
        v2 = _mm_min_pd(v2, upper);
        _mm_store_sd(&cur_min, v2);
 
        if (isinf(cur_min) && cur_min < 0.0) { *out = -INFINITY; return NO_ERROR; }
    }
 
    /* Handle 2-element SSE chunk */
    if (i + 2u <= len) {
        __m128d vmin    = _mm_set1_pd(cur_min);
        __m128d v       = _mm_loadu_pd(data + i);
        __m128d nan_chk = _mm_cmpunord_pd(v, v);
        if (_mm_movemask_pd(nan_chk)) { *out = NAN; return NO_ERROR; }
        vmin = _mm_min_pd(vmin, v);
        i += 2u;
 
        __m128d upper = _mm_unpackhi_pd(vmin, vmin);
        vmin = _mm_min_pd(vmin, upper);
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
#endif /* CSALT_SIMD_AVX512_DOUBLE_INL */

