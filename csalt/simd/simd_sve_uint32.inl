// ================================================================================
// ================================================================================
// - File:    simd_sve_lsearch_uint16.inl
// - Purpose: ARM SVE implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements using SVE scalable vector comparisons.
//
//            SVE provides svcmpeq_u16 which compares all active lanes
//            simultaneously and returns a predicate register.  The
//            vector length is determined at runtime via svcnth() (number
//            of uint16_t elements per vector).  svptest_first checks
//            whether the first active true lane exists, and
//            svlasta_u16 with a first-true predicate extracts its index
//            via the lane position computed from the predicate.
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
#ifndef CSALT_SIMD_SVE_UINT32_INL
#define CSALT_SIMD_SVE_UINT32_INL

#include "c_error.h"

#include <arm_sve.h>
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint32(const uint32_t* data,
                                  size_t          len,
                                  uint32_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    size_t vl = svcntw();   /* number of uint32_t elements per SVE vector */
 
    if (len >= vl) {
        svuint32_t target = svdup_n_u32(value);
 
        size_t i = 0u;
        for (; i + vl <= len; i += vl) {
            svbool_t   pg    = svwhilelt_b32((uint64_t)i, (uint64_t)len);
            svuint32_t chunk = svld1_u32(pg, data + i);
            svbool_t   match = svcmpeq_u32(pg, chunk, target);
 
            if (svptest_any(pg, match)) {
                svuint32_t lane_idx   = svindex_u32(0u, 1u);
                svbool_t   first_match = svbrka_b_z(match, match);
                uint32_t   lane = svlasta_u32(first_match, lane_idx);
                return i + (size_t)lane;
            }
        }
 
        /* Scalar remainder */
        for (; i < len; i++) {
            if (data[i] == value) return i;
        }
        return SIZE_MAX;
    }
 
    for (size_t i = 0u; i < len; i++) {
        if (data[i] == value) return i;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_uint32(const uint32_t* data,
                                           size_t          len,
                                           uint32_t*       out) {
    uint32_t cur_min = *out;
    size_t   i       = 0u;
    uint64_t vl      = svcntw();         /* uint32 elements per SVE register */
 
    if (len >= vl) {
        svuint32_t vmin  = svdup_n_u32(UINT32_MAX);
        svbool_t   ptrue = svptrue_b32();
 
        for (; i + vl <= len; i += vl) {
            svuint32_t v = svld1_u32(ptrue, data + i);
            vmin = svmin_u32_x(ptrue, vmin, v);
        }
 
        uint32_t lane_min = svminv_u32(ptrue, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Predicated tail */
    if (i < len) {
        svbool_t   pred = svwhilelt_b32((uint64_t)i, (uint64_t)len);
        svuint32_t v    = svld1_u32(pred, data + i);
        svuint32_t vmin = svdup_n_u32(cur_min);
        vmin = svmin_u32_x(pred, vmin, v);
 
        uint32_t lane_min = svminv_u32(pred, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SVE_UINT32_INL */

