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
// eof
