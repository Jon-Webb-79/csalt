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
#ifndef CSALT_SIMD_SVE_DOUBLE_INL
#define CSALT_SIMD_SVE_DOUBLE_INL

#include <arm_sve.h>
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_double(const double* data,
                                  size_t        len,
                                  double        value,
                                  double        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    size_t vl = svcntd();   /* number of double elements per SVE vector */
 
    if (len >= vl) {
        svfloat64_t target = svdup_n_f64(value);
        svfloat64_t tol    = svdup_n_f64(tolerance);
 
        size_t i = 0u;
        for (; i + vl <= len; i += vl) {
            svbool_t    pg      = svwhilelt_b64((uint64_t)i, (uint64_t)len);
            svfloat64_t chunk   = svld1_f64(pg, data + i);
            svfloat64_t diff    = svsub_f64_z(pg, chunk, target);
            svfloat64_t absdiff = svabs_f64_z(pg, diff);
            svbool_t    match   = svcmple_f64(pg, absdiff, tol);
 
            if (svptest_any(pg, match)) {
                svuint64_t lane_idx    = svindex_u64(0u, 1u);
                svbool_t   first_match = svbrka_b_z(match, match);
                uint64_t   lane = svlasta_u64(first_match, lane_idx);
                return i + (size_t)lane;
            }
        }
 
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
#endif /* CSALT_SIMD_SVE_DOUBLE_INL */

