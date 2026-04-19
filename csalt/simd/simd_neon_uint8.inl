
#ifndef CSALT_SIMD_NEON_UINT8_INL
#define CSALT_SIMD_NEON_UINT8_INL

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <arm_sve.h>
// ================================================================================ 
// ================================================================================ 

static inline uint8x16_t _neon_reverse_bytes(uint8x16_t v) {
    /* vrev64q_u8: reverse bytes within each 64-bit half */
    v = vrev64q_u8(v);
    /* Swap the two 64-bit halves to complete the full 128-bit reversal */
    return vcombine_u8(vget_high_u8(v), vget_low_u8(v));
}

// ================================================================================
// Public interface
// ================================================================================

static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;

    if (data_size < 16u && (16u % data_size == 0u)) {
        size_t lo = 0u;
        size_t hi = len - 1u;
        size_t elems_per_reg = 16u / data_size;

        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;

            if ((hi - lo + 1u) >= 2u * elems_per_reg) {
                uint8x16_t vlo = vld1q_u8(lo_ptr);
                uint8x16_t vhi = vld1q_u8(hi_ptr);

                vlo = _neon_reverse_bytes(vlo);
                vhi = _neon_reverse_bytes(vhi);

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

    /* Scalar fallback */
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

static size_t simd_contains_uint8(const uint8_t* data,
                                   size_t         start,
                                   size_t         end,
                                   size_t         data_size,
                                   const uint8_t* needle) {
    size_t i = start;

    if (data_size == 1u) {
        uint8x16_t vn = vdupq_n_u8(*needle);
        while (i + 16u <= end) {
            uint8x16_t chunk = vld1q_u8(data + i);
            uint8x16_t cmp   = vceqq_u8(chunk, vn);
            /* vmaxvq_u8: horizontal max — non-zero means at least one match */
            if (vmaxvq_u8(cmp) != 0) {
                /* Find the exact lane */
                uint8_t tmp[16];
                vst1q_u8(tmp, cmp);
                for (size_t e = 0; e < 16u && i + e < end; e++) {
                    if (tmp[e] != 0) return i + e;
                }
            }
            i += 16u;
        }
    } else if (data_size == 2u) {
        uint16x8_t vn = vdupq_n_u16(*(const uint16_t*)needle);
        while (i + 8u <= end) {
            uint16x8_t chunk = vld1q_u16((const uint16_t*)(data + i * 2u));
            uint16x8_t cmp   = vceqq_u16(chunk, vn);
            if (vmaxvq_u16(cmp) != 0) {
                uint16_t tmp[8];
                vst1q_u16(tmp, cmp);
                for (size_t e = 0; e < 8u && i + e < end; e++) {
                    if (tmp[e] != 0) return i + e;
                }
            }
            i += 8u;
        }
    } else if (data_size == 4u) {
        uint32x4_t vn = vdupq_n_u32(*(const uint32_t*)needle);
        while (i + 4u <= end) {
            uint32x4_t chunk = vld1q_u32((const uint32_t*)(data + i * 4u));
            uint32x4_t cmp   = vceqq_u32(chunk, vn);
            if (vmaxvq_u32(cmp) != 0) {
                uint32_t tmp[4];
                vst1q_u32(tmp, cmp);
                for (size_t e = 0; e < 4u && i + e < end; e++) {
                    if (tmp[e] != 0) return i + e;
                }
            }
            i += 4u;
        }
    } else if (data_size == 8u) {
        uint64x2_t vn = vdupq_n_u64(*(const uint64_t*)needle);
        while (i + 2u <= end) {
            uint64x2_t chunk = vld1q_u64((const uint64_t*)(data + i * 8u));
            uint64x2_t cmp   = vceqq_u64(chunk, vn);
            if (vmaxvq_u64(cmp) != 0) {
                if (vgetq_lane_u64(cmp, 0) != 0 && i     < end) return i;
                if (vgetq_lane_u64(cmp, 1) != 0 && i + 1 < end) return i + 1u;
            }
            i += 2u;
        }
    }

    /* Scalar remainder for all sizes */
    for (; i < end; i++) {
        if (memcmp(data + i * data_size, needle, data_size) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}
// -------------------------------------------------------------------------------- 

#ifdef __aarch64__
#  define _neon_hmin_u8(v)  vminvq_u8(v)
#  define _neon_hmax_u8(v)  vmaxvq_u8(v)
#  define _neon_hmin_u16(v) vminvq_u16(v)
#  define _neon_hmax_u16(v) vmaxvq_u16(v)
#  define _neon_hmin_u32(v) vminvq_u32(v)
#  define _neon_hmax_u32(v) vmaxvq_u32(v)
#else
static inline uint8_t _neon_hmin_u8(uint8x16_t v) {
    uint8x8_t lo = vmin_u8(vget_low_u8(v), vget_high_u8(v));
    lo = vpmin_u8(lo, lo); lo = vpmin_u8(lo, lo); lo = vpmin_u8(lo, lo);
    return vget_lane_u8(lo, 0);
}
static inline uint8_t _neon_hmax_u8(uint8x16_t v) {
    uint8x8_t lo = vmax_u8(vget_low_u8(v), vget_high_u8(v));
    lo = vpmax_u8(lo, lo); lo = vpmax_u8(lo, lo); lo = vpmax_u8(lo, lo);
    return vget_lane_u8(lo, 0);
}
static inline uint16_t _neon_hmin_u16(uint16x8_t v) {
    uint16x4_t lo = vmin_u16(vget_low_u16(v), vget_high_u16(v));
    lo = vpmin_u16(lo, lo); lo = vpmin_u16(lo, lo);
    return vget_lane_u16(lo, 0);
}
static inline uint16_t _neon_hmax_u16(uint16x8_t v) {
    uint16x4_t lo = vmax_u16(vget_low_u16(v), vget_high_u16(v));
    lo = vpmax_u16(lo, lo); lo = vpmax_u16(lo, lo);
    return vget_lane_u16(lo, 0);
}
static inline uint32_t _neon_hmin_u32(uint32x4_t v) {
    uint32x2_t lo = vmin_u32(vget_low_u32(v), vget_high_u32(v));
    lo = vpmin_u32(lo, lo);
    return vget_lane_u32(lo, 0);
}
static inline uint32_t _neon_hmax_u32(uint32x4_t v) {
    uint32x2_t lo = vmax_u32(vget_low_u32(v), vget_high_u32(v));
    lo = vpmax_u32(lo, lo);
    return vget_lane_u32(lo, 0);
}
#endif

static size_t simd_min_uint8(const uint8_t* data,
                              size_t         len,
                              size_t         data_size,
                              int          (*cmp)(const void*, const void*)) {
    if (data_size == 1u) {
        uint8x16_t vmin = vdupq_n_u8(0xFF);
        size_t i = 0u;
        while (i + 16u <= len) { vmin = vminq_u8(vmin, vld1q_u8(data+i)); i+=16u; }
        for (; i < len; i++) vmin = vminq_u8(vmin, vdupq_n_u8(data[i]));
        uint8_t min_val = _neon_hmin_u8(vmin);
        for (size_t j = 0u; j < len; j++) if (data[j] == min_val) return j;
    } else if (data_size == 2u) {
        const uint16_t* p = (const uint16_t*)data;
        uint16x8_t vmin = vdupq_n_u16(0xFFFF);
        size_t i = 0u;
        while (i + 8u <= len) { vmin = vminq_u16(vmin, vld1q_u16(p+i)); i+=8u; }
        for (; i < len; i++) vmin = vminq_u16(vmin, vdupq_n_u16(p[i]));
        uint16_t mv = _neon_hmin_u16(vmin);
        for (size_t j = 0u; j < len; j++) if (p[j] == mv) {
            size_t best = j;
            for (size_t k = j+1u; k < len; k++)
                if (cmp(data+k*2u, data+best*2u) < 0) best = k;
            return best;
        }
    } else if (data_size == 4u) {
        const uint32_t* p = (const uint32_t*)data;
        uint32x4_t vmin = vdupq_n_u32(0xFFFFFFFFu);
        size_t i = 0u;
        while (i + 4u <= len) { vmin = vminq_u32(vmin, vld1q_u32(p+i)); i+=4u; }
        for (; i < len; i++) vmin = vminq_u32(vmin, vdupq_n_u32(p[i]));
        uint32_t mv = _neon_hmin_u32(vmin);
        for (size_t j = 0u; j < len; j++) if (p[j] == mv) {
            size_t best = j;
            for (size_t k = j+1u; k < len; k++)
                if (cmp(data+k*4u, data+best*4u) < 0) best = k;
            return best;
        }
    }
    size_t best = 0u;
    for (size_t i = 1u; i < len; i++)
        if (cmp(data+i*data_size, data+best*data_size) < 0) best = i;
    return best;
}

static size_t simd_max_uint8(const uint8_t* data,
                              size_t         len,
                              size_t         data_size,
                              int          (*cmp)(const void*, const void*)) {
    if (data_size == 1u) {
        uint8x16_t vmax = vdupq_n_u8(0x00);
        size_t i = 0u;
        while (i + 16u <= len) { vmax = vmaxq_u8(vmax, vld1q_u8(data+i)); i+=16u; }
        for (; i < len; i++) vmax = vmaxq_u8(vmax, vdupq_n_u8(data[i]));
        uint8_t max_val = _neon_hmax_u8(vmax);
        for (size_t j = 0u; j < len; j++) if (data[j] == max_val) return j;
    } else if (data_size == 2u) {
        const uint16_t* p = (const uint16_t*)data;
        uint16x8_t vmax = vdupq_n_u16(0x0000);
        size_t i = 0u;
        while (i + 8u <= len) { vmax = vmaxq_u16(vmax, vld1q_u16(p+i)); i+=8u; }
        for (; i < len; i++) vmax = vmaxq_u16(vmax, vdupq_n_u16(p[i]));
        uint16_t mv = _neon_hmax_u16(vmax);
        for (size_t j = 0u; j < len; j++) if (p[j] == mv) {
            size_t best = j;
            for (size_t k = j+1u; k < len; k++)
                if (cmp(data+k*2u, data+best*2u) > 0) best = k;
            return best;
        }
    } else if (data_size == 4u) {
        const uint32_t* p = (const uint32_t*)data;
        uint32x4_t vmax = vdupq_n_u32(0x00000000u);
        size_t i = 0u;
        while (i + 4u <= len) { vmax = vmaxq_u32(vmax, vld1q_u32(p+i)); i+=4u; }
        for (; i < len; i++) vmax = vmaxq_u32(vmax, vdupq_n_u32(p[i]));
        uint32_t mv = _neon_hmax_u32(vmax);
        for (size_t j = 0u; j < len; j++) if (p[j] == mv) {
            size_t best = j;
            for (size_t k = j+1u; k < len; k++)
                if (cmp(data+k*4u, data+best*4u) > 0) best = k;
            return best;
        }
    }
    size_t best = 0u;
    for (size_t i = 1u; i < len; i++)
        if (cmp(data+i*data_size, data+best*data_size) > 0) best = i;
    return best;
}
// -------------------------------------------------------------------------------- 

static void simd_sum_uint8(const uint8_t* data,
                            size_t         len,
                            size_t         data_size,
                            void*          accum,
                            void         (*add)(void* accum, const void* element)) {
    size_t i = 0u;
    if (data_size == 1u) {
        while (i + 16u <= len) {
            uint8x16_t c = vld1q_u8(data + i);
            uint8_t v;
            v=vgetq_lane_u8(c, 0);add(accum,&v); v=vgetq_lane_u8(c, 1);add(accum,&v);
            v=vgetq_lane_u8(c, 2);add(accum,&v); v=vgetq_lane_u8(c, 3);add(accum,&v);
            v=vgetq_lane_u8(c, 4);add(accum,&v); v=vgetq_lane_u8(c, 5);add(accum,&v);
            v=vgetq_lane_u8(c, 6);add(accum,&v); v=vgetq_lane_u8(c, 7);add(accum,&v);
            v=vgetq_lane_u8(c, 8);add(accum,&v); v=vgetq_lane_u8(c, 9);add(accum,&v);
            v=vgetq_lane_u8(c,10);add(accum,&v); v=vgetq_lane_u8(c,11);add(accum,&v);
            v=vgetq_lane_u8(c,12);add(accum,&v); v=vgetq_lane_u8(c,13);add(accum,&v);
            v=vgetq_lane_u8(c,14);add(accum,&v); v=vgetq_lane_u8(c,15);add(accum,&v);
            i += 16u;
        }
    } else if (data_size == 2u) {
        while (i + 8u <= len) {
            uint16x8_t c = vld1q_u16((const uint16_t*)(data + i * 2u));
            uint16_t v;
            v=vgetq_lane_u16(c,0);add(accum,&v); v=vgetq_lane_u16(c,1);add(accum,&v);
            v=vgetq_lane_u16(c,2);add(accum,&v); v=vgetq_lane_u16(c,3);add(accum,&v);
            v=vgetq_lane_u16(c,4);add(accum,&v); v=vgetq_lane_u16(c,5);add(accum,&v);
            v=vgetq_lane_u16(c,6);add(accum,&v); v=vgetq_lane_u16(c,7);add(accum,&v);
            i += 8u;
        }
    } else if (data_size == 4u) {
        while (i + 4u <= len) {
            uint32x4_t c = vld1q_u32((const uint32_t*)(data + i * 4u));
            uint32_t v;
            v=vgetq_lane_u32(c,0);add(accum,&v); v=vgetq_lane_u32(c,1);add(accum,&v);
            v=vgetq_lane_u32(c,2);add(accum,&v); v=vgetq_lane_u32(c,3);add(accum,&v);
            i += 4u;
        }
    } else if (data_size == 8u) {
        while (i + 2u <= len) {
            uint64x2_t c = vld1q_u64((const uint64_t*)(data + i * 8u));
            uint64_t v;
            v=vgetq_lane_u64(c,0);add(accum,&v);
            v=vgetq_lane_u64(c,1);add(accum,&v);
            i += 2u;
        }
    }
    for (; i < len; i++) add(accum, data + i * data_size);
}
// ================================================================================ 
// ================================================================================ 

static inline void simd_fill_uint8(uint8_t* data, size_t count, uint8_t value) {
    memset(data, value, count);
}
// -------------------------------------------------------------------------------- 
 
static inline void simd_transpose_uint8(const uint8_t* src,
                                        uint8_t*       dst,
                                        size_t         src_rows,
                                        size_t         src_cols) {
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
        uint8x16_t av = vld1q_u8(a + i);
        uint8x16_t bv = vld1q_u8(b + i);

        uint8x16_t cmp = vceqq_u8(av, bv);

#if defined(__aarch64__)
        if (vminvq_u8(cmp) != 0xFF) {
            return false;
        }
#else
        uint8_t lanes[16];
        vst1q_u8(lanes, cmp);
        for (int j = 0; j < 16; ++j) {
            if (lanes[j] != 0xFF) return false;
        }
#endif
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
 
    uint8x16_t zero = vdupq_n_u8(0u);
 
    /*
     * For each lane: ceq gives 0xFF if zero, 0x00 if nonzero.
     * Invert (mvn): 0xFF if nonzero, 0x00 if zero.
     * Right-shift by 7: 0x01 if nonzero, 0x00 if zero.
     * vaddvq_u8 sums all 16 lanes → count of nonzero bytes.
     */
    size_t body_end = (count / 64u) * 64u;
    for (; i < body_end; i += 64u) {
        uint8x16_t v0 = vld1q_u8(data + i);
        uint8x16_t v1 = vld1q_u8(data + i + 16u);
        uint8x16_t v2 = vld1q_u8(data + i + 32u);
        uint8x16_t v3 = vld1q_u8(data + i + 48u);
 
        /* Fast all-zero skip */
        uint8x16_t any = vorrq_u8(vorrq_u8(v0, v1), vorrq_u8(v2, v3));
        if (vmaxvq_u8(any) == 0u) continue;
 
        uint8x16_t nz0 = vshrq_n_u8(vmvnq_u8(vceqq_u8(v0, zero)), 7);
        uint8x16_t nz1 = vshrq_n_u8(vmvnq_u8(vceqq_u8(v1, zero)), 7);
        uint8x16_t nz2 = vshrq_n_u8(vmvnq_u8(vceqq_u8(v2, zero)), 7);
        uint8x16_t nz3 = vshrq_n_u8(vmvnq_u8(vceqq_u8(v3, zero)), 7);
 
        /* Sum pairs to avoid u8 overflow (max 16 per vector, 64 total) */
        uint16x8_t s01 = vaddl_u8(vget_low_u8(nz0), vget_low_u8(nz1));
        s01 = vaddw_u8(s01, vget_high_u8(nz0));
        s01 = vaddw_u8(s01, vget_high_u8(nz1));
 
        uint16x8_t s23 = vaddl_u8(vget_low_u8(nz2), vget_low_u8(nz3));
        s23 = vaddw_u8(s23, vget_high_u8(nz2));
        s23 = vaddw_u8(s23, vget_high_u8(nz3));
 
        uint16x8_t sum = vaddq_u16(s01, s23);
        nnz += (size_t)vaddvq_u16(sum);
    }
 
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        uint8x16_t nz = vshrq_n_u8(vmvnq_u8(vceqq_u8(vld1q_u8(data + i), zero)), 7);
        nnz += (size_t)vaddvq_u8(nz);
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
        uint8x16_t any = vorrq_u8(
            vorrq_u8(vld1q_u8(data + i),      vld1q_u8(data + i + 16u)),
            vorrq_u8(vld1q_u8(data + i + 32u), vld1q_u8(data + i + 48u)));
 
        if (vmaxvq_u8(any) != 0u) return false;
    }
 
    size_t vec_end = i + ((count - i) / 16u) * 16u;
    for (; i < vec_end; i += 16u) {
        if (vmaxvq_u8(vld1q_u8(data + i)) != 0u) return false;
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
            uint8_t* d = (uint8_t*)data;
            const uint8x16_t s = vdupq_n_u8(*(const uint8_t*)scalar);
            size_t vec_end = (len / 16u) * 16u;
            for (; i < vec_end; i += 16u) {
                uint8x16_t v = vld1q_u8(d + i);
                v = vaddq_u8(v, s);
                vst1q_u8(d + i, v);
            }
            for (; i < len; ++i) d[i] += *(const uint8_t*)scalar;
            return;
        }

        case 2u: {
            uint16_t* d = (uint16_t*)data;
            const uint16x8_t s = vdupq_n_u16(*(const uint16_t*)scalar);
            size_t vec_end = (len / 8u) * 8u;
            for (; i < vec_end; i += 8u) {
                uint16x8_t v = vld1q_u16(d + i);
                v = vaddq_u16(v, s);
                vst1q_u16(d + i, v);
            }
            for (; i < len; ++i) d[i] += *(const uint16_t*)scalar;
            return;
        }

        case 4u: {
            uint32_t* d = (uint32_t*)data;
            const uint32x4_t s = vdupq_n_u32(*(const uint32_t*)scalar);
            size_t vec_end = (len / 4u) * 4u;
            for (; i < vec_end; i += 4u) {
                uint32x4_t v = vld1q_u32(d + i);
                v = vaddq_u32(v, s);
                vst1q_u32(d + i, v);
            }
            for (; i < len; ++i) d[i] += *(const uint32_t*)scalar;
            return;
        }

        case 8u: {
#if defined(__aarch64__)
            uint64_t* d = (uint64_t*)data;
            const uint64x2_t s = vdupq_n_u64(*(const uint64_t*)scalar);
            size_t vec_end = (len / 2u) * 2u;
            for (; i < vec_end; i += 2u) {
                uint64x2_t v = vld1q_u64(d + i);
                v = vaddq_u64(v, s);
                vst1q_u64(d + i, v);
            }
            for (; i < len; ++i) d[i] += *(const uint64_t*)scalar;
            return;
#else
            for (i = 0u; i < len; ++i) {
                ((uint64_t*)data)[i] += *(const uint64_t*)scalar;
            }
            return;
#endif
        }

        default:
            for (i = 0u; i < len; ++i) add_scalar(data + i * data_size, scalar);
            return;
    }
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_NEON_UINT8_INL */

