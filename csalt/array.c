// ================================================================================
// ================================================================================
// - File:    array.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    November 20, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/array.h"
// ================================================================================
// ================================================================================
// Constants for memory management

const size_t ARR_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
const size_t ARR_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB

// Constants to manage Tim sort algorithm
const size_t ARR_RUN = 32;
// ================================================================================
// ================================================================================
// Begin code

char_arr init_char_array(char* arr, size_t buff, size_t len) {
    char_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

uchar_arr init_uchar_array(unsigned char* arr, size_t buff, size_t len) {
    uchar_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

short_arr init_short_array(short int* arr, size_t buff, size_t len) {
    short_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

ushort_arr init_ushort_array(unsigned short int* arr, size_t buff, size_t len) {
    ushort_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

int_arr init_int_array(int* arr, size_t buff, size_t len) {
    int_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

uint_arr init_uint_array(unsigned int* arr, size_t buff, size_t len) {
    uint_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

long_arr init_long_array(long int* arr, size_t buff, size_t len) {
    long_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

ulong_arr init_ulong_array(unsigned long int* arr, size_t buff, size_t len) {
    ulong_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

llong_arr init_llong_array(long long int* arr, size_t buff, size_t len) {
    llong_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

ullong_arr init_ullong_array(unsigned long long* arr, size_t buff, size_t len) {
    ullong_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

float_arr init_float_array(float* arr, size_t buff, size_t len) {
    float_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

double_arr init_double_array(double* arr, size_t buff, size_t len) {
    double_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

ldouble_arr init_ldouble_array(long double* arr, size_t buff, size_t len) {
    ldouble_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

bool_arr init_bool_array(bool* arr, size_t buff, size_t len) {
    bool_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// ================================================================================
// ================================================================================
// PUSH ARRAY FUNCTIONS 

bool push_char_array(char_arr* arr, char var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(char));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_uchar_array(uchar_arr* arr, unsigned char var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(unsigned char));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_short_array(short_arr* arr, short int var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(short int));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ushort_array(ushort_arr* arr, unsigned short int var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(unsigned short int));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_int_array(int_arr* arr, int var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(int));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_uint_array(uint_arr* arr, unsigned int var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(unsigned int));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_long_array(long_arr* arr, long int var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(long int));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ulong_array(ulong_arr* arr, unsigned long int var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(unsigned long int));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_llong_array(llong_arr* arr, long long int var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(long long int));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ullong_array(ullong_arr* arr, unsigned long long int var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(unsigned long long int));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_float_array(float_arr* arr, float var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(float));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_double_array(double_arr* arr, double var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(double));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ldouble_array(ldouble_arr* arr, long double var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(long double));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_bool_array(bool_arr* arr, bool var, size_t index) {
    if (!arr) {
        errno = EINVAL;
        return false;
    }
    if (arr->len >= arr->alloc || index > arr->len) {
        errno = ERANGE;
        return false;
    }
    // Increment length first if inserting not at the end
    if (index < arr->len) {
        memmove(arr->data + index + 1,
                arr->data + index,
                (arr->len - index) * sizeof(bool));
    } 
    arr->data[index] = var;
    arr->len++;
    return true;
}
// ================================================================================
// ================================================================================
// GET ARRAY FUNCTIONS 

char get_char_array(char_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

unsigned char get_uchar_array(uchar_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

short int get_short_array(short_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

unsigned short int get_ushort_array(ushort_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

int get_int_array(int_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

unsigned int get_uint_array(uint_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

long int get_long_array(long_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

unsigned long int get_ulong_array(ulong_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

long long int get_llong_array(llong_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

unsigned long long int get_ullong_array(ullong_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

float get_float_array(float_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0.0f;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0.0f; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

double get_double_array(double_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0.0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0.0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

long double get_ldouble_array(ldouble_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0.0;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return 0.0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// --------------------------------------------------------------------------------

bool get_bool_array(bool_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return false;
    }
    if (index >= arr->len) {
        errno = ERANGE; // Result not within range
        return false; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return arr->data[index];
}
// ================================================================================
// ================================================================================
// VECTOR LENGTH FUNCTIONS 

size_t char_array_length(char_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t uchar_array_length(uchar_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t short_array_length(short_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t ushort_array_length(ushort_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t int_array_length(int_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t uint_array_length(uint_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t long_array_length(long_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t ulong_array_length(ulong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t llong_array_length(llong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t ullong_array_length(ullong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t float_array_length(float_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t double_array_length(double_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t ldouble_array_length(ldouble_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// --------------------------------------------------------------------------------

size_t bool_array_length(bool_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->len;
}
// ================================================================================
// ================================================================================
// ARRAY MEMORY FUNCTIONS 

size_t char_array_memory(char_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t uchar_array_memory(uchar_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t short_array_memory(short_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t ushort_array_memory(ushort_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t int_array_memory(int_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------


size_t uint_array_memory(uint_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t long_array_memory(long_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t ulong_array_memory(ulong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t llong_array_memory(llong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t ullong_array_memory(ullong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t float_array_memory(float_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t double_array_memory(double_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t ldouble_array_memory(ldouble_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// --------------------------------------------------------------------------------

size_t bool_array_memory(bool_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return arr->alloc;
}
// ================================================================================
// ================================================================================
// POP ARRAY FUNCTIONS 

char pop_char_array(char_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    char val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(char)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned char pop_uchar_array(uchar_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned char val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(unsigned char)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

short int pop_short_array(short_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    short int val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(short int)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned short int pop_ushort_array(ushort_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned short int val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(unsigned short int)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

int pop_int_array(int_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    int val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(int)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned int pop_uint_array(uint_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned int val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(unsigned int)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

long int pop_long_array(long_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    long int val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(long int)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned long int pop_ulong_array(ulong_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned long int val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(unsigned long int)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

long long int pop_llong_array(llong_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    long long int val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(long long int)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned long long int pop_ullong_array(ullong_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned long long int val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(unsigned long long int)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

float pop_float_array(float_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0.f;
    }
    float val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(float)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

double pop_double_array(double_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0.;
    }
    double val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(double)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

long double pop_ldouble_array(ldouble_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len - 1) {
        errno = ERANGE;
        return 0.;
    }
    long double val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(long double)); 
    arr->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

bool pop_bool_array(bool_arr* arr, size_t index) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > arr->len -1) {
        errno = ERANGE;
        return false;
    }
    bool val = arr->data[index];
    memmove(arr->data + index, 
            arr->data + index + 1, 
            (arr->len - index - 1) * sizeof(bool)); 
    arr->len -= 1;
    return val;
}
// ================================================================================
// ================================================================================

void reverse_char_array(char_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_char(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_uchar_array(uchar_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_uchar(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_short_array(short_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_short(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_ushort_array(ushort_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_ushort(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_int_array(int_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_int(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_uint_array(uint_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_uint(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_long_array(long_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_long(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_ulong_array(ulong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_ulong(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_llong_array(llong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_llong(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_ullong_array(ullong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_ullong(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_float_array(float_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_float(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_double_array(double_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_double(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_ldouble_array(ldouble_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_ldouble(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_bool_array(bool_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = arr->len - 1;
    while (i < j) {
       swap_bool(&arr->data[i], &arr->data[j]);
       i++;
       j--;
    }
}
// ================================================================================
// ================================================================================
// PRIVATE BUBBLE SORT ALGORITHMS

static void _bubble_sort_char(char* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_char(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_uchar(unsigned char* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_uchar(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_short(short int* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_short(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ushort(unsigned short int* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_ushort(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_int(int* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_int(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_uint(unsigned int* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_uint(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_long(long int* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_long(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ulong(unsigned long int* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_ulong(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_llong(long long int* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_llong(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ullong(unsigned long long int* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_ullong(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_float(float* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_float(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_double(double* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_double(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ldouble(long double* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_ldouble(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// --------------------------------------------------------------------------------

static void _bubble_sort_bool(bool* vec, size_t len, iter_dir direction) {
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if ((direction == FORWARD && vec[j] > vec[j + 1]) ||
                (direction == REVERSE && vec[j] < vec[j + 1])) {
                swap_bool(&vec[j], &vec[j + 1]);
            }
        }
    } 
}
// ================================================================================
// PRIVATE INSERT SORT ALGORITHMS 

static void _insert_sort_char(char* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        char key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_uchar(unsigned char* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        unsigned char key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_short(short int* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        short int key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_ushort(unsigned short int* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        unsigned short int key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_int(int* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        int key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_uint(unsigned int* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        unsigned int key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_long(long int* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        long int key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_ulong(unsigned long int* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        unsigned int key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_llong(long long int* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        long long int key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_ullong(unsigned long long int* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        unsigned long long int key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_float(float* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        float key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_double(double* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        unsigned int key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_ldouble(long double* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        long double key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// --------------------------------------------------------------------------------

static void _insert_sort_bool(bool* vec, size_t len, iter_dir direction) {
    int j;
    for (size_t i = 1; i < len; i++) {
        bool key = vec[i];
        j = i - 1;

        // Move elements of array[0..i-1], that are greater/smaller than key,
        // to one position ahead of their current position
        if (direction == FORWARD) {
            while (j >= 0 && vec[j] > key) {
                vec[j + 1] = vec[j];
                j--;
            }
        } else { // REVERSE
            while (j >= 0 && vec[j] < key) {
                vec[j + 1] = vec[j];
                j--;
            }
        }
        vec[j + 1] = key;
    } 
}
// ================================================================================
// PRIVATE MERGE SORT ALGORITHMS 

static void _merge_char(char *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    char L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_char(char* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_char(vec, l, m, direction);
        _merge_sort_char(vec, m + 1, r, direction);

        _merge_char(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_uchar(unsigned char *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    unsigned char L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_uchar(unsigned char* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_uchar(vec, l, m, direction);
        _merge_sort_uchar(vec, m + 1, r, direction);

        _merge_uchar(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_short(short int *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    short int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_short(short int* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_short(vec, l, m, direction);
        _merge_sort_short(vec, m + 1, r, direction);

        _merge_short(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_ushort(unsigned short int *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    unsigned short int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_ushort(unsigned short int* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_ushort(vec, l, m, direction);
        _merge_sort_ushort(vec, m + 1, r, direction);

        _merge_ushort(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_int(int *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_int(int* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_int(vec, l, m, direction);
        _merge_sort_int(vec, m + 1, r, direction);

        _merge_int(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_uint(unsigned int *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    unsigned int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_uint(unsigned int* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_uint(vec, l, m, direction);
        _merge_sort_uint(vec, m + 1, r, direction);

        _merge_uint(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_long(long int *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    long int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_long(long int* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_long(vec, l, m, direction);
        _merge_sort_long(vec, m + 1, r, direction);

        _merge_long(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_ulong(unsigned long int *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    unsigned long int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_ulong(unsigned long int* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_ulong(vec, l, m, direction);
        _merge_sort_ulong(vec, m + 1, r, direction);

        _merge_ulong(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_llong(long long int *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    long long int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_llong(long long int* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_llong(vec, l, m, direction);
        _merge_sort_llong(vec, m + 1, r, direction);

        _merge_llong(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_ullong(unsigned long long int *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    unsigned long long int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_ullong(unsigned long long int* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_ullong(vec, l, m, direction);
        _merge_sort_ullong(vec, m + 1, r, direction);

        _merge_ullong(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_float(float *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    float L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_float(float* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_float(vec, l, m, direction);
        _merge_sort_float(vec, m + 1, r, direction);

        _merge_float(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_double(double *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    double L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_double(double* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_double(vec, l, m, direction);
        _merge_sort_double(vec, m + 1, r, direction);

        _merge_double(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_ldouble(long double *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    long double L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_ldouble(long double* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_ldouble(vec, l, m, direction);
        _merge_sort_ldouble(vec, m + 1, r, direction);

        _merge_ldouble(vec, l, m, r, direction);
    }
}
// --------------------------------------------------------------------------------

static void _merge_bool(bool *vec, int l, int m, int r, iter_dir direction) {
    // l, m, r represent left, middle, and rightmost indices in array
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    bool L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = vec[l + i];
    for (j = 0; j < n2; j++)
        R[j] = vec[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if ((direction == FORWARD && L[i] <= R[j]) ||
            (direction == REVERSE && L[i] >= R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}
// --------------------------------------------------------------------------------

static void _merge_sort_bool(bool* vec, int l, int r, iter_dir direction) {
    if (l < r) {
        int m = l + (r - l) / 2;

        _merge_sort_bool(vec, l, m, direction);
        _merge_sort_bool(vec, m + 1, r, direction);

        _merge_bool(vec, l, m, r, direction);
    }
}
// ================================================================================
// PRIVATE QUICK SORT ALGORITHMS 

static int _partition_char(char* vec, int low, int high, iter_dir direction) {
    char pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_char(&vec[i], &vec[j]);
        }
    }
    swap_char(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_char(char* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_char(vec, low, high, direction);

        _quicksort_char(vec, low, pi - 1, direction);
        _quicksort_char(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_uchar(unsigned char* vec, int low, int high, iter_dir direction) {
    unsigned char pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_uchar(&vec[i], &vec[j]);
        }
    }
    swap_uchar(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_uchar(unsigned char* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_uchar(vec, low, high, direction);

        _quicksort_uchar(vec, low, pi - 1, direction);
        _quicksort_uchar(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_short(short int* vec, int low, int high, iter_dir direction) {
    short int pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_short(&vec[i], &vec[j]);
        }
    }
    swap_short(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_short(short int* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_short(vec, low, high, direction);

        _quicksort_short(vec, low, pi - 1, direction);
        _quicksort_short(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_ushort(unsigned short int* vec, int low, int high, iter_dir direction) {
    unsigned short int pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_ushort(&vec[i], &vec[j]);
        }
    }
    swap_ushort(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_ushort(unsigned short int* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_ushort(vec, low, high, direction);

        _quicksort_ushort(vec, low, pi - 1, direction);
        _quicksort_ushort(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_int(int* vec, int low, int high, iter_dir direction) {
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
    if (low < high) {
        int pi = _partition_int(vec, low, high, direction);

        _quicksort_int(vec, low, pi - 1, direction);
        _quicksort_int(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_uint(unsigned int* vec, int low, int high, iter_dir direction) {
    unsigned int pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_uint(&vec[i], &vec[j]);
        }
    }
    swap_uint(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_uint(unsigned int* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_uint(vec, low, high, direction);

        _quicksort_uint(vec, low, pi - 1, direction);
        _quicksort_uint(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_long(long int* vec, int low, int high, iter_dir direction) {
    long int pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_long(&vec[i], &vec[j]);
        }
    }
    swap_long(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_long(long int* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_long(vec, low, high, direction);

        _quicksort_long(vec, low, pi - 1, direction);
        _quicksort_long(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_ulong(unsigned long int* vec, int low, int high, iter_dir direction) {
    unsigned long int pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_ulong(&vec[i], &vec[j]);
        }
    }
    swap_ulong(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_ulong(unsigned long int* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_ulong(vec, low, high, direction);

        _quicksort_ulong(vec, low, pi - 1, direction);
        _quicksort_ulong(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_llong(long long int* vec, int low, int high, iter_dir direction) {
    long long int pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_llong(&vec[i], &vec[j]);
        }
    }
    swap_llong(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_llong(long long int* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_llong(vec, low, high, direction);

        _quicksort_llong(vec, low, pi - 1, direction);
        _quicksort_llong(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_ullong(unsigned long long int* vec, int low, int high, iter_dir direction) {
    unsigned long long int pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_ullong(&vec[i], &vec[j]);
        }
    }
    swap_ullong(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_ullong(unsigned long long int* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_ullong(vec, low, high, direction);

        _quicksort_ullong(vec, low, pi - 1, direction);
        _quicksort_ullong(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_float(float* vec, int low, int high, iter_dir direction) {
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
    if (low < high) {
        int pi = _partition_float(vec, low, high, direction);

        _quicksort_float(vec, low, pi - 1, direction);
        _quicksort_float(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_double(double* vec, int low, int high, iter_dir direction) {
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
    if (low < high) {
        int pi = _partition_double(vec, low, high, direction);

        _quicksort_double(vec, low, pi - 1, direction);
        _quicksort_double(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_ldouble(long double* vec, int low, int high, iter_dir direction) {
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
    if (low < high) {
        int pi = _partition_ldouble(vec, low, high, direction);

        _quicksort_ldouble(vec, low, pi - 1, direction);
        _quicksort_ldouble(vec, pi + 1, high, direction);
    }
}
// --------------------------------------------------------------------------------

static int _partition_bool(bool* vec, int low, int high, iter_dir direction) {
    bool pivot = vec[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_bool(&vec[i], &vec[j]);
        }
    }
    swap_bool(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_bool(bool* vec, int low, int high, iter_dir direction) {
    if (low < high) {
        int pi = _partition_bool(vec, low, high, direction);

        _quicksort_bool(vec, low, pi - 1, direction);
        _quicksort_bool(vec, pi + 1, high, direction);
    }
}
// ================================================================================
// PRIVATE TIM SORT ALGORITHMS 
// - Shamless adaptation from geeks for geeks 
// - May want to allow for adaptive RUN size in future iterations.

static void _insertion_sort_for_timsort_char(char* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        char temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_char(char* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_char(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_char(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_uchar(unsigned char* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        unsigned char temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_uchar(unsigned char* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_uchar(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_uchar(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_short(short int* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        short int temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_short(short int* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_short(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_short(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_ushort(unsigned short int* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        unsigned short int temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_ushort(unsigned short int* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_ushort(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_ushort(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_int(int* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        int temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_int(int* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_int(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_int(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_uint(unsigned int* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        unsigned int temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_uint(unsigned int* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_uint(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_uint(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_long(long int* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        long int temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_long(long int* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_long(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_long(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_ulong(unsigned long int* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        unsigned long int temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_ulong(unsigned long int* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_ulong(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_ulong(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_llong(long long int* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        long long int temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_llong(long long int* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_llong(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_llong(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_ullong(unsigned long long int* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        unsigned long long int temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_ullong(unsigned long long int* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_ullong(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_ullong(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_float(float* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        float temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_float(float* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_float(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_float(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_double(double* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        double temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_double(double* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_double(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_double(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_ldouble(long double* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        long double temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_ldouble(long double* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_ldouble(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_ldouble(vec, left, mid, right, direction);
            }
        }
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_for_timsort_bool(bool* vec, int left, int right, iter_dir direction) {
    for (int i = left + 1; i <= right; i++) {
        bool temp = vec[i];
        int j = i - 1;
        while (j >= left && ((direction == FORWARD && vec[j] > temp) || 
                             (direction == REVERSE && vec[j] < temp))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = temp;
    }
}
// --------------------------------------------------------------------------------

static void _timsort_bool(bool* vec, int n, iter_dir direction) {
    // Sort individual subarrays of size ARR_RUN
    for (int i = 0; i < n; i += ARR_RUN) {
        _insertion_sort_for_timsort_bool(vec, i, min((i + ARR_RUN - 1), (n - 1)), direction);
    }

    // Start merging from size ARR_RUN (or 32). It will merge to form size 64, then 128, 256, and so on.
    for (int size = ARR_RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), (n - 1));
            if (mid < right) {
                _merge_bool(vec, left, mid, right, direction);
            }
        }
    }
}
// ================================================================================
// Actual interfaces for sort algoorithms 

void sort_char_array(char_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_char(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_char(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_char(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_char(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_char(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_uchar_array(uchar_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_uchar(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_uchar(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_uchar(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_uchar(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_uchar(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_short_array(short_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_short(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_short(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_short(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_short(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_short(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ushort_array(ushort_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_ushort(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_ushort(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_ushort(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_ushort(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_ushort(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_int_array(int_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_int(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_int(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_int(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_int(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_int(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_uint_array(uint_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_uint(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_uint(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_uint(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_uint(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_uint(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_long_array(long_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_long(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_long(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_long(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_long(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_long(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ulong_array(ulong_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_ulong(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_ulong(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_ulong(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_ulong(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_ulong(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_llong_array(llong_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_llong(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_llong(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_llong(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_llong(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_llong(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ullong_array(ullong_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_ullong(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_ullong(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_ullong(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_ullong(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_ullong(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_float_array(float_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_float(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_float(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_float(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_float(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_float(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_double_array(double_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_double(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_double(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_double(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_double(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_double(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ldouble_array(ldouble_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_ldouble(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_ldouble(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_ldouble(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_ldouble(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_ldouble(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_bool_array(bool_arr* vec, sort_type stype, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    switch (stype) {
        case BUBBLE: 
            _bubble_sort_bool(vec->data, vec->len, direction);
            break;
        case INSERT:
            _insert_sort_bool(vec->data, vec->len, direction);
            break;
        case MERGE:
            _merge_sort_bool(vec->data, 0, vec->len - 1, direction);
            break;
        case QUICK:
            _quicksort_bool(vec->data, 0, vec->len - 1, direction);
            break;
        case TIM:
            _timsort_bool(vec->data, vec->len, direction);
            break;
        default:
            errno = EINVAL;
            return;
    }
}
// ================================================================================
// ================================================================================

char max_char_array(char_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    char max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned char max_uchar_array(uchar_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned char max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

short int max_short_array(short_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    short int max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned short int max_ushort_array(ushort_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned short int max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

int max_int_array(int_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    int max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned int max_uint_array(uint_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned int max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

long int max_long_array(long_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long int max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned long int max_ulong_array(ulong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long int max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

long long int max_llong_array(llong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long int max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned long long int max_ullong_array(ullong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long long int max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

float max_float_array(float_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0.f;
    }
    float max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

double max_double_array(double_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    double max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

long double max_ldouble_array(ldouble_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    long double max_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] > max_value) max_value = arr->data[i];
    }
    return max_value;
}
// ================================================================================
// ================================================================================
// MIN ARRAY FUNCTIONS 

char min_char_array(char_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    char min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned char min_uchar_array(uchar_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned char min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

short int min_short_array(short_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    short int min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned short int min_ushort_array(ushort_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned short int min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

int min_int_array(int_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    int min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned int min_uint_array(uint_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned int min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

long int min_long_array(long_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long int min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned long int min_ulong_array(ulong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long int min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

long long int min_llong_array(llong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long int min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned long long int min_ullong_array(ullong_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long long int min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

float min_float_array(float_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0.f;
    }
    float min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

double min_double_array(double_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    double min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

long double min_ldouble_array(ldouble_arr* arr) {
    if ( arr == NULL || arr->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    long double min_value = arr->data[0];
    for (size_t i = 1; i < arr->len; i++) {
        if (arr->data[i] < min_value) min_value = arr->data[i];
    }
    return min_value;
}
// ================================================================================
// ================================================================================
// eof
