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
// --------------------------------------------------------------------------------

string_arr init_string_array(str* arr, size_t buff, size_t len) {
    string_arr arr_struct = {.data=arr, .len=len, .alloc=buff};
    return arr_struct;
}
// --------------------------------------------------------------------------------

InitArrFunc init_array(dtype dat_type) {
    switch(dat_type) {
        case dChar: return (InitArrFunc)init_char_array;
        case dUChar: return (InitArrFunc)init_uchar_array;
        case dShort: return (InitArrFunc)init_short_array;
        case dUShort: return (InitArrFunc)init_ushort_array;
        case dInt: return (InitArrFunc)init_int_array;
        case dUInt: return (InitArrFunc)init_uint_array;
        case dLong: return (InitArrFunc)init_long_array;
        case dULong: return (InitArrFunc)init_ulong_array;
        case dLongLong: return (InitArrFunc)init_llong_array;
        case dULongLong: return (InitArrFunc)init_ullong_array;
        case dFloat: return (InitArrFunc)init_float_array;
        case dDouble: return (InitArrFunc)init_double_array;
        case dLDouble: return (InitArrFunc)init_ldouble_array;
        case dBool: return (InitArrFunc)init_bool_array;
        case dString: return (InitArrFunc)init_string_array;
        default: return NULL;
    }
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
// POP ARRAY FUNCTIONS 

// ================================================================================
// ================================================================================
// eof
