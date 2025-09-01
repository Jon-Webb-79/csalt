/* simd_sse2_float.inl
   SSE2-only SIMD helpers for float operations.
   Include into a C TU compiled with -msse2 (GCC/Clang) or enabled SSE2 on MSVC.
   Requires: <xmmintrin.h>, <emmintrin.h>, and the including TU should include <math.h> for sqrtf.
   Contract:
     - Caller validates pointers and sizes (n>0 for min/max).
     - Functions are internal helpers; keep them static inline.
*/

#ifndef CSALT_SIMD_SSE2_FLOAT_INL
#define CSALT_SIMD_SSE2_FLOAT_INL

#include <xmmintrin.h>  /* SSE  */
#include <emmintrin.h>  /* SSE2 */

/* ===== Private horizontal reductions (pure SSE2) ===== */

/* 4-lane horizontal sum using shuffles/adds (no SSE3 hadd). */
static inline float csalt_sse2_hsum128_ps(__m128 v) {
    /* v = [a3 a2 a1 a0] */
    __m128 shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,3,0,1)); /* [a2 a3 a0 a1] */
    __m128 sums = _mm_add_ps(v, shuf);                        /* [a3+a2, a2+a3, a1+a0, a0+a1] */
    shuf = _mm_movehl_ps(shuf, sums);                         /* move high pair to low: [*,*,a3+a2,a2+a3] -> [a3+a2,a2+a3,*,*] */
    sums = _mm_add_ss(sums, shuf);                            /* (a1+a0) + (a3+a2) in lane 0 */
    return _mm_cvtss_f32(sums);
}

/* 4-lane horizontal min (pure SSE2). */
static inline float csalt_sse2_hmin128_ps(__m128 v) {
    __m128 shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,3,0,1)); /* swap pairs */
    __m128 mins = _mm_min_ps(v, shuf);                        /* min of pairs */
    shuf = _mm_shuffle_ps(mins, mins, _MM_SHUFFLE(1,0,3,2));  /* swap within pairs */
    mins = _mm_min_ps(mins, shuf);                            /* min across all 4 */
    return _mm_cvtss_f32(mins);
}

/* 4-lane horizontal max (pure SSE2). */
static inline float csalt_sse2_hmax128_ps(__m128 v) {
    __m128 shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,3,0,1)); /* swap pairs */
    __m128 maxs = _mm_max_ps(v, shuf);                        /* max of pairs */
    shuf = _mm_shuffle_ps(maxs, maxs, _MM_SHUFFLE(1,0,3,2));  /* swap within pairs */
    maxs = _mm_max_ps(maxs, shuf);                            /* max across all 4 */
    return _mm_cvtss_f32(maxs);
}

/* ===== Public SSE2 helpers (suffixed) ===== */

/* SUM */
static inline float simd_sum_f32_sse(const float* x, size_t n) {
    size_t i = 0;
    __m128 vsum = _mm_setzero_ps();
    for (; i + 4 <= n; i += 4) {
        __m128 v = _mm_loadu_ps(&x[i]);
        vsum = _mm_add_ps(vsum, v);
    }
    float sum = csalt_sse2_hsum128_ps(vsum);
    for (; i < n; ++i) sum += x[i];
    return sum;
}

/* MIN (assumes n>=1) */
static inline float simd_min_f32_sse(const float* x, size_t n) {
    size_t i = 0;
    __m128 vmin = _mm_set1_ps(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m128 v = _mm_loadu_ps(&x[i]);
        vmin = _mm_min_ps(vmin, v);
    }
    float m = csalt_sse2_hmin128_ps(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}

/* MAX (assumes n>=1) */
static inline float simd_max_f32_sse(const float* x, size_t n) {
    size_t i = 0;
    __m128 vmax = _mm_set1_ps(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m128 v = _mm_loadu_ps(&x[i]);
        vmax = _mm_max_ps(vmax, v);
    }
    float m = csalt_sse2_hmax128_ps(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}

/* DOT */
static inline float simd_dot_f32_sse(const float* a, const float* b, size_t n) {
    size_t i = 0;
    __m128 vacc = _mm_setzero_ps();
    for (; i + 4 <= n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 prod = _mm_mul_ps(va, vb);
        vacc = _mm_add_ps(vacc, prod);
    }
    float sum = csalt_sse2_hsum128_ps(vacc);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}

/* MEAN */
static inline float simd_mean_f32_sse(const float* x, size_t n) {
    return n ? simd_sum_f32_sse(x, n) / (float)n : 0.0f;
}

/* STDEV (population; change to n-1 outside if you need sample) */
static inline float simd_stdev_f32_sse(const float* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mean = simd_mean_f32_sse(x, n);

    __m128 vmean = _mm_set1_ps(mean);
    __m128 vsum  = _mm_setzero_ps();
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        __m128 v  = _mm_loadu_ps(&x[i]);
        __m128 d  = _mm_sub_ps(v, vmean);
        __m128 sq = _mm_mul_ps(d, d);
        vsum = _mm_add_ps(vsum, sq);
    }
    float ss = csalt_sse2_hsum128_ps(vsum);
    for (; i < n; ++i) {
        float d = x[i] - mean;
        ss += d * d;
    }
    return sqrtf(ss / (float)n);
}

#endif /* CSALT_SIMD_SSE2_FLOAT_INL */

