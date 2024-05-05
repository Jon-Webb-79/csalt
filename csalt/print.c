// ================================================================================
// ================================================================================
// - File:    print.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 28, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/print.h"

const size_t PRINT_LENGTH = 20;

// Scalar print functions
void print_short(short x) { printf("%hd", x); }
void print_ushort(unsigned short x) { printf("%hu", x); }
void print_int(int x) { printf("%d", x); }
void print_uint(unsigned int x) { printf("%u", x); }
void print_long(long int x) { printf("%ld", x); }
void print_ulong(unsigned long int x) { printf("%lu", x); }
void print_llong(long long int x) { printf("%lld", x); }
void print_ullong(unsigned long long int x) { printf("%llu", x); }
void print_float(float x) { printf("%f", x); }
void print_double(double x) { printf("%lf", x); }
void print_longdouble(long double x) { printf("%Lf", x); }
void print_char(char x) { printf("%hhd", x); }
void print_uchar(unsigned char x) { printf("%hhu", x); }
void print_bool(bool x) { printf(x ? "true" : "false"); }
void print_string(char* x) { printf("%s", x); }
void print_sstring(str* x) { 
    if (!x) { 
        printf("This struct is a NULL pointer!");
        return;
    }
    if (!x->data) {
        printf("The string is a NULL pointer!");
        return;
    }
    printf("%s", x->data); 
}
// ================================================================================
// ================================================================================
// PRINT VECTOR DATA TYPES  

void print_char_vector(char_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_char(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_char(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_uchar_vector(uchar_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_uchar(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_uchar(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_short_vector(short_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_short(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_short(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ushort_vector(ushort_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_ushort(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ushort(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_int_vector(int_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_int(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_int(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_uint_vector(uint_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_uint(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_uint(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_long_vector(long_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_long(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_long(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ulong_vector(ulong_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_ulong(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ulong(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_llong_vector(llong_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_llong(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_llong(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ullong_vector(ullong_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_ullong(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ullong(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_float_vector(float_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_float(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_float(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_double_vector(double_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_double(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_double(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ldouble_vector(ldouble_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_longdouble(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_longdouble(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_bool_vector(bool_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_bool(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_bool(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_string_vector(string_v* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_string(vec->data[i].data);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_string(vec->data[vec->len - 1].data);
    printf(" ]");
}
// ================================================================================
// ================================================================================

void print_char_array(char_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_char(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_char(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_uchar_array(uchar_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_uchar(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_uchar(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_short_array(short_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_short(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_short(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ushort_array(ushort_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_ushort(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ushort(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_int_array(int_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_int(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_int(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_uint_array(uint_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_uint(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_uint(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_long_array(long_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_long(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_long(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ulong_array(ulong_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_ulong(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ulong(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_llong_array(llong_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_llong(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_llong(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ullong_array(ullong_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_ullong(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ullong(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_float_array(float_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_float(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_float(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_double_array(double_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_double(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_double(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ldouble_array(ldouble_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_longdouble(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_longdouble(arr->data[arr->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_bool_array(bool_arr* arr) {
    if (arr->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < arr->len - 1; i++) {
        print_bool(arr->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_bool(arr->data[arr->len - 1]);
    printf(" ]");
}
// ================================================================================
// ================================================================================
// PRINT MIN HEAP 

void print_char_min_heap(char_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_char(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_char(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_uchar_min_heap(uchar_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_uchar(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_uchar(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_short_min_heap(short_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_short(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_short(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ushort_min_heap(ushort_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_ushort(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ushort(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_int_min_heap(int_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_int(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_int(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_uint_min_heap(uint_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_uint(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_uint(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_long_min_heap(long_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_long(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_long(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ulong_min_heap(ulong_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_ulong(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ulong(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_llong_min_heap(llong_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_llong(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_llong(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ullong_min_heap(ullong_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_ullong(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ullong(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_float_min_heap(float_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_float(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_float(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_double_min_heap(double_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_double(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_double(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ldouble_min_heap(ldouble_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_longdouble(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_longdouble(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_bool_min_heap(bool_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_bool(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_bool(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_string_min_heap(string_min_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_string(vec->data[i].data);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_string(vec->data[vec->len - 1].data);
    printf(" ]");
}
// ================================================================================
// ================================================================================
// PRINT MIN HEAP 

void print_char_max_heap(char_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_char(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_char(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_uchar_max_heap(uchar_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_uchar(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_uchar(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_short_max_heap(short_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_short(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_short(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ushort_max_heap(ushort_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_ushort(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ushort(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_int_max_heap(int_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_int(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_int(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_uint_max_heap(uint_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_uint(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_uint(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_long_max_heap(long_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_long(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_long(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ulong_max_heap(ulong_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_ulong(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ulong(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_llong_max_heap(llong_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_llong(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_llong(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ullong_max_heap(ullong_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_ullong(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_ullong(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_float_max_heap(float_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_float(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_float(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_double_max_heap(double_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_double(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_double(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_ldouble_max_heap(ldouble_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_longdouble(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_longdouble(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_bool_max_heap(bool_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_bool(vec->data[i]);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_bool(vec->data[vec->len - 1]);
    printf(" ]");
}
// --------------------------------------------------------------------------------

void print_string_max_heap(string_max_hp* vec) {
    if (vec->len == 0) {
        printf("[ NULL ]");
        return;
    }
    printf("[ ");
    size_t j = 0;
    for (size_t i = 0; i < vec->len - 1; i++) {
        print_string(vec->data[i].data);
        printf(", ");
        j++;
        if (j == PRINT_LENGTH) {
            printf("\n");
            printf(" ");
            j = 0;
        }
    }
    print_string(vec->data[vec->len - 1].data);
    printf(" ]");
}
// ================================================================================
// ================================================================================

void print_char_sllist(char_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    char_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_char(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_uchar_sllist(uchar_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    uchar_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_uchar(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_short_sllist(short_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    short_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_short(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_ushort_sllist(ushort_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    ushort_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_ushort(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_int_sllist(int_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    int_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_int(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_uint_sllist(uint_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    uint_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_uint(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_long_sllist(long_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    long_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_long(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_ulong_sllist(ulong_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    ulong_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_ulong(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_llong_sllist(llong_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    llong_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_llong(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_ullong_sllist(ullong_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    ullong_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_ullong(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_float_sllist(float_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    float_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_float(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_double_sllist(double_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    double_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_double(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_ldouble_sllist(ldouble_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    ldouble_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_longdouble(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_bool_sllist(bool_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    bool_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_bool(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_string_sllist(string_sl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    string_slnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_string(current->data->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// ================================================================================
// ================================================================================

void print_char_dllist(char_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    char_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_char(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_uchar_dllist(uchar_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    uchar_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_uchar(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_short_dllist(short_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    short_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_short(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_ushort_dllist(ushort_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    ushort_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_ushort(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_int_dllist(int_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    int_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_int(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_uint_dllist(uint_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    uint_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_uint(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_long_dllist(long_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    long_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_long(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_ulong_dllist(ulong_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    ulong_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_ulong(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_llong_dllist(llong_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    llong_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_llong(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_ullong_dllist(ullong_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    ullong_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_ullong(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_float_dllist(float_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    float_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_float(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_double_dllist(double_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    double_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_double(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_ldouble_dllist(ldouble_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    ldouble_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_longdouble(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_bool_dllist(bool_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    bool_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_bool(current->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// --------------------------------------------------------------------------------

void print_string_dllist(string_dl* list) {
    if ( !list && !list->head) {
        errno = EINVAL;
        printf("NULL");
    }
    printf("{ ");
    string_dlnode* current = list->head;
    size_t i = 0;
    while (current != NULL) {
        print_string(current->data->data);
        if (i < list->len - 1)
            printf(", ");
        current = current->next;
        i++;
    }
    printf(" }\n");
}
// ================================================================================ 
// ================================================================================ 

static void _char_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    charAVLNode* datNode = (charAVLNode*) node;

    _char_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%hhd", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %hhd", datNode->data);
    }
    _char_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_char_avltree(charAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _char_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _uchar_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    ucharAVLNode* datNode = (ucharAVLNode*) node;

    _uchar_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%hhu", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %hhu", datNode->data);
    }
    _uchar_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_uchar_avltree(ucharAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _uchar_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _short_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    shortAVLNode* datNode = (shortAVLNode*) node;

    _short_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%hd", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %hd", datNode->data);
    }
    _short_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_short_avltree(shortAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _short_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _ushort_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    ushortAVLNode* datNode = (ushortAVLNode*) node;

    _ushort_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%hu", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %hu", datNode->data);
    }
    _ushort_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_ushort_avltree(ushortAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _ushort_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _int_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    intAVLNode* datNode = (intAVLNode*) node;

    _int_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%d", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %d", datNode->data);
    }
    _int_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_int_avltree(intAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _int_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _uint_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    uintAVLNode* datNode = (uintAVLNode*) node;

    _int_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%u", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %u", datNode->data);
    }
    _uint_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_uint_avltree(uintAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _uint_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _long_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    longAVLNode* datNode = (longAVLNode*) node;

    _long_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%ld", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %ld", datNode->data);
    }
    _long_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_long_avltree(longAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _long_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _ulong_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    ulongAVLNode* datNode = (ulongAVLNode*) node;

    _ulong_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%lu", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %lu", datNode->data);
    }
    _ulong_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_ulong_avltree(ulongAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _ulong_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _llong_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    llongAVLNode* datNode = (llongAVLNode*) node;

    _llong_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%lld", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %lld", datNode->data);
    }
    _llong_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_llong_avltree(llongAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _llong_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _ullong_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    ullongAVLNode* datNode = (ullongAVLNode*) node;

    _ullong_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%llu", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %llu", datNode->data);
    }
    _ullong_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_ullong_avltree(ullongAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _ullong_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _float_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    floatAVLNode* datNode = (floatAVLNode*) node;

    _float_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%f", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %f", datNode->data);
    }
    _float_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_float_avltree(floatAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _float_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _double_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    doubleAVLNode* datNode = (doubleAVLNode*) node;

    _double_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%lf", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %lf", datNode->data);
    }
    _double_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_double_avltree(doubleAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _double_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _ldouble_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    ldoubleAVLNode* datNode = (ldoubleAVLNode*) node;

    _ldouble_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%Lf", datNode->data);
        *isFirst = 0;
    } else {
        printf(", %Lf", datNode->data);
    }
    _ldouble_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_ldouble_avltree(ldoubleAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _ldouble_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _bool_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    boolAVLNode* datNode = (boolAVLNode*) node;

    _bool_order(datNode->base.left, isFirst);

    if (*isFirst) {
        if (datNode->data)
            printf("true");
        else
            printf("false");
        *isFirst = 0;
    } else {
        if (datNode->data)
            printf(", true");
        else
            printf(", false");
    }
    _bool_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_bool_avltree(boolAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _bool_order(tree->root, &isFirst);
    printf(" ]");
}
// --------------------------------------------------------------------------------

static void _string_order(AVLNode* node, int* isFirst) {
    if (!node)
        return;
    stringAVLNode* datNode = (stringAVLNode*) node;

    _string_order(datNode->base.left, isFirst);

    if (*isFirst) {
        printf("%s", datNode->data->data);
        *isFirst = 0;
    } else {
        printf(", %s", datNode->data->data);
    }
    _string_order(datNode->base.right, isFirst);
}
// --------------------------------------------------------------------------------

void print_string_avltree(stringAVLTree* tree) {
    if (!tree || !tree->root) {
        printf("[ ]");
    }
    printf("[ ");
    int isFirst = 1;
    _string_order(tree->root, &isFirst);
    printf(" ]");
}
// ================================================================================
// ================================================================================
// eof
