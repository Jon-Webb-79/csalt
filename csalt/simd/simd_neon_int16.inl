// ================================================================================
// ================================================================================
// - File:    simd_neon_min_int16.inl
// - Purpose: ARM NEON fast path for finding the minimum int16_t in a
//            contiguous buffer.  Processes 8 elements per iteration using
//            vminq_s16.  Early-exits when the running minimum reaches
//            INT16_MIN (-32768).
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_NEON_MIN_INT16_INL
#define SIMD_NEON_MIN_INT16_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <arm_neon.h>

static inline error_code_t simd_min_int16(const int16_t* data,
                                          size_t         len,
                                          int16_t*       out) {
    int16_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 8u) {
        int16x8_t vmin = vdupq_n_s16(INT16_MAX);

        for (; i + 8u <= len; i += 8u) {
            int16x8_t v = vld1q_s16(data + i);
            vmin = vminq_s16(vmin, v);
        }

        int16_t lane_min = vminvq_s16(vmin);
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

#endif /* SIMD_NEON_MIN_INT16_INL */
// ================================================================================
// ================================================================================
// eof
