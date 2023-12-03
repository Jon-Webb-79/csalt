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
// eof
