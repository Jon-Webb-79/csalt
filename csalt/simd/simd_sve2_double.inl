/* simd_sve2_double.inl
   SVE2 helpers for double operations (same FP ops as SVE).
   Compile with -march=armv9-a+sve2 (or equivalent). Requires: <arm_sve.h>
*/
#ifndef CSALT_SIMD_SVE2_DOUBLE_INL
#define CSALT_SIMD_SVE2_DOUBLE_INL

#include <arm_sve.h>

static inline double simd_sum_f64_sve2(const double* x, size_t n) {
    size_t i = 0; double sum = 0.0;
    while (i < n) {
        svbool_t pg = svwhilelt_b64((uint64_t)i, (uint64_t)n);
        svfloat64_t vx = svld1_f64(pg, &x[i]);
        sum += svaddv_f64(pg, vx);
        i += svcntd();
    }
    return sum;
}
static inline double simd_min_f64_sve2(const double* x, size_t n) {
    size_t i = 0; double m = x[0];
    while (i < n) {
        svbool_t pg = svwhilelt_b64((uint64_t)i, (uint64_t)n);
        svfloat64_t vx = svld1_f64(pg, &x[i]);
        double chunk = svminv_f64(pg, vx);
        if (chunk < m) m = chunk;
        i += svcntd();
    }
    return m;
}
static inline double simd_max_f64_sve2(const double* x, size_t n) {
    size_t i = 0; double m = x[0];
    while (i < n) {
        svbool_t pg = svwhilelt_b64((uint64_t)i, (uint64_t)n);
        svfloat64_t vx = svld1_f64(pg, &x[i]);
        double chunk = svmaxv_f64(pg, vx);
        if (chunk > m) m = chunk;
        i += svcntd();
    }
    return m;
}
static inline double simd_dot_f64_sve2(const double* a, const double* b, size_t n) {
    size_t i = 0; double sum = 0.0;
    while (i < n) {
        svbool_t pg = svwhilelt_b64((uint64_t)i, (uint64_t)n);
        svfloat64_t va = svld1_f64(pg, &a[i]);
        svfloat64_t vb = svld1_f64(pg, &b[i]);
        svfloat64_t prod = svmul_f64_x(pg, va, vb);
        sum += svaddv_f64(pg, prod);
        i += svcntd();
    }
    return sum;
}
static inline double simd_mean_f64_sve2(const double* x, size_t n) {
    return n ? simd_sum_f64_sve2(x, n) / (double)n : 0.0;
}
static inline double simd_stdev_f64_sve2(const double* x, size_t n) {
    if (n < 2) return 0.0;
    const double mean = simd_mean_f64_sve2(x, n);
    size_t i = 0; double ss = 0.0;
    svfloat64_t vmean = svdup_f64(mean);
    while (i < n) {
        svbool_t pg = svwhilelt_b64((uint64_t)i, (uint64_t)n);
        svfloat64_t v  = svld1_f64(pg, &x[i]);
        svfloat64_t d  = svsub_f64_x(pg, v, vmean);
        svfloat64_t sq = svmul_f64_x(pg, d, d);
        ss += svaddv_f64(pg, sq);
        i += svcntd();
    }
    return sqrt(ss / (double)n);
}

#endif /* CSALT_SIMD_SVE2_DOUBLE_INL */

