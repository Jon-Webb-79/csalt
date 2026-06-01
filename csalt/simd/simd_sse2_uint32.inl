
// ================================================================================
// - File:    simd_sse2_lsearch_uint16.inl
// - Purpose: SSE2 implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements using SSE2 128-bit integer comparisons.
//
//            SSE2 provides _mm_cmpeq_epi16 which compares 8 uint16_t
//            lanes simultaneously.  _mm_movemask_epi8 extracts a 16-bit
//            byte mask from the comparison result; the position of the
//            first set bit divided by 2 gives the element index within
//            the current chunk.
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
#ifndef CSALT_SIMD_SSE2_UINT32_INL
#define CSALT_SIMD_SSE2_UINT32_INL
// ================================================================================ 
// ================================================================================ 

#include "c_error.h"

#include <emmintrin.h>   /* SSE2 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint32(const uint32_t* data,
                                  size_t          len,
                                  uint32_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 4u;   /* 128 bits / 32 bits per uint32_t */
 
#if defined(__SSE2__)
    if (len >= elems_per_reg) {
        __m128i target = _mm_set1_epi32((int32_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _mm_cmpeq_epi32(chunk, target);
 
            /* movemask produces one bit per byte — 16 bits for 16 bytes.
             * Each uint32_t match sets four consecutive bits. */
            int mask = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                /* __builtin_ctz finds lowest set bit (byte offset).
                 * Divide by 4 to convert byte offset to element offset. */
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 4u);
            }
        }
 
        /* Scalar remainder for the last < 4 elements */
        for (; i < len; i++) {
            if (data[i] == value) return i;
        }
        return SIZE_MAX;
    }
#endif
 
    /* Scalar fallback */
    for (size_t i = 0u; i < len; i++) {
        if (data[i] == value) return i;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline __m128i _sse2_min_epu32(__m128i a, __m128i b, __m128i bias) {
    __m128i a_biased = _mm_xor_si128(a, bias);
    __m128i b_biased = _mm_xor_si128(b, bias);
    __m128i mask     = _mm_cmplt_epi32(a_biased, b_biased); /* -1 where a < b */
    return _mm_or_si128(_mm_and_si128(mask, a),
                        _mm_andnot_si128(mask, b));
}
 
static inline error_code_t simd_min_uint32(const uint32_t* data,
                                           size_t          len,
                                           uint32_t*       out) {
    uint32_t cur_min = *out;
    size_t   i       = 0u;
 
    if (len >= 4u) {
        __m128i bias = _mm_set1_epi32((int)0x80000000);
        __m128i vmin = _mm_set1_epi32((int)0xFFFFFFFF);  /* UINT32_MAX */
 
        for (; i + 4u <= len; i += 4u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _sse2_min_epu32(vmin, v, bias);
        }
 
        /* Horizontal reduction: 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _sse2_min_epu32(vmin, shifted, bias);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _sse2_min_epu32(vmin, shifted, bias);
 
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
#endif /* CSALT_SIMD_SSE2_UINT32_INL */

