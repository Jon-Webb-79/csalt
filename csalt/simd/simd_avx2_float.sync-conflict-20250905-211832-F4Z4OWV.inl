/* simd_avx2_float.inl
   Internal SIMD helpers for float operations.
   Include this into c_float.c AFTER standard headers.
   Requires: <immintrin.h> when __AVX2__ is defined.
*/

#ifndef CSALT_SIMD_AVX2_FLOAT_INL
#define CSALT_SIMD_AVX2_FLOAT_INL

/* Horizontal reduction helpers */
static inline float csalt_hsum256_ps(__m256 v) {
    __m128 low  = _mm256_castps256_ps128(v);
    __m128 high = _mm256_extractf128_ps(v, 1);
    __m128 sum  = _mm_add_ps(low, high);             /* 4-wide */
    sum = _mm_hadd_ps(sum, sum);
    sum = _mm_hadd_ps(sum, sum);
    return _mm_cvtss_f32(sum);
}

static inline float csalt_hmin256_ps(__m256 v) {
    __m128 low  = _mm256_castps256_ps128(v);
    __m128 high = _mm256_extractf128_ps(v, 1);
    __m128 m    = _mm_min_ps(low, high);
    m = _mm_min_ps(m, _mm_movehl_ps(m, m));
    m = _mm_min_ps(m, _mm_shuffle_ps(m, m, 0x1));
    return _mm_cvtss_f32(m);
}

static inline float csalt_hmax256_ps(__m256 v) {
    __m128 low  = _mm256_castps256_ps128(v);
    __m128 high = _mm256_extractf128_ps(v, 1);
    __m128 m    = _mm_max_ps(low, high);
    m = _mm_max_ps(m, _mm_movehl_ps(m, m));
    m = _mm_max_ps(m, _mm_shuffle_ps(m, m, 0x1));
    return _mm_cvtss_f32(m);
}

/* SUM */
static inline float simd_sum_f32(const float* x, size_t n) {
#if defined(__AVX2__)
    size_t i = 0;
    __m256 vsum = _mm256_setzero_ps();
    for (; i + 8 <= n; i += 8) {
        __m256 v = _mm256_loadu_ps(&x[i]);
        vsum = _mm256_add_ps(vsum, v);
    }
    float sum = csalt_hsum256_ps(vsum);
    for (; i < n; ++i) sum += x[i];
    return sum;
#else
    float s = 0.0f;
    for (size_t i = 0; i < n; ++i) s += x[i];
    return s;
#endif
}

/* MIN */
static inline float simd_min_f32(const float* x, size_t n) {
#if defined(__AVX2__)
    size_t i = 0;
    /* guard: if n==0 caller must handle, else seed with first element */
    __m256 vmin = _mm256_set1_ps(x[0]);
    i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 v = _mm256_loadu_ps(&x[i]);
        vmin = _mm256_min_ps(vmin, v);
    }
    float m = csalt_hmin256_ps(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
#else
    float m = x[0];
    for (size_t i = 1; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
#endif
}

/* MAX */
static inline float simd_max_f32(const float* x, size_t n) {
#if defined(__AVX2__)
    size_t i = 0;
    __m256 vmax = _mm256_set1_ps(x[0]);
    for (; i + 8 <= n; i += 8) {
        __m256 v = _mm256_loadu_ps(&x[i]);
        vmax = _mm256_max_ps(vmax, v);
    }
    float m = csalt_hmax256_ps(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
#else
    float m = x[0];
    for (size_t i = 1; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
#endif
}

/* DOT */
static inline float simd_dot_f32(const float* a, const float* b, size_t n) {
#if defined(__AVX2__)
    size_t i = 0;
    __m256 vsum = _mm256_setzero_ps();
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 prod = _mm256_mul_ps(va, vb);
        vsum = _mm256_add_ps(vsum, prod);
    }
    float sum = csalt_hsum256_ps(vsum);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
#else
    float s = 0.0f;
    for (size_t i = 0; i < n; ++i) s += a[i] * b[i];
    return s;
#endif
}

/* MEAN */
static inline float simd_mean_f32(const float* x, size_t n) {
    return n ? simd_sum_f32(x, n) / (float)n : 0.0f;
}

/* STDEV (population by default; switch to sample by flag if desired) */
static inline float simd_stdev_f32(const float* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mean = simd_mean_f32(x, n);

#if defined(__AVX2__)
    __m256 vmean = _mm256_set1_ps(mean);
    __m256 vsum  = _mm256_setzero_ps();
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 v   = _mm256_loadu_ps(&x[i]);
        __m256 d   = _mm256_sub_ps(v, vmean);
        __m256 sq  = _mm256_mul_ps(d, d);
        vsum = _mm256_add_ps(vsum, sq);
    }
    float ss = csalt_hsum256_ps(vsum);
    for (; i < n; ++i) {
        float d = x[i] - mean;
        ss += d * d;
    }
#else
    float ss = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        float d = x[i] - mean;
        ss += d * d;
    }
#endif
    /* Population stdev; change denominator to (n-1) for sample stdev */
    return sqrtf(ss / (float)n);
}

#endif /* CSALT_SIMD_AVX2_FLOAT_INL */

