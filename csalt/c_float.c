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

#include <immintrin.h>  // AVX/SSE
#include "c_float.h"
#include <errno.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

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
    return struct_ptr;
}
// -------------------------------------------------------------------------------- 

float* c_float_ptr(float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    return vec->data;
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
        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(float));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    vec->data[vec->len] = value; 
    vec->len++;
   
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_float_vector(float_v* vec, const float value) {
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
        if (new_alloc > SIZE_MAX / sizeof(float)) {
            errno = ERANGE;
            return false;
        }
       
        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(float));
       
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
        memmove(vec->data + 1, vec->data, vec->len * sizeof(float));
    }
    
    vec->data[0] = value;    
    vec->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_float_vector(float_v* vec, float value, size_t index) {
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
        if (new_alloc > SIZE_MAX / sizeof(float)) {
            errno = ERANGE;
            return false;
        }
       
        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            errno = ENOMEM;
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
            errno = ERANGE;
            return false;
        }
        memmove(vec->data + index + 1, vec->data + index, 
                (vec->len - index) * sizeof(float));
    }
    
    vec->data[index] = value;
    vec->len++;
    return true;
}
// -------------------------------------------------------------------------------- 


float pop_back_float_vector(float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return FLT_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }
    
    // Create copy of last element
    float temp = vec->data[vec->len - 1];
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(float));
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

float pop_front_float_vector(float_v* vec) {  // Fixed function name
    if (!vec || !vec->data) {
        errno = EINVAL;
        return FLT_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }
   
    // Check for overflow in memmove size calculation
    if (vec->len > SIZE_MAX / sizeof(float)) {
        errno = ERANGE;
        return FLT_MAX;
    }
   
    // Create copy of first element
    float temp = vec->data[0];
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(float));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(float));
   
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

float pop_any_float_vector(float_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return FLT_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }
    
    if (index >= vec->len) {
        errno = ERANGE;
        return FLT_MAX;
    }
    
    // Create copy of element to pop
    float temp = vec->data[index];
    
    // If not the last element, shift remaining elements left
    if (index < vec->len - 1) {
        // Check for overflow in memmove size calculation
        if ((vec->len - index - 1) > SIZE_MAX / sizeof(float)) {
            errno = ERANGE;
            return FLT_MAX;
        }
        
        memmove(&vec->data[index], &vec->data[index + 1], 
                (vec->len - index - 1) * sizeof(float));
    }
   
    // Clear the last element
    memset(&vec->data[vec->len - 1], 0, sizeof(float));
    
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

const float float_vector_index(const float_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return FLT_MAX;
    }
    return vec->data[index];
}
// --------------------------------------------------------------------------------

const size_t float_vector_size(const float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->len;
}
// --------------------------------------------------------------------------------

const size_t float_vector_alloc(const float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->alloc;
}
// --------------------------------------------------------------------------------

void reverse_float_vector(float_v* vec) {
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
       swap_float(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// ================================================================================
// ================================================================================ 
// QUICKSORT

void swap_float(float* a, float* b) {
    if (!a || !b) {
        errno = EINVAL;
        return;
    }
    float temp = *a;
    *a = *b;
    *b = temp;
}
// -------------------------------------------------------------------------------- 

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
    if (!vec) {
        errno = EINVAL;
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

    float min_val = FLT_MAX;

#if defined(__AVX__)
    __m256 vmin = _mm256_set1_ps(min_val);
    size_t i = 0;

    for (; i + 7 < vec->len; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        vmin = _mm256_min_ps(vmin, v);
    }

    __m128 low = _mm256_castps256_ps128(vmin);
    __m128 high = _mm256_extractf128_ps(vmin, 1);
    __m128 min128 = _mm_min_ps(low, high);
    min128 = _mm_min_ps(min128, _mm_movehl_ps(min128, min128));
    min128 = _mm_min_ps(min128, _mm_shuffle_ps(min128, min128, 0x1));
    min_val = _mm_cvtss_f32(min128);

    for (; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];

#elif defined(__SSE__)
    __m128 vmin = _mm_set1_ps(min_val);
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m128 v = _mm_loadu_ps(&vec->data[i]);
        vmin = _mm_min_ps(vmin, v);
    }

    vmin = _mm_min_ps(vmin, _mm_movehl_ps(vmin, vmin));
    vmin = _mm_min_ps(vmin, _mm_shuffle_ps(vmin, vmin, 0x1));
    min_val = _mm_cvtss_f32(vmin);

    for (; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];

#else
    for (size_t i = 0; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];
#endif

    return min_val;
}

// -------------------------------------------------------------------------------- 

float max_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }

    float max_val = -FLT_MAX;

#if defined(__AVX__)
    __m256 vmax = _mm256_set1_ps(max_val);
    size_t i = 0;

    for (; i + 7 < vec->len; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        vmax = _mm256_max_ps(vmax, v);
    }

    __m128 low = _mm256_castps256_ps128(vmax);
    __m128 high = _mm256_extractf128_ps(vmax, 1);
    __m128 max128 = _mm_max_ps(low, high);
    max128 = _mm_max_ps(max128, _mm_movehl_ps(max128, max128));
    max128 = _mm_max_ps(max128, _mm_shuffle_ps(max128, max128, 0x1));
    max_val = _mm_cvtss_f32(max128);

    for (; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];

#elif defined(__SSE__)
    __m128 vmax = _mm_set1_ps(max_val);
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m128 v = _mm_loadu_ps(&vec->data[i]);
        vmax = _mm_max_ps(vmax, v);
    }

    vmax = _mm_max_ps(vmax, _mm_movehl_ps(vmax, vmax));
    vmax = _mm_max_ps(vmax, _mm_shuffle_ps(vmax, vmax, 0x1));
    max_val = _mm_cvtss_f32(vmax);

    for (; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];

#else
    for (size_t i = 0; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];
#endif

    return max_val;
}

// -------------------------------------------------------------------------------- 

float sum_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }

    const size_t len = vec->len;
    const float* data = vec->data;

    float sum = 0.0f;

#if defined(__AVX__)
    __m256 vsum = _mm256_setzero_ps();
    size_t i = 0;

    for (; i + 7 < len; i += 8) {
        __m256 chunk = _mm256_loadu_ps(&data[i]);
        vsum = _mm256_add_ps(vsum, chunk);
    }

    // Horizontal sum of vsum
    __m128 low  = _mm256_castps256_ps128(vsum);         // lower 128
    __m128 high = _mm256_extractf128_ps(vsum, 1);       // upper 128
    __m128 sum128 = _mm_add_ps(low, high);              // add lower + upper

    // Shuffle and reduce
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum += _mm_cvtss_f32(sum128);

    // Handle remaining elements
    for (; i < len; ++i) {
        sum += data[i];
    }

#elif defined(__SSE__)
    __m128 vsum = _mm_setzero_ps();
    size_t i = 0;

    for (; i + 3 < len; i += 4) {
        __m128 chunk = _mm_loadu_ps(&data[i]);
        vsum = _mm_add_ps(vsum, chunk);
    }

    vsum = _mm_hadd_ps(vsum, vsum);
    vsum = _mm_hadd_ps(vsum, vsum);
    sum += _mm_cvtss_f32(vsum);

    for (; i < len; ++i) {
        sum += data[i];
    }

#else
    // Fallback to scalar
    for (size_t i = 0; i < len; ++i) {
        sum += data[i];
    }
#endif

    return sum;
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

    float mean = average_float_vector(vec);
    if (errno != 0) return FLT_MAX;

    float sum_sq_diff = 0.0f;

#if defined(__AVX__)
    __m256 vmean = _mm256_set1_ps(mean);
    __m256 vsum = _mm256_setzero_ps();
    size_t i = 0;

    for (; i + 7 < vec->len; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        __m256 diff = _mm256_sub_ps(v, vmean);
        __m256 sq = _mm256_mul_ps(diff, diff);
        if (!_mm256_testz_ps(sq, sq)) { // Skip all-zero check — we need isinf
            for (int j = 0; j < 8; ++j) {
                float x = vec->data[i + j];
                if (isinf(x)) return INFINITY;
            }
        }
        vsum = _mm256_add_ps(vsum, sq);
    }

    __m128 low = _mm256_castps256_ps128(vsum);
    __m128 high = _mm256_extractf128_ps(vsum, 1);
    __m128 sum128 = _mm_add_ps(low, high);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum_sq_diff += _mm_cvtss_f32(sum128);

    for (; i < vec->len; ++i) {
        float diff = vec->data[i] - mean;
        float sq = diff * diff;
        if (isinf(vec->data[i])) return INFINITY;
        sum_sq_diff += sq;
    }

#elif defined(__SSE__)
    __m128 vmean = _mm_set1_ps(mean);
    __m128 vsum = _mm_setzero_ps();
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m128 v = _mm_loadu_ps(&vec->data[i]);
        __m128 diff = _mm_sub_ps(v, vmean);
        __m128 sq = _mm_mul_ps(diff, diff);
        for (int j = 0; j < 4; ++j) {
            if (isinf(vec->data[i + j])) return INFINITY;
        }
        vsum = _mm_add_ps(vsum, sq);
    }

    vsum = _mm_hadd_ps(vsum, vsum);
    vsum = _mm_hadd_ps(vsum, vsum);
    sum_sq_diff += _mm_cvtss_f32(vsum);

    for (; i < vec->len; ++i) {
        if (isinf(vec->data[i])) return INFINITY;
        float diff = vec->data[i] - mean;
        sum_sq_diff += diff * diff;
    }

#else
    for (size_t i = 0; i < vec->len; ++i) {
        if (isinf(vec->data[i])) return INFINITY;
        float diff = vec->data[i] - mean;
        sum_sq_diff += diff * diff;
    }
#endif

    return sqrtf(sum_sq_diff / vec->len);
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

    float result = 0.0f;

#if defined(__AVX__)
    __m256 sum_vec = _mm256_setzero_ps();
    size_t i = 0;

    for (; i + 8 <= len; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        sum_vec = _mm256_add_ps(sum_vec, _mm256_mul_ps(va, vb));
    }

    float sum_arr[8];
    _mm256_storeu_ps(sum_arr, sum_vec);
    for (int j = 0; j < 8; ++j) {
        result += sum_arr[j];
    }

#elif defined(__SSE__)
    __m128 sum_vec = _mm_setzero_ps();
    size_t i = 0;

    for (; i + 4 <= len; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        sum_vec = _mm_add_ps(sum_vec, _mm_mul_ps(va, vb));
    }

    float sum_arr[4];
    _mm_storeu_ps(sum_arr, sum_vec);
    for (int j = 0; j < 4; ++j) {
        result += sum_arr[j];
    }

#else
    size_t i = 0;
#endif

    // Scalar tail for AVX, SSE, or full scalar path
#if defined(__AVX__)
    for (; i < len; ++i)
        result += a[i] * b[i];
#elif defined(__SSE__)
    for (; i < len; ++i)
        result += a[i] * b[i];
#else
    for (size_t i = 0; i < len; ++i)
        result += a[i] * b[i];
#endif

    return result;
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

matrix_f* create_float_dense_matrix(size_t rows, size_t cols) {
    errno = 0;

    matrix_f* mat = malloc(sizeof(matrix_f));
    if (!mat) {
        errno = ENOMEM;
        return NULL;
    }

    float* data_ptr = calloc(rows * cols, sizeof(float));
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

matrix_f* create_float_coo_matrix(size_t rows, size_t cols) {
    errno = 0;

    matrix_f* mat = malloc(sizeof(matrix_f));
    if (!mat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t* row_indices = malloc(INITIAL_COO_CAPACITY * sizeof(size_t));
    size_t* col_indices = malloc(INITIAL_COO_CAPACITY * sizeof(size_t));
    float* values = malloc(INITIAL_COO_CAPACITY * sizeof(float));

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

matrix_f* create_float_csr_matrix(size_t rows, size_t cols, size_t nnz) {
    errno = 0;

    matrix_f* mat = malloc(sizeof(matrix_f));
    if (!mat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t* row_ptrs = calloc(rows + 1, sizeof(size_t));
    size_t* col_indices = malloc(nnz * sizeof(size_t));
    float* values = malloc(nnz * sizeof(float));

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

matrix_f* create_float_matrix(size_t rows, size_t cols, size_t estimated_nnz) {
    errno = 0;

    size_t total = rows * cols;

    if (total < MIN_SPARSE_SIZE) {
        return create_float_dense_matrix(rows, cols);
    }

    float estimated_density = (float)estimated_nnz / total;

    if (estimated_density < DENSE_THRESHOLD) {
        // Very sparse → Start with COO (can convert to CSR later)
        return create_float_coo_matrix(rows, cols);
    } else {
        // Semi-sparse or dense enough → Start dense
        return create_float_dense_matrix(rows, cols);
    }
}
// -------------------------------------------------------------------------------- 

void free_float_matrix(matrix_f* mat) {
    if (!mat) return;

    switch (mat->type) {
        case DENSE_MATRIX:
            free_float_dense_matrix(mat);
            break;

        case SPARSE_CSR_MATRIX:
            free_float_csr_matrix(mat);
            break;

        case SPARSE_COO_MATRIX:
            free_float_coo_matrix(mat);
            break;

        default:
            // Unknown or uninitialized type
            break;
    }
}
// -------------------------------------------------------------------------------- 

void _free_float_matrix(matrix_f** mat) {
    if (mat && *mat) {
        free_float_matrix(*mat);
        *mat = NULL;
    }
}
// -------------------------------------------------------------------------------- 

void free_float_dense_matrix(matrix_f* mat) {
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

void free_float_csr_matrix(matrix_f* mat) {
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

void free_float_coo_matrix(matrix_f* mat) {
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

bool is_float_element_initialized(const matrix_f* mat, size_t row, size_t col) {
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

static void set_element_initialized(matrix_f* mat, size_t row, size_t col, bool initialized) {
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

static bool set_dense_matrix(matrix_f* mat, size_t row, size_t col, float value, bool allow_updates) {
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
    bool initialized = is_float_element_initialized(mat, row, col);
    
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

bool insert_float_dense_matrix(matrix_f* mat, size_t row, size_t col, float value) {
    return set_dense_matrix(mat, row, col, value, false);
}
// -------------------------------------------------------------------------------- 

bool update_float_dense_matrix(matrix_f* mat, size_t row, size_t col, float value) {
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

static bool set_coo_matrix(matrix_f* mat, size_t row, size_t col, float value, bool allow_updates) {
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
        float*  new_vals = realloc(mat->storage.coo.values, new_capacity * sizeof(float));
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
    memmove(&mat->storage.coo.values[left + 1], &mat->storage.coo.values[left], (mat->count - left) * sizeof(float));

    // Insert new entry
    mat->storage.coo.rows[left] = row;
    mat->storage.coo.cols[left] = col;
    mat->storage.coo.values[left] = value;
    mat->count++;

    return true;
}
// -------------------------------------------------------------------------------- 

bool insert_float_coo_matrix(matrix_f* mat, size_t row, size_t col, float value) {
    return set_coo_matrix(mat, row, col, value, false);
}
// -------------------------------------------------------------------------------- 

bool update_float_coo_matrix(matrix_f* mat, size_t row, size_t col, float value) {
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

static bool set_csr_matrix(matrix_f* mat, size_t row, size_t col, float value, bool allow_updates) {
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

bool insert_float_csr_matrix(matrix_f* mat, size_t row, size_t col, float value) {
    return set_csr_matrix(mat, row, col, value, false);
}
// -------------------------------------------------------------------------------- 

bool update_float_csr_matrix(matrix_f* mat, size_t row, size_t col, float value) {
    return set_csr_matrix(mat, row, col, value, true);
}
// -------------------------------------------------------------------------------- 

static matrix_f* convert_float_dense_to_coo(const matrix_f* dense_mat) {
    errno = 0;

    if (!dense_mat || dense_mat->type != DENSE_MATRIX ||
        !dense_mat->storage.dense.data || !dense_mat->storage.dense.init) {
        errno = EINVAL;
        return NULL;
    }

    size_t rows = dense_mat->rows;
    size_t cols = dense_mat->cols;
    size_t count = dense_mat->count;

    matrix_f* coo_mat = malloc(sizeof(matrix_f));
    if (!coo_mat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t* rows_arr = malloc(count * sizeof(size_t));
    size_t* cols_arr = malloc(count * sizeof(size_t));
    float* values_arr = malloc(count * sizeof(float));
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

static matrix_f* convert_float_coo_to_csr(const matrix_f* coo_mat) {
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
    float* values = malloc(nnz * sizeof(float));
    if (!row_ptrs || !col_indices || !values) {
        free(row_ptrs); free(col_indices); free(values);
        errno = ENOMEM;
        return NULL;
    }

    // Create sortable array of (row, col, value)
    typedef struct {
        size_t row, col;
        float value;
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

    matrix_f* csr_mat = malloc(sizeof(matrix_f));
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
void maybe_convert_float_matrix(matrix_f** pmat, bool convert_to_csr) {
    matrix_f* mat = *pmat;

    size_t total = mat->rows * mat->cols;

    // If dense and large or sparse → convert to COO
    if (mat->type == DENSE_MATRIX) {
        float density = (float)mat->count / total;
        if (total > MAX_DENSE_ELEMENTS && density < SPARSE_THRESHOLD) {
            matrix_f* new_mat = convert_float_dense_to_coo(mat);
            if (new_mat) {
                free_float_matrix(mat);
                *pmat = new_mat;
            }
        }
    }
    // If COO and too large → convert to CSR
    else if (mat->type == SPARSE_COO_MATRIX && convert_to_csr) {
        if (mat->count >= COO_TO_CSR_TRIGGER) {
            matrix_f* new_mat = convert_float_coo_to_csr(mat);
            if (new_mat) {
                free_float_matrix(mat);
                *pmat = new_mat;
            }
        }
    }
    // CSR doesn't auto-convert
}
// -------------------------------------------------------------------------------- 

bool insert_float_matrix(matrix_f** pmat, size_t row, size_t col, float value,
                         bool convert_to_csr) {
    if (!pmat || !*pmat) {
        errno = EINVAL;
        return false;
    }

    maybe_convert_float_matrix(pmat, convert_to_csr);  // Can replace *pmat with a new matrix
    matrix_f* mat = *pmat;

    switch (mat->type) {
        case DENSE_MATRIX:
            return insert_float_dense_matrix(mat, row, col, value);
        case SPARSE_COO_MATRIX:
            return insert_float_coo_matrix(mat, row, col, value);
        case SPARSE_CSR_MATRIX:
            return insert_float_csr_matrix(mat, row, col, value);
        default:
            errno = EINVAL;
            return false;
    }
}
// -------------------------------------------------------------------------------- 

bool update_float_matrix(matrix_f* mat, size_t row, size_t col, float value) {
    errno = 0;
    if (!mat) {
        errno = EINVAL;
        return false;
    }

    switch (mat->type) {
        case DENSE_MATRIX:
            return update_float_dense_matrix(mat, row, col, value);
        case SPARSE_COO_MATRIX:
            return update_float_coo_matrix(mat, row, col, value);
        case SPARSE_CSR_MATRIX:
            return update_float_csr_matrix(mat, row, col, value);
        default:
            errno = EINVAL;
            return false;
    }
}
// -------------------------------------------------------------------------------- 

float get_float_dense_matrix(const matrix_f* mat, size_t row, size_t col) {
    errno = 0;
    if (!mat || !mat->storage.dense.data || !mat->storage.dense.init || mat->type != DENSE_MATRIX) {
        errno = EINVAL;
        return FLT_MAX;
    }

    if (row >= mat->rows || col >= mat->cols) {
        errno = ERANGE;
        return FLT_MAX;
    }

    if (!is_float_element_initialized(mat, row, col)) {
        errno = EINVAL;
        return FLT_MAX;
    }

    return mat->storage.dense.data[row * mat->cols + col];
}
// -------------------------------------------------------------------------------- 

float get_float_coo_matrix(const matrix_f* mat, size_t row, size_t col) {
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

float get_float_csr_matrix(const matrix_f* mat, size_t row, size_t col) {
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

float get_float_matrix(const matrix_f* mat, size_t row, size_t col) {
    errno = 0;
    if (!mat) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (mat->type == DENSE_MATRIX) return get_float_dense_matrix(mat, row, col);
    else if (mat->type == SPARSE_COO_MATRIX) return get_float_coo_matrix(mat, row, col);
    else if (mat->type == SPARSE_CSR_MATRIX) return get_float_csr_matrix(mat, row, col);
    else {
        errno = EINVAL;
        return FLT_MAX;
    }
}
// -------------------------------------------------------------------------------- 

float pop_float_dense_matrix(matrix_f* mat, size_t row, size_t col) {
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

    if (!is_float_element_initialized(mat, row, col)) {
        errno = ENOENT;
        return FLT_MAX;
    }

    size_t index = row * mat->cols + col;
    float value = mat->storage.dense.data[index];

    // Mark as uninitialized
    set_element_initialized(mat, row, col, false);
    mat->storage.dense.data[index] = 0.0f;  // Optional

    return value;
}
// -------------------------------------------------------------------------------- 

float pop_float_coo_matrix(matrix_f* mat, size_t row, size_t col) {
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

        float value = mat->storage.coo.values[left];
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
                    elements_to_move * sizeof(float));
        }

        mat->count--;
        return value;
    }

    errno = ENOENT;
    return FLT_MAX;
}
// -------------------------------------------------------------------------------- 

static void maybe_compact_csr(matrix_f* mat, size_t original_nnz) {
    if (!mat || mat->type != SPARSE_CSR_MATRIX || original_nnz == 0) return;

    float tombstone_ratio = (float)(original_nnz - mat->count) / original_nnz;

    if (tombstone_ratio > CSR_COMPACT_THRESHOLD) {
        compact_float_csr_matrix(mat);
    }
}
// -------------------------------------------------------------------------------- 

float pop_float_csr_matrix(matrix_f* mat, size_t row, size_t col) {
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
    float value = mat->storage.csr.values[index];

    // Mark entry as tombstone
    mat->storage.csr.col_indices[index] = CSR_TOMBSTONE_COL;
    mat->storage.csr.values[index] = 0.0f;
    size_t original_nnz = mat->count;
    mat->count--;

    maybe_compact_csr(mat, original_nnz);

    return value;
}
// -------------------------------------------------------------------------------- 

void compact_float_csr_matrix(matrix_f* mat) {
    errno = 0;
    if (!mat || mat->type != SPARSE_CSR_MATRIX) return;

    size_t nnz = mat->count;
    // size_t new_capacity = nnz;
    size_t* new_col_indices = malloc(nnz * sizeof(size_t));
    float* new_values = malloc(nnz * sizeof(float));
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

static matrix_f* convert_float_csr_to_coo(const matrix_f* csr_mat) {
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
    float* values   = malloc(nnz * sizeof(float));
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

    matrix_f* coo_mat = malloc(sizeof(matrix_f));
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

static matrix_f* convert_float_coo_to_dense(const matrix_f* coo_mat) {
    errno = 0;

    if (!coo_mat || coo_mat->type != SPARSE_COO_MATRIX ||
        !coo_mat->storage.coo.rows || !coo_mat->storage.coo.cols || !coo_mat->storage.coo.values) {
        errno = EINVAL;
        return NULL;
    }

    size_t rows = coo_mat->rows;
    size_t cols = coo_mat->cols;
    size_t nnz  = coo_mat->count;

    float* data = calloc(rows * cols, sizeof(float));
    uint8_t* init = calloc(rows * cols, sizeof(uint8_t));  // ← fixed type
    if (!data || !init) {
        free(data); free(init);
        errno = ENOMEM;
        return NULL;
    }

    for (size_t i = 0; i < nnz; ++i) {
        size_t r = coo_mat->storage.coo.rows[i];
        size_t c = coo_mat->storage.coo.cols[i];
        float  v = coo_mat->storage.coo.values[i];
        size_t idx = r * cols + c;
        data[idx] = v;
        init[idx] = 1;  // ← use 1 instead of `true`
    }

    matrix_f* dense_mat = malloc(sizeof(matrix_f));
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

void convert_floatMat_to_dense(matrix_f** pmat) {
    errno = 0;
    matrix_f* mat = *pmat;
    if (!mat) {
        errno = EINVAL;
        return;
    }

    if (mat->type == DENSE_MATRIX) return;

    if (mat->type == SPARSE_COO_MATRIX) {
        matrix_f* new_mat = convert_float_coo_to_dense(mat);
        if (!new_mat) return;
        free_float_matrix(mat);
        *pmat = new_mat;
    } else if (mat->type == SPARSE_CSR_MATRIX) {
        matrix_f* coo_mat = convert_float_csr_to_coo(mat);
        if (!coo_mat) return;
        free_float_matrix(mat);
        matrix_f* dense_mat = convert_float_coo_to_dense(coo_mat);
        free_float_matrix(coo_mat);
        if (!dense_mat) return;
        *pmat = dense_mat;
    } else {
        errno = EINVAL;
    }
}
// -------------------------------------------------------------------------------- 

void convert_floatMat_to_coo(matrix_f** pmat) {
    errno = 0;
    matrix_f* mat = *pmat;
    if (!mat) {
        errno = EINVAL;
        return;
    }

    if (mat->type == SPARSE_COO_MATRIX) return;

    if (mat->type == DENSE_MATRIX) {
        matrix_f* new_mat = convert_float_dense_to_coo(mat);
        if (!new_mat) return;
        free_float_matrix(mat);
        *pmat = new_mat;
    } else if (mat->type == SPARSE_CSR_MATRIX) {
        matrix_f* new_mat = convert_float_csr_to_coo(mat);
        if (!new_mat) return;
        free_float_matrix(mat);
        *pmat = new_mat;
    } else {
        errno = EINVAL;
    }
}
// -------------------------------------------------------------------------------- 

void convert_floatMat_to_csr(matrix_f** pmat) {
    errno = 0;
    matrix_f* mat = *pmat;
    if (!mat) {
        errno = EINVAL;
        return;
    }

    if (mat->type == SPARSE_CSR_MATRIX) return;

    if (mat->type == SPARSE_COO_MATRIX) {
        matrix_f* new_mat = convert_float_coo_to_csr(mat);
        if (!new_mat) return;
        free_float_matrix(mat);
        *pmat = new_mat;
    } else if (mat->type == DENSE_MATRIX) {
        matrix_f* coo_mat = convert_float_dense_to_coo(mat);
        if (!coo_mat) return;
        free_float_matrix(mat);
        matrix_f* csr_mat = convert_float_coo_to_csr(coo_mat);
        free_float_matrix(coo_mat);
        if (!csr_mat) return;
        *pmat = csr_mat;
    } else {
        errno = EINVAL;
    }
}
// -------------------------------------------------------------------------------- 

static bool should_downgrade_float_matrix(const matrix_f* mat) {
    if (!mat || mat->type != SPARSE_CSR_MATRIX) return false;

    size_t total = mat->rows * mat->cols;
    if (total == 0) return false;

    float density = (float)mat->count / total;
    return density < CSR_COMPACT_THRESHOLD;
}
// -------------------------------------------------------------------------------- 

static void maybe_downgrade_matrix_format(matrix_f** pmat) {
    if (!pmat || !*pmat) return;
    matrix_f* mat = *pmat;

    // Step 1: Try CSR → COO downgrade
    if (mat->type == SPARSE_CSR_MATRIX && should_downgrade_float_matrix(mat)) {
        matrix_f* coo = convert_float_csr_to_coo(mat);
        if (coo) {
            free_float_matrix(mat);
            mat = coo;
            *pmat = mat;
        }
    }

    // Step 2: Try COO → Dense downgrade
    if (mat->type == SPARSE_COO_MATRIX) {
        size_t total = mat->rows * mat->cols;
        if (total < MIN_SPARSE_SIZE || ((float)mat->count / total) > DENSE_THRESHOLD) {
            matrix_f* dense = convert_float_coo_to_dense(mat);
            if (dense) {
                free_float_matrix(mat);
                *pmat = dense;
            }
        }
    }
}
// -------------------------------------------------------------------------------- 

float pop_float_matrix(matrix_f** pmat, size_t row, size_t col) {
    errno = 0;
    if (!pmat || !*pmat) {
        errno = EINVAL;
        return FLT_MAX;
    }

    matrix_f* mat = *pmat;
    float result = FLT_MAX;

    if (mat->type == DENSE_MATRIX)
        result = pop_float_dense_matrix(mat, row, col);
    else if (mat->type == SPARSE_COO_MATRIX)
        result = pop_float_coo_matrix(mat, row, col);
    else if (mat->type == SPARSE_CSR_MATRIX)
        result = pop_float_csr_matrix(mat, row, col);
    else {
        errno = EINVAL;
        return FLT_MAX;
    }

    // Try format downgrade (e.g., CSR → COO → DENSE)
    maybe_downgrade_matrix_format(pmat);

    return result;
}
// -------------------------------------------------------------------------------- 

bool finalize_float_matrix(matrix_f** mat_ptr) {
    errno = 0;
    if (!mat_ptr || !*mat_ptr) {
        errno = EINVAL;
        return false;
    }
    maybe_convert_float_matrix(mat_ptr, true);  // allow_csr = true
    return true;
}
// -------------------------------------------------------------------------------- 

MatrixStorageType get_float_matrix_type(matrix_f* mat) {
    errno = 0;
    if (!mat) {
        errno = 0;
        return 0;
    }
    return mat->type;
}
// -------------------------------------------------------------------------------- 

size_t get_float_matrix_rows(matrix_f* mat) {
    errno = 0;
    if (!mat) {
        errno = 0;
        return (size_t)-1;
    }
    return mat->rows;
}
// -------------------------------------------------------------------------------- 

size_t get_float_matrix_cols(matrix_f* mat) {
    errno = 0;
    if (!mat) {
        errno = 0;
        return (size_t)-1;
    }
    return mat->cols;
}
// -------------------------------------------------------------------------------- 

size_t get_float_matrix_element_count(matrix_f* mat) {
    errno = 0;
    if (!mat) {
        errno = 0;
        return (size_t)-1;
    }
    return mat->count;
}
// -------------------------------------------------------------------------------- 

matrix_f* invert_float_dense_matrix(const matrix_f* mat) {
    errno = 0;

    if (!mat || mat->type != DENSE_MATRIX || mat->rows != mat->cols) {
        errno = EINVAL;
        return NULL;
    }

    size_t n = mat->rows;
    matrix_f* inverse = create_float_matrix(n, n, 0);
    if (!inverse) {
        errno = ENOMEM;
        return NULL;
    }

    float* a = malloc(n * n * sizeof(float));
    if (!a) {
        free_float_matrix(inverse);
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
        float max = fabsf(a[i * n + i]);
        for (size_t j = i + 1; j < n; ++j) {
            float temp = fabsf(a[j * n + i]);
            if (temp > max) {
                max = temp;
                pivot = j;
            }
        }

        if (fabsf(a[pivot * n + i]) < 1e-8f) {
            free(a);
            free_float_matrix(inverse);
            errno = ERANGE;
            return NULL;
        }

        // Swap rows in A and inverse
        if (pivot != i) {
            for (size_t j = 0; j < n; ++j) {
                float tmp = a[i * n + j];
                a[i * n + j] = a[pivot * n + j];
                a[pivot * n + j] = tmp;

                tmp = inverse->storage.dense.data[i * n + j];
                inverse->storage.dense.data[i * n + j] = inverse->storage.dense.data[pivot * n + j];
                inverse->storage.dense.data[pivot * n + j] = tmp;
            }
        }

        float pivot_val = a[i * n + i];

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
            float factor = a[k * n + i];
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

bool transpose_float_dense_matrix(matrix_f** pmat) {
    errno = 0;
    if (!pmat || !*pmat || (*pmat)->type != DENSE_MATRIX) {
        errno = EINVAL;
        return false;
    }

    matrix_f* mat = *pmat;
    size_t rows = mat->rows;
    size_t cols = mat->cols;
    float* data = mat->storage.dense.data;

    // Case 1: Square matrix — transpose in place
    if (rows == cols) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = i + 1; j < cols; ++j) {
                size_t idx1 = i * cols + j;
                size_t idx2 = j * cols + i;
                float tmp = data[idx1];
                data[idx1] = data[idx2];
                data[idx2] = tmp;
            }
        }
        return true;
    }

    // Case 2: Rectangular matrix — allocate new transposed matrix
    matrix_f* transposed = create_float_dense_matrix(cols, rows);
    if (!transposed) {
        errno = ENOMEM;
        return false;
    }

    //float* new_data = transposed->storage.dense.data;
    for (size_t i = 0; i < get_float_matrix_rows(mat); ++i) {
        for (size_t j = 0; j < get_float_matrix_cols(mat); ++j) {
            float value = get_float_dense_matrix(mat, i, j);   // Access original at (i, j)
            insert_float_dense_matrix(transposed, j, i, value); // Store transposed at (j, i)
        }
    }

    free_float_matrix(mat);
    *pmat = transposed;
    return true;
}
// -------------------------------------------------------------------------------- 

bool transpose_float_coo_matrix(matrix_f** pmat) {
    errno = 0;

    if (!pmat || !*pmat || (*pmat)->type != SPARSE_COO_MATRIX) {
        errno = EINVAL;
        return false;
    }

    matrix_f* mat = *pmat;

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

bool transpose_float_csr_matrix(matrix_f** pmat) {
    errno = 0;

    if (!pmat || !*pmat || (*pmat)->type != SPARSE_CSR_MATRIX) {
        errno = EINVAL;
        return false;
    }

    matrix_f* mat = *pmat;
    size_t rows = mat->rows;
    size_t cols = mat->cols;
    size_t nnz = mat->count;

    size_t* row_ptrs_T = calloc(cols + 1, sizeof(size_t));
    size_t* col_indices_T = malloc(nnz * sizeof(size_t));
    float* values_T = malloc(nnz * sizeof(float));
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
            float val = mat->storage.csr.values[i];

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

bool transpose_float_matrix(matrix_f** pmat) {
    errno = 0;

    if (!pmat || !*pmat) {
        errno = EINVAL;
        return false;
    }

    matrix_f* mat = *pmat;

    switch (mat->type) {
        case DENSE_MATRIX:
            return transpose_float_dense_matrix(pmat);

        case SPARSE_COO_MATRIX:
            return transpose_float_coo_matrix(pmat);

        case SPARSE_CSR_MATRIX:
            return transpose_float_csr_matrix(pmat);

        default:
            errno = EINVAL;
            return false;
    }
}
// -------------------------------------------------------------------------------- 

matrix_f* copy_float_dense_matrix(const matrix_f* mat) {
    errno = 0;
    if (!mat || mat->type != DENSE_MATRIX) {
        errno = EINVAL;
        return NULL;
    }

    matrix_f* newMat = create_float_dense_matrix(mat->rows, mat->cols);
    if (!newMat) {
        errno = ENOMEM;
        return NULL;
    }

    newMat->count = mat->count;

    size_t total_elements = mat->rows * mat->cols;
    memcpy(newMat->storage.dense.data, mat->storage.dense.data, total_elements * sizeof(float));
    memcpy(newMat->storage.dense.init, mat->storage.dense.init, total_elements * sizeof(uint8_t));

    return newMat;
}
// -------------------------------------------------------------------------------- 

matrix_f* copy_float_coo_matrix(const matrix_f* mat) {
    errno = 0;

    if (!mat || mat->type != SPARSE_COO_MATRIX) {
        errno = EINVAL;
        return NULL;
    }

    matrix_f* newMat = malloc(sizeof(matrix_f));
    if (!newMat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t cap = mat->storage.coo.capacity;
    size_t* row_indices = malloc(cap * sizeof(size_t));
    size_t* col_indices = malloc(cap * sizeof(size_t));
    float* values = malloc(cap * sizeof(float));

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
    memcpy(values, mat->storage.coo.values, mat->count * sizeof(float));

    return newMat;
}
// -------------------------------------------------------------------------------- 

matrix_f* copy_float_csr_matrix(const matrix_f* mat) {
    errno = 0;

    if (!mat || mat->type != SPARSE_CSR_MATRIX) {
        errno = EINVAL;
        return NULL;
    }

    size_t rows = mat->rows;
    size_t cols = mat->cols;
    size_t nnz = mat->count;

    matrix_f* newMat = malloc(sizeof(matrix_f));
    if (!newMat) {
        errno = ENOMEM;
        return NULL;
    }

    size_t* row_ptrs = calloc(rows + 1, sizeof(size_t));
    size_t* col_indices = malloc(nnz * sizeof(size_t));
    float* values = malloc(nnz * sizeof(float));

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
    memcpy(values, mat->storage.csr.values, nnz * sizeof(float));

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

matrix_f* copy_float_matrix(const matrix_f* mat) {
    errno = 0;

    if (!mat) {
        errno = EINVAL;
        return false;
    }

    switch (mat->type) {
        case DENSE_MATRIX:
            return copy_float_dense_matrix(mat);

        case SPARSE_COO_MATRIX:
            return copy_float_coo_matrix(mat);

        case SPARSE_CSR_MATRIX:
            return copy_float_csr_matrix(mat);

        default:
            errno = EINVAL;
            return false;
    }
}
// -------------------------------------------------------------------------------- 

float float_dense_matrix_det(const matrix_f* mat) {
    errno = 0;

    if (!mat || mat->type != DENSE_MATRIX || mat->rows != mat->cols) {
        errno = EINVAL;
        return NAN;
    }

    size_t n = mat->rows;
    float* original = mat->storage.dense.data;

    float* temp = malloc(n * n * sizeof(float));
    if (!temp) {
        errno = ENOMEM;
        return NAN;
    }

    memcpy(temp, original, n * n * sizeof(float));
    float det = 1.0f;

    for (size_t i = 0; i < n; ++i) {
        // Pivot selection
        size_t pivot = i;
        float max_val = fabsf(temp[i * n + i]);
        for (size_t j = i + 1; j < n; ++j) {
            float val = fabsf(temp[j * n + i]);
            if (val > max_val) {
                max_val = val;
                pivot = j;
            }
        }

        if (max_val < 1e-8f) {
            free(temp);
            return 0.0f; // Singular matrix
        }

        if (pivot != i) {
            for (size_t k = 0; k < n; ++k) {
                float tmp = temp[i * n + k];
                temp[i * n + k] = temp[pivot * n + k];
                temp[pivot * n + k] = tmp;
            }
            det *= -1.0f;
        }

        float pivot_val = temp[i * n + i];
        det *= pivot_val;

        for (size_t j = i + 1; j < n; ++j) {
            float factor = temp[j * n + i] / pivot_val;

#if defined(__AVX__)
            size_t k = i;
            for (; k + 8 <= n; k += 8) {
                __m256 row_i = _mm256_loadu_ps(&temp[i * n + k]);
                __m256 row_j = _mm256_loadu_ps(&temp[j * n + k]);
                __m256 scaled = _mm256_mul_ps(row_i, _mm256_set1_ps(factor));
                __m256 result = _mm256_sub_ps(row_j, scaled);
                _mm256_storeu_ps(&temp[j * n + k], result);
            }

            // Handle tail
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
