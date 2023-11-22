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
// eof
