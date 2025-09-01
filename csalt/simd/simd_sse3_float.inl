/* simd_sse3_float.inl
   SSE3 helpers for float operations (uses _mm_hadd_ps for horizontal reductions).
   Compile with -msse3 (GCC/Clang) or ensure SSE3 is enabled on MSVC.
   Requires: <xmmintrin.h>, <emmintrin.h>, <pmmintrin.h>
   Contract:
     - Caller validates pointers; for min/max, n > 0.
*/

#ifndef CSALT_SIMD_SSE3_FLOAT_INL
#define CSALT_SIMD_SSE3_FLOAT_INL

#include <xmmintrin.h>  /* SSE  */
#include <emmintrin.h>  /* SSE2 */
#include <pmmintrin.h>  /* SSE3 */

/* ===== Horizontal reductions (SSE3) ===== */
static inline float csalt_sse3_hsum128_ps(__m128 v) {
    __m128 t = _mm_hadd_ps(v, v);  /* [a0+a1, a2+a3, a0+a1, a2+a3] */
    t = _mm_hadd_ps(t, t);         /* [sum, sum, sum, sum] */
    return _mm_cvtss_f32(t);
}

static inline float csalt_sse3_hmin128_ps(__m128 v) {
    __m128 sh = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,3,0,1));
    __m128 m  = _mm_min_ps(v, sh);
    sh = _mm_shuffle_ps(m, m, _MM_SHUFFLE(1,0,3,2));
    m  = _mm_min_ps(m, sh);
    return _mm_cvtss_f32(m);
}

static inline float csalt_sse3_hmax128_ps(__m128 v) {
    __m128 sh = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,3,0,1));
    __m128 m  = _mm_max_ps(v, sh);
    sh = _mm_shuffle_ps(m, m, _MM_SHUFFLE(1,0,3,2));
    m  = _mm_max_ps(m, sh);
    return _mm_cvtss_f32(m);
}

/* ===== Public SSE3 helpers (suffixed) ===== */

static inline float simd_sum_f32_sse3(const float* x, size_t n) {
    size_t i = 0;
    __m128 vsum = _mm_setzero_ps();
    for (; i + 4 <= n; i += 4) {
        __m128 v = _mm_loadu_ps(&x[i]);
        vsum = _mm_add_ps(vsum, v);
    }
    float sum = csalt_sse3_hsum128_ps(vsum);
    for (; i < n; ++i) sum += x[i];
    return sum;
}

static inline float simd_min_f32_sse3(const float* x, size_t n) {
    size_t i = 0;
    __m128 vmin = _mm_set1_ps(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m128 v = _mm_loadu_ps(&x[i]);
        vmin = _mm_min_ps(vmin, v);
    }
    float m = csalt_sse3_hmin128_ps(vmin);
    for (; i < n; ++i) if (x[i] < m) m = x[i];
    return m;
}

static inline float simd_max_f32_sse3(const float* x, size_t n) {
    size_t i = 0;
    __m128 vmax = _mm_set1_ps(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m128 v = _mm_loadu_ps(&x[i]);
        vmax = _mm_max_ps(vmax, v);
    }
    float m = csalt_sse3_hmax128_ps(vmax);
    for (; i < n; ++i) if (x[i] > m) m = x[i];
    return m;
}

static inline float simd_dot_f32_sse3(const float* a, const float* b, size_t n) {
    size_t i = 0;
    __m128 vacc = _mm_setzero_ps();
    for (; i + 4 <= n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        vacc = _mm_add_ps(vacc, _mm_mul_ps(va, vb));
    }
    float sum = csalt_sse3_hsum128_ps(vacc);
    for (; i < n; ++i) sum += a[i] * b[i];
    return sum;
}

static inline float simd_mean_f32_sse3(const float* x, size_t n) {
    return n ? simd_sum_f32_sse3(x, n) / (float)n : 0.0f;
}

static inline float simd_stdev_f32_sse3(const float* x, size_t n) {
   if (n < 2) return 0.0f;
   const float mean = simd_mean_f32_sse3(x, n);
   __m128 vmean = _mm_set1_ps(mean);
   __m128 vsum  = _mm_setzero_ps();

   size_t i = 0;
   for (; i + 4 <= n; i += 4) {
       __m128 v  = _mm_loadu_ps(&x[i]);
       __m128 d  = _mm_sub_ps(v, vmean);
       vsum = _mm_add_ps(vsum, _mm_mul_ps(d, d));
   }
   float ss = csalt_sse3_hsum128_ps(vsum);
   for (; i < n; ++i) { float d = x[i] - mean; ss += d * d; }

   return sqrtf(ss / (float)n); /* population stdev */
}

#endif /* CSALT_SIMD_SSE3_FLOAT_INL */

