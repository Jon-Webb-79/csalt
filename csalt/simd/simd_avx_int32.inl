// ================================================================================
// ================================================================================
// - File:    simd_avx2_min_int32.inl
// - Purpose: AVX2 fast path for finding the minimum int32_t in a contiguous
//            buffer.  Processes 8 elements per iteration using
//            _mm256_min_epi32.  Folds to 128 bits for horizontal reduction.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_AVX2_MIN_INT32_INL
#define SIMD_AVX2_MIN_INT32_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>   /* AVX2 */

static inline error_code_t simd_min_int32(const int32_t* data,
                                          size_t         len,
                                          int32_t*       out) {
    int32_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 8u) {
        __m256i vmin = _mm256_set1_epi32(INT32_MAX);

        for (; i + 8u <= len; i += 8u) {
            __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
            vmin = _mm256_min_epi32(vmin, v);
        }

        /* Fold 256 → 128 */
        __m128i lo  = _mm256_castsi256_si128(vmin);
        __m128i hi  = _mm256_extracti128_si256(vmin, 1);
        __m128i v16 = _mm_min_epi32(lo, hi);

        /* Horizontal reduction: 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(v16, 8);
        v16 = _mm_min_epi32(v16, shifted);
        shifted = _mm_srli_si128(v16, 4);
        v16 = _mm_min_epi32(v16, shifted);

        int32_t lane_min = _mm_cvtsi128_si32(v16);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT32_MIN) { *out = INT32_MIN; return NO_ERROR; }
    }

    /* Handle 4-element chunk if remaining tail >= 4 */
    if (i + 4u <= len) {
        __m128i vmin = _mm_set1_epi32(cur_min);
        __m128i v    = _mm_loadu_si128((const __m128i*)(data + i));
        vmin = _mm_min_epi32(vmin, v);
        i += 4u;

        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _mm_min_epi32(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _mm_min_epi32(vmin, shifted);

        int32_t lane_min = _mm_cvtsi128_si32(vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT32_MIN) { *out = INT32_MIN; return NO_ERROR; }
    }

    /* Scalar tail */
    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT32_MIN) { *out = INT32_MIN; return NO_ERROR; }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_AVX2_MIN_INT32_INL */
// ================================================================================
// ================================================================================
// eof
