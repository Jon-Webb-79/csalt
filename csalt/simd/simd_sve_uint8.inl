
#ifndef CSALT_SIMD_SVE_UINT8_INL
#define CSALT_SIMD_SVE_UINT8_INL

#include "c_error.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <arm_sve.h>
// ================================================================================ 
// ================================================================================ 

/**
 * Build an SVE index vector that reverses element positions for elements of
 * data_size bytes within a vector of vl bytes.
 *
 * For data_size == 1: index[i] = vl - 1 - i  (byte-level reverse)
 * For data_size == d: index[i] = ((vl/d - 1 - i/d) * d) + (i % d)
 *                               (reverse element order, preserve intra-element bytes)
 */
static svuint8_t _sve_reverse_index(size_t vl, size_t data_size) {
    uint8_t idx[2048];   /* SVE max width is 2048 bits = 256 bytes */
    size_t nelems = vl / data_size;
    for (size_t i = 0u; i < vl; i++) {
        size_t elem    = i / data_size;
        size_t byte_in = i % data_size;
        size_t rev_elem = nelems - 1u - elem;
        idx[i] = (uint8_t)(rev_elem * data_size + byte_in);
    }
    svbool_t pg = svwhilelt_b8((uint64_t)0u, (uint64_t)vl);
    return svld1_u8(pg, idx);
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;
 
    /* SVE path: valid for element sizes that evenly divide the vector length.
     * The vector length is runtime-variable so we check divisibility at
     * runtime rather than at compile time. */
    size_t vl = svcntb();   /* vector length in bytes */
 
    if (data_size <= vl && (vl % data_size == 0u)) {
        size_t elems_per_reg = vl / data_size;
        svuint8_t vidx       = _sve_reverse_index(vl, data_size);
        svbool_t  pg         = svwhilelt_b8((uint64_t)0u, (uint64_t)vl);
 
        size_t lo = 0u;
        size_t hi = len - 1u;
 
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
 
            if ((hi - lo + 1u) >= 2u * elems_per_reg) {
                svuint8_t vlo = svld1_u8(pg, lo_ptr);
                svuint8_t vhi = svld1_u8(pg, hi_ptr);
                vlo = svtbl_u8(vlo, vidx);
                vhi = svtbl_u8(vhi, vidx);
                svst1_u8(pg, hi_ptr, vlo);
                svst1_u8(pg, lo_ptr, vhi);
                lo += elems_per_reg;
                hi -= elems_per_reg;
                continue;
            }
 
            /* Scalar remainder */
            uint8_t tmp[256];
            size_t copy_bytes = data_size < sizeof(tmp) ? data_size : sizeof(tmp);
            if (data_size <= sizeof(tmp)) {
                memcpy(tmp,    lo_ptr, data_size);
                memcpy(lo_ptr, hi_ptr, data_size);
                memcpy(hi_ptr, tmp,    data_size);
            } else {
                for (size_t b = 0u; b < data_size; b++) {
                    uint8_t byte = lo_ptr[b];
                    lo_ptr[b]    = hi_ptr[b];
                    hi_ptr[b]    = byte;
                }
            }
            (void)copy_bytes;
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
    uint8_t  cur_min = *out;
    size_t   i       = 0u;
    uint64_t vl      = svcntb();         /* bytes per SVE register */
 
    if (len >= vl) {
        svuint8_t vmin = svdup_n_u8(0xFF);
        svbool_t  ptrue = svptrue_b8();
 
        for (; i + vl <= len; i += vl) {
            svuint8_t v = svld1_u8(ptrue, data + i);
            vmin = svmin_u8_x(ptrue, vmin, v);
        }
 
        uint8_t lane_min = svminv_u8(ptrue, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Predicated tail — handles remaining elements in one vector op */
    if (i < len) {
        svbool_t  pred = svwhilelt_b8((uint64_t)i, (uint64_t)len);
        svuint8_t v    = svld1_u8(pred, data + i);
        svuint8_t vmin = svdup_n_u8(cur_min);
        vmin = svmin_u8_x(pred, vmin, v);
 
        uint8_t lane_min = svminv_u8(pred, vmin);
        if (lane_min < cur_min) cur_min = lane_min;
    }
 
    *out = cur_min;
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SVE_UINT8_INL */

