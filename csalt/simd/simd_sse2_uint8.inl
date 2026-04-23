
#ifndef CSALT_SIMD_SSE2_UINT8_INL
#define CSALT_SIMD_SSE2_UINT8_INL

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <emmintrin.h>   /* SSE2 */
// ================================================================================ 
// ================================================================================ 

static inline __m128i _sse2_reverse_bytes(__m128i v) {
    /* Step 1: reverse the 8 x uint16_t words within the register.
       _MM_SHUFFLE(0,1,2,3) reverses 4 x uint32_t lanes; we then fix
       the byte order within each lane in step 2. */
    v = _mm_shufflelo_epi16(v, _MM_SHUFFLE(0, 1, 2, 3));
    v = _mm_shufflehi_epi16(v, _MM_SHUFFLE(0, 1, 2, 3));
    v = _mm_shuffle_epi32(v, _MM_SHUFFLE(1, 0, 3, 2));

    /* Step 2: byte-swap each 16-bit word.
       ((v >> 8) & 0x00FF00FF00FF00FF) | ((v << 8) & 0xFF00FF00FF00FF00) */
    __m128i lo  = _mm_srli_epi16(v, 8);
    __m128i hi  = _mm_slli_epi16(v, 8);
    return _mm_or_si128(lo, hi);
}

// ================================================================================
// Public interface
// ================================================================================

static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;

    /* SIMD fast path: only for element sizes that evenly divide 16 */
    if (data_size < 16u && (16u % data_size == 0u)) {
        size_t lo = 0u;
        size_t hi = len - 1u;

        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;

            /* How many elements fit in one 16-byte register? */
            size_t elems_per_reg = 16u / data_size;

            /* Only use SIMD when there is a full register's worth to swap
               on both sides; otherwise fall to scalar for the remainder. */
            if ((hi - lo + 1u) >= 2u * elems_per_reg) {
                __m128i vlo = _mm_loadu_si128((__m128i*)lo_ptr);
                __m128i vhi = _mm_loadu_si128((__m128i*)hi_ptr);

                vlo = _sse2_reverse_bytes(vlo);
                vhi = _sse2_reverse_bytes(vhi);

                _mm_storeu_si128((__m128i*)hi_ptr, vlo);
                _mm_storeu_si128((__m128i*)lo_ptr, vhi);

                lo += elems_per_reg;
                hi -= elems_per_reg;
                continue;
            }

            /* Scalar swap for the remaining elements */
            uint8_t tmp[16];
            memcpy(tmp,     lo_ptr, data_size);
            memcpy(lo_ptr,  hi_ptr, data_size);
            memcpy(hi_ptr,  tmp,    data_size);
            lo++;
            hi--;
        }
        return;
    }

    /* Scalar fallback for non-power-of-two or large element sizes */
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

// ================================================================================
// Internal: broadcast the first data_size bytes of needle into a 128-bit register.
// Repeats the element pattern across all 16 bytes.
// ================================================================================

static inline __m128i _sse2_broadcast(const uint8_t* needle, size_t data_size) {
    uint8_t buf[16] = {0};
    for (size_t i = 0; i < 16; i++) {
        buf[i] = needle[i % data_size];
    }
    return _mm_loadu_si128((__m128i*)buf);
}

// ================================================================================
// Internal: given a movemask result and element size, return the index of the
// first element whose data_size bytes all match, or SIZE_MAX if none.
// elem_base is the index of the first element in this 16-byte chunk.
// ================================================================================

static inline size_t _sse2_first_match(int mask,
                                        size_t data_size,
                                        size_t elem_base,
                                        size_t end) {
    /* Build the expected per-element mask: data_size consecutive set bits */
    int elem_mask = (data_size == 16) ? -1 : (1 << (int)data_size) - 1;
    size_t elems_per_reg = 16u / data_size;

    for (size_t e = 0; e < elems_per_reg; e++) {
        int shift    = (int)(e * data_size);
        int got      = (mask >> shift) & elem_mask;
        size_t idx   = elem_base + e;
        if (idx >= end) break;
        if (got == elem_mask) return idx;
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static size_t simd_contains_uint8(const uint8_t* data,
                                   size_t         start,
                                   size_t         end,
                                   size_t         data_size,
                                   const uint8_t* needle) {
    if (data_size <= 16u && (16u % data_size == 0u)) {
        size_t elems_per_reg = 16u / data_size;
        __m128i vneedle      = _sse2_broadcast(needle, data_size);

        size_t i = start;

        /* Align to element-reg boundary then process full registers */
        while (i + elems_per_reg <= end) {
            __m128i chunk = _mm_loadu_si128(
                (__m128i*)(data + i * data_size)
            );
            int mask = _mm_movemask_epi8(_mm_cmpeq_epi8(chunk, vneedle));
            if (mask != 0) {
                size_t found = _sse2_first_match(mask, data_size, i, end);
                if (found != SIZE_MAX) return found;
            }
            i += elems_per_reg;
        }

        /* Scalar remainder */
        for (; i < end; i++) {
            if (memcmp(data + i * data_size, needle, data_size) == 0) {
                return i;
            }
        }
        return SIZE_MAX;
    }

    /* Scalar fallback */
    for (size_t i = start; i < end; i++) {
        if (memcmp(data + i * data_size, needle, data_size) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

static inline uint8_t _sse2_hmin_u8(__m128i v) {
    v = _mm_min_epu8(v, _mm_srli_si128(v, 8));
    v = _mm_min_epu8(v, _mm_srli_si128(v, 4));
    v = _mm_min_epu8(v, _mm_srli_si128(v, 2));
    v = _mm_min_epu8(v, _mm_srli_si128(v, 1));
    return (uint8_t)_mm_cvtsi128_si32(v);
}

static inline uint8_t _sse2_hmax_u8(__m128i v) {
    v = _mm_max_epu8(v, _mm_srli_si128(v, 8));
    v = _mm_max_epu8(v, _mm_srli_si128(v, 4));
    v = _mm_max_epu8(v, _mm_srli_si128(v, 2));
    v = _mm_max_epu8(v, _mm_srli_si128(v, 1));
    return (uint8_t)_mm_cvtsi128_si32(v);
}

static size_t simd_min_uint8(const uint8_t* data,
                              size_t         len,
                              size_t         data_size,
                              int          (*cmp)(const void*, const void*)) {
    if (data_size == 1u) {
        __m128i vmin = _mm_set1_epi8((char)0xFF);
        size_t  i    = 0u;
        while (i + 16u <= len) {
            vmin = _mm_min_epu8(vmin, _mm_loadu_si128((__m128i*)(data + i)));
            i += 16u;
        }
        for (; i < len; i++) {
            __m128i v = _mm_set1_epi8((char)data[i]);
            vmin = _mm_min_epu8(vmin, v);
        }
        uint8_t min_val = _sse2_hmin_u8(vmin);
        for (size_t j = 0u; j < len; j++)
            if (data[j] == min_val) return j;
    }
    size_t best = 0u;
    for (size_t i = 1u; i < len; i++)
        if (cmp(data + i * data_size, data + best * data_size) < 0) best = i;
    return best;
}

static size_t simd_max_uint8(const uint8_t* data,
                              size_t         len,
                              size_t         data_size,
                              int          (*cmp)(const void*, const void*)) {
    if (data_size == 1u) {
        __m128i vmax = _mm_setzero_si128();
        size_t  i    = 0u;
        while (i + 16u <= len) {
            vmax = _mm_max_epu8(vmax, _mm_loadu_si128((__m128i*)(data + i)));
            i += 16u;
        }
        for (; i < len; i++) {
            __m128i v = _mm_set1_epi8((char)data[i]);
            vmax = _mm_max_epu8(vmax, v);
        }
        uint8_t max_val = _sse2_hmax_u8(vmax);
        for (size_t j = 0u; j < len; j++)
            if (data[j] == max_val) return j;
    }
    size_t best = 0u;
    for (size_t i = 1u; i < len; i++)
        if (cmp(data + i * data_size, data + best * data_size) > 0) best = i;
    return best;
}
// ================================================================================ 
// ================================================================================ 

static inline void simd_fill_uint8(uint8_t* data, size_t count, uint8_t value) {
    /* memset is typically SIMD-optimized by libc and handles alignment */
    memset(data, value, count);
}
// -------------------------------------------------------------------------------- 
 
static inline void simd_transpose_uint8(const uint8_t* src,
                                        uint8_t*       dst,
                                        size_t         src_rows,
                                        size_t         src_cols) {
    /*
     * Byte-level in-register transpose is possible with pshufb (SSSE3)
     * but not with SSE2.  Use cache-friendly tiling with scalar kernel.
     */
    const size_t TILE = 16u;
    size_t i = 0u;
    size_t j = 0u;
 
    size_t row_body = (src_rows / TILE) * TILE;
    size_t col_body = (src_cols / TILE) * TILE;
 
    for (i = 0u; i < row_body; i += TILE) {
        for (j = 0u; j < col_body; j += TILE) {
            for (size_t ii = i; ii < i + TILE; ++ii) {
                for (size_t jj = j; jj < j + TILE; ++jj) {
                    dst[jj * src_rows + ii] = src[ii * src_cols + jj];
                }
            }
        }
    }
 
    for (i = 0u; i < row_body; i += TILE) {
        for (j = col_body; j < src_cols; ++j) {
            for (size_t ii = i; ii < i + TILE; ++ii) {
                dst[j * src_rows + ii] = src[ii * src_cols + j];
            }
        }
    }
 
    for (i = row_body; i < src_rows; ++i) {
        for (j = 0u; j < src_cols; ++j) {
            dst[j * src_rows + i] = src[i * src_cols + j];
        }
    }
}
// -------------------------------------------------------------------------------- 
 
static inline bool simd_uint8_arrays_equal(const uint8_t* a,
                                           const uint8_t* b,
                                           size_t         count) {
    if (a == NULL || b == NULL) return false;

    size_t i = 0u;
    size_t vec_end = (count / 16u) * 16u;

    for (; i < vec_end; i += 16u) {
        __m128i av = _mm_loadu_si128((const __m128i*)(a + i));
        __m128i bv = _mm_loadu_si128((const __m128i*)(b + i));

        __m128i cmp = _mm_cmpeq_epi8(av, bv);

        if (_mm_movemask_epi8(cmp) != 0xFFFF) {
            return false;
        }
    }

    for (; i < count; ++i) {
        if (a[i] != b[i]) return false;
    }

    return true;
}
// -------------------------------------------------------------------------------- 
 
static inline size_t simd_count_nonzero_uint8(const uint8_t* data,
                                              size_t         count) {
    size_t nnz = 0u;
    size_t i = 0u;
 
    __m128i zero = _mm_setzero_si128();
 
    /*
     * Compare 16 bytes against zero.  _mm_cmpeq_epi8 produces 0xFF for
     * zero lanes and 0x00 for nonzero lanes.  Invert the movemask to
     * count nonzero lanes.  movemask returns one bit per byte (16 bits).
     */
    size_t body_end = (count / 64u) * 64u;
    for (; i < body_end; i += 64u) {
        int m0 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i)), zero)) & 0xFFFF;
        int m1 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i + 16u)), zero)) & 0xFFFF;
        int m2 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i + 32u)), zero)) & 0xFFFF;
        int m3 = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i + 48u)), zero)) & 0xFFFF;
 
        nnz += (size_t)(__builtin_popcount((unsigned)m0) +
                        __builtin_popcount((unsigned)m1) +
                        __builtin_popcount((unsigned)m2) +
                        __builtin_popcount((unsigned)m3));
    }
 
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        int m = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
            _mm_loadu_si128((const __m128i*)(data + i)), zero)) & 0xFFFF;
        nnz += (size_t)__builtin_popcount((unsigned)m);
    }
 
    for (; i < count; ++i) {
        if (data[i] != 0u) ++nnz;
    }
 
    return nnz;
}
// -------------------------------------------------------------------------------- 
 
static inline size_t simd_scatter_csr_row_uint8(const uint8_t* row_data,
                                                size_t         cols,
                                                size_t         col_offset,
                                                size_t*        col_idx,
                                                uint8_t*       values,
                                                size_t         k) {
    for (size_t j = 0u; j < cols; ++j) {
        if (row_data[j] != 0u) {
            col_idx[k] = col_offset + j;
            values[k]  = row_data[j];
            ++k;
        }
    }
    return k;
}
// -------------------------------------------------------------------------------- 
 
static inline bool simd_is_all_zero_uint8(const uint8_t* data, size_t count) {
    size_t i = 0u;
 
    size_t body_end = (count / 64u) * 64u;
    for (; i < body_end; i += 64u) {
        __m128i v0 = _mm_loadu_si128((const __m128i*)(data + i));
        __m128i v1 = _mm_loadu_si128((const __m128i*)(data + i + 16u));
        __m128i v2 = _mm_loadu_si128((const __m128i*)(data + i + 32u));
        __m128i v3 = _mm_loadu_si128((const __m128i*)(data + i + 48u));
 
        __m128i any = _mm_or_si128(_mm_or_si128(v0, v1),
                                   _mm_or_si128(v2, v3));
 
        if (_mm_movemask_epi8(
                _mm_cmpeq_epi8(any, _mm_setzero_si128())) != 0xFFFF)
            return false;
    }
 
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
 
        if (_mm_movemask_epi8(
                _mm_cmpeq_epi8(v, _mm_setzero_si128())) != 0xFFFF)
            return false;
    }
 
    for (; i < count; ++i) {
        if (data[i] != 0u) return false;
    }
 
    return true;
}
// -------------------------------------------------------------------------------- 

static inline void simd_add_scalar_uint8(uint8_t*      data,
                                         size_t        len,
                                         size_t        data_size,
                                         const void*   scalar,
                                         void        (*add_scalar)(void* element,
                                                                   const void* scalar)) {
    size_t i = 0u;

    switch (data_size) {
        case 1u: {
            const __m128i s = _mm_set1_epi8((char)(*(const uint8_t*)scalar));
            size_t vec_end = (len / 16u) * 16u;
            for (; i < vec_end; i += 16u) {
                __m128i v = _mm_loadu_si128((const __m128i*)(data + i));
                v = _mm_add_epi8(v, s);
                _mm_storeu_si128((__m128i*)(data + i), v);
            }
            for (; i < len; ++i) {
                ((uint8_t*)data)[i] += *(const uint8_t*)scalar;
            }
            return;
        }

        case 2u: {
            uint16_t* d = (uint16_t*)data;
            const __m128i s = _mm_set1_epi16((short)(*(const uint16_t*)scalar));
            size_t vec_end = (len / 8u) * 8u;
            for (; i < vec_end; i += 8u) {
                __m128i v = _mm_loadu_si128((const __m128i*)(d + i));
                v = _mm_add_epi16(v, s);
                _mm_storeu_si128((__m128i*)(d + i), v);
            }
            for (; i < len; ++i) {
                d[i] += *(const uint16_t*)scalar;
            }
            return;
        }

        case 4u: {
            uint32_t* d = (uint32_t*)data;
            const __m128i s = _mm_set1_epi32((int)(*(const uint32_t*)scalar));
            size_t vec_end = (len / 4u) * 4u;
            for (; i < vec_end; i += 4u) {
                __m128i v = _mm_loadu_si128((const __m128i*)(d + i));
                v = _mm_add_epi32(v, s);
                _mm_storeu_si128((__m128i*)(d + i), v);
            }
            for (; i < len; ++i) {
                d[i] += *(const uint32_t*)scalar;
            }
            return;
        }

        case 8u: {
            uint64_t* d = (uint64_t*)data;
            const __m128i s = _mm_set1_epi64x((long long)(*(const uint64_t*)scalar));
            size_t vec_end = (len / 2u) * 2u;
            for (; i < vec_end; i += 2u) {
                __m128i v = _mm_loadu_si128((const __m128i*)(d + i));
                v = _mm_add_epi64(v, s);
                _mm_storeu_si128((__m128i*)(d + i), v);
            }
            for (; i < len; ++i) {
                d[i] += *(const uint64_t*)scalar;
            }
            return;
        }

        default:
            for (i = 0u; i < len; ++i) {
                add_scalar(data + i * data_size, scalar);
            }
            return;
    }
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SSE2_UINT8_INL */

