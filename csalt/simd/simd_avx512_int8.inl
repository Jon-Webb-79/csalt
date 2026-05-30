// ================================================================================
// ================================================================================
// - File:    simd_avx512_min_int8.inl
// - Purpose: AVX-512BW fast path for finding the minimum int8_t in a
//            contiguous buffer.  Processes 64 bytes per iteration using
//            _mm512_min_epi8.  Early-exits when the running minimum reaches
//            INT8_MIN (-128).
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_AVX512_MIN_INT8_INL
#define SIMD_AVX512_MIN_INT8_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>   /* AVX-512 */

static inline error_code_t simd_min_int8(const int8_t* data,
                                         size_t        len,
                                         int8_t*       out) {
    int8_t cur_min = *out;
    size_t i       = 0u;

    if (len >= 64u) {
        __m512i vmin = _mm512_set1_epi8((char)0x7F);   /* INT8_MAX */

        for (; i + 64u <= len; i += 64u) {
            __m512i v = _mm512_loadu_si512((const void*)(data + i));
            vmin = _mm512_min_epi8(vmin, v);
        }

        /* Fold 512 → 256 → 128 */
        __m256i lo256 = _mm512_castsi512_si256(vmin);
        __m256i hi256 = _mm512_extracti64x4_epi64(vmin, 1);
        __m256i v32   = _mm256_min_epi8(lo256, hi256);

        __m128i lo128 = _mm256_castsi256_si128(v32);
        __m128i hi128 = _mm256_extracti128_si256(v32, 1);
        __m128i v16   = _mm_min_epi8(lo128, hi128);

        /* Horizontal reduction: 16 → 8 → 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(v16, 8);
        v16 = _mm_min_epi8(v16, shifted);
        shifted = _mm_srli_si128(v16, 4);
        v16 = _mm_min_epi8(v16, shifted);
        shifted = _mm_srli_si128(v16, 2);
        v16 = _mm_min_epi8(v16, shifted);
        shifted = _mm_srli_si128(v16, 1);
        v16 = _mm_min_epi8(v16, shifted);

        int8_t lane_min = (int8_t)(_mm_extract_epi8(v16, 0));
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT8_MIN) { *out = INT8_MIN; return NO_ERROR; }
    }

    /* Handle 32-byte chunk */
    if (i + 32u <= len) {
        __m256i vmin = _mm256_set1_epi8((char)cur_min);
        __m256i v    = _mm256_loadu_si256((const __m256i*)(data + i));
        vmin = _mm256_min_epi8(vmin, v);
        i += 32u;

        __m128i lo  = _mm256_castsi256_si128(vmin);
        __m128i hi  = _mm256_extracti128_si256(vmin, 1);
        __m128i v16 = _mm_min_epi8(lo, hi);

        __m128i shifted = _mm_srli_si128(v16, 8);
        v16 = _mm_min_epi8(v16, shifted);
        shifted = _mm_srli_si128(v16, 4);
        v16 = _mm_min_epi8(v16, shifted);
        shifted = _mm_srli_si128(v16, 2);
        v16 = _mm_min_epi8(v16, shifted);
        shifted = _mm_srli_si128(v16, 1);
        v16 = _mm_min_epi8(v16, shifted);

        int8_t lane_min = (int8_t)(_mm_extract_epi8(v16, 0));
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT8_MIN) { *out = INT8_MIN; return NO_ERROR; }
    }

    /* Handle 16-byte chunk */
    if (i + 16u <= len) {
        __m128i vmin = _mm_set1_epi8((char)cur_min);
        __m128i v    = _mm_loadu_si128((const __m128i*)(data + i));
        vmin = _mm_min_epi8(vmin, v);
        i += 16u;

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

    /* Scalar tail */
    for (; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT8_MIN) { *out = INT8_MIN; return NO_ERROR; }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_AVX512_MIN_INT8_INL */
// ================================================================================
// ================================================================================
// eof
