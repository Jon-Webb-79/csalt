// ================================================================================
// ================================================================================
// - File:    simd_scalar_min_int8.inl
// - Purpose: Scalar fallback for finding the minimum int8_t in a contiguous
//            buffer.  Early-exits when the global minimum (-128) is encountered.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SCALAR_MIN_INT8_INL
#define SIMD_SCALAR_MIN_INT8_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>

static inline error_code_t simd_min_int8(const int8_t* data,
                                         size_t        len,
                                         int8_t*       out) {
    int8_t cur_min = *out;               /* caller seeds with INT8_MAX */

    for (size_t i = 0u; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT8_MIN) {   /* global minimum — stop early */
                *out = INT8_MIN;
                return NO_ERROR;
            }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SCALAR_MIN_INT8_INL */
// ================================================================================
// ================================================================================
// eof
