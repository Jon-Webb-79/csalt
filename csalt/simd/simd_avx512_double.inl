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
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_AVX512_DOUBLE_INL */

