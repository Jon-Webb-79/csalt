// ================================================================================
// ================================================================================
// - File:    simd_scalar_min_int64.inl
// - Purpose: Scalar fallback for finding the minimum int64_t in a contiguous
//            buffer.  Early-exits when the global minimum (INT64_MIN) is
//            encountered.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SCALAR_MIN_INT64_INL
#define SIMD_SCALAR_MIN_INT64_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>

static inline error_code_t simd_min_int64(const int64_t* data,
                                          size_t         len,
                                          int64_t*       out) {
    int64_t cur_min = *out;              /* caller seeds with INT64_MAX */

    for (size_t i = 0u; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT64_MIN) {
                *out = INT64_MIN;
                return NO_ERROR;
            }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SCALAR_MIN_INT64_INL */
// ================================================================================
// ================================================================================
// eof
