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
#ifndef CSALT_SIMD_AVX2_UINT64_INL
#define CSALT_SIMD_AVX2_UINT64_INL

#include "c_error.h"

#include <immintrin.h>   /* AVX2 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint64(const uint64_t* data,
                                  size_t          len,
                                  uint64_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 4u;   /* 256 bits / 64 bits per uint64_t */
 
#if defined(__AVX2__)
    if (len >= elems_per_reg) {
        __m256i target = _mm256_set1_epi64x((int64_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m256i chunk = _mm256_loadu_si256((const __m256i*)(data + i));
            __m256i cmp   = _mm256_cmpeq_epi64(chunk, target);
 
            /* movemask produces one bit per byte — 32 bits for 32 bytes.
             * Each uint64_t match sets 8 consecutive bits. */
            int mask = _mm256_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 8u);
            }
        }
 
        /* Handle remaining full SSE4.1 chunks (2 elements) */
        __m128i target128 = _mm_set1_epi64x((int64_t)value);
        for (; i + 2u <= len; i += 2u) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _mm_cmpeq_epi64(chunk, target128);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 8u);
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

static inline __m256i _avx2_min_epu64(__m256i a, __m256i b, __m256i bias) {
    __m256i a_biased = _mm256_xor_si256(a, bias);
    __m256i b_biased = _mm256_xor_si256(b, bias);
    __m256i a_gt_b   = _mm256_cmpgt_epi64(a_biased, b_biased); /* -1 where a > b */
    return _mm256_blendv_epi8(a, b, a_gt_b);  /* select b where a > b, else a */
}
 
static inline error_code_t simd_min_uint64(const uint64_t* data,
                                           size_t          len,
                                           uint64_t*       out) {
    uint64_t cur_min = *out;
    size_t   i       = 0u;
 
    if (len >= 4u) {
        __m256i bias = _mm256_set1_epi64x((long long)0x8000000000000000LL);
        __m256i vmin = _mm256_set1_epi64x((long long)0xFFFFFFFFFFFFFFFFLL);
 
        for (; i + 4u <= len; i += 4u) {
            __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
            vmin = _avx2_min_epu64(vmin, v, bias);
        }
 
        /* Horizontal reduction: 4 → 2 → 1 */
        __m128i lo  = _mm256_castsi256_si128(vmin);
        __m128i hi  = _mm256_extracti128_si256(vmin, 1);
 
        /* 128-bit unsigned min using SSE4.2 _mm_cmpgt_epi64 */
        __m128i bias128  = _mm_set1_epi64x((long long)0x8000000000000000LL);
        __m128i lo_b     = _mm_xor_si128(lo, bias128);
        __m128i hi_b     = _mm_xor_si128(hi, bias128);
        __m128i lo_gt_hi = _mm_cmpgt_epi64(lo_b, hi_b);
        __m128i v2       = _mm_blendv_epi8(lo, hi, lo_gt_hi);
 
        /* 2 → 1 */
        __m128i upper    = _mm_srli_si128(v2, 8);
        __m128i v2_b     = _mm_xor_si128(v2, bias128);
        __m128i upper_b  = _mm_xor_si128(upper, bias128);
        __m128i v2_gt    = _mm_cmpgt_epi64(v2_b, upper_b);
        __m128i v1       = _mm_blendv_epi8(v2, upper, v2_gt);
 
        uint64_t lane_min = (uint64_t)_mm_extract_epi64(v1, 0);
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
#endif /* CSALT_SIMD_AVX2_UINT64_INL */

