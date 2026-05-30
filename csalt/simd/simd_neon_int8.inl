// ================================================================================
// ================================================================================
// - File:    simd_neon_min_int8.inl
// - Purpose: ARM NEON fast path for finding the minimum int8_t in a
//            contiguous buffer.  Processes 16 bytes per iteration using
//            vminq_s8.  Early-exits when the running minimum reaches
//            INT8_MIN (-128).
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_NEON_MIN_INT8_INL
#define SIMD_NEON_MIN_INT8_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <arm_neon.h>

static inline error_code_t simd_min_int8(const int8_t* data,
                                         size_t        len,
                                         int8_t*       out) {
    int8_t cur_min = *out;
    size_t i       = 0u;

    if (len >= 16u) {
        int8x16_t vmin = vdupq_n_s8(INT8_MAX);

        for (; i + 16u <= len; i += 16u) {
            int8x16_t v = vld1q_s8(data + i);
            vmin = vminq_s8(vmin, v);
        }

        /* Horizontal reduction using NEON pairwise min */
        int8_t lane_min = vminvq_s8(vmin);
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

#endif /* SIMD_NEON_MIN_INT8_INL */
// ================================================================================
// ================================================================================
// eof
