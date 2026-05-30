// ================================================================================
// ================================================================================
// - File:    simd_avx2_min_int16.inl
// - Purpose: AVX2 fast path for finding the minimum int16_t in a contiguous
//            buffer.  Processes 16 elements per iteration using
//            _mm256_min_epi16.  Folds to 128 bits and uses XOR-biased
//            _mm_minpos_epu16 for the horizontal reduction.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_AVX2_MIN_INT16_INL
#define SIMD_AVX2_MIN_INT16_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>   /* AVX2 */

static inline error_code_t simd_min_int16(const int16_t* data,
                                          size_t         len,
                                          int16_t*       out) {
    int16_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 16u) {
        __m256i vmin = _mm256_set1_epi16((short)0x7FFF);   /* INT16_MAX */

        for (; i + 16u <= len; i += 16u) {
            __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
            vmin = _mm256_min_epi16(vmin, v);
        }

        /* Fold 256 → 128 */
        __m128i lo  = _mm256_castsi256_si128(vmin);
        __m128i hi  = _mm256_extracti128_si256(vmin, 1);
        __m128i v16 = _mm_min_epi16(lo, hi);

        /* Bias signed → unsigned for PHMINPOSUW, then bias back */
        __m128i bias   = _mm_set1_epi16((short)0x8000);
        __m128i biased = _mm_xor_si128(v16, bias);
        __m128i minpos = _mm_minpos_epu16(biased);
        int16_t lane_min = (int16_t)((uint16_t)_mm_extract_epi16(minpos, 0) ^ 0x8000u);

        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT16_MIN) { *out = INT16_MIN; return NO_ERROR; }
    }

    /* Handle 8-element chunk if remaining tail >= 8 */
    if (i + 8u <= len) {
        __m128i vmin = _mm_set1_epi16((short)cur_min);
        __m128i v    = _mm_loadu_si128((const __m128i*)(data + i));
        vmin = _mm_min_epi16(vmin, v);
        i += 8u;

        __m128i bias   = _mm_set1_epi16((short)0x8000);
        __m128i biased = _mm_xor_si128(vmin, bias);
        __m128i minpos = _mm_minpos_epu16(biased);
        int16_t lane_min = (int16_t)((uint16_t)_mm_extract_epi16(minpos, 0) ^ 0x8000u);

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

#endif /* SIMD_AVX2_MIN_INT16_INL */
// ================================================================================
// ================================================================================
// eof
