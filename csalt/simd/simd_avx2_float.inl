/* simd_avx2_float.inl
   AVX2-only SIMD helpers for float operations.
   Include into a C TU that is compiled with -mavx2 (or MSVC /arch:AVX2).
   Requires: <immintrin.h>
   Contract:
     - Caller guarantees n > 0 for min/max (we seed with x[0])
     - Functions are internal helpers; keep them static inline
*/

#ifndef CSALT_SIMD_AVX2_FLOAT_INL
#define CSALT_SIMD_AVX2_FLOAT_INL

/* ===== Private reduction helpers (AVX/AVX2) =====
   Note: These use _mm_hadd_ps (SSE3). All AVX2-capable CPUs support SSE3.
*/
static inline float csalt_avx2_hsum256_ps(__m256 v) {
    __m128 low  = _mm256_castps256_ps128(v);
    __m128 high = _mm256_extractf128_ps(v, 1);
    __m128 sum  = _mm_add_ps(low, high);    /* 4-wide partial sum */
    sum = _mm_hadd_ps(sum, sum);
    sum = _mm_hadd_ps(sum, sum);
    return _mm_cvtss_f32(sum);
}

static inline float csalt_avx2_hmin256_ps(__m256 v) {
    __m128 low  = _mm256_castps256_ps128(v);
    __m128 high = _mm256_extractf128_ps(v, 1);
    __m128 m    = _mm_min_ps(low, high);
    m = _mm_min_ps(m, _mm_movehl_ps(m, m));
    m = _mm_min_ps(m, _mm_shuffle_ps(m, m, 0x1));
    return _mm_cvtss_f32(m);
}

static inline float csalt_avx2_hmax256_ps(__m256 v) {
    __m128 low  = _mm256_castps256_ps128(v);
    __m128 high = _mm256_extractf128_ps(v, 1);
    __m128 m    = _mm_max_ps(low, high);
    m = _mm_max_ps(m, _mm_movehl_ps(m, m));
    m = _mm_max_ps(m, _mm_shuffle_ps(m, m, 0x1));
    return _mm_cvtss_f32(m);
}

/* ===== Public AVX2 helpers (suffixed) ===== */

static inline float simd_sum_f32_avx2(const float* x, size_t n) {
    size_t i = 0;
    __m256 vsum = _mm256_setzero_ps();
    for (; i + 8 <= n; i += 8) {
        __m256 v = _mm256_loadu_ps(&x[i]);
        vsum = _mm256_add_ps(vsum, v);
    }
    float sum = csalt_avx2_hsum256_ps(vsum);
    for (; i < n; ++i) sum += x[i];
    return sum;
}

static inline float simd_min_f32_avx2(const float* x, size_t n) {
    size_t i = 0;
    __m256 vmin = _mm256_set1_ps(x[0]);
    for (; i + 8 <= n; i += 8) {
        __m256 v = _mm256_loadu_ps(&x[i]);
        vmin = _mm256_min_ps(vmin, v);
    }
    float m = csalt_avx2_hmin256_ps(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}

static inline float simd_max_f32_avx2(const float* x, size_t n) {
    size_t i = 0;
    __m256 vmax = _mm256_set1_ps(x[0]);
    for (; i + 8 <= n; i += 8) {
        __m256 v = _mm256_loadu_ps(&x[i]);
        vmax = _mm256_max_ps(vmax, v);
    }
    float m = csalt_avx2_hmax256_ps(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}

static inline float simd_dot_f32_avx2(const float* a, const float* b, size_t n) {
    size_t i = 0;
    __m256 vsum = _mm256_setzero_ps();
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 prod = _mm256_mul_ps(va, vb);
        vsum = _mm256_add_ps(vsum, prod);
    }
    float sum = csalt_avx2_hsum256_ps(vsum);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}

static inline float simd_mean_f32_avx2(const float* x, size_t n) {
    return n ? simd_sum_f32_avx2(x, n) / (float)n : 0.0f;
}

/* Population stdev by default; change denom to (n-1) in your dispatcher if you need sample. */
/* Population stdev by default; change denom to (n-1) in your dispatcher if you need sample. */
static inline float simd_stdev_f32_avx2(const float* x, size_t n) {
    if (n < 2) return 0.0f;

    const float mean = simd_mean_f32_avx2(x, n);
    __m256 vmean = _mm256_set1_ps(mean);
    __m256 vsum  = _mm256_setzero_ps();

    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 v  = _mm256_loadu_ps(&x[i]);
        __m256 d  = _mm256_sub_ps(v, vmean);
        #if defined(__FMA__)
            vsum = _mm256_fmadd_ps(d, d, vsum);   /* vsum += d*d */
        #else
            __m256 sq = _mm256_mul_ps(d, d);
            vsum = _mm256_add_ps(vsum, sq);
        #endif
    }

    float ss = csalt_avx2_hsum256_ps(vsum);
    for (; i < n; ++i) {
        float d = x[i] - mean;
        ss += d * d;
    }

    return sqrtf(ss / (float)n);  /* population stdev */
}

