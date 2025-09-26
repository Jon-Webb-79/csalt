/* simd_neon_char.inl
   NEON helpers for last-occurrence search in bytes.
   Requires: <arm_neon.h>
*/
#ifndef CSALT_SIMD_NEON_CHAR_INL
#define CSALT_SIMD_NEON_CHAR_INL

#include <stddef.h>
#include <stdint.h>
#include <arm_neon.h>

/* Build a 16-bit movemask from MSBs of each byte */
static inline uint32_t csalt_neon_movemask_u8(uint8x16_t v) {
    /* MSB -> bit0 per lane */
    uint8x16_t msb = vshrq_n_u8(v, 7);
    /* Multiply by powers of two and horizontally add to a 16-bit mask */
    static const uint8_t kbits_data[16] = {
        1,2,4,8,16,32,64,128, 1,2,4,8,16,32,64,128
    };
    uint8x16_t kbits = vld1q_u8(kbits_data);
    uint8x16_t masked = vandq_u8(msb, kbits);

    uint16x8_t sum16 = vpaddlq_u8(masked);   /* 16 -> 8 */
    uint32x4_t sum32 = vpaddlq_u16(sum16);   /* 8  -> 4 */
    uint64x2_t sum64 = vpaddlq_u32(sum32);   /* 4  -> 2 */
    uint64_t lo = vgetq_lane_u64(sum64, 0);
    uint64_t hi = vgetq_lane_u64(sum64, 1);
    return (uint32_t)(lo | (hi << 8));       /* 16-bit mask in low bits */
}

static inline int csalt_highbit_u32(uint32_t m) { return 31 - __builtin_clz(m); }

static inline size_t simd_last_index_u8_neon(const unsigned char* s, size_t n, unsigned char c) {
    size_t i = 0, last = SIZE_MAX;
    const uint8x16_t needle = vdupq_n_u8(c);

    while (i + 16 <= n) {
        uint8x16_t v  = vld1q_u8(s + i);
        uint8x16_t eq = vceqq_u8(v, needle);
        uint32_t mask = csalt_neon_movemask_u8(eq);
        if (mask) last = i + (size_t)csalt_highbit_u32(mask);
        i += 16;
    }
    for (size_t j = n; j-- > i; ) if (s[j] == c) return j;
    return last;
}

#define CSALT_LAST_U8_INDEX(s, n, c) simd_last_index_u8_neon((s), (n), (c))

#endif /* CSALT_SIMD_NEON_CHAR_INL */

