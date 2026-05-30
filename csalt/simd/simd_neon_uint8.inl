
#ifndef CSALT_SIMD_NEON_UINT8_INL
#define CSALT_SIMD_NEON_UINT8_INL

#include "c_error.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <arm_sve.h>
// ================================================================================ 
// ================================================================================ 

/**
 * Reverse element order within a 128-bit NEON register (uint8x16_t) without
 * disturbing the internal byte order of each element.
 *
 * vqtbl1q_u8 performs a table lookup on the full 16-byte vector, equivalent
 * to x86 pshufb.  Indices >= 16 produce zero — all indices here are < 16
 * so this is not a concern.
 */
static inline uint8x16_t _neon_reverse_elems(uint8x16_t v, size_t data_size) {
    uint8x16_t mask;
    switch (data_size) {
        case 1u: {
            uint8_t m[16] = {
                15, 14, 13, 12, 11, 10,  9,  8,
                 7,  6,  5,  4,  3,  2,  1,  0
            };
            mask = vld1q_u8(m);
            break;
        }
        case 2u: {
            uint8_t m[16] = {
                14, 15, 12, 13, 10, 11,  8,  9,
                 6,  7,  4,  5,  2,  3,  0,  1
            };
            mask = vld1q_u8(m);
            break;
        }
        case 4u: {
            uint8_t m[16] = {
                12, 13, 14, 15,  8,  9, 10, 11,
                 4,  5,  6,  7,  0,  1,  2,  3
            };
            mask = vld1q_u8(m);
            break;
        }
        case 8u: {
            uint8_t m[16] = {
                 8,  9, 10, 11, 12, 13, 14, 15,
                 0,  1,  2,  3,  4,  5,  6,  7
            };
            mask = vld1q_u8(m);
            break;
        }
        default:
            return v;
    }
    return vqtbl1q_u8(v, mask);
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;
 
    /* NEON path: 16-byte registers, element sizes with defined masks */
    if (data_size <= 8u &&
        (16u % data_size == 0u) &&
        data_size != 3u &&
        data_size != 5u &&
        data_size != 6u &&
        data_size != 7u) {
 
        size_t lo            = 0u;
        size_t hi            = len - 1u;
        size_t elems_per_reg = 16u / data_size;
 
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
 
            if ((hi - lo + 1u) >= 2u * elems_per_reg) {
                uint8x16_t vlo = vld1q_u8(lo_ptr);
                uint8x16_t vhi = vld1q_u8(hi_ptr);
                vlo = _neon_reverse_elems(vlo, data_size);
                vhi = _neon_reverse_elems(vhi, data_size);
                vst1q_u8(hi_ptr, vlo);
                vst1q_u8(lo_ptr, vhi);
                lo += elems_per_reg;
                hi -= elems_per_reg;
                continue;
            }
 
            /* Scalar remainder */
            uint8_t tmp[16];
            memcpy(tmp,    lo_ptr, data_size);
            memcpy(lo_ptr, hi_ptr, data_size);
            memcpy(hi_ptr, tmp,    data_size);
            lo++;
            hi--;
        }
        return;
    }
 
    /* Scalar fallback — correct for any data_size */
    size_t lo = 0u;
    size_t hi = len - 1u;
    uint8_t tmp[256];
 
    if (data_size <= sizeof(tmp)) {
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
            memcpy(tmp,    lo_ptr, data_size);
            memcpy(lo_ptr, hi_ptr, data_size);
            memcpy(hi_ptr, tmp,    data_size);
            lo++; hi--;
        }
    } else {
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
            for (size_t b = 0u; b < data_size; b++) {
                uint8_t byte = lo_ptr[b];
                lo_ptr[b]    = hi_ptr[b];
                hi_ptr[b]    = byte;
            }
            lo++; hi--;
        }
    }
}
// -------------------------------------------------------------------------------- 

static inline error_code_t simd_min_uint8(const uint8_t* data,
                                          size_t         len,
                                          uint8_t*       out) {
    uint8_t cur_min = *out;
    size_t  i       = 0u;
 
    if (len >= 16u) {
        uint8x16_t vmin = vdupq_n_u8(0xFF);
 
        for (; i + 16u <= len; i += 16u) {
            uint8x16_t v = vld1q_u8(data + i);
            vmin = vminq_u8(vmin, v);
        }
 
        /* Horizontal reduction using NEON pairwise min */
        uint8_t lane_min = vminvq_u8(vmin);
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
#endif /* CSALT_SIMD_NEON_UINT8_INL */

