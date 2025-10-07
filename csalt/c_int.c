// ================================================================================
// ================================================================================
// - File:    c_int.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 12, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <immintrin.h>  // AVX/SSE
#include "c_int.h"
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <float.h>

#if defined(__AVX2__)
  #include "simd_avx2_int32.inl"
#elif defined(__SSE4_1__)
  #include "simd_sse41_int32.inl"
#elif defined(__SSE2__)
  #include "simd_sse2_int32.inl"
#elif defined(__ARM_FEATURE_SVE2)
  #include "simd_sve2_int32.inl"
#elif defined(__ARM_FEATURE_SVE)
  #include "simd_sve_int32.inl"
#elif defined(__aarch64__)
  #include "simd_neon_int32.inl"
#endif

static const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
static const size_t hashSize = 16;  //  Size fo hash map init functions
static const uint32_t HASH_SEED = 0x45d9f3b; 
// ================================================================================
// ================================================================================ 

static inline int simd_sum_i32(const int32_t* x, size_t n) {
    #if defined(__AVX2__)
        return (int)simd_sum_i32_avx2(x, n);
    #elif defined(__AVX__)
        /* AVX has no 256-bit integer arithmetic; use SSE4.1 path */
        return (int)simd_sum_i32_sse41(x, n);
    #elif defined(__SSE4_1__)
        return (int)simd_sum_i32_sse41(x, n);
    #elif defined(__SSE2__)
        return (int)simd_sum_i32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return (int)simd_sum_i32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return (int)simd_sum_i32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return (int)simd_sum_i32_neon(x, n);
    #else
        long long s = 0; for (size_t i=0;i<n;++i) s += x[i]; return (int)s;
    #endif
}
// --------------------------------------------------------------------------------

/* min: returns int */
static inline int simd_min_i32(const int32_t* x, size_t n) {
    #if defined(__AVX2__)
        return (int)simd_min_i32_avx2(x, n);
    #elif defined(__AVX__)
        return (int)simd_min_i32_sse41(x, n);
    #elif defined(__SSE4_1__)
        return (int)simd_min_i32_sse41(x, n);
    #elif defined(__SSE2__)
        return (int)simd_min_i32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return (int)simd_min_i32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return (int)simd_min_i32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return (int)simd_min_i32_neon(x, n);
    #else
        int m = x[0]; for (size_t i=1;i<n;++i) if (x[i] < m) m = x[i]; return m;
    #endif
}
// --------------------------------------------------------------------------------

/* max: returns int */
static inline int simd_max_i32(const int32_t* x, size_t n) {
    #if defined(__AVX2__)
        return (int)simd_max_i32_avx2(x, n);
    #elif defined(__AVX__)
        return (int)simd_max_i32_sse41(x, n);
    #elif defined(__SSE4_1__)
        return (int)simd_max_i32_sse41(x, n);
    #elif defined(__SSE2__)
        return (int)simd_max_i32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return (int)simd_max_i32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return (int)simd_max_i32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return (int)simd_max_i32_neon(x, n);
    #else
        int m = x[0]; for (size_t i=1;i<n;++i) if (x[i] > m) m = x[i]; return m;
    #endif
}
// --------------------------------------------------------------------------------

/* dot: returns int (accumulate in i64 inside ISA impl; cast here). Beware potential overflow on cast. */
static inline int simd_dot_i32(const int32_t* a, const int32_t* b, size_t n) {
    #if defined(__AVX2__)
        return (int)simd_dot_i32_avx2(a, b, n);
    #elif defined(__AVX__)
        return (int)simd_dot_i32_sse41(a, b, n);
    #elif defined(__SSE4_1__)
        return (int)simd_dot_i32_sse41(a, b, n);
    #elif defined(__SSE2__)
        return (int)simd_dot_i32_sse(a, b, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return (int)simd_dot_i32_sve2(a, b, n);
    #elif defined(__ARM_FEATURE_SVE)
        return (int)simd_dot_i32_sve(a, b, n);
    #elif defined(__ARM_FEATURE_NEON)
        return (int)simd_dot_i32_neon(a, b, n);
    #else
        long long s = 0; for (size_t i=0;i<n;++i) s += (long long)a[i] * (long long)b[i]; return (int)s;
    #endif
}
// --------------------------------------------------------------------------------

/* mean: returns float (compute sum in i64 inside ISA impl; divide in FP) */
static inline float simd_mean_i32(const int32_t* x, size_t n) {
    if (!n) return 0.0f;
    #if defined(__AVX2__)
        return simd_mean_i32_avx2(x, n);
    #elif defined(__AVX__)
        return simd_mean_i32_sse41(x, n);
    #elif defined(__SSE4_1__)
        return simd_mean_i32_sse41(x, n);
    #elif defined(__SSE2__)
        return simd_mean_i32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_mean_i32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_mean_i32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return simd_mean_i32_neon(x, n);
    #else
        long long s = 0; for (size_t i=0;i<n;++i) s += x[i]; return (float)((double)s / (double)n);
    #endif
}
// --------------------------------------------------------------------------------

/* stdev (population): returns float (two-pass; second pass in float SIMD in ISA impls) */
static inline float simd_stdev_i32(const int32_t* x, size_t n) {
    if (n < 2) return 0.0f;
    #if defined(__AVX2__)
        return simd_stdev_i32_avx2(x, n);
    #elif defined(__AVX__)
        return simd_stdev_i32_sse41(x, n);
    #elif defined(__SSE4_1__)
        return simd_stdev_i32_sse41(x, n);
    #elif defined(__SSE2__)
        return simd_stdev_i32_sse(x, n);
    #elif defined(__ARM_FEATURE_SVE2)
        return simd_stdev_i32_sve2(x, n);
    #elif defined(__ARM_FEATURE_SVE)
        return simd_stdev_i32_sve(x, n);
    #elif defined(__ARM_FEATURE_NEON)
        return simd_stdev_i32_neon(x, n);
    #else
        /* scalar two-pass */
        double mu = 0.0; for (size_t i=0;i<n;++i) mu += (double)x[i]; mu /= (double)n;
        double ss = 0.0; for (size_t i=0;i<n;++i) { double d = (double)x[i] - mu; ss += d*d; }
        return (float)sqrt(ss / (double)n);
    #endif
}


// ================================================================================ 
// ================================================================================ 

int_v* init_int_vector(size_t buff) {
    if (buff == 0) {
        errno = EINVAL;
        return NULL;
    }
    int_v* struct_ptr = malloc(sizeof(int_v));
    if (struct_ptr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
   
    int* data_ptr = calloc(buff, sizeof(int));
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

void free_int_vector(int_v* vec) {
   if (!vec || !vec->alloc_type || vec->alloc_type == STATIC) {
       errno = EINVAL;
       return;
   }
   if (vec->data) free(vec->data);
   free(vec);
}
// --------------------------------------------------------------------------------

void _free_int_vector(int_v** vec) {
    if (vec && *vec) {
        free_int_vector(*vec);
        *vec = NULL;
    }
}
// -------------------------------------------------------------------------------- 

bool push_back_int_vector(int_v* vec, int value) {
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
            errno = set_errno_from_error(vec->error);
            return false;
        }
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        // Allocate more space for the array of str structs
        int* new_data = realloc(vec->data, new_alloc * sizeof(int));
        if (!new_data) {
            vec->error = REALLOC_FAIL;
            errno = set_errno_from_error(vec->error);
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(int));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    vec->data[vec->len] = value; 
    vec->len++;
    vec->error = NO_ERROR; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_int_vector(int_v* vec, int value) {
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
        
        int* new_data = realloc(vec->data, new_alloc * sizeof(int));
        if (!new_data) {
            vec->error = REALLOC_FAIL;
            errno = set_errno_from_error(vec->error);
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(int));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    
    // Move existing elements right if there are any
    if (vec->len > 0) {
        memmove(vec->data + 1, vec->data, vec->len * sizeof(int));
    }
    
    vec->data[0] = value;    
    vec->len++;
    vec->error = NO_ERROR;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_int_vector(int_v* vec, int value, size_t index) {
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
       
        int* new_data = realloc(vec->data, new_alloc * sizeof(int));
        if (!new_data) {
            vec->error = REALLOC_FAIL;
            errno = set_errno_from_error(vec->error);
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(int));
       
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
                (vec->len - index) * sizeof(int));
    }
    
    vec->data[index] = value;
    vec->len++;
    vec->error = NO_ERROR;
    return true;
}
// -------------------------------------------------------------------------------- 


int pop_back_int_vector(int_v* vec) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return INT_MAX;
    }
    
    if (vec->len == 0) {
        vec->error = INVALID_ARG;
        errno = set_errno_from_error(vec->error);
        return INT_MAX;
    }
    
    // Create copy of last element
    int temp = vec->data[vec->len - 1];
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(int));
    vec->len--;
    vec->error = NO_ERROR;
    return temp;
}
// --------------------------------------------------------------------------------

int pop_front_int_vector(int_v* vec) {  // Fixed function name
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return INT_MAX;
    }
   
    if (vec->len == 0) {
        vec->error = INVALID_ARG;
        errno = set_errno_from_error(vec->error);
        return INT_MAX;
    }
   
    // Create copy of first element
    int temp = vec->data[0];
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(int));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(int));
   
    vec->len--;
    vec->error = NO_ERROR;
    return temp;
}
// --------------------------------------------------------------------------------

int pop_any_int_vector(int_v* vec, size_t index) {
    if (!vec || !vec->data) {
        if (vec) {
            vec->error = NULL_POINTER;
            errno = set_errno_from_error(vec->error);
        } else errno = EINVAL;
        return INT_MAX;
    }
   
    if (vec->len == 0) {
        vec->error = INVALID_ARG;
        errno = set_errno_from_error(vec->error);
        return INT_MAX;
    }
    
    if (index >= vec->len) {
        vec->error = OUT_OF_BOUNDS;
        errno = set_errno_from_error(vec->error);
        return INT_MAX;
    }
    
    // Create copy of element to pop
    int temp = vec->data[index];
    
    // If not the last element, shift remaining elements left
    if (index < vec->len - 1) {
        // Check for overflow in memmove size calculation
        if ((vec->len - index - 1) > SIZE_MAX / sizeof(int)) {
            vec->error = SIZE_MISMATCH;
            errno = set_errno_from_error(vec->error);
            return INT_MAX;
        }
        
        memmove(&vec->data[index], &vec->data[index + 1], 
                (vec->len - index - 1) * sizeof(int));
    }
   
    // Clear the last element
    memset(&vec->data[vec->len - 1], 0, sizeof(int));
    
    vec->len--;
    vec->error = NO_ERROR;
    return temp;
}
// --------------------------------------------------------------------------------

void reverse_int_vector(int_v* vec) {
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
       swap_int(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// ================================================================================
// ================================================================================ 
// QUICKSORT

void swap_int(int* a, int* b) {
    if (!a || !b) {
        errno = EINVAL;
        return;
    }
    int temp = *a;
    *a = *b;
    *b = temp;
}
// -------------------------------------------------------------------------------- 

static int* _median_of_three(int* a, int* b, int* c, iter_dir direction) {
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

static void _insertion_sort(int* vec, int low, int high, iter_dir direction) {
    for (int i = low + 1; i <= high; i++) {
        int key = vec[i];
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

static int _partition_int(int* vec, int low, int high, iter_dir direction) {
    int mid = low + (high - low) / 2;
    int* pivot_ptr = _median_of_three(&vec[low], &vec[mid], &vec[high], direction);
    
    if (pivot_ptr != &vec[high])
        swap_int(pivot_ptr, &vec[high]);
    
    int pivot = vec[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_int(&vec[i], &vec[j]);
        }
    }
    swap_int(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// -------------------------------------------------------------------------------- 

static void _quicksort_int(int* vec, int low, int high, iter_dir direction) {
    while (low < high) {
        if (high - low < 10) {
            _insertion_sort(vec, low, high, direction);
            break;
        }
        
        int pi = _partition_int(vec, low, high, direction);
        
        if (pi - low < high - pi) {
            _quicksort_int(vec, low, pi - 1, direction);
            low = pi + 1;
        } else {
            _quicksort_int(vec, pi + 1, high, direction);
            high = pi - 1;
        }
    }
}
// -------------------------------------------------------------------------------- 

void sort_int_vector(int_v* vec, iter_dir direction) {
    if (!vec) {
        errno = EINVAL;
        return;
    }
    if (vec->len < 2) return;
    
    _quicksort_int(vec->data, 0, vec->len - 1, direction);
}
// -------------------------------------------------------------------------------- 

void trim_int_vector(int_v* vec) {
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
    if (vec->len > SIZE_MAX / sizeof(int)) {
        errno = ERANGE;
        return;
    }
    
    int* ptr = realloc(vec->data, sizeof(int) * vec->len);
    if (ptr == NULL) {
        errno = ENOMEM;
        return;
    }
    
    vec->data = ptr;
    vec->alloc = vec->len;
}
// --------------------------------------------------------------------------------

size_t binary_search_int_vector(int_v* vec, int value, bool sort_first) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return SIZE_MAX;
    }

    if (vec->len == 0) {
        errno = ENODATA;
        return SIZE_MAX;
    }

    if (sort_first && vec->len > 1) {
        sort_int_vector(vec, FORWARD);
    }

    size_t left = 0;
    size_t right = vec->len - 1;

    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        int mid_val = vec->data[mid];

        if (mid_val == value) {
            return mid;
        } else if (mid_val < value) {
            left = mid + 1;
        } else {
            if (mid == 0) break;  // prevent underflow
            right = mid - 1;
        }
    }

    return SIZE_MAX;  // Not found
}
// -------------------------------------------------------------------------------- 

bin_dat binary_search_bounds_int_vector(int_v *vec,
                                        int value,
                                        bool sort_first) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return (bin_dat){ .lower = SIZE_MAX, .upper = SIZE_MAX };
    }
    if (vec->len == 0) {
        errno = ENODATA;
        return (bin_dat){ .lower = SIZE_MAX, .upper = SIZE_MAX };
    }

    if (sort_first && vec->len > 1) {
        sort_int_vector(vec, FORWARD);  // mutates vec
    }

    size_t left  = 0;
    size_t right = vec->len - 1;

    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        int diff = vec->data[mid] - value;

        if (diff == 0) {
            // exact hit: collapse bounds to mid
            return (bin_dat){ .lower = mid, .upper = mid };
        }
        if (diff < 0) {
            left = mid + 1;
        } else {
            if (mid == 0) {
                // no element less than value
                right = SIZE_MAX;  // sentinel
                break;
            }
            right = mid - 1;
        }
    }

    // Post-conditions:
    //  right = last index with element < value, or SIZE_MAX if none
    //  left  = first index with element > value, or vec->len if none

    if (right == SIZE_MAX) {
        // value < vec->data[0]
        return (bin_dat){ .lower = SIZE_MAX, .upper = 0 };
    }
    if (left >= vec->len) {
        // value > vec->data[len-1]
        return (bin_dat){ .lower = vec->len - 1, .upper = SIZE_MAX };
    }

    // In-range miss: strictly between right and left
    return (bin_dat){ .lower = right, .upper = left };
}
// -------------------------------------------------------------------------------- 

void update_int_vector(int_v* vec, size_t index, int replacement_value) {
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

int min_int_vector(int_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return INT_MAX;
    }
    return simd_min_i32(vec->data, vec->len);
}
// -------------------------------------------------------------------------------- 

int max_int_vector(int_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return INT_MAX;
    }
    return simd_max_i32(vec->data, vec->len);
}
// -------------------------------------------------------------------------------- 

int sum_int_vector(int_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return INT_MAX;
    }
    return simd_sum_i32(vec->data, vec->len);
}
// -------------------------------------------------------------------------------- 

float average_int_vector(int_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }

    int sum = sum_int_vector(vec);
    if (errno != 0) return FLT_MAX;
    return (float)sum / (float)vec->len;
}

// -------------------------------------------------------------------------------- 

float stdev_int_vector(int_v* vec) {
    if (!vec || !vec->data || vec->len < 2) {
        errno = ENODATA;
        return FLT_MAX;
    }
    return simd_stdev_i32(vec->data, vec->len);
}
// -------------------------------------------------------------------------------- 

int_v* cum_sum_int_vector(int_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return NULL;
    }

    int_v* new_vec = init_int_vector(vec->len);
    if (!new_vec) {
        errno = ENOMEM;
        return NULL;
    }

    int sum = 0;
    for (size_t i = 0; i < vec->len; ++i) {
        int val = vec->data[i];
        sum += val;


        if (!push_back_int_vector(new_vec, sum)) {
            free_int_vector(new_vec);
            return NULL;
        }
    }

    return new_vec;
}
// -------------------------------------------------------------------------------- 

int_v* copy_int_vector(const int_v* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    int_v* copy = init_int_vector(original->alloc);
    if (!copy) {
        return NULL;
    }

    for (size_t i = 0; i < original->len; ++i) {
        if (!push_back_int_vector(copy, original->data[i])) {
            free_int_vector(copy);
            return NULL;
        }
    }

    return copy;
}
// -------------------------------------------------------------------------------- 

int int_lin_interp(int x1, int y1,
                   int x2, int y2,
                   int x3) {
    if (x2 == x1) {
        errno = EINVAL;
        return 0;
    }

    double slope = (double)(y2 - y1) / (double)(x2 - x1);
    double y3 = (double)y1 + slope * (double)(x3 - x1);

    // Clamp to int range if needed
    if (y3 > INT_MAX) return INT_MAX;
    if (y3 < INT_MIN) return INT_MIN;

    return (int)lrint(y3);   // round to nearest int (C99)
}
// ================================================================================ 
// ================================================================================ 
// DICTIONARY IMPLEMENTATION

typedef struct idictNode {
    char* key;
    int value;
    struct idictNode* next;
} idictNode;
// --------------------------------------------------------------------------------

struct dict_i {
    idictNode* keyValues;
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
static bool resize_dict(dict_i* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    idictNode* new_table = calloc(new_size, sizeof(idictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table in case we need to recover
    idictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash existing entries
    for (size_t i = 0; i < old_size; i++) {
        idictNode* current = old_table[i].next;
        
        while (current) {
            idictNode* next = current->next;  // Save next pointer before modifying node

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
            idictNode* current = new_table[i].next;
            while (current) {
                idictNode* next = current->next;
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

dict_i* init_int_dict(void) {
    // Allocate the dictionary structure
    dict_i* dict = calloc(1, sizeof(dict_i));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(idictNode));
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

bool insert_int_dict(dict_i* dict, const char* key, int value) {
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
    for (idictNode* current = dict->keyValues[index].next; current; current = current->next) {
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

    idictNode* new_node = malloc(sizeof(idictNode));
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

int pop_int_dict(dict_i* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return INT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    idictNode* prev = &dict->keyValues[index];
    idictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Save value and unlink node
            int value = current->value;
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
    return INT_MAX;
}
// --------------------------------------------------------------------------------

int get_int_dict_value(const dict_i* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return INT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const idictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return INT_MAX;
}
// --------------------------------------------------------------------------------

void free_int_dict(dict_i* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        idictNode* current = dict->keyValues[i].next;
        while (current) {      
            idictNode* next = current->next;  // Save next pointer before freeing
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

void _free_int_dict(dict_i** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_int_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// --------------------------------------------------------------------------------

bool update_int_dict(dict_i* dict, const char* key, int value) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (idictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return true;
        }
    }

    errno = ENOENT;  // More specific error code for missing key
    return false;
}
// --------------------------------------------------------------------------------

size_t int_dict_size(const dict_i* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t int_dict_alloc(const dict_i* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// --------------------------------------------------------------------------------

size_t int_dict_hash_size(const dict_i* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

bool has_key_int_dict(const dict_i* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const idictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

dict_i* copy_int_dict(const dict_i* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with same capacity
    dict_i* new_dict = malloc(sizeof(dict_i));
    if (!new_dict) {
        errno = ENOMEM;
        return NULL;
    }

    new_dict->keyValues = calloc(dict->alloc, sizeof(idictNode));
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
        idictNode* current = dict->keyValues[i].next;
        while (current) {
            // Insert will handle incrementing hash_size and len
            if (!insert_int_dict(new_dict, current->key, current->value)) {
                free_int_dict(new_dict);  // Clean up on failure
                return NULL;
            }
            current = current->next;
        }
    }

    return new_dict;
}
// -------------------------------------------------------------------------------- 

bool clear_int_dict(dict_i* dict) {
    if (!dict) {
        errno = EINVAL;
        return false;
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        idictNode* current = dict->keyValues[i].next;
        while (current) {
            idictNode* next = current->next;
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

string_v* get_keys_int_dict(const dict_i* dict) {
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
        for (idictNode* current = dict->keyValues[i].next; current; current = current->next) {
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

int_v* get_values_int_dict(const dict_i* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }
    int_v* vec = init_int_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }
    // Iterate through all buckets
    for (size_t i = 0; i < dict->alloc; i++) {
        for (idictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_int_vector(vec, current->value)) {
                free_int_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }
    return vec;
}
// -------------------------------------------------------------------------------- 

dict_i* merge_int_dict(const dict_i* dict1, const dict_i* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with capacity for all items
    //size_t initial_size = dict1->hash_size + dict2->hash_size;
    dict_i* merged = init_int_dict();
    if (!merged) {
        return NULL;  // errno set by init_int_dict
    }

    // First, copy all entries from dict1
    for (size_t i = 0; i < dict1->alloc; i++) {
        for (idictNode* current = dict1->keyValues[i].next; current; current = current->next) {
            if (!insert_int_dict(merged, current->key, current->value)) {
                free_int_dict(merged);
                return NULL;
            }
        }
    }

    // Then handle dict2 entries
    for (size_t i = 0; i < dict2->alloc; i++) {
        for (idictNode* current = dict2->keyValues[i].next; current; current = current->next) {
            int existing_value;
            // Check if key exists in merged dictionary
            if ((existing_value = get_int_dict_value(merged, current->key)) != FLT_MAX) {
                if (overwrite) {
                    // Update existing value if overwrite is true
                    if (!update_int_dict(merged, current->key, current->value)) {
                        free_int_dict(merged);
                        return NULL;
                    }
                }
                // If overwrite is false, keep original value
            } else {
                // Key doesn't exist, insert new entry
                if (!insert_int_dict(merged, current->key, current->value)) {
                    free_int_dict(merged);
                    return NULL;
                }
            }
        }
    }

    return merged;
}
// --------------------------------------------------------------------------------

bool foreach_int_dict(const dict_i* dict, idict_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; i++) {
        for (idictNode* current = dict->keyValues[i].next; current; current = current->next) {
            iter(current->key, current->value, user_data);
        }
    }

    return true;
}
// ================================================================================ 
// ================================================================================ 

typedef struct ivdictNode {
    char* key;
    int_v* value;
    struct ivdictNode* next;
} ivdictNode;
// --------------------------------------------------------------------------------

struct dict_iv {
    ivdictNode* keyValues;
    size_t hash_size;
    size_t len;
    size_t alloc;
};
// --------------------------------------------------------------------------------

dict_iv* init_intv_dict(void) {
    // Allocate the dictionary structure
    dict_iv* dict = calloc(1, sizeof(dict_iv));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate vector dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(ivdictNode));
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

static bool resize_dictv(dict_iv* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better hash distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    ivdictNode* new_table = calloc(new_size, sizeof(ivdictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table for cleanup if something fails
    ivdictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash all existing entries into the new table
    for (size_t i = 0; i < old_size; ++i) {
        ivdictNode* current = old_table[i].next;
        while (current) {
            ivdictNode* next = current->next;

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
            ivdictNode* current = new_table[i].next;
            while (current) {
                ivdictNode* next = current->next;
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

bool create_intv_dict(dict_iv* dict, char* key, size_t size) {
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
    for (ivdictNode* current = dict->keyValues[index].next; current; current = current->next) {
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

    ivdictNode* new_node = malloc(sizeof(ivdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    int_v* value = NULL;
    value = init_int_vector(size);
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

bool pop_intv_dict(dict_iv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    ivdictNode* prev = &dict->keyValues[index];
    ivdictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            prev->next = current->next;
            
            // Update dictionary metadata
            dict->hash_size--;
            if (!prev->next) {  // If bucket is now empty
                dict->len--;
            }
            
            // Clean up node memory
            free_int_vector(current->value);
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

int_v* return_intv_pointer(dict_iv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return NULL;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const ivdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return NULL;
}
// -------------------------------------------------------------------------------- 

void free_intv_dict(dict_iv* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        ivdictNode* current = dict->keyValues[i].next;
        while (current) {      
            ivdictNode* next = current->next;

            free_int_vector(current->value);
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

void _free_intv_dict(dict_iv** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_intv_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// -------------------------------------------------------------------------------- 

bool has_key_intv_dict(const dict_iv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const ivdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

bool insert_intv_dict(dict_iv* dict, const char* key, int_v* value) {
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
    for (ivdictNode* current = dict->keyValues[index].next; current; current = current->next) {
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
    ivdictNode* new_node = malloc(sizeof(ivdictNode));
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

size_t int_dictv_size(const dict_iv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t int_dictv_alloc(const dict_iv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// -------------------------------------------------------------------------------- 

size_t int_dictv_hash_size(const dict_iv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

dict_iv* copy_intv_dict(const dict_iv* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    dict_iv* copy = init_intv_dict();
    if (!copy) {
        return NULL;  // errno already set
    }

    for (size_t i = 0; i < original->alloc; ++i) {
        ivdictNode* current = original->keyValues[i].next;
        while (current) {
            // if (current->value->alloc_type != DYNAMIC) {
            //     free_intv_dict(copy);
            //     errno = EPERM;
            //     return NULL;
            // }

            int_v* vec_copy = copy_int_vector(current->value);
            if (!vec_copy) {
                free_intv_dict(copy);
                return NULL;
            }

            if (!insert_intv_dict(copy, current->key, vec_copy)) {
                free_int_vector(vec_copy);
                free_intv_dict(copy);
                return NULL;
            }

            current = current->next;
        }
    }

    return copy;
}
// -------------------------------------------------------------------------------- 

dict_iv* merge_intv_dict(const dict_iv* dict1, const dict_iv* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Start by deep copying dict1
    dict_iv* merged = copy_intv_dict(dict1);
    if (!merged) {
        return NULL;
    }

    // Now process dict2 entries
    for (size_t i = 0; i < dict2->alloc; ++i) {
        ivdictNode* current = dict2->keyValues[i].next;
        while (current) {
            if (!current->key || !current->value || current->value->alloc_type != DYNAMIC) {
                free_intv_dict(merged);
                errno = EPERM;
                return NULL;
            }

            bool exists = has_key_intv_dict(merged, current->key);
            if (exists && !overwrite) {
                current = current->next;
                continue;
            }

            int_v* vec_copy = copy_int_vector(current->value);
            if (!vec_copy) {
                free_intv_dict(merged);
                return NULL; // errno set by copy_int_vector
            }

            if (exists) {
                pop_intv_dict(merged, current->key);
            }

            if (!insert_intv_dict(merged, current->key, vec_copy)) {
                free_int_vector(vec_copy);
                free_intv_dict(merged);
                return NULL;
            }

            current = current->next;
        }
    }

    return merged;
}
// -------------------------------------------------------------------------------- 

void clear_intv_dict(dict_iv* dict) {
    if (!dict) {
        errno = EINVAL;
        return;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        ivdictNode* current = dict->keyValues[i].next;
        dict->keyValues[i].next = NULL;

        while (current) {
            ivdictNode* next = current->next;

            if (current->value) {
                if (current->value->alloc_type == STATIC) {
                    free(current->value);
                } else {
                    free_int_vector(current->value);
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

bool foreach_intv_dict(const dict_iv* dict, dict_iv_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        ivdictNode* current = dict->keyValues[i].next;
        while (current) {
            iter(current->key, current->value, user_data);
            current = current->next;
        }
    }

    return true;
}
// -------------------------------------------------------------------------------- 

string_v* get_keys_intv_dict(const dict_iv* dict) {
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
        for (ivdictNode* current = dict->keyValues[i].next; current; current = current->next) {
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
// eof
