// ================================================================================
// ================================================================================
// - File:    simd_sse41_min_int16.inl
// - Purpose: SSE4.1 fast path for finding the minimum int16_t in a contiguous
//            buffer.  Uses _mm_min_epi16 (SSE2) for the vertical pass.
//            For the horizontal reduction, the final 8 signed lanes are
//            XOR-biased to unsigned with 0x8000, reduced with
//            _mm_minpos_epu16 (PHMINPOSUW), then XOR'd back to signed.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SSE41_MIN_INT16_INL
#define SIMD_SSE41_MIN_INT16_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <smmintrin.h>   /* SSE4.1 */

static inline error_code_t simd_min_int16(const int16_t* data,
                                          size_t         len,
                                          int16_t*       out) {
    int16_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 8u) {
        __m128i vmin = _mm_set1_epi16((short)0x7FFF);   /* INT16_MAX */

        for (; i + 8u <= len; i += 8u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _mm_min_epi16(vmin, v);
        }

        /* Bias signed → unsigned for PHMINPOSUW, then bias back */
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

#endif /* SIMD_SSE41_MIN_INT16_INL */
// ================================================================================
// ================================================================================
// eof
