// ================================================================================
// ================================================================================
// - File:    simd_avx_min_int8.inl
// - Purpose: AVX fast path for finding the minimum int8_t in a contiguous
//            buffer.  AVX (without AVX2) provides no 256-bit integer min
//            intrinsic.  Since AVX implies SSE4.1, this variant uses 128-bit
//            _mm_min_epi8.  The file exists to satisfy the per-ISA dispatch
//            convention.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_AVX_MIN_INT8_INL
#define SIMD_AVX_MIN_INT8_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>   /* AVX — implies SSE4.1 */

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

#endif /* SIMD_AVX_MIN_INT8_INL */
// ================================================================================
// ================================================================================
// eof
