// ================================================================================
// ================================================================================
// - File:    simd_avx512_lsearch_uint16.inl
// - Purpose: AVX-512 implementation of simd_lsearch_uint16.
//            Searches for the first occurrence of value in an array of
//            uint16_t elements using AVX-512BW 512-bit integer comparisons.
//
//            AVX-512BW provides _mm512_cmpeq_epi16_mask which compares
//            32 uint16_t lanes simultaneously and writes the result
//            directly into a 32-bit k-register mask, eliminating the
//            movemask step required by AVX2.  _tzcnt_u32 on the mask
//            gives the element index of the first match directly without
//            the divide-by-2 needed for byte masks.
//
//            Requires: AVX-512F + AVX-512BW
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
#ifndef CSALT_SIMD_AVX512_UINT64_INL
#define CSALT_SIMD_AVX512_UINT64_INL

#include "c_error.h"

#include <immintrin.h>   /* AVX-512 */
#include <stdint.h>
#include <stddef.h>
// ================================================================================ 
// ================================================================================ 

static size_t simd_lsearch_uint64(const uint64_t* data,
                                  size_t          len,
                                  uint64_t        value) {
    if (data == NULL || len == 0u) return SIZE_MAX;
 
    const size_t elems_per_reg = 8u;   /* 512 bits / 64 bits per uint64_t */
 
#if defined(__AVX512F__)
    if (len >= elems_per_reg) {
        __m512i target = _mm512_set1_epi64((int64_t)value);
 
        size_t i = 0u;
        for (; i + elems_per_reg <= len; i += elems_per_reg) {
            __m512i  chunk = _mm512_loadu_si512((const __m512i*)(data + i));
            /* 8-bit k-register: bit N set if lane N matched.
             * No divide needed — bit directly maps to element index. */
            __mmask8 mask  = _mm512_cmpeq_epi64_mask(chunk, target);
            if (mask != 0u) {
                return i + (size_t)_tzcnt_u32((uint32_t)mask);
            }
        }
 
        /* Handle remaining full AVX2 chunks (4 elements) */
        if (i + 4u <= len) {
            __m256i target256 = _mm256_set1_epi64x((int64_t)value);
            for (; i + 4u <= len; i += 4u) {
                __m256i chunk = _mm256_loadu_si256((const __m256i*)(data + i));
                __m256i cmp   = _mm256_cmpeq_epi64(chunk, target256);
                int     mask  = _mm256_movemask_epi8(cmp);
                if (mask != 0) {
                    return i + (size_t)(__builtin_ctz((unsigned)mask) / 8u);
                }
            }
        }
 
        /* Handle remaining full SSE4.1 chunks (2 elements) */
        __m128i target128 = _mm_set1_epi64x((int64_t)value);
        for (; i + 2u <= len; i += 2u) {
            __m128i chunk = _mm_loadu_si128((const __m128i*)(data + i));
            __m128i cmp   = _mm_cmpeq_epi64(chunk, target128);
            int     mask  = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                return i + (size_t)(__builtin_ctz((unsigned)mask) / 8u);
            }
        }
 
        /* Scalar remainder */
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
    size_t   i       = 0u;
 
    if (len >= 8u) {
        __m512i vmin = _mm512_set1_epi64((long long)0xFFFFFFFFFFFFFFFFLL);
 
        for (; i + 8u <= len; i += 8u) {
            __m512i v = _mm512_loadu_si512((const void*)(data + i));
            vmin = _mm512_min_epu64(vmin, v);
        }
 
        /* AVX-512 horizontal reduction via _mm512_reduce_min_epu64
         * is a pseudo-intrinsic that the compiler expands into a
         * fold sequence.  Use it when available for clarity. */
        uint64_t lane_min = _mm512_reduce_min_epu64(vmin);
        if (lane_min < cur_min) cur_min = lane_min;
        if (cur_min == 0u) { *out = 0u; return NO_ERROR; }
    }
 
    /* Handle 4-element AVX2 chunk */
    if (i + 4u <= len) {
        __m256i bias = _mm256_set1_epi64x((long long)0x8000000000000000LL);
        __m256i vmin = _mm256_set1_epi64x((long long)cur_min);
        /* XOR bias for unsigned compare via signed cmpgt */
        __m256i vmin_b = _mm256_xor_si256(vmin, bias);
 
        __m256i v   = _mm256_loadu_si256((const __m256i*)(data + i));
        __m256i v_b = _mm256_xor_si256(v, bias);
        __m256i vmin_gt = _mm256_cmpgt_epi64(vmin_b, v_b);
        vmin = _mm256_blendv_epi8(vmin, v, vmin_gt);
        i += 4u;
 
        /* Scalar extract from 4 lanes */
        uint64_t tmp[4];
        _mm256_storeu_si256((__m256i*)tmp, vmin);
        for (int k = 0; k < 4; k++) {
            if (tmp[k] < cur_min) cur_min = tmp[k];
        }
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
#endif /* CSALT_SIMD_AVX512_UINT64_INL */

