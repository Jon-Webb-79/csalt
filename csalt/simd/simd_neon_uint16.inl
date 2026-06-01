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
#ifndef CSALT_SIMD_NEON_UINT16_INL
#define CSALT_SIMD_NEON_UINT16_INL

#include "c_error.h"

#include <arm_neon.h>
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

/**
 * Convert a uint16x8_t comparison mask (lanes are 0x0000 or 0xFFFF) into
 * a scalar bitmask with one bit per lane.  Bit N is set if lane N matched.
 * Uses vshrn_n_u16 to narrow each 16-bit lane to 8 bits (keeping the high
 * byte of 0xFFFF = 0xFF, or 0x00), then vget_lane_u64 to extract all 8
 * bytes as a 64-bit integer where each byte represents one lane.
 */
static inline uint64_t _neon_mask_u16(uint16x8_t cmp) {
    /* Shift each 16-bit lane right by 4 to get 0x0FFF or 0x0000,
     * then narrow to 8-bit lanes: 0xFF or 0x00. */
    uint8x8_t  narrow = vshrn_n_u16(cmp, 4);
    uint64_t   mask64;
    vst1_u8((uint8_t*)&mask64, narrow);
    return mask64;
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static size_t simd_lsearch_uint16(const uint16_t* data,
                                  size_t          len,
                                  uint16_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 8u;
 
    if (len >= elems_per_reg) {
        uint16x8_t target = vdupq_n_u16(value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            uint16x8_t chunk = vld1q_u16(data + i);
            uint16x8_t cmp   = vceqq_u16(chunk, target);
 
            /* Fast path: vmaxvq_u16 returns 0xFFFF if any lane matched */
            if (vmaxvq_u16(cmp) != 0u) {
                uint64_t mask = _neon_mask_u16(cmp);
                /* Each matching lane contributes 0xFF to one byte of mask64.
                 * __builtin_ctzll finds the first set bit; divide by 8 to
                 * get the lane index. */
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

static inline error_code_t simd_min_uint16(const uint16_t* data,
                                           size_t          len,
                                           uint16_t*       out) {
    uint16_t cur_min = *out;
    size_t   i       = 0u;
 
    if (len >= 8u) {
        uint16x8_t vmin = vdupq_n_u16(0xFFFF);
 
        for (; i + 8u <= len; i += 8u) {
            uint16x8_t v = vld1q_u16(data + i);
            vmin = vminq_u16(vmin, v);
        }
 
        uint16_t lane_min = vminvq_u16(vmin);
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
#endif /* CSALT_SIMD_NEON_UINT16_INL */

