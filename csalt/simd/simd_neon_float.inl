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
#ifndef CSALT_SIMD_NEON_FLOAT_INL
#define CSALT_SIMD_NEON_FLOAT_INL

#include "c_error.h"

#include <arm_neon.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
// ================================================================================ 
// ================================================================================ 

/**
 * Convert a uint32x4_t comparison mask (lanes are 0x00000000 or 0xFFFFFFFF)
 * into a scalar bitmask with one bit per lane for __builtin_ctzll.
 */
static inline uint64_t _neon_mask_f32(uint32x4_t cmp) {
    uint8x8_t narrow = vshrn_n_u32(cmp, 4);
    uint64_t  mask64;
    vst1_u8((uint8_t*)&mask64, narrow);
    return mask64;
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static size_t simd_lsearch_float(const float* data,
                                 size_t       len,
                                 float        value,
                                 float        tolerance) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 4u;
 
    if (len >= elems_per_reg) {
        float32x4_t target = vdupq_n_f32(value);
        float32x4_t tol    = vdupq_n_f32(tolerance);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            float32x4_t chunk   = vld1q_f32(data + i);
            float32x4_t diff    = vsubq_f32(chunk, target);
            float32x4_t absdiff = vabsq_f32(diff);
            /* vcleq_f32: absdiff <= tol, element-wise */
            uint32x4_t  cmp     = vcleq_f32(absdiff, tol);
 
            /* vmaxvq_u32: returns non-zero if any lane matched */
            if (vmaxvq_u32(cmp) != 0u) {
                uint64_t mask = _neon_mask_f32(cmp);
                return i + (size_t)(__builtin_ctzll(mask) / 8u);
            }
        }
 
        /* Scalar remainder */
        for (; i < len; i++) {
            float diff = data[i] - value;
            if (diff < 0.0f) diff = -diff;
            if (diff <= tolerance) return i;
        }
        return SIZE_MAX;
    }
 
    for (size_t i = 0u; i < len; i++) {
        float diff = data[i] - value;
        if (diff < 0.0f) diff = -diff;
        if (diff <= tolerance) return i;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static bool simd_floats_equal(const float* a,
                               const float* b,
                               size_t       len,
                               float        tolerance) {
    if (a == NULL || b == NULL) return false;
    if (len == 0u)              return true;
    if (a == b)                 return true;
 
    float32x4_t tol = vdupq_n_f32(tolerance);
 
    size_t i = 0u;
    for (; i + 4u <= len; i += 4u) {
        float32x4_t va      = vld1q_f32(a + i);
        float32x4_t vb      = vld1q_f32(b + i);
        float32x4_t diff    = vsubq_f32(va, vb);
        float32x4_t absdiff = vabsq_f32(diff);
        uint32x4_t  cmp     = vcleq_f32(absdiff, tol);
        if (vminvq_u32(cmp) == 0u) return false;
    }
 
    for (; i < len; i++) {
        float diff = a[i] - b[i];
        if (diff != diff)     return false;
        if (diff < 0.0f)      diff = -diff;
        if (diff > tolerance) return false;
    }
    return true;
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_float(const float* data,
                                          size_t       len,
                                          float*       out) {
    float  cur_min = *out;
    size_t i       = 0u;
 
    if (len >= 4u) {
        float32x4_t vmin    = vdupq_n_f32(*out);
        uint32x4_t  has_nan = vdupq_n_u32(0u);
 
        for (; i + 4u <= len; i += 4u) {
            float32x4_t v = vld1q_f32(data + i);
 
            /* NaN check: vceqq_f32(v, v) is 0 for NaN lanes */
            uint32x4_t nan_mask = vmvnq_u32(vceqq_f32(v, v));
            has_nan = vorrq_u32(has_nan, nan_mask);
 
            vmin = vminq_f32(vmin, v);
 
            /* Early-exit on NaN */
            if (vmaxvq_u32(has_nan)) { *out = NAN; return NO_ERROR; }
        }
 
        cur_min = vminvq_f32(vmin);
        if (isinf(cur_min) && cur_min < 0.0f) { *out = -INFINITY; return NO_ERROR; }
    }
 
    /* Scalar tail */
    for (; i < len; i++) {
        if (isnan(data[i])) { *out = NAN; return NO_ERROR; }
        if (data[i] < cur_min) {
            cur_min = data[i];
            if (isinf(cur_min) && cur_min < 0.0f) { *out = -INFINITY; return NO_ERROR; }
        }
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_NEON_FLOAT_INL */

