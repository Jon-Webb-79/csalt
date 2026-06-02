// ================================================================================
// ================================================================================
// - File:    simd_neon_lsearch_uint16.inl
// - Purpose: ARM NEON implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements using NEON 128-bit integer comparisons.
//
//            NEON provides vceqq_u16 which compares 8 uint16_t lanes
//            simultaneously and produces a uint16x8_t mask vector with
//            0xFFFF in matching lanes and 0x0000 elsewhere.
//
//            To extract the index of the first matching lane efficiently
//            on AArch64, vmaxvq_u16 checks whether any lane matched,
//            and vgetq_lane_u16 with a loop or vshrn_n_u16 + vmovmask
//            narrows the mask to a scalar for __builtin_ctz.
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
#ifndef CSALT_SIMD_NEON_UINT64_INL
#define CSALT_SIMD_NEON_UINT64_INL

#include "c_error.h"

#include <arm_neon.h>
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Static helpers
// ================================================================================
 
/**
 * Convert a uint64x2_t comparison mask (lanes are 0 or 0xFFFFFFFFFFFFFFFF)
 * into a scalar bitmask with one bit per lane.
 * vshrn_n_u64 narrows each 64-bit lane to 32 bits (0xFFFFFFFF or 0x00000000),
 * then vmovn_u32 narrows further to 16 bits per lane (0xFFFF or 0x0000),
 * and vget_lane_u32 extracts the result as a 32-bit scalar where
 * bits [15:0] represent lane 0 and bits [31:16] represent lane 1.
 */
static inline uint32_t _neon_mask_u64(uint64x2_t cmp) {
    uint32x2_t narrow32 = vshrn_n_u64(cmp, 32);
    uint16x4_t narrow16 = vmovn_u32(vcombine_u32(narrow32, narrow32));
    return vget_lane_u32(vreinterpret_u32_u16(narrow16), 0);
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static size_t simd_lsearch_uint64(const uint64_t* data,
                                  size_t          len,
                                  uint64_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 2u;
 
    if (len >= elems_per_reg) {
        uint64x2_t target = vdupq_n_u64(value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            uint64x2_t chunk = vld1q_u64(data + i);
            uint64x2_t cmp   = vceqq_u64(chunk, target);
 
            /* Check lane 0 first, then lane 1 to find first match */
            if (vgetq_lane_u64(cmp, 0) != 0u) return i;
            if (vgetq_lane_u64(cmp, 1) != 0u) return i + 1u;
        }
 
        /* Scalar remainder */
        for (; i < len; i++) {
            if (data[i] == value) return i;
        }
        return SIZE_MAX;
    }
 
    for (size_t i = 0u; i < len; i++) {
        if (data[i] == value) return i;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_uint64(const uint64_t* data,
                                           size_t          len,
                                           uint64_t*       out) {
    uint64_t cur_min = *out;
 
    for (size_t i = 0u; i < len; i++) {
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
        }
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_NEON_UINT64_INL */

