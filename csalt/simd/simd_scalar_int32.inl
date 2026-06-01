// ================================================================================
// ================================================================================
// - File:    simd_scalar_min_int32.inl
// - Purpose: Scalar fallback for finding the minimum int32_t in a contiguous
//            buffer.  Early-exits when the global minimum (INT32_MIN) is
//            encountered.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SCALAR_MIN_INT32_INL
#define SIMD_SCALAR_MIN_INT32_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>

static inline error_code_t simd_min_int32(const int32_t* data,
                                          size_t         len,
                                          int32_t*       out) {
    int32_t cur_min = *out;              /* caller seeds with INT32_MAX */

    for (size_t i = 0u; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT32_MIN) {
                *out = INT32_MIN;
                return NO_ERROR;
            }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SCALAR_MIN_INT32_INL */
// ================================================================================
// ================================================================================
// eof
