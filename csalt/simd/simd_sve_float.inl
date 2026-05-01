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
#ifndef CSALT_SIMD_SVE_FLOAT_INL
#define CSALT_SIMD_SVE_FLOAT_INL

#include <arm_sve.h>
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_float(const float* data,
                                 size_t       len,
                                 float        value,
                                 float        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    size_t vl = svcntw();   /* number of float elements per SVE vector */
 
    if (len >= vl) {
        svfloat32_t target = svdup_n_f32(value);
        svfloat32_t tol    = svdup_n_f32(tolerance);
 
        size_t i = 0u;
        for (; i + vl <= len; i += vl) {
            svbool_t    pg      = svwhilelt_b32((uint64_t)i, (uint64_t)len);
            svfloat32_t chunk   = svld1_f32(pg, data + i);
            svfloat32_t diff    = svsub_f32_z(pg, chunk, target);
            svfloat32_t absdiff = svabs_f32_z(pg, diff);
            svbool_t    match   = svcmple_f32(pg, absdiff, tol);
 
            if (svptest_any(pg, match)) {
                svuint32_t lane_idx   = svindex_u32(0u, 1u);
                svbool_t   first_match = svbrka_b_z(match, match);
                uint32_t   lane = svlasta_u32(first_match, lane_idx);
                return i + (size_t)lane;
            }
        }
 
        /* Scalar remainder */
        for (; i < len; i++) {
            float diff = data[i] - value;
            if (diff < 0.0f) diff = -diff;
            if (diff <= tolerance) return i;
        }
        return SIZE_MAX;
    }
 
    for (size_t i = 0u; i < len; i++) {
        float diff = data[i] - value;
        if (diff < 0.0f) diff = -diff;
        if (diff <= tolerance) return i;
    }
    return SIZE_MAX;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SVE_FLOAT_INL */

