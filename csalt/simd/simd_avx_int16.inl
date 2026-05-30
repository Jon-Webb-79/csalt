// ================================================================================
// ================================================================================
// - File:    simd_avx_min_int16.inl
// - Purpose: AVX fast path for finding the minimum int16_t in a contiguous
//            buffer.  AVX (without AVX2) provides no 256-bit integer min.
//            Since AVX implies SSE4.1, this uses 128-bit _mm_min_epi16 with
//            PHMINPOSUW-based horizontal reduction.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_AVX_MIN_INT16_INL
#define SIMD_AVX_MIN_INT16_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>   /* AVX — implies SSE4.1 */

static inline error_code_t simd_min_int16(const int16_t* data,
                                          size_t         len,
                                          int16_t*       out) {
    int16_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 8u) {
        __m128i vmin = _mm_set1_epi16((short)0x7FFF);

        for (; i + 8u <= len; i += 8u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _mm_min_epi16(vmin, v);
        }

        __m128i bias   = _mm_set1_epi16((short)0x8000);
        __m128i biased = _mm_xor_si128(vmin, bias);
        __m128i minpos = _mm_minpos_epu16(biased);
        int16_t lane_min = (int16_t)((uint16_t)_mm_extract_epi16(minpos, 0) ^ 0x8000u);

        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT16_MIN) { *out = INT16_MIN; return NO_ERROR; }
    }

    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT16_MIN) { *out = INT16_MIN; return NO_ERROR; }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_AVX_MIN_INT16_INL */
// ================================================================================
// ================================================================================
// eof
