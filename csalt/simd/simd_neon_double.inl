/* simd_neon_double.inl
   NEON helpers for double operations (AArch64 only).
   Compile on AArch64 targets. Requires: <arm_neon.h>
*/
#ifndef CSALT_SIMD_NEON_DOUBLE_INL
#define CSALT_SIMD_NEON_DOUBLE_INL

#include <arm_neon.h>

/* reductions */
static inline double csalt_neon_hsum2_f64(float64x2_t v) {
#if defined(__aarch64__)
    return vaddvq_f64(v);
#else
    /* Should not reach: NEON FP64 vectors require AArch64 */
    double tmp[2]; vst1q_f64(tmp, v); return tmp[0] + tmp[1];
#endif
}
static inline double csalt_neon_hmin2_f64(float64x2_t v) {
#if defined(__aarch64__)
    return vminvq_f64(v);
#else
    double tmp[2]; vst1q_f64(tmp, v); return tmp[0] < tmp[1] ? tmp[0] : tmp[1];
#endif
}
static inline double csalt_neon_hmax2_f64(float64x2_t v) {
#if defined(__aarch64__)
    return vmaxvq_f64(v);
#else
    double tmp[2]; vst1q_f64(tmp, v); return tmp[0] > tmp[1] ? tmp[0] : tmp[1];
#endif
}

/* helpers */
static inline double simd_sum_f64_neon(const double* x, size_t n) {
    size_t i = 0;
    float64x2_t acc = vdupq_n_f64(0.0);
    for (; i + 2 <= n; i += 2) {
        float64x2_t v = vld1q_f64(&x[i]);
        acc = vaddq_f64(acc, v);
    }
    double sum = csalt_neon_hsum2_f64(acc);
    for (; i < n; ++i) sum += x[i];
    return sum;
}
static inline double simd_min_f64_neon(const double* x, size_t n) {
    size_t i = 0;
    float64x2_t vmin = vdupq_n_f64(x[0]);
    for (; i + 2 <= n; i += 2) {
        float64x2_t v = vld1q_f64(&x[i]);
        vmin = vminq_f64(vmin, v);
    }
    double m = csalt_neon_hmin2_f64(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}
static inline double simd_max_f64_neon(const double* x, size_t n) {
    size_t i = 0;
    float64x2_t vmax = vdupq_n_f64(x[0]);
    for (; i + 2 <= n; i += 2) {
        float64x2_t v = vld1q_f64(&x[i]);
        vmax = vmaxq_f64(vmax, v);
    }
    double m = csalt_neon_hmax2_f64(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}
static inline double simd_dot_f64_neon(const double* a, const double* b, size_t n) {
    size_t i = 0;
    float64x2_t acc = vdupq_n_f64(0.0);
    for (; i + 2 <= n; i += 2) {
        float64x2_t va = vld1q_f64(&a[i]);
        float64x2_t vb = vld1q_f64(&b[i]);
        /* vmlaq_f64: acc += va * vb (non-fused multiply-add) */
        acc = vmlaq_f64(acc, va, vb);
    }
    double sum = csalt_neon_hsum2_f64(acc);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}
static inline double simd_mean_f64_neon(const double* x, size_t n) {
    return n ? simd_sum_f64_neon(x, n) / (double)n : 0.0;
}
static inline double simd_stdev_f64_neon(const double* x, size_t n) {
    if (n < 2) return 0.0;
    const double mean = simd_mean_f64_neon(x, n);
    size_t i = 0; 
    float64x2_t vmean = vdupq_n_f64(mean);
    float64x2_t acc   = vdupq_n_f64(0.0);
    for (; i + 2 <= n; i += 2) {
        float64x2_t v  = vld1q_f64(&x[i]);
        float64x2_t d  = vsubq_f64(v, vmean);
        acc = vmlaq_f64(acc, d, d);
    }
    double ss = csalt_neon_hsum2_f64(acc);
    for (; i < n; ++i) { double d = x[i] - mean; ss += d * d; }
    return sqrt(ss / (double)n);
}

#endif /* CSALT_SIMD_NEON_DOUBLE_INL */

