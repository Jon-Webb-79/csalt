// ================================================================================
// ================================================================================
// - File:    simd_sse41_min_int8.inl
// - Purpose: SSE4.1 fast path for finding the minimum int8_t in a contiguous
//            buffer.  SSE4.1 provides _mm_min_epi8 for native signed-byte
//            comparison.  Early-exits when the running minimum reaches
//            INT8_MIN (-128).
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SSE41_MIN_INT8_INL
#define SIMD_SSE41_MIN_INT8_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <smmintrin.h>   /* SSE4.1 */

static inline error_code_t simd_min_int8(const int8_t* data,
                                         size_t        len,
                                         int8_t*       out) {
    int8_t cur_min = *out;
    size_t i       = 0u;

    if (len >= 16u) {
        __m128i vmin = _mm_set1_epi8((char)0x7F);   /* INT8_MAX */

        for (; i + 16u <= len; i += 16u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _mm_min_epi8(vmin, v);
        }

        /* Horizontal reduction using _mm_min_epi8: 16 → 8 → 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _mm_min_epi8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _mm_min_epi8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 2);
        vmin = _mm_min_epi8(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 1);
        vmin = _mm_min_epi8(vmin, shifted);

        int8_t lane_min = (int8_t)(_mm_extract_epi8(vmin, 0));
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

#endif /* SIMD_SSE41_MIN_INT8_INL */
// ================================================================================
// ================================================================================
// eof
