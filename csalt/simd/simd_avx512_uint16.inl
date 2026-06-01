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
#ifndef CSALT_SIMD_AVX512_UINT16_INL
#define CSALT_SIMD_AVX512_UINT16_INL

#include "c_error.h"

#include <immintrin.h>   /* AVX-512 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint16(const uint16_t* data,
                                  size_t          len,
                                  uint16_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 32u;  /* 512 bits / 16 bits per uint16_t */
 
#if defined(__AVX512BW__)
    if (len >= elems_per_reg) {
        __m512i target = _mm512_set1_epi16((int16_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m512i   chunk = _mm512_loadu_si512((const __m512i*)(data + i));
            /* k-register mask: bit N set if lane N matched */
            __mmask32 mask  = _mm512_cmpeq_epi16_mask(chunk, target);
            if (mask != 0u) {
                /* _tzcnt_u32 gives element index directly — no /2 needed */
                return i + (size_t)_tzcnt_u32((uint32_t)mask);
            }
        }
 
        /* Handle remaining full AVX2 chunks (16 elements) */
        if (i + 16u <= len) {
            __m256i target256 = _mm256_set1_epi16((int16_t)value);
            for (; i + 16u <= len; i += 16u) {
                __m256i chunk = _mm256_loadu_si256((const __m256i*)(data + i));
                __m256i cmp   = _mm256_cmpeq_epi16(chunk, target256);
                int     mask  = _mm256_movemask_epi8(cmp);
                if (mask != 0) {
                    return i + (size_t)(__builtin_ctz((unsigned)mask) / 2u);
                }
            }
        }
 
        /* Handle remaining full SSE2 chunks (8 elements) */
        __m128i target128 = _mm_set1_epi16((int16_t)value);
        for (; i + 8u <= len; i += 8u) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _mm_cmpeq_epi16(chunk, target128);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 2u);
            }
        }
 
        /* Scalar remainder */
        for (; i < len; i++) {
            if (data[i] == value) return i;
        }
        return SIZE_MAX;
    }
#endif
 
    for (size_t i = 0u; i < len; i++) {
        if (data[i] == value) return i;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_uint16(const uint16_t* data,
                                           size_t          len,
                                           uint16_t*       out) {
    uint16_t cur_min = *out;
    size_t   i       = 0u;
 
    if (len >= 32u) {
        __m512i vmin = _mm512_set1_epi16((short)0xFFFF);
 
        for (; i + 32u <= len; i += 32u) {
            __m512i v = _mm512_loadu_si512((const void*)(data + i));
            vmin = _mm512_min_epu16(vmin, v);
        }
 
        /* Fold 512 → 256 → 128 */
        __m256i lo256 = _mm512_castsi512_si256(vmin);
        __m256i hi256 = _mm512_extracti64x4_epi64(vmin, 1);
        __m256i v32   = _mm256_min_epu16(lo256, hi256);
 
        __m128i lo128 = _mm256_castsi256_si128(v32);
        __m128i hi128 = _mm256_extracti128_si256(v32, 1);
        __m128i v16   = _mm_min_epu16(lo128, hi128);
 
        __m128i minpos    = _mm_minpos_epu16(v16);
        uint16_t lane_min = (uint16_t)_mm_extract_epi16(minpos, 0);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Handle 16-element chunk */
    if (i + 16u <= len) {
        __m256i vmin = _mm256_set1_epi16((short)cur_min);
        __m256i v    = _mm256_loadu_si256((const __m256i*)(data + i));
        vmin = _mm256_min_epu16(vmin, v);
        i += 16u;
 
        __m128i lo  = _mm256_castsi256_si128(vmin);
        __m128i hi  = _mm256_extracti128_si256(vmin, 1);
        __m128i v16 = _mm_min_epu16(lo, hi);
 
        __m128i minpos    = _mm_minpos_epu16(v16);
        uint16_t lane_min = (uint16_t)_mm_extract_epi16(minpos, 0);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Handle 8-element chunk */
    if (i + 8u <= len) {
        __m128i vmin = _mm_set1_epi16((short)cur_min);
        __m128i v    = _mm_loadu_si128((const __m128i*)(data + i));
        vmin = _mm_min_epu16(vmin, v);
        i += 8u;
 
        __m128i minpos    = _mm_minpos_epu16(vmin);
        uint16_t lane_min = (uint16_t)_mm_extract_epi16(minpos, 0);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Scalar tail */
    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
        }
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_AVX512_UINT16_INL */

