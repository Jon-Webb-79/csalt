/* simd_sse2_int32.inl
   SSE2 helpers for int32 ops + float mean/stdev.
   Requires: <emmintrin.h>, <xmmintrin.h>, <math.h>.
   Note: SSE2 lacks min/max for epi32; implement via compare/select.
*/

#ifndef CSALT_SIMD_SSE2_INT32_INL
#define CSALT_SIMD_SSE2_INT32_INL

#include <emmintrin.h>
#include <xmmintrin.h>
#include <stdint.h>
#include <math.h>

static inline int64_t csalt_hsum128_epi64(__m128i v) {
    __m128i sh = _mm_unpackhi_epi64(v, v);
    __m128i s  = _mm_add_epi64(v, sh);
    return (int64_t)_mm_cvtsi128_si64(s);
}

static inline int64_t simd_sum_i32_sse(const int32_t* x, size_t n) {
    size_t i = 0; __m128i acc0 = _mm_setzero_si128(), acc1 = _mm_setzero_si128();
    for (; i + 4 <= n; i += 4) {
        __m128i v = _mm_loadu_si128((const __m128i*)&x[i]);
        __m128i lo64 = _mm_cvtepi32_epi64(v); /* x0,x1 */
        __m128i hi32 = _mm_shuffle_epi32(v, _MM_SHUFFLE(3,2,3,2));
        __m128i hi64 = _mm_cvtepi32_epi64(hi32); /* x2,x3 */
        acc0 = _mm_add_epi64(acc0, lo64);
        acc1 = _mm_add_epi64(acc1, hi64);
    }
    int64_t sum = csalt_hsum128_epi64(_mm_add_epi64(acc0, acc1));
    for (; i < n; ++i) sum += x[i];
    return sum;
}

/* min/max via cmp/select */
static inline int32_t simd_min_i32_sse(const int32_t* x, size_t n) {
    size_t i = 0; __m128i vmin = _mm_set1_epi32(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m128i v = _mm_loadu_si128((const __m128i*)&x[i]);
        __m128i m = _mm_cmpgt_epi32(vmin, v); /* if vmin>v -> pick v else vmin */
        vmin = _mm_or_si128(_mm_and_si128(m, v), _mm_andnot_si128(m, vmin));
    }
    __m128i t = _mm_min_epi32 ? vmin : vmin; /* (no intrinsic; keep vmin) */
    /* reduce vmin manually */
    int32_t out = x[0];
    __m128i tmp = vmin;
    int32_t buf[4]; _mm_storeu_si128((__m128i*)buf, tmp);
    for (int k=0;k<4;k++) if (buf[k] < out) out = buf[k];
    for (; i < n; ++i) if (x[i] < out) out = x[i];
    return out;
}
static inline int32_t simd_max_i32_sse(const int32_t* x, size_t n) {
    size_t i = 0; __m128i vmax = _mm_set1_epi32(x[0]);
    for (; i + 4 <= n; i += 4) {
        __m128i v = _mm_loadu_si128((const __m128i*)&x[i]);
        __m128i m = _mm_cmpgt_epi32(v, vmax); /* if v>vmax -> pick v else vmax */
        vmax = _mm_or_si128(_mm_and_si128(m, v), _mm_andnot_si128(m, vmax));
    }
    int32_t out = x[0];
    int32_t buf[4]; _mm_storeu_si128((__m128i*)buf, vmax);
    for (int k=0;k<4;k++) if (buf[k] > out) out = buf[k];
    for (; i < n; ++i) if (x[i] > out) out = x[i];
    return out;
}

/* dot: even/odd trick with _mm_mul_epi32 */
static inline int64_t simd_dot_i32_sse(const int32_t* a, const int32_t* b, size_t n) {
    size_t i = 0; __m128i acc = _mm_setzero_si128();
    for (; i + 4 <= n; i += 4) {
        __m128i va = _mm_loadu_si128((const __m128i*)&a[i]);
        __m128i vb = _mm_loadu_si128((const __m128i*)&b[i]);
        __m128i even = _mm_mul_epi32(va, vb);
        __m128i va_odd = _mm_srli_si128(va, 4);
        __m128i vb_odd = _mm_srli_si128(vb, 4);
        __m128i odd  = _mm_mul_epi32(va_odd, vb_odd);
        acc = _mm_add_epi64(acc, even);
        acc = _mm_add_epi64(acc, odd);
    }
    int64_t sum = csalt_hsum128_epi64(acc);
    for (; i < n; ++i) sum += (int64_t)a[i] * (int64_t)b[i];
    return sum;
}

static inline float simd_mean_i32_sse(const int32_t* x, size_t n) {
    if (!n) return 0.0f;
    int64_t s = simd_sum_i32_sse(x, n);
    return (float)((double)s / (double)n);
}
static inline float simd_stdev_i32_sse(const int32_t* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mu = simd_mean_i32_sse(x, n);
    __m128 vmu = _mm_set1_ps(mu), acc = _mm_setzero_ps();
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        __m128  vf = _mm_cvtepi32_ps(_mm_loadu_si128((const __m128i*)&x[i]));
        __m128  d  = _mm_sub_ps(vf, vmu);
        acc = _mm_add_ps(acc, _mm_mul_ps(d, d));
    }
    __m128 t = _mm_hadd_ps(acc, acc); t = _mm_hadd_ps(t, t);
    float ss = _mm_cvtss_f32(t);
    for (; i < n; ++i) { float d = (float)x[i] - mu; ss += d * d; }
    return sqrtf(ss / (float)n);
}

#endif /* CSALT_SIMD_SSE2_INT32_INL */

