/* simd_sve_int32.inl
   SVE helpers for int32 ops + float mean/stdev.
   Compile with -march=armv8.2-a+sve (or armv9 for SVE2). Requires: <arm_sve.h>.
*/

#ifndef CSALT_SIMD_SVE_INT32_INL
#define CSALT_SIMD_SVE_INT32_INL

#include <arm_sve.h>
#include <stdint.h>
#include <math.h>

static inline int64_t simd_sum_i32_sve(const int32_t* x, size_t n) {
    size_t i = 0; int64_t sum = 0;
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svint32_t v = svld1_s32(pg, &x[i]);
        /* widen to i64 and reduce to scalar */
        svint64_t w = svcvt_s64_s32_x(pg, v);
        sum += svaddv_s64(pg, w);
        i += svcntw();
    }
    return sum;
}

static inline int32_t simd_min_i32_sve(const int32_t* x, size_t n) {
    size_t i = 0; int32_t m = x[0];
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svint32_t v = svld1_s32(pg, &x[i]);
        int32_t chunk = svminv_s32(pg, v);
        if (chunk < m) m = chunk;
        i += svcntw();
    }
    return m;
}
static inline int32_t simd_max_i32_sve(const int32_t* x, size_t n) {
    size_t i = 0; int32_t m = x[0];
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svint32_t v = svld1_s32(pg, &x[i]);
        int32_t chunk = svmaxv_s32(pg, v);
        if (chunk > m) m = chunk;
        i += svcntw();
    }
    return m;
}

static inline int64_t simd_dot_i32_sve(const int32_t* a, const int32_t* b, size_t n) {
    size_t i = 0; int64_t sum = 0;
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svint32_t va = svld1_s32(pg, &a[i]);
        svint32_t vb = svld1_s32(pg, &b[i]);
        /* widen to i64 then multiply as i64 to get exact 32x32->64 */
        svint64_t wa = svcvt_s64_s32_x(pg, va);
        svint64_t wb = svcvt_s64_s32_x(pg, vb);
        svint64_t prod = svmul_s64_x(pg, wa, wb);
        sum += svaddv_s64(pg, prod);
        i += svcntw();
    }
    return sum;
}

static inline float simd_mean_i32_sve(const int32_t* x, size_t n) {
    if (!n) return 0.0f;
    int64_t s = simd_sum_i32_sve(x, n);
    return (float)((double)s / (double)n);
}
static inline float simd_stdev_i32_sve(const int32_t* x, size_t n) {
    if (n < 2) return 0.0f;
    const float mu = simd_mean_i32_sve(x, n);
    size_t i = 0; float ss = 0.0f;
    svfloat32_t vmu = svdup_f32(mu);
    while (i < n) {
        svbool_t pg = svwhilelt_b32((uint64_t)i, (uint64_t)n);
        svint32_t vi = svld1_s32(pg, &x[i]);
        svfloat32_t vf = svcvt_f32_s32_x(pg, vi);
        svfloat32_t d  = svsub_f32_x(pg, vf, vmu);
        svfloat32_t sq = svmul_f32_x(pg, d, d);
        ss += svaddv_f32(pg, sq);
        i += svcntw();
    }
    return sqrtf(ss / (float)n);
}

#endif /* CSALT_SIMD_SVE_INT32_INL */

