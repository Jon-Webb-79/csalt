/* simd_neon_float.inl
   ARM NEON helpers for float operations.
   On AArch64, NEON is baseline; on 32-bit ARM use -mfpu=neon (GCC/Clang).
   Requires: <arm_neon.h>
*/
#ifndef CSALT_SIMD_NEON_FLOAT_INL
#define CSALT_SIMD_NEON_FLOAT_INL

#include <arm_neon.h>

/* ----- Private reduction helpers ----- */
static inline float csalt_neon_hsum4_f32(float32x4_t v) {
#if defined(__aarch64__)
    return vaddvq_f32(v);  /* horizontal add (AArch64) */
#else
    float32x2_t vlow  = vget_low_f32(v);
    float32x2_t vhigh = vget_high_f32(v);
    float32x2_t sum2  = vpadd_f32(vlow, vhigh); /* [a0+a1, a2+a3] */
    float32x2_t sum1  = vpadd_f32(sum2, sum2);  /* [sum, sum] */
    return vget_lane_f32(sum1, 0);
#endif
}
static inline float csalt_neon_hmin4_f32(float32x4_t v) {
#if defined(__aarch64__)
    return vminvq_f32(v);
#else
    float32x2_t lo = vget_low_f32(v);
    float32x2_t hi = vget_high_f32(v);
    float32x2_t m2 = vpmin_f32(lo, hi);   /* pairwise min */
    float32x2_t m1 = vpmin_f32(m2, m2);
    return vget_lane_f32(m1, 0);
#endif
}
static inline float csalt_neon_hmax4_f32(float32x4_t v) {
#if defined(__aarch64__)
    return vmaxvq_f32(v);
#else
    float32x2_t lo = vget_low_f32(v);
    float32x2_t hi = vget_high_f32(v);
    float32x2_t m2 = vpmax_f32(lo, hi);   /* pairwise max */
    float32x2_t m1 = vpmax_f32(m2, m2);
    return vget_lane_f32(m1, 0);
#endif
}

/* ----- Public helpers (NEON) ----- */
static inline float simd_sum_f32_neon(const float* x, size_t n) {
    size_t i = 0;
    float32x4_t acc = vdupq_n_f32(0.0f);
    for (; i + 4 <= n; i += 4) {
        float32x4_t v = vld1q_f32(&x[i]);
        acc = vaddq_f32(acc, v);
    }
    float sum = csalt_neon_hsum4_f32(acc);
    for (; i < n; ++i) sum += x[i];
    return sum;
}

static inline float simd_min_f32_neon(const float* x, size_t n) {
    size_t i = 0;
    float32x4_t vmin = vdupq_n_f32(x[0]);
    for (; i + 4 <= n; i += 4) {
        float32x4_t v = vld1q_f32(&x[i]);
        vmin = vminq_f32(vmin, v);
    }
    float m = csalt_neon_hmin4_f32(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}

static inline float simd_max_f32_neon(const float* x, size_t n) {
    size_t i = 0;
    float32x4_t vmax = vdupq_n_f32(x[0]);
    for (; i + 4 <= n; i += 4) {
        float32x4_t v = vld1q_f32(&x[i]);
        vmax = vmaxq_f32(vmax, v);
    }
    float m = csalt_neon_hmax4_f32(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}

static inline float simd_dot_f32_neon(const float* a, const float* b, size_t n) {
    size_t i = 0;
    float32x4_t acc = vdupq_n_f32(0.0f);
    for (; i + 4 <= n; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        acc = vmlaq_f32(acc, va, vb); /* acc += va*vb */
    }
    float sum = csalt_neon_hsum4_f32(acc);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}

static inline float simd_mean_f32_neon(const float* x, size_t n) {
    return n ? simd_sum_f32_neon(x, n) / (float)n : 0.0f;
}

static inline float simd_stdev_f32_neon(const float* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mean = simd_mean_f32_neon(x, n);

    size_t i = 0;
    float32x4_t vmean = vdupq_n_f32(mean);
    float32x4_t acc   = vdupq_n_f32(0.0f);
    for (; i + 4 <= n; i += 4) {
        float32x4_t v  = vld1q_f32(&x[i]);
        float32x4_t d  = vsubq_f32(v, vmean);
        acc = vmlaq_f32(acc, d, d);     /* acc += d*d */
    }
    float ss = csalt_neon_hsum4_f32(acc);
    for (; i < n; ++i) { float d = x[i] - mean; ss += d * d; }

    return sqrtf(ss / (float)n);        /* population stdev */
}

#endif /* CSALT_SIMD_NEON_FLOAT_INL */

