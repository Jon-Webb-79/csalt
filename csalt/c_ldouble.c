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

static const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
static const size_t hashSize = 16;  //  Size fo hash map init functions
static const uint32_t HASH_SEED = 0x45d9f3b;
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

long double min_ldouble_vector(ldouble_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return LDBL_MAX;
    }

    long double min_val = vec->data[0];
    for (size_t i = 1; i < vec->len; ++i) {
        if (vec->data[i] < min_val)
            min_val = vec->data[i];
    }

    return min_val;
}
// -------------------------------------------------------------------------------- 

long double max_ldouble_vector(ldouble_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return -LDBL_MAX;
    }

    long double max_val = vec->data[0];
    for (size_t i = 1; i < vec->len; ++i) {
        if (vec->data[i] > max_val)
            max_val = vec->data[i];
    }

    return max_val;
}
// -------------------------------------------------------------------------------- 

long double sum_ldouble_vector(ldouble_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return LDBL_MAX;
    }

    if (vec->len == 0)
        return 0.0;

    long double sum = ldouble_vector_index(vec, 0);
    for (size_t i = 1; i < vec->len; i++) {
        sum += ldouble_vector_index(vec, i);
    }
    return sum;
}
// -------------------------------------------------------------------------------- 

long double average_ldouble_vector(ldouble_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return LDBL_MAX;
    }

    long double sum = sum_ldouble_vector(vec);
    if (errno != 0) return LDBL_MAX;
    return sum / vec->len;
}
// -------------------------------------------------------------------------------- 

long double stdev_ldouble_vector(ldouble_v* vec) {
    if (!vec || !vec->data || vec->len < 2) {
        errno = ENODATA;
        return LDBL_MAX;
    }

    long double mean = average_ldouble_vector(vec);
    if (errno != 0) return LDBL_MAX;

    long double sum_sq_diff = 0.0L;

    for (size_t i = 0; i < vec->len; ++i) {
        if (isinf(vec->data[i])) {
            return INFINITY;
        }

        long double diff = vec->data[i] - mean;
        sum_sq_diff += diff * diff;
    }

    return sqrtl(sum_sq_diff / vec->len);
}
// -------------------------------------------------------------------------------- 

ldouble_v* cum_sum_ldouble_vector(ldouble_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return NULL;
    }

    ldouble_v* new_vec = init_ldouble_vector(vec->len);
    if (!new_vec) {
        errno = ENOMEM;
        return NULL;
    }

    long double sum = 0.0;
    for (size_t i = 0; i < vec->len; ++i) {
        double val = vec->data[i];
        if (isnan(val)) {
            errno = EINVAL;
            free_ldouble_vector(new_vec);
            return NULL;
        }

        sum += val;

        if (isinf(sum)) {
            // Fill rest with infinity
            for (; i < vec->len; ++i) {
                push_back_ldouble_vector(new_vec, INFINITY);
            }
            return new_vec;
        }

        if (!push_back_ldouble_vector(new_vec, sum)) {
            free_ldouble_vector(new_vec);
            return NULL;
        }
    }

    return new_vec;
}
// -------------------------------------------------------------------------------- 

ldouble_v* copy_ldouble_vector(const ldouble_v* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    ldouble_v* copy = init_ldouble_vector(original->alloc);
    if (!copy) {
        return NULL;
    }

    for (size_t i = 0; i < original->len; ++i) {
        if (!push_back_ldouble_vector(copy, original->data[i])) {
            free_ldouble_vector(copy);
            return NULL;
        }
    }

    return copy;
}
// ================================================================================ 
// ================================================================================ 

// DICTIONARY IMPLEMENTATION

typedef struct lddictNode {
    char* key;
    long double value;
    struct lddictNode* next;
} lddictNode;
// --------------------------------------------------------------------------------

struct dict_ld {
    lddictNode* keyValues;
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
static bool resize_dict(dict_ld* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    lddictNode* new_table = calloc(new_size, sizeof(lddictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table in case we need to recover
    lddictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash existing entries
    for (size_t i = 0; i < old_size; i++) {
        lddictNode* current = old_table[i].next;
        
        while (current) {
            lddictNode* next = current->next;  // Save next pointer before modifying node

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
            lddictNode* current = new_table[i].next;
            while (current) {
                lddictNode* next = current->next;
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

dict_ld* init_ldouble_dict(void) {
    // Allocate the dictionary structure
    dict_ld* dict = calloc(1, sizeof(dict_ld));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(lddictNode));
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

bool insert_ldouble_dict(dict_ld* dict, const char* key, long double value) {
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
            printf("Failed at resize\n");
            return false;  // resize_dict sets appropriate errno
        }
    }

    // Rest of the function remains the same as previous improvement...
    const size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    // Check for existing key
    for (lddictNode* current = dict->keyValues[index].next; current; current = current->next) {
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

    lddictNode* new_node = malloc(sizeof(lddictNode));
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

long double pop_ldouble_dict(dict_ld* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    lddictNode* prev = &dict->keyValues[index];
    lddictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Save value and unlink node
            long double value = current->value;
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

long double get_ldouble_dict_value(const dict_ld* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const lddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return FLT_MAX;
}
// --------------------------------------------------------------------------------

void free_ldouble_dict(dict_ld* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        lddictNode* current = dict->keyValues[i].next;
        while (current) {      
            lddictNode* next = current->next;  // Save next pointer before freeing
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

void _free_ldouble_dict(dict_ld** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_ldouble_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// --------------------------------------------------------------------------------

bool update_ldouble_dict(dict_ld* dict, const char* key, long double value) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (lddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return true;
        }
    }

    errno = ENOENT;  // More specific error code for missing key
    return false;
}
// --------------------------------------------------------------------------------

size_t ldouble_dict_size(const dict_ld* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t ldouble_dict_alloc(const dict_ld* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// --------------------------------------------------------------------------------

size_t ldouble_dict_hash_size(const dict_ld* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

bool has_key_ldouble_dict(const dict_ld* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const lddictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

dict_ld* copy_ldouble_dict(const dict_ld* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with same capacity
    dict_ld* new_dict = malloc(sizeof(dict_ld));
    if (!new_dict) {
        errno = ENOMEM;
        return NULL;
    }

    new_dict->keyValues = calloc(dict->alloc, sizeof(lddictNode));
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
        lddictNode* current = dict->keyValues[i].next;
        while (current) {
            // Insert will handle incrementing hash_size and len
            if (!insert_ldouble_dict(new_dict, current->key, current->value)) {
                free_ldouble_dict(new_dict);  // Clean up on failure
                return NULL;
            }
            current = current->next;
        }
    }

    return new_dict;
}
// -------------------------------------------------------------------------------- 

bool clear_ldouble_dict(dict_ld* dict) {
    if (!dict) {
        errno = EINVAL;
        return false;
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        lddictNode* current = dict->keyValues[i].next;
        while (current) {
            lddictNode* next = current->next;
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

string_v* get_keys_ldouble_dict(const dict_ld* dict) {
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
        for (lddictNode* current = dict->keyValues[i].next; current; current = current->next) {
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

ldouble_v* get_values_ldouble_dict(const dict_ld* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }
    ldouble_v* vec = init_ldouble_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }
    // Iterate through all buckets
    for (size_t i = 0; i < dict->alloc; i++) {
        for (lddictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_ldouble_vector(vec, current->value)) {
                free_ldouble_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }
    return vec;
}
// -------------------------------------------------------------------------------- 

dict_ld* merge_ldouble_dict(const dict_ld* dict1, const dict_ld* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with capacity for all items
    //size_t initial_size = dict1->hash_size + dict2->hash_size;
    dict_ld* merged = init_ldouble_dict();
    if (!merged) {
        return NULL;  // errno set by init_long double_dict
    }

    // First, copy all entries from dict1
    for (size_t i = 0; i < dict1->alloc; i++) {
        for (lddictNode* current = dict1->keyValues[i].next; current; current = current->next) {
            if (!insert_ldouble_dict(merged, current->key, current->value)) {
                free_ldouble_dict(merged);
                return NULL;
            }
        }
    }

    // Then handle dict2 entries
    for (size_t i = 0; i < dict2->alloc; i++) {
        for (lddictNode* current = dict2->keyValues[i].next; current; current = current->next) {
            long double existing_value;
            // Check if key exists in merged dictionary
            if ((existing_value = get_ldouble_dict_value(merged, current->key)) != FLT_MAX) {
                if (overwrite) {
                    // Update existing value if overwrite is true
                    if (!update_ldouble_dict(merged, current->key, current->value)) {
                        free_ldouble_dict(merged);
                        return NULL;
                    }
                }
                // If overwrite is false, keep original value
            } else {
                // Key doesn't exist, insert new entry
                if (!insert_ldouble_dict(merged, current->key, current->value)) {
                    free_ldouble_dict(merged);
                    return NULL;
                }
            }
        }
    }

    return merged;
}
// --------------------------------------------------------------------------------

bool foreach_ldouble_dict(const dict_ld* dict, dict_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; i++) {
        for (lddictNode* current = dict->keyValues[i].next; current; current = current->next) {
            iter(current->key, current->value, user_data);
        }
    }

    return true;
}
// ================================================================================ 
// ================================================================================ 

typedef struct ldvdictNode {
    char* key;
    ldouble_v* value;
    struct ldvdictNode* next;
} ldvdictNode;
// --------------------------------------------------------------------------------

struct dict_ldv {
    ldvdictNode* keyValues;
    size_t hash_size;
    size_t len;
    size_t alloc;
};
// --------------------------------------------------------------------------------

dict_ldv* init_ldoublev_dict(void) {
    // Allocate the dictionary structure
    dict_ldv* dict = calloc(1, sizeof(dict_ldv));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate vector dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(ldvdictNode));
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

static bool resize_dictv(dict_ldv* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better hash distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    ldvdictNode* new_table = calloc(new_size, sizeof(ldvdictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table for cleanup if something fails
    ldvdictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash all existing entries into the new table
    for (size_t i = 0; i < old_size; ++i) {
        ldvdictNode* current = old_table[i].next;
        while (current) {
            ldvdictNode* next = current->next;

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
        // Rollback: disconnect moved nodes so they aren't ldouble-freed
        for (size_t i = 0; i < new_size; ++i) {
            ldvdictNode* current = new_table[i].next;
            while (current) {
                ldvdictNode* next = current->next;
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

bool create_ldoublev_dict(dict_ldv* dict, char* key, size_t size) {
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
    for (ldvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
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

    ldvdictNode* new_node = malloc(sizeof(ldvdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    ldouble_v* value = NULL;
    value = init_ldouble_vector(size);
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

bool pop_ldoublev_dict(dict_ldv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    ldvdictNode* prev = &dict->keyValues[index];
    ldvdictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            prev->next = current->next;
            
            // Update dictionary metadata
            dict->hash_size--;
            if (!prev->next) {  // If bucket is now empty
                dict->len--;
            }
            
            // Clean up node memory
            free_ldouble_vector(current->value);
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

ldouble_v* return_ldoublev_pointer(dict_ldv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return NULL;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const ldvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return NULL;
}
// -------------------------------------------------------------------------------- 

void free_ldoublev_dict(dict_ldv* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        ldvdictNode* current = dict->keyValues[i].next;
        while (current) {      
            ldvdictNode* next = current->next;

            free_ldouble_vector(current->value);
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

void _free_ldoublev_dict(dict_ldv** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_ldoublev_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// -------------------------------------------------------------------------------- 

bool has_key_ldoublev_dict(const dict_ldv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const ldvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

bool insert_ldoublev_dict(dict_ldv* dict, const char* key, ldouble_v* value) {
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
    for (ldvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
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
    ldvdictNode* new_node = malloc(sizeof(ldvdictNode));
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

size_t ldouble_dictv_size(const dict_ldv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t ldouble_dictv_alloc(const dict_ldv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// -------------------------------------------------------------------------------- 

size_t ldouble_dictv_hash_size(const dict_ldv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

dict_ldv* copy_ldoublev_dict(const dict_ldv* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    dict_ldv* copy = init_ldoublev_dict();
    if (!copy) {
        return NULL;  // errno already set
    }

    for (size_t i = 0; i < original->alloc; ++i) {
        ldvdictNode* current = original->keyValues[i].next;
        while (current) {
            // if (current->value->alloc_type != DYNAMIC) {
            //     free_ldoublev_dict(copy);
            //     errno = EPERM;
            //     return NULL;
            // }

            ldouble_v* vec_copy = copy_ldouble_vector(current->value);
            if (!vec_copy) {
                free_ldoublev_dict(copy);
                return NULL;
            }

            if (!insert_ldoublev_dict(copy, current->key, vec_copy)) {
                free_ldouble_vector(vec_copy);
                free_ldoublev_dict(copy);
                return NULL;
            }

            current = current->next;
        }
    }

    return copy;
}
// -------------------------------------------------------------------------------- 

dict_ldv* merge_ldoublev_dict(const dict_ldv* dict1, const dict_ldv* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Start by deep copying dict1
    dict_ldv* merged = copy_ldoublev_dict(dict1);
    if (!merged) {
        return NULL;
    }

    // Now process dict2 entries
    for (size_t i = 0; i < dict2->alloc; ++i) {
        ldvdictNode* current = dict2->keyValues[i].next;
        while (current) {
            if (!current->key || !current->value || current->value->alloc_type != DYNAMIC) {
                free_ldoublev_dict(merged);
                errno = EPERM;
                return NULL;
            }

            bool exists = has_key_ldoublev_dict(merged, current->key);
            if (exists && !overwrite) {
                current = current->next;
                continue;
            }

            ldouble_v* vec_copy = copy_ldouble_vector(current->value);
            if (!vec_copy) {
                free_ldoublev_dict(merged);
                return NULL; // errno set by copy_ldouble_vector
            }

            if (exists) {
                pop_ldoublev_dict(merged, current->key);
            }

            if (!insert_ldoublev_dict(merged, current->key, vec_copy)) {
                free_ldouble_vector(vec_copy);
                free_ldoublev_dict(merged);
                return NULL;
            }

            current = current->next;
        }
    }

    return merged;
}
// -------------------------------------------------------------------------------- 

void clear_ldoublev_dict(dict_ldv* dict) {
    if (!dict) {
        errno = EINVAL;
        return;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        ldvdictNode* current = dict->keyValues[i].next;
        dict->keyValues[i].next = NULL;

        while (current) {
            ldvdictNode* next = current->next;

            if (current->value) {
                if (current->value->alloc_type == STATIC) {
                    free(current->value);
                } else {
                    free_ldouble_vector(current->value);
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

bool foreach_ldoublev_dict(const dict_ldv* dict, dict_ldv_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        ldvdictNode* current = dict->keyValues[i].next;
        while (current) {
            iter(current->key, current->value, user_data);
            current = current->next;
        }
    }

    return true;
}
// -------------------------------------------------------------------------------- 

string_v* get_keys_ldoublev_dict(const dict_ldv* dict) {
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
        for (ldvdictNode* current = dict->keyValues[i].next; current; current = current->next) {
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
