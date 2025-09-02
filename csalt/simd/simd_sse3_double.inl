/* simd_sse3_double.inl
   SSE3 helpers for double operations (uses _mm_hadd_pd for hsum).
   Compile with -msse3. Requires: <xmmintrin.h>, <emmintrin.h>, <pmmintrin.h>
*/
#ifndef CSALT_SIMD_SSE3_DOUBLE_INL
#define CSALT_SIMD_SSE3_DOUBLE_INL

#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

/* reductions using SSE3 hadd (min/max via SSE2) */
static inline double csalt_sse3_hsum128_pd(__m128d v) {
    __m128d t = _mm_hadd_pd(v, v);  /* [a0+a1, a0+a1] */
    return _mm_cvtsd_f64(t);
}

static inline double csalt_sse3_hmin128_pd(__m128d v) {
    __m128d sh = _mm_unpackhi_pd(v, v);
    __m128d m  = _mm_min_sd(v, sh);
    return _mm_cvtsd_f64(m);
}
static inline double csalt_sse3_hmax128_pd(__m128d v) {
    __m128d sh = _mm_unpackhi_pd(v, v);
    __m128d m  = _mm_max_sd(v, sh);
    return _mm_cvtsd_f64(m);
}

/* helpers */
static inline double simd_sum_f64_sse3(const double* x, size_t n) {
    size_t i = 0;
    __m128d acc = _mm_setzero_pd();
    for (; i + 2 <= n; i += 2) {
        __m128d v = _mm_loadu_pd(&x[i]);
        acc = _mm_add_pd(acc, v);
    }
    double sum = csalt_sse3_hsum128_pd(acc);
    for (; i < n; ++i) sum += x[i];
    return sum;
}
static inline double simd_min_f64_sse3(const double* x, size_t n) {
    size_t i = 0;
    __m128d vmin = _mm_set1_pd(x[0]);
    for (; i + 2 <= n; i += 2) {
        __m128d v = _mm_loadu_pd(&x[i]);
        vmin = _mm_min_pd(vmin, v);
    }
    double m = csalt_sse3_hmin128_pd(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}
static inline double simd_max_f64_sse3(const double* x, size_t n) {
    size_t i = 0;
    __m128d vmax = _mm_set1_pd(x[0]);
    for (; i + 2 <= n; i += 2) {
        __m128d v = _mm_loadu_pd(&x[i]);
        vmax = _mm_max_pd(vmax, v);
    }
    double m = csalt_sse3_hmax128_pd(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}
static inline double simd_dot_f64_sse3(const double* a, const double* b, size_t n) {
    size_t i = 0;
    __m128d acc = _mm_setzero_pd();
    for (; i + 2 <= n; i += 2) {
        __m128d va = _mm_loadu_pd(&a[i]);
        __m128d vb = _mm_loadu_pd(&b[i]);
        acc = _mm_add_pd(acc, _mm_mul_pd(va, vb));
    }
    double sum = csalt_sse3_hsum128_pd(acc);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}
static inline double simd_mean_f64_sse3(const double* x, size_t n) {
    return n ? simd_sum_f64_sse3(x, n) / (double)n : 0.0;
}
static inline double simd_stdev_f64_sse3(const double* x, size_t n) {
    if (n < 2) return 0.0;
    const double mean = simd_mean_f64_sse3(x, n);
    __m128d vmean = _mm_set1_pd(mean);
    __m128d acc   = _mm_setzero_pd();
    size_t i = 0;
    for (; i + 2 <= n; i += 2) {
        __m128d v  = _mm_loadu_pd(&x[i]);
        __m128d d  = _mm_sub_pd(v, vmean);
        acc = _mm_add_pd(acc, _mm_mul_pd(d, d));
    }
    double ss = csalt_sse3_hsum128_pd(acc);
    for (; i < n; ++i) { double d = x[i] - mean; ss += d * d; }
    return sqrt(ss / (double)n);
}

#endif /* CSALT_SIMD_SSE3_DOUBLE_INL */

