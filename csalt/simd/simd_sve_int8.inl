// ================================================================================
// ================================================================================
// - File:    simd_sve_min_int8.inl
// - Purpose: ARM SVE fast path for finding the minimum int8_t in a contiguous
//            buffer.  Uses predicated loads and svmin_s8_x for the vertical
//            pass and svminv_s8 for the final horizontal reduction.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SVE_MIN_INT8_INL
#define SIMD_SVE_MIN_INT8_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <arm_sve.h>

static inline error_code_t simd_min_int8(const int8_t* data,
                                         size_t        len,
                                         int8_t*       out) {
    int8_t   cur_min = *out;
    size_t   i       = 0u;
    uint64_t vl      = svcntb();         /* bytes per SVE register */

    if (len >= vl) {
        svint8_t vmin  = svdup_n_s8(INT8_MAX);
        svbool_t ptrue = svptrue_b8();

        for (; i + vl <= len; i += vl) {
            svint8_t v = svld1_s8(ptrue, data + i);
            vmin = svmin_s8_x(ptrue, vmin, v);
        }

        int8_t lane_min = svminv_s8(ptrue, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT8_MIN) { *out = INT8_MIN; return NO_ERROR; }
    }

    /* Predicated tail — handles remaining elements in one vector op */
    if (i < len) {
        svbool_t  pred = svwhilelt_b8((uint64_t)i, (uint64_t)len);
        svint8_t  v    = svld1_s8(pred, data + i);
        svint8_t  vmin = svdup_n_s8(cur_min);
        vmin = svmin_s8_x(pred, vmin, v);

        int8_t lane_min = svminv_s8(pred, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SVE_MIN_INT8_INL */
// ================================================================================
// ================================================================================
// eof
