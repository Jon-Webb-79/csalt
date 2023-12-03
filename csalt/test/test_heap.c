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
// eof
