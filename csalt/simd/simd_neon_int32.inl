// ================================================================================
// ================================================================================
// - File:    simd_neon_min_int32.inl
// - Purpose: ARM NEON fast path for finding the minimum int32_t in a
//            contiguous buffer.  Processes 4 elements per iteration using
//            vminq_s32.  Early-exits when the running minimum reaches
//            INT32_MIN.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_NEON_MIN_INT32_INL
#define SIMD_NEON_MIN_INT32_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <arm_neon.h>

static inline error_code_t simd_min_int32(const int32_t* data,
                                          size_t         len,
                                          int32_t*       out) {
    int32_t cur_min = *out;
    size_t  i       = 0u;

    if (len >= 4u) {
        int32x4_t vmin = vdupq_n_s32(INT32_MAX);

        for (; i + 4u <= len; i += 4u) {
            int32x4_t v = vld1q_s32(data + i);
            vmin = vminq_s32(vmin, v);
        }

        int32_t lane_min = vminvq_s32(vmin);
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

#endif /* SIMD_NEON_MIN_INT32_INL */
// ================================================================================
// ================================================================================
// eof
