// ================================================================================
// ================================================================================
// - File:    simd_sve2_min_int32.inl
// - Purpose: ARM SVE2 fast path for finding the minimum int32_t in a
//            contiguous buffer.  SVE2 does not add new signed-word min
//            instructions beyond SVE, so the logic is identical to the SVE
//            variant.  This file exists to satisfy the per-ISA dispatch
//            convention.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SVE2_MIN_INT32_INL
#define SIMD_SVE2_MIN_INT32_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <arm_sve.h>

static inline error_code_t simd_min_int32(const int32_t* data,
                                          size_t         len,
                                          int32_t*       out) {
    int32_t  cur_min = *out;
    size_t   i       = 0u;
    uint64_t vl      = svcntw();

    if (len >= vl) {
        svint32_t vmin  = svdup_n_s32(INT32_MAX);
        svbool_t  ptrue = svptrue_b32();

        for (; i + vl <= len; i += vl) {
            svint32_t v = svld1_s32(ptrue, data + i);
            vmin = svmin_s32_x(ptrue, vmin, v);
        }

        int32_t lane_min = svminv_s32(ptrue, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT32_MIN) { *out = INT32_MIN; return NO_ERROR; }
    }

    /* Predicated tail */
    if (i < len) {
        svbool_t  pred = svwhilelt_b32((uint64_t)i, (uint64_t)len);
        svint32_t v    = svld1_s32(pred, data + i);
        svint32_t vmin = svdup_n_s32(cur_min);
        vmin = svmin_s32_x(pred, vmin, v);

        int32_t lane_min = svminv_s32(pred, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SVE2_MIN_INT32_INL */
// ================================================================================
// ================================================================================
// eof
