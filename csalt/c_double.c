// ================================================================================
// ================================================================================
// - File:    c_double.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 04, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <immintrin.h>  // AVX/SSE
#include "c_double.h"
#include <errno.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#if defined(__AVX512F__)
  #include <immintrin.h>
  #include "simd_avx512_double.inl"
#elif defined(__AVX2__)
  #include <immintrin.h>
  #include "simd_avx2_double.inl"
#elif defined(__AVX__)
  #include <immintrin.h>
  #include "simd_avx_double.inl"
#elif defined(__SSE4_1__)
  #include <smmintrin.h>
  #include "simd_sse41_double.inl"
#elif defined(__SSE3__)
  #include <pmmintrin.h>
  #include "simd_sse3_double.inl"
#elif defined(__SSE2__)
  #include <emmintrin.h>
  #include "simd_sse2_double.inl"
#elif defined(__ARM_FEATURE_SVE2)
  #include <arm_sve.h>
  #include "simd_sve2_double.inl"
#elif defined(__ARM_FEATURE_SVE)
  #include <arm_sve.h>
  #include "simd_sve_double.inl"
#elif defined(__aarch64__) && defined(__ARM_NEON)
  #include <arm_neon.h>
  #include "simd_neon_double.inl"
#endif

static const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
static const size_t hashSize = 16;  //  Size fo hash map init functions
static const uint32_t HASH_SEED = 0x45d9f3b;

static const float SPARSE_THRESHOLD = 0.15;  // Convert to sparse if < 15% filled
static const float DENSE_THRESHOLD = 0.30;   // Convert to dense if > 30% filled
static const size_t MIN_SPARSE_SIZE = 1000;  // Below this many elements, just use dense
static const size_t INITIAL_COO_CAPACITY = 16;

static const size_t MAX_DENSE_ELEMENTS = 100000;
static const size_t COO_TO_CSR_TRIGGER = 10000;

static const float CSR_COMPACT_THRESHOLD = 0.25f;
static const size_t CSR_TOMBSTONE_COL = SIZE_MAX;
// ================================================================================
// ================================================================================ 

static inline double simd_sum_f64(const double* x, size_t n) {
    #if defined(__AVX512F__)
        return simd_sum_f64_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_sum_f64_avx2(x, n);
    #elif defined(__AVX__)
        return simd_sum_f64_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_sum_f64_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_sum_f64_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_sum_f64_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_sum_f64_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_sum_f64_sve(x, n);
    #elif defined(__aarch64__) && defined(__ARM_NEON)
        return simd_sum_f64_neon(x, n);
    #else
        double s = 0.0; for (size_t i=0;i<n;++i) s += x[i]; return s;
    #endif
}
// --------------------------------------------------------------------------------

static inline double simd_min_f64(const double* x, size_t n) {
    #if defined(__AVX512F__)
        return simd_min_f64_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_min_f64_avx2(x, n);
    #elif defined(__AVX__)
        return simd_min_f64_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_min_f64_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_min_f64_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_min_f64_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_min_f64_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_min_f64_sve(x, n);
    #elif defined(__aarch64__) && defined(__ARM_NEON)
        return simd_min_f64_neon(x, n);
    #else
        double m = x[0]; for (size_t i=1;i<n;++i) if (x[i]<m) m=x[i]; return m;
    #endif
}
// --------------------------------------------------------------------------------

static inline double simd_max_f64(const double* x, size_t n) {
    #if defined(__AVX512F__)
        return simd_max_f64_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_max_f64_avx2(x, n);
    #elif defined(__AVX__)
        return simd_max_f64_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_max_f64_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_max_f64_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_max_f64_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_max_f64_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_max_f64_sve(x, n);
    #elif defined(__aarch64__) && defined(__ARM_NEON)
        return simd_max_f64_neon(x, n);
    #else
        double m = x[0]; for (size_t i=1;i<n;++i) if (x[i]>m) m=x[i]; return m;
    #endif
}
// --------------------------------------------------------------------------------

static inline double simd_dot_f64(const double* a, const double* b, size_t n) {
    #if defined(__AVX512F__)
        return simd_dot_f64_avx512(a, b, n);
    #elif defined(__AVX2__)
        return simd_dot_f64_avx2(a, b, n);
    #elif defined(__AVX__)
        return simd_dot_f64_avx(a, b, n);
    #elif defined(__SSE4_1__)
        return simd_dot_f64_sse41(a, b, n);
    #elif defined(__SSE3__)
        return simd_dot_f64_sse3(a, b, n);
    #elif defined(__SSE2__)
        return simd_dot_f64_sse(a, b, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_dot_f64_sve2(a, b, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_dot_f64_sve(a, b, n);
    #elif defined(__aarch64__) && defined(__ARM_NEON)
        return simd_dot_f64_neon(a, b, n);
    #else
        double s = 0.0; for (size_t i=0;i<n;++i) s += a[i]*b[i]; return s;
    #endif
}
// --------------------------------------------------------------------------------

static inline double simd_mean_f64(const double* x, size_t n) {
    if (n == 0) return 0.0;
    #if defined(__AVX512F__)
        return simd_mean_f64_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_mean_f64_avx2(x, n);
    #elif defined(__AVX__)
        return simd_mean_f64_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_mean_f64_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_mean_f64_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_mean_f64_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_mean_f64_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_mean_f64_sve(x, n);
    #elif defined(__aarch64__) && defined(__ARM_NEON)
        return simd_mean_f64_neon(x, n);
    #else
        double s = 0.0; for (size_t i=0;i<n;++i) s += x[i]; return s / (double)n;
    #endif
}
// --------------------------------------------------------------------------------

static inline double simd_stdev_f64(const double* x, size_t n) {
    if (n < 2) return 0.0;
    #if defined(__AVX512F__)
        return simd_stdev_f64_avx512(x, n);
    #elif defined(__AVX2__)
        return simd_stdev_f64_avx2(x, n);
    #elif defined(__AVX__)
        return simd_stdev_f64_avx(x, n);
    #elif defined(__SSE4_1__)
        return simd_stdev_f64_sse41(x, n);
    #elif defined(__SSE3__)
        return simd_stdev_f64_sse3(x, n);
    #elif defined(__SSE2__)
        return simd_stdev_f64_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_stdev_f64_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_stdev_f64_sve(x, n);
    #elif defined(__aarch64__) && defined(__ARM_NEON)
        return simd_stdev_f64_neon(x, n);
    #else
        double mean = 0.0;
        for (size_t i=0;i<n;++i) mean += x[i];
        mean /= (double)n;
        double ss = 0.0;
        for (size_t i=0;i<n;++i) { double d = x[i] - mean; ss += d*d; }
        return sqrt(ss / (double)n);  // population stdev
    #endif
}


// ================================================================================ 
// ================================================================================ 

double_v* init_double_vector(size_t buff) {
    if (buff == 0) {
        errno = EINVAL;
        return NULL;
    }
    double_v* struct_ptr = malloc(sizeof(double_v));
    if (struct_ptr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
   
    double* data_ptr = calloc(buff, sizeof(double));
    if (data_ptr == NULL) {
        free(struct_ptr);
        errno = ENOMEM;
        return NULL; 
    }
   
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    struct_ptr->alloc_type = DYNAMIC;
    return struct_ptr;
}
// -------------------------------------------------------------------------------- 

double* c_double_ptr(double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    return vec->data;
}
// --------------------------------------------------------------------------------

void free_double_vector(double_v* vec) {
   if (!vec || !vec->alloc_type || vec->alloc_type == STATIC) {
       errno = EINVAL;
       return;
   }
   if (vec->data) free(vec->data);
   free(vec);
}
// --------------------------------------------------------------------------------

void _free_double_vector(double_v** vec) {
    if (vec && *vec) {
        free_double_vector(*vec);
        *vec = NULL;
    }
}
// -------------------------------------------------------------------------------- 

bool push_back_double_vector(double_v* vec, const double value) {
    if (vec == NULL|| vec->data == NULL) {
        errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            errno = EINVAL;
            return false;
        }
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        // Allocate more space for the array of str structs
        double* new_data = realloc(vec->data, new_alloc * sizeof(double));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(double));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    vec->data[vec->len] = value; 
    vec->len++;
   
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_double_vector(double_v* vec, const double value) {
    if (vec == NULL || vec->data == NULL) {
        errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            errno = EINVAL;
            return false;
        }
        
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
        
        // Check for size_t overflow
        if (new_alloc > SIZE_MAX / sizeof(double)) {
            errno = ERANGE;
            return false;
        }
       
        double* new_data = realloc(vec->data, new_alloc * sizeof(double));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(double));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Check for length overflow
    if (vec->len > SIZE_MAX - 1) {
        errno = ERANGE;
        return false;
    }
    
    // Move existing elements right if there are any
    if (vec->len > 0) {
        memmove(vec->data + 1, vec->data, vec->len * sizeof(double));
    }
    
    vec->data[0] = value;    
    vec->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_double_vector(double_v* vec, double value, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }
    if (vec->len >= SIZE_MAX) {  // Check length overflow
        errno = ERANGE;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            errno = EINVAL;
            return false;
        }
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
        
        // Check allocation size overflow
        if (new_alloc > SIZE_MAX / sizeof(double)) {
            errno = ERANGE;
            return false;
        }
       
        double* new_data = realloc(vec->data, new_alloc * sizeof(double));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(double));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    
    // Move existing elements right
    if (index < vec->len) {  // Only move if not appending
        // Check for size_t overflow in move operation
        if (vec->len - index > SIZE_MAX - 1) {
            errno = ERANGE;
            return false;
        }
        memmove(vec->data + index + 1, vec->data + index, 
                (vec->len - index) * sizeof(double));
    }
    
    vec->data[index] = value;
    vec->len++;
    return true;
}
// -------------------------------------------------------------------------------- 


double pop_back_double_vector(double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return DBL_MAX;
    }
    
    // Create copy of last element
    double temp = vec->data[vec->len - 1];
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(double));
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

double pop_front_double_vector(double_v* vec) {  // Fixed function name
    if (!vec || !vec->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return DBL_MAX;
    }
   
    // Check for overflow in memmove size calculation
    if (vec->len > SIZE_MAX / sizeof(double)) {
        errno = ERANGE;
        return DBL_MAX;
    }
   
    // Create copy of first element
    double temp = vec->data[0];
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(double));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(double));
   
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

double pop_any_double_vector(double_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return DBL_MAX;
    }
    
    if (index >= vec->len) {
        errno = ERANGE;
        return DBL_MAX;
    }
    
    // Create copy of element to pop
    double temp = vec->data[index];
    
    // If not the last element, shift remaining elements left
    if (index < vec->len - 1) {
        // Check for overflow in memmove size calculation
        if ((vec->len - index - 1) > SIZE_MAX / sizeof(double)) {
            errno = ERANGE;
            return DBL_MAX;
        }
        
        memmove(&vec->data[index], &vec->data[index + 1], 
                (vec->len - index - 1) * sizeof(double));
    }
   
    // Clear the last element
    memset(&vec->data[vec->len - 1], 0, sizeof(double));
    
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

double double_vector_index(const double_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return DBL_MAX;
    }
    return vec->data[index];
}
// // --------------------------------------------------------------------------------
//
size_t double_vector_size(const double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t double_vector_alloc(const double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->alloc;
}
// --------------------------------------------------------------------------------

void reverse_double_vector(double_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }

    if (vec->len == 0) {
        errno = ENODATA;
        return;
    }

    size_t i = 0;
    size_t j = vec->len - 1;
    while (i < j) {
       swap_double(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// ================================================================================
// ================================================================================ 
// QUICKSORT

void swap_double(double* a, double* b) {
    if (!a || !b) {
        errno = EINVAL;
        return;
    }
    double temp = *a;
    *a = *b;
    *b = temp;
}
// -------------------------------------------------------------------------------- 

static double* _median_of_three(double* a, double* b, double* c, iter_dir direction) {
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

static void _insertion_sort(double* vec, int low, int high, iter_dir direction) {
    for (int i = low + 1; i <= high; i++) {
        double key = vec[i];
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

static int _partition_double(double* vec, int low, int high, iter_dir direction) {
    int mid = low + (high - low) / 2;
    double* pivot_ptr = _median_of_three(&vec[low], &vec[mid], &vec[high], direction);
    
    if (pivot_ptr != &vec[high])
        swap_double(pivot_ptr, &vec[high]);
    
    double pivot = vec[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_double(&vec[i], &vec[j]);
        }
    }
    swap_double(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// -------------------------------------------------------------------------------- 

static void _quicksort_double(double* vec, int low, int high, iter_dir direction) {
    while (low < high) {
        if (high - low < 10) {
            _insertion_sort(vec, low, high, direction);
            break;
        }
        
        int pi = _partition_double(vec, low, high, direction);
        
        if (pi - low < high - pi) {
            _quicksort_double(vec, low, pi - 1, direction);
            low = pi + 1;
        } else {
            _quicksort_double(vec, pi + 1, high, direction);
            high = pi - 1;
        }
    }
}
// -------------------------------------------------------------------------------- 

void sort_double_vector(double_v* vec, iter_dir direction) {
    if (!vec) {
        errno = EINVAL;
        return;
    }
    if (vec->len < 2) return;
    
    _quicksort_double(vec->data, 0, vec->len - 1, direction);
}
// -------------------------------------------------------------------------------- 

void trim_double_vector(double_v* vec) {
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
    if (vec->len > SIZE_MAX / sizeof(double)) {
        errno = ERANGE;
        return;
    }
    
    double* ptr = realloc(vec->data, sizeof(double) * vec->len);
    if (ptr == NULL) {
        errno = ENOMEM;
        return;
    }
    
    vec->data = ptr;
    vec->alloc = vec->len;
}
// --------------------------------------------------------------------------------

size_t binary_search_double_vector(double_v* vec, double value, double tolerance, bool sort_first) {
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
        sort_double_vector(vec, FORWARD);
    }
    
    size_t left = 0;
    size_t right = vec->len - 1;
    
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        double diff = vec->data[mid] - value;
        
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

bin_dat binary_search_bounds_double_vector(double_v *vec,
                                           double value,
                                           double tolerance,
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
        sort_double_vector(vec, FORWARD);
    }

    size_t left = 0;
    size_t right = vec->len - 1;

    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        float diff = vec->data[mid] - value;

        // Within tolerance => treat as exact hit; bounds collapse to mid
        if (fabs(diff) <= tolerance) {
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

void update_double_vector(double_v* vec, size_t index, double replacement_value) {
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

double min_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return DBL_MAX;
    }
    return simd_min_f64(vec->data, vec->len);
}
// -------------------------------------------------------------------------------- 

double max_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return -DBL_MAX;
    }
    return simd_max_f64(vec->data, vec->len);
}
// -------------------------------------------------------------------------------- 

double sum_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return DBL_MAX;
    }
    return simd_sum_f64(vec->data, vec->len);
}
// -------------------------------------------------------------------------------- 

double average_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return DBL_MAX;
    }

    double sum = sum_double_vector(vec);
    if (errno != 0) return DBL_MAX;
    return sum / vec->len;
}

// -------------------------------------------------------------------------------- 

double stdev_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len < 2) {
        errno = ENODATA;
        return DBL_MAX;
    }
    return simd_stdev_f64(vec->data, vec->len);
}
// -------------------------------------------------------------------------------- 

double_v* cum_sum_double_vector(double_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return NULL;
    }

    double_v* new_vec = init_double_vector(vec->len);
    if (!new_vec) {
        errno = ENOMEM;
        return NULL;
    }

    double sum = 0.0f;
    for (size_t i = 0; i < vec->len; ++i) {
        double val = vec->data[i];
        if (isnan(val)) {
            errno = EINVAL;
            free_double_vector(new_vec);
            return NULL;
        }

        sum += val;

        if (isinf(sum)) {
            // Fill rest with infinity
            for (; i < vec->len; ++i) {
                push_back_double_vector(new_vec, INFINITY);
            }
            return new_vec;
        }

        if (!push_back_double_vector(new_vec, sum)) {
            free_double_vector(new_vec);
            return NULL;
        }
    }

    return new_vec;
}
// -------------------------------------------------------------------------------- 

double_v* copy_double_vector(const double_v* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    double_v* copy = init_double_vector(original->alloc);
    if (!copy) {
        return NULL;
    }

    for (size_t i = 0; i < original->len; ++i) {
        if (!push_back_double_vector(copy, original->data[i])) {
            free_double_vector(copy);
            return NULL;
        }
    }

    return copy;
}
// -------------------------------------------------------------------------------- 

double dot_double(const double* a, const double* b, size_t len) {
    errno = 0;

    if (!a || !b || len == 0) {
        errno = EINVAL;
        return DBL_MAX;
    }
    return simd_dot_f64(a, b, len);
}

// -------------------------------------------------------------------------------- 

double dot_double_vector(const double_v* vec1, const double_v* vec2) {
    errno = 0;
    if (!vec1 || !vec2 || !vec1->data || !vec2->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
    if (vec1->len == 0) {
        errno = ENODATA;
        return DBL_MAX;
    }
    if (vec1->len != vec2->len) {
        errno = ERANGE;
        return DBL_MAX;
    }
    return dot_double(vec1->data, vec2->data, vec1->len);
}
// -------------------------------------------------------------------------------- 

bool cross_double(const double* a, const double* b, double* result) {
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

double_v* cross_double_vector(const double_v* vec1, const double_v* vec2) {
    errno = 0;

    if (!vec1 || !vec2 || !vec1->data || !vec2->data) {
        errno = EINVAL;
        return NULL;
    }

    if (vec1->len < 3 || vec2->len < 3) {
        errno = ERANGE;
        return NULL;
    }

    double_v* result = init_double_vector(3);
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
// ================================================================================ 
// ================================================================================ 

// DICTIONARY IMPLEMENTATION

typedef struct ddictNode {
    char* key;
    double value;
    struct ddictNode* next;
} ddictNode;
// --------------------------------------------------------------------------------

struct dict_d {
    ddictNode* keyValues;
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
static bool resize_dict(dict_d* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    ddictNode* new_table = calloc(new_size, sizeof(ddictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table in case we need to recover
    ddictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash existing entries
    for (size_t i = 0; i < old_size; i++) {
        ddictNode* current = old_table[i].next;
        
        while (current) {
            ddictNode* next = current->next;  // Save next pointer before modifying node

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
            ddictNode* current = new_table[i].next;
            while (current) {
                ddictNode* next = current->next;
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

dict_d* init_double_dict(void) {
    // Allocate the dictionary structure
    dict_d* dict = calloc(1, sizeof(dict_d));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(ddictNode));
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

bool insert_double_dict(dict_d* dict, const char* key, double value) {
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
    for (ddictNode* current = dict->keyValues[index].next; current; current = current->next) {
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

    ddictNode* new_node = malloc(sizeof(ddictNode));
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

double pop_double_dict(dict_d* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    ddictNode* prev = &dict->keyValues[index];
    ddictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Save value and unlink node
            double value = current->value;
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

double get_double_dict_value(const dict_d* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const ddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return FLT_MAX;
}
// --------------------------------------------------------------------------------

void free_double_dict(dict_d* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        ddictNode* current = dict->keyValues[i].next;
        while (current) {      
            ddictNode* next = current->next;  // Save next pointer before freeing
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

void _free_double_dict(dict_d** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_double_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// --------------------------------------------------------------------------------

bool update_double_dict(dict_d* dict, const char* key, double value) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (ddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return true;
        }
    }

    errno = ENOENT;  // More specific error code for missing key
    return false;
}
// --------------------------------------------------------------------------------

size_t double_dict_size(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t double_dict_alloc(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// --------------------------------------------------------------------------------

size_t double_dict_hash_size(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

bool has_key_double_dict(const dict_d* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const ddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

dict_d* copy_double_dict(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with same capacity
    dict_d* new_dict = malloc(sizeof(dict_d));
    if (!new_dict) {
        errno = ENOMEM;
        return NULL;
    }

    new_dict->keyValues = calloc(dict->alloc, sizeof(ddictNode));
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
        ddictNode* current = dict->keyValues[i].next;
        while (current) {
            // Insert will handle incrementing hash_size and len
            if (!insert_double_dict(new_dict, current->key, current->value)) {
                free_double_dict(new_dict);  // Clean up on failure
                return NULL;
            }
            current = current->next;
        }
    }

    return new_dict;
}
// -------------------------------------------------------------------------------- 

bool clear_double_dict(dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return false;
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        ddictNode* current = dict->keyValues[i].next;
        while (current) {
            ddictNode* next = current->next;
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

string_v* get_keys_double_dict(const dict_d* dict) {
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
        for (ddictNode* current = dict->keyValues[i].next; current; current = current->next) {
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

double_v* get_values_double_dict(const dict_d* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }
    double_v* vec = init_double_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }
    // Iterate through all buckets
    for (size_t i = 0; i < dict->alloc; i++) {
        for (ddictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_double_vector(vec, current->value)) {
                free_double_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }
    return vec;
}
// -------------------------------------------------------------------------------- 

dict_d* merge_double_dict(const dict_d* dict1, const dict_d* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with capacity for all items
    //size_t initial_size = dict1->hash_size + dict2->hash_size;
    dict_d* merged = init_double_dict();
    if (!merged) {
        return NULL;  // errno set by init_double_dict
    }

    // First, copy all entries from dict1
    for (size_t i = 0; i < dict1->alloc; i++) {
        for (ddictNode* current = dict1->keyValues[i].next; current; current = current->next) {
            if (!insert_double_dict(merged, current->key, current->value)) {
                free_double_dict(merged);
                return NULL;
            }
        }
    }

    // Then handle dict2 entries
    for (size_t i = 0; i < dict2->alloc; i++) {
        for (ddictNode* current = dict2->keyValues[i].next; current; current = current->next) {
            double existing_value;
            // Check if key exists in merged dictionary
            if ((existing_value = get_double_dict_value(merged, current->key)) != FLT_MAX) {
                if (overwrite) {
                    // Update existing value if overwrite is true
                    if (!update_double_dict(merged, current->key, current->value)) {
                        free_double_dict(merged);
                        return NULL;
                    }
                }
                // If overwrite is false, keep original value
            } else {
                // Key doesn't exist, insert new entry
                if (!insert_double_dict(merged, current->key, current->value)) {
                    free_double_dict(merged);
                    return NULL;
                }
            }
        }
    }

    return merged;
}
// --------------------------------------------------------------------------------

bool foreach_double_dict(const dict_d* dict, ddict_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; i++) {
        for (ddictNode* current = dict->keyValues[i].next; current; current = current->next) {
            iter(current->key, current->value, user_data);
        }
    }

    return true;
}
// ================================================================================ 
// ================================================================================ 

typedef struct dvdictNode {
    char* key;
    double_v* value;
    struct dvdictNode* next;
} dvdictNode;
// --------------------------------------------------------------------------------

struct dict_dv {
    dvdictNode* keyValues;
    size_t hash_size;
    size_t len;
    size_t alloc;
};
// --------------------------------------------------------------------------------

dict_dv* init_doublev_dict(void) {
    // Allocate the dictionary structure
    dict_dv* dict = calloc(1, sizeof(dict_d));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate vector dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(dvdictNode));
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

static bool resize_dictv(dict_dv* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better hash distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    dvdictNode* new_table = calloc(new_size, sizeof(dvdictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table for cleanup if something fails
    dvdictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash all existing entries into the new table
    for (size_t i = 0; i < old_size; ++i) {
        dvdictNode* current = old_table[i].next;
        while (current) {
            dvdictNode* next = current->next;

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
            dvdictNode* current = new_table[i].next;
            while (current) {
                dvdictNode* next = current->next;
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

bool create_doublev_dict(dict_dv* dict, char* key, size_t size) {
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
    for (dvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
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

    dvdictNode* new_node = malloc(sizeof(dvdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    double_v* value = NULL;
    value = init_double_vector(size);
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

bool pop_doublev_dict(dict_dv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    dvdictNode* prev = &dict->keyValues[index];
    dvdictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            prev->next = current->next;
            
            // Update dictionary metadata
            dict->hash_size--;
            if (!prev->next) {  // If bucket is now empty
                dict->len--;
            }
            
            // Clean up node memory
            free_double_vector(current->value);
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

double_v* return_doublev_pointer(dict_dv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return NULL;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const dvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return NULL;
}
// -------------------------------------------------------------------------------- 

void free_doublev_dict(dict_dv* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        dvdictNode* current = dict->keyValues[i].next;
        while (current) {      
            dvdictNode* next = current->next;

            free_double_vector(current->value);
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

void _free_doublev_dict(dict_dv** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_doublev_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// -------------------------------------------------------------------------------- 

bool has_key_doublev_dict(const dict_dv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const dvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

bool insert_doublev_dict(dict_dv* dict, const char* key, double_v* value) {
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
    for (dvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
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
    dvdictNode* new_node = malloc(sizeof(dvdictNode));
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

size_t double_dictv_size(const dict_dv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t double_dictv_alloc(const dict_dv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// -------------------------------------------------------------------------------- 

size_t double_dictv_hash_size(const dict_dv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

dict_dv* copy_doublev_dict(const dict_dv* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    dict_dv* copy = init_doublev_dict();
    if (!copy) {
        return NULL;  // errno already set
    }

    for (size_t i = 0; i < original->alloc; ++i) {
        dvdictNode* current = original->keyValues[i].next;
        while (current) {
            // if (current->value->alloc_type != DYNAMIC) {
            //     free_doublev_dict(copy);
            //     errno = EPERM;
            //     return NULL;
            // }

            double_v* vec_copy = copy_double_vector(current->value);
            if (!vec_copy) {
                free_doublev_dict(copy);
                return NULL;
            }

            if (!insert_doublev_dict(copy, current->key, vec_copy)) {
                free_double_vector(vec_copy);
                free_doublev_dict(copy);
                return NULL;
            }

            current = current->next;
        }
    }

    return copy;
}
// -------------------------------------------------------------------------------- 

dict_dv* merge_doublev_dict(const dict_dv* dict1, const dict_dv* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Start by deep copying dict1
    dict_dv* merged = copy_doublev_dict(dict1);
    if (!merged) {
        return NULL;
    }

    // Now process dict2 entries
    for (size_t i = 0; i < dict2->alloc; ++i) {
        dvdictNode* current = dict2->keyValues[i].next;
        while (current) {
            if (!current->key || !current->value || current->value->alloc_type != DYNAMIC) {
                free_doublev_dict(merged);
                errno = EPERM;
                return NULL;
            }

            bool exists = has_key_doublev_dict(merged, current->key);
            if (exists && !overwrite) {
                current = current->next;
                continue;
            }

            double_v* vec_copy = copy_double_vector(current->value);
            if (!vec_copy) {
                free_doublev_dict(merged);
                return NULL; // errno set by copy_double_vector
            }

            if (exists) {
                pop_doublev_dict(merged, current->key);
            }

            if (!insert_doublev_dict(merged, current->key, vec_copy)) {
                free_double_vector(vec_copy);
                free_doublev_dict(merged);
                return NULL;
            }

            current = current->next;
        }
    }

    return merged;
}
// -------------------------------------------------------------------------------- 

void clear_doublev_dict(dict_dv* dict) {
    if (!dict) {
        errno = EINVAL;
        return;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        dvdictNode* current = dict->keyValues[i].next;
        dict->keyValues[i].next = NULL;

        while (current) {
            dvdictNode* next = current->next;

            if (current->value) {
                if (current->value->alloc_type == STATIC) {
                    free(current->value);
                } else {
                    free_double_vector(current->value);
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

bool foreach_doublev_dict(const dict_dv* dict, dict_dv_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        dvdictNode* current = dict->keyValues[i].next;
        while (current) {
            iter(current->key, current->value, user_data);
            current = current->next;
        }
    }

    return true;
}
// -------------------------------------------------------------------------------- 

string_v* get_keys_doublev_dict(const dict_dv* dict) {
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
        for (dvdictNode* current = dict->keyValues[i].next; current; current = current->next) {
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

matrix_d* create_double_dense_matrix(size_t rows, size_t cols) {
    errno = 0;

    matrix_d* mat = malloc(sizeof(matrix_d));
    if (!mat) {
        errno = ENOMEM;
        return NULL;
    }

    double* data_ptr = calloc(rows * cols, sizeof(double));
    if (!data_ptr) {
        errno = ENOMEM;
        free(mat);
        return NULL;
    }

    uint8_t* init_ptr = calloc(rows * cols, sizeof(uint8_t));
    if (!init_ptr) {
        errno = ENOMEM;
        free(data_ptr);
        free(mat);
        return NULL;
    }

    mat->type = DENSE_MATRIX;
    mat->storage.dense.data = data_ptr;
    mat->storage.dense.init = init_ptr;
    mat->rows = rows;
    mat->cols = cols;
    mat->count = 0;

    return mat;
}
// -------------------------------------------------------------------------------- 

matrix_d* create_double_coo_matrix(size_t rows, size_t cols) {
    errno = 0;

    matrix_d* mat = malloc(sizeof(matrix_d));
    if (!mat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t* row_indices = malloc(INITIAL_COO_CAPACITY * sizeof(size_t));
    size_t* col_indices = malloc(INITIAL_COO_CAPACITY * sizeof(size_t));
    double* values = malloc(INITIAL_COO_CAPACITY * sizeof(double));

    if (!row_indices || !col_indices || !values) {
        errno = ENOMEM;
        free(row_indices);
        free(col_indices);
        free(values);
        free(mat);
        return NULL;
    }

    mat->type = SPARSE_COO_MATRIX;
    mat->storage.coo.rows = row_indices;
    mat->storage.coo.cols = col_indices;
    mat->storage.coo.values = values;
    mat->storage.coo.capacity = INITIAL_COO_CAPACITY;

    mat->rows = rows;
    mat->cols = cols;
    mat->count = 0;

    return mat;
}
// -------------------------------------------------------------------------------- 

matrix_d* create_double_csr_matrix(size_t rows, size_t cols, size_t nnz) {
    errno = 0;

    matrix_d* mat = malloc(sizeof(matrix_d));
    if (!mat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t* row_ptrs = calloc(rows + 1, sizeof(size_t));
    size_t* col_indices = malloc(nnz * sizeof(size_t));
    double* values = malloc(nnz * sizeof(double));

    if (!row_ptrs || !col_indices || !values) {
        errno = ENOMEM;
        free(row_ptrs);
        free(col_indices);
        free(values);
        free(mat);
        return NULL;
    }

    mat->type = SPARSE_CSR_MATRIX;
    mat->storage.csr.row_ptrs = row_ptrs;
    mat->storage.csr.col_indices = col_indices;
    mat->storage.csr.values = values;

    mat->rows = rows;
    mat->cols = cols;
    mat->count = nnz;

    return mat;
}
// -------------------------------------------------------------------------------- 

matrix_d* create_double_matrix(size_t rows, size_t cols, size_t estimated_nnz) {
    errno = 0;

    size_t total = rows * cols;

    if (total < MIN_SPARSE_SIZE) {
        return create_double_dense_matrix(rows, cols);
    }

    double estimated_density = (double)estimated_nnz / total;

    if (estimated_density < DENSE_THRESHOLD) {
        // Very sparse → Start with COO (can convert to CSR later)
        return create_double_coo_matrix(rows, cols);
    } else {
        // Semi-sparse or dense enough → Start dense
        return create_double_dense_matrix(rows, cols);
    }
}
// -------------------------------------------------------------------------------- 

void free_double_matrix(matrix_d* mat) {
    if (!mat) return;

    switch (mat->type) {
        case DENSE_MATRIX:
            free_double_dense_matrix(mat);
            break;

        case SPARSE_CSR_MATRIX:
            free_double_csr_matrix(mat);
            break;

        case SPARSE_COO_MATRIX:
            free_double_coo_matrix(mat);
            break;

        default:
            // Unknown or uninitialized type
            break;
    }
}
// -------------------------------------------------------------------------------- 

void _free_double_matrix(matrix_d** mat) {
    if (mat && *mat) {
        free_double_matrix(*mat);
        *mat = NULL;
    }
}
// -------------------------------------------------------------------------------- 

void free_double_dense_matrix(matrix_d* mat) {
    if (!mat || mat->type != DENSE_MATRIX) {
        errno = EINVAL;
        return;
    }
    if (mat->storage.dense.data) free(mat->storage.dense.data);
    if (mat->storage.dense.init) free(mat->storage.dense.init);
    mat->type = 0;
    mat->rows = 0;
    mat->cols = 0;
    mat->count = 0;
    // Finally free the structure itself
    free(mat);
}
// -------------------------------------------------------------------------------- 

void free_double_csr_matrix(matrix_d* mat) {
    errno = 0;
    if (!mat || mat->type != SPARSE_CSR_MATRIX) {
        errno = EINVAL;
        return;
    }
    if (mat->storage.csr.row_ptrs) free(mat->storage.csr.row_ptrs);
    if (mat->storage.csr.col_indices) free(mat->storage.csr.col_indices);
    if (mat->storage.csr.values) free(mat->storage.csr.values);
    mat->type = 0;
    mat->rows = 0;
    mat->cols = 0;
    mat->count = 0;
    // Finally free the structure itself
    free(mat);
}
// --------------------------------------------------------------------------------

void free_double_coo_matrix(matrix_d* mat) {
    errno = 0;
    if (!mat || mat->type != SPARSE_COO_MATRIX) {
        errno = EINVAL;
        return;
    }
    if (mat->storage.coo.rows)   free(mat->storage.coo.rows);
    if (mat->storage.coo.cols)   free(mat->storage.coo.cols);
    if (mat->storage.coo.values) free(mat->storage.coo.values); 
    mat->type = 0;
    mat->rows = 0;
    mat->cols = 0;
    mat->count = 0;
    // Finally free the structure itself
    free(mat);
}
// -------------------------------------------------------------------------------- 

bool is_double_element_initialized(const matrix_d* mat, size_t row, size_t col) {
    if (!mat) {
        errno = EINVAL;
        return false;
    }

    // Bounds checking
    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return false;
    }

    if (mat->count == 0)
        return false;

    switch (mat->type) {
        case DENSE_MATRIX:
            if (!mat->storage.dense.init) {
                errno = EINVAL;
                return false;
            }
            return mat->storage.dense.init[row * mat->cols + col] != 0;

        case SPARSE_COO_MATRIX: {
            for (size_t i = 0; i < mat->count; ++i) {
                if (mat->storage.coo.rows[i] == row &&
                    mat->storage.coo.cols[i] == col) {
                    return true;
                }
            }
            return false;
        }

        case SPARSE_CSR_MATRIX: {
            size_t row_start = mat->storage.csr.row_ptrs[row];
            size_t row_end = mat->storage.csr.row_ptrs[row + 1];
            for (size_t i = row_start; i < row_end; ++i) {
                if (mat->storage.csr.col_indices[i] == col) {
                    return true;
                }
            }
            return false;
        }

        default:
            errno = EINVAL;
            return false;
    }
}
// -------------------------------------------------------------------------------- 

static void set_element_initialized(matrix_d* mat, size_t row, size_t col, bool initialized) {
    // Parameter validation
    if (!mat || !mat->storage.dense.init) {
        errno = EINVAL;
        return;
    }
    
    // Bounds checking
    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return;
    }
    
    // Get current state
    bool was_initialized = mat->storage.dense.init[row * mat->cols + col] != 0;
    
    // Update initialization status
    mat->storage.dense.init[row * mat->cols + col] = initialized ? 1 : 0;
    
    // Update count if state changed
    if (initialized && !was_initialized) {
        mat->count++;
    } else if (!initialized && was_initialized) {
        mat->count--;
    }
}
// -------------------------------------------------------------------------------- 

static bool set_dense_matrix(matrix_d* mat, size_t row, size_t col, double value, bool allow_updates) {
    // Reset errno
    errno = 0;
    if (!mat || mat->type != DENSE_MATRIX) {
        errno = EINVAL;
        return false;
    }
    
    // Parameter validation
    if (!mat->storage.dense.data || !mat->storage.dense.init) {
        errno = EINVAL;
        return false;
    }
    
    // Bounds checking
    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return false;
    }
    
    // Check if already initialized
    bool initialized = is_double_element_initialized(mat, row, col);
    
    // Handle initialization rules
    if (initialized && !allow_updates) {
        errno = EEXIST;  // Element already exists
        return false;
    }
    
    // Set the value
    mat->storage.dense.data[row * mat->cols + col] = value;
    
    // Update initialization status if needed
    if (!initialized) {
        set_element_initialized(mat, row, col, true);
    }
    
    return true;
}
// -------------------------------------------------------------------------------- 

bool insert_double_dense_matrix(matrix_d* mat, size_t row, size_t col, double value) {
    return set_dense_matrix(mat, row, col, value, false);
}
// -------------------------------------------------------------------------------- 

bool update_double_dense_matrix(matrix_d* mat, size_t row, size_t col, double value) {
    return set_dense_matrix(mat, row, col, value, true);
}
// -------------------------------------------------------------------------------- 

static int compare_row_col(size_t r1, size_t c1, size_t r2, size_t c2) {
    if (r1 < r2) return -1;
    if (r1 > r2) return 1;
    if (c1 < c2) return -1;
    if (c1 > c2) return 1;
    return 0;
}

static bool set_coo_matrix(matrix_d* mat, size_t row, size_t col, double value, bool allow_updates) {
    errno = 0;

    if (!mat || mat->type != SPARSE_COO_MATRIX || row >= mat->rows || col >= mat->cols) {
        errno = EINVAL;
        return false;
    }

    // Binary search for insert position
    size_t left = 0, right = mat->count;
    while (left < right) {
        size_t mid = (left + right) / 2;
        int cmp = compare_row_col(mat->storage.coo.rows[mid], mat->storage.coo.cols[mid], row, col);
        if (cmp < 0)
            left = mid + 1;
        else
            right = mid;
    }

    // Check for existing value
    if (left < mat->count &&
        mat->storage.coo.rows[left] == row &&
        mat->storage.coo.cols[left] == col) {
        if (!allow_updates) {
            errno = EEXIST;
            return false;
        }
        mat->storage.coo.values[left] = value;
        return true;
    }

    // Reallocate if needed
    if (mat->count >= mat->storage.coo.capacity) {
        size_t new_capacity = mat->storage.coo.capacity == 0 ? 1 : mat->storage.coo.capacity;
        new_capacity = (new_capacity < VEC_THRESHOLD) ? new_capacity * 2 : new_capacity + VEC_FIXED_AMOUNT;

        size_t* new_rows = realloc(mat->storage.coo.rows, new_capacity * sizeof(size_t));
        size_t* new_cols = realloc(mat->storage.coo.cols, new_capacity * sizeof(size_t));
        double*  new_vals = realloc(mat->storage.coo.values, new_capacity * sizeof(double));
        if (!new_rows || !new_cols || !new_vals) {
            errno = ENOMEM;
            return false;
        }
        mat->storage.coo.rows = new_rows;
        mat->storage.coo.cols = new_cols;
        mat->storage.coo.values = new_vals;
        mat->storage.coo.capacity = new_capacity;
    }

    // Shift data to make space
    memmove(&mat->storage.coo.rows[left + 1], &mat->storage.coo.rows[left], (mat->count - left) * sizeof(size_t));
    memmove(&mat->storage.coo.cols[left + 1], &mat->storage.coo.cols[left], (mat->count - left) * sizeof(size_t));
    memmove(&mat->storage.coo.values[left + 1], &mat->storage.coo.values[left], (mat->count - left) * sizeof(double));

    // Insert new entry
    mat->storage.coo.rows[left] = row;
    mat->storage.coo.cols[left] = col;
    mat->storage.coo.values[left] = value;
    mat->count++;

    return true;
}
// -------------------------------------------------------------------------------- 

bool insert_double_coo_matrix(matrix_d* mat, size_t row, size_t col, double value) {
    return set_coo_matrix(mat, row, col, value, false);
}
// -------------------------------------------------------------------------------- 

bool update_double_coo_matrix(matrix_d* mat, size_t row, size_t col, double value) {
    return set_coo_matrix(mat, row, col, value, true);
}
// -------------------------------------------------------------------------------- 

static int compare_size_t(const void* a, const void* b) {
    size_t lhs = *(const size_t*)a;
    size_t rhs = *(const size_t*)b;
    if (lhs < rhs) return -1;
    if (lhs > rhs) return 1;
    return 0;
}
// -------------------------------------------------------------------------------- 

static bool set_csr_matrix(matrix_d* mat, size_t row, size_t col, double value, bool allow_updates) {
    errno = 0;

    if (!mat || mat->type != SPARSE_CSR_MATRIX) {
        errno = EINVAL;
        return false;
    }

    if (!mat->storage.csr.row_ptrs || !mat->storage.csr.col_indices || !mat->storage.csr.values) {
        errno = EINVAL;
        return false;
    }

    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return false;
    }

    size_t start = mat->storage.csr.row_ptrs[row];
    size_t end   = mat->storage.csr.row_ptrs[row + 1];
    size_t count = end - start;

    size_t* found = bsearch(&col,
                            &mat->storage.csr.col_indices[start],
                            count,
                            sizeof(size_t),
                            compare_size_t);

    if (found) {
        if (!allow_updates) {
            errno = EEXIST;
            return false;
        }

        size_t index = found - mat->storage.csr.col_indices;
        mat->storage.csr.values[index] = value;
        return true;
    }

    errno = allow_updates ? ENOENT : EEXIST;
    return false;
}
// -------------------------------------------------------------------------------- 

bool insert_double_csr_matrix(matrix_d* mat, size_t row, size_t col, double value) {
    return set_csr_matrix(mat, row, col, value, false);
}
// -------------------------------------------------------------------------------- 

bool update_double_csr_matrix(matrix_d* mat, size_t row, size_t col, double value) {
    return set_csr_matrix(mat, row, col, value, true);
}
// -------------------------------------------------------------------------------- 

static matrix_d* convert_double_dense_to_coo(const matrix_d* dense_mat) {
    errno = 0;

    if (!dense_mat || dense_mat->type != DENSE_MATRIX ||
        !dense_mat->storage.dense.data || !dense_mat->storage.dense.init) {
        errno = EINVAL;
        return NULL;
    }

    size_t rows = dense_mat->rows;
    size_t cols = dense_mat->cols;
    size_t count = dense_mat->count;

    matrix_d* coo_mat = malloc(sizeof(matrix_d));
    if (!coo_mat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t* rows_arr = malloc(count * sizeof(size_t));
    size_t* cols_arr = malloc(count * sizeof(size_t));
    double* values_arr = malloc(count * sizeof(double));
    if (!rows_arr || !cols_arr || !values_arr) {
        free(rows_arr); free(cols_arr); free(values_arr); free(coo_mat);
        errno = ENOMEM;
        return NULL;
    }

    size_t k = 0;
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            size_t idx = i * cols + j;
            if (dense_mat->storage.dense.init[idx]) {
                rows_arr[k] = i;
                cols_arr[k] = j;
                values_arr[k] = dense_mat->storage.dense.data[idx];
                ++k;
            }
        }
    }

    coo_mat->type = SPARSE_COO_MATRIX;
    coo_mat->rows = rows;
    coo_mat->cols = cols;
    coo_mat->count = count;
    coo_mat->storage.coo.rows = rows_arr;
    coo_mat->storage.coo.cols = cols_arr;
    coo_mat->storage.coo.values = values_arr;
    coo_mat->storage.coo.capacity = count;

    return coo_mat;
}
// -------------------------------------------------------------------------------- 

static int compare_coo_entries(const void* a, const void* b) {
    const size_t* ia = (const size_t*)a;
    const size_t* ib = (const size_t*)b;
    // First sort by row, then column
    if (ia[0] != ib[0]) return (ia[0] < ib[0]) ? -1 : 1;
    return (ia[1] < ib[1]) ? -1 : (ia[1] > ib[1]);
}
// -------------------------------------------------------------------------------- 

static matrix_d* convert_double_coo_to_csr(const matrix_d* coo_mat) {
    errno = 0;

    if (!coo_mat || coo_mat->type != SPARSE_COO_MATRIX ||
        !coo_mat->storage.coo.rows || !coo_mat->storage.coo.cols || !coo_mat->storage.coo.values) {
        errno = EINVAL;
        return NULL;
    }

    size_t nnz = coo_mat->count;
    size_t rows = coo_mat->rows;
    size_t cols = coo_mat->cols;

    // Allocate CSR structures
    size_t* row_ptrs = calloc(rows + 1, sizeof(size_t));
    size_t* col_indices = malloc(nnz * sizeof(size_t));
    double* values = malloc(nnz * sizeof(double));
    if (!row_ptrs || !col_indices || !values) {
        free(row_ptrs); free(col_indices); free(values);
        errno = ENOMEM;
        return NULL;
    }

    // Create sortable array of (row, col, value)
    typedef struct {
        size_t row, col;
        double value;
    } entry_t;

    entry_t* entries = malloc(nnz * sizeof(entry_t));
    if (!entries) {
        free(row_ptrs); free(col_indices); free(values);
        errno = ENOMEM;
        return NULL;
    }

    for (size_t i = 0; i < nnz; ++i) {
        entries[i].row = coo_mat->storage.coo.rows[i];
        entries[i].col = coo_mat->storage.coo.cols[i];
        entries[i].value = coo_mat->storage.coo.values[i];
    }

    // Sort by row, then col
    qsort(entries, nnz, sizeof(entry_t), (int (*)(const void*, const void*))compare_coo_entries);

    // Fill CSR
    for (size_t i = 0; i < nnz; ++i) {
        row_ptrs[entries[i].row + 1]++;
        col_indices[i] = entries[i].col;
        values[i] = entries[i].value;
    }

    // Convert row counts to prefix sum
    for (size_t i = 1; i <= rows; ++i) {
        row_ptrs[i] += row_ptrs[i - 1];
    }

    matrix_d* csr_mat = malloc(sizeof(matrix_d));
    if (!csr_mat) {
        free(row_ptrs); free(col_indices); free(values); free(entries);
        errno = ENOMEM;
        return NULL;
    }

    csr_mat->type = SPARSE_CSR_MATRIX;
    csr_mat->rows = rows;
    csr_mat->cols = cols;
    csr_mat->count = nnz;
    csr_mat->storage.csr.row_ptrs = row_ptrs;
    csr_mat->storage.csr.col_indices = col_indices;
    csr_mat->storage.csr.values = values;

    free(entries);
    return csr_mat;
}
// -------------------------------------------------------------------------------- 
void maybe_convert_double_matrix(matrix_d** pmat, bool convert_to_csr) {
    matrix_d* mat = *pmat;

    size_t total = mat->rows * mat->cols;

    // If dense and large or sparse → convert to COO
    if (mat->type == DENSE_MATRIX) {
        double density = (double)mat->count / total;
        if (total > MAX_DENSE_ELEMENTS && density < SPARSE_THRESHOLD) {
            matrix_d* new_mat = convert_double_dense_to_coo(mat);
            if (new_mat) {
                free_double_matrix(mat);
                *pmat = new_mat;
            }
        }
    }
    // If COO and too large → convert to CSR
    else if (mat->type == SPARSE_COO_MATRIX && convert_to_csr) {
        if (mat->count >= COO_TO_CSR_TRIGGER) {
            matrix_d* new_mat = convert_double_coo_to_csr(mat);
            if (new_mat) {
                free_double_matrix(mat);
                *pmat = new_mat;
            }
        }
    }
    // CSR doesn't auto-convert
}
// -------------------------------------------------------------------------------- 

bool insert_double_matrix(matrix_d** pmat, size_t row, size_t col, double value,
                         bool convert_to_csr) {
    if (!pmat || !*pmat) {
        errno = EINVAL;
        return false;
    }

    maybe_convert_double_matrix(pmat, convert_to_csr);  // Can replace *pmat with a new matrix
    matrix_d* mat = *pmat;

    switch (mat->type) {
        case DENSE_MATRIX:
            return insert_double_dense_matrix(mat, row, col, value);
        case SPARSE_COO_MATRIX:
            return insert_double_coo_matrix(mat, row, col, value);
        case SPARSE_CSR_MATRIX:
            return insert_double_csr_matrix(mat, row, col, value);
        default:
            errno = EINVAL;
            return false;
    }
}
// -------------------------------------------------------------------------------- 

bool update_double_matrix(matrix_d* mat, size_t row, size_t col, double value) {
    errno = 0;
    if (!mat) {
        errno = EINVAL;
        return false;
    }

    switch (mat->type) {
        case DENSE_MATRIX:
            return update_double_dense_matrix(mat, row, col, value);
        case SPARSE_COO_MATRIX:
            return update_double_coo_matrix(mat, row, col, value);
        case SPARSE_CSR_MATRIX:
            return update_double_csr_matrix(mat, row, col, value);
        default:
            errno = EINVAL;
            return false;
    }
}
// -------------------------------------------------------------------------------- 

double get_double_dense_matrix(const matrix_d* mat, size_t row, size_t col) {
    errno = 0;
    if (!mat || !mat->storage.dense.data || !mat->storage.dense.init || mat->type != DENSE_MATRIX) {
        errno = EINVAL;
        return FLT_MAX;
    }

    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return FLT_MAX;
    }

    if (!is_double_element_initialized(mat, row, col)) {
        errno = EINVAL;
        return FLT_MAX;
    }

    return mat->storage.dense.data[row * mat->cols + col];
}
// -------------------------------------------------------------------------------- 

double get_double_coo_matrix(const matrix_d* mat, size_t row, size_t col) {
    errno = 0;

    if (!mat || mat->type != SPARSE_COO_MATRIX ||
        !mat->storage.coo.rows || !mat->storage.coo.cols || !mat->storage.coo.values) {
        errno = EINVAL;
        return FLT_MAX;
    }

    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return FLT_MAX;
    }

    for (size_t i = 0; i < mat->count; ++i) {
        if (mat->storage.coo.rows[i] == row &&
            mat->storage.coo.cols[i] == col) {
            return mat->storage.coo.values[i];
        }
    }

    // Element not found
    errno = ENOENT;
    return FLT_MAX;
}
// -------------------------------------------------------------------------------- 

double get_double_csr_matrix(const matrix_d* mat, size_t row, size_t col) {
    errno = 0;

    if (!mat || mat->type != SPARSE_CSR_MATRIX ||
        !mat->storage.csr.row_ptrs || !mat->storage.csr.col_indices || !mat->storage.csr.values) {
        errno = EINVAL;
        return FLT_MAX;
    }

    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return FLT_MAX;
    }

    size_t start = mat->storage.csr.row_ptrs[row];
    size_t end   = mat->storage.csr.row_ptrs[row + 1];

    for (size_t i = start; i < end; ++i) {
        if (mat->storage.csr.col_indices[i] == col) {
            return mat->storage.csr.values[i];
        }
    }

    errno = ENOENT;
    return FLT_MAX;
}
// -------------------------------------------------------------------------------- 

double get_double_matrix(const matrix_d* mat, size_t row, size_t col) {
    errno = 0;
    if (!mat) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (mat->type == DENSE_MATRIX) return get_double_dense_matrix(mat, row, col);
    else if (mat->type == SPARSE_COO_MATRIX) return get_double_coo_matrix(mat, row, col);
    else if (mat->type == SPARSE_CSR_MATRIX) return get_double_csr_matrix(mat, row, col);
    else {
        errno = EINVAL;
        return FLT_MAX;
    }
}
// -------------------------------------------------------------------------------- 

double pop_double_dense_matrix(matrix_d* mat, size_t row, size_t col) {
    errno = 0;

    if (!mat || mat->type != DENSE_MATRIX ||
        !mat->storage.dense.data || !mat->storage.dense.init) {
        errno = EINVAL;
        return FLT_MAX;
    }

    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return FLT_MAX;
    }

    if (!is_double_element_initialized(mat, row, col)) {
        errno = ENOENT;
        return FLT_MAX;
    }

    size_t index = row * mat->cols + col;
    double value = mat->storage.dense.data[index];

    // Mark as uninitialized
    set_element_initialized(mat, row, col, false);
    mat->storage.dense.data[index] = 0.0f;  // Optional

    return value;
}
// -------------------------------------------------------------------------------- 

double pop_double_coo_matrix(matrix_d* mat, size_t row, size_t col) {
    errno = 0;

    if (!mat || mat->type != SPARSE_COO_MATRIX ||
        !mat->storage.coo.rows || !mat->storage.coo.cols || !mat->storage.coo.values) {
        errno = EINVAL;
        return FLT_MAX;
    }

    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return FLT_MAX;
    }

    size_t left = 0, right = mat->count;
    while (left < right) {
        size_t mid = (left + right) / 2;
        size_t mid_row = mat->storage.coo.rows[mid];
        size_t mid_col = mat->storage.coo.cols[mid];

        if (mid_row < row || (mid_row == row && mid_col < col)) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    if (left < mat->count &&
        mat->storage.coo.rows[left] == row &&
        mat->storage.coo.cols[left] == col) {

        double value = mat->storage.coo.values[left];
        size_t elements_to_move = mat->count - left - 1;

        if (elements_to_move > 0) {
            memmove(&mat->storage.coo.rows[left],
                    &mat->storage.coo.rows[left + 1],
                    elements_to_move * sizeof(size_t));

            memmove(&mat->storage.coo.cols[left],
                    &mat->storage.coo.cols[left + 1],
                    elements_to_move * sizeof(size_t));

            memmove(&mat->storage.coo.values[left],
                    &mat->storage.coo.values[left + 1],
                    elements_to_move * sizeof(double));
        }

        mat->count--;
        return value;
    }

    errno = ENOENT;
    return FLT_MAX;
}
// -------------------------------------------------------------------------------- 

static void maybe_compact_csr(matrix_d* mat, size_t original_nnz) {
    if (!mat || mat->type != SPARSE_CSR_MATRIX || original_nnz == 0) return;

    double tombstone_ratio = (double)(original_nnz - mat->count) / original_nnz;

    if (tombstone_ratio > CSR_COMPACT_THRESHOLD) {
        compact_double_csr_matrix(mat);
    }
}
// -------------------------------------------------------------------------------- 

double pop_double_csr_matrix(matrix_d* mat, size_t row, size_t col) {
    errno = 0;

    if (!mat || mat->type != SPARSE_CSR_MATRIX ||
        !mat->storage.csr.row_ptrs || !mat->storage.csr.col_indices || !mat->storage.csr.values) {
        errno = EINVAL;
        return FLT_MAX;
    }

    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return FLT_MAX;
    }

    size_t start = mat->storage.csr.row_ptrs[row];
    size_t end   = mat->storage.csr.row_ptrs[row + 1];
    size_t count = end - start;
    size_t* found = bsearch(&col,
                            &mat->storage.csr.col_indices[start],
                            count,
                            sizeof(size_t),
                            compare_size_t);

    if (!found) {
        errno = ENOENT;
        return FLT_MAX;
    }

    size_t index = found - mat->storage.csr.col_indices;
    double value = mat->storage.csr.values[index];

    // Mark entry as tombstone
    mat->storage.csr.col_indices[index] = CSR_TOMBSTONE_COL;
    mat->storage.csr.values[index] = 0.0f;
    size_t original_nnz = mat->count;
    mat->count--;

    maybe_compact_csr(mat, original_nnz);

    return value;
}
// -------------------------------------------------------------------------------- 

void compact_double_csr_matrix(matrix_d* mat) {
    errno = 0;
    if (!mat || mat->type != SPARSE_CSR_MATRIX) return;

    size_t nnz = mat->count;
    // size_t new_capacity = nnz;
    size_t* new_col_indices = malloc(nnz * sizeof(size_t));
    double* new_values = malloc(nnz * sizeof(double));
    size_t* new_row_ptrs = calloc(mat->rows + 1, sizeof(size_t));

    if (!new_col_indices || !new_values || !new_row_ptrs) {
        free(new_col_indices); free(new_values); free(new_row_ptrs);
        errno = ENOMEM;
        return;
    }

    size_t index = 0;
    for (size_t row = 0; row < mat->rows; ++row) {
        size_t start = mat->storage.csr.row_ptrs[row];
        size_t end   = mat->storage.csr.row_ptrs[row + 1];

        new_row_ptrs[row] = index;

        for (size_t i = start; i < end; ++i) {
            if (mat->storage.csr.col_indices[i] != CSR_TOMBSTONE_COL) {
                new_col_indices[index] = mat->storage.csr.col_indices[i];
                new_values[index] = mat->storage.csr.values[i];
                ++index;
            }
        }
    }

    new_row_ptrs[mat->rows] = index;

    // Replace old data
    free(mat->storage.csr.col_indices);
    free(mat->storage.csr.values);
    free(mat->storage.csr.row_ptrs);
    mat->storage.csr.col_indices = new_col_indices;
    mat->storage.csr.values = new_values;
    mat->storage.csr.row_ptrs = new_row_ptrs;
}
// -------------------------------------------------------------------------------- 

static matrix_d* convert_double_csr_to_coo(const matrix_d* csr_mat) {
    errno = 0;

    if (!csr_mat || csr_mat->type != SPARSE_CSR_MATRIX ||
        !csr_mat->storage.csr.row_ptrs || !csr_mat->storage.csr.col_indices || !csr_mat->storage.csr.values) {
        errno = EINVAL;
        return NULL;
    }

    size_t rows = csr_mat->rows;
    size_t cols = csr_mat->cols;
    size_t nnz  = csr_mat->count;

    size_t* row_arr = malloc(nnz * sizeof(size_t));
    size_t* col_arr = malloc(nnz * sizeof(size_t));
    double* values   = malloc(nnz * sizeof(double));
    if (!row_arr || !col_arr || !values) {
        free(row_arr); free(col_arr); free(values);
        errno = ENOMEM;
        return NULL;
    }

    size_t k = 0;
    for (size_t i = 0; i < rows; ++i) {
        size_t start = csr_mat->storage.csr.row_ptrs[i];
        size_t end   = csr_mat->storage.csr.row_ptrs[i + 1];
        for (size_t j = start; j < end; ++j) {
            row_arr[k] = i;
            col_arr[k] = csr_mat->storage.csr.col_indices[j];
            values[k]  = csr_mat->storage.csr.values[j];
            ++k;
        }
    }

    matrix_d* coo_mat = malloc(sizeof(matrix_d));
    if (!coo_mat) {
        free(row_arr); free(col_arr); free(values);
        errno = ENOMEM;
        return NULL;
    }

    coo_mat->type = SPARSE_COO_MATRIX;
    coo_mat->rows = rows;
    coo_mat->cols = cols;
    coo_mat->count = nnz;
    coo_mat->storage.coo.rows = row_arr;
    coo_mat->storage.coo.cols = col_arr;
    coo_mat->storage.coo.values = values;
    coo_mat->storage.coo.capacity = nnz;

    return coo_mat;
}
// -------------------------------------------------------------------------------- 

static matrix_d* convert_double_coo_to_dense(const matrix_d* coo_mat) {
    errno = 0;

    if (!coo_mat || coo_mat->type != SPARSE_COO_MATRIX ||
        !coo_mat->storage.coo.rows || !coo_mat->storage.coo.cols || !coo_mat->storage.coo.values) {
        errno = EINVAL;
        return NULL;
    }

    size_t rows = coo_mat->rows;
    size_t cols = coo_mat->cols;
    size_t nnz  = coo_mat->count;

    double* data = calloc(rows * cols, sizeof(double));
    uint8_t* init = calloc(rows * cols, sizeof(uint8_t));  // ← fixed type
    if (!data || !init) {
        free(data); free(init);
        errno = ENOMEM;
        return NULL;
    }

    for (size_t i = 0; i < nnz; ++i) {
        size_t r = coo_mat->storage.coo.rows[i];
        size_t c = coo_mat->storage.coo.cols[i];
        double  v = coo_mat->storage.coo.values[i];
        size_t idx = r * cols + c;
        data[idx] = v;
        init[idx] = 1;  // ← use 1 instead of `true`
    }

    matrix_d* dense_mat = malloc(sizeof(matrix_d));
    if (!dense_mat) {
        free(data); free(init);
        errno = ENOMEM;
        return NULL;
    }

    dense_mat->type = DENSE_MATRIX;
    dense_mat->rows = rows;
    dense_mat->cols = cols;
    dense_mat->count = nnz;
    dense_mat->storage.dense.data = data;
    dense_mat->storage.dense.init = init;

    return dense_mat;
}
// -------------------------------------------------------------------------------- 

void convert_doubleMat_to_dense(matrix_d** pmat) {
    errno = 0;
    matrix_d* mat = *pmat;
    if (!mat) {
        errno = EINVAL;
        return;
    }

    if (mat->type == DENSE_MATRIX) return;

    if (mat->type == SPARSE_COO_MATRIX) {
        matrix_d* new_mat = convert_double_coo_to_dense(mat);
        if (!new_mat) return;
        free_double_matrix(mat);
        *pmat = new_mat;
    } else if (mat->type == SPARSE_CSR_MATRIX) {
        matrix_d* coo_mat = convert_double_csr_to_coo(mat);
        if (!coo_mat) return;
        free_double_matrix(mat);
        matrix_d* dense_mat = convert_double_coo_to_dense(coo_mat);
        free_double_matrix(coo_mat);
        if (!dense_mat) return;
        *pmat = dense_mat;
    } else {
        errno = EINVAL;
    }
}
// -------------------------------------------------------------------------------- 

void convert_doubleMat_to_coo(matrix_d** pmat) {
    errno = 0;
    matrix_d* mat = *pmat;
    if (!mat) {
        errno = EINVAL;
        return;
    }

    if (mat->type == SPARSE_COO_MATRIX) return;

    if (mat->type == DENSE_MATRIX) {
        matrix_d* new_mat = convert_double_dense_to_coo(mat);
        if (!new_mat) return;
        free_double_matrix(mat);
        *pmat = new_mat;
    } else if (mat->type == SPARSE_CSR_MATRIX) {
        matrix_d* new_mat = convert_double_csr_to_coo(mat);
        if (!new_mat) return;
        free_double_matrix(mat);
        *pmat = new_mat;
    } else {
        errno = EINVAL;
    }
}
// -------------------------------------------------------------------------------- 

void convert_doubleMat_to_csr(matrix_d** pmat) {
    errno = 0;
    matrix_d* mat = *pmat;
    if (!mat) {
        errno = EINVAL;
        return;
    }

    if (mat->type == SPARSE_CSR_MATRIX) return;

    if (mat->type == SPARSE_COO_MATRIX) {
        matrix_d* new_mat = convert_double_coo_to_csr(mat);
        if (!new_mat) return;
        free_double_matrix(mat);
        *pmat = new_mat;
    } else if (mat->type == DENSE_MATRIX) {
        matrix_d* coo_mat = convert_double_dense_to_coo(mat);
        if (!coo_mat) return;
        free_double_matrix(mat);
        matrix_d* csr_mat = convert_double_coo_to_csr(coo_mat);
        free_double_matrix(coo_mat);
        if (!csr_mat) return;
        *pmat = csr_mat;
    } else {
        errno = EINVAL;
    }
}
// -------------------------------------------------------------------------------- 

static bool should_downgrade_double_matrix(const matrix_d* mat) {
    if (!mat || mat->type != SPARSE_CSR_MATRIX) return false;

    size_t total = mat->rows * mat->cols;
    if (total == 0) return false;

    double density = (double)mat->count / total;
    return density < CSR_COMPACT_THRESHOLD;
}
// -------------------------------------------------------------------------------- 

static void maybe_downgrade_matrix_dormat(matrix_d** pmat) {
    if (!pmat || !*pmat) return;
    matrix_d* mat = *pmat;

    // Step 1: Try CSR → COO downgrade
    if (mat->type == SPARSE_CSR_MATRIX && should_downgrade_double_matrix(mat)) {
        matrix_d* coo = convert_double_csr_to_coo(mat);
        if (coo) {
            free_double_matrix(mat);
            mat = coo;
            *pmat = mat;
        }
    }

    // Step 2: Try COO → Dense downgrade
    if (mat->type == SPARSE_COO_MATRIX) {
        size_t total = mat->rows * mat->cols;
        if (total < MIN_SPARSE_SIZE || ((double)mat->count / total) > DENSE_THRESHOLD) {
            matrix_d* dense = convert_double_coo_to_dense(mat);
            if (dense) {
                free_double_matrix(mat);
                *pmat = dense;
            }
        }
    }
}
// -------------------------------------------------------------------------------- 

double pop_double_matrix(matrix_d** pmat, size_t row, size_t col) {
    errno = 0;
    if (!pmat || !*pmat) {
        errno = EINVAL;
        return FLT_MAX;
    }

    matrix_d* mat = *pmat;
    double result = FLT_MAX;

    if (mat->type == DENSE_MATRIX)
        result = pop_double_dense_matrix(mat, row, col);
    else if (mat->type == SPARSE_COO_MATRIX)
        result = pop_double_coo_matrix(mat, row, col);
    else if (mat->type == SPARSE_CSR_MATRIX)
        result = pop_double_csr_matrix(mat, row, col);
    else {
        errno = EINVAL;
        return FLT_MAX;
    }

    // Try format downgrade (e.g., CSR → COO → DENSE)
    maybe_downgrade_matrix_dormat(pmat);

    return result;
}
// -------------------------------------------------------------------------------- 

bool finalize_double_matrix(matrix_d** mat_ptr) {
    errno = 0;
    if (!mat_ptr || !*mat_ptr) {
        errno = EINVAL;
        return false;
    }
    maybe_convert_double_matrix(mat_ptr, true);  // allow_csr = true
    return true;
}
// -------------------------------------------------------------------------------- 

MatrixStorageType get_double_matrix_type(matrix_d* mat) {
    errno = 0;
    if (!mat) {
        errno = 0;
        return 0;
    }
    return mat->type;
}
// -------------------------------------------------------------------------------- 

size_t get_double_matrix_rows(matrix_d* mat) {
    errno = 0;
    if (!mat) {
        errno = 0;
        return (size_t)-1;
    }
    return mat->rows;
}
// -------------------------------------------------------------------------------- 

size_t get_double_matrix_cols(matrix_d* mat) {
    errno = 0;
    if (!mat) {
        errno = 0;
        return (size_t)-1;
    }
    return mat->cols;
}
// -------------------------------------------------------------------------------- 

size_t get_double_matrix_element_count(matrix_d* mat) {
    errno = 0;
    if (!mat) {
        errno = 0;
        return (size_t)-1;
    }
    return mat->count;
}
// -------------------------------------------------------------------------------- 

matrix_d* invert_double_dense_matrix(const matrix_d* mat) {
    errno = 0;

    if (!mat || mat->type != DENSE_MATRIX || mat->rows != mat->cols) {
        errno = EINVAL;
        return NULL;
    }

    size_t n = mat->rows;
    matrix_d* inverse = create_double_matrix(n, n, 0);
    if (!inverse) {
        errno = ENOMEM;
        return NULL;
    }

    double* a = malloc(n * n * sizeof(double));
    if (!a) {
        free_double_matrix(inverse);
        errno = ENOMEM;
        return NULL;
    }

    // Copy matrix and initialize identity
    for (size_t i = 0; i < n * n; ++i) {
        a[i] = mat->storage.dense.data[i];
        inverse->storage.dense.data[i] = (i / n == i % n) ? 1.0f : 0.0f;
    }

    for (size_t i = 0; i < n; ++i) {
        size_t pivot = i;
        double max = fabs(a[i * n + i]);
        for (size_t j = i + 1; j < n; ++j) {
            double temp = fabs(a[j * n + i]);
            if (temp > max) {
                max = temp;
                pivot = j;
            }
        }

        if (fabs(a[pivot * n + i]) < 1e-8f) {
            free(a);
            free_double_matrix(inverse);
            errno = ERANGE;
            return NULL;
        }

        // Swap rows in A and inverse
        if (pivot != i) {
            for (size_t j = 0; j < n; ++j) {
                double tmp = a[i * n + j];
                a[i * n + j] = a[pivot * n + j];
                a[pivot * n + j] = tmp;

                tmp = inverse->storage.dense.data[i * n + j];
                inverse->storage.dense.data[i * n + j] = inverse->storage.dense.data[pivot * n + j];
                inverse->storage.dense.data[pivot * n + j] = tmp;
            }
        }

        double pivot_val = a[i * n + i];

#if defined(__AVX__)
        __m256 vpivot = _mm256_set1_ps(pivot_val);
        size_t j = 0;
        for (; j + 8 <= n; j += 8) {
            __m256 va = _mm256_loadu_ps(&a[i * n + j]);
            __m256 vinv = _mm256_loadu_ps(&inverse->storage.dense.data[i * n + j]);
            va = _mm256_div_ps(va, vpivot);
            vinv = _mm256_div_ps(vinv, vpivot);
            _mm256_storeu_ps(&a[i * n + j], va);
            _mm256_storeu_ps(&inverse->storage.dense.data[i * n + j], vinv);
        }
        for (; j < n; ++j) {
            a[i * n + j] /= pivot_val;
            inverse->storage.dense.data[i * n + j] /= pivot_val;
        }
#else
        for (size_t j = 0; j < n; ++j) {
            a[i * n + j] /= pivot_val;
            inverse->storage.dense.data[i * n + j] /= pivot_val;
        }
#endif

        for (size_t k = 0; k < n; ++k) {
            if (k == i) continue;
            double factor = a[k * n + i];
#if defined(__AVX__)
            j = 0;
            __m256 vfactor = _mm256_set1_ps(factor);
            for (; j + 8 <= n; j += 8) {
                __m256 vk = _mm256_loadu_ps(&a[k * n + j]);
                __m256 vi = _mm256_loadu_ps(&a[i * n + j]);
                vk = _mm256_sub_ps(vk, _mm256_mul_ps(vfactor, vi));
                _mm256_storeu_ps(&a[k * n + j], vk);

                __m256 vinvk = _mm256_loadu_ps(&inverse->storage.dense.data[k * n + j]);
                __m256 vinvi = _mm256_loadu_ps(&inverse->storage.dense.data[i * n + j]);
                vinvk = _mm256_sub_ps(vinvk, _mm256_mul_ps(vfactor, vinvi));
                _mm256_storeu_ps(&inverse->storage.dense.data[k * n + j], vinvk);
            }
            for (; j < n; ++j) {
                a[k * n + j] -= factor * a[i * n + j];
                inverse->storage.dense.data[k * n + j] -= factor * inverse->storage.dense.data[i * n + j];
            }
#else
            for (size_t j = 0; j < n; ++j) {
                a[k * n + j] -= factor * a[i * n + j];
                inverse->storage.dense.data[k * n + j] -= factor * inverse->storage.dense.data[i * n + j];
            }
#endif
        }
    }

    free(a);
    return inverse;
}
// -------------------------------------------------------------------------------- 

bool transpose_double_dense_matrix(matrix_d** pmat) {
    errno = 0;
    if (!pmat || !*pmat || (*pmat)->type != DENSE_MATRIX) {
        errno = EINVAL;
        return false;
    }

    matrix_d* mat = *pmat;
    size_t rows = mat->rows;
    size_t cols = mat->cols;
    double* data = mat->storage.dense.data;

    // Case 1: Square matrix — transpose in place
    if (rows == cols) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = i + 1; j < cols; ++j) {
                size_t idx1 = i * cols + j;
                size_t idx2 = j * cols + i;
                double tmp = data[idx1];
                data[idx1] = data[idx2];
                data[idx2] = tmp;
            }
        }
        return true;
    }

    // Case 2: Rectangular matrix — allocate new transposed matrix
    matrix_d* transposed = create_double_dense_matrix(cols, rows);
    if (!transposed) {
        errno = ENOMEM;
        return false;
    }

    //double* new_data = transposed->storage.dense.data;
    for (size_t i = 0; i < get_double_matrix_rows(mat); ++i) {
        for (size_t j = 0; j < get_double_matrix_cols(mat); ++j) {
            double value = get_double_dense_matrix(mat, i, j);   // Access original at (i, j)
            insert_double_dense_matrix(transposed, j, i, value); // Store transposed at (j, i)
        }
    }

    free_double_matrix(mat);
    *pmat = transposed;
    return true;
}

// bool transpose_double_dense_matrix(matrix_d** pmat) {
//     errno = 0;
//     if (!pmat || !*pmat || (*pmat)->type != DENSE_MATRIX) {
//         errno = EINVAL;
//         return false;
//     }
//
//     matrix_d* mat = *pmat;
//
//     size_t rows = mat->rows;
//     size_t cols = mat->cols;
//     double* data = mat->storage.dense.data;
//
//     // Case 1: Square matrix — transpose in place
//     if (rows == cols) {
//         for (size_t i = 0; i < rows; ++i) {
//             for (size_t j = i + 1; j < cols; ++j) {
//                 double tmp = data[i * cols + j];
//                 data[i * cols + j] = data[j * cols + i];
//                 data[j * cols + i] = tmp;
//             }
//         }
//         return true;
//     }
//
//     // Case 2: Rectangular matrix — need to allocate new dense matrix
//     matrix_d* transposed = create_double_dense_matrix(cols, rows);
//     if (!transposed) {
//         errno = ENOMEM;
//         return false;
//     }
//
//     // Fill in transposed values
//     for (size_t i = 0; i < rows; ++i) {
//         for (size_t j = 0; j < cols; ++j) {
//             transposed->storage.dense.data[j * rows + i] = data[i * cols + j];
//         }
//     }
//
//     // Free old matrix and replace pointer
//     free_double_matrix(mat);
//     *pmat = transposed;
//     return true;
// }
//
// bool transpose_double_dense_matrix(matrix_d** pmat) {
//     errno = 0;
//     if (!pmat || !*pmat || (*pmat)->type != DENSE_MATRIX) {
//         errno = EINVAL;
//         return false;
//     }
//
//     matrix_d* mat = *pmat;
//
//     size_t rows = mat->rows;
//     size_t cols = mat->cols;
//     double* data = mat->storage.dense.data;
//
//     // Case 1: Square matrix — transpose in place
//     if (rows == cols) {
//         for (size_t i = 0; i < rows; ++i) {
//             for (size_t j = i + 1; j < cols; ++j) {
//                 double tmp = data[i * cols + j];
//                 data[i * cols + j] = data[j * cols + i];
//                 data[j * cols + i] = tmp;
//             }
//         }
//         return true;
//     }
//
//     // Case 2: Rectangular matrix — need to allocate new matrix
//     matrix_d* transposed = create_double_matrix(cols, rows, 0);
//     if (!transposed) {
//         errno = ENOMEM;
//         return false;
//     }
//
//     // Fill in transposed values
//     for (size_t i = 0; i < rows; ++i) {
//         for (size_t j = 0; j < cols; ++j) {
//             transposed->storage.dense.data[j * rows + i] = data[i * cols + j];
//         }
//     }
//
//     // Free old matrix and replace pointer
//     free_double_matrix(mat);
//     *pmat = transposed;
//     return true;
// }
//
// bool transpose_double_dense_matrix(matrix_d** pmat) {
//     errno = 0;
//
//     if (!pmat || !*pmat || (*pmat)->type != DENSE_MATRIX) {
//         errno = EINVAL;
//         return false;
//     }
//
//     matrix_d* mat = *pmat;
//
//     size_t rows = mat->rows;
//     size_t cols = mat->cols;
//     double* data = mat->storage.dense.data;
//
//     // Case 1: Square matrix — transpose in place
//     if (rows == cols) {
//         for (size_t i = 0; i < rows; ++i) {
//             for (size_t j = i + 1; j < cols; ++j) {
//                 double tmp = data[i * cols + j];
//                 data[i * cols + j] = data[j * cols + i];
//                 data[j * cols + i] = tmp;
//             }
//         }
//         return true;
//     }
//
//     // Case 2: Rectangular matrix — need to allocate new storage
//     matrix_d* transposed = create_double_matrix(cols, rows, 0);
//     if (!transposed) {
//         errno = ENOMEM;
//         return false;
//     }
//
//     for (size_t i = 0; i < rows; ++i) {
//         for (size_t j = 0; j < cols; ++j) {
//             transposed->storage.dense.data[j * rows + i] = data[i * cols + j];
//         }
//     }
//
//     free_double_matrix(mat);
//     *pmat = transposed;
//     return true;
// }
// -------------------------------------------------------------------------------- 

bool transpose_double_coo_matrix(matrix_d** pmat) {
    errno = 0;

    if (!pmat || !*pmat || (*pmat)->type != SPARSE_COO_MATRIX) {
        errno = EINVAL;
        return false;
    }

    matrix_d* mat = *pmat;

    for (size_t i = 0; i < mat->count; ++i) {
        size_t tmp = mat->storage.coo.rows[i];
        mat->storage.coo.rows[i] = mat->storage.coo.cols[i];
        mat->storage.coo.cols[i] = tmp;
    }

    // Swap the matrix dimensions
    size_t tmp_dim = mat->rows;
    mat->rows = mat->cols;
    mat->cols = tmp_dim;

    return true;
}
// -------------------------------------------------------------------------------- 

bool transpose_double_csr_matrix(matrix_d** pmat) {
    errno = 0;

    if (!pmat || !*pmat || (*pmat)->type != SPARSE_CSR_MATRIX) {
        errno = EINVAL;
        return false;
    }

    matrix_d* mat = *pmat;
    size_t rows = mat->rows;
    size_t cols = mat->cols;
    size_t nnz = mat->count;

    size_t* row_ptrs_T = calloc(cols + 1, sizeof(size_t));
    size_t* col_indices_T = malloc(nnz * sizeof(size_t));
    double* values_T = malloc(nnz * sizeof(double));
    if (!row_ptrs_T || !col_indices_T || !values_T) {
        free(row_ptrs_T); free(col_indices_T); free(values_T);
        errno = ENOMEM;
        return false;
    }

    // Step 1: Count non-zeros per column (future row in transpose)
    for (size_t i = 0; i < nnz; ++i) {
        size_t col = mat->storage.csr.col_indices[i];
        row_ptrs_T[col + 1]++;
    }

    // Step 2: Prefix sum to get row_ptrs
    for (size_t i = 1; i <= cols; ++i) {
        row_ptrs_T[i] += row_ptrs_T[i - 1];
    }

    // Step 3: Fill transposed values
    size_t* counter = calloc(cols, sizeof(size_t));
    if (!counter) {
        free(row_ptrs_T); free(col_indices_T); free(values_T);
        errno = ENOMEM;
        return false;
    }

    for (size_t row = 0; row < rows; ++row) {
        size_t start = mat->storage.csr.row_ptrs[row];
        size_t end   = mat->storage.csr.row_ptrs[row + 1];

        for (size_t i = start; i < end; ++i) {
            size_t col = mat->storage.csr.col_indices[i];
            double val = mat->storage.csr.values[i];

            size_t dest = row_ptrs_T[col] + counter[col]++;
            col_indices_T[dest] = row;
            values_T[dest] = val;
        }
    }

    free(counter);

    // Replace contents of original matrix
    free(mat->storage.csr.row_ptrs);
    free(mat->storage.csr.col_indices);
    free(mat->storage.csr.values);

    mat->storage.csr.row_ptrs = row_ptrs_T;
    mat->storage.csr.col_indices = col_indices_T;
    mat->storage.csr.values = values_T;
    mat->rows = cols;
    mat->cols = rows;

    return true;
}
// -------------------------------------------------------------------------------- 

bool transpose_double_matrix(matrix_d** pmat) {
    errno = 0;

    if (!pmat || !*pmat) {
        errno = EINVAL;
        return false;
    }

    matrix_d* mat = *pmat;

    switch (mat->type) {
        case DENSE_MATRIX:
            return transpose_double_dense_matrix(pmat);

        case SPARSE_COO_MATRIX:
            return transpose_double_coo_matrix(pmat);

        case SPARSE_CSR_MATRIX:
            return transpose_double_csr_matrix(pmat);

        default:
            errno = EINVAL;
            return false;
    }
}
// -------------------------------------------------------------------------------- 

matrix_d* copy_double_dense_matrix(const matrix_d* mat) {
    errno = 0;
    if (!mat || mat->type != DENSE_MATRIX) {
        errno = EINVAL;
        return NULL;
    }

    matrix_d* newMat = create_double_dense_matrix(mat->rows, mat->cols);
    if (!newMat) {
        errno = ENOMEM;
        return NULL;
    }

    newMat->count = mat->count;

    size_t total_elements = mat->rows * mat->cols;
    memcpy(newMat->storage.dense.data, mat->storage.dense.data, total_elements * sizeof(double));
    memcpy(newMat->storage.dense.init, mat->storage.dense.init, total_elements * sizeof(uint8_t));

    return newMat;
}
// -------------------------------------------------------------------------------- 

matrix_d* copy_double_coo_matrix(const matrix_d* mat) {
    errno = 0;

    if (!mat || mat->type != SPARSE_COO_MATRIX) {
        errno = EINVAL;
        return NULL;
    }

    matrix_d* newMat = malloc(sizeof(matrix_d));
    if (!newMat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t cap = mat->storage.coo.capacity;
    size_t* row_indices = malloc(cap * sizeof(size_t));
    size_t* col_indices = malloc(cap * sizeof(size_t));
    double* values = malloc(cap * sizeof(double));

    if (!row_indices || !col_indices || !values) {
        free(row_indices);
        free(col_indices);
        free(values);
        free(newMat);
        errno = ENOMEM;
        return NULL;
    }

    // Copy structure metadata
    newMat->type = SPARSE_COO_MATRIX;
    newMat->rows = mat->rows;
    newMat->cols = mat->cols;
    newMat->count = mat->count;
    newMat->storage.coo.capacity = cap;

    // Assign allocated arrays
    newMat->storage.coo.rows = row_indices;
    newMat->storage.coo.cols = col_indices;
    newMat->storage.coo.values = values;

    // Copy used data only (up to `count`)
    memcpy(row_indices, mat->storage.coo.rows, mat->count * sizeof(size_t));
    memcpy(col_indices, mat->storage.coo.cols, mat->count * sizeof(size_t));
    memcpy(values, mat->storage.coo.values, mat->count * sizeof(double));

    return newMat;
}
// -------------------------------------------------------------------------------- 

matrix_d* copy_double_csr_matrix(const matrix_d* mat) {
    errno = 0;

    if (!mat || mat->type != SPARSE_CSR_MATRIX) {
        errno = EINVAL;
        return NULL;
    }

    size_t rows = mat->rows;
    size_t cols = mat->cols;
    size_t nnz = mat->count;

    matrix_d* newMat = malloc(sizeof(matrix_d));
    if (!newMat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t* row_ptrs = calloc(rows + 1, sizeof(size_t));
    size_t* col_indices = malloc(nnz * sizeof(size_t));
    double* values = malloc(nnz * sizeof(double));

    if (!row_ptrs || !col_indices || !values) {
        free(row_ptrs);
        free(col_indices);
        free(values);
        free(newMat);
        errno = ENOMEM;
        return NULL;
    }

    memcpy(row_ptrs, mat->storage.csr.row_ptrs, (rows + 1) * sizeof(size_t));
    memcpy(col_indices, mat->storage.csr.col_indices, nnz * sizeof(size_t));
    memcpy(values, mat->storage.csr.values, nnz * sizeof(double));

    newMat->type = SPARSE_CSR_MATRIX;
    newMat->rows = rows;
    newMat->cols = cols;
    newMat->count = nnz;

    newMat->storage.csr.row_ptrs = row_ptrs;
    newMat->storage.csr.col_indices = col_indices;
    newMat->storage.csr.values = values;

    return newMat;
}
// -------------------------------------------------------------------------------- 

matrix_d* copy_double_matrix(const matrix_d* mat) {
    errno = 0;

    if (!mat) {
        errno = EINVAL;
        return false;
    }

    switch (mat->type) {
        case DENSE_MATRIX:
            return copy_double_dense_matrix(mat);

        case SPARSE_COO_MATRIX:
            return copy_double_coo_matrix(mat);

        case SPARSE_CSR_MATRIX:
            return copy_double_csr_matrix(mat);

        default:
            errno = EINVAL;
            return false;
    }
}
// -------------------------------------------------------------------------------- 

double double_dense_matrix_det(const matrix_d* mat) {
    errno = 0;

    if (!mat || mat->type != DENSE_MATRIX || mat->rows != mat->cols) {
        errno = EINVAL;
        return NAN;
    }

    size_t n = mat->rows;
    const double* original = mat->storage.dense.data;

    double* temp = malloc(n * n * sizeof(double));
    if (!temp) {
        errno = ENOMEM;
        return NAN;
    }

    memcpy(temp, original, n * n * sizeof(double));
    double det = 1.0;

    for (size_t i = 0; i < n; ++i) {
        // Pivot selection
        size_t pivot = i;
        double max_val = fabs(temp[i * n + i]);
        for (size_t j = i + 1; j < n; ++j) {
            double val = fabs(temp[j * n + i]);
            if (val > max_val) {
                max_val = val;
                pivot = j;
            }
        }

        if (max_val < 1e-12) {
            free(temp);
            return 0.0; // Singular matrix
        }

        if (pivot != i) {
            for (size_t k = 0; k < n; ++k) {
                double tmp = temp[i * n + k];
                temp[i * n + k] = temp[pivot * n + k];
                temp[pivot * n + k] = tmp;
            }
            det *= -1.0;
        }

        double pivot_val = temp[i * n + i];
        det *= pivot_val;

        for (size_t j = i + 1; j < n; ++j) {
            double factor = temp[j * n + i] / pivot_val;

#if defined(__AVX__)
            size_t k = i;
            for (; k + 4 <= n; k += 4) {
                __m256d row_i = _mm256_loadu_pd(&temp[i * n + k]);
                __m256d row_j = _mm256_loadu_pd(&temp[j * n + k]);
                __m256d scaled = _mm256_mul_pd(row_i, _mm256_set1_pd(factor));
                __m256d result = _mm256_sub_pd(row_j, scaled);
                _mm256_storeu_pd(&temp[j * n + k], result);
            }
            for (; k < n; ++k) {
                temp[j * n + k] -= factor * temp[i * n + k];
            }
#else
            for (size_t k = i; k < n; ++k) {
                temp[j * n + k] -= factor * temp[i * n + k];
            }
#endif
        }
    }

    free(temp);
    return det;
}

// ================================================================================
// ================================================================================
// eof
