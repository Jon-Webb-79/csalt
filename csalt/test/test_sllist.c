// ================================================================================
// ================================================================================
// - File:    test_sllist.c
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

#include "test_sllist.h"
// ================================================================================
// ================================================================================

void test_push_char_sllist(void **state)
{
	(void) state;
	char_sl* list = init_sllist(dChar)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    char arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_uchar_sllist(void **state)
{
	(void) state;
	uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    unsigned char arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_short_sllist(void **state)
{
	(void) state;
	short_sl* list = init_sllist(dShort)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    short int arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_ushort_sllist(void **state)
{
	(void) state;
	ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    unsigned short int arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_int_sllist(void **state)
{
	(void) state;
	int_sl* list = init_sllist(dInt)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    int arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_uint_sllist(void **state)
{
	(void) state;
	uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    unsigned int arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_long_sllist(void **state)
{
	(void) state;
	long_sl* list = init_sllist(dLong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    long int arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_ulong_sllist(void **state)
{
	(void) state;
	ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    unsigned long int arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_llong_sllist(void **state)
{
	(void) state;
	llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    long long int arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_ullong_sllist(void **state)
{
	(void) state;
	ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 1);
    push_sllist(list, 5, 0);
    unsigned long long int arr[5] = {5, 1, 4, 2, 3};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_float_sllist(void **state)
{
	(void) state;
	float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 1., 0);
    push_sllist(list, 2., 1);
    push_sllist(list, 3., 2);
    push_sllist(list, 4., 1);
    push_sllist(list, 5., 0);
    float arr[5] = {5., 1., 4., 2., 3.};
    for (size_t i = 0; i < 5; i++) {
        assert_float_equal(arr[i], get_sllist(list, i), 1.0e-3);
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_double_sllist(void **state)
{
	(void) state;
	double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 1., 0);
    push_sllist(list, 2., 1);
    push_sllist(list, 3., 2);
    push_sllist(list, 4., 1);
    push_sllist(list, 5., 0);
    double arr[5] = {5., 1., 4., 2., 3.};
    for (size_t i = 0; i < 5; i++) {
        assert_double_equal(arr[i], get_sllist(list, i), 1.0e-3);
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_ldouble_sllist(void **state)
{
	(void) state;
	ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 1., 0);
    push_sllist(list, 2., 1);
    push_sllist(list, 3., 2);
    push_sllist(list, 4., 1);
    push_sllist(list, 5., 0);
    long double arr[5] = {5., 1., 4., 2., 3.};
    for (size_t i = 0; i < 5; i++) {
        assert_double_equal(arr[i], get_sllist(list, i), 1.0e-3);
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_bool_sllist(void **state)
{
	(void) state;
	bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, false, 1);
    push_sllist(list, true, 2);
    push_sllist(list, false, 1);
    push_sllist(list, true, 0);
    bool arr[5] = {true, true, false, false, true};
    for (size_t i = 0; i < 5; i++) {
        assert_int_equal(arr[i], get_sllist(list, i));
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_push_string_sllist(void **state)
{
	(void) state;
	string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 1);
    push_sllist(list, "Three", 2);
    push_sllist(list, "Four", 1);
    push_sllist(list, "Five", 0);
    char *arr[5] = {"Five", "One", "Four", "Two", "Three"};
    for (size_t i = 0; i < 5; i++) {
        str* test = get_sllist(list, i);
        assert_string_equal(arr[i], test->data);
        free_string(test);
    }
    assert_int_equal(5, sllist_length(list));
    free_sllist(list);
}
// ================================================================================
// ================================================================================

void test_pop_char_sllist(void **state) {
    (void) state;
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    char a[4] = {1, 2, 4, 5};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_uchar_sllist(void **state) {
    (void) state;
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    unsigned char a[4] = {1, 2, 4, 5};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_short_sllist(void **state) {
    (void) state;
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    short int a[4] = {1, 2, 4, 5};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_ushort_sllist(void **state) {
    (void) state;
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    unsigned short int a[4] = {1, 2, 4, 5};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_int_sllist(void **state) {
    (void) state;
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    int a[4] = {1, 2, 4, 5};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_uint_sllist(void **state) {
    (void) state;
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    unsigned int a[4] = {1, 2, 4, 5};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_long_sllist(void **state) {
    (void) state;
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    long int a[4] = {1, 2, 4, 5};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_ulong_sllist(void **state) {
    (void) state;
    ulong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    unsigned long a[4] = {1, 2, 4, 5};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_llong_sllist(void **state) {
    (void) state;
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    long long int a[4] = {1, 2, 4, 5};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_ullong_sllist(void **state) {
    (void) state;
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 0, list->len);
    push_sllist(list, 1, list->len);
    push_sllist(list, 2, list->len);
    push_sllist(list, 3, list->len);
    push_sllist(list, 4, list->len);
    push_sllist(list, 5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    unsigned long long int a[4] = {1, 2, 4, 5};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_float_sllist(void **state) {
    (void) state;
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 0.0, list->len);
    push_sllist(list, 1.1, list->len);
    push_sllist(list, 2.2, list->len);
    push_sllist(list, 3.3, list->len);
    push_sllist(list, 4.4, list->len);
    push_sllist(list, 5.5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    float a[4] = {1.1, 2.2, 4.4, 5.5};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_double_sllist(void **state) {
    (void) state;
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 0.0, list->len);
    push_sllist(list, 1.1, list->len);
    push_sllist(list, 2.2, list->len);
    push_sllist(list, 3.3, list->len);
    push_sllist(list, 4.4, list->len);
    push_sllist(list, 5.5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    double a[4] = {1.1, 2.2, 4.4, 5.5};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_ldouble_sllist(void **state) {
    (void) state;
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 0.0, list->len);
    push_sllist(list, 1.1, list->len);
    push_sllist(list, 2.2, list->len);
    push_sllist(list, 3.3, list->len);
    push_sllist(list, 4.4, list->len);
    push_sllist(list, 5.5, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    long double a[4] = {1.1, 2.2, 4.4, 5.5};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_bool_sllist(void **state) {
    (void) state;
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, list->len);
    push_sllist(list, false, list->len);
    push_sllist(list, true, list->len);
    push_sllist(list, true, list->len);
    push_sllist(list, false, list->len);
    push_sllist(list, false, list->len);
    pop_sllist(list, 0);
    pop_sllist(list, 2);
    bool a[4] = {false, true, false, false};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_pop_string_sllist(void **state) {
    (void) state;
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "Zero", list->len);
    push_sllist(list, "One", list->len);
    push_sllist(list, "Two", list->len);
    push_sllist(list, "Three", list->len);
    push_sllist(list, "Four", list->len);
    push_sllist(list, "Five", list->len);
    str* one = pop_sllist(list, 0);
    str* two = pop_sllist(list, 2);
    assert_string_equal("Zero", one->data);
    assert_string_equal("Three", two->data);
    char *a[4] = {"One", "Two", "Four", "Five"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_string(one);
    free_string(two);
    free_sllist(list);
}
// ================================================================================
// ================================================================================
// TEST REVERSE LIST 

void test_reverse_char_sllist(void **state) {
    (void) state;
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    char a[7] = {1, 2, 3, 4, 5, 6, 7};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_uchar_sllist(void **state) {
    (void) state;
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    unsigned char a[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_short_sllist(void **state) {
    (void) state;
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_ushort_sllist(void **state) {
    (void) state;
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    unsigned short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_int_sllist(void **state) {
    (void) state;
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_uint_sllist(void **state) {
    (void) state;
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    unsigned int a[7] = {1, 2, 3, 4, 5, 6, 7};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_long_sllist(void **state) {
    (void) state;
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_ulong_sllist(void **state) {
    (void) state;
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    unsigned long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_llong_sllist(void **state) {
    (void) state;
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_ullong_sllist(void **state) {
    (void) state;
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 5, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 7, 0);
    reverse_sllist(list);
    unsigned long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_float_sllist(void **state) {
    (void) state;
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 1., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 5., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 7., 0);
    reverse_sllist(list);
    float a[7] = {1., 2., 3., 4., 5., 6., 7.};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_double_sllist(void **state) {
    (void) state;
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 1., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 5., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 7., 0);
    reverse_sllist(list);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_ldouble_sllist(void **state) {
    (void) state;
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 1., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 5., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 7., 0);
    reverse_sllist(list);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_bool_sllist(void **state) {
    (void) state;
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, true, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, false, 0);
    push_sllist(list, false, 0);
    push_sllist(list, false, 0);
    reverse_sllist(list);
    bool a[7] = {true, true, true, false, false, false, false};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_reverse_string_sllist(void **state) {
    (void) state;
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 0);
    push_sllist(list, "Three", 0);
    push_sllist(list, "Four", 0);
    push_sllist(list, "Five", 0);
    push_sllist(list, "Six", 0);
    push_sllist(list, "Seven", 0);
    reverse_sllist(list);
    char *a[7] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_sllist(list);
}
// ================================================================================
// ================================================================================
// TEST BUBBLE SORT 

void test_bubble_sort_char_sllist_forward(void **state) {
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    char a[7] = {1, 2, 3, 4, 5, 6, 7};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uchar_sllist_forward(void **state) {
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    unsigned char a[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_short_sllist_forward(void **state) {
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ushort_sllist_forward(void **state) {
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    unsigned short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_int_sllist_forward(void **state) {
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uint_sllist_forward(void **state) {
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    unsigned int a[7] = {1, 2, 3, 4, 5, 6, 7};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_long_sllist_forward(void **state) {
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ulong_sllist_forward(void **state) {
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    unsigned long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_llong_sllist_forward(void **state) {
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ullong_sllist_forward(void **state) {
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    unsigned long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_float_sllist_forward(void **state) {
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, BUBBLE, FORWARD);
    float a[7] = {1., 2., 3., 4., 5., 6., 7.};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_double_sllist_forward(void **state) {
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, BUBBLE, FORWARD);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ldouble_sllist_forward(void **state) {
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, BUBBLE, FORWARD);
    long double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_bool_sllist_forward(void **state) {
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    sort_sllist(list, BUBBLE, FORWARD);
    bool a[7] = {false, false, false, true, true, true, true};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_string_sllist_forward(void **state) {
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 0);
    push_sllist(list, "Three", 0);
    push_sllist(list, "Four", 0);
    push_sllist(list, "Five", 0);
    push_sllist(list, "Six", 0);
    push_sllist(list, "Seven", 0);
    sort_sllist(list, BUBBLE, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_char_sllist_reverse(void **state) {
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    char a[7] = {7, 6, 5, 4, 3, 2, 1};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uchar_sllist_reverse(void **state) {
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    unsigned char a[7] = {7, 6, 5, 4, 3, 2, 1};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_short_sllist_reverse(void **state) {
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    short int a[7] = {7, 6, 5, 4, 3, 2, 1};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ushort_sllist_reverse(void **state) {
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    unsigned short int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_int_sllist_reverse(void **state) {
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    int a[7] = {7, 6, 5, 4, 3, 2, 1};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uint_sllist_reverse(void **state) {
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    unsigned int a[7] = {7, 6, 5, 4, 3, 2, 1};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_long_sllist_reverse(void **state) {
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ulong_sllist_reverse(void **state) {
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    unsigned long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_llong_sllist_reverse(void **state) {
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ullong_sllist_reverse(void **state) {
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    unsigned long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_float_sllist_reverse(void **state) {
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, BUBBLE, REVERSE);
    float a[7] = {7., 6., 5., 4., 3., 2., 1.};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_double_sllist_reverse(void **state) {
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, BUBBLE, REVERSE);
    double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ldouble_sllist_reverse(void **state) {
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, BUBBLE, REVERSE);
    long double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_bool_sllist_reverse(void **state) {
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    sort_sllist(list, BUBBLE, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_string_sllist_reverse(void **state) {
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 0);
    push_sllist(list, "Three", 0);
    push_sllist(list, "Four", 0);
    push_sllist(list, "Five", 0);
    push_sllist(list, "Six", 0);
    push_sllist(list, "Seven", 0);
    sort_sllist(list, BUBBLE, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// ================================================================================
// ================================================================================
// TEST SELLECTION SORT 

void test_selection_sort_char_sllist_forward(void **state) {
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    char a[7] = {1, 2, 3, 4, 5, 6, 7};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_uchar_sllist_forward(void **state) {
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    unsigned char a[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_short_sllist_forward(void **state) {
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ushort_sllist_forward(void **state) {
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    unsigned short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_int_sllist_forward(void **state) {
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_uint_sllist_forward(void **state) {
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    unsigned int a[7] = {1, 2, 3, 4, 5, 6, 7};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_long_sllist_forward(void **state) {
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ulong_sllist_forward(void **state) {
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    unsigned long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_llong_sllist_forward(void **state) {
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ullong_sllist_forward(void **state) {
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, FORWARD);
    unsigned long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_float_sllist_forward(void **state) {
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, SELECTION, FORWARD);
    float a[7] = {1., 2., 3., 4., 5., 6., 7.};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_double_sllist_forward(void **state) {
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, SELECTION, FORWARD);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ldouble_sllist_forward(void **state) {
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, SELECTION, FORWARD);
    long double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_bool_sllist_forward(void **state) {
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    sort_sllist(list, SELECTION, FORWARD);
    bool a[7] = {false, false, false, true, true, true, true};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_string_sllist_forward(void **state) {
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 0);
    push_sllist(list, "Three", 0);
    push_sllist(list, "Four", 0);
    push_sllist(list, "Five", 0);
    push_sllist(list, "Six", 0);
    push_sllist(list, "Seven", 0);
    sort_sllist(list, SELECTION, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_char_sllist_reverse(void **state) {
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    char a[7] = {7, 6, 5, 4, 3, 2, 1};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_uchar_sllist_reverse(void **state) {
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    unsigned char a[7] = {7, 6, 5, 4, 3, 2, 1};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_short_sllist_reverse(void **state) {
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    short int a[7] = {7, 6, 5, 4, 3, 2, 1};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ushort_sllist_reverse(void **state) {
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    unsigned short int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_int_sllist_reverse(void **state) {
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    int a[7] = {7, 6, 5, 4, 3, 2, 1};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_uint_sllist_reverse(void **state) {
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    unsigned int a[7] = {7, 6, 5, 4, 3, 2, 1};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_long_sllist_reverse(void **state) {
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ulong_sllist_reverse(void **state) {
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    unsigned long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_llong_sllist_reverse(void **state) {
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ullong_sllist_reverse(void **state) {
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, SELECTION, REVERSE);
    unsigned long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_float_sllist_reverse(void **state) {
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, SELECTION, REVERSE);
    float a[7] = {7., 6., 5., 4., 3., 2., 1.};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_double_sllist_reverse(void **state) {
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, SELECTION, REVERSE);
    double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ldouble_sllist_reverse(void **state) {
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, SELECTION, REVERSE);
    long double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_bool_sllist_reverse(void **state) {
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    sort_sllist(list, SELECTION, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_string_sllist_reverse(void **state) {
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 0);
    push_sllist(list, "Three", 0);
    push_sllist(list, "Four", 0);
    push_sllist(list, "Five", 0);
    push_sllist(list, "Six", 0);
    push_sllist(list, "Seven", 0);
    sort_sllist(list, SELECTION, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// ================================================================================
// ================================================================================
// TEST INSERT SORT 

void test_insert_sort_char_sllist_forward(void **state) {
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    char a[7] = {1, 2, 3, 4, 5, 6, 7};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_uchar_sllist_forward(void **state) {
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    unsigned char a[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_short_sllist_forward(void **state) {
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ushort_sllist_forward(void **state) {
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    unsigned short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_int_sllist_forward(void **state) {
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_uint_sllist_forward(void **state) {
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    unsigned int a[7] = {1, 2, 3, 4, 5, 6, 7};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_long_sllist_forward(void **state) {
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ulong_sllist_forward(void **state) {
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    unsigned long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_llong_sllist_forward(void **state) {
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ullong_sllist_forward(void **state) {
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, FORWARD);
    unsigned long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_float_sllist_forward(void **state) {
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, INSERT, FORWARD);
    float a[7] = {1., 2., 3., 4., 5., 6., 7.};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_double_sllist_forward(void **state) {
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, INSERT, FORWARD);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ldouble_sllist_forward(void **state) {
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, INSERT, FORWARD);
    long double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_bool_sllist_forward(void **state) {
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    sort_sllist(list, INSERT, FORWARD);
    bool a[7] = {false, false, false, true, true, true, true};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_string_sllist_forward(void **state) {
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 0);
    push_sllist(list, "Three", 0);
    push_sllist(list, "Four", 0);
    push_sllist(list, "Five", 0);
    push_sllist(list, "Six", 0);
    push_sllist(list, "Seven", 0);
    sort_sllist(list, INSERT, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_char_sllist_reverse(void **state) {
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    char a[7] = {7, 6, 5, 4, 3, 2, 1};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_uchar_sllist_reverse(void **state) {
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    unsigned char a[7] = {7, 6, 5, 4, 3, 2, 1};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_short_sllist_reverse(void **state) {
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    short int a[7] = {7, 6, 5, 4, 3, 2, 1};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ushort_sllist_reverse(void **state) {
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    unsigned short int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_int_sllist_reverse(void **state) {
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    int a[7] = {7, 6, 5, 4, 3, 2, 1};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_uint_sllist_reverse(void **state) {
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    unsigned int a[7] = {7, 6, 5, 4, 3, 2, 1};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_long_sllist_reverse(void **state) {
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ulong_sllist_reverse(void **state) {
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    unsigned long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_llong_sllist_reverse(void **state) {
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ullong_sllist_reverse(void **state) {
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, INSERT, REVERSE);
    unsigned long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_float_sllist_reverse(void **state) {
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, INSERT, REVERSE);
    float a[7] = {7., 6., 5., 4., 3., 2., 1.};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_double_sllist_reverse(void **state) {
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, INSERT, REVERSE);
    double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ldouble_sllist_reverse(void **state) {
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, INSERT, REVERSE);
    long double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_bool_sllist_reverse(void **state) {
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    sort_sllist(list, INSERT, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_insert_sort_string_sllist_reverse(void **state) {
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 0);
    push_sllist(list, "Three", 0);
    push_sllist(list, "Four", 0);
    push_sllist(list, "Five", 0);
    push_sllist(list, "Six", 0);
    push_sllist(list, "Seven", 0);
    sort_sllist(list, INSERT, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// ================================================================================
// ================================================================================
// TEST MERGE SORT 

void test_merge_sort_char_sllist_forward(void **state) {
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    char a[7] = {1, 2, 3, 4, 5, 6, 7};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uchar_sllist_forward(void **state) {
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    unsigned char a[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_short_sllist_forward(void **state) {
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ushort_sllist_forward(void **state) {
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    unsigned short int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_int_sllist_forward(void **state) {
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uint_sllist_forward(void **state) {
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    unsigned int a[7] = {1, 2, 3, 4, 5, 6, 7};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_long_sllist_forward(void **state) {
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ulong_sllist_forward(void **state) {
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    unsigned long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_llong_sllist_forward(void **state) {
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ullong_sllist_forward(void **state) {
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, FORWARD);
    unsigned long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_float_sllist_forward(void **state) {
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, MERGE, FORWARD);
    float a[7] = {1., 2., 3., 4., 5., 6., 7.};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_double_sllist_forward(void **state) {
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, MERGE, FORWARD);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ldouble_sllist_forward(void **state) {
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, MERGE, FORWARD);
    long double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_bool_sllist_forward(void **state) {
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    sort_sllist(list, MERGE, FORWARD);
    bool a[7] = {false, false, false, true, true, true, true};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_string_sllist_forward(void **state) {
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 0);
    push_sllist(list, "Three", 0);
    push_sllist(list, "Four", 0);
    push_sllist(list, "Five", 0);
    push_sllist(list, "Six", 0);
    push_sllist(list, "Seven", 0);
    sort_sllist(list, MERGE, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_char_sllist_reverse(void **state) {
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    char a[7] = {7, 6, 5, 4, 3, 2, 1};
    char_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uchar_sllist_reverse(void **state) {
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    unsigned char a[7] = {7, 6, 5, 4, 3, 2, 1};
    uchar_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_short_sllist_reverse(void **state) {
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    short int a[7] = {7, 6, 5, 4, 3, 2, 1};
    short_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ushort_sllist_reverse(void **state) {
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    unsigned short int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ushort_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_int_sllist_reverse(void **state) {
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    int a[7] = {7, 6, 5, 4, 3, 2, 1};
    int_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uint_sllist_reverse(void **state) {
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    unsigned int a[7] = {7, 6, 5, 4, 3, 2, 1};
    uint_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_long_sllist_reverse(void **state) {
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    long_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ulong_sllist_reverse(void **state) {
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    unsigned long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ulong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_llong_sllist_reverse(void **state) {
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    llong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ullong_sllist_reverse(void **state) {
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 5, 0);
    push_sllist(list, 1, 0);
    push_sllist(list, 6, 0);
    push_sllist(list, 3, 0);
    push_sllist(list, 2, 0);
    push_sllist(list, 4, 0);
    push_sllist(list, 7, 0);
    sort_sllist(list, MERGE, REVERSE);
    unsigned long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ullong_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_float_sllist_reverse(void **state) {
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, MERGE, REVERSE);
    float a[7] = {7., 6., 5., 4., 3., 2., 1.};
    float_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_double_sllist_reverse(void **state) {
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, MERGE, REVERSE);
    double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    double_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ldouble_sllist_reverse(void **state) {
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 5., 0);
    push_sllist(list, 1., 0);
    push_sllist(list, 6., 0);
    push_sllist(list, 3., 0);
    push_sllist(list, 2., 0);
    push_sllist(list, 4., 0);
    push_sllist(list, 7., 0);
    sort_sllist(list, MERGE, REVERSE);
    long double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    ldouble_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_bool_sllist_reverse(void **state) {
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    push_sllist(list, false, 0);
    push_sllist(list, true, 0);
    sort_sllist(list, MERGE, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    bool_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_string_sllist_reverse(void **state) {
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 0);
    push_sllist(list, "Three", 0);
    push_sllist(list, "Four", 0);
    push_sllist(list, "Five", 0);
    push_sllist(list, "Six", 0);
    push_sllist(list, "Seven", 0);
    sort_sllist(list, MERGE, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    string_slnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    } 
    free_sllist(list);
}
// ================================================================================
// ================================================================================

void test_sllist_char_iterator(void **state) {
    char_sl* list = init_sllist(dChar)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    char_sl_iterator it = sllist_iterator(list);
    char_slnode* begin = it.begin(list);
    char_slnode* end = it.end(list);
    char a;
    for (char_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    char b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (char_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_uchar_iterator(void **state) {
    uchar_sl* list = init_sllist(dUChar)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    uchar_sl_iterator it = sllist_iterator(list);
    uchar_slnode* begin = it.begin(list);
    uchar_slnode* end = it.end(list);
    unsigned char a;
    for (uchar_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    unsigned char b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (uchar_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_short_iterator(void **state) {
    short_sl* list = init_sllist(dShort)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    short_sl_iterator it = sllist_iterator(list);
    short_slnode* begin = it.begin(list);
    short_slnode* end = it.end(list);
    short a;
    for (short_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    short b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (short_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_ushort_iterator(void **state) {
    ushort_sl* list = init_sllist(dUShort)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    ushort_sl_iterator it = sllist_iterator(list);
    ushort_slnode* begin = it.begin(list);
    ushort_slnode* end = it.end(list);
    unsigned short a;
    for (ushort_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    unsigned short b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (ushort_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_int_iterator(void **state) {
    int_sl* list = init_sllist(dInt)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    int_sl_iterator it = sllist_iterator(list);
    int_slnode* begin = it.begin(list);
    int_slnode* end = it.end(list);
    int a;
    for (int_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    int b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (int_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_uint_iterator(void **state) {
    uint_sl* list = init_sllist(dUInt)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    uint_sl_iterator it = sllist_iterator(list);
    uint_slnode* begin = it.begin(list);
    uint_slnode* end = it.end(list);
    unsigned int a;
    for (uint_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    unsigned int b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (uint_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_long_iterator(void **state) {
    long_sl* list = init_sllist(dLong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    long_sl_iterator it = sllist_iterator(list);
    long_slnode* begin = it.begin(list);
    long_slnode* end = it.end(list);
    long int a;
    for (long_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    long int b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (long_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_ulong_iterator(void **state) {
    ulong_sl* list = init_sllist(dULong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    ulong_sl_iterator it = sllist_iterator(list);
    ulong_slnode* begin = it.begin(list);
    ulong_slnode* end = it.end(list);
    unsigned long a;
    for (ulong_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    unsigned long b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (ulong_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_llong_iterator(void **state) {
    llong_sl* list = init_sllist(dLongLong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    llong_sl_iterator it = sllist_iterator(list);
    llong_slnode* begin = it.begin(list);
    llong_slnode* end = it.end(list);
    long long a;
    for (llong_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    long long int b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (llong_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_ullong_iterator(void **state) {
    ullong_sl* list = init_sllist(dULongLong)();
    push_sllist(list, 1, 0);
    push_sllist(list, 2, 1);
    push_sllist(list, 3, 2);
    push_sllist(list, 4, 3);
    push_sllist(list, 5, 4);
    ullong_sl_iterator it = sllist_iterator(list);
    ullong_slnode* begin = it.begin(list);
    ullong_slnode* end = it.end(list);
    long long a;
    for (ullong_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    unsigned long long int b[5] = {11, 12, 13, 14, 15};

    size_t j = 0;
    for (ullong_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_float_iterator(void **state) {
    float_sl* list = init_sllist(dFloat)();
    push_sllist(list, 1., 0);
    push_sllist(list, 2., 1);
    push_sllist(list, 3., 2);
    push_sllist(list, 4., 3);
    push_sllist(list, 5., 4);
    float_sl_iterator it = sllist_iterator(list);
    float_slnode* begin = it.begin(list);
    float_slnode* end = it.end(list);
    float a;
    for (float_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    float b[5] = {11., 12., 13., 14., 15.};

    size_t j = 0;
    for (float_slnode* i = begin; i != end; it.next(&i)) {
        assert_float_equal(b[j], it.get(i), 1.0e-3);
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_double_iterator(void **state) {
    double_sl* list = init_sllist(dDouble)();
    push_sllist(list, 1., 0);
    push_sllist(list, 2., 1);
    push_sllist(list, 3., 2);
    push_sllist(list, 4., 3);
    push_sllist(list, 5., 4);
    double_sl_iterator it = sllist_iterator(list);
    double_slnode* begin = it.begin(list);
    double_slnode* end = it.end(list);
    double a;
    for (double_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    double b[5] = {11., 12., 13., 14., 15.};

    size_t j = 0;
    for (double_slnode* i = begin; i != end; it.next(&i)) {
        assert_double_equal(b[j], it.get(i), 1.0e-3);
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_ldouble_iterator(void **state) {
    ldouble_sl* list = init_sllist(dLDouble)();
    push_sllist(list, 1., 0);
    push_sllist(list, 2., 1);
    push_sllist(list, 3., 2);
    push_sllist(list, 4., 3);
    push_sllist(list, 5., 4);
    ldouble_sl_iterator it = sllist_iterator(list);
    ldouble_slnode* begin = it.begin(list);
    ldouble_slnode* end = it.end(list);
    long long a;
    for (ldouble_slnode* i = begin; i != end; it.next(&i)) {
        a = it.get(i);
        i->data = a + 10;
    }

    long double b[5] = {11., 12., 13., 14., 15.};

    size_t j = 0;
    for (ldouble_slnode* i = begin; i != end; it.next(&i)) {
        assert_double_equal(b[j], it.get(i), 1.0e-3);
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_bool_iterator(void **state) {
    bool_sl* list = init_sllist(dBool)();
    push_sllist(list, true, 0);
    push_sllist(list, true, 1);
    push_sllist(list, true, 2);
    push_sllist(list, false, 3);
    push_sllist(list, false, 4);
    bool_sl_iterator it = sllist_iterator(list);
    bool_slnode* begin = it.begin(list);
    bool_slnode* end = it.end(list);

    bool b[5] = {true, true, true, false, false};

    size_t j = 0;
    for (bool_slnode* i = begin; i != end; it.next(&i)) {
        assert_int_equal(b[j], it.get(i));
        j++;
    }
    free_sllist(list);
}
// --------------------------------------------------------------------------------

void test_sllist_string_iterator(void **state) {
    string_sl* list = init_sllist(dString)();
    push_sllist(list, "One", 0);
    push_sllist(list, "Two", 1);
    push_sllist(list, "Three", 2);
    push_sllist(list, "Four", 3);
    push_sllist(list, "Five", 4);
    string_sl_iterator it = sllist_iterator(list);
    string_slnode* begin = it.begin(list);
    string_slnode* end = it.end(list);

    char *b[5] = {"One", "Two", "Three", "Four", "Five"};

    size_t j = 0;
    for (string_slnode* i = begin; i != end; it.next(&i)) {
        assert_string_equal(b[j], it.get(i)->data);
        j++;
    }
    free_sllist(list);
}
// ================================================================================
// ================================================================================
// eof
