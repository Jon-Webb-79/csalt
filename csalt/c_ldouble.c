// ================================================================================
// ================================================================================
// - File:    c_ldouble.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 05, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <immintrin.h>  // AVX/SSE
#include "c_ldouble.h"
#include <errno.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

// static const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
// static const size_t hashSize = 16;  //  Size fo hash map init functions
// static const uint32_t HASH_SEED = 0x45d9f3b;
// ================================================================================ 
// ================================================================================ 

ldouble_v* init_ldouble_vector(size_t buff) {
    if (buff == 0) {
        errno = EINVAL;
        return NULL;
    }
    ldouble_v* struct_ptr = malloc(sizeof(ldouble_v));
    if (struct_ptr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
   
    long double* data_ptr = malloc(buff * sizeof(long double));
    if (data_ptr == NULL) {
        free(struct_ptr);
        errno = ENOMEM;
        return NULL; 
    }
   
    // Initialize all elements
    memset(data_ptr, 0, buff * sizeof(long double));
   
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    struct_ptr->alloc_type = DYNAMIC;
    return struct_ptr;
}
// -------------------------------------------------------------------------------- 

long double* c_ldouble_ptr(ldouble_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    return vec->data;
}
// --------------------------------------------------------------------------------

void free_ldouble_vector(ldouble_v* vec) {
   if (!vec || !vec->alloc_type || vec->alloc_type == STATIC) {
       errno = EINVAL;
       return;
   }
   if (vec->data) free(vec->data);
   free(vec);
}
// --------------------------------------------------------------------------------

void _free_ldouble_vector(ldouble_v** vec) {
    if (vec && *vec) {
        free_ldouble_vector(*vec);
        *vec = NULL;
    }
}
// -------------------------------------------------------------------------------- 

bool push_back_ldouble_vector(ldouble_v* vec, const long double value) {
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
        long double* new_data = realloc(vec->data, new_alloc * sizeof(long double));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(long double));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    vec->data[vec->len] = value; 
    vec->len++;
   
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_ldouble_vector(ldouble_v* vec, const long double value) {
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
        if (new_alloc > SIZE_MAX / sizeof(long double)) {
            errno = ERANGE;
            return false;
        }
       
        long double* new_data = realloc(vec->data, new_alloc * sizeof(long double));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(long double));
       
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
        memmove(vec->data + 1, vec->data, vec->len * sizeof(long double));
    }
    
    vec->data[0] = value;    
    vec->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_ldouble_vector(ldouble_v* vec, long double value, size_t index) {
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
        if (new_alloc > SIZE_MAX / sizeof(long double)) {
            errno = ERANGE;
            return false;
        }
       
        long double* new_data = realloc(vec->data, new_alloc * sizeof(long double));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(long double));
       
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
                (vec->len - index) * sizeof(long double));
    }
    
    vec->data[index] = value;
    vec->len++;
    return true;
}
// -------------------------------------------------------------------------------- 


long double pop_back_ldouble_vector(ldouble_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return LDBL_MAX;
    }
    
    // Create copy of last element
    double temp = vec->data[vec->len - 1];
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(long double));
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

long double pop_front_ldouble_vector(ldouble_v* vec) {  // Fixed function name
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LDBL_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return LDBL_MAX;
    }
   
    // Check for overflow in memmove size calculation
    if (vec->len > SIZE_MAX / sizeof(long double)) {
        errno = ERANGE;
        return LDBL_MAX;
    }
   
    // Create copy of first element
    long double temp = vec->data[0];
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(long double));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(long double));
   
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

long double pop_any_ldouble_vector(ldouble_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LDBL_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return LDBL_MAX;
    }
    
    if (index >= vec->len) {
        errno = ERANGE;
        return LDBL_MAX;
    }
    
    // Create copy of element to pop
    long double temp = vec->data[index];
    
    // If not the last element, shift remaining elements left
    if (index < vec->len - 1) {
        // Check for overflow in memmove size calculation
        if ((vec->len - index - 1) > SIZE_MAX / sizeof(long double)) {
            errno = ERANGE;
            return LDBL_MAX;
        }
        
        memmove(&vec->data[index], &vec->data[index + 1], 
                (vec->len - index - 1) * sizeof(long double));
    }
   
    // Clear the last element
    memset(&vec->data[vec->len - 1], 0, sizeof(long double));
    
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

const long double ldouble_vector_index(const ldouble_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return LDBL_MAX;
    }
    return vec->data[index];
}
// // --------------------------------------------------------------------------------
//
const size_t ldouble_vector_size(const ldouble_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->len;
}
// --------------------------------------------------------------------------------

const size_t ldouble_vector_alloc(const ldouble_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->alloc;
}
// --------------------------------------------------------------------------------

void reverse_ldouble_vector(ldouble_v* vec) {
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
       swap_ldouble(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// ================================================================================
// ================================================================================ 
// QUICKSORT

void swap_ldouble(long double* a, long double* b) {
    if (!a || !b) {
        errno = EINVAL;
        return;
    }
    long double temp = *a;
    *a = *b;
    *b = temp;
}
// -------------------------------------------------------------------------------- 

static long double* _median_of_three(long double* a, long double* b, long double* c, iter_dir direction) {
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

static void _insertion_sort(long double* vec, int low, int high, iter_dir direction) {
    for (int i = low + 1; i <= high; i++) {
        long double key = vec[i];
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

static int _partition_ldouble(long double* vec, int low, int high, iter_dir direction) {
    int mid = low + (high - low) / 2;
    long double* pivot_ptr = _median_of_three(&vec[low], &vec[mid], &vec[high], direction);
    
    if (pivot_ptr != &vec[high])
        swap_ldouble(pivot_ptr, &vec[high]);
    
    long double pivot = vec[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_ldouble(&vec[i], &vec[j]);
        }
    }
    swap_ldouble(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// -------------------------------------------------------------------------------- 

static void _quicksort_ldouble(long double* vec, int low, int high, iter_dir direction) {
    while (low < high) {
        if (high - low < 10) {
            _insertion_sort(vec, low, high, direction);
            break;
        }
        
        int pi = _partition_ldouble(vec, low, high, direction);
        
        if (pi - low < high - pi) {
            _quicksort_ldouble(vec, low, pi - 1, direction);
            low = pi + 1;
        } else {
            _quicksort_ldouble(vec, pi + 1, high, direction);
            high = pi - 1;
        }
    }
}
// -------------------------------------------------------------------------------- 

void sort_ldouble_vector(ldouble_v* vec, iter_dir direction) {
    if (!vec) {
        errno = EINVAL;
        return;
    }
    if (vec->len < 2) return;
    
    _quicksort_ldouble(vec->data, 0, vec->len - 1, direction);
}
// -------------------------------------------------------------------------------- 

void trim_ldouble_vector(ldouble_v* vec) {
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
    if (vec->len > SIZE_MAX / sizeof(long double)) {
        errno = ERANGE;
        return;
    }
    
    long double* ptr = realloc(vec->data, sizeof(long double) * vec->len);
    if (ptr == NULL) {
        errno = ENOMEM;
        return;
    }
    
    vec->data = ptr;
    vec->alloc = vec->len;
}
// --------------------------------------------------------------------------------

size_t binary_search_ldouble_vector(ldouble_v* vec, long double value, long double tolerance, bool sort_first) {
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
        sort_ldouble_vector(vec, FORWARD);
    }
    
    size_t left = 0;
    size_t right = vec->len - 1;
    
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        long double diff = vec->data[mid] - value;
        
        // Check if we found a match within tolerance
        if (fabsl(diff) <= tolerance) {
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

void update_ldouble_vector(ldouble_v* vec, size_t index, long double replacement_value) {
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

// long double min_ldouble_vector(ldouble_v* vec) {
//     if (!vec || !vec->data || vec->len == 0) {
//         errno = EINVAL;
//         return LDBL_MAX;
//     }
//
//     long double min_val = LDBL_MAX;
//
// #if defined(__AVX__)
//     __m256d vmin = _mm256_set1_pd(min_val);
//     size_t i = 0;
//
//     for (; i + 3 < vec->len; i += 4) {
//         __m256d v = _mm256_loadu_pd(&vec->data[i]);
//         vmin = _mm256_min_pd(vmin, v);
//     }
//
//     __m128d low = _mm256_castpd256_pd128(vmin);
//     __m128d high = _mm256_extractf128_pd(vmin, 1);
//     __m128d min128 = _mm_min_pd(low, high);
//     min128 = _mm_min_pd(min128, _mm_unpackhi_pd(min128, min128));
//     min_val = _mm_cvtsd_f64(min128);
//
//     for (; i < vec->len; ++i)
//         if (vec->data[i] < min_val)
//             min_val = vec->data[i];
//
// #elif defined(__SSE2__)
//     __m128d vmin = _mm_set1_pd(min_val);
//     size_t i = 0;
//
//     for (; i + 1 < vec->len; i += 2) {
//         __m128d v = _mm_loadu_pd(&vec->data[i]);
//         vmin = _mm_min_pd(vmin, v);
//     }
//
//     vmin = _mm_min_pd(vmin, _mm_unpackhi_pd(vmin, vmin));
//     min_val = _mm_cvtsd_f64(vmin);
//
//     for (; i < vec->len; ++i)
//         if (vec->data[i] < min_val)
//             min_val = vec->data[i];
//
// #else
//     for (size_t i = 0; i < vec->len; ++i)
//         if (vec->data[i] < min_val)
//             min_val = vec->data[i];
// #endif
//
//     return min_val;
// }
// // -------------------------------------------------------------------------------- 
//
// long double max_ldouble_vector(ldouble_v* vec) {
//     if (!vec || !vec->data || vec->len == 0) {
//         errno = EINVAL;
//         return -LDBL_MAX;
//     }
//
//     double max_val = -LDBL_MAX;
//
// #if defined(__AVX__)
//     __m256d vmax = _mm256_set1_pd(max_val);
//     size_t i = 0;
//
//     for (; i + 3 < vec->len; i += 4) {
//         __m256d v = _mm256_loadu_pd(&vec->data[i]);
//         vmax = _mm256_max_pd(vmax, v);
//     }
//
//     // Reduce 4 values to 1 max
//     __m128d low = _mm256_castpd256_pd128(vmax);
//     __m128d high = _mm256_extractf128_pd(vmax, 1);
//     __m128d max128 = _mm_max_pd(low, high);
//     max128 = _mm_max_pd(max128, _mm_unpackhi_pd(max128, max128));
//     max_val = _mm_cvtsd_f64(max128);
//
//     // Scalar fallback for remainder
//     for (; i < vec->len; ++i)
//         if (vec->data[i] > max_val)
//             max_val = vec->data[i];
//
// #elif defined(__SSE2__)
//     __m128d vmax = _mm_set1_pd(max_val);
//     size_t i = 0;
//
//     for (; i + 1 < vec->len; i += 2) {
//         __m128d v = _mm_loadu_pd(&vec->data[i]);
//         vmax = _mm_max_pd(vmax, v);
//     }
//
//     // Reduce 2 values to 1
//     vmax = _mm_max_pd(vmax, _mm_unpackhi_pd(vmax, vmax));
//     max_val = _mm_cvtsd_f64(vmax);
//
//     for (; i < vec->len; ++i)
//         if (vec->data[i] > max_val)
//             max_val = vec->data[i];
//
// #else
//     // Pure scalar fallback
//     for (size_t i = 0; i < vec->len; ++i)
//         if (vec->data[i] > max_val)
//             max_val = vec->data[i];
// #endif
//
//     return max_val;
// }
// // -------------------------------------------------------------------------------- 
//
// long double sum_ldouble_vector(ldouble_v* vec) {
//     if (!vec || !vec->data || vec->len == 0) {
//         errno = EINVAL;
//         return LDBL_MAX;
//     }
//
//     const size_t len = vec->len;
//     const long double* data = vec->data;
//
//     long double sum = 0.0;
//
// #if defined(__AVX__)
//     __m256d vsum = _mm256_setzero_pd();
//     size_t i = 0;
//
//     for (; i + 3 < len; i += 4) {
//         __m256d chunk = _mm256_loadu_pd(&data[i]);
//         vsum = _mm256_add_pd(vsum, chunk);
//     }
//
//     // Horizontal sum of 4 doubles
//     __m128d low  = _mm256_castpd256_pd128(vsum);        // lower 2 doubles
//     __m128d high = _mm256_extractf128_pd(vsum, 1);      // upper 2 doubles
//     __m128d sum128 = _mm_add_pd(low, high);             // add pairs
//     sum128 = _mm_add_pd(sum128, _mm_unpackhi_pd(sum128, sum128)); // final sum
//     sum += _mm_cvtsd_f64(sum128);
//
//     // Handle remaining elements
//     for (; i < len; ++i) {
//         sum += data[i];
//     }
//
// #elif defined(__SSE2__)
//     __m128d vsum = _mm_setzero_pd();
//     size_t i = 0;
//
//     for (; i + 1 < len; i += 2) {
//         __m128d chunk = _mm_loadu_pd(&data[i]);
//         vsum = _mm_add_pd(vsum, chunk);
//     }
//
//     vsum = _mm_add_pd(vsum, _mm_unpackhi_pd(vsum, vsum)); // sum 2 doubles
//     sum += _mm_cvtsd_f64(vsum);
//
//     for (; i < len; ++i) {
//         sum += data[i];
//     }
//
// #else
//     // Fallback to scalar sum
//     for (size_t i = 0; i < len; ++i) {
//         sum += data[i];
//     }
// #endif
//
//     return sum;
// }
// -------------------------------------------------------------------------------- 

// long double average_ldouble_vector(ldouble_v* vec) {
//     if (!vec || !vec->data || vec->len == 0) {
//         errno = EINVAL;
//         return LDBL_MAX;
//     }
//
//     long double sum = sum_ldouble_vector(vec);
//     if (errno != 0) return LDBL_MAX;
//     return sum / vec->len;
// }
//
// // -------------------------------------------------------------------------------- 
//
// long double stdev_ldouble_vector(ldouble_v* vec) {
//     if (!vec || !vec->data || vec->len < 2) {
//         errno = ENODATA;
//         return LDBL_MAX;
//     }
//
//     long double mean = average_ldouble_vector(vec);
//     if (errno != 0) return LDBL_MAX;
//
//     long double sum_sq_diff = 0.0;
//
// #if defined(__AVX__)
//     __m256d vmean = _mm256_set1_pd(mean);
//     __m256d vsum = _mm256_setzero_pd();
//     size_t i = 0;
//
//     for (; i + 3 < vec->len; i += 4) {
//         __m256d v = _mm256_loadu_pd(&vec->data[i]);
//         __m256d diff = _mm256_sub_pd(v, vmean);
//         __m256d sq = _mm256_mul_pd(diff, diff);
//
//         // Check for infinity in any of the 4 values
//         for (int j = 0; j < 4; ++j) {
//             if (isinf(vec->data[i + j])) return INFINITY;
//         }
//
//         vsum = _mm256_add_pd(vsum, sq);
//     }
//
//     // Horizontal sum of vsum
//     __m128d low  = _mm256_castpd256_pd128(vsum);
//     __m128d high = _mm256_extractf128_pd(vsum, 1);
//     __m128d sum128 = _mm_add_pd(low, high);
//     sum128 = _mm_add_pd(sum128, _mm_unpackhi_pd(sum128, sum128));
//     sum_sq_diff += _mm_cvtsd_f64(sum128);
//
//     // Remainder (scalar)
//     for (; i < vec->len; ++i) {
//         if (isinf(vec->data[i])) return INFINITY;
//         double diff = vec->data[i] - mean;
//         sum_sq_diff += diff * diff;
//     }
//
// #elif defined(__SSE2__)
//     __m128d vmean = _mm_set1_pd(mean);
//     __m128d vsum = _mm_setzero_pd();
//     size_t i = 0;
//
//     for (; i + 1 < vec->len; i += 2) {
//         __m128d v = _mm_loadu_pd(&vec->data[i]);
//         __m128d diff = _mm_sub_pd(v, vmean);
//         __m128d sq = _mm_mul_pd(diff, diff);
//
//         for (int j = 0; j < 2; ++j) {
//             if (isinf(vec->data[i + j])) return INFINITY;
//         }
//
//         vsum = _mm_add_pd(vsum, sq);
//     }
//
//     vsum = _mm_add_pd(vsum, _mm_unpackhi_pd(vsum, vsum));
//     sum_sq_diff += _mm_cvtsd_f64(vsum);
//
//     for (; i < vec->len; ++i) {
//         if (isinf(vec->data[i])) return INFINITY;
//         double diff = vec->data[i] - mean;
//         sum_sq_diff += diff * diff;
//     }
//
// #else
//     for (size_t i = 0; i < vec->len; ++i) {
//         if (isinf(vec->data[i])) return INFINITY;
//         double diff = vec->data[i] - mean;
//         sum_sq_diff += diff * diff;
//     }
// #endif
//
//     return sqrt(sum_sq_diff / vec->len);
// }
// // -------------------------------------------------------------------------------- 
//
// ldouble_v* cum_sum_ldouble_vector(ldouble_v* vec) {
//     if (!vec || !vec->data || vec->len == 0) {
//         errno = EINVAL;
//         return NULL;
//     }
//
//     ldouble_v* new_vec = init_ldouble_vector(vec->len);
//     if (!new_vec) {
//         errno = ENOMEM;
//         return NULL;
//     }
//
//     long double sum = 0.0f;
//     for (size_t i = 0; i < vec->len; ++i) {
//         double val = vec->data[i];
//         if (isnan(val)) {
//             errno = EINVAL;
//             free_ldouble_vector(new_vec);
//             return NULL;
//         }
//
//         sum += val;
//
//         if (isinf(sum)) {
//             // Fill rest with infinity
//             for (; i < vec->len; ++i) {
//                 push_back_ldouble_vector(new_vec, INFINITY);
//             }
//             return new_vec;
//         }
//
//         if (!push_back_ldouble_vector(new_vec, sum)) {
//             free_ldouble_vector(new_vec);
//             return NULL;
//         }
//     }
//
//     return new_vec;
// }
// // -------------------------------------------------------------------------------- 
//
// ldouble_v* copy_ldouble_vector(const ldouble_v* original) {
//     if (!original) {
//         errno = EINVAL;
//         return NULL;
//     }
//
//     ldouble_v* copy = init_ldouble_vector(original->alloc);
//     if (!copy) {
//         return NULL;
//     }
//
//     for (size_t i = 0; i < original->len; ++i) {
//         if (!push_back_ldouble_vector(copy, original->data[i])) {
//             free_ldouble_vector(copy);
//             return NULL;
//         }
//     }
//
//     return copy;
// }
// ================================================================================
// ================================================================================
// eof
