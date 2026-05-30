// ================================================================================
// ================================================================================
// - File:    simd_sse2_min_int16.inl
// - Purpose: SSE2 fast path for finding the minimum int16_t in a contiguous
//            buffer.  Unlike uint16, SSE2 provides a native signed 16-bit min
//            intrinsic (_mm_min_epi16).  Processes 8 elements per iteration.
//            Early-exits when the running minimum reaches INT16_MIN (-32768).
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SSE2_MIN_INT16_INL
#define SIMD_SSE2_MIN_INT16_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <emmintrin.h>   /* SSE2 */

static inline error_code_t simd_min_int16(const int16_t* data,
                                          size_t         len,
                                          int16_t*       out) {
    int16_t cur_min = *out;              /* caller seeds with INT16_MAX */
    size_t  i       = 0u;

    if (len >= 8u) {
        __m128i vmin = _mm_set1_epi16((short)0x7FFF);   /* INT16_MAX */

        for (; i + 8u <= len; i += 8u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _mm_min_epi16(vmin, v);
        }

        /* Horizontal reduction: 8 → 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _mm_min_epi16(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _mm_min_epi16(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 2);
        vmin = _mm_min_epi16(vmin, shifted);

        int16_t lane_min = (int16_t)_mm_extract_epi16(vmin, 0);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT16_MIN) { *out = INT16_MIN; return NO_ERROR; }
    }

    /* Scalar tail */
    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT16_MIN) { *out = INT16_MIN; return NO_ERROR; }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SSE2_MIN_INT16_INL */
// ================================================================================
// ================================================================================
// eof
