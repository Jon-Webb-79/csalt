// ================================================================================
// ================================================================================
// - File:    simd_avx512_min_int64.inl
// - Purpose: AVX-512F fast path for finding the minimum int64_t in a
//            contiguous buffer.  AVX-512F provides native _mm512_min_epi64.
//            Processes 8 elements per iteration.  Uses
//            _mm512_reduce_min_epi64 for horizontal reduction.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_AVX512_MIN_INT64_INL
#define SIMD_AVX512_MIN_INT64_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>   /* AVX-512 */

static inline error_code_t simd_min_int64(const int64_t* data,
                                          size_t         len,
                                          int64_t*       out) {
    int64_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 8u) {
        __m512i vmin = _mm512_set1_epi64(INT64_MAX);

        for (; i + 8u <= len; i += 8u) {
            __m512i v = _mm512_loadu_si512((const void*)(data + i));
            vmin = _mm512_min_epi64(vmin, v);
        }

        int64_t lane_min = _mm512_reduce_min_epi64(vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT64_MIN) { *out = INT64_MIN; return NO_ERROR; }
    }

    /* Handle 4-element AVX2 chunk */
    if (i + 4u <= len) {
        __m256i vmin = _mm256_set1_epi64x(cur_min);
        __m256i v    = _mm256_loadu_si256((const __m256i*)(data + i));

        __m256i vmin_gt = _mm256_cmpgt_epi64(vmin, v);
        vmin = _mm256_blendv_epi8(vmin, v, vmin_gt);
        i += 4u;

        /* Scalar extract from 4 lanes */
        int64_t tmp[4];
        _mm256_storeu_si256((__m256i*)tmp, vmin);
        for (int k = 0; k < 4; k++) {
            if (tmp[k] < cur_min) cur_min = tmp[k];
        }
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

#endif /* SIMD_AVX512_MIN_INT64_INL */
// ================================================================================
// ================================================================================
// eof
