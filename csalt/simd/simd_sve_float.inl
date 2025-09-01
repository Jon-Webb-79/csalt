/* simd_sve_float.inl
   ARM SVE helpers for float operations (vector-length agnostic).
   Compile with -march=<armv8.x-a>+sve and include <arm_sve.h>.
*/
#ifndef CSALT_SIMD_SVE_FLOAT_INL
#define CSALT_SIMD_SVE_FLOAT_INL

#include <arm_sve.h>

/* ----- Public helpers (SVE) ----- */
static inline float simd_sum_f32_sve(const float* x, size_t n) {
    size_t i = 0;
    float sum = 0.0f;
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svfloat32_t vx = svld1(pg, &x[i]);
        sum += svaddv_f32(pg, vx);                 /* reduce active lanes to scalar */
        i += svcntw();
    }
    return sum;
}

static inline float simd_min_f32_sve(const float* x, size_t n) {
    size_t i = 0;
    float m = x[0];
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svfloat32_t vx = svld1(pg, &x[i]);
        float chunk = svminv_f32(pg, vx);
        if (chunk < m) m = chunk;
        i += svcntw();
    }
    return m;
}

static inline float simd_max_f32_sve(const float* x, size_t n) {
    size_t i = 0;
    float m = x[0];
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svfloat32_t vx = svld1(pg, &x[i]);
        float chunk = svmaxv_f32(pg, vx);
        if (chunk > m) m = chunk;
        i += svcntw();
    }
    return m;
}

static inline float simd_dot_f32_sve(const float* a, const float* b, size_t n) {
    size_t i = 0;
    float sum = 0.0f;
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svfloat32_t va = svld1(pg, &a[i]);
        svfloat32_t vb = svld1(pg, &b[i]);
        svfloat32_t prod = svmul_f32_x(pg, va, vb);
        sum += svaddv_f32(pg, prod);
        i += svcntw();
    }
    return sum;
}

static inline float simd_mean_f32_sve(const float* x, size_t n) {
    return n ? simd_sum_f32_sve(x, n) / (float)n : 0.0f;
}

static inline float simd_stdev_f32_sve(const float* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mean = simd_mean_f32_sve(x, n);

    size_t i = 0;
    float ss = 0.0f;
    svfloat32_t vmean = svdup_f32(mean);
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svfloat32_t v  = svld1(pg, &x[i]);
        svfloat32_t d  = svsub_f32_x(pg, v, vmean);
        svfloat32_t sq = svmul_f32_x(pg, d, d);
        ss += svaddv_f32(pg, sq);
        i += svcntw();
    }
    return sqrtf(ss / (float)n);        /* population stdev */
}

#endif /* CSALT_SIMD_SVE_FLOAT_INL */

