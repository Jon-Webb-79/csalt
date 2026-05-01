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
#ifndef CSALT_SIMD_AVX512_UINT32_INL
#define CSALT_SIMD_AVX512_UINT32_INL

#include <immintrin.h>   /* AVX-512 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint32(const uint32_t* data,
                                  size_t          len,
                                  uint32_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 16u;  /* 512 bits / 32 bits per uint32_t */
 
#if defined(__AVX512F__)
    if (len >= elems_per_reg) {
        __m512i target = _mm512_set1_epi32((int32_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m512i   chunk = _mm512_loadu_si512((const __m512i*)(data + i));
            /* k-register mask: bit N set if lane N matched.
             * No divide needed — bit directly maps to element index. */
            __mmask16 mask  = _mm512_cmpeq_epi32_mask(chunk, target);
            if (mask != 0u) {
                return i + (size_t)_tzcnt_u32((uint32_t)mask);
            }
        }
 
        /* Handle remaining full AVX2 chunks (8 elements) */
        if (i + 8u <= len) {
            __m256i target256 = _mm256_set1_epi32((int32_t)value);
            for (; i + 8u <= len; i += 8u) {
                __m256i chunk = _mm256_loadu_si256((const __m256i*)(data + i));
                __m256i cmp   = _mm256_cmpeq_epi32(chunk, target256);
                int     mask  = _mm256_movemask_epi8(cmp);
                if (mask != 0) {
                    return i + (size_t)(__builtin_ctz((unsigned)mask) / 4u);
                }
            }
        }
 
        /* Handle remaining full SSE2 chunks (4 elements) */
        __m128i target128 = _mm_set1_epi32((int32_t)value);
        for (; i + 4u <= len; i += 4u) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _mm_cmpeq_epi32(chunk, target128);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 4u);
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
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_AVX512_UINT32_INL */

