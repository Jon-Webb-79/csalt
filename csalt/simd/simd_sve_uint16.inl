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
#ifndef CSALT_SIMD_SVE_UINT16_INL
#define CSALT_SIMD_SVE_UINT16_INL

#include "c_error.h"

#include <arm_sve.h>
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint16(const uint16_t* data,
                                  size_t          len,
                                  uint16_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    size_t vl = svcnth();   /* number of uint16_t elements per SVE vector */
 
    if (len >= vl) {
        svuint16_t target = svdup_n_u16(value);
 
        size_t i = 0u;
        for (; i + vl <= len; i += vl) {
            svbool_t   pg    = svwhilelt_b16((uint64_t)i, (uint64_t)len);
            svuint16_t chunk = svld1_u16(pg, data + i);
            svbool_t   match = svcmpeq_u16(pg, chunk, target);
 
            if (svptest_any(pg, match)) {
                /* Find the index of the first true lane in match.
                 * svlasta extracts the first active element of an index
                 * vector under the match predicate. */
                svuint16_t lane_idx = svindex_u16(0u, 1u);
                /* Use svfirst to get the predicate of just the first match */
                svbool_t first_match = svbrka_b_z(match, match);
                uint16_t lane = svlasta_u16(first_match, lane_idx);
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

static inline error_code_t simd_min_uint16(const uint16_t* data,
                                           size_t          len,
                                           uint16_t*       out) {
    uint16_t cur_min = *out;
    size_t   i       = 0u;
    uint64_t vl      = svcnth();         /* uint16 elements per SVE register */
 
    if (len >= vl) {
        svuint16_t vmin  = svdup_n_u16(0xFFFF);
        svbool_t   ptrue = svptrue_b16();
 
        for (; i + vl <= len; i += vl) {
            svuint16_t v = svld1_u16(ptrue, data + i);
            vmin = svmin_u16_x(ptrue, vmin, v);
        }
 
        uint16_t lane_min = svminv_u16(ptrue, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Predicated tail */
    if (i < len) {
        svbool_t   pred = svwhilelt_b16((uint64_t)i, (uint64_t)len);
        svuint16_t v    = svld1_u16(pred, data + i);
        svuint16_t vmin = svdup_n_u16(cur_min);
        vmin = svmin_u16_x(pred, vmin, v);
 
        uint16_t lane_min = svminv_u16(pred, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SVE_UINT16_INL */

