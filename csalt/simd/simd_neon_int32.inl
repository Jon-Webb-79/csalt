/* simd_neon_int32.inl
   NEON helpers for int32 ops + float mean/stdev (AArch64).
   Requires: <arm_neon.h>, <math.h>.
*/

#ifndef CSALT_SIMD_NEON_INT32_INL
#define CSALT_SIMD_NEON_INT32_INL

#include <arm_neon.h>
#include <stdint.h>
#include <math.h>

static inline int64_t simd_sum_i32_neon(const int32_t* x, size_t n) {
    size_t i = 0;
    int64x2_t acc0 = vdupq_n_s64(0), acc1 = vdupq_n_s64(0);
    for (; i + 4 <= n; i += 4) {
        int32x4_t v = vld1q_s32(&x[i]);
        acc0 = vaddq_s64(acc0, vmovl_s32(vget_low_s32(v)));
        acc1 = vaddq_s64(acc1, vmovl_s32(vget_high_s32(v)));
    }
    int64x2_t acc = vaddq_s64(acc0, acc1);
    int64_t sum = vaddvq_s64(acc);
    for (; i < n; ++i) sum += x[i];
    return sum;
}

static inline int32_t simd_min_i32_neon(const int32_t* x, size_t n) {
    size_t i = 0; int32x4_t vmin = vdupq_n_s32(x[0]);
    for (; i + 4 <= n; i += 4) vmin = vminq_s32(vmin, vld1q_s32(&x[i]));
    int32_t m = vminvq_s32(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}
static inline int32_t simd_max_i32_neon(const int32_t* x, size_t n) {
    size_t i = 0; int32x4_t vmax = vdupq_n_s32(x[0]);
    for (; i + 4 <= n; i += 4) vmax = vmaxq_s32(vmax, vld1q_s32(&x[i]));
    int32_t m = vmaxvq_s32(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}

static inline int64_t simd_dot_i32_neon(const int32_t* a, const int32_t* b, size_t n) {
    size_t i = 0; int64x2_t acc = vdupq_n_s64(0);
    for (; i + 4 <= n; i += 4) {
        int32x4_t va = vld1q_s32(&a[i]);
        int32x4_t vb = vld1q_s32(&b[i]);
        /* widen multiply low and high halves */
        acc = vaddq_s64(acc, vmull_s32(vget_low_s32(va),  vget_low_s32(vb)));
        acc = vaddq_s64(acc, vmull_s32(vget_high_s32(va), vget_high_s32(vb)));
    }
    int64_t sum = vaddvq_s64(acc);
    for (; i < n; ++i) sum += (int64_t)a[i] * (int64_t)b[i];
    return sum;
}

static inline float simd_mean_i32_neon(const int32_t* x, size_t n) {
    if (!n) return 0.0f;
    int64_t s = simd_sum_i32_neon(x, n);
    return (float)((double)s / (double)n);
}
static inline float simd_stdev_i32_neon(const int32_t* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mu = simd_mean_i32_neon(x, n);
    float32x4_t vmu = vdupq_n_f32(mu), acc = vdupq_n_f32(0.0f);
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        float32x4_t vf = vcvtq_f32_s32(vld1q_s32(&x[i]));
        float32x4_t d  = vsubq_f32(vf, vmu);
        acc = vmlaq_f32(acc, d, d);
    }
    float ss = vaddvq_f32(acc);
    for (; i < n; ++i) { float d = (float)x[i] - mu; ss += d * d; }
    return sqrtf(ss / (float)n);
}

#endif /* CSALT_SIMD_NEON_INT32_INL */

