// ================================================================================
// ================================================================================
// - File:    simd_ssse3_min_int8.inl
// - Purpose: SSSE3 fast path for finding the minimum int8_t in a contiguous
//            buffer.  SSSE3 still lacks _mm_min_epi8, so the XOR-bias
//            technique from the SSE2 variant is reused.  This file exists to
//            satisfy the per-ISA dispatch convention.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SSSE3_MIN_INT8_INL
#define SIMD_SSSE3_MIN_INT8_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <tmmintrin.h>   /* SSSE3 — implies SSE2 */

static inline error_code_t simd_min_int8(const int8_t* data,
                                         size_t        len,
                                         int8_t*       out) {
    int8_t cur_min = *out;
    size_t i       = 0u;

    if (len >= 16u) {
        __m128i bias = _mm_set1_epi8((char)0x80);
        __m128i vmin = _mm_set1_epi8((char)0xFF);

        for (; i + 16u <= len; i += 16u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            v = _mm_xor_si128(v, bias);
            vmin = _mm_min_epu8(vmin, v);
        }

        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _mm_min_epu8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _mm_min_epu8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 2);
        vmin = _mm_min_epu8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 1);
        vmin = _mm_min_epu8(vmin, shifted);

        uint8_t biased_min = (uint8_t)(_mm_extract_epi16(vmin, 0) & 0xFFu);
        int8_t  lane_min   = (int8_t)(biased_min ^ 0x80u);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT8_MIN) { *out = INT8_MIN; return NO_ERROR; }
    }

    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT8_MIN) { *out = INT8_MIN; return NO_ERROR; }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SSSE3_MIN_INT8_INL */
// ================================================================================
// ================================================================================
// eof
