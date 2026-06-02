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
#ifndef CSALT_SIMD_SSE3_UINT64_INL
#define CSALT_SIMD_SSE3_UINT64_INL

#include "c_error.h"

#include <tmmintrin.h>   /* SSSE3 (includes SSE2, SSE3) */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Static helpers
// ================================================================================
 
static inline __m128i _ssse3_cmpeq_epi64(__m128i a, __m128i b) {
    __m128i cmp32 = _mm_cmpeq_epi32(a, b);
    return _mm_and_si128(cmp32, _mm_shuffle_epi32(cmp32, 0xB1));
}
 
// ================================================================================
// Public interface
// ================================================================================
 
static size_t simd_lsearch_uint64(const uint64_t* data,
                                  size_t          len,
                                  uint64_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 2u;
 
#if defined(__SSSE3__)
    if (len >= elems_per_reg) {
        __m128i target = _mm_set1_epi64x((int64_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _ssse3_cmpeq_epi64(chunk, target);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 8u);
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
#endif /* CSALT_SIMD_SSE3_UINT64_INL */

