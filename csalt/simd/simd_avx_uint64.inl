// ================================================================================
// ================================================================================
// - File:    simd_avx_lsearch_uint16.inl
// - Purpose: AVX (non-AVX2) implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements.
//
//            Plain AVX provides 256-bit floating-point registers but
//            256-bit integer operations are not available until AVX2.
//            This implementation therefore uses two 128-bit SSE2 passes
//            per 256-byte-wide loop iteration, processing 16 uint16_t
//            elements per iteration (2 x 8) via the lower 128-bit lanes
//            of the AVX registers.  This avoids the AVX-SSE transition
//            penalty while still amortising the loop overhead over more
//            elements than the pure SSE2 version.
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
#ifndef CSALT_SIMD_AVX_UINT64_INL
#define CSALT_SIMD_AVX_UINT64_INL

#include "c_error.h"

#include <immintrin.h>   /* AVX */
#include <smmintrin.h>   /* SSE4.1 _mm_cmpeq_epi64 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint64(const uint64_t* data,
                                  size_t          len,
                                  uint64_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_iter = 4u;
    const size_t elems_per_reg  = 2u;
 
#if defined(__AVX__)
    if (len >= elems_per_iter) {
        __m128i target = _mm_set1_epi64x((int64_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_iter <= len; i += elems_per_iter) {
            __m128i lo = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i hi = _mm_loadu_si128((const __m128i*)(data + i + elems_per_reg));
 
            __m128i cmp_lo = _mm_cmpeq_epi64(lo, target);
            __m128i cmp_hi = _mm_cmpeq_epi64(hi, target);
 
            int mask_lo = _mm_movemask_epi8(cmp_lo);
            int mask_hi = _mm_movemask_epi8(cmp_hi);
 
            if (mask_lo != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask_lo) / 8u);
            }
            if (mask_hi != 0) {
                return i + elems_per_reg +
                       (size_t)(__builtin_ctz((unsigned)mask_hi) / 8u);
            }
        }
 
        /* Handle remaining full 2-element chunks */
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _mm_cmpeq_epi64(chunk, target);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 8u);
            }
        }
 
        /* Scalar remainder */
        for (; i < len; i++) {
            if (data[i] == value) return i;
        }
 
        _mm256_zeroupper();
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
#endif /* CSALT_SIMD_AVX_UINT64_INL */

