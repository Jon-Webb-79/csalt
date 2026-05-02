// ================================================================================
// ================================================================================
// - File:    simd_neon_lsearch_uint16.inl
// - Purpose: ARM NEON implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements using NEON 128-bit integer comparisons.
//
//            NEON provides vceqq_u16 which compares 8 uint16_t lanes
//            simultaneously and produces a uint16x8_t mask vector with
//            0xFFFF in matching lanes and 0x0000 elsewhere.
//
//            To extract the index of the first matching lane efficiently
//            on AArch64, vmaxvq_u16 checks whether any lane matched,
//            and vgetq_lane_u16 with a loop or vshrn_n_u16 + vmovmask
//            narrows the mask to a scalar for __builtin_ctz.
//
//            Returns the zero-based index of the first match, or
//            SIZE_MAX if the value is not found.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 27, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
#ifndef CSALT_SIMD_NEON_DOUBLE_INL
#define CSALT_SIMD_NEON_DOUBLE_INL

#include <arm_neon.h>
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_double(const double* data,
                                  size_t        len,
                                  double        value,
                                  double        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 2u;
 
    if (len >= elems_per_reg) {
        float64x2_t target = vdupq_n_f64(value);
        float64x2_t tol    = vdupq_n_f64(tolerance);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            float64x2_t chunk   = vld1q_f64(data + i);
            float64x2_t diff    = vsubq_f64(chunk, target);
            float64x2_t absdiff = vabsq_f64(diff);
            /* vcleq_f64: absdiff <= tol, element-wise */
            uint64x2_t  cmp     = vcleq_f64(absdiff, tol);
 
            /* With only 2 lanes, check each directly */
            if (vgetq_lane_u64(cmp, 0) != 0u) return i;
            if (vgetq_lane_u64(cmp, 1) != 0u) return i + 1u;
        }
 
        /* Scalar remainder */
        for (; i < len; i++) {
            double diff = data[i] - value;
            if (diff < 0.0) diff = -diff;
            if (diff <= tolerance) return i;
        }
        return SIZE_MAX;
    }
 
    for (size_t i = 0u; i < len; i++) {
        double diff = data[i] - value;
        if (diff < 0.0) diff = -diff;
        if (diff <= tolerance) return i;
    }
    return SIZE_MAX;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_NEON_DOUBLE_INL */

