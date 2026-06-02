
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
#ifndef CSALT_SIMD_SSE2_UINT64_INL
#define CSALT_SIMD_SSE2_UINT64_INL
// ================================================================================ 
// ================================================================================ 

#include "c_error.h"

#include <emmintrin.h>   /* SSE2 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

/**
 * Compare 2 uint64_t lanes for equality using SSE2 32-bit comparisons.
 * Returns a mask vector with 0xFFFFFFFFFFFFFFFF in matching lanes and
 * 0x0000000000000000 in non-matching lanes.
 */
static inline __m128i _sse2_cmpeq_epi64(__m128i a, __m128i b) {
    /* Compare both 32-bit halves of each 64-bit lane */
    __m128i cmp32 = _mm_cmpeq_epi32(a, b);
    /* AND each lane with its neighbour: both halves must match.
     * _mm_and_si128(cmp32, _mm_shuffle_epi32(cmp32, 0xB1)) swaps the
     * two 32-bit words within each 64-bit lane (0xB1 = 10110001b) and
     * ANDs them so the result is all-ones only when both halves matched. */
    return _mm_and_si128(cmp32, _mm_shuffle_epi32(cmp32, 0xB1));
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static size_t simd_lsearch_uint64(const uint64_t* data,
                                  size_t          len,
                                  uint64_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 2u;   /* 128 bits / 64 bits per uint64_t */
 
#if defined(__SSE2__)
    if (len >= elems_per_reg) {
        /* Broadcast target into both 64-bit lanes */
        __m128i target = _mm_set1_epi64x((int64_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _sse2_cmpeq_epi64(chunk, target);
 
            /* movemask produces one bit per byte — 16 bits for 16 bytes.
             * Each uint64_t match sets 8 consecutive bits. */
            int mask = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 8u);
            }
        }
 
        /* Scalar remainder for the last < 2 elements */
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

static inline error_code_t simd_min_uint64(const uint64_t* data,
                                           size_t          len,
                                           uint64_t*       out) {
    uint64_t cur_min = *out;
 
    for (size_t i = 0u; i < len; i++) {
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
#endif /* CSALT_SIMD_SSE2_UINT64_INL */

