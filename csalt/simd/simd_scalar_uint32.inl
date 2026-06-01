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
#ifndef CSALT_SIMD_SCALAR_UINT32_INL
#define CSALT_SIMD_SCALAR_UINT32_INL

#include "c_error.h"

#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint32(const uint32_t* data,
                                  size_t          len,
                                  uint32_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    for (size_t i = 0u; i < len; i++) {
        if (data[i] == value) return i;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_uint32(const uint32_t* data,
                                           size_t          len,
                                           uint32_t*       out) {
    uint32_t cur_min = *out;             /* caller seeds with UINT32_MAX */
 
    for (size_t i = 0u; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == 0u) {
                *out = 0u;
                return NO_ERROR;
            }
        }
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SCALAR_UINT32_INL */

