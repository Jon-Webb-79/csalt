// ================================================================================
// ================================================================================
// - File:    simd_avx2_min_int64.inl
// - Purpose: AVX2 fast path for finding the minimum int64_t in a contiguous
//            buffer.  AVX2 provides _mm256_cmpgt_epi64 which is natively
//            signed — no bias trick is needed (unlike uint64).  Uses
//            cmpgt + blendv to emulate signed 64-bit min.  Processes 4
//            elements per iteration.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_AVX2_MIN_INT64_INL
#define SIMD_AVX2_MIN_INT64_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>   /* AVX2 */

/**
 * Emulated signed 64-bit min for AVX2.
 * _mm256_cmpgt_epi64 is natively signed — no bias needed.
 * mask = -1 where a > b, so select b where mask is set, else a.
 */
static inline __m256i _avx2_min_epi64(__m256i a, __m256i b) {
    __m256i a_gt_b = _mm256_cmpgt_epi64(a, b);
    return _mm256_blendv_epi8(a, b, a_gt_b);
}

static inline error_code_t simd_min_int64(const int64_t* data,
                                          size_t         len,
                                          int64_t*       out) {
    int64_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 4u) {
        __m256i vmin = _mm256_set1_epi64x(INT64_MAX);

        for (; i + 4u <= len; i += 4u) {
            __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
            vmin = _avx2_min_epi64(vmin, v);
        }

        /* Horizontal reduction: 4 → 2 using 128-bit SSE4.2 _mm_cmpgt_epi64 */
        __m128i lo = _mm256_castsi256_si128(vmin);
        __m128i hi = _mm256_extracti128_si256(vmin, 1);

        __m128i lo_gt_hi = _mm_cmpgt_epi64(lo, hi);
        __m128i v2       = _mm_blendv_epi8(lo, hi, lo_gt_hi);

        /* 2 → 1 */
        __m128i upper   = _mm_srli_si128(v2, 8);
        __m128i v2_gt   = _mm_cmpgt_epi64(v2, upper);
        __m128i v1      = _mm_blendv_epi8(v2, upper, v2_gt);

        int64_t lane_min = (int64_t)_mm_extract_epi64(v1, 0);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT64_MIN) { *out = INT64_MIN; return NO_ERROR; }
    }

    /* Scalar tail */
    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT64_MIN) { *out = INT64_MIN; return NO_ERROR; }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_AVX2_MIN_INT64_INL */
// ================================================================================
// ================================================================================
// eof
