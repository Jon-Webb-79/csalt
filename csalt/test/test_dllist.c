// ================================================================================
// ================================================================================
// - File:    test_dllist.c
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

#include "test_dllist.h"
// ================================================================================
// ================================================================================

void test_push_front_char_dllist(void **state) {
	(void) state;
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    char a[5] = {5, 4, 3, 2, 1};
    char_dlnode* current = list->head;
    char_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_uchar_dllist(void **state) {
	(void) state;
    uchar_dl* list = init_dllist(dUChar)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    unsigned char a[5] = {5, 4, 3, 2, 1};
    uchar_dlnode* current = list->head;
    uchar_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_short_dllist(void **state) {
	(void) state;
    short_dl* list = init_dllist(dShort)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    short int a[5] = {5, 4, 3, 2, 1};
    short_dlnode* current = list->head;
    short_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_ushort_dllist(void **state) {
	(void) state;
    ushort_dl* list = init_dllist(dUShort)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    unsigned short int a[5] = {5, 4, 3, 2, 1};
    ushort_dlnode* current = list->head;
    ushort_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_int_dllist(void **state) {
	(void) state;
    int_dl* list = init_dllist(dInt)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    int a[5] = {5, 4, 3, 2, 1};
    int_dlnode* current = list->head;
    int_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_uint_dllist(void **state) {
	(void) state;
    uint_dl* list = init_dllist(dUInt)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    unsigned int a[5] = {5, 4, 3, 2, 1};
    uint_dlnode* current = list->head;
    uint_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_long_dllist(void **state) {
	(void) state;
    long_dl* list = init_dllist(dLong)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    long int a[5] = {5, 4, 3, 2, 1};
    long_dlnode* current = list->head;
    long_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_ulong_dllist(void **state) {
	(void) state;
    ulong_dl* list = init_dllist(dULong)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    unsigned long int a[5] = {5, 4, 3, 2, 1};
    ulong_dlnode* current = list->head;
    ulong_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_llong_dllist(void **state) {
	(void) state;
    llong_dl* list = init_dllist(dLongLong)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    long long int a[5] = {5, 4, 3, 2, 1};
    llong_dlnode* current = list->head;
    llong_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_ullong_dllist(void **state) {
	(void) state;
    ullong_dl* list = init_dllist(dULongLong)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
    assert_int_equal(5, dlist_size(list));
    unsigned long long int a[5] = {5, 4, 3, 2, 1};
    ullong_dlnode* current = list->head;
    ullong_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_float_dllist(void **state) {
	(void) state;
    float_dl* list = init_dllist(dFloat)();
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 5.f);
    assert_int_equal(5, dlist_size(list));
    float a[5] = {5.f, 4.f, 3.f, 2.f, 1.f};
    float_dlnode* current = list->head;
    float_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_float_equal(a[i], current->data, 1.0e-3);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_double_dllist(void **state) {
	(void) state;
    double_dl* list = init_dllist(dDouble)();
    push_front_dllist(list, 1.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 5.);
    assert_int_equal(5, dlist_size(list));
    double a[5] = {5., 4., 3., 2., 1.};
    double_dlnode* current = list->head;
    double_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_double_equal(a[i], current->data, 1.0e-3);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_ldouble_dllist(void **state) {
	(void) state;
    ldouble_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, 1.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 5.);
    assert_int_equal(5, dlist_size(list));
    long double a[5] = {5., 4., 3., 2., 1.};
    ldouble_dlnode* current = list->head;
    ldouble_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_double_equal(a[i], current->data, 1.0e-3);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_bool_dllist(void **state) {
	(void) state;
    bool_dl* list = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, false);
    push_front_dllist(list, false);
    assert_int_equal(5, dlist_size(list));
    bool a[5] = {false, false, false, true, true};
    bool_dlnode* current = list->head;
    bool_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_int_equal(a[i], current->data);
        i++;
        current = next;
    }
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_push_front_string_dllist(void **state) {
	(void) state;
    string_dl* list = init_dllist(dString)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    str *val = init_string_nol("Five");
    push_str_front_dlist(list, val);
    assert_int_equal(5, dlist_size(list));
    char *a[5] = {"Five", "Four", "Three", "Two", "One"};
    string_dlnode* current = list->head;
    string_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_string_equal(a[i], current->data->data);
        i++;
        current = next;
    }
    free_string(val);
    free_dllist(list);
}
// ================================================================================
// ================================================================================
// eof