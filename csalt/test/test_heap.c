// ================================================================================
// ================================================================================
// - File:    test_heap.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "test_heap.h"

void test_min_enqueue_char(void **state)
{
	(void) state;
    char a[7] = {5, 1, 2, 3, 6, 4, 7};
    char_min_hp* heap = init_min_heap(dChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    char b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_uchar(void **state)
{
	(void) state;
    unsigned char a[7] = {5, 1, 2, 3, 6, 4, 7};
    uchar_min_hp* heap = init_min_heap(dUChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned char b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_short(void **state)
{
	(void) state;
    short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    short_min_hp* heap = init_min_heap(dShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    short int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_ushort(void **state)
{
	(void) state;
    unsigned short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ushort_min_hp* heap = init_min_heap(dUShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned short int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_int(void **state)
{
	(void) state;
    int a[7] = {5, 1, 2, 3, 6, 4, 7};
    int_min_hp* heap = init_min_heap(dInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_uint(void **state)
{
	(void) state;
    unsigned int a[7] = {5, 1, 2, 3, 6, 4, 7};
    uint_min_hp* heap = init_min_heap(dUInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_long(void **state)
{
	(void) state;
    long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    long_min_hp* heap = init_min_heap(dLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    long int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_ulong(void **state)
{
	(void) state;
    unsigned long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ulong_min_hp* heap = init_min_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned long int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_llong(void **state)
{
	(void) state;
    long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    llong_min_hp* heap = init_min_heap(dLongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    long long int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_ullong(void **state)
{
	(void) state;
    unsigned long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ullong_min_hp* heap = init_min_heap(dULongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned long long int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_float(void **state)
{
	(void) state;
    float a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    float_min_hp* heap = init_min_heap(dFloat)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    float b[7] = {1.1, 3.3, 2.2, 5.5, 6.6, 4.4, 7.7};
    for (size_t i = 0; i < 7; i++) {
        assert_float_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_double(void **state)
{
	(void) state;
    double a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    double_min_hp* heap = init_min_heap(dDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    double b[7] = {1.1, 3.3, 2.2, 5.5, 6.6, 4.4, 7.7};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_ldouble(void **state)
{
	(void) state;
    long double a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    ldouble_min_hp* heap = init_min_heap(dLDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    double b[7] = {1.1, 3.3, 2.2, 5.5, 6.6, 4.4, 7.7};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_min_heap(heap);
} 
// --------------------------------------------------------------------------------

void test_min_enqueue_bool(void **state)
{
	(void) state;
    bool a[7] = {true, false, true, false, true, false, true};
    bool_min_hp* heap = init_min_heap(dBool)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    bool b[7] = {false, false, false, true, true, true, true};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_enqueue_string(void **state)
{
	(void) state;
    char* a[7] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
    string_min_hp* heap = init_min_heap(dString)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    char* b[7] = {"Five", "Four", "Seven", "Two", "One", "Three", "Six"};
    for (size_t i = 0; i < 7; i++) {
        assert_string_equal(heap->data[i].data, b[i]);
    }
    free_min_heap(heap);
}
// ================================================================================
// ================================================================================

void test_max_enqueue_char(void **state)
{
	(void) state;
    char a[7] = {5, 1, 2, 3, 6, 4, 7};
    char_max_hp* heap = init_max_heap(dChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    char b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_uchar(void **state)
{
	(void) state;
    unsigned char a[7] = {5, 1, 2, 3, 6, 4, 7};
    uchar_max_hp* heap = init_max_heap(dUChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    char b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_short(void **state)
{
	(void) state;
    short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    short_max_hp* heap = init_max_heap(dShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    short int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_ushort(void **state)
{
	(void) state;
    unsigned short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ushort_max_hp* heap = init_max_heap(dUShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned short int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_int(void **state)
{
	(void) state;
    int a[7] = {5, 1, 2, 3, 6, 4, 7};
    int_max_hp* heap = init_max_heap(dInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_uint(void **state)
{
	(void) state;
    unsigned int a[7] = {5, 1, 2, 3, 6, 4, 7};
    uint_max_hp* heap = init_max_heap(dUInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_long(void **state)
{
	(void) state;
    long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    long_max_hp* heap = init_max_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    long int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_ulong(void **state)
{
	(void) state;
    unsigned long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ulong_max_hp* heap = init_max_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned long int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_llong(void **state)
{
	(void) state;
    long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    llong_max_hp* heap = init_max_heap(dLongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    long long int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_ullong(void **state)
{
	(void) state;
    unsigned long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ullong_max_hp* heap = init_max_heap(dULongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned long long int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_float(void **state)
{
	(void) state;
    float a[7] = {5., 1., 2., 3., 6., 4., 7.};
    float_max_hp* heap = init_max_heap(dFloat)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    float b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_float_equal(heap->data[i], b[i], 1.0e-3);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_double(void **state)
{
	(void) state;
    double a[7] = {5., 1., 2., 3., 6., 4., 7.};
    double_max_hp* heap = init_max_heap(dDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    double b[7] = {7., 5., 6., 1., 3., 2., 4.};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_ldouble(void **state)
{
	(void) state;
    long double a[7] = {5., 1., 2., 3., 6., 4., 7.};
    ldouble_max_hp* heap = init_max_heap(dLDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    long double b[7] = {7., 5., 6., 1., 3., 2., 4.};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_bool(void **state)
{
	(void) state;
    bool a[7] = {true, false, true, false, true, false, true};
    bool_max_hp* heap = init_max_heap(dBool)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    bool b[7] = {true, true, true, false, false, false, true};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_enqueue_string(void **state)
{
	(void) state;
    char* a[7] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
    string_max_hp* heap = init_max_heap(dString)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    char* b[7] = {"Seven", "Four", "Six", "One", "Three", "Two", "Five"};
    for (size_t i = 0; i < 7; i++) {
        assert_string_equal(heap->data[i].data, b[i]);
    }
    free_max_heap(heap);
}
// ================================================================================
// ================================================================================

void test_min_enqueue_char_gbc(void **state)
{
	(void) state;
    char a[7] = {5, 1, 2, 3, 6, 4, 7};
    char_min_hp* heap gbc_char_min_hp = init_min_heap(dChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    char b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_uchar_gbc(void **state)
{
	(void) state;
    unsigned char a[7] = {5, 1, 2, 3, 6, 4, 7};
    uchar_min_hp* heap gbc_uchar_min_hp = init_min_heap(dUChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned char b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_short_gbc(void **state)
{
	(void) state;
    short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    short_min_hp* heap gbc_short_min_hp = init_min_heap(dShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    short int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_ushort_gbc(void **state)
{
	(void) state;
    unsigned short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ushort_min_hp* heap gbc_ushort_min_hp = init_min_heap(dUShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned short int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_int_gbc(void **state)
{
	(void) state;
    int a[7] = {5, 1, 2, 3, 6, 4, 7};
    int_min_hp* heap gbc_int_min_hp = init_min_heap(dInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_uint_gbc(void **state)
{
	(void) state;
    unsigned int a[7] = {5, 1, 2, 3, 6, 4, 7};
    uint_min_hp* heap gbc_uint_min_hp = init_min_heap(dUInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_long_gbc(void **state)
{
	(void) state;
    long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    long_min_hp* heap gbc_long_min_hp = init_min_heap(dLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    long int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_ulong_gbc(void **state)
{
	(void) state;
    unsigned long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ulong_min_hp* heap gbc_ulong_min_hp = init_min_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned long int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_llong_gbc(void **state)
{
	(void) state;
    long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    llong_min_hp* heap gbc_llong_min_hp = init_min_heap(dLongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    long long int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_ullong_gbc(void **state)
{
	(void) state;
    unsigned long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ullong_min_hp* heap gbc_ullong_min_hp = init_min_heap(dULongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned long long int b[7] = {1, 3, 2, 5, 6, 4, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_float_gbc(void **state)
{
	(void) state;
    float a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    float_min_hp* heap gbc_float_min_hp = init_min_heap(dFloat)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    float b[7] = {1.1, 3.3, 2.2, 5.5, 6.6, 4.4, 7.7};
    for (size_t i = 0; i < 7; i++) {
        assert_float_equal(heap->data[i], b[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_double_gbc(void **state)
{
	(void) state;
    double a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    double_min_hp* heap gbc_double_min_hp = init_min_heap(dDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    double b[7] = {1.1, 3.3, 2.2, 5.5, 6.6, 4.4, 7.7};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_ldouble_gbc(void **state)
{
	(void) state;
    long double a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    ldouble_min_hp* heap gbc_ldouble_min_hp = init_min_heap(dLDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    double b[7] = {1.1, 3.3, 2.2, 5.5, 6.6, 4.4, 7.7};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
} 
// --------------------------------------------------------------------------------

void test_min_enqueue_bool_gbc(void **state)
{
	(void) state;
    bool a[7] = {true, false, true, false, true, false, true};
    bool_min_hp* heap gbc_bool_min_hp = init_min_heap(dBool)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    bool b[7] = {false, false, false, true, true, true, true};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
}
// --------------------------------------------------------------------------------

void test_min_enqueue_string_gbc(void **state)
{
	(void) state;
    char* a[7] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
    string_min_hp* heap gbc_string_min_hp = init_min_heap(dString)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    char* b[7] = {"Five", "Four", "Seven", "Two", "One", "Three", "Six"};
    for (size_t i = 0; i < 7; i++) {
        assert_string_equal(heap->data[i].data, b[i]);
    }
}
// ================================================================================
// ================================================================================

void test_max_enqueue_char_gbc(void **state)
{
	(void) state;
    char a[7] = {5, 1, 2, 3, 6, 4, 7};
    char_max_hp* heap gbc_char_max_hp = init_max_heap(dChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    char b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_uchar_gbc(void **state)
{
	(void) state;
    unsigned char a[7] = {5, 1, 2, 3, 6, 4, 7};
    uchar_max_hp* heap gbc_uchar_max_hp = init_max_heap(dUChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    char b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_short_gbc(void **state)
{
	(void) state;
    short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    short_max_hp* heap gbc_short_max_hp = init_max_heap(dShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    short int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_ushort_gbc(void **state)
{
	(void) state;
    unsigned short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ushort_max_hp* heap gbc_ushort_max_hp = init_max_heap(dUShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned short int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_int_gbc(void **state)
{
	(void) state;
    int a[7] = {5, 1, 2, 3, 6, 4, 7};
    int_max_hp* heap gbc_int_max_hp = init_max_heap(dInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_uint_gbc(void **state)
{
	(void) state;
    unsigned int a[7] = {5, 1, 2, 3, 6, 4, 7};
    uint_max_hp* heap gbc_uint_max_hp = init_max_heap(dUInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_long_gbc(void **state)
{
	(void) state;
    long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    long_max_hp* heap gbc_long_max_hp = init_max_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    long int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_ulong_gbc(void **state)
{
	(void) state;
    unsigned long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ulong_max_hp* heap gbc_ulong_max_hp = init_max_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned long int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_llong_gbc(void **state)
{
	(void) state;
    long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    llong_max_hp* heap gbc_llong_max_hp = init_max_heap(dLongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    long long int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_ullong_gbc(void **state)
{
	(void) state;
    unsigned long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ullong_max_hp* heap gbc_ullong_max_hp = init_max_heap(dULongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned long long int b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_float_gbc(void **state)
{
	(void) state;
    float a[7] = {5., 1., 2., 3., 6., 4., 7.};
    float_max_hp* heap gbc_float_max_hp = init_max_heap(dFloat)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    float b[7] = {7, 5, 6, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_float_equal(heap->data[i], b[i], 1.0e-3);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_double_gbc(void **state)
{
	(void) state;
    double a[7] = {5., 1., 2., 3., 6., 4., 7.};
    double_max_hp* gbc_double_max_hp heap = init_max_heap(dDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    double b[7] = {7., 5., 6., 1., 3., 2., 4.};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_ldouble_gbc(void **state)
{
	(void) state;
    long double a[7] = {5., 1., 2., 3., 6., 4., 7.};
    ldouble_max_hp* heap gbc_ldouble_max_hp = init_max_heap(dLDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    long double b[7] = {7., 5., 6., 1., 3., 2., 4.};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_bool_gbc(void **state)
{
	(void) state;
    bool a[7] = {true, false, true, false, true, false, true};
    bool_max_hp* heap gbc_bool_max_hp = init_max_heap(dBool)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    bool b[7] = {true, true, true, false, false, false, true};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }

}
// --------------------------------------------------------------------------------

void test_max_enqueue_string_gbc(void **state)
{
	(void) state;
    char* a[7] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
    string_max_hp* heap gbc_string_max_hp = init_max_heap(dString)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    char* b[7] = {"Seven", "Four", "Six", "One", "Three", "Two", "Five"};
    for (size_t i = 0; i < 7; i++) {
        assert_string_equal(heap->data[i].data, b[i]);
    }
}
// ================================================================================
// ================================================================================
// TEST DEQUEUE MIN HEAP 

void test_min_dequeue_char(void **state) {
    (void) state;
    char a[7] = {5, 1, 2, 3, 6, 4, 7};
    char_min_hp* heap = init_min_heap(dChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    char val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    char b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_uchar(void **state) {
    (void) state;
    unsigned char a[7] = {5, 1, 2, 3, 6, 4, 7};
    uchar_min_hp* heap = init_min_heap(dUChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned char val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    unsigned char b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_short(void **state) {
    (void) state;
    short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    short_min_hp* heap = init_min_heap(dShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    short int val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    short int b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_ushort(void **state) {
    (void) state;
    unsigned short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ushort_min_hp* heap = init_min_heap(dUShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned short int val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    unsigned short int b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_int(void **state) {
    (void) state;
    int a[7] = {5, 1, 2, 3, 6, 4, 7};
    int_min_hp* heap = init_min_heap(dInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    int val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    int b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_uint(void **state) {
    (void) state;
    unsigned int a[7] = {5, 1, 2, 3, 6, 4, 7};
    uint_min_hp* heap = init_min_heap(dUInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned int val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    unsigned int b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_long(void **state) {
    (void) state;
    long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    long_min_hp* heap = init_min_heap(dLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    long int val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    long int b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_ulong(void **state) {
    (void) state;
    unsigned long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ulong_min_hp* heap = init_min_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned long int val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    unsigned long int b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_llong(void **state) {
    (void) state;
    long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    llong_min_hp* heap = init_min_heap(dLongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    long long int val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    long long int b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_ullong(void **state) {
    (void) state;
    unsigned long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ullong_min_hp* heap = init_min_heap(dULongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    unsigned long long int val = dequeue_min_heap(heap);
    assert_int_equal(val, 1);
    unsigned long long int b[6] = {2, 3, 4, 5, 6, 7};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_float(void **state) {
    (void) state;
    float a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    float_min_hp* heap = init_min_heap(dFloat)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    float val = dequeue_min_heap(heap);
    assert_float_equal(val, 1.1, 1.0e-3);
    float b[6] = {2.2, 3.3, 4.4, 5.5, 6.6, 7.7};
    for (size_t i = 0; i < 6; i++) {
        assert_float_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_double(void **state) {
    (void) state;
    double a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    double_min_hp* heap = init_min_heap(dDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    double val = dequeue_min_heap(heap);
    assert_double_equal(val, 1.1, 1.0e-3);
    double b[6] = {2.2, 3.3, 4.4, 5.5, 6.6, 7.7};
    for (size_t i = 0; i < 6; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_ldouble(void **state) {
    (void) state;
    long double a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    ldouble_min_hp* heap = init_min_heap(dLDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    long double val = dequeue_min_heap(heap);
    assert_double_equal(val, 1.1, 1.0e-3);
    long double b[6] = {2.2, 3.3, 4.4, 5.5, 6.6, 7.7};
    for (size_t i = 0; i < 6; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_bool(void **state)
{
	(void) state;
    bool a[7] = {true, false, true, false, true, false, true};
    bool_min_hp* heap = init_min_heap(dBool)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    bool val = dequeue_min_heap(heap);
    assert_int_equal(val, false);
    bool b[6] = {false, true, false, true, true, true};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_dequeue_string(void **state)
{
	(void) state;
    char* a[7] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
    string_min_hp* heap = init_min_heap(dString)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    str* val = dequeue_min_heap(heap);
    assert_string_equal(val->data, "Five");
    char* b[6] = {"Four", "One", "Seven", "Two", "Six", "Three"};
    for (size_t i = 0; i < 6; i++) {
        assert_string_equal(heap->data[i].data, b[i]);
    }
    free_min_heap(heap);
    free_string(val);
}
// ================================================================================
// ================================================================================

void test_max_dequeue_char(void **state) {
    (void) state;
    char a[7] = {5, 1, 2, 3, 6, 4, 7};
    char_max_hp* heap = init_max_heap(dChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    char val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    char b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_uchar(void **state) {
    (void) state;
    unsigned char a[7] = {5, 1, 2, 3, 6, 4, 7};
    uchar_max_hp* heap = init_max_heap(dUChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned char val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    unsigned char b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_short(void **state) {
    (void) state;
    short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    short_max_hp* heap = init_max_heap(dShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    short int val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    short int b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_ushort(void **state) {
    (void) state;
    unsigned short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ushort_max_hp* heap = init_max_heap(dUShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned short int val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    char b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_int(void **state) {
    (void) state;
    int a[7] = {5, 1, 2, 3, 6, 4, 7};
    int_max_hp* heap = init_max_heap(dInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    int val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    int b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_uint(void **state) {
    (void) state;
    unsigned int a[7] = {5, 1, 2, 3, 6, 4, 7};
    uint_max_hp* heap = init_max_heap(dUInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned int val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    unsigned int b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_long(void **state) {
    (void) state;
    long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    long_max_hp* heap = init_max_heap(dLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    long int val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    long int b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}  
// --------------------------------------------------------------------------------

void test_max_dequeue_ulong(void **state) {
    (void) state;
    unsigned long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ulong_max_hp* heap = init_max_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned long int val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    unsigned long int b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_llong(void **state) {
    (void) state;
    long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    llong_max_hp* heap = init_max_heap(dLongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    long long int val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    long long int b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_ullong(void **state) {
    (void) state;
    unsigned long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ullong_max_hp* heap = init_max_heap(dULongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    unsigned long long int val = dequeue_max_heap(heap);
    assert_int_equal(val, 7);
    unsigned long long int b[6] = {4, 5, 6, 1, 3, 2};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_float(void **state) {
    (void) state;
    float a[7] = {5., 1., 2., 3., 6., 4., 7.};
    float_max_hp* heap = init_max_heap(dFloat)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    float val = dequeue_max_heap(heap);
    assert_float_equal(val, 7.0, 1.0e-3);
    float b[6] = {4., 5., 6., 1., 3., 2.};
    for (size_t i = 0; i < 6; i++) {
        assert_float_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_double(void **state) {
    (void) state;
    double a[7] = {5., 1., 2., 3., 6., 4., 7.};
    double_max_hp* heap = init_max_heap(dDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    double val = dequeue_max_heap(heap);
    assert_double_equal(val, 7.0, 1.0e-3);
    float b[6] = {4., 5., 6., 1., 3., 2.};
    for (size_t i = 0; i < 6; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_ldouble(void **state) {
    (void) state;
    long double a[7] = {5., 1., 2., 3., 6., 4., 7.};
    ldouble_max_hp* heap = init_max_heap(dLDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    long double val = dequeue_max_heap(heap);
    assert_double_equal(val, 7.0, 1.0e-3);
    float b[6] = {4., 5., 6., 1., 3., 2.};
    for (size_t i = 0; i < 6; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_bool(void **state) {
    (void) state;
    bool a[7] = {true, false, true, false, true, false, true};
    bool_max_hp* heap = init_max_heap(dBool)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    bool val = dequeue_max_heap(heap);
    assert_int_equal(val, true);
    bool b[6] = {true, true, true, false, false, false};
    for (size_t i = 0; i < 6; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_dequeue_string(void **state)
{
	(void) state;
    char* a[7] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
    string_max_hp* heap = init_min_heap(dString)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    str* val = dequeue_max_heap(heap);
    assert_string_equal(val->data, "Seven");
    char* b[6] = {"Five", "Four", "Six", "One", "Three", "Two"};
    for (size_t i = 0; i < 6; i++) {
        assert_string_equal(heap->data[i].data, b[i]);
    }
    free_max_heap(heap);
    free_string(val);
}
// ================================================================================
// ================================================================================
// TEST REPLACE INDEX 

void test_min_replace_index_char(void **state)
{
	(void) state;
    char a[7] = {5, 1, 2, 3, 6, 4, 7};
    char_min_hp* heap = init_min_heap(dChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    char b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_uchar(void **state)
{
	(void) state;
    unsigned char a[7] = {5, 1, 2, 3, 6, 4, 7};
    uchar_min_hp* heap = init_min_heap(dUChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    unsigned char b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_short(void **state)
{
	(void) state;
    short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    short_min_hp* heap = init_min_heap(dShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    short int b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_ushort(void **state)
{
	(void) state;
    unsigned short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ushort_min_hp* heap = init_min_heap(dUShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    unsigned short int b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_int(void **state)
{
	(void) state;
    int a[7] = {5, 1, 2, 3, 6, 4, 7};
    int_min_hp* heap = init_min_heap(dInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    int b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_uint(void **state)
{
	(void) state;
    unsigned int a[7] = {5, 1, 2, 3, 6, 4, 7};
    uint_min_hp* heap = init_min_heap(dUInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    unsigned int b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_long(void **state)
{
	(void) state;
    long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    long_min_hp* heap = init_min_heap(dLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    long int b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_ulong(void **state)
{
	(void) state;
    unsigned long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ulong_min_hp* heap = init_min_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    unsigned long int b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_llong(void **state)
{
	(void) state;
    long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    llong_min_hp* heap = init_min_heap(dLongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    long long int b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_ullong(void **state)
{
	(void) state;
    unsigned long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ullong_min_hp* heap = init_min_heap(dULongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12);
    unsigned long long int b[7] = {1, 3, 4, 5, 6, 12, 7};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_float(void **state)
{
	(void) state;
    float a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    float_min_hp* heap = init_min_heap(dFloat)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12.12);
    float b[7] = {1.1, 3.3, 4.4, 5.5, 6.6, 12.12, 7.7};
    for (size_t i = 0; i < 7; i++) {
        assert_float_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_double(void **state)
{
	(void) state;
    double a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    double_min_hp* heap = init_min_heap(dDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12.12);
    double b[7] = {1.1, 3.3, 4.4, 5.5, 6.6, 12.12, 7.7};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_ldouble(void **state)
{
	(void) state;
    long double a[7] = {5.5, 1.1, 2.2, 3.3, 6.6, 4.4, 7.7};
    ldouble_min_hp* heap = init_min_heap(dLDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, 12.12);
    long double b[7] = {1.1, 3.3, 4.4, 5.5, 6.6, 12.12, 7.7};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_min_heap(heap);
} 
// --------------------------------------------------------------------------------

void test_min_replace_index_bool(void **state)
{
	(void) state;
    bool a[7] = {true, false, true, false, true, false, true};
    bool_min_hp* heap = init_min_heap(dBool)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, true);
    bool b[7] = {false, false, true, true, true, true, true};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_min_heap(heap);
}
// --------------------------------------------------------------------------------

void test_min_replace_index_string(void **state)
{
	(void) state;
    char* a[7] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
    string_min_hp* heap = init_min_heap(dString)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_min_heap(heap, a[i]);
    }
    replace_min_heap_index(heap, 2, "Goodbye");
    char* b[7] = {"Five", "Four", "Goodbye", "Two", "One", "Three", "Six"};
    for (size_t i = 0; i < 7; i++) {
        assert_string_equal(heap->data[i].data, b[i]);
    }
    free_min_heap(heap);
}
// ================================================================================
// ================================================================================

void test_max_replace_index_char(void **state)
{
	(void) state;
    char a[7] = {5, 1, 2, 3, 6, 4, 7};
    char_max_hp* heap = init_max_heap(dChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    char b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_uchar(void **state)
{
	(void) state;
    unsigned char a[7] = {5, 1, 2, 3, 6, 4, 7};
    uchar_max_hp* heap = init_max_heap(dUChar)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    unsigned char b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_short(void **state)
{
	(void) state;
    short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    short_max_hp* heap = init_max_heap(dShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    short int b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_ushort(void **state)
{
	(void) state;
    unsigned short int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ushort_max_hp* heap = init_max_heap(dUShort)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    short int b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_int(void **state)
{
	(void) state;
    int a[7] = {5, 1, 2, 3, 6, 4, 7};
    int_max_hp* heap = init_max_heap(dInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    int b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_uint(void **state)
{
	(void) state;
    unsigned int a[7] = {5, 1, 2, 3, 6, 4, 7};
    uint_max_hp* heap = init_max_heap(dUInt)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    unsigned int b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_long(void **state)
{
	(void) state;
    long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    long_max_hp* heap = init_max_heap(dLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    long int b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_ulong(void **state)
{
	(void) state;
    unsigned long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ulong_max_hp* heap = init_max_heap(dULong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    unsigned long int b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_llong(void **state)
{
	(void) state;
    long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    llong_max_hp* heap = init_max_heap(dLongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    long long int b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_ullong(void **state)
{
	(void) state;
    unsigned long long int a[7] = {5, 1, 2, 3, 6, 4, 7};
    ullong_max_hp* heap = init_max_heap(dULongLong)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    unsigned long long int b[7] = {12, 5, 7, 1, 3, 2, 4};
    for (size_t i = 0; i < 7; i++) {
        assert_int_equal(heap->data[i], b[i]);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_float(void **state)
{
	(void) state;
    float a[7] = {5., 1., 2., 3., 6., 4., 7.};
    float_max_hp* heap = init_max_heap(dFloat)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    float b[7] = {12., 5., 7., 1., 3., 2., 4.};
    for (size_t i = 0; i < 7; i++) {
        assert_float_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_double(void **state)
{
	(void) state;
    double a[7] = {5., 1., 2., 3., 6., 4., 7.};
    double_max_hp* heap = init_max_heap(dDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    double b[7] = {12., 5., 7., 1., 3., 2., 4.};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_ldouble(void **state)
{
	(void) state;
    long double a[7] = {5., 1., 2., 3., 6., 4., 7.};
    ldouble_max_hp* heap = init_max_heap(dLDouble)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, 12);
    long double b[7] = {12., 5., 7., 1., 3., 2., 4.};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_max_heap(heap);
}
// --------------------------------------------------------------------------------

void test_max_replace_index_bool(void **state)
{
	(void) state;
    bool a[7] = {true, false, true, false, true, false, true};
    bool_max_hp* heap = init_max_heap(dBool)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, false);
    bool b[7] = {true, true, false, false, false, false, true};
    for (size_t i = 0; i < 7; i++) {
        assert_double_equal(heap->data[i], b[i], 1.0e-3);
    }
    free_max_heap(heap);
}  
// --------------------------------------------------------------------------------

void test_max_replace_index_string(void **state)
{
	(void) state;
    char* a[7] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
    string_max_hp* heap = init_min_heap(dString)(7);
    for (size_t i = 0; i < 7; i++) {
        enqueue_max_heap(heap, a[i]);
    }
    replace_max_heap_index(heap, 2, "Goodbye");
    char* b[7] = {"Seven", "Four", "Five", "One", "Three", "Two", "Goodbye"};
    for (size_t i = 0; i < 7; i++) {
        assert_string_equal(heap->data[i].data, b[i]);
    }
    free_max_heap(heap);
}
// ================================================================================
// ================================================================================
// eof
