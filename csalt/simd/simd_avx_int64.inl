// ================================================================================
// ================================================================================
// - File:    simd_avx_min_int64.inl
// - Purpose: AVX path for finding the minimum int64_t in a contiguous
//            buffer.  AVX implies SSE4.2 which provides _mm_cmpgt_epi64,
//            but only 2 int64 elements fit per 128-bit vector (AVX has
//            no 256-bit integer ops), making vectorized min slower than
//            scalar.  Uses the scalar loop; exists for the per-ISA dispatch
//            convention.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_AVX_MIN_INT64_INL
#define SIMD_AVX_MIN_INT64_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>   /* AVX */

static inline error_code_t simd_min_int64(const int64_t* data,
                                          size_t         len,
                                          int64_t*       out) {
    int64_t cur_min = *out;

    for (size_t i = 0u; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT64_MIN) { *out = INT64_MIN; return NO_ERROR; }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_AVX_MIN_INT64_INL */
// ================================================================================
// ================================================================================
// eof
