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

static const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
static const size_t hashSize = 16;  //  Size fo hash map init functions
static const uint32_t HASH_SEED = 0x45d9f3b; 
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
    return struct_ptr;
}
// -------------------------------------------------------------------------------- 

int* c_int_ptr(int_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    return vec->data;
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

bool push_back_int_vector(int_v* vec, const int value) {
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
        int* new_data = realloc(vec->data, new_alloc * sizeof(int));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(int));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    vec->data[vec->len] = value; 
    vec->len++;
   
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_int_vector(int_v* vec, const int value) {
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
        if (new_alloc > SIZE_MAX / sizeof(int)) {
            errno = ERANGE;
            return false;
        }
       
        int* new_data = realloc(vec->data, new_alloc * sizeof(int));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(int));
       
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
        memmove(vec->data + 1, vec->data, vec->len * sizeof(int));
    }
    
    vec->data[0] = value;    
    vec->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_int_vector(int_v* vec, int value, size_t index) {
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
        if (new_alloc > SIZE_MAX / sizeof(int)) {
            errno = ERANGE;
            return false;
        }
       
        int* new_data = realloc(vec->data, new_alloc * sizeof(int));
        if (!new_data) {
            errno = ENOMEM;
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
            errno = ERANGE;
            return false;
        }
        memmove(vec->data + index + 1, vec->data + index, 
                (vec->len - index) * sizeof(int));
    }
    
    vec->data[index] = value;
    vec->len++;
    return true;
}
// -------------------------------------------------------------------------------- 


int pop_back_int_vector(int_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return INT_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return INT_MAX;
    }
    
    // Create copy of last element
    int temp = vec->data[vec->len - 1];
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(int));
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

int pop_front_int_vector(int_v* vec) {  // Fixed function name
    if (!vec || !vec->data) {
        errno = EINVAL;
        return INT_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return INT_MAX;
    }
   
    // Check for overflow in memmove size calculation
    if (vec->len > SIZE_MAX / sizeof(int)) {
        errno = ERANGE;
        return INT_MAX;
    }
   
    // Create copy of first element
    int temp = vec->data[0];
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(int));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(int));
   
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

int pop_any_int_vector(int_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return INT_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return INT_MAX;
    }
    
    if (index >= vec->len) {
        errno = ERANGE;
        return INT_MAX;
    }
    
    // Create copy of element to pop
    int temp = vec->data[index];
    
    // If not the last element, shift remaining elements left
    if (index < vec->len - 1) {
        // Check for overflow in memmove size calculation
        if ((vec->len - index - 1) > SIZE_MAX / sizeof(int)) {
            errno = ERANGE;
            return INT_MAX;
        }
        
        memmove(&vec->data[index], &vec->data[index + 1], 
                (vec->len - index - 1) * sizeof(int));
    }
   
    // Clear the last element
    memset(&vec->data[vec->len - 1], 0, sizeof(int));
    
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

const int int_vector_index(const int_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return INT_MAX;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return INT_MAX;
    }
    return vec->data[index];
}
// --------------------------------------------------------------------------------

const size_t int_vector_size(const int_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->len;
}
// --------------------------------------------------------------------------------

const size_t int_vector_alloc(const int_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return vec->alloc;
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

    int min_val = INT_MAX;

#if defined(__AVX2__)
    __m256i vmin = _mm256_set1_epi32(min_val);
    size_t i = 0;

    for (; i + 8 <= vec->len; i += 8) {
        __m256i v = _mm256_loadu_si256((__m256i*)&vec->data[i]);
        vmin = _mm256_min_epi32(vmin, v);
    }

    __m128i low = _mm256_castsi256_si128(vmin);
    __m128i high = _mm256_extracti128_si256(vmin, 1);
    __m128i min128 = _mm_min_epi32(low, high);
    min128 = _mm_min_epi32(min128, _mm_shuffle_epi32(min128, _MM_SHUFFLE(2, 3, 0, 1)));
    min128 = _mm_min_epi32(min128, _mm_shuffle_epi32(min128, _MM_SHUFFLE(1, 0, 3, 2)));
    min_val = _mm_cvtsi128_si32(min128);

    for (; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];

#elif defined(__SSE4_1__)
    __m128i vmin = _mm_set1_epi32(min_val);
    size_t i = 0;

    for (; i + 4 <= vec->len; i += 4) {
        __m128i v = _mm_loadu_si128((__m128i*)&vec->data[i]);
        vmin = _mm_min_epi32(vmin, v);
    }

    vmin = _mm_min_epi32(vmin, _mm_shuffle_epi32(vmin, _MM_SHUFFLE(2, 3, 0, 1)));
    vmin = _mm_min_epi32(vmin, _mm_shuffle_epi32(vmin, _MM_SHUFFLE(1, 0, 3, 2)));
    min_val = _mm_cvtsi128_si32(vmin);

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

int max_int_vector(int_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return INT_MAX;
    }

    int max_val = -INT_MAX;

#if defined(__AVX2__)
    __m256i vmax = _mm256_set1_epi32(max_val);
    size_t i = 0;

    for (; i + 8 <= vec->len; i += 8) {
        __m256i v = _mm256_loadu_si256((__m256i*)&vec->data[i]);
        vmax = _mm256_max_epi32(vmax, v);
    }

    __m128i low = _mm256_castsi256_si128(vmax);
    __m128i high = _mm256_extracti128_si256(vmax, 1);
    __m128i max128 = _mm_max_epi32(low, high);
    max128 = _mm_max_epi32(max128, _mm_shuffle_epi32(max128, _MM_SHUFFLE(2, 3, 0, 1)));
    max128 = _mm_max_epi32(max128, _mm_shuffle_epi32(max128, _MM_SHUFFLE(1, 0, 3, 2)));
    max_val = _mm_cvtsi128_si32(max128);

    for (; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];

#elif defined(__SSE4_1__)
    __m128i vmax = _mm_set1_epi32(max_val);
    size_t i = 0;

    for (; i + 4 <= vec->len; i += 4) {
        __m128i v = _mm_loadu_si128((__m128i*)&vec->data[i]);
        vmax = _mm_max_epi32(vmax, v);
    }

    vmax = _mm_max_epi32(vmax, _mm_shuffle_epi32(vmax, _MM_SHUFFLE(2, 3, 0, 1)));
    vmax = _mm_max_epi32(vmax, _mm_shuffle_epi32(vmax, _MM_SHUFFLE(1, 0, 3, 2)));
    max_val = _mm_cvtsi128_si32(vmax);

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

int sum_int_vector(int_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return INT_MAX;
    }

    const size_t len = vec->len;
    const int* data = vec->data;

    int sum = 0;

#if defined(__AVX2__)
    __m256i vsum = _mm256_setzero_si256();
    size_t i = 0;

    for (; i + 8 <= len; i += 8) {
        __m256i chunk = _mm256_loadu_si256((__m256i*)&data[i]);
        vsum = _mm256_add_epi32(vsum, chunk);
    }

    // Reduce 8 lanes to 1 int
    __m128i low = _mm256_castsi256_si128(vsum);
    __m128i high = _mm256_extracti128_si256(vsum, 1);
    __m128i sum128 = _mm_add_epi32(low, high);
    sum128 = _mm_add_epi32(sum128, _mm_shuffle_epi32(sum128, _MM_SHUFFLE(2, 3, 0, 1)));
    sum128 = _mm_add_epi32(sum128, _mm_shuffle_epi32(sum128, _MM_SHUFFLE(1, 0, 3, 2)));
    sum += _mm_cvtsi128_si32(sum128);

    for (; i < len; ++i)
        sum += data[i];

#elif defined(__SSE2__)
    __m128i vsum = _mm_setzero_si128();
    size_t i = 0;

    for (; i + 4 <= len; i += 4) {
        __m128i chunk = _mm_loadu_si128((__m128i*)&data[i]);
        vsum = _mm_add_epi32(vsum, chunk);
    }

    vsum = _mm_add_epi32(vsum, _mm_shuffle_epi32(vsum, _MM_SHUFFLE(2, 3, 0, 1)));
    vsum = _mm_add_epi32(vsum, _mm_shuffle_epi32(vsum, _MM_SHUFFLE(1, 0, 3, 2)));
    sum += _mm_cvtsi128_si32(vsum);

    for (; i < len; ++i)
        sum += data[i];

#else
    for (size_t i = 0; i < len; ++i)
        sum += data[i];
#endif

    return sum;
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

    const int len = vec->len;
    const int* data = vec->data;

    int mean = average_int_vector(vec);
    if (errno != 0) return FLT_MAX;

    double sum_sq_diff = 0.0;

#if defined(__AVX2__)
    __m256i vmean = _mm256_set1_epi32(mean);
    __m256i vsum = _mm256_setzero_si256();
    size_t i = 0;

    for (; i + 8 <= len; i += 8) {
        __m256i v = _mm256_loadu_si256((__m256i*)&data[i]);
        __m256i diff = _mm256_sub_epi32(v, vmean);
        __m256i sq = _mm256_mullo_epi32(diff, diff);
        vsum = _mm256_add_epi32(vsum, sq);
    }

    // Horizontal reduction of vsum
    __m128i low = _mm256_castsi256_si128(vsum);
    __m128i high = _mm256_extracti128_si256(vsum, 1);
    __m128i sum128 = _mm_add_epi32(low, high);
    sum128 = _mm_add_epi32(sum128, _mm_shuffle_epi32(sum128, _MM_SHUFFLE(2, 3, 0, 1)));
    sum128 = _mm_add_epi32(sum128, _mm_shuffle_epi32(sum128, _MM_SHUFFLE(1, 0, 3, 2)));
    sum_sq_diff += _mm_cvtsi128_si32(sum128);

    for (; i < len; ++i) {
        int diff = data[i] - mean;
        sum_sq_diff += (double)(diff * diff);
    }

#elif defined(__SSE2__)
    __m128i vmean = _mm_set1_epi32(mean);
    __m128i vsum = _mm_setzero_si128();
    size_t i = 0;

    for (; i + 4 <= len; i += 4) {
        __m128i v = _mm_loadu_si128((__m128i*)&data[i]);
        __m128i diff = _mm_sub_epi32(v, vmean);
        __m128i sq = _mm_mullo_epi32(diff, diff);
        vsum = _mm_add_epi32(vsum, sq);
    }

    vsum = _mm_add_epi32(vsum, _mm_shuffle_epi32(vsum, _MM_SHUFFLE(2, 3, 0, 1)));
    vsum = _mm_add_epi32(vsum, _mm_shuffle_epi32(vsum, _MM_SHUFFLE(1, 0, 3, 2)));
    sum_sq_diff += _mm_cvtsi128_si32(vsum);

    for (; i < len; ++i) {
        int diff = data[i] - mean;
        sum_sq_diff += (double)(diff * diff);
    }

#else
    for (size_t i = 0; i < len; ++i) {
        int diff = data[i] - mean;
        sum_sq_diff += (double)(diff * diff);
    }
#endif

    return (float)sqrt(sum_sq_diff / len);
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
