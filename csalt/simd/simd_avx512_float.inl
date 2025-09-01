/* simd_avx512_float.inl
   AVX-512F helpers for float operations.
   Compile with -mavx512f (GCC/Clang) or /arch:AVX512 on MSVC (where supported).
   Requires: <immintrin.h>
*/
#ifndef CSALT_SIMD_AVX512_FLOAT_INL
#define CSALT_SIMD_AVX512_FLOAT_INL

#include <immintrin.h>

/* ----- Private reductions (portable across compilers) ----- */
static inline float csalt_avx512_hsum512_ps(__m512 v) {
    /* Reduce 512->256->scalar using AVX ops (works everywhere with AVX-512 headers) */
    __m256 lo = _mm512_castps512_ps256(v);                 /* low 256 */
    __m256 hi = _mm512_extractf32x8_ps(v, 1);              /* high 256 */
    __m256 sum256 = _mm256_add_ps(lo, hi);

    __m128 low  = _mm256_castps256_ps128(sum256);
    __m128 high = _mm256_extractf128_ps(sum256, 1);
    __m128 sum  = _mm_add_ps(low, high);                   /* 4-wide */
    sum = _mm_hadd_ps(sum, sum);
    sum = _mm_hadd_ps(sum, sum);
    return _mm_cvtss_f32(sum);
}
static inline float csalt_avx512_hmin512_ps(__m512 v) {
    __m256 lo = _mm512_castps512_ps256(v);
    __m256 hi = _mm512_extractf32x8_ps(v, 1);
    __m256 m256 = _mm256_min_ps(lo, hi);

    __m128 low  = _mm256_castps256_ps128(m256);
    __m128 high = _mm256_extractf128_ps(m256, 1);
    __m128 m    = _mm_min_ps(low, high);
    m = _mm_min_ps(m, _mm_movehl_ps(m, m));
    m = _mm_min_ps(m, _mm_shuffle_ps(m, m, 0x1));
    return _mm_cvtss_f32(m);
}
static inline float csalt_avx512_hmax512_ps(__m512 v) {
    __m256 lo = _mm512_castps512_ps256(v);
    __m256 hi = _mm512_extractf32x8_ps(v, 1);
    __m256 m256 = _mm256_max_ps(lo, hi);

    __m128 low  = _mm256_castps256_ps128(m256);
    __m128 high = _mm256_extractf128_ps(m256, 1);
    __m128 m    = _mm_max_ps(low, high);
    m = _mm_max_ps(m, _mm_movehl_ps(m, m));
    m = _mm_max_ps(m, _mm_shuffle_ps(m, m, 0x1));
    return _mm_cvtss_f32(m);
}

/* ----- Public helpers (AVX-512F) ----- */
static inline float simd_sum_f32_avx512(const float* x, size_t n) {
    size_t i = 0;
    __m512 acc = _mm512_setzero_ps();
    for (; i + 16 <= n; i += 16) {
        __m512 v = _mm512_loadu_ps(&x[i]);
        acc = _mm512_add_ps(acc, v);
    }
    float sum = csalt_avx512_hsum512_ps(acc);
    for (; i < n; ++i) sum += x[i];
    return sum;
}

static inline float simd_min_f32_avx512(const float* x, size_t n) {
    size_t i = 0;
    __m512 vmin = _mm512_set1_ps(x[0]);
    for (; i + 16 <= n; i += 16) {
        __m512 v = _mm512_loadu_ps(&x[i]);
        vmin = _mm512_min_ps(vmin, v);
    }
    float m = csalt_avx512_hmin512_ps(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}

static inline float simd_max_f32_avx512(const float* x, size_t n) {
    size_t i = 0;
    __m512 vmax = _mm512_set1_ps(x[0]);
    for (; i + 16 <= n; i += 16) {
        __m512 v = _mm512_loadu_ps(&x[i]);
        vmax = _mm512_max_ps(vmax, v);
    }
    float m = csalt_avx512_hmax512_ps(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}

static inline float simd_dot_f32_avx512(const float* a, const float* b, size_t n) {
    size_t i = 0;
    __m512 acc = _mm512_setzero_ps();
    for (; i + 16 <= n; i += 16) {
        __m512 va = _mm512_loadu_ps(&a[i]);
        __m512 vb = _mm512_loadu_ps(&b[i]);
        acc = _mm512_fmadd_ps(va, vb, acc); /* acc += a*b */
    }
    float sum = csalt_avx512_hsum512_ps(acc);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}

static inline float simd_mean_f32_avx512(const float* x, size_t n) {
    return n ? simd_sum_f32_avx512(x, n) / (float)n : 0.0f;
}

static inline float simd_stdev_f32_avx512(const float* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mean = simd_mean_f32_avx512(x, n);
    __m512 vmean = _mm512_set1_ps(mean);
    __m512 acc   = _mm512_setzero_ps();

    size_t i = 0;
    for (; i + 16 <= n; i += 16) {
        __m512 v  = _mm512_loadu_ps(&x[i]);
        __m512 d  = _mm512_sub_ps(v, vmean);
        acc = _mm512_fmadd_ps(d, d, acc);                 /* acc += d*d */
    }
    float ss = csalt_avx512_hsum512_ps(acc);
    for (; i < n; ++i) { float d = x[i] - mean; ss += d * d; }

    return sqrtf(ss / (float)n);                           /* population stdev */
}

#endif /* CSALT_SIMD_AVX512_FLOAT_INL */

