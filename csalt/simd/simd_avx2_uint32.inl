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
#ifndef CSALT_SIMD_AVX2_UINT32_INL
#define CSALT_SIMD_AVX2_UINT32_INL

#include "c_error.h" 

#include <immintrin.h>   /* AVX2 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint32(const uint32_t* data,
                                  size_t          len,
                                  uint32_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 8u;   /* 256 bits / 32 bits per uint32_t */
 
#if defined(__AVX2__)
    if (len >= elems_per_reg) {
        __m256i target = _mm256_set1_epi32((int32_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m256i chunk = _mm256_loadu_si256((const __m256i*)(data + i));
            __m256i cmp   = _mm256_cmpeq_epi32(chunk, target);
 
            /* movemask produces one bit per byte — 32 bits for 32 bytes.
             * Each uint32_t match sets four consecutive bits. */
            int mask = _mm256_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 4u);
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
 
        _mm256_zeroupper();
        return SIZE_MAX;
    }
#endif
 
    for (size_t i = 0u; i < len; i++) {
        if (data[i] == value) return i;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_uint32(const uint32_t* data,
                                           size_t          len,
                                           uint32_t*       out) {
    uint32_t cur_min = *out;
    size_t   i       = 0u;
 
    if (len >= 8u) {
        __m256i vmin = _mm256_set1_epi32((int)0xFFFFFFFF);
 
        for (; i + 8u <= len; i += 8u) {
            __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
            vmin = _mm256_min_epu32(vmin, v);
        }
 
        /* Fold 256 → 128 */
        __m128i lo  = _mm256_castsi256_si128(vmin);
        __m128i hi  = _mm256_extracti128_si256(vmin, 1);
        __m128i v16 = _mm_min_epu32(lo, hi);
 
        /* Horizontal reduction: 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(v16, 8);
        v16 = _mm_min_epu32(v16, shifted);
        shifted = _mm_srli_si128(v16, 4);
        v16 = _mm_min_epu32(v16, shifted);
 
        uint32_t lane_min = (uint32_t)_mm_cvtsi128_si32(v16);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Handle 4-element chunk if remaining tail >= 4 */
    if (i + 4u <= len) {
        __m128i vmin = _mm_set1_epi32((int)cur_min);
        __m128i v    = _mm_loadu_si128((const __m128i*)(data + i));
        vmin = _mm_min_epu32(vmin, v);
        i += 4u;
 
        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _mm_min_epu32(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _mm_min_epu32(vmin, shifted);
 
        uint32_t lane_min = (uint32_t)_mm_cvtsi128_si32(vmin);
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
#endif /* CSALT_SIMD_AVX2_UINT32_INL */

