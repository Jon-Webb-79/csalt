// ================================================================================
// ================================================================================
// - File:    simd_scalar_min_int16.inl
// - Purpose: Scalar fallback for finding the minimum int16_t in a contiguous
//            buffer.  Early-exits when the global minimum (INT16_MIN) is
//            encountered.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef SIMD_SCALAR_MIN_INT16_INL
#define SIMD_SCALAR_MIN_INT16_INL

#include "c_error.h"
#include <stdint.h>
#include <stddef.h>

static inline error_code_t simd_min_int16(const int16_t* data,
                                          size_t         len,
                                          int16_t*       out) {
    int16_t cur_min = *out;              /* caller seeds with INT16_MAX */

    for (size_t i = 0u; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == INT16_MIN) {  /* global minimum — stop early */
                *out = INT16_MIN;
                return NO_ERROR;
            }
        }
    }

    *out = cur_min;
    return NO_ERROR;
}

#endif /* SIMD_SCALAR_MIN_INT16_INL */
// ================================================================================
// ================================================================================
// eof
