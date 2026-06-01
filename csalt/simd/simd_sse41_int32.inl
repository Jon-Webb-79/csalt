// ================================================================================
// ================================================================================
// - File:    simd_sse41_min_int32.inl
// - Purpose: SSE4.1 fast path for finding the minimum int32_t in a contiguous
//            buffer.  Uses native _mm_min_epi32 for both the vertical pass
//            and horizontal reduction.  Processes 4 elements per iteration.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SSE41_MIN_INT32_INL
#define SIMD_SSE41_MIN_INT32_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <smmintrin.h>   /* SSE4.1 */

static inline error_code_t simd_min_int32(const int32_t* data,
                                          size_t         len,
                                          int32_t*       out) {
    int32_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 4u) {
        __m128i vmin = _mm_set1_epi32(INT32_MAX);

        for (; i + 4u <= len; i += 4u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _mm_min_epi32(vmin, v);
        }

        /* Horizontal reduction: 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _mm_min_epi32(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _mm_min_epi32(vmin, shifted);

        int32_t lane_min = _mm_cvtsi128_si32(vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT32_MIN) { *out = INT32_MIN; return NO_ERROR; }
    }

    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT32_MIN) { *out = INT32_MIN; return NO_ERROR; }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SSE41_MIN_INT32_INL */
// ================================================================================
// ================================================================================
// eof
