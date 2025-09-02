/* simd_avx2_double.inl
   AVX2-only SIMD helpers for double operations.
   Compile with -mavx2 (optionally -mfma to enable FMA fast path).
   Requires: <immintrin.h>
   Contract:
     - Caller validates pointers; for min/max, n > 0.
*/

#ifndef CSALT_SIMD_AVX2_DOUBLE_INL
#define CSALT_SIMD_AVX2_DOUBLE_INL

#include <immintrin.h>

/* ===== Private reductions (256 -> 128 -> scalar via SSE2) ===== */

static inline double csalt_avx2_hsum256_pd(__m256d v) {
    __m128d lo = _mm256_castpd256_pd128(v);
    __m128d hi = _mm256_extractf128_pd(v, 1);
    __m128d s  = _mm_add_pd(lo, hi);            /* [a0+a2, a1+a3] */
    __m128d sh = _mm_unpackhi_pd(s, s);         /* [a1+a3, a1+a3] */
    __m128d r  = _mm_add_sd(s, sh);             /* scalar sum */
    return _mm_cvtsd_f64(r);
}

static inline double csalt_avx2_hmin256_pd(__m256d v) {
    __m128d lo = _mm256_castpd256_pd128(v);
    __m128d hi = _mm256_extractf128_pd(v, 1);
    __m128d m  = _mm_min_pd(lo, hi);
    __m128d sh = _mm_unpackhi_pd(m, m);
    __m128d r  = _mm_min_sd(m, sh);
    return _mm_cvtsd_f64(r);
}

static inline double csalt_avx2_hmax256_pd(__m256d v) {
    __m128d lo = _mm256_castpd256_pd128(v);
    __m128d hi = _mm256_extractf128_pd(v, 1);
    __m128d m  = _mm_max_pd(lo, hi);
    __m128d sh = _mm_unpackhi_pd(m, m);
    __m128d r  = _mm_max_sd(m, sh);
    return _mm_cvtsd_f64(r);
}

/* ===== Public AVX2 helpers (suffixed) ===== */

static inline double simd_sum_f64_avx2(const double* x, size_t n) {
    size_t i = 0;
    __m256d acc = _mm256_setzero_pd();
    for (; i + 4 <= n; i += 4) {
        __m256d v = _mm256_loadu_pd(&x[i]);
        acc = _mm256_add_pd(acc, v);
    }
    double sum = csalt_avx2_hsum256_pd(acc);
    for (; i < n; ++i) sum += x[i];
    return sum;
}

static inline double simd_min_f64_avx2(const double* x, size_t n) {
    size_t i = 0;
    __m256d vmin = _mm256_set1_pd(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m256d v = _mm256_loadu_pd(&x[i]);
        vmin = _mm256_min_pd(vmin, v);
    }
    double m = csalt_avx2_hmin256_pd(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}

static inline double simd_max_f64_avx2(const double* x, size_t n) {
    size_t i = 0;
    __m256d vmax = _mm256_set1_pd(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m256d v = _mm256_loadu_pd(&x[i]);
        vmax = _mm256_max_pd(vmax, v);
    }
    double m = csalt_avx2_hmax256_pd(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}

static inline double simd_dot_f64_avx2(const double* a, const double* b, size_t n) {
    size_t i = 0;
    __m256d acc = _mm256_setzero_pd();
    for (; i + 4 <= n; i += 4) {
        __m256d va = _mm256_loadu_pd(&a[i]);
        __m256d vb = _mm256_loadu_pd(&b[i]);
        #if defined(__FMA__)
            acc = _mm256_fmadd_pd(va, vb, acc);  /* acc += va*vb */
        #else
            acc = _mm256_add_pd(acc, _mm256_mul_pd(va, vb));
        #endif
    }
    double sum = csalt_avx2_hsum256_pd(acc);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}

static inline double simd_mean_f64_avx2(const double* x, size_t n) {
    return n ? simd_sum_f64_avx2(x, n) / (double)n : 0.0;
}

/* Population stdev; switch to (n-1) outside if you need sample stdev. */
static inline double simd_stdev_f64_avx2(const double* x, size_t n) {
    if (n < 2) return 0.0;
    const double mean = simd_mean_f64_avx2(x, n);

    __m256d vmean = _mm256_set1_pd(mean);
    __m256d acc   = _mm256_setzero_pd();
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        __m256d v  = _mm256_loadu_pd(&x[i]);
        __m256d d  = _mm256_sub_pd(v, vmean);
        #if defined(__FMA__)
            acc = _mm256_fmadd_pd(d, d, acc);   /* acc += d*d */
        #else
            acc = _mm256_add_pd(acc, _mm256_mul_pd(d, d));
        #endif
    }
    double ss = csalt_avx2_hsum256_pd(acc);
    for (; i < n; ++i) { double d = x[i] - mean; ss += d * d; }
    return sqrt(ss / (double)n);
}

#endif /* CSALT_SIMD_AVX2_DOUBLE_INL */

