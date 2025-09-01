/* simd_sse41_float.inl
   SSE4.1 helpers for float operations (uses _mm_dp_ps for dot).
   Compile with -msse4.1 (GCC/Clang) or enable SSE4.1 on MSVC.
   Requires: <xmmintrin.h>, <emmintrin.h>, <smmintrin.h>
   Contract:
     - Caller validates pointers; for min/max, n > 0.
*/

#ifndef CSALT_SIMD_SSE41_FLOAT_INL
#define CSALT_SIMD_SSE4_FLOAT_INL

#include <xmmintrin.h>  /* SSE  */
#include <emmintrin.h>  /* SSE2 */
#include <smmintrin.h>  /* SSE4.1 */

/* Horizontal reductions (pure SSE2 to avoid SSE3 dependency) */
static inline float csalt_sse41_hsum128_ps(__m128 v) {
    __m128 sh = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,3,0,1));
    __m128 s1 = _mm_add_ps(v, sh);
    sh = _mm_shuffle_ps(s1, s1, _MM_SHUFFLE(1,0,3,2));
    s1 = _mm_add_ss(s1, sh);
    return _mm_cvtss_f32(s1);
}

static inline float csalt_sse41_hmin128_ps(__m128 v) {
    __m128 sh = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,3,0,1));
    __m128 m  = _mm_min_ps(v, sh);
    sh = _mm_shuffle_ps(m, m, _MM_SHUFFLE(1,0,3,2));
    m  = _mm_min_ps(m, sh);
    return _mm_cvtss_f32(m);
}

static inline float csalt_sse41_hmax128_ps(__m128 v) {
    __m128 sh = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,3,0,1));
    __m128 m  = _mm_max_ps(v, sh);
    sh = _mm_shuffle_ps(m, m, _MM_SHUFFLE(1,0,3,2));
    m  = _mm_max_ps(m, sh);
    return _mm_cvtss_f32(m);
}

/* ===== Public SSE4.1 helpers (suffixed) ===== */

static inline float simd_sum_f32_sse41(const float* x, size_t n) {
    size_t i = 0;
    __m128 vsum = _mm_setzero_ps();
    for (; i + 4 <= n; i += 4) {
        __m128 v = _mm_loadu_ps(&x[i]);
        vsum = _mm_add_ps(vsum, v);
    }
    float sum = csalt_sse41_hsum128_ps(vsum);
    for (; i < n; ++i) sum += x[i];
    return sum;
}

static inline float simd_min_f32_sse41(const float* x, size_t n) {
    size_t i = 0;
    __m128 vmin = _mm_set1_ps(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m128 v = _mm_loadu_ps(&x[i]);
        vmin = _mm_min_ps(vmin, v);
    }
    float m = csalt_sse41_hmin128_ps(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}

static inline float simd_max_f32_sse41(const float* x, size_t n) {
    size_t i = 0;
    __m128 vmax = _mm_set1_ps(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m128 v = _mm_loadu_ps(&x[i]);
        vmax = _mm_max_ps(vmax, v);
    }
    float m = csalt_sse41_hmax128_ps(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}

/* DOT using DPPS: mask 0xF1 => multiply all 4 lanes, sum to lane0, zero others */
static inline float simd_dot_f32_sse41(const float* a, const float* b, size_t n) {
    size_t i = 0;
    __m128 vsum = _mm_setzero_ps(); /* we will accumulate in lane0 */
    for (; i + 4 <= n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 dp = _mm_dp_ps(va, vb, 0xF1); /* all lanes multiplied; sum -> lane0 */
        vsum = _mm_add_ps(vsum, dp);
    }
    float sum = _mm_cvtss_f32(vsum);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}

static inline float simd_mean_f32_sse41(const float* x, size_t n) {
    return n ? simd_sum_f32_sse41(x, n) / (float)n : 0.0f;
}

static inline float simd_stdev_f32_sse41(const float* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mean = simd_mean_f32_sse41(x, n);

    __m128 vmean = _mm_set1_ps(mean);
    __m128 vsum  = _mm_setzero_ps();
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        __m128 v  = _mm_loadu_ps(&x[i]);
        __m128 d  = _mm_sub_ps(v, vmean);
        vsum = _mm_add_ps(vsum, _mm_mul_ps(d, d));
    }
    float ss = csalt_sse41_hsum128_ps(vsum);
    for (; i < n; ++i) { float d = x[i] - mean; ss += d * d; }

    return sqrtf(ss / (float)n); /* population stdev */
}

#endif /* CSALT_SIMD_SSE41_FLOAT_INL */

