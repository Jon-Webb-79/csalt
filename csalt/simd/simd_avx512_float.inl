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
#ifndef CSALT_SIMD_AVX512_FLOAT_INL
#define CSALT_SIMD_AVX512_FLOAT_INL

#include "c_error.h"

#include <immintrin.h>   /* AVX-512 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_float(const float* data,
                                 size_t       len,
                                 float        value,
                                 float        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 16u;  /* 512 bits / 32 bits per float */
 
#if defined(__AVX512F__)
    if (len >= elems_per_reg) {
        __m512  target   = _mm512_set1_ps(value);
        __m512  tol      = _mm512_set1_ps(tolerance);
        __m512i sign_mask = _mm512_set1_epi32(0x7FFFFFFF);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m512  chunk   = _mm512_loadu_ps(data + i);
            __m512  diff    = _mm512_sub_ps(chunk, target);
            /* Absolute value: AND with sign mask to clear bit 31 */
            __m512  absdiff = _mm512_castsi512_ps(
                                  _mm512_and_epi32(
                                      _mm512_castps_si512(diff),
                                      sign_mask));
            /* k-register mask: bit N set if |data[i+N] - value| <= tol
             * _CMP_LE_OQ: ordered, quiet — NaN produces false */
            __mmask16 mask  = _mm512_cmp_ps_mask(absdiff, tol, _CMP_LE_OQ);
            if (mask != 0u) {
                return i + (size_t)_tzcnt_u32((uint32_t)mask);
            }
        }
 
        /* Handle remaining full AVX chunks (8 elements) */
        __m256 target256   = _mm256_set1_ps(value);
        __m256 tol256      = _mm256_set1_ps(tolerance);
        __m256 abs256      = _mm256_castsi256_ps(
                                 _mm256_set1_epi32(0x7FFFFFFF));
 
        for (; i + 8u <= len; i += 8u) {
            __m256 chunk   = _mm256_loadu_ps(data + i);
            __m256 diff    = _mm256_sub_ps(chunk, target256);
            __m256 absdiff = _mm256_and_ps(diff, abs256);
            __m256 cmp     = _mm256_cmp_ps(absdiff, tol256, _CMP_LE_OQ);
            int    mask    = _mm256_movemask_ps(cmp);
            if (mask != 0) {
                return i + (size_t)__builtin_ctz((unsigned)mask);
            }
        }
 
        /* Handle remaining full SSE2 chunks (4 elements) */
        __m128 target128   = _mm_set1_ps(value);
        __m128 tol128      = _mm_set1_ps(tolerance);
        __m128 abs128      = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
 
        for (; i + 4u <= len; i += 4u) {
            __m128 chunk   = _mm_loadu_ps(data + i);
            __m128 diff    = _mm_sub_ps(chunk, target128);
            __m128 absdiff = _mm_and_ps(diff, abs128);
            __m128 cmp     = _mm_cmple_ps(absdiff, tol128);
            int    mask    = _mm_movemask_ps(cmp);
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
 
#if defined(__AVX512F__)
    __m512  tol512    = _mm512_set1_ps(tolerance);
    __m512i sign512   = _mm512_set1_epi32(0x7FFFFFFF);
    __m256  tol256    = _mm256_set1_ps(tolerance);
    __m256  abs256    = _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF));
    __m128  tol128    = _mm_set1_ps(tolerance);
    __m128  abs128    = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
 
    size_t i = 0u;
    for (; i + 16u <= len; i += 16u) {
        __m512    va      = _mm512_loadu_ps(a + i);
        __m512    vb      = _mm512_loadu_ps(b + i);
        __m512    diff    = _mm512_sub_ps(va, vb);
        __m512    absdiff = _mm512_castsi512_ps(
                                _mm512_and_epi32(
                                    _mm512_castps_si512(diff), sign512));
        __mmask16 mask    = _mm512_cmp_ps_mask(absdiff, tol512, _CMP_LE_OQ);
        if (mask != (uint16_t)0xFFFF) return false;
    }
 
    for (; i + 8u <= len; i += 8u) {
        __m256 va      = _mm256_loadu_ps(a + i);
        __m256 vb      = _mm256_loadu_ps(b + i);
        __m256 diff    = _mm256_sub_ps(va, vb);
        __m256 absdiff = _mm256_and_ps(diff, abs256);
        __m256 cmp     = _mm256_cmp_ps(absdiff, tol256, _CMP_LE_OQ);
        if (_mm256_movemask_ps(cmp) != 0xFF) return false;
    }
 
    for (; i + 4u <= len; i += 4u) {
        __m128 va      = _mm_loadu_ps(a + i);
        __m128 vb      = _mm_loadu_ps(b + i);
        __m128 diff    = _mm_sub_ps(va, vb);
        __m128 absdiff = _mm_and_ps(diff, abs128);
        __m128 cmp     = _mm_cmple_ps(absdiff, tol128);
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
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_float(const float* data,
                                          size_t       len,
                                          float*       out) {
    float  cur_min = *out;
    size_t i       = 0u;
 
    if (len >= 16u) {
        __m512    vmin    = _mm512_set1_ps(*out);
        __mmask16 nan_acc = 0u;
 
        for (; i + 16u <= len; i += 16u) {
            __m512 v = _mm512_loadu_ps(data + i);
            nan_acc |= _mm512_cmp_ps_mask(v, v, _CMP_UNORD_Q);
            vmin = _mm512_min_ps(vmin, v);
            if (nan_acc) { *out = NAN; return NO_ERROR; }
        }
 
        cur_min = _mm512_reduce_min_ps(vmin);
        if (isinf(cur_min) && cur_min < 0.0f) { *out = -INFINITY; return NO_ERROR; }
    }
 
    /* Handle 8-element AVX chunk */
    if (i + 8u <= len) {
        __m256 vmin    = _mm256_set1_ps(cur_min);
        __m256 v       = _mm256_loadu_ps(data + i);
        __m256 nan_chk = _mm256_cmp_ps(v, v, _CMP_UNORD_Q);
        if (_mm256_movemask_ps(nan_chk)) { *out = NAN; return NO_ERROR; }
        vmin = _mm256_min_ps(vmin, v);
        i += 8u;
 
        __m128 lo  = _mm256_castps256_ps128(vmin);
        __m128 hi  = _mm256_extractf128_ps(vmin, 1);
        __m128 v4  = _mm_min_ps(lo, hi);
        __m128 shuf = _mm_movehl_ps(v4, v4);
        v4 = _mm_min_ps(v4, shuf);
        shuf = _mm_shuffle_ps(v4, v4, _MM_SHUFFLE(1,1,1,1));
        v4 = _mm_min_ps(v4, shuf);
        _mm_store_ss(&cur_min, v4);
 
        if (isinf(cur_min) && cur_min < 0.0f) { *out = -INFINITY; return NO_ERROR; }
    }
 
    /* Handle 4-element SSE chunk */
    if (i + 4u <= len) {
        __m128 vmin    = _mm_set1_ps(cur_min);
        __m128 v       = _mm_loadu_ps(data + i);
        __m128 nan_chk = _mm_cmpunord_ps(v, v);
        if (_mm_movemask_ps(nan_chk)) { *out = NAN; return NO_ERROR; }
        vmin = _mm_min_ps(vmin, v);
        i += 4u;
 
        __m128 shuf = _mm_movehl_ps(vmin, vmin);
        vmin = _mm_min_ps(vmin, shuf);
        shuf = _mm_shuffle_ps(vmin, vmin, _MM_SHUFFLE(1,1,1,1));
        vmin = _mm_min_ps(vmin, shuf);
        _mm_store_ss(&cur_min, vmin);
 
        if (isinf(cur_min) && cur_min < 0.0f) { *out = -INFINITY; return NO_ERROR; }
    }
 
    /* Scalar tail */
    for (; i < len; i++) {
        if (isnan(data[i])) { *out = NAN; return NO_ERROR; }
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (isinf(cur_min) && cur_min < 0.0f) { *out = -INFINITY; return NO_ERROR; }
        }
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_AVX512_FLOAT_INL */

