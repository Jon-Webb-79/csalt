/* simd_avx2_int32.inl
   AVX2 helpers for int32 operations (sum/min/max/dot) + float mean/stdev.
   Compile with -mavx2 (optionally -mfma). Requires: <immintrin.h>, <math.h>.
   Returns:
     - sum:   int64_t
     - min:   int32_t
     - max:   int32_t
     - dot:   int64_t
     - mean:  float
     - stdev: float  (population; divide by (n-1) externally for sample)
*/

#ifndef CSALT_SIMD_AVX2_INT32_INL
#define CSALT_SIMD_AVX2_INT32_INL

#include <immintrin.h>
#include <stdint.h>
#include <math.h>

/* ---- reduce 4x int64 lanes in a 256-bit reg to scalar ---- */
static inline int64_t csalt_hsum256_epi64(__m256i v) {
    __m128i lo = _mm256_castsi256_si128(v);
    __m128i hi = _mm256_extracti128_si256(v, 1);
    __m128i s  = _mm_add_epi64(lo, hi);              /* [s0 s1] */
    __m128i sh = _mm_unpackhi_epi64(s, s);           /* [s1 s1] */
    __m128i r  = _mm_add_epi64(s, sh);               /* [s0+s1, ...] */
    return (int64_t)_mm_cvtsi128_si64(r);
}

/* ---- SUM (widen to i64 to avoid overflow) ---- */
static inline int64_t simd_sum_i32_avx2(const int32_t* x, size_t n) {
    size_t i = 0;
    __m256i acc_lo = _mm256_setzero_si256();  /* 4x i64 */
    __m256i acc_hi = _mm256_setzero_si256();  /* 4x i64 */
    for (; i + 8 <= n; i += 8) {
        __m256i v   = _mm256_loadu_si256((const __m256i*)&x[i]);
        __m128i vlo = _mm256_castsi256_si128(v);
        __m128i vhi = _mm256_extracti128_si256(v, 1);
        acc_lo = _mm256_add_epi64(acc_lo, _mm256_cvtepi32_epi64(vlo));
        acc_hi = _mm256_add_epi64(acc_hi, _mm256_cvtepi32_epi64(vhi));
    }
    int64_t sum = csalt_hsum256_epi64(_mm256_add_epi64(acc_lo, acc_hi));
    for (; i < n; ++i) sum += x[i];
    return sum;
}

/* ---- MIN / MAX (epi32) ---- */
static inline int32_t simd_min_i32_avx2(const int32_t* x, size_t n) {
    size_t i = 0;
    __m256i vmin = _mm256_set1_epi32(x[0]);
    for (; i + 8 <= n; i += 8) {
        __m256i v = _mm256_loadu_si256((const __m256i*)&x[i]);
        vmin = _mm256_min_epi32(vmin, v);
    }
    __m128i lo = _mm256_castsi256_si128(vmin);
    __m128i hi = _mm256_extracti128_si256(vmin, 1);
    __m128i m  = _mm_min_epi32(lo, hi);
    m = _mm_min_epi32(m, _mm_shuffle_epi32(m, _MM_SHUFFLE(2,3,0,1)));
    m = _mm_min_epi32(m, _mm_shuffle_epi32(m, _MM_SHUFFLE(1,0,3,2)));
    int32_t out = _mm_cvtsi128_si32(m);
    for (; i < n; ++i) if (x[i] < out) out = x[i];
    return out;
}
static inline int32_t simd_max_i32_avx2(const int32_t* x, size_t n) {
    size_t i = 0;
    __m256i vmax = _mm256_set1_epi32(x[0]);
    for (; i + 8 <= n; i += 8) {
        __m256i v = _mm256_loadu_si256((const __m256i*)&x[i]);
        vmax = _mm256_max_epi32(vmax, v);
    }
    __m128i lo = _mm256_castsi256_si128(vmax);
    __m128i hi = _mm256_extracti128_si256(vmax, 1);
    __m128i m  = _mm_max_epi32(lo, hi);
    m = _mm_max_epi32(m, _mm_shuffle_epi32(m, _MM_SHUFFLE(2,3,0,1)));
    m = _mm_max_epi32(m, _mm_shuffle_epi32(m, _MM_SHUFFLE(1,0,3,2)));
    int32_t out = _mm_cvtsi128_si32(m);
    for (; i < n; ++i) if (x[i] > out) out = x[i];
    return out;
}

/* ---- DOT (exact 64-bit accumulate) ---- */
static inline int64_t simd_dot_i32_avx2(const int32_t* a, const int32_t* b, size_t n) {
    size_t i = 0;
    __m256i acc = _mm256_setzero_si256();   /* 4x i64 */
    for (; i + 8 <= n; i += 8) {
        __m256i va = _mm256_loadu_si256((const __m256i*)&a[i]);
        __m256i vb = _mm256_loadu_si256((const __m256i*)&b[i]);
        /* even lanes (0,2,4,6) */
        __m256i prod_even = _mm256_mul_epi32(va, vb);  /* 32x32 -> 64 (even lanes) */
        /* odd lanes: shift to make them even-aligned */
        __m256i va_odd = _mm256_srli_si256(va, 4);
        __m256i vb_odd = _mm256_srli_si256(vb, 4);
        __m256i prod_odd  = _mm256_mul_epi32(va_odd, vb_odd);
        acc = _mm256_add_epi64(acc, prod_even);
        acc = _mm256_add_epi64(acc, prod_odd);
    }
    int64_t sum = csalt_hsum256_epi64(acc);
    for (; i < n; ++i) sum += (int64_t)a[i] * (int64_t)b[i];
    return sum;
}

/* ---- MEAN (float) ---- */
static inline float simd_mean_i32_avx2(const int32_t* x, size_t n) {
    if (!n) return 0.0f;
    int64_t s = simd_sum_i32_avx2(x, n);
    return (float)((double)s / (double)n);
}

/* ---- STDEV (float; two-pass, convert to f32) ---- */
static inline float simd_stdev_i32_avx2(const int32_t* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mu = simd_mean_i32_avx2(x, n);

    size_t i = 0;
    __m256  vmu  = _mm256_set1_ps(mu);
    __m256  accf = _mm256_setzero_ps();
    for (; i + 8 <= n; i += 8) {
        __m256i vi   = _mm256_loadu_si256((const __m256i*)&x[i]);
        __m256  vf   = _mm256_cvtepi32_ps(vi);
        __m256  d    = _mm256_sub_ps(vf, vmu);
        #if defined(__FMA__)
            accf = _mm256_fmadd_ps(d, d, accf);
        #else
            accf = _mm256_add_ps(accf, _mm256_mul_ps(d, d));
        #endif
    }
    /* horizontal sum of 8 floats */
    __m128 low  = _mm256_castps256_ps128(accf);
    __m128 high = _mm256_extractf128_ps(accf, 1);
    __m128 sum4 = _mm_add_ps(low, high);
    sum4 = _mm_hadd_ps(sum4, sum4);
    sum4 = _mm_hadd_ps(sum4, sum4);
    float ss = _mm_cvtss_f32(sum4);
    for (; i < n; ++i) { float d = (float)x[i] - mu; ss += d * d; }

    return sqrtf(ss / (float)n); /* population */
}

#endif /* CSALT_SIMD_AVX2_INT32_INL */

