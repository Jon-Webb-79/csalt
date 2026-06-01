// ================================================================================
// ================================================================================
// - File:    simd_sse41_lsearch_uint16.inl
// - Purpose: SSE4.1 implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements.
//
//            SSE4.1 adds _mm_test_all_zeros which can short-circuit the
//            mask check more efficiently on some microarchitectures, and
//            _mm_blendv_epi8 for branchless selection.  For a simple
//            linear search _mm_cmpeq_epi16 + _mm_movemask_epi8 from
//            SSE2 remains the primary mechanism.  The file is provided
//            as a distinct compilation unit for build system dispatch
//            when SSE4.1 is the highest available feature level.
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
#ifndef CSALT_SIMD_SSE41_UINT32_INL
#define CSALT_SIMD_SSE41_UINT32_INL

#include "c_error.h"

#include <smmintrin.h>   /* SSE4.1 (includes SSSE3, SSE3, SSE2) */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint32(const uint32_t* data,
                                  size_t          len,
                                  uint32_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 4u;
 
#if defined(__SSE4_1__)
    if (len >= elems_per_reg) {
        __m128i target = _mm_set1_epi32((int32_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _mm_cmpeq_epi32(chunk, target);
 
            /* _mm_test_all_zeros avoids movemask when no lane matched */
            if (!_mm_test_all_zeros(cmp, cmp)) {
                int mask = _mm_movemask_epi8(cmp);
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

static inline error_code_t simd_min_uint32(const uint32_t* data,
                                           size_t          len,
                                           uint32_t*       out) {
    uint32_t cur_min = *out;
    size_t   i       = 0u;
 
    if (len >= 4u) {
        __m128i vmin = _mm_set1_epi32((int)0xFFFFFFFF);
 
        for (; i + 4u <= len; i += 4u) {
            __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
            vmin = _mm_min_epu32(vmin, v);
        }
 
        /* Horizontal reduction: 4 → 2 → 1 */
        __m128i shifted = _mm_srli_si128(vmin, 8);
        vmin = _mm_min_epu32(vmin, shifted);
        shifted = _mm_srli_si128(vmin, 4);
        vmin = _mm_min_epu32(vmin, shifted);
 
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
#endif /* CSALT_SIMD_SSE41_UINT32_INL */

