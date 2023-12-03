// ================================================================================
// ================================================================================
// - File:    heap.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 01, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/heap.h"
// ================================================================================
// ================================================================================
// Constants for memory management

const size_t HEAP_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
const size_t HEAP_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
// ================================================================================
// ================================================================================
// INIT FUNCTIONS

char_min_hp* init_char_min_heap(size_t buff) {
    char_min_hp* struct_ptr = malloc(sizeof(char_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    char* data_ptr = malloc(buff * sizeof(char));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

uchar_min_hp* init_uchar_min_heap(size_t buff) {
    uchar_min_hp* struct_ptr = malloc(sizeof(uchar_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned char* data_ptr = malloc(buff * sizeof(unsigned char));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

short_min_hp* init_short_min_heap(size_t buff) {
    short_min_hp* struct_ptr = malloc(sizeof(short_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    short int* data_ptr = malloc(buff * sizeof(short int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

ushort_min_hp* init_ushort_min_heap(size_t buff) {
    ushort_min_hp* struct_ptr = malloc(sizeof(ushort_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned short int* data_ptr = malloc(buff * sizeof(unsigned short int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

int_min_hp* init_int_min_heap(size_t buff) {
    int_min_hp* struct_ptr = malloc(sizeof(int_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    int* data_ptr = malloc(buff * sizeof(int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

uint_min_hp* init_uint_min_heap(size_t buff) {
    uint_min_hp* struct_ptr = malloc(sizeof(uint_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned int* data_ptr = malloc(buff * sizeof(unsigned int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

long_min_hp* init_long_min_heap(size_t buff) {
    long_min_hp* struct_ptr = malloc(sizeof(long_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    long int* data_ptr = malloc(buff * sizeof(long int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

ulong_min_hp* init_ulong_min_heap(size_t buff) {
    ulong_min_hp* struct_ptr = malloc(sizeof(ulong_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned long int* data_ptr = malloc(buff * sizeof(unsigned long int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

llong_min_hp* init_llong_min_heap(size_t buff) {
    llong_min_hp* struct_ptr = malloc(sizeof(llong_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    long long int* data_ptr = malloc(buff * sizeof(long long int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

ullong_min_hp* init_ullong_min_heap(size_t buff) {
    ullong_min_hp* struct_ptr = malloc(sizeof(ullong_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned long long int* data_ptr = malloc(buff * sizeof(unsigned long long int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

float_min_hp* init_float_min_heap(size_t buff) {
    float_min_hp* struct_ptr = malloc(sizeof(float_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    float* data_ptr = malloc(buff * sizeof(float));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

double_min_hp* init_double_min_heap(size_t buff) {
    double_min_hp* struct_ptr = malloc(sizeof(double_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    double* data_ptr = malloc(buff * sizeof(double));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

ldouble_min_hp* init_ldouble_min_heap(size_t buff) {
    ldouble_min_hp* struct_ptr = malloc(sizeof(ldouble_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    long double* data_ptr = malloc(buff * sizeof(long double));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

bool_min_hp* init_bool_min_heap(size_t buff) {
    bool_min_hp* struct_ptr = malloc(sizeof(bool_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    bool* data_ptr = malloc(buff * sizeof(bool));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

string_min_hp* init_string_min_heap(size_t buff) {
    string_min_hp* struct_ptr = malloc(sizeof(string_min_hp));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    str* data_ptr = malloc(buff * sizeof(str));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    for (size_t i = 0; i < buff; i++) {
        data_ptr[i].data = NULL;
        data_ptr[i].len = 0;
        data_ptr[i].alloc = 0;
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

InitHeapFunc init_min_heap(dtype dat_type) {
    switch(dat_type) {
        case dChar: return (InitHeapFunc)init_char_min_heap;
        case dUChar: return (InitHeapFunc)init_uchar_min_heap;
        case dShort: return (InitHeapFunc)init_short_min_heap;
        case dUShort: return (InitHeapFunc)init_ushort_min_heap;
        case dInt: return (InitHeapFunc)init_int_min_heap;
        case dUInt: return (InitHeapFunc)init_uint_min_heap;
        case dLong: return (InitHeapFunc)init_long_min_heap;
        case dULong: return (InitHeapFunc)init_ulong_min_heap;
        case dLongLong: return (InitHeapFunc)init_llong_min_heap;
        case dULongLong: return (InitHeapFunc)init_ullong_min_heap;
        case dFloat: return (InitHeapFunc)init_float_min_heap;
        case dDouble: return (InitHeapFunc)init_double_min_heap;
        case dLDouble: return (InitHeapFunc)init_ldouble_min_heap;
        case dBool: return (InitHeapFunc)init_bool_min_heap;
        case dString: return (InitHeapFunc)init_string_min_heap;
        default: return NULL;
    }
}
// ================================================================================
// ================================================================================
// PRIVATE FUNCTION FOR FREE MEMORY 

static void _free_str(str *s) {
    if ( s == NULL ) return;
    free(s->data);
    s->data = NULL;
}
// ================================================================================
// ================================================================================
// FREE FUNCTIONS 

void free_char_min_heap(char_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_uchar_min_heap(uchar_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_short_min_heap(short_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ushort_min_heap(ushort_min_hp* vec) {
    if (vec == NULL) { 
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_int_min_heap(int_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_uint_min_heap(uint_min_hp* vec) {
    if (vec == NULL) { 
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_long_min_heap(long_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ulong_min_heap(ulong_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_llong_min_heap(llong_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ullong_min_heap(ullong_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_float_min_heap(float_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_double_min_heap(double_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ldouble_min_heap(ldouble_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_bool_min_heap(bool_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_string_min_heap(string_min_hp* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }

    // Free each individual string in the vector
    for (size_t i = 0; i < vec->len; ++i) {
        // Assume _free_string takes a pointer to a str struct and frees it
        _free_str(&vec->data[i]);
    }

    // Now that all the str data has been freed, it's safe to free the array
    free(vec->data); // Free the array of str structs
    vec->data = NULL; // Set the data pointer to NULL to avoid use-after-free

    // Finally, free the string_min_hp struct itself
    free(vec); // Free the string_min_hp struct pointer
}
// ================================================================================
// ================================================================================
// Functions for garbage collection 

void _free_char_min_heap(char_min_hp** vec) {
    if (vec && *vec) {
        free_char_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_uchar_min_heap(uchar_min_hp** vec) {
    if (vec && *vec) {
        free_uchar_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_short_min_heap(short_min_hp** vec) {
    if (vec && *vec) {
        free_short_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ushort_min_heap(ushort_min_hp** vec) {
    if (vec && *vec) {
        free_ushort_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_int_min_heap(int_min_hp** vec) {
    if (vec && *vec) {
        free_int_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_uint_min_heap(uint_min_hp** vec) {
    if (vec && *vec) {
        free_uint_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_long_min_heap(long_min_hp** vec) {
    if (vec && *vec) {
        free_long_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ulong_min_heap(ulong_min_hp** vec) {
    if (vec && *vec) {
        free_ulong_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_llong_min_heap(llong_min_hp** vec) {
    if (vec && *vec) {
        free_llong_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ullong_min_heap(ullong_min_hp** vec) {
    if (vec && *vec) {
        free_ullong_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_float_min_heap(float_min_hp** vec) {
    if (vec && *vec) {
        free_float_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_double_min_heap(double_min_hp** vec) {
    if (vec && *vec) {
        free_double_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ldouble_min_heap(ldouble_min_hp** vec) {
    if (vec && *vec) {
        free_ldouble_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_bool_min_heap(bool_min_hp** vec) {
    if (vec && *vec) {
        free_bool_min_heap(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_sstring_min_heap(string_min_hp** vec) {
    if (vec && *vec) {
        free_string_min_heap(*vec);
    }
}
// ================================================================================
// ================================================================================
// SIFT UP MIN HEAP

static void _sift_min_heap_up_char(char_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_char(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_uchar(uchar_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_uchar(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_short(short_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_short(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_ushort(ushort_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_ushort(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_int(int_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_int(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_uint(uint_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_uint(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_long(long_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_long(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_ulong(ulong_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_ulong(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_llong(llong_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_llong(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_ullong(ullong_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_ullong(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_float(float_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_float(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_double(double_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_double(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_ldouble(ldouble_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_ldouble(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_bool(bool_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;

        if (heap->data[index] >= heap->data[parent_index]) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_bool(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_up_string(string_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }
    
    int val;
    while (index > 0) {
        size_t parent_index = (index - 1) / 2;
        val = compare_strings_str(&heap->data[index], &heap->data[parent_index]);
        if (val >= 0) {
            break; // Heap property is satisfied
        }

        // Swap the element with its parent
        swap_string(&heap->data[index], &heap->data[parent_index]);
        
        // Move up the index
        index = parent_index;
    }
}
// ================================================================================
// ================================================================================
// SIFT DOWN MIN HEAP

static void _sift_min_heap_down_char(char_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_char(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_char(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_uchar(uchar_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_uchar(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_uchar(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_short(short_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_short(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_short(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_ushort(ushort_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_ushort(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_ushort(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_int(int_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_int(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_int(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_uint(uint_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_uint(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_uint(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_long(long_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_long(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_long(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_ulong(ulong_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_ulong(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_ulong(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_llong(llong_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_llong(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_llong(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_ullong(ullong_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_ullong(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_ullong(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_float(float_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_float(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_float(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_double(double_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_double(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_double(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_ldouble(ldouble_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_ldouble(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_ldouble(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_bool(bool_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    if (left_child < heap->len && heap->data[left_child] < heap->data[smallest]) {
        smallest = left_child;
    }

    if (right_child < heap->len && heap->data[right_child] < heap->data[smallest]) {
        smallest = right_child;
    }

    if (smallest != index) {
        // Swap the element with the smallest child
        swap_bool(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_bool(heap, smallest);
    }
}
// --------------------------------------------------------------------------------

static void _sift_min_heap_down_string(string_min_hp* heap, size_t index) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return; // Invalid heap or data pointer, or index out of bounds
    }

    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t smallest = index;

    // Check if left child exists and is not NULL
    if (left_child < heap->len && heap->data[left_child].data != NULL) {
        int val = compare_strings_str(&heap->data[left_child], &heap->data[smallest]);
        if (val < 0) {
            smallest = left_child;
        }
    }

    // Check if right child exists and is not NULL
    if (right_child < heap->len && heap->data[right_child].data != NULL) {
        int val = compare_strings_str(&heap->data[right_child], &heap->data[smallest]);
        if (val < 0) {
            smallest = right_child;
        }
    }
    
    if (smallest != index) {
        // Swap the element with the smallest child
        swap_string(&heap->data[index], &heap->data[smallest]);

        // Recursively heapify down
        _sift_min_heap_down_string(heap, smallest);
    }
}
// ================================================================================
// ================================================================================
// ENQUEUE MIN HEAP

bool enqueue_min_heap_char(char_min_hp* heap, char element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        char* new_data_ptr = realloc(heap->data, new_alloc * sizeof(char));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_char(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_uchar(uchar_min_hp* heap, unsigned char element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        unsigned char* new_data_ptr = realloc(heap->data, new_alloc * sizeof(unsigned char));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_uchar(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_short(short_min_hp* heap, short int element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        short int* new_data_ptr = realloc(heap->data, new_alloc * sizeof(short int));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_short(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_ushort(ushort_min_hp* heap, unsigned short int element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        unsigned short int* new_data_ptr = realloc(heap->data, new_alloc * sizeof(unsigned short int));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_ushort(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_int(int_min_hp* heap, int element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        int* new_data_ptr = realloc(heap->data, new_alloc * sizeof(int));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_int(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_uint(uint_min_hp* heap, unsigned int element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        unsigned int* new_data_ptr = realloc(heap->data, new_alloc * sizeof(unsigned int));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_uint(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_long(long_min_hp* heap, long int element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        long int* new_data_ptr = realloc(heap->data, new_alloc * sizeof(long int));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_long(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_ulong(ulong_min_hp* heap, unsigned long int element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        unsigned long int* new_data_ptr = realloc(heap->data, new_alloc * sizeof(unsigned long int));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_ulong(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_llong(llong_min_hp* heap, long long int element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        long long int* new_data_ptr = realloc(heap->data, new_alloc * sizeof(long long int));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_llong(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_ullong(ullong_min_hp* heap, unsigned long long int element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        unsigned long long int* new_data_ptr = realloc(heap->data, new_alloc * sizeof(unsigned long long int));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_ullong(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_float(float_min_hp* heap, float element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        float* new_data_ptr = realloc(heap->data, new_alloc * sizeof(float));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_float(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_double(double_min_hp* heap, double element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        double* new_data_ptr = realloc(heap->data, new_alloc * sizeof(double));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_double(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_ldouble(ldouble_min_hp* heap, long double element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        long double* new_data_ptr = realloc(heap->data, new_alloc * sizeof(long double));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_ldouble(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_bool(bool_min_hp* heap, bool element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        bool* new_data_ptr = realloc(heap->data, new_alloc * sizeof(bool));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }

    // Add the new element at the end of the heap
    heap->data[heap->len] = element;
    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_bool(heap, heap->len - 1);

    return true; // Element added successfully 
}
// --------------------------------------------------------------------------------

bool enqueue_min_heap_string(string_min_hp* heap, char* element) {
    if (heap == NULL || heap->data == NULL) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer
    }

    if (heap->len >= heap->alloc) {
        size_t new_alloc = heap->alloc == 0 ? 1: heap->alloc;
        if (new_alloc < HEAP_THRESHOLD) {
            new_alloc *= 2;
        }
        else {
            new_alloc += HEAP_FIXED_AMOUNT;
        }
        str* new_data_ptr = realloc(heap->data, new_alloc * sizeof(str));
        if (new_data_ptr == NULL) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data = new_data_ptr;
        heap->alloc = new_alloc;
    }
    size_t str_len = strlen(element);
    heap->data[heap->len].data = malloc(str_len + 1); // +1 for the null terminator
    if (!heap->data[heap->len].data) {
        errno = ENOMEM;
        return false;
    } 
    strcpy(heap->data[heap->len].data, element);
    heap->data[heap->len].alloc = str_len + 1;
    heap->data[heap->len].len = str_len;

    heap->len++;

    // Perform heapify up to maintain the heap property
    _sift_min_heap_up_string(heap, heap->len - 1);

    return true; // Element added successfully 
}
// ================================================================================
// ================================================================================
// DEQUEUE MIN HEAP 

char dequeue_min_heap_char(char_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    char smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_char(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

unsigned char dequeue_min_heap_uchar(uchar_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    unsigned char smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_uchar(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

short int dequeue_min_heap_short(short_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    short int smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_short(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

unsigned short int dequeue_min_heap_ushort(ushort_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    unsigned short int smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_ushort(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

int dequeue_min_heap_int(int_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    int smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_int(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

unsigned int dequeue_min_heap_uint(uint_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    unsigned int smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_uint(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

long int dequeue_min_heap_long(long_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    long int smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_long(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

unsigned long int dequeue_min_heap_ulong(ulong_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    unsigned long int smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_ulong(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

long long int dequeue_min_heap_llong(llong_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    long long int smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_llong(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

unsigned long long int dequeue_min_heap_ullong(ullong_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    unsigned long long int smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_ullong(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

float dequeue_min_heap_float(float_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    float smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_float(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

double dequeue_min_heap_double(double_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    double smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_double(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

long double dequeue_min_heap_ldouble(ldouble_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return 0; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    long double smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_ldouble(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

bool dequeue_min_heap_bool(bool_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return false; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    bool smallest = heap->data[0];

    // Replace the root with the last element in the heap.
    heap->data[0] = heap->data[heap->len - 1];
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_bool(heap, 0);

    return smallest;
}
// --------------------------------------------------------------------------------

str* dequeue_min_heap_string(string_min_hp* heap) {
    if (heap == NULL || heap->data == NULL || heap->len == 0) {
        // Handle the error condition (e.g., return a sentinel value or set an error flag)
        // You may want to modify the error handling based on your requirements.
        errno = EINVAL;
        return NULL; // Returning 0 as a placeholder for error handling.
    }

    // The smallest element is always at the root (index 0).
    str* smallest = init_string_nol(heap->data[0].data);

    // Replace the root with the last element in the heap.
    if (heap->data[0].alloc < heap->data[heap->len - 1].len + 1) {
        char* ptr = realloc(heap->data[0].data, heap->data[heap->len - 1].len + 1);
        if (!ptr) {
            errno = ENOMEM;
            return NULL; // Returning NULL for memory allocation failure.
        }
        heap->data[0].data = ptr;
    }

    // Copy the data from the last element to the root.
    strcpy(heap->data[0].data, heap->data[heap->len - 1].data);

    // Update length and allocation for the root element.
    heap->data[0].len = heap->data[heap->len - 1].len;
    heap->data[0].alloc = heap->data[heap->len - 1].len + 1;

    // Free memory for the last element's data.
    free(heap->data[heap->len - 1].data); 
    heap->len--;

    // Perform heapify down to maintain the min-heap property.
    _sift_min_heap_down_string(heap, 0);

    return smallest;
}
// ================================================================================
// ================================================================================
// HEAP LENGTH FUNCTIONS 

size_t char_min_heap_length(char_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t uchar_min_heap_length(uchar_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t short_min_heap_length(short_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t ushort_min_heap_length(ushort_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t int_min_heap_length(int_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t uint_min_heap_length(uint_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t long_min_heap_length(long_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t ulong_min_heap_length(ulong_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t llong_min_heap_length(llong_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t ullong_min_heap_length(ullong_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t float_min_heap_length(float_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t double_min_heap_length(double_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t ldouble_min_heap_length(ldouble_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t bool_min_heap_length(bool_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// --------------------------------------------------------------------------------

size_t string_min_heap_length(string_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->len;
}
// ================================================================================
// ================================================================================
// GET HEAP MEMORY FUNCTIONS 

size_t char_min_heap_memory(char_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t uchar_min_heap_memory(uchar_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t short_min_heap_memory(short_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t ushort_min_heap_memory(ushort_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t int_min_heap_memory(int_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t uint_min_heap_memory(uint_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t long_min_heap_memory(long_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t ulong_min_heap_memory(ulong_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t llong_min_heap_memory(llong_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t ullong_min_heap_memory(ullong_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t float_min_heap_memory(float_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t double_min_heap_memory(double_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t ldouble_min_heap_memory(ldouble_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t bool_min_heap_memory(bool_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// --------------------------------------------------------------------------------

size_t string_min_heap_memory(string_min_hp* heap) {
    if (!heap) {
        errno = EINVAL;
        return 0;
    }
    return heap->alloc;
}
// ================================================================================
// ================================================================================
// REPLACE INDEX FUNCTIONS

bool replace_char_min_heap_index(char_min_hp* heap, size_t index, char new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_char(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_uchar_min_heap_index(uchar_min_hp* heap, size_t index, unsigned char new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_uchar(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_short_min_heap_index(short_min_hp* heap, size_t index, short int new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_short(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_ushort_min_heap_index(ushort_min_hp* heap, size_t index, unsigned short int new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_ushort(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_int_min_heap_index(int_min_hp* heap, size_t index, int new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_int(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_uint_min_heap_index(uint_min_hp* heap, size_t index, unsigned int new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_uint(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_long_min_heap_index(long_min_hp* heap, size_t index, long int new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_long(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_ulong_min_heap_index(ulong_min_hp* heap, size_t index, unsigned long int new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_ulong(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_llong_min_heap_index(llong_min_hp* heap, size_t index, long long int new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_llong(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_ullong_min_heap_index(ullong_min_hp* heap, size_t index, unsigned long long int new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_ullong(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_float_min_heap_index(float_min_hp* heap, size_t index, float new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_float(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_double_min_heap_index(double_min_hp* heap, size_t index, double new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_double(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_ldouble_min_heap_index(ldouble_min_hp* heap, size_t index, long double new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_ldouble(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_bool_min_heap_index(bool_min_hp* heap, size_t index, bool new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    // Update the value at the specified index
    heap->data[index] = new_value;

    // Perform heapify down to maintain the Min Heap property
    _sift_min_heap_down_bool(heap, index);

    return true; // Value replaced successfully
}
// --------------------------------------------------------------------------------

bool replace_string_min_heap_index(string_min_hp* heap, size_t index, char* new_value) {
    if (heap == NULL || heap->data == NULL || index >= heap->len) {
        errno = EINVAL;
        return false; // Invalid heap or data pointer, or index out of bounds
    }

    size_t str_len = strlen(new_value);

    // Check if reallocation is needed
    if (heap->data[index].alloc < str_len + 1) {
        char* ptr = realloc(heap->data[index].data, str_len + 1);
        if (!ptr) {
            errno = ENOMEM;
            return false; // Memory allocation failed
        }
        heap->data[index].data = ptr;
        heap->data[index].alloc = str_len + 1;
    }

    // Copy the new value to the specified index
    strcpy(heap->data[index].data, new_value);
    heap->data[index].len = str_len;

    // Perform heapify up and down to maintain the Min Heap property
    _sift_min_heap_up_string(heap, index);
    _sift_min_heap_down_string(heap, index);

    return true; // Value replaced successfully
}
// ================================================================================
// ================================================================================
// eof
