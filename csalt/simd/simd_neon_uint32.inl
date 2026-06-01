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
#ifndef CSALT_SIMD_NEON_UINT32_INL
#define CSALT_SIMD_NEON_UINT32_INL

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
 * Convert a uint32x4_t comparison mask (lanes are 0x00000000 or 0xFFFFFFFF)
 * into a scalar bitmask with one bit per lane.
 * vshrn_n_u32 narrows each 32-bit lane to 8 bits (0xFF or 0x00),
 * then vget_lane_u64 extracts all 4 bytes as a 64-bit integer where
 * each byte represents one lane.
 */
static inline uint64_t _neon_mask_u32(uint32x4_t cmp) {
    uint8x8_t narrow = vshrn_n_u32(cmp, 4);
    uint64_t  mask64;
    vst1_u8((uint8_t*)&mask64, narrow);
    return mask64;
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static size_t simd_lsearch_uint32(const uint32_t* data,
                                  size_t          len,
                                  uint32_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 4u;
 
    if (len >= elems_per_reg) {
        uint32x4_t target = vdupq_n_u32(value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            uint32x4_t chunk = vld1q_u32(data + i);
            uint32x4_t cmp   = vceqq_u32(chunk, target);
 
            /* vmaxvq_u32 returns 0xFFFFFFFF if any lane matched */
            if (vmaxvq_u32(cmp) != 0u) {
                uint64_t mask = _neon_mask_u32(cmp);
                /* Each matching lane contributes 0xFF to one byte of mask64.
                 * __builtin_ctzll finds the first set bit; divide by 8 to
                 * get the lane index within the register. */
                return i + (size_t)(__builtin_ctzll(mask) / 8u);
            }
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

static inline error_code_t simd_min_uint32(const uint32_t* data,
                                           size_t          len,
                                           uint32_t*       out) {
    uint32_t cur_min = *out;
    size_t   i       = 0u;
 
    if (len >= 4u) {
        uint32x4_t vmin = vdupq_n_u32(UINT32_MAX);
 
        for (; i + 4u <= len; i += 4u) {
            uint32x4_t v = vld1q_u32(data + i);
            vmin = vminq_u32(vmin, v);
        }
 
        uint32_t lane_min = vminvq_u32(vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Scalar tail */
    for (; i < len; i++) {
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
#endif /* CSALT_SIMD_NEON_UINT32_INL */

