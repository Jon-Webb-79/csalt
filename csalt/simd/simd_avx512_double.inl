/* simd_avx512_double.inl
   AVX-512F helpers for double operations.
   Compile with -mavx512f. Requires: <immintrin.h>
*/
#ifndef CSALT_SIMD_AVX512_DOUBLE_INL
#define CSALT_SIMD_AVX512_DOUBLE_INL

#include <immintrin.h>

/* ---- Private reductions ---- */
static inline double csalt_avx512_hsum512_pd(__m512d v) {
    __m256d lo256 = _mm512_castpd512_pd256(v);
    __m256d hi256 = _mm512_extractf64x4_pd(v, 1);
    __m256d s256  = _mm256_add_pd(lo256, hi256);
    __m128d lo128 = _mm256_castpd256_pd128(s256);
    __m128d hi128 = _mm256_extractf128_pd(s256, 1);
    __m128d s128  = _mm_add_pd(lo128, hi128);           /* [a0+a2, a1+a3] */
    __m128d shuf  = _mm_unpackhi_pd(s128, s128);        /* [a1+a3] */
    __m128d sum   = _mm_add_sd(s128, shuf);             /* scalar */
    return _mm_cvtsd_f64(sum);
}
static inline double csalt_avx512_hmin512_pd(__m512d v) {
    __m256d lo256 = _mm512_castpd512_pd256(v);
    __m256d hi256 = _mm512_extractf64x4_pd(v, 1);
    __m256d m256  = _mm256_min_pd(lo256, hi256);
    __m128d lo128 = _mm256_castpd256_pd128(m256);
    __m128d hi128 = _mm256_extractf128_pd(m256, 1);
    __m128d m128  = _mm_min_pd(lo128, hi128);
    __m128d shuf  = _mm_unpackhi_pd(m128, m128);
    __m128d minv  = _mm_min_sd(m128, shuf);
    return _mm_cvtsd_f64(minv);
}
static inline double csalt_avx512_hmax512_pd(__m512d v) {
    __m256d lo256 = _mm512_castpd512_pd256(v);
    __m256d hi256 = _mm512_extractf64x4_pd(v, 1);
    __m256d m256  = _mm256_max_pd(lo256, hi256);
    __m128d lo128 = _mm256_castpd256_pd128(m256);
    __m128d hi128 = _mm256_extractf128_pd(m256, 1);
    __m128d m128  = _mm_max_pd(lo128, hi128);
    __m128d shuf  = _mm_unpackhi_pd(m128, m128);
    __m128d maxv  = _mm_max_sd(m128, shuf);
    return _mm_cvtsd_f64(maxv);
}

/* ---- Public helpers ---- */
static inline double simd_sum_f64_avx512(const double* x, size_t n) {
    size_t i = 0;
    __m512d acc = _mm512_setzero_pd();
    for (; i + 16/2*2 <= n && i + 16 <= n; i += 16) { /* clarity: 16 doubles per 512b reg */
        __m512d v = _mm512_loadu_pd(&x[i]);
        acc = _mm512_add_pd(acc, v);
    }
    double sum = csalt_avx512_hsum512_pd(acc);
    for (; i < n; ++i) sum += x[i];
    return sum;
}

static inline double simd_min_f64_avx512(const double* x, size_t n) {
    size_t i = 0;
    __m512d vmin = _mm512_set1_pd(x[0]);
    for (; i + 16 <= n; i += 16) {
        __m512d v = _mm512_loadu_pd(&x[i]);
        vmin = _mm512_min_pd(vmin, v);
    }
    double m = csalt_avx512_hmin512_pd(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}

static inline double simd_max_f64_avx512(const double* x, size_t n) {
    size_t i = 0;
    __m512d vmax = _mm512_set1_pd(x[0]);
    for (; i + 16 <= n; i += 16) {
        __m512d v = _mm512_loadu_pd(&x[i]);
        vmax = _mm512_max_pd(vmax, v);
    }
    double m = csalt_avx512_hmax512_pd(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}

static inline double simd_dot_f64_avx512(const double* a, const double* b, size_t n) {
    size_t i = 0;
    __m512d acc = _mm512_setzero_pd();
    for (; i + 16 <= n; i += 16) {
        __m512d va = _mm512_loadu_pd(&a[i]);
        __m512d vb = _mm512_loadu_pd(&b[i]);
    #if defined(__FMA__)
        acc = _mm512_fmadd_pd(va, vb, acc);
    #else
        acc = _mm512_add_pd(acc, _mm512_mul_pd(va, vb));
    #endif
    }
    double sum = csalt_avx512_hsum512_pd(acc);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}

static inline double simd_mean_f64_avx512(const double* x, size_t n) {
    return n ? simd_sum_f64_avx512(x, n) / (double)n : 0.0;
}

static inline double simd_stdev_f64_avx512(const double* x, size_t n) {
    if (n < 2) return 0.0;
    const double mean = simd_mean_f64_avx512(x, n);
    __m512d vmean = _mm512_set1_pd(mean);
    __m512d acc   = _mm512_setzero_pd();
    size_t i = 0;
    for (; i + 16 <= n; i += 16) {
        __m512d v  = _mm512_loadu_pd(&x[i]);
        __m512d d  = _mm512_sub_pd(v, vmean);
    #if defined(__FMA__)
        acc = _mm512_fmadd_pd(d, d, acc);
    #else
        acc = _mm512_add_pd(acc, _mm512_mul_pd(d, d));
    #endif
    }
    double ss = csalt_avx512_hsum512_pd(acc);
    for (; i < n; ++i) { double d = x[i] - mean; ss += d * d; }
    return sqrt(ss / (double)n); /* population */
}

#endif /* CSALT_SIMD_AVX512_DOUBLE_INL */

