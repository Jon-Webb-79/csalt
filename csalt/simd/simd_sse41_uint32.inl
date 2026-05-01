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
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SSE41_UINT32_INL */

