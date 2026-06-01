// ================================================================================
// ================================================================================
// - File:    simd_ssse3_lsearch_uint16.inl
// - Purpose: SSSE3 implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements.
//
//            SSSE3 adds no new integer comparison instructions beyond
//            SSE2 for this use case.  The implementation is identical
//            to the SSE2 version but compiled under a SSSE3 target,
//            which allows the compiler to use additional addressing
//            modes and scheduling improvements available in SSSE3
//            processors.  The file is provided as a distinct compilation
//            unit so the build system can select it when SSSE3 is the
//            highest available feature level.
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
#ifndef CSALT_SIMD_SSE3_UINT32_INL
#define CSALT_SIMD_SSE3_UINT32_INL

#include "c_error.h"

#include <tmmintrin.h>   /* SSSE3 (includes SSE2, SSE3) */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint32(const uint32_t* data,
                                  size_t          len,
                                  uint32_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 4u;
 
#if defined(__SSSE3__)
    if (len >= elems_per_reg) {
        __m128i target = _mm_set1_epi32((int32_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _mm_cmpeq_epi32(chunk, target);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 4u);
            }
        }
 
        for (; i < len; i++) {
            if (data[i] == value) return i;
        }
        return SIZE_MAX;
    }
#endif
 
    for (size_t i = 0u; i < len; i++) {
        if (data[i] == value) return i;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline __m128i _ssse3_min_epu32(__m128i a, __m128i b, __m128i bias) {
    __m128i a_biased = _mm_xor_si128(a, bias);
    __m128i b_biased = _mm_xor_si128(b, bias);
    __m128i mask     = _mm_cmplt_epi32(a_biased, b_biased);
    return _mm_or_si128(_mm_and_si128(mask, a),
                        _mm_andnot_si128(mask, b));
}
 
static inline error_code_t simd_min_uint32(const uint32_t* data,
                                           size_t          len,
                                           uint32_t*       out) {
    uint32_t cur_min = *out;
    size_t   i       = 0u;
 
    if (len >= 4u) {
        __m128i bias = _mm_set1_epi32((int)0x80000000);
        __m128i vmin = _mm_set1_epi32((int)0xFFFFFFFF);
 
        for (; i + 4u <= len; i += 4u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _ssse3_min_epu32(vmin, v, bias);
        }
 
        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _ssse3_min_epu32(vmin, shifted, bias);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _ssse3_min_epu32(vmin, shifted, bias);
 
        uint32_t lane_min = (uint32_t)_mm_cvtsi128_si32(vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
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
#endif /* CSALT_SIMD_SSE3_UINT32_INL */

