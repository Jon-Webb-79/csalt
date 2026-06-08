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
#ifndef CSALT_SIMD_SVE2_DOUBLE_INL
#define CSALT_SIMD_SVE2_DOUBLE_INL

#include <arm_sve.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_double(const double* data,
                                  size_t        len,
                                  double        value,
                                  double        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    size_t vl = svcntd();
 
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
// -------------------------------------------------------------------------------- 

static bool simd_doubles_equal(const double* a,
                                const double* b,
                                size_t        len,
                                double        tolerance) {
    if (a == NULL || b == NULL) return false;
    if (len == 0u)              return true;
    if (a == b)                 return true;
 
    size_t      vl  = svcntd();
    svfloat64_t tol = svdup_n_f64(tolerance);
 
    size_t i = 0u;
    for (; i + vl <= len; i += vl) {
        svbool_t    pg      = svwhilelt_b64((uint64_t)i, (uint64_t)len);
        svfloat64_t va      = svld1_f64(pg, a + i);
        svfloat64_t vb      = svld1_f64(pg, b + i);
        svfloat64_t diff    = svsub_f64_z(pg, va, vb);
        svfloat64_t absdiff = svabs_f64_z(pg, diff);
        svbool_t    pass    = svcmple_f64(pg, absdiff, tol);
        if (svptest_any(pg, svnot_b_z(pg, pass))) return false;
    }
 
    for (; i < len; i++) {
        double diff = a[i] - b[i];
        if (diff != diff)     return false;
        if (diff < 0.0)       diff = -diff;
        if (diff > tolerance) return false;
    }
    return true;
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_double(const double* data,
                                           size_t        len,
                                           double*       out) {
    double   cur_min = *out;
    size_t   i       = 0u;
    uint64_t vl      = svcntd();
 
    if (len >= vl) {
        svfloat64_t vmin  = svdup_n_f64(INFINITY);
        svbool_t    ptrue = svptrue_b64();
 
        for (; i + vl <= len; i += vl) {
            svfloat64_t v = svld1_f64(ptrue, data + i);
 
            svbool_t nan_pred = svcmpuo_f64(ptrue, v, v);
            if (svptest_any(ptrue, nan_pred)) { *out = NAN; return NO_ERROR; }
 
            vmin = svmin_f64_x(ptrue, vmin, v);
        }
 
        cur_min = svminv_f64(ptrue, vmin);
        if (isinf(cur_min) && cur_min < 0.0) { *out = -INFINITY; return NO_ERROR; }
    }
 
    if (i < len) {
        svbool_t    pred = svwhilelt_b64((uint64_t)i, (uint64_t)len);
        svfloat64_t v    = svld1_f64(pred, data + i);
 
        svbool_t nan_pred = svcmpuo_f64(pred, v, v);
        if (svptest_any(pred, nan_pred)) { *out = NAN; return NO_ERROR; }
 
        svfloat64_t vmin = svdup_n_f64(cur_min);
        vmin = svmin_f64_x(pred, vmin, v);
        double tail_min = svminv_f64(pred, vmin);
        if (tail_min < cur_min) cur_min = tail_min;
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SVE2_DOUBLE_INL */

