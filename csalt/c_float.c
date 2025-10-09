// ================================================================================
// ================================================================================
// - File:    c_float.c
// - Purpose: This file contains the implementation for all functions used in 
//            the c_float library
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    January 12, 2025
// - Version: 0.1
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <asm-generic/errno-base.h>
#include <immintrin.h>  // AVX/SSE
#include "c_float.h"
#include <errno.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#if defined(__AVX512F__)
    #include "simd_avx512_float.inl"
#elif defined(__AVX2__)
    #include <immintrin.h>
    #include "simd_avx2_float.inl"   /* simd_*_f32_avx2 */
#elif defined(__AVX__)
    #include <immintrin.h> 
    #include "simd_avx_float.inl"
#elif defined(__SSE4_1__)
    #include <smmintrin.h>
    #include "simd_sse41_float.inl"
#elif defined(__SSE3__)
    #include <pmmintrin.h>
    #include "simd_sse3_float.inl"
#elif defined(__SSE2__)
    #include <xmmintrin.h>
    #include <emmintrin.h>
    #include "simd_sse2_float.inl"   /* simd_*_f32_sse  */
#elif defined(__ARM_FEATURE_SVE2)
    #include "simd_sve2_float.inl"
#elif defined(__ARM_FEATURE_SVE)
    #include "simd_sve_float.inl"
#elif defined(__ARM_NEON)
    #include "simd_neon_float.inl"
#endif

static const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
static const size_t hashSize = 16;  //  Size fo hash map init functions
static const uint32_t HASH_SEED = 0x45d9f3b; 

// static const float SPARSE_THRESHOLD = 0.15;  // Convert to sparse if < 15% filled
// static const float DENSE_THRESHOLD = 0.30;   // Convert to dense if > 30% filled
// static const size_t MIN_SPARSE_SIZE = 1000;  // Below this many elements, just use dense
// static const size_t INITIAL_COO_CAPACITY = 16;
//
// static const size_t MAX_DENSE_ELEMENTS = 100000;
// static const size_t COO_TO_CSR_TRIGGER = 10000;
//
// static const float CSR_COMPACT_THRESHOLD = 0.25f;
// static const size_t CSR_TOMBSTONE_COL = SIZE_MAX;
// ================================================================================
// ================================================================================ 
// SIMD INSTRUCTION SETS

static inline float simd_sum_f32(const float* x, size_t n) {
    #if defined(__AVX512F__)
        return simd_sum_f32_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_sum_f32_avx2(x, n);
    #elif defined(__AVX__)
        return simd_sum_f32_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_sum_f32_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_sum_f32_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_sum_f32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_sum_f32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_sum_f32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return simd_sum_f32_neon(x, n);
    #else
        float s = 0.f; for (size_t i=0;i<n;++i) s += x[i]; return s;
    #endif
}
// -------------------------------------------------------------------------------- 

static inline float simd_min_f32(const float* x, size_t n) {
    #if defined(__AVX512F__)
        return simd_min_f32_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_min_f32_avx2(x, n);
    #elif defined(__AVX__)
        return simd_min_f32_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_min_f32_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_min_f32_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_min_f32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_min_f32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_min_f32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return simd_min_f32_neon(x, n);
    #else
        float m = x[0]; for (size_t i=1;i<n;++i) if (x[i]<m) m=x[i]; return m;
    #endif
}
// -------------------------------------------------------------------------------- 

static inline float simd_max_f32(const float* x, size_t n) {
    #if defined(__AVX512F__)
        return simd_max_f32_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_max_f32_avx2(x, n);
    #elif defined(__AVX__)
        return simd_max_f32_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_max_f32_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_max_f32_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_max_f32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_max_f32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_max_f32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return simd_max_f32_neon(x, n);
    #else
        float m = x[0]; for (size_t i=1;i<n;++i) if (x[i]>m) m=x[i]; return m;
    #endif
}
// -------------------------------------------------------------------------------- 

static inline float simd_dot_f32(const float* a, const float* b, size_t n) {
    #if defined(__AVX512F__)
        return simd_dot_f32_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_dot_f32_avx2(a, b, n);
    #elif defined(__AVX__)
        return simd_dot_f32_avx(a, b, n);
    #elif defined(__SSE4_1__)
        return simd_dot_f32_sse41(a, b, n);
    #elif defined(__SSE3__)
        return simd_dot_f32_sse3(a, b, n);
    #elif defined(__SSE2__)
        return simd_dot_f32_sse(a, b, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_dot_f32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_dot_f32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return simd_dot_f32_neon(x, n);
    #else
        float s = 0.f;
        for (size_t i = 0; i < n; ++i) s += a[i] * b[i];
        return s;
    #endif
}
// -------------------------------------------------------------------------------- 

static inline float simd_mean_f32(const float* x, size_t n) {
    if (n == 0) return 0.0f;
    #if defined(__AVX512F__)
        return simd_mean_f32_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_mean_f32_avx2(x, n);
    #elif defined(__AVX__)
        return simd_mean_f32_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_mean_f32_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_mean_f32_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_mean_f32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_mean_f32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_mean_f32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return simd_mean_f32_neon(x, n);
    #else
        float s = 0.f;
        for (size_t i = 0; i < n; ++i) s += x[i];
        return s / (float)n;
    #endif
}
// -------------------------------------------------------------------------------- 

static inline float simd_stdev_f32(const float* x, size_t n) {
    if (n < 2) return 0.0f;
    #if defined(__AVX512F__)
        return simd_stdev_f32_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_stdev_f32_avx2(x, n);
    #elif defined(__AVX__)
        return simd_stdev_f32_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_stdev_f32_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_stdev_f32_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_stdev_f32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_stdev_f32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_stdev_f32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return simd_stdev_f32_neon(x, n);
    #else
        float mean = 0.f;
        for (size_t i = 0; i < n; ++i) mean += x[i];
        mean /= (float)n;
        float ss = 0.f;
        for (size_t i = 0; i < n; ++i) {
            float d = x[i] - mean;
            ss += d * d;
        }
        return sqrtf(ss / (float)n);  // population stdev
    #endif
}

// ================================================================================ 
// ================================================================================ 

float_v* init_float_vector(size_t buff) {
    if (buff == 0) {
        errno = EINVAL;
        return NULL;
    }
    float_v* struct_ptr = malloc(sizeof(float_v));
    if (struct_ptr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
   
    float* data_ptr = calloc(buff,  sizeof(float));
    if (data_ptr == NULL) {
        free(struct_ptr);
        errno = ENOMEM;
        return NULL; 
    }
   
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    struct_ptr->alloc_type = DYNAMIC;
    struct_ptr->error = NO_ERROR;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

void free_float_vector(float_v* vec) {
   if (!vec || !vec->alloc_type || vec->alloc_type == STATIC) {
       errno = EINVAL;
       return;
   }
   if (vec->data) free(vec->data);
   free(vec);
}
// --------------------------------------------------------------------------------

void _free_float_vector(float_v** vec) {
    if (vec && *vec) {
        free_float_vector(*vec);
        *vec = NULL;
    }
}
// -------------------------------------------------------------------------------- 

bool push_back_float_vector(float_v* vec, const float value) {
    if (vec == NULL|| vec->data == NULL) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            vec->error = INVALID_ARG;
            errno = set_errno_from_error(vec->error);;
            return false;
        }
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        // Allocate more space for the array of str structs
        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            vec->error = REALLOC_FAIL;
            errno = set_errno_from_error(vec->error);;
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(float));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    vec->data[vec->len] = value; 
    vec->len++;
    vec->error = NO_ERROR; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_float_vector(float_v* vec, float value) {
    if (vec == NULL || vec->data == NULL) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            vec->error = INVALID_ARG;
            errno = set_errno_from_error(vec->error);
            return false;
        }
        
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
        
        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            vec->error = REALLOC_FAIL;
            errno = set_errno_from_error(vec->error);
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(float));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    
    // Move existing elements right if there are any
    if (vec->len > 0) {
        memmove(vec->data + 1, vec->data, vec->len * sizeof(float));
    }
    
    vec->data[0] = value;    
    vec->len++;
    vec->error = NO_ERROR;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_float_vector(float_v* vec, float value, size_t index) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        vec->error = OUT_OF_BOUNDS;
        errno = set_errno_from_error(vec->error);
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            vec->error = INVALID_ARG;
            errno = set_errno_from_error(vec->error);
            return false;
        }
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
        
       
        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            vec->error = REALLOC_FAIL;
            errno = set_errno_from_error(vec->error);
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(float));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    
    // Move existing elements right
    if (index < vec->len) {  // Only move if not appending
        // Check for size_t overflow in move operation
        if (vec->len - index > SIZE_MAX - 1) {
            vec->error = SIZE_MISMATCH;
            errno = set_errno_from_error(vec->error);
            return false;
        }
        memmove(vec->data + index + 1, vec->data + index, 
                (vec->len - index) * sizeof(float));
    }
    
    vec->data[index] = value;
    vec->len++;
    vec->error = NO_ERROR;
    return true;
}
// -------------------------------------------------------------------------------- 


float pop_back_float_vector(float_v* vec) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return FLT_MAX;
    }
    
    if (vec->len == 0) {
        vec->error = INVALID_ARG;
        errno = set_errno_from_error(vec->error);
        return FLT_MAX;
    }
    
    // Create copy of last element
    float temp = vec->data[vec->len - 1];
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(float));
    vec->len--;
    vec->error = NO_ERROR;
    return temp;
}
// --------------------------------------------------------------------------------

float pop_front_float_vector(float_v* vec) {  // Fixed function name
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return FLT_MAX;
    }
   
    if (vec->len == 0) {
        vec->error = INVALID_ARG;
        errno = set_errno_from_error(vec->error);
        return FLT_MAX;
    }
   
    // Create copy of first element
    float temp = vec->data[0];
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(float));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(float));
   
    vec->len--;
    vec->error = NO_ERROR;
    return temp;
}
// --------------------------------------------------------------------------------

float pop_any_float_vector(float_v* vec, size_t index) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return FLT_MAX;
    }
   
    if (vec->len == 0) {
        vec->error = INVALID_ARG;
        errno = set_errno_from_error(vec->error);
        return FLT_MAX;
    }
    
    if (index >= vec->len) {
        vec->error = OUT_OF_BOUNDS;
        errno = set_errno_from_error(vec->error);
        return FLT_MAX;
    }
    
    // Create copy of element to pop
    float temp = vec->data[index];
    
    // If not the last element, shift remaining elements left
    if (index < vec->len - 1) {
        // Check for overflow in memmove size calculation
        if ((vec->len - index - 1) > SIZE_MAX / sizeof(float)) {
            vec->error = SIZE_MISMATCH;
            errno = set_errno_from_error(vec->error);
            return FLT_MAX;
        }
        
        memmove(&vec->data[index], &vec->data[index + 1], 
                (vec->len - index - 1) * sizeof(float));
    }
   
    // Clear the last element
    memset(&vec->data[vec->len - 1], 0, sizeof(float));
    
    vec->len--;
    vec->error = NO_ERROR;
    return temp;
}
// --------------------------------------------------------------------------------

void reverse_float_vector(float_v* vec) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return;
    }

    if (vec->len == 0) {
        vec->error = INVALID_ARG;
        errno = set_errno_from_error(vec->error);
        return;
    }

    size_t i = 0;
    size_t j = vec->len - 1;
    while (i < j) {
       swap_float(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// ================================================================================
// ================================================================================ 
// QUICKSORT

static float* _median_of_three(float* a, float* b, float* c, iter_dir direction) {
    if ((direction == FORWARD && *a < *b) ||
        (direction == REVERSE && *a > *b)) {
        if ((direction == FORWARD && *b < *c) ||
            (direction == REVERSE && *b > *c))
            return b;
        if ((direction == FORWARD && *a < *c) ||
            (direction == REVERSE && *a > *c))
            return c;
        return a;
    }
    if ((direction == FORWARD && *a < *c) ||
        (direction == REVERSE && *a > *c))
        return a;
    if ((direction == FORWARD && *b < *c) ||
        (direction == REVERSE && *b > *c))
        return c;
    return b;
}
// -------------------------------------------------------------------------------- 

static void _insertion_sort(float* vec, int low, int high, iter_dir direction) {
    for (int i = low + 1; i <= high; i++) {
        float key = vec[i];
        int j = i - 1;
        while (j >= low && ((direction == FORWARD && vec[j] > key) ||
                           (direction == REVERSE && vec[j] < key))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = key;
    }
}
// --------------------------------------------------------------------------------

static int _partition_float(float* vec, int low, int high, iter_dir direction) {
    int mid = low + (high - low) / 2;
    float* pivot_ptr = _median_of_three(&vec[low], &vec[mid], &vec[high], direction);
    
    if (pivot_ptr != &vec[high])
        swap_float(pivot_ptr, &vec[high]);
    
    float pivot = vec[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_float(&vec[i], &vec[j]);
        }
    }
    swap_float(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// -------------------------------------------------------------------------------- 

static void _quicksort_float(float* vec, int low, int high, iter_dir direction) {
    while (low < high) {
        if (high - low < 10) {
            _insertion_sort(vec, low, high, direction);
            break;
        }
        
        int pi = _partition_float(vec, low, high, direction);
        
        if (pi - low < high - pi) {
            _quicksort_float(vec, low, pi - 1, direction);
            low = pi + 1;
        } else {
            _quicksort_float(vec, pi + 1, high, direction);
            high = pi - 1;
        }
    }
}
// -------------------------------------------------------------------------------- 

void sort_float_vector(float_v* vec, iter_dir direction) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return;
    }
    if (vec->len < 2) return;
    
    _quicksort_float(vec->data, 0, vec->len - 1, direction);
}
// -------------------------------------------------------------------------------- 

void trim_float_vector(float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    
    if (vec->alloc_type == STATIC || vec->len == vec->alloc) {
        return;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return;
    }

    // Check for overflow
    if (vec->len > SIZE_MAX / sizeof(float)) {
        errno = ERANGE;
        return;
    }
    
    float* ptr = realloc(vec->data, sizeof(float) * vec->len);
    if (ptr == NULL) {
        errno = ENOMEM;
        return;
    }
    
    vec->data = ptr;
    vec->alloc = vec->len;
}
// --------------------------------------------------------------------------------

bin_dat binary_search_bounds_float_vector(float_v* vec,
                                          float value,
                                          float tolerance,
                                          bool sort_first)
{
    if (!vec || !vec->data) {
        errno = EINVAL;
        return (bin_dat){ .lower = SIZE_MAX, .upper = SIZE_MAX };
    }
    if (vec->len == 0) {
        errno = ENODATA;
        return (bin_dat){ .lower = SIZE_MAX, .upper = SIZE_MAX };
    }
    if (tolerance < 0.0f || isnan(value) || isnan(tolerance)) {
        errno = EINVAL;
        return (bin_dat){ .lower = SIZE_MAX, .upper = SIZE_MAX };
    }

    if (sort_first && vec->len > 1) {
        sort_float_vector(vec, FORWARD);
    }

    size_t left = 0;
    size_t right = vec->len - 1;

    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        float diff = vec->data[mid] - value;

        // Within tolerance => treat as exact hit; bounds collapse to mid
        if (fabsf(diff) <= tolerance) {
            return (bin_dat){ .lower = mid, .upper = mid };
        }

        if (diff < 0.0f) {
            left = mid + 1;
        } else {
            // Avoid unsigned underflow: only decrement when mid > 0
            if (mid == 0) {
                // All remaining elements (index 0) are >= value
                // No element less than value
                right = (size_t)-1; // == SIZE_MAX
                break;
            }
            right = mid - 1;
        }
    }

    // ---------- compute bounds when no exact match ----------
    // After the loop, invariants:
    // right = index of last element < value, or SIZE_MAX if none
    // left  = index of first element > value, or vec->len if none

    // If value is smaller than the first element:
    if (right == SIZE_MAX) {
        return (bin_dat){ .lower = SIZE_MAX, .upper = 0 };
    }

    // If value is larger than the last element:
    if (left >= vec->len) {
        return (bin_dat){ .lower = vec->len - 1, .upper = SIZE_MAX };
    }

    // Value is strictly between elements right and left
    return (bin_dat){ .lower = right, .upper = left };
}
// -------------------------------------------------------------------------------- 

size_t binary_search_float_vector(float_v* vec, float value, float tolerance, bool sort_first) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return LONG_MAX;
    }

    if (tolerance < 0) {
        errno = EINVAL;
        return LONG_MAX;
    }

    if (isnan(value) || isnan(tolerance)) {
        errno = EINVAL;
        return LONG_MAX;
    }
    
    // Sort if requested and vector has more than 1 element
    if (sort_first && vec->len > 1) {
        sort_float_vector(vec, FORWARD);
    }
    
    size_t left = 0;
    size_t right = vec->len - 1;
    
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        float diff = vec->data[mid] - value;
        
        // Check if we found a match within tolerance
        if (fabs(diff) <= tolerance) {
            return mid;
        }
        
        if (diff < 0) {
            left = mid + 1;
        } else {
            // Handle case where mid is 0 to prevent underflow
            if (mid == 0) {
                break;
            }
            right = mid - 1;
        }
    }
    // The value does not exist in the array 
    return LONG_MAX;
}
// -------------------------------------------------------------------------------- 

void update_float_vector(float_v* vec, size_t index, float replacement_value) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return;
    }
    if (index > vec->len -1) {
        errno = ERANGE;
        return;
    }
    vec->data[index] = replacement_value;
}
// -------------------------------------------------------------------------------- 

float min_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }
    return simd_min_f32(vec->data, vec->len);
}

// -------------------------------------------------------------------------------- 

float max_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }
    return simd_max_f32(vec->data, vec->len);
}

// -------------------------------------------------------------------------------- 

float sum_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }
    return simd_sum_f32(vec->data, vec->len);
}

// -------------------------------------------------------------------------------- 

float average_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }

    float sum = sum_float_vector(vec);
    if (errno != 0) return FLT_MAX;
    return sum / vec->len;
}

// -------------------------------------------------------------------------------- 

float stdev_float_vector(float_v* vec) {
    if (!vec || !vec->data) {
        errno = ENODATA;
        return FLT_MAX;
    }

    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }

    if (vec->len == 1) {
        errno = ENODATA;
        return FLT_MAX;
    }
    return simd_stdev_f32(vec->data, vec->len);
}

// -------------------------------------------------------------------------------- 

float_v* cum_sum_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return NULL;
    }

    float_v* new_vec = init_float_vector(vec->len);
    if (!new_vec) {
        errno = ENOMEM;
        return NULL;
    }

    float sum = 0.0f;
    for (size_t i = 0; i < vec->len; ++i) {
        float val = vec->data[i];
        if (isnan(val)) {
            errno = EINVAL;
            free_float_vector(new_vec);
            return NULL;
        }

        sum += val;

        if (isinf(sum)) {
            // Fill rest with infinity
            for (; i < vec->len; ++i) {
                push_back_float_vector(new_vec, INFINITY);
            }
            return new_vec;
        }

        if (!push_back_float_vector(new_vec, sum)) {
            free_float_vector(new_vec);
            return NULL;
        }
    }

    return new_vec;
}
// -------------------------------------------------------------------------------- 

float_v* copy_float_vector(const float_v* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    float_v* copy = init_float_vector(original->alloc);
    if (!copy) {
        return NULL;
    }

    for (size_t i = 0; i < original->len; ++i) {
        if (!push_back_float_vector(copy, original->data[i])) {
            free_float_vector(copy);
            return NULL;
        }
    }

    return copy;
}
// -------------------------------------------------------------------------------- 

float dot_float(const float* a, const float* b, size_t len) {
    errno = 0;

    if (!a || !b || len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }
    return simd_dot_f32(a, b, len);
}
// -------------------------------------------------------------------------------- 

float dot_float_vector(const float_v* vec1, const float_v* vec2) {
    errno = 0;
    if (!vec1 || !vec2 || !vec1->data || !vec2->data) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (vec1->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }
    if (vec1->len != vec2->len) {
        errno = ERANGE;
        return FLT_MAX;
    }
    return dot_float(vec1->data, vec2->data, vec1->len);
}
// -------------------------------------------------------------------------------- 

bool cross_float(const float* a, const float* b, float* result) {
    errno = 0;

    if (!a || !b || !result) {
        errno = EINVAL;
        return false;
    }

    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];

    return true;
}
// -------------------------------------------------------------------------------- 

float_v* cross_float_vector(const float_v* vec1, const float_v* vec2) {
    errno = 0;

    if (!vec1 || !vec2 || !vec1->data || !vec2->data) {
        errno = EINVAL;
        return NULL;
    }

    if (vec1->len < 3 || vec2->len < 3) {
        errno = ERANGE;
        return NULL;
    }

    float_v* result = init_float_vector(3);
    if (!result) {
        errno = ENOMEM;
        return NULL;
    }

    result->data[0] = vec1->data[1] * vec2->data[2] - vec1->data[2] * vec2->data[1];
    result->data[1] = vec1->data[2] * vec2->data[0] - vec1->data[0] * vec2->data[2];
    result->data[2] = vec1->data[0] * vec2->data[1] - vec1->data[1] * vec2->data[0];

    result->len = 3;
    return result;
}
// -------------------------------------------------------------------------------- 

float float_lin_interp(float x1, float y1, float x2, float y2, float x3) {
    if (x2 == x1 || isnan(x1) || isnan(y1) ||
        isnan(x2) || isnan(y2) || isnan(x3)) {
        errno = EINVAL;
        return NAN;
    }

    return y1 + ( (y2 - y1) / (x2 - x1) ) * (x3 - x1);
}
// ================================================================================
// ================================================================================
// DICTIONARY IMPLEMENTATION

typedef struct fdictNode {
    char* key;
    float value;
    struct fdictNode* next;
} fdictNode;
// --------------------------------------------------------------------------------

struct dict_f {
    fdictNode* keyValues;
    size_t hash_size;
    size_t len;
    size_t alloc;
};
// --------------------------------------------------------------------------------

/**
 * @brief MurmurHash3-inspired hash function for strings
 * 
 * @param key The string key to hash
 * @param seed Optional seed for hash randomization (helps prevent hash flooding)
 * @return size_t The computed hash value
 */
static size_t hash_function(const char* key, const uint32_t seed) {
    if (!key) {
        return 0;
    }

    // Constants for mixing
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    uint32_t h1 = seed;

    // Process key in 4-byte chunks
    const unsigned char* data = (const unsigned char*)key;
    size_t len = strlen(key);
    const size_t nblocks = len / 4;

    // Body
    const uint32_t* blocks = (const uint32_t*)(data);
    for (size_t i = 0; i < nblocks; i++) {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> 17);  // ROTL32(k1, 15)
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19);  // ROTL32(h1, 13)
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Tail
    const unsigned char* tail = data + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
            /* fallthrough */
        case 2:
            k1 ^= tail[1] << 8;
            /* fallthrough */
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = (k1 << 15) | (k1 >> 17);  // ROTL32(k1, 15)
            k1 *= c2;
            h1 ^= k1;
    }

    // Finalization
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return (size_t)h1;
}
// --------------------------------------------------------------------------------

/**
 * @brief Resizes the dictionary's hash table
 * 
 * @param dict Pointer to the dictionary
 * @param new_size Desired new size for the hash table
 * @return bool true if resize successful, false otherwise
 */
static bool resize_dict(dict_f* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    fdictNode* new_table = calloc(new_size, sizeof(fdictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table in case we need to recover
    fdictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash existing entries
    for (size_t i = 0; i < old_size; i++) {
        fdictNode* current = old_table[i].next;
        
        while (current) {
            fdictNode* next = current->next;  // Save next pointer before modifying node

            // Calculate new index using enhanced hash function
            size_t new_index = hash_function(current->key, HASH_SEED) % new_size;

            // Insert at the beginning of the new chain
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            rehashed_count++;
            current = next;
        }
    }

    // Verify all entries were rehashed successfully
    if (rehashed_count != dict->hash_size) {
        // Cleanup and restore old state
        for (size_t i = 0; i < new_size; i++) {
            fdictNode* current = new_table[i].next;
            while (current) {
                fdictNode* next = current->next;
                current->next = NULL;
                current = next;
            }
        }
        free(new_table);
        errno = EAGAIN;
        return false;
    }

    // Update the dictionary only after successful rehashing
    dict->keyValues = new_table;
    dict->alloc = new_size;

    // Clean up old table (but not the nodes, as they were moved)
    free(old_table);

    return true;
}
// --------------------------------------------------------------------------------

dict_f* init_float_dict(void) {
    // Allocate the dictionary structure
    dict_f* dict = calloc(1, sizeof(dict_f));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(fdictNode));
    if (!dict->keyValues) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate hash table array\n");
        free(dict);
        return NULL;
    }

    // Initialize dictionary metadata
    dict->hash_size = 0;   // No items yet
    dict->len = 0;         // No occupied buckets
    dict->alloc = hashSize;

    return dict;
}
// --------------------------------------------------------------------------------

bool insert_float_dict(dict_f* dict, const char* key, float value) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }
    // Check load factor and resize if necessary using adaptive growth strategy
    if (dict->hash_size >= dict->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_size;
        if (dict->alloc < VEC_THRESHOLD) {
            new_size = dict->alloc * 2;  // Double when small
        } else {
            new_size = dict->alloc + VEC_FIXED_AMOUNT;  // Linear growth when large
        }
        
        if (!resize_dict(dict, new_size)) {
            return false;  // resize_dict sets appropriate errno
        }
    }

    // Rest of the function remains the same as previous improvement...
    const size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    // Check for existing key
    for (fdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            errno = EEXIST;
            return false;
        }
    }

    char* new_key = strdup(key);
    if (!new_key) {
        errno = ENOMEM;
        return false;
    }

    fdictNode* new_node = malloc(sizeof(fdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    new_node->key = new_key;
    new_node->value = value;
    new_node->next = dict->keyValues[index].next;
    dict->keyValues[index].next = new_node;

    dict->hash_size++;
    if (new_node->next == NULL) {
        dict->len++;
    }

    return true;
}
// --------------------------------------------------------------------------------

float pop_float_dict(dict_f* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    fdictNode* prev = &dict->keyValues[index];
    fdictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Save value and unlink node
            float value = current->value;
            prev->next = current->next;
            
            // Update dictionary metadata
            dict->hash_size--;
            if (!prev->next) {  // If bucket is now empty
                dict->len--;
            }
            
            // Clean up node memory
            free(current->key);
            free(current);
            
            return value;
        }
        prev = current;
        current = current->next;
    }

    errno = ENOENT;  // Set errno when key not found
    return FLT_MAX;
}
// --------------------------------------------------------------------------------

float get_float_dict_value(const dict_f* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const fdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return FLT_MAX;
}
// --------------------------------------------------------------------------------

void free_float_dict(dict_f* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        fdictNode* current = dict->keyValues[i].next;
        while (current) {      
            fdictNode* next = current->next;  // Save next pointer before freeing
            free(current->key);
            free(current);
            current = next;
        }
    }

    // Free the hash table and dictionary struct
    free(dict->keyValues);
    free(dict);
}
// -------------------------------------------------------------------------------- 

void _free_float_dict(dict_f** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_float_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// --------------------------------------------------------------------------------

bool update_float_dict(dict_f* dict, const char* key, float value) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (fdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return true;
        }
    }

    errno = ENOENT;  // More specific error code for missing key
    return false;
}
// --------------------------------------------------------------------------------

size_t float_dict_size(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t float_dict_alloc(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// --------------------------------------------------------------------------------

size_t float_dict_hash_size(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

bool has_key_float_dict(const dict_f* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const fdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

dict_f* copy_float_dict(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with same capacity
    dict_f* new_dict = malloc(sizeof(dict_f));
    if (!new_dict) {
        errno = ENOMEM;
        return NULL;
    }

    new_dict->keyValues = calloc(dict->alloc, sizeof(fdictNode));
    if (!new_dict->keyValues) {
        free(new_dict);
        errno = ENOMEM;
        return NULL;
    }

    // Copy metadata
    new_dict->alloc = dict->alloc;
    new_dict->hash_size = 0;  // Will be incremented during insertion
    new_dict->len = 0;        // Will be incremented during insertion

    // Copy all entries
    for (size_t i = 0; i < dict->alloc; i++) {
        fdictNode* current = dict->keyValues[i].next;
        while (current) {
            // Insert will handle incrementing hash_size and len
            if (!insert_float_dict(new_dict, current->key, current->value)) {
                free_float_dict(new_dict);  // Clean up on failure
                return NULL;
            }
            current = current->next;
        }
    }

    return new_dict;
}
// -------------------------------------------------------------------------------- 

bool clear_float_dict(dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return false;
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        fdictNode* current = dict->keyValues[i].next;
        while (current) {
            fdictNode* next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
        dict->keyValues[i].next = NULL;  // Reset bucket head
    }

    // Reset dictionary metadata
    dict->hash_size = 0;
    dict->len = 0;

    return true;
}
// -------------------------------------------------------------------------------- 

string_v* get_keys_float_dict(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }
    string_v* vec = init_str_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }
    for (size_t i = 0; i < dict->alloc; i++) {
        for (fdictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_str_vector(vec, current->key)) {
                free_str_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }
    return vec;
}
// -------------------------------------------------------------------------------- 

float_v* get_values_float_dict(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }
    float_v* vec = init_float_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }
    // Iterate through all buckets
    for (size_t i = 0; i < dict->alloc; i++) {
        for (fdictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_float_vector(vec, current->value)) {
                free_float_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }
    return vec;
}
// -------------------------------------------------------------------------------- 

dict_f* merge_float_dict(const dict_f* dict1, const dict_f* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with capacity for all items
    //size_t initial_size = dict1->hash_size + dict2->hash_size;
    dict_f* merged = init_float_dict();
    if (!merged) {
        return NULL;  // errno set by init_float_dict
    }

    // First, copy all entries from dict1
    for (size_t i = 0; i < dict1->alloc; i++) {
        for (fdictNode* current = dict1->keyValues[i].next; current; current = current->next) {
            if (!insert_float_dict(merged, current->key, current->value)) {
                free_float_dict(merged);
                return NULL;
            }
        }
    }

    // Then handle dict2 entries
    for (size_t i = 0; i < dict2->alloc; i++) {
        for (fdictNode* current = dict2->keyValues[i].next; current; current = current->next) {
            float existing_value;
            // Check if key exists in merged dictionary
            if ((existing_value = get_float_dict_value(merged, current->key)) != FLT_MAX) {
                if (overwrite) {
                    // Update existing value if overwrite is true
                    if (!update_float_dict(merged, current->key, current->value)) {
                        free_float_dict(merged);
                        return NULL;
                    }
                }
                // If overwrite is false, keep original value
            } else {
                // Key doesn't exist, insert new entry
                if (!insert_float_dict(merged, current->key, current->value)) {
                    free_float_dict(merged);
                    return NULL;
                }
            }
        }
    }

    return merged;
}
// --------------------------------------------------------------------------------

bool foreach_float_dict(const dict_f* dict, dict_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; i++) {
        for (fdictNode* current = dict->keyValues[i].next; current; current = current->next) {
            iter(current->key, current->value, user_data);
        }
    }

    return true;
}
// ================================================================================ 
// ================================================================================

typedef struct fvdictNode {
    char* key;
    float_v* value;
    struct fvdictNode* next;
} fvdictNode;
// --------------------------------------------------------------------------------

struct dict_fv {
    fvdictNode* keyValues;
    size_t hash_size;
    size_t len;
    size_t alloc;
};
// --------------------------------------------------------------------------------

dict_fv* init_floatv_dict(void) {
    // Allocate the dictionary structure
    dict_fv* dict = calloc(1, sizeof(dict_f));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate vector dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(fvdictNode));
    if (!dict->keyValues) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate hash table array\n");
        free(dict);
        return NULL;
    }

    // Initialize dictionary metadata
    dict->hash_size = 0;   // No items yet
    dict->len = 0;         // No occupied buckets
    dict->alloc = hashSize;

    return dict;
}
// -------------------------------------------------------------------------------- 

static bool resize_dictv(dict_fv* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better hash distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    fvdictNode* new_table = calloc(new_size, sizeof(fvdictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table for cleanup if something fails
    fvdictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash all existing entries into the new table
    for (size_t i = 0; i < old_size; ++i) {
        fvdictNode* current = old_table[i].next;
        while (current) {
            fvdictNode* next = current->next;

            size_t new_index = hash_function(current->key, HASH_SEED) % new_size;

            // Reinsert into the new hash bucket (head insertion)
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;

            rehashed_count++;
            current = next;
        }
    }

    // Validate rehash count matches the number of entries
    if (rehashed_count != dict->hash_size) {
        // Rollback: disconnect moved nodes so they aren't double-freed
        for (size_t i = 0; i < new_size; ++i) {
            fvdictNode* current = new_table[i].next;
            while (current) {
                fvdictNode* next = current->next;
                current->next = NULL;
                current = next;
            }
        }
        free(new_table);
        errno = EAGAIN;
        return false;
    }

    // Replace table on success
    dict->keyValues = new_table;
    dict->alloc = new_size;

    // Free old hash bucket array (nodes were moved, not freed)
    free(old_table);

    return true;
}
// --------------------------------------------------------------------------------

bool create_floatv_dict(dict_fv* dict, char* key, size_t size) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    // Resize if load factor exceeded
    if (dict->hash_size >= dict->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_size = (dict->alloc < VEC_THRESHOLD)
                          ? dict->alloc * 2
                          : dict->alloc + VEC_FIXED_AMOUNT;

        if (!resize_dictv(dict, new_size)) {
            return false;
        }
    }

    const size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    // Check for key collision
    for (fvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            errno = EEXIST;
            return false;
        }
    }

    char* new_key = strdup(key);
    if (!new_key) {
        errno = ENOMEM;
        return false;
    }

    fvdictNode* new_node = malloc(sizeof(fvdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    float_v* value = NULL;
    value = init_float_vector(size);
    if (!value) {
        free(new_key);
        free(new_node);
        errno = ENOMEM;
        return false;
    }

    new_node->key = new_key;
    new_node->value = value;
    new_node->next = dict->keyValues[index].next;
    dict->keyValues[index].next = new_node;

    dict->hash_size++;
    if (new_node->next == NULL) {
        dict->len++;
    }

    return true;
}
// --------------------------------------------------------------------------------

bool pop_floatv_dict(dict_fv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    fvdictNode* prev = &dict->keyValues[index];
    fvdictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            prev->next = current->next;
            
            // Update dictionary metadata
            dict->hash_size--;
            if (!prev->next) {  // If bucket is now empty
                dict->len--;
            }
            
            // Clean up node memory
            free_float_vector(current->value);
            free(current->key);
            free(current);
            
            return true;
        }
        prev = current;
        current = current->next;
    }

    errno = ENOENT;  // Set errno when key not found
    return false;
}
// -------------------------------------------------------------------------------- 

float_v* return_floatv_pointer(dict_fv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return NULL;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const fvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return NULL;
}
// -------------------------------------------------------------------------------- 

void free_floatv_dict(dict_fv* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        fvdictNode* current = dict->keyValues[i].next;
        while (current) {      
            fvdictNode* next = current->next;

            free_float_vector(current->value);
            free(current->key);
            free(current);

            current = next;
        }
    }

    // Free the hash table and dictionary struct
    free(dict->keyValues);
    free(dict);
}
// -------------------------------------------------------------------------------- 

void _free_floatv_dict(dict_fv** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_floatv_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// -------------------------------------------------------------------------------- 

bool has_key_floatv_dict(const dict_fv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const fvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

bool insert_floatv_dict(dict_fv* dict, const char* key, float_v* value) {
    if (!dict || !key || !value) {
        errno = EINVAL;
        return false;
    }

    // Only allow insertion of DYNAMIC vectors
    if (value->alloc_type != DYNAMIC) {
        errno = EPERM;  // Operation not permitted
        return false;
    }

    // Resize if load factor exceeded
    if (dict->hash_size >= dict->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_size = (dict->alloc < VEC_THRESHOLD)
                          ? dict->alloc * 2
                          : dict->alloc + VEC_FIXED_AMOUNT;

        if (!resize_dictv(dict, new_size)) {
            return false;
        }
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    // Check for existing key
    for (fvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            errno = EEXIST;
            return false;
        }
    }

    // Allocate new key string
    char* new_key = strdup(key);
    if (!new_key) {
        errno = ENOMEM;
        return false;
    }

    // Allocate node
    fvdictNode* new_node = malloc(sizeof(fvdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    new_node->key = new_key;
    new_node->value = value;
    new_node->next = dict->keyValues[index].next;
    dict->keyValues[index].next = new_node;

    dict->hash_size++;
    if (new_node->next == NULL) {
        dict->len++;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

size_t float_dictv_size(const dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t float_dictv_alloc(const dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// -------------------------------------------------------------------------------- 

size_t float_dictv_hash_size(const dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

dict_fv* copy_floatv_dict(const dict_fv* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    dict_fv* copy = init_floatv_dict();
    if (!copy) {
        return NULL;  // errno already set
    }

    for (size_t i = 0; i < original->alloc; ++i) {
        fvdictNode* current = original->keyValues[i].next;
        while (current) {
            // if (current->value->alloc_type != DYNAMIC) {
            //     free_floatv_dict(copy);
            //     errno = EPERM;
            //     return NULL;
            // }

            float_v* vec_copy = copy_float_vector(current->value);
            if (!vec_copy) {
                free_floatv_dict(copy);
                return NULL;
            }

            if (!insert_floatv_dict(copy, current->key, vec_copy)) {
                free_float_vector(vec_copy);
                free_floatv_dict(copy);
                return NULL;
            }

            current = current->next;
        }
    }

    return copy;
}
// -------------------------------------------------------------------------------- 

dict_fv* merge_floatv_dict(const dict_fv* dict1, const dict_fv* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Start by deep copying dict1
    dict_fv* merged = copy_floatv_dict(dict1);
    if (!merged) {
        return NULL;
    }

    // Now process dict2 entries
    for (size_t i = 0; i < dict2->alloc; ++i) {
        fvdictNode* current = dict2->keyValues[i].next;
        while (current) {
            if (!current->key || !current->value || current->value->alloc_type != DYNAMIC) {
                free_floatv_dict(merged);
                errno = EPERM;
                return NULL;
            }

            bool exists = has_key_floatv_dict(merged, current->key);
            if (exists && !overwrite) {
                current = current->next;
                continue;
            }

            float_v* vec_copy = copy_float_vector(current->value);
            if (!vec_copy) {
                free_floatv_dict(merged);
                return NULL; // errno set by copy_float_vector
            }

            if (exists) {
                pop_floatv_dict(merged, current->key);
            }

            if (!insert_floatv_dict(merged, current->key, vec_copy)) {
                free_float_vector(vec_copy);
                free_floatv_dict(merged);
                return NULL;
            }

            current = current->next;
        }
    }

    return merged;
}
// -------------------------------------------------------------------------------- 

void clear_floatv_dict(dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        fvdictNode* current = dict->keyValues[i].next;
        dict->keyValues[i].next = NULL;

        while (current) {
            fvdictNode* next = current->next;

            if (current->value) {
                if (current->value->alloc_type == STATIC) {
                    free(current->value);
                } else {
                    free_float_vector(current->value);
                }
            }

            free(current->key);
            free(current);
            current = next;
        }
    }

    dict->hash_size = 0;
    dict->len = 0;
}
// -------------------------------------------------------------------------------- 

bool foreach_floatv_dict(const dict_fv* dict, dict_fv_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        fvdictNode* current = dict->keyValues[i].next;
        while (current) {
            iter(current->key, current->value, user_data);
            current = current->next;
        }
    }

    return true;
}
// -------------------------------------------------------------------------------- 

string_v* get_keys_floatv_dict(const dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }

    string_v* vec = init_str_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        for (fvdictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_str_vector(vec, current->key)) {
                free_str_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }

    return vec;
}
// ================================================================================ 
// ================================================================================ 
// MATRIX FUNCTIONS

static inline matrix_f* init_dense_matrix(size_t rows, size_t cols, size_t initial_alloc) {
    if (cols != 0 && rows > SIZE_MAX / cols) { errno = EOVERFLOW; return NULL; }
    size_t need = rows * cols;
    if (initial_alloc < need) { errno = EINVAL; return NULL; }

    matrix_f* mat = (matrix_f*)malloc(sizeof *mat);
    if (!mat) { errno = ENOMEM; return NULL; }

    float* data_ptr = (float*)calloc(initial_alloc, sizeof(float));
    if (!data_ptr) { errno = ENOMEM; free(mat); return NULL; }

    // 1 byte per cell for now (simple, but bigger than a bitset)
    uint8_t* init_ptr = (uint8_t*)calloc(initial_alloc, sizeof(uint8_t));
    if (!init_ptr) { errno = ENOMEM; free(data_ptr); free(mat); return NULL; }

    mat->type = DENSE;
    mat->rows = rows;
    mat->cols = cols;

    mat->storage.dense.data      = data_ptr;
    mat->storage.dense.init_bits = init_ptr;
    mat->storage.dense.alloc     = initial_alloc;
    mat->storage.dense.size      = 0;

    return mat;
}
// -------------------------------------------------------------------------------- 

static inline matrix_f* init_coo_matrix(size_t rows, size_t cols, size_t initial_alloc) {
    // COO capacity is independent of rows*cols; allow 0 -> bump to 1.
    if (initial_alloc == 0) initial_alloc = 1;

    matrix_f* mat = (matrix_f*)malloc(sizeof *mat);
    if (!mat) { errno = ENOMEM; return NULL; }

    triplet_f* triplets = (triplet_f*)malloc(initial_alloc * sizeof *triplets);
    if (!triplets) { errno = ENOMEM; free(mat); return NULL; }

    mat->type = COO;
    mat->rows = rows;
    mat->cols = cols;

    mat->storage.coo.triplets = triplets;
    mat->storage.coo.alloc    = initial_alloc;
    mat->storage.coo.size     = 0;
    mat->storage.coo.fast_set = true;

    return mat;
}
// -------------------------------------------------------------------------------- 

matrix_f* init_float_matrix(size_t rows, size_t cols, size_t initial_alloc, MatrixType mat_type) {
    switch (mat_type) {
        case DENSE: return init_dense_matrix(rows, cols, initial_alloc);
        case COO:   return init_coo_matrix(rows, cols, initial_alloc);
        default:    errno = EINVAL; return NULL; // CSR/CSC must be built via convertors
    }
}
// -------------------------------------------------------------------------------- 

static inline void destroy_matrix_members(matrix_f* mat) {
    if (!mat) return;
    switch (mat->type) {
        case DENSE: {
            free(mat->storage.dense.data);
            free(mat->storage.dense.init_bits);
            mat->storage.dense.data = NULL;
            mat->storage.dense.init_bits = NULL;
            mat->storage.dense.alloc = 0;
            mat->storage.dense.size = 0;
        } break;
        case COO: {
            free(mat->storage.coo.triplets);
            mat->storage.coo.triplets = NULL;
            mat->storage.coo.alloc = 0;
            mat->storage.coo.size = 0;
            mat->storage.coo.fast_set = true;  // optional default
        } break;
        case CSR: {
            free(mat->storage.csr.data);
            free(mat->storage.csr.col_idx);
            free(mat->storage.csr.row_ptr);
            mat->storage.csr.data = NULL;
            mat->storage.csr.col_idx = NULL;
            mat->storage.csr.row_ptr = NULL;
            mat->storage.csr.alloc = 0;
            mat->storage.csr.size = 0;
        } break;
        case CSC: {
            free(mat->storage.csc.data);
            free(mat->storage.csc.row_idx);
            free(mat->storage.csc.col_ptr);
            mat->storage.csc.data = NULL;
            mat->storage.csc.row_idx = NULL;
            mat->storage.csc.col_ptr = NULL;
            mat->storage.csc.alloc = 0;
            mat->storage.csc.size = 0;
        } break;
        default:
            /* Unknown/unused type: do nothing */
            break;
    }
    mat->type = MATRIX_INVALID;
    mat->rows = 0;
    mat->cols = 0;
}
// -------------------------------------------------------------------------------- 

void free_float_matrix(matrix_f* mat) {
    if (!mat) return;
    destroy_matrix_members(mat);
    free(mat);
}
// -------------------------------------------------------------------------------- 

void _free_float_matrix(matrix_f** pm) {
    if (pm && *pm) {
        free_float_matrix(*pm);
        *pm = NULL;
    }
}
// -------------------------------------------------------------------------------- 

static inline bool _dense_bounds_ok(const matrix_f* m, size_t r, size_t c) {
    return m && m->type == DENSE && r < m->rows && c < m->cols;
}
// -------------------------------------------------------------------------------- 

static inline size_t _dense_index(const matrix_f* m, size_t r, size_t c) {
    // r < rows and c < cols already checked; guard overflow just in case
    if (m->cols && r > SIZE_MAX / m->cols) return (size_t)-1;
    return r * m->cols + c;
}
// -------------------------------------------------------------------------------- 

static bool _set_float_dense_matrix(matrix_f* m, size_t r, size_t c, float v) {
    if (!_dense_bounds_ok(m, r, c)) { errno = EINVAL; return false; }
    if (!m->storage.dense.data || !m->storage.dense.init_bits) { errno = ENOTSUP; return false; }

    const size_t idx = _dense_index(m, r, c);
    if (idx == (size_t)-1 || idx >= m->storage.dense.alloc) { errno = EOVERFLOW; return false; }

    if (m->storage.dense.init_bits[idx]) { errno = EEXIST; return false; }

    m->storage.dense.data[idx] = v;
    m->storage.dense.init_bits[idx] = 1u;
    if (m->storage.dense.size < m->storage.dense.alloc) {
        m->storage.dense.size += 1;  // track how many cells have been initialized
    }
    return true;
}
// -------------------------------------------------------------------------------- 

static bool _insert_float_dense_matrix(matrix_f* m, size_t r, size_t c, float v) {
    if (!_dense_bounds_ok(m, r, c)) { errno = EINVAL; return false; }
    if (!m->storage.dense.data) { errno = EINVAL; return false; }

    const size_t idx = _dense_index(m, r, c);
    if (idx == (size_t)-1 || idx >= m->storage.dense.alloc) { errno = EOVERFLOW; return false; }

    m->storage.dense.data[idx] = v;

    if (m->storage.dense.init_bits) {
        if (!m->storage.dense.init_bits[idx]) {
            m->storage.dense.init_bits[idx] = 1u;
            if (m->storage.dense.size < m->storage.dense.alloc) {
                m->storage.dense.size += 1;
            }
        }
    }
    return true;
}
// -------------------------------------------------------------------------------- 

/* Sort helper: (row, col) */
static int _cmp_triplet_row_col(const void* a, const void* b) {
    const triplet_f* ta = (const triplet_f*)a;
    const triplet_f* tb = (const triplet_f*)b;
    if (ta->row < tb->row) return -1;
    if (ta->row > tb->row) return  1;
    if (ta->col < tb->col) return -1;
    if (ta->col > tb->col) return  1;
    return 0;
}

/* Finalize: sort by (row,col), coalesce duplicates by SUM, and mark read-optimized */
bool finalize_float_coo_matrix(matrix_f* m) {
    if (!m || m->type != COO) { errno = EINVAL; return false; }
    coo_f* C = &m->storage.coo;
    if (!C->triplets || C->size <= 1) { C->fast_set = false; return true; }
    if (!C->fast_set) return true;  // already finalized

    /* 1) sort */
    qsort(C->triplets, C->size, sizeof *C->triplets, _cmp_triplet_row_col);

    /* 2) coalesce in place: sum consecutive duplicates with same (row,col) */
    size_t w = 0;
    for (size_t i = 0; i < C->size; ) {
        uint32_t r = C->triplets[i].row;
        uint32_t c = C->triplets[i].col;
        double   acc = 0.0;
        do {
            acc += (double)C->triplets[i].data;
            ++i;
        } while (i < C->size &&
                 C->triplets[i].row == r &&
                 C->triplets[i].col == c);

        C->triplets[w].row  = r;
        C->triplets[w].col  = c;
        C->triplets[w].data = (float)acc;
        ++w;
    }
    C->size = w;

    /* 3) mark read-optimized */
    C->fast_set = false;
    return true;
}
// -------------------------------------------------------------------------------- 

/* key + lower_bound on (row,col) */
static inline unsigned long long _key_rc(uint32_t r, uint32_t c) {
    return ((unsigned long long)r << 32) | (unsigned long long)c;
}
// -------------------------------------------------------------------------------- 

static size_t _lower_bound_triplets(const triplet_f* t, size_t n, uint32_t r, uint32_t c) {
    const unsigned long long k = _key_rc(r, c);
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + ((hi - lo) >> 1);
        unsigned long long km = _key_rc(t[mid].row, t[mid].col);
        if (km < k) lo = mid + 1; else hi = mid;
    }
    return lo;
}
// -------------------------------------------------------------------------------- 

static bool _coo_ensure_capacity(coo_f* C, size_t need) {
    if (need <= C->alloc) return true;
    size_t new_alloc = (C->alloc == 0) ? 1 : C->alloc;
    do {
        if (new_alloc < VEC_THRESHOLD) {
            if (new_alloc > SIZE_MAX / 2) { errno = EOVERFLOW; return false; }
            new_alloc *= 2;
        } else {
            if (SIZE_MAX - new_alloc < (size_t)VEC_FIXED_AMOUNT) { errno = EOVERFLOW; return false; }
            new_alloc += (size_t)VEC_FIXED_AMOUNT;
        }
    } while (new_alloc < need);

    triplet_f* t2 = (triplet_f*)realloc(C->triplets, new_alloc * sizeof *t2);
    if (!t2) { errno = ENOMEM; return false; }
    C->triplets = t2;
    C->alloc = new_alloc;
    return true;
}
// -------------------------------------------------------------------------------- 

/* Core inserter: in build mode -> append; in finalized mode -> unique insert or overwrite */
static bool _insert_float_coo_matrix(matrix_f* m, size_t r, size_t c, float v, bool overwrite_if_exists) {
    if (!m || m->type != COO) { errno = EINVAL; return false; }
    if (r >= m->rows || c >= m->cols) { errno = EINVAL; return false; }
    coo_f* C = &m->storage.coo;

    if (C->fast_set) {
        if (!_coo_ensure_capacity(C, C->size + 1)) return false;
        C->triplets[C->size++] = (triplet_f){ r, c, v };
        return true;
    }

    /* finalized: keep sorted by (row,col) */
    size_t pos = _lower_bound_triplets(C->triplets, C->size, r, c);
    if (pos < C->size && C->triplets[pos].row == r && C->triplets[pos].col == c) {
        if (overwrite_if_exists) {
            C->triplets[pos].data = v;     /* overwrite in place */
            return true;
        } else {
            errno = EEXIST;                /* enforce uniqueness */
            return false;
        }
    }

    /* no existing entry: insert at pos */
    if (!_coo_ensure_capacity(C, C->size + 1)) return false;
    size_t tail = C->size - pos;
    if (tail) {
        memmove(&C->triplets[pos + 1], &C->triplets[pos], tail * sizeof(triplet_f));
    }
    C->triplets[pos] = (triplet_f){ r, c, v };
    C->size++;
    return true;
}
// -------------------------------------------------------------------------------- 

static bool _insert_float_coo_matrix_unique(matrix_f* m, size_t r, size_t c, float v) {
    return _insert_float_coo_matrix(m, r, c, v, /*overwrite_if_exists=*/false);
}
// -------------------------------------------------------------------------------- 

static bool _set_float_coo_matrix(matrix_f* m, size_t r, size_t c, float v) {
    return _insert_float_coo_matrix(m, r, c, v, /*overwrite_if_exists=*/true);
}
// -------------------------------------------------------------------------------- 

bool insert_float_matrix(matrix_f* m, size_t r, size_t c, float v) {
    if (!m) {
        errno = EINVAL;
        return false;
    }
    switch(m->type) {
        case DENSE: return _insert_float_dense_matrix(m, r, c, v);
        case COO: return _set_float_coo_matrix(m, r, c, v);
        default: errno = ENOTSUP; return false;
    }
}
// -------------------------------------------------------------------------------- 

bool set_float_matrix(matrix_f* m, size_t r, size_t c, float v) {
    if (!m) {
        errno = EINVAL;
        return false;
    }
    switch(m->type) {
        case DENSE: return _set_float_dense_matrix(m, r, c, v);
        case COO: return _insert_float_coo_matrix_unique(m, r, c, v);
        default: errno = ENOTSUP; return false;
    }
}
// -------------------------------------------------------------------------------- 

static inline size_t _float_dense_matrix_size(const matrix_f* mat) {
    if (mat->type != DENSE) {
        errno = EINVAL;
        return 0;
    }
    return mat->storage.dense.size;
}
// -------------------------------------------------------------------------------- 

static inline size_t _float_coo_matrix_size(const matrix_f* mat) {
    if (mat->type != COO) {
        errno = EINVAL;
        return 0;
    }
    return mat->storage.coo.size;
}
// -------------------------------------------------------------------------------- 

static inline size_t _float_csr_matrix_size(const matrix_f* mat) {
    if (mat->type != CSR) {
        errno = EINVAL;
        return 0;
    }
    return mat->storage.csr.size;
}
// -------------------------------------------------------------------------------- 

static inline size_t _float_csc_matrix_size(const matrix_f* mat) {
    if (mat->type != CSC) {
        errno = EINVAL;
        return 0;
    }
    return mat->storage.csc.size;
}
// -------------------------------------------------------------------------------- 

size_t float_matrix_size(const matrix_f* mat) {
    switch(mat->type) {
        case DENSE: return _float_dense_matrix_size(mat);
        case COO: return _float_coo_matrix_size(mat);
        case CSR: return _float_csr_matrix_size(mat);
        case CSC: return _float_csc_matrix_size(mat);
        default: errno = ENOTSUP; return 0;
    }
}
// -------------------------------------------------------------------------------- 

static inline size_t _float_dense_matrix_alloc(const matrix_f* mat) {
    if (mat->type != DENSE) {
        errno = EINVAL;
        return 0;
    }
    return mat->storage.dense.alloc;
}
// -------------------------------------------------------------------------------- 

static inline size_t _float_coo_matrix_alloc(const matrix_f* mat) {
    if (mat->type != COO) {
        errno = EINVAL;
        return 0;
    }
    return mat->storage.coo.alloc;
}
// -------------------------------------------------------------------------------- 

static inline size_t _float_csr_matrix_alloc(const matrix_f* mat) {
    if (mat->type != CSR) {
        errno = EINVAL;
        return 0;
    }
    return mat->storage.csr.alloc;
}
// -------------------------------------------------------------------------------- 

static inline size_t _float_csc_matrix_alloc(const matrix_f* mat) {
    if (mat->type != CSC) {
        errno = EINVAL;
        return 0;
    }
    return mat->storage.csc.alloc;
}
// -------------------------------------------------------------------------------- 

size_t float_matrix_alloc(const matrix_f* mat) {
    switch(mat->type) {
        case DENSE: return _float_dense_matrix_alloc(mat);
        case COO: return _float_coo_matrix_alloc(mat);
        case CSR: return _float_csr_matrix_alloc(mat);
        case CSC: return _float_csc_matrix_alloc(mat);
        default: errno = ENOTSUP; return 0;
    }
}
// -------------------------------------------------------------------------------- 

MatrixType float_matrix_type(const matrix_f* mat) {
    if (!mat) {
        errno = EINVAL;
        return MATRIX_INVALID;
    }
    return mat->type;
}
// -------------------------------------------------------------------------------- 

bool set_float_fast_set(matrix_f* m) {
    if (!m || m->type != COO) { 
        errno = EINVAL; 
        return false; 
    }
    m->storage.coo.fast_set = true;
    return true;
}
// -------------------------------------------------------------------------------- 

static bool _delete_float_dense_element(matrix_f* m, size_t r, size_t c) {
    if (!_dense_bounds_ok(m, r, c)) { errno = EINVAL; return false; }
    if (!m->storage.dense.data || !m->storage.dense.init_bits) {
        errno = ENOTSUP; return false;
    }

    const size_t idx = _dense_index(m, r, c);
    if (idx == (size_t)-1 || idx >= m->storage.dense.alloc) { errno = EOVERFLOW; return false; }

    if (!m->storage.dense.init_bits[idx]) { errno = ENOENT; return false; }

    m->storage.dense.data[idx] = 0.0f;      /* optional: zero the value */
    m->storage.dense.init_bits[idx] = 0u;   /* mark as uninitialized */
    if (m->storage.dense.size > 0) m->storage.dense.size--;
    return true;
}
// -------------------------------------------------------------------------------- 

static bool _delete_float_coo_element(matrix_f* m, size_t r, size_t c) {
    if (!m || m->type != COO) { errno = EINVAL; return false; }
    if (r >= m->rows || c >= m->cols) { errno = EINVAL; return false; }

    coo_f* C = &m->storage.coo;
    if (C->size == 0) { errno = ENOENT; return false; }

    if (C->fast_set) {
        /* Build mode: linear search + swap-with-last (O(1) removal) */
        for (size_t i = 0; i < C->size; ++i) {
            if (C->triplets[i].row == (uint32_t)r && C->triplets[i].col == (uint32_t)c) {
                size_t last = C->size - 1;
                if (i != last) C->triplets[i] = C->triplets[last];
                C->size--;
                return true;
            }
        }
        errno = ENOENT; return false;
    } else {
        /* Finalized: keep sorted -> lower_bound + memmove */
        size_t pos = _lower_bound_triplets(C->triplets, C->size, (uint32_t)r, (uint32_t)c);
        if (pos >= C->size ||
            C->triplets[pos].row != (uint32_t)r ||
            C->triplets[pos].col != (uint32_t)c) { errno = ENOENT; return false; }
        size_t tail = C->size - 1 - pos;
        if (tail) memmove(&C->triplets[pos], &C->triplets[pos + 1], tail * sizeof(triplet_f));
        C->size--;
        return true;
    }
}
// -------------------------------------------------------------------------------- 

bool delete_float_matrix_element(matrix_f* m, size_t r, size_t c) {
    if (!m) { errno = EINVAL; return false; }
    switch (m->type) {
        case DENSE: return _delete_float_dense_element(m, r, c);
        case COO:   return _delete_float_coo_element(m, r, c);
        default:    errno = ENOTSUP; return false;
    }
}
// -------------------------------------------------------------------------------- 

static size_t _upper_bound_triplets(const triplet_f* t, size_t n, uint32_t r, uint32_t c) {
    unsigned long long k = ((unsigned long long)r << 32) | (unsigned long long)c;
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + ((hi - lo) >> 1);
        unsigned long long km = ((unsigned long long)t[mid].row << 32) | (unsigned long long)t[mid].col;
        if (km <= k) lo = mid + 1; else hi = mid;
    }
    return lo;
}
// -------------------------------------------------------------------------------- 

static float _get_float_dense_element(const matrix_f* m, size_t r, size_t c) {
    if (!m || m->type != DENSE) { errno = EINVAL; return NAN; }
    if (r >= m->rows || c >= m->cols) { errno = EINVAL; return NAN; }
    if (!m->storage.dense.data || !m->storage.dense.init_bits) { errno = ENOTSUP; return NAN; }

    /* index = r*cols + c with overflow guard (also guarded by bounds above) */
    if (m->cols && r > SIZE_MAX / m->cols) { errno = EOVERFLOW; return NAN; }
    size_t idx = r * m->cols + c;
    if (idx >= m->storage.dense.alloc) { errno = EOVERFLOW; return NAN; }

    if (!m->storage.dense.init_bits[idx]) { errno = ENODATA; return NAN; }
    return m->storage.dense.data[idx];
}
// -------------------------------------------------------------------------------- 

static float _get_float_coo_element(const matrix_f* m, size_t r, size_t c) {
    if (!m || m->type != COO) { errno = EINVAL; return NAN; }
    if (r >= m->rows || c >= m->cols) { errno = EINVAL; return NAN; }
    const coo_f* C = &m->storage.coo;
    if (C->size == 0) { errno = ENOENT; return NAN; }

    double acc = 0.0;
    if (C->fast_set) {
        bool found = false;
        for (size_t i = 0; i < C->size; ++i) {
            if (C->triplets[i].row == (uint32_t)r && C->triplets[i].col == (uint32_t)c) {
                acc += (double)C->triplets[i].data;
                found = true;
            }
        }
        if (!found) { errno = ENOENT; return NAN; }
    } else {
        size_t lo = _lower_bound_triplets(C->triplets, C->size, (uint32_t)r, (uint32_t)c);
        if (lo == C->size ||
            C->triplets[lo].row != (uint32_t)r ||
            C->triplets[lo].col != (uint32_t)c) {
            errno = ENOENT; return NAN;
        }
        size_t hi = _upper_bound_triplets(C->triplets, C->size, (uint32_t)r, (uint32_t)c);
        for (size_t i = lo; i < hi; ++i) acc += (double)C->triplets[i].data;
    }
    return (float)acc;
}
// -------------------------------------------------------------------------------- 

static float _get_float_csr_element(const matrix_f* m, size_t r, size_t c) {
    if (!m || m->type != CSR) { errno = EINVAL; return NAN; }
    if (r >= m->rows || c >= m->cols) { errno = EINVAL; return NAN; }

    const csr_f* S = &m->storage.csr;
    if (!S->row_ptr || !S->col_idx || !S->data) { errno = ENOTSUP; return NAN; }
    if (S->size == 0) { errno = ENOENT; return NAN; }

    size_t start = S->row_ptr[r];
    size_t end   = S->row_ptr[r + 1];
    if (end < start || end > S->size) { errno = EINVAL; return NAN; }

    /* Binary search in col_idx[start..end) for c */
    size_t lo = start, hi = end;
    while (lo < hi) {
        size_t mid = lo + ((hi - lo) >> 1);
        uint32_t cmid = S->col_idx[mid];
        if (cmid < (uint32_t)c) lo = mid + 1; else hi = mid;
    }
    if (lo == end || S->col_idx[lo] != (uint32_t)c) { errno = ENOENT; return NAN; }

    /* Sum any duplicates adjacent (just in case) */
    double acc = 0.0;
    for (size_t i = lo; i < end && S->col_idx[i] == (uint32_t)c; ++i) acc += (double)S->data[i];
    return (float)acc;
}
// -------------------------------------------------------------------------------- 

static float _get_float_csc_element(const matrix_f* m, size_t r, size_t c) {
    if (!m || m->type != CSC) { errno = EINVAL; return NAN; }
    if (r >= m->rows || c >= m->cols) { errno = EINVAL; return NAN; }

    const csc_f* S = &m->storage.csc;
    if (!S->col_ptr || !S->row_idx || !S->data) { errno = ENOTSUP; return NAN; }
    if (S->size == 0) { errno = ENOENT; return NAN; }

    size_t start = S->col_ptr[c];
    size_t end   = S->col_ptr[c + 1];
    if (end < start || end > S->size) { errno = EINVAL; return NAN; }

    /* Binary search in row_idx[start..end) for r */
    size_t lo = start, hi = end;
    while (lo < hi) {
        size_t mid = lo + ((hi - lo) >> 1);
        uint32_t rmid = S->row_idx[mid];
        if (rmid < (uint32_t)r) lo = mid + 1; else hi = mid;
    }
    if (lo == end || S->row_idx[lo] != (uint32_t)r) { errno = ENOENT; return NAN; }

    /* Sum duplicates adjacent (if any) */
    double acc = 0.0;
    for (size_t i = lo; i < end && S->row_idx[i] == (uint32_t)r; ++i) acc += (double)S->data[i];
    return (float)acc;
}
// -------------------------------------------------------------------------------- 

float get_float_matrix_element(const matrix_f* m, size_t r, size_t c) {
    if (!m) { errno = EINVAL; return NAN; }
    switch (m->type) {
        case DENSE: return _get_float_dense_element(m, r, c);
        case COO:   return _get_float_coo_element(m, r, c);
        case CSR:   return _get_float_csr_element(m, r, c);
        case CSC:   return _get_float_csc_element(m, r, c);
        default:    errno = ENOTSUP; return NAN;
    }
}
// -------------------------------------------------------------------------------- 

static matrix_f* _init_csr_matrix(size_t rows, size_t cols, size_t initial_alloc) {
    if (rows == 0 || cols == 0) { errno = EINVAL; return NULL; }
    if (initial_alloc == 0) initial_alloc = 1;

    /* guard rows+1 overflow */
    if (rows == (size_t)-1) { errno = EOVERFLOW; return NULL; }

    matrix_f* m = (matrix_f*)malloc(sizeof *m);
    if (!m) { errno = ENOMEM; return NULL; }

    float*    data    = (float*)   malloc(initial_alloc * sizeof(float));
    uint32_t* col_idx = (uint32_t*)malloc(initial_alloc * sizeof(uint32_t));
    size_t*   row_ptr = (size_t*)  calloc(rows + 1, sizeof(size_t)); /* zeroed */

    if (!data || !col_idx || !row_ptr) {
        errno = ENOMEM;
        free(data); free(col_idx); free(row_ptr); free(m);
        return NULL;
    }

    m->type = CSR; m->rows = rows; m->cols = cols;
    m->storage.csr.data    = data;
    m->storage.csr.col_idx = col_idx;
    m->storage.csr.row_ptr = row_ptr;
    m->storage.csr.alloc   = initial_alloc;  /* nnz capacity */
    m->storage.csr.size    = 0;              /* nnz used */

    return m;
}
// -------------------------------------------------------------------------------- 

static matrix_f* _init_csc_matrix(size_t rows, size_t cols, size_t initial_alloc) {
    if (rows == 0 || cols == 0) { errno = EINVAL; return NULL; }
    if (initial_alloc == 0) initial_alloc = 1;

    /* guard cols+1 overflow */
    if (cols == (size_t)-1) { errno = EOVERFLOW; return NULL; }

    matrix_f* m = (matrix_f*)malloc(sizeof *m);
    if (!m) { errno = ENOMEM; return NULL; }

    float*    data    = (float*)   malloc(initial_alloc * sizeof(float));
    uint32_t* row_idx = (uint32_t*)malloc(initial_alloc * sizeof(uint32_t));
    size_t*   col_ptr = (size_t*)  calloc(cols + 1, sizeof(size_t)); /* zeroed */

    if (!data || !row_idx || !col_ptr) {
        errno = ENOMEM;
        free(data); free(row_idx); free(col_ptr); free(m);
        return NULL;
    }

    m->type = CSC; m->rows = rows; m->cols = cols;
    m->storage.csc.data    = data;
    m->storage.csc.row_idx = row_idx;
    m->storage.csc.col_ptr = col_ptr;
    m->storage.csc.alloc   = initial_alloc;  /* nnz capacity */
    m->storage.csc.size    = 0;              /* nnz used */

    return m;
}

// -------------------------------------------------------------------------------- 

static matrix_f* _dense_matrix_from_array(size_t rows, size_t cols,
                                          const float* array, size_t arr_len,
                                          size_t initial_alloc) {
    if (!array) { errno = EINVAL; return NULL; }
    if (cols && rows > SIZE_MAX / cols) { errno = EOVERFLOW; return NULL; }
    const size_t need = rows * cols;

    if (arr_len != need) { errno = ERANGE; return NULL; }          // must match exactly
    if (initial_alloc < need) { errno = EINVAL; return NULL; }     // you can relax this if you want

    matrix_f* m = init_dense_matrix(rows, cols, initial_alloc);
    if (!m) return NULL;

    memcpy(m->storage.dense.data, array, need * sizeof(float));

    /* Mark initialized cells: you currently use 1 byte per cell */
    memset(m->storage.dense.init_bits, 1, need);
    m->storage.dense.size = need;   /* number of initialized cells */

    return m;
}
// -------------------------------------------------------------------------------- 

static matrix_f* _coo_matrix_from_array(size_t rows, size_t cols,
                                        const float* array, size_t arr_len,
                                        size_t initial_alloc) {
    if (!array) { errno = EINVAL; return NULL; }
    if (cols && rows > SIZE_MAX / cols) { errno = EOVERFLOW; return NULL; }
    const size_t need = rows * cols;

    if (arr_len != need) { errno = ERANGE; return NULL; }

    matrix_f* m = init_coo_matrix(rows, cols, initial_alloc);  
    if (!m) return NULL;

    coo_f* C = &m->storage.coo;
    C->size = 0;                    /* start empty, fast_set == true */
    for (size_t idx = 0; idx < need; ++idx) {
        float v = array[idx];
        if (v != 0.0f) {
            /* ensure capacity then append (O(1) in build mode) */
            if (!_coo_ensure_capacity(C, C->size + 1)) { free_float_matrix(m); return NULL; }
            size_t r = idx / cols;
            size_t c = idx % cols;
            C->triplets[C->size++] = (triplet_f){ (uint32_t)r, (uint32_t)c, v };
        }
    }

    if (!finalize_float_coo_matrix(m)) { free_float_matrix(m); return NULL; }

    return m;
}
// -------------------------------------------------------------------------------- 

static inline matrix_f* _csr_matrix_from_array(size_t rows, size_t cols,
                                               const float* array, size_t arr_len,
                                               size_t initial_alloc) {
    if (!array || rows == 0 || cols == 0) { errno = EINVAL; return NULL; }
    if (cols && rows > SIZE_MAX / cols)    { errno = EOVERFLOW; return NULL; }
    const size_t need = rows * cols;
    if (arr_len != need) { errno = ERANGE; return NULL; }

    /* Pass 1: count nnz per row */
    size_t* row_cnt = (size_t*)calloc(rows, sizeof(size_t));
    if (!row_cnt) { errno = ENOMEM; return NULL; }

    size_t nnz = 0;
    for (size_t idx = 0; idx < need; ++idx) {
        float v = array[idx];
        if (v != 0.0f) { row_cnt[idx / cols]++; nnz++; }
    }

    size_t cap = initial_alloc;
    if (cap < nnz) cap = nnz;
    if (cap == 0)  cap = 1;

    matrix_f* m = _init_csr_matrix(rows, cols, cap);
    if (!m) { free(row_cnt); return NULL; }

    csr_f* S = &m->storage.csr;

    /* Build row_ptr = prefix sums of counts */
    S->row_ptr[0] = 0;
    for (size_t r = 0; r < rows; ++r) {
        S->row_ptr[r + 1] = S->row_ptr[r] + row_cnt[r];
    }
    S->size = nnz;

    /* Scatter: use a row cursor that starts at row_ptr[r] */
    size_t* cursor = (size_t*)malloc(rows * sizeof(size_t));
    if (!cursor) { free(row_cnt); free_float_matrix(m); errno = ENOMEM; return NULL; }
    memcpy(cursor, S->row_ptr, rows * sizeof(size_t));

    for (size_t idx = 0; idx < need; ++idx) {
        float v = array[idx];
        if (v != 0.0f) {
            size_t r = idx / cols;
            size_t c = idx % cols;
            size_t pos = cursor[r]++;
            /* pos is guaranteed < nnz <= cap */
            S->data[pos]    = v;
            S->col_idx[pos] = (uint32_t)c;
        }
    }

    free(cursor);
    free(row_cnt);
    return m;
}
// -------------------------------------------------------------------------------- 

static inline matrix_f* _csc_matrix_from_array(size_t rows, size_t cols,
                                               const float* array, size_t arr_len,
                                               size_t initial_alloc) {
    if (!array || rows == 0 || cols == 0) { errno = EINVAL; return NULL; }
    if (cols && rows > SIZE_MAX / cols)    { errno = EOVERFLOW; return NULL; }
    const size_t need = rows * cols;
    if (arr_len != need) { errno = ERANGE; return NULL; }

    /* Pass 1: count nnz per column */
    size_t* col_cnt = (size_t*)calloc(cols, sizeof(size_t));
    if (!col_cnt) { errno = ENOMEM; return NULL; }

    size_t nnz = 0;
    for (size_t idx = 0; idx < need; ++idx) {
        float v = array[idx];
        if (v != 0.0f) { col_cnt[idx % cols]++; nnz++; }
    }

    size_t cap = initial_alloc;
    if (cap < nnz) cap = nnz;
    if (cap == 0)  cap = 1;

    matrix_f* m = _init_csc_matrix(rows, cols, cap);
    if (!m) { free(col_cnt); return NULL; }

    csc_f* S = &m->storage.csc;

    /* Build col_ptr = prefix sums of counts */
    S->col_ptr[0] = 0;
    for (size_t c = 0; c < cols; ++c) {
        S->col_ptr[c + 1] = S->col_ptr[c] + col_cnt[c];
    }
    S->size = nnz;

    /* Scatter: use a column cursor that starts at col_ptr[c] */
    size_t* cursor = (size_t*)malloc(cols * sizeof(size_t));
    if (!cursor) { free(col_cnt); free_float_matrix(m); errno = ENOMEM; return NULL; }
    memcpy(cursor, S->col_ptr, cols * sizeof(size_t));

    for (size_t idx = 0; idx < need; ++idx) {
        float v = array[idx];
        if (v != 0.0f) {
            size_t r = idx / cols;
            size_t c = idx % cols;
            size_t pos = cursor[c]++;
            S->data[pos]    = v;
            S->row_idx[pos] = (uint32_t)r;
        }
    }

    free(cursor);
    free(col_cnt);
    return m;
}
// -------------------------------------------------------------------------------- 

matrix_f* float_matrix_from_array(size_t rows, size_t cols, MatrixType type,
                                  const float* array, size_t arr_len,
                                  size_t initial_alloc) {
    if (!array) { errno = EINVAL; return NULL; }
    if (type == MATRIX_INVALID) { errno = ENOTSUP; return NULL; }

    switch (type) {
        case DENSE:
            return _dense_matrix_from_array(rows, cols, array, arr_len, initial_alloc);
        case COO:
            return _coo_matrix_from_array(rows, cols, array, arr_len, initial_alloc);
        case CSR:
            return _csr_matrix_from_array(rows, cols, array, arr_len, initial_alloc);
        case CSC:
            return _csc_matrix_from_array(rows, cols, array, arr_len, initial_alloc);
        default:
            errno = ENOTSUP; return NULL;   // CSR/CSC not buildable from this form (yet)
    }
}
// ================================================================================ 
// ================================================================================ 
// eof
