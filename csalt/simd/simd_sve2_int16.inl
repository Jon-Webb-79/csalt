// ================================================================================
// ================================================================================
// - File:    simd_sve2_min_int16.inl
// - Purpose: ARM SVE2 fast path for finding the minimum int16_t in a
//            contiguous buffer.  SVE2 does not add new signed-halfword min
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

#ifndef SIMD_SVE2_MIN_INT16_INL
#define SIMD_SVE2_MIN_INT16_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <arm_sve.h>

static inline error_code_t simd_min_int16(const int16_t* data,
                                          size_t         len,
                                          int16_t*       out) {
    int16_t  cur_min = *out;
    size_t   i       = 0u;
    uint64_t vl      = svcnth();

    if (len >= vl) {
        svint16_t vmin  = svdup_n_s16(INT16_MAX);
        svbool_t  ptrue = svptrue_b16();

        for (; i + vl <= len; i += vl) {
            svint16_t v = svld1_s16(ptrue, data + i);
            vmin = svmin_s16_x(ptrue, vmin, v);
        }

        int16_t lane_min = svminv_s16(ptrue, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT16_MIN) { *out = INT16_MIN; return NO_ERROR; }
    }

    /* Predicated tail */
    if (i < len) {
        svbool_t  pred = svwhilelt_b16((uint64_t)i, (uint64_t)len);
        svint16_t v    = svld1_s16(pred, data + i);
        svint16_t vmin = svdup_n_s16(cur_min);
        vmin = svmin_s16_x(pred, vmin, v);

        int16_t lane_min = svminv_s16(pred, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SVE2_MIN_INT16_INL */
// ================================================================================
// ================================================================================
// eof
