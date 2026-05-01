// ================================================================================
// ================================================================================
// - File:    simd_sve2_lsearch_uint16.inl
// - Purpose: ARM SVE2 implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements using SVE2 scalable vector comparisons.
//
//            SVE2 is a strict superset of SVE.  For a linear search the
//            SVE and SVE2 implementations are identical in structure —
//            SVE2 adds polynomial and complex arithmetic extensions that
//            do not improve a simple equality search.  This file exists
//            as a distinct compilation unit so the build system can
//            select it when SVE2 is the highest available feature level,
//            allowing the compiler to use SVE2 scheduling and
//            microarchitectural optimisations transparently.
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
#ifndef CSALT_SIMD_SVE2_FLOAT_INL
#define CSALT_SIMD_SVE2_FLOAT_INL

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
 
    size_t vl = svcntw();
 
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
                svuint32_t lane_idx    = svindex_u32(0u, 1u);
                svbool_t   first_match = svbrka_b_z(match, match);
                uint32_t   lane = svlasta_u32(first_match, lane_idx);
                return i + (size_t)lane;
            }
        }
 
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
#endif /* CSALT_SIMD_SVE2_FLOAT_INL */

