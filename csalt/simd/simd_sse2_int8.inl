// ================================================================================
// ================================================================================
// - File:    simd_sse2_min_int8.inl
// - Purpose: SSE2 fast path for finding the minimum int8_t in a contiguous
//            buffer.  SSE2 lacks _mm_min_epi8, so every element is XOR'd with
//            0x80 to convert signed ordering to unsigned ordering, reduced with
//            _mm_min_epu8, then XOR'd back.  Early-exits when the running
//            minimum reaches INT8_MIN (-128).
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SSE2_MIN_INT8_INL
#define SIMD_SSE2_MIN_INT8_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <emmintrin.h>   /* SSE2 */

static inline error_code_t simd_min_int8(const int8_t* data,
                                         size_t        len,
                                         int8_t*       out) {
    int8_t cur_min = *out;               /* caller seeds with INT8_MAX */
    size_t i       = 0u;

    if (len >= 16u) {
        /* Bias constant: XOR with 0x80 maps signed [-128,127] → unsigned [0,255] */
        __m128i bias = _mm_set1_epi8((char)0x80);

        /* Seed the accumulator with biased INT8_MAX: 127 ^ 0x80 = 0xFF */
        __m128i vmin = _mm_set1_epi8((char)0xFF);

        for (; i + 16u <= len; i += 16u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            v = _mm_xor_si128(v, bias);          /* to unsigned domain */
            vmin = _mm_min_epu8(vmin, v);
        }

        /* Horizontal reduction: 16 → 8 → 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _mm_min_epu8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _mm_min_epu8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 2);
        vmin = _mm_min_epu8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 1);
        vmin = _mm_min_epu8(vmin, shifted);

        /* Convert back to signed domain */
        uint8_t biased_min = (uint8_t)(_mm_extract_epi16(vmin, 0) & 0xFFu);
        int8_t  lane_min   = (int8_t)(biased_min ^ 0x80u);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT8_MIN) { *out = INT8_MIN; return NO_ERROR; }
    }

    /* Scalar tail */
    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT8_MIN) { *out = INT8_MIN; return NO_ERROR; }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SSE2_MIN_INT8_INL */
// ================================================================================
// ================================================================================
// eof
