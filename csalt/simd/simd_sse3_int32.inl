// ================================================================================
// ================================================================================
// - File:    simd_ssse3_min_int32.inl
// - Purpose: SSSE3 fast path for finding the minimum int32_t in a contiguous
//            buffer.  SSSE3 still lacks _mm_min_epi32, so the emulation from
//            the SSE2 variant is reused.  This file exists to satisfy the
//            per-ISA dispatch convention.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SSSE3_MIN_INT32_INL
#define SIMD_SSSE3_MIN_INT32_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <tmmintrin.h>   /* SSSE3 — implies SSE2 */

static inline __m128i _ssse3_min_epi32(__m128i a, __m128i b) {
    __m128i mask = _mm_cmplt_epi32(a, b);
    return _mm_or_si128(_mm_and_si128(mask, a),
                        _mm_andnot_si128(mask, b));
}

static inline error_code_t simd_min_int32(const int32_t* data,
                                          size_t         len,
                                          int32_t*       out) {
    int32_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 4u) {
        __m128i vmin = _mm_set1_epi32(INT32_MAX);

        for (; i + 4u <= len; i += 4u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _ssse3_min_epi32(vmin, v);
        }

        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _ssse3_min_epi32(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _ssse3_min_epi32(vmin, shifted);

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

#endif /* SIMD_SSSE3_MIN_INT32_INL */
// ================================================================================
// ================================================================================
// eof
