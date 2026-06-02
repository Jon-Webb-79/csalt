// ================================================================================
// ================================================================================
// - File:    simd_sve2_min_int64.inl
// - Purpose: ARM SVE2 fast path for finding the minimum int64_t in a
//            contiguous buffer.  SVE2 does not add new signed-doubleword
//            min instructions beyond SVE, so the logic is identical to the
//            SVE variant.  This file exists to satisfy the per-ISA dispatch
//            convention.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SVE2_MIN_INT64_INL
#define SIMD_SVE2_MIN_INT64_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>
#include <arm_sve.h>

static inline error_code_t simd_min_int64(const int64_t* data,
                                          size_t         len,
                                          int64_t*       out) {
    int64_t  cur_min = *out;
    size_t   i       = 0u;
    uint64_t vl      = svcntd();

    if (len >= vl) {
        svint64_t vmin  = svdup_n_s64(INT64_MAX);
        svbool_t  ptrue = svptrue_b64();

        for (; i + vl <= len; i += vl) {
            svint64_t v = svld1_s64(ptrue, data + i);
            vmin = svmin_s64_x(ptrue, vmin, v);
        }

        int64_t lane_min = svminv_s64(ptrue, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == INT64_MIN) { *out = INT64_MIN; return NO_ERROR; }
    }

    /* Predicated tail */
    if (i < len) {
        svbool_t  pred = svwhilelt_b64((uint64_t)i, (uint64_t)len);
        svint64_t v    = svld1_s64(pred, data + i);
        svint64_t vmin = svdup_n_s64(cur_min);
        vmin = svmin_s64_x(pred, vmin, v);

        int64_t lane_min = svminv_s64(pred, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SVE2_MIN_INT64_INL */
// ================================================================================
// ================================================================================
// eof
