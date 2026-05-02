// ================================================================================
// ================================================================================
// - File:    simd_scalar_lsearch_uint16.inl
// - Purpose: Scalar fallback implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements using a simple linear scan.
//
//            This file is included when no SIMD instruction set is
//            available or targeted.  It is correct for all array lengths
//            and serves as the reference implementation against which
//            all SIMD versions are validated.
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
#ifndef CSALT_SIMD_SCALAR_DOUBLE_INL
#define CSALT_SIMD_SCALAR_DOUBLE_INL

#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_double(const double* data,
                                  size_t        len,
                                  double        value,
                                  double        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    for (size_t i = 0u; i < len; i++) {
        double diff = data[i] - value;
        if (diff < 0.0) diff = -diff;
        if (diff <= tolerance) return i;
    }
    return SIZE_MAX;

// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SCALAR_DOUBLE_INL */

