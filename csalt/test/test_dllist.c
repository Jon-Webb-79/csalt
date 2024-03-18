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
    push_str_front_dllist(list, val);
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
// This section of tests should be performed with valgrind 

#ifdef __GNUC__ 

void test_char_dlist_gbc(void **state) {
	(void) state;
    char_dl* list gbc_char_dl = init_dllist(dChar)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_uchar_dlist_gbc(void **state) {
	(void) state;
    uchar_dl* list gbc_uchar_dl = init_dllist(dUChar)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_short_dlist_gbc(void **state) {
	(void) state;
    short_dl* list gbc_short_dl = init_dllist(dShort)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_ushort_dlist_gbc(void **state) {
	(void) state;
    ushort_dl* list gbc_ushort_dl = init_dllist(dUShort)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_int_dlist_gbc(void **state) {
	(void) state;
    int_dl* list gbc_int_dl = init_dllist(dInt)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_uint_dlist_gbc(void **state) {
	(void) state;
    uint_dl* list gbc_uint_dl = init_dllist(dUInt)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_long_dlist_gbc(void **state) {
	(void) state;
    long_dl* list gbc_long_dl = init_dllist(dLong)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_ulong_dlist_gbc(void **state) {
	(void) state;
    ulong_dl* list gbc_ulong_dl = init_dllist(dULong)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_llong_dlist_gbc(void **state) {
	(void) state;
    llong_dl* list gbc_llong_dl = init_dllist(dLongLong)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_ullong_dlist_gbc(void **state) {
	(void) state;
    ullong_dl* list gbc_ullong_dl = init_dllist(dULongLong)();
    push_front_dllist(list, 1);
    push_front_dllist(list, 2);
    push_front_dllist(list, 3);
    push_front_dllist(list, 4);
    push_front_dllist(list, 5);
}
// --------------------------------------------------------------------------------

void test_float_dlist_gbc(void **state) {
	(void) state;
    float_dl* list gbc_float_dl = init_dllist(dFloat)();
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 5.5);
}
// --------------------------------------------------------------------------------

void test_double_dlist_gbc(void **state) {
	(void) state;
    double_dl* list gbc_double_dl = init_dllist(dDouble)();
    push_front_dllist(list, 1.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 5.);
}
// --------------------------------------------------------------------------------

void test_ldouble_dlist_gbc(void **state) {
	(void) state;
    ldouble_dl* list gbc_ldouble_dl = init_dllist(dLDouble)();
    push_front_dllist(list, 1.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 5.);
}
// --------------------------------------------------------------------------------

void test_bool_dlist_gbc(void **state) {
	(void) state;
    bool_dl* list gbc_bool_dl = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
}
// --------------------------------------------------------------------------------

void test_string_dlist_gbc(void **state) {
	(void) state;
    string_dl* list gbc_string_dl = init_dllist(dString)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    push_front_dllist(list, "Five");
}
#endif /* __GNUC__ */
// ================================================================================
// ================================================================================

void test_char_dlist_push_back(void **state) {
    (void) state;
    char_dl* list = init_dllist(dChar)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    char a[5] = {0, 1, 2, 3, 4};
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

void test_uchar_dlist_push_back(void **state) {
    (void) state;
    uchar_dl* list = init_dllist(dUChar)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned char a[5] = {0, 1, 2, 3, 4};
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

void test_short_dlist_push_back(void **state) {
    (void) state;
    short_dl* list = init_dllist(dShort)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    short int a[5] = {0, 1, 2, 3, 4};
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

void test_ushort_dlist_push_back(void **state) {
    (void) state;
    ushort_dl* list = init_dllist(dUShort)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned short int a[5] = {0, 1, 2, 3, 4};
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

void test_int_dlist_push_back(void **state) {
    (void) state;
    int_dl* list = init_dllist(dInt)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    int a[5] = {0, 1, 2, 3, 4};
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

void test_uint_dlist_push_back(void **state) {
    (void) state;
    uint_dl* list = init_dllist(dUInt)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned int a[5] = {0, 1, 2, 3, 4};
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

void test_long_dlist_push_back(void **state) {
    (void) state;
    long_dl* list = init_dllist(dULong)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    long int a[5] = {0, 1, 2, 3, 4};
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

void test_ulong_dlist_push_back(void **state) {
    (void) state;
    ulong_dl* list = init_dllist(dULong)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned long int a[5] = {0, 1, 2, 3, 4};
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

void test_llong_dlist_push_back(void **state) {
    (void) state;
    llong_dl* list = init_dllist(dULongLong)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    long long int a[5] = {0, 1, 2, 3, 4};
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

void test_ullong_dlist_push_back(void **state) {
    (void) state;
    ullong_dl* list = init_dllist(dULongLong)();
    push_back_dllist(list, 0);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned long long int a[5] = {0, 1, 2, 3, 4};
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

void test_float_dlist_push_back(void **state) {
    (void) state;
    float_dl* list = init_dllist(dFloat)();
    push_back_dllist(list, 0.f);
    push_back_dllist(list, 1.f);
    push_back_dllist(list, 2.f);
    push_back_dllist(list, 3.f);
    push_back_dllist(list, 4.f);
    float a[5] = {0.f, 1.f, 2.f, 3.f, 4.f};
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

void test_double_dlist_push_back(void **state) {
    (void) state;
    double_dl* list = init_dllist(dDouble)();
    push_back_dllist(list, 0.);
    push_back_dllist(list, 1.);
    push_back_dllist(list, 2.);
    push_back_dllist(list, 3.);
    push_back_dllist(list, 4.);
    double a[5] = {0., 1., 2., 3., 4.};
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

void test_ldouble_dlist_push_back(void **state) {
    (void) state;
    ldouble_dl* list = init_dllist(dLDouble)();
    push_back_dllist(list, 0.);
    push_back_dllist(list, 1.);
    push_back_dllist(list, 2.);
    push_back_dllist(list, 3.);
    push_back_dllist(list, 4.);
    long double a[5] = {0., 1., 2., 3., 4.};
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

void test_bool_dlist_push_back(void **state) {
    (void) state;
    bool_dl* list = init_dllist(dBool)();
    push_back_dllist(list, true);
    push_back_dllist(list, true);
    push_back_dllist(list, false);
    push_back_dllist(list, false);
    push_back_dllist(list, false);
    bool a[5] = {true, true, false, false, false};
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

void test_string_dlist_push_back(void **state) {
    (void) state;
    string_dl* list = init_dllist(dString)();
    push_back_dllist(list, "One");
    push_back_dllist(list, "Two");
    push_back_dllist(list, "Three");
    push_back_dllist(list, "Four");
    str *val = init_string_nol("Five");
    push_str_back_dllist(list, val);
    char *a[5] = {"One", "Two", "Three", "Four", "Five"};
    string_dlnode* current = list->head;
    string_dlnode* next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_string_equal(a[i], current->data->data);
        i++;
        current = next;
    }
    free_dllist(list);
    free_string(val);
}
// ================================================================================
// ================================================================================

void test_char_insert_dlist(void **state) {
    (void) state;
    char_dl* list = init_dllist(dChar)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    char a[5] = {3, 4, 5, 5, 2};
    char_dlnode *current = list->head;
    char_dlnode *next = NULL;
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

void test_uchar_insert_dlist(void **state) {
    (void) state;
    uchar_dl* list = init_dllist(dUChar)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    unsigned char a[5] = {3, 4, 5, 5, 2};
    uchar_dlnode *current = list->head;
    uchar_dlnode *next = NULL;
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

void test_short_insert_dlist(void **state) {
    (void) state;
    short_dl* list = init_dllist(dShort)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    short int a[5] = {3, 4, 5, 5, 2};
    short_dlnode *current = list->head;
    short_dlnode *next = NULL;
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

void test_ushort_insert_dlist(void **state) {
    (void) state;
    ushort_dl* list = init_dllist(dUShort)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    unsigned short int a[5] = {3, 4, 5, 5, 2};
    ushort_dlnode *current = list->head;
    ushort_dlnode *next = NULL;
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

void test_int_insert_dlist(void **state) {
    (void) state;
    int_dl* list = init_dllist(dInt)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    int a[5] = {3, 4, 5, 5, 2};
    int_dlnode *current = list->head;
    int_dlnode *next = NULL;
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

void test_uint_insert_dlist(void **state) {
    (void) state;
    uint_dl* list = init_dllist(dUInt)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    unsigned int a[5] = {3, 4, 5, 5, 2};
    uint_dlnode *current = list->head;
    uint_dlnode *next = NULL;
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

void test_long_insert_dlist(void **state) {
    (void) state;
    long_dl* list = init_dllist(dLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    long int a[5] = {3, 4, 5, 5, 2};
    long_dlnode *current = list->head;
    long_dlnode *next = NULL;
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

void test_ulong_insert_dlist(void **state) {
    (void) state;
    ulong_dl* list = init_dllist(dULong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    unsigned long a[5] = {3, 4, 5, 5, 2};
    ulong_dlnode *current = list->head;
    ulong_dlnode *next = NULL;
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

void test_llong_insert_dlist(void **state) {
    (void) state;
    llong_dl* list = init_dllist(dLongLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    long long int a[5] = {3, 4, 5, 5, 2};
    llong_dlnode *current = list->head;
    llong_dlnode *next = NULL;
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

void test_ullong_insert_dlist(void **state) {
    (void) state;
    ullong_dl* list = init_dllist(dULongLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    unsigned long long int a[5] = {3, 4, 5, 5, 2};
    ullong_dlnode *current = list->head;
    ullong_dlnode *next = NULL;
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

void test_float_insert_dlist(void **state) {
    (void) state;
    float_dl* list = init_dllist(dFloat)();
    insert_dllist(list, 5.f, 0);
    insert_dllist(list, 2.f, 1);
    insert_dllist(list, 3.f, 0);
    insert_dllist(list, 4.f, 1);
    insert_dllist(list, 5.f, 2);
    float a[5] = {3.f, 4.f, 5.f, 5.f, 2.f};
    float_dlnode *current = list->head;
    float_dlnode *next = NULL;
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

void test_double_insert_dlist(void **state) {
    (void) state;
    double_dl* list = init_dllist(dDouble)();
    insert_dllist(list, 5., 0);
    insert_dllist(list, 2., 1);
    insert_dllist(list, 3., 0);
    insert_dllist(list, 4., 1);
    insert_dllist(list, 5., 2);
    double a[5] = {3., 4., 5., 5., 2.};
    double_dlnode *current = list->head;
    double_dlnode *next = NULL;
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

void test_ldouble_insert_dlist(void **state) {
    (void) state;
    ldouble_dl* list = init_dllist(dLDouble)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    long double a[5] = {3., 4., 5., 5., 2.};
    ldouble_dlnode *current = list->head;
    ldouble_dlnode *next = NULL;
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

void test_bool_insert_dlist(void **state) {
    (void) state;
    bool_dl* list = init_dllist(dBool)();
    insert_dllist(list, true, 0);
    insert_dllist(list, true, 1);
    insert_dllist(list, false, 0);
    insert_dllist(list, false, 1);
    insert_dllist(list, false, 2);
    bool a[5] = {false, false, false, true, true};
    bool_dlnode *current = list->head;
    bool_dlnode *next = NULL;
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

void test_string_insert_dlist(void **state) {
    (void) state;
    string_dl* list = init_dllist(dString)();
    insert_dllist(list, "Five", 0);
    insert_dllist(list, "Two", 1);
    insert_dllist(list, "Three", 0);
    insert_dllist(list, "Four", 1);
    str *val = init_string_nol("Five");
    insert_str_dllist(list, val, 2);
    char *a[5] = {"Three", "Four", "Five", "Five", "Two"};
    string_dlnode *current = list->head;
    string_dlnode *next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_string_equal(a[i], current->data->data);
        i++;
        current = next;
    }
    free_dllist(list);
    free_string(val);
}
// ================================================================================
// ================================================================================

void test_pop_char_front_dlist(void **state) {
    (void) state;
    char_dl* list = init_dllist(dChar)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    char a[5] = {5, 5, 2};
    char_dlnode *current = list->head;
    char_dlnode *next = NULL;
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

void test_pop_uchar_front_dlist(void **state) {
    (void) state;
    uchar_dl* list = init_dllist(dUChar)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    unsigned char a[5] = {5, 5, 2};
    uchar_dlnode *current = list->head;
    uchar_dlnode *next = NULL;
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

void test_pop_short_front_dlist(void **state) {
    (void) state;
    short_dl* list = init_dllist(dShort)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    short int a[5] = {5, 5, 2};
    short_dlnode *current = list->head;
    short_dlnode *next = NULL;
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

void test_pop_ushort_front_dlist(void **state) {
    (void) state;
    ushort_dl* list = init_dllist(dUShort)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    unsigned short int a[5] = {5, 5, 2};
    ushort_dlnode *current = list->head;
    ushort_dlnode *next = NULL;
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

void test_pop_int_front_dlist(void **state) {
    (void) state;
    int_dl* list = init_dllist(dInt)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    int a[5] = {5, 5, 2};
    int_dlnode *current = list->head;
    int_dlnode *next = NULL;
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

void test_pop_uint_front_dlist(void **state) {
    (void) state;
    uint_dl* list = init_dllist(dUInt)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    unsigned int a[5] = {5, 5, 2};
    uint_dlnode *current = list->head;
    uint_dlnode *next = NULL;
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

void test_pop_long_front_dlist(void **state) {
    (void) state;
    long_dl* list = init_dllist(dLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    long int a[5] = {5, 5, 2};
    long_dlnode *current = list->head;
    long_dlnode *next = NULL;
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

void test_pop_ulong_front_dlist(void **state) {
    (void) state;
    ulong_dl* list = init_dllist(dULong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    unsigned long a[5] = {5, 5, 2};
    ulong_dlnode *current = list->head;
    ulong_dlnode *next = NULL;
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

void test_pop_llong_front_dlist(void **state) {
    (void) state;
    llong_dl* list = init_dllist(dLongLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    long long int a[5] = {5, 5, 2};
    llong_dlnode *current = list->head;
    llong_dlnode *next = NULL;
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

void test_pop_ullong_front_dlist(void **state) {
    (void) state;
    ullong_dl* list = init_dllist(dULongLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    unsigned long long int a[5] = {5, 5, 2};
    ullong_dlnode *current = list->head;
    ullong_dlnode *next = NULL;
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

void test_pop_float_front_dlist(void **state) {
    (void) state;
    float_dl* list = init_dllist(dFloat)();
    insert_dllist(list, 5.f, 0);
    insert_dllist(list, 2.f, 1);
    insert_dllist(list, 3.f, 0);
    insert_dllist(list, 4.f, 1);
    insert_dllist(list, 5.f, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    float a[5] = {5.f, 5.f, 2.f};
    float_dlnode *current = list->head;
    float_dlnode *next = NULL;
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

void test_pop_double_front_dlist(void **state) {
    (void) state;
    double_dl* list = init_dllist(dDouble)();
    insert_dllist(list, 5., 0);
    insert_dllist(list, 2., 1);
    insert_dllist(list, 3., 0);
    insert_dllist(list, 4., 1);
    insert_dllist(list, 5., 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    double a[5] = {5., 5., 2.};
    double_dlnode *current = list->head;
    double_dlnode *next = NULL;
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

void test_pop_ldouble_front_dlist(void **state) {
    (void) state;
    ldouble_dl* list = init_dllist(dLDouble)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    long double a[5] = {5., 5., 2.};
    ldouble_dlnode *current = list->head;
    ldouble_dlnode *next = NULL;
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

void test_pop_bool_front_dlist(void **state) {
    (void) state;
    bool_dl* list = init_dllist(dBool)();
    insert_dllist(list, true, 0);
    insert_dllist(list, true, 1);
    insert_dllist(list, false, 0);
    insert_dllist(list, false, 1);
    insert_dllist(list, false, 2);
    pop_front_dllist(list);
    pop_front_dllist(list);
    bool a[5] = {false, true, true};
    bool_dlnode *current = list->head;
    bool_dlnode *next = NULL;
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

void test_pop_string_front_dlist(void **state) {
    (void) state;
    string_dl* list = init_dllist(dString)();
    insert_dllist(list, "Five", 0);
    insert_dllist(list, "Two", 1);
    insert_dllist(list, "Three", 0);
    insert_dllist(list, "Four", 1);
    str *val = init_string_nol("Five");
    insert_str_dllist(list, val, 2);
    str* one = pop_front_dllist(list);
    str* two = pop_front_dllist(list);
    char *a[5] = {"Five", "Five", "Two"};
    string_dlnode *current = list->head;
    string_dlnode *next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_string_equal(a[i], current->data->data);
        i++;
        current = next;
    }
    free_dllist(list);
    free_string(val);
    free_string(one);
    free_string(two);
}
// ================================================================================
// ================================================================================

void test_pop_char_back_dlist(void **state) {
    (void) state;
    char_dl* list = init_dllist(dChar)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    char a[5] = {3, 4, 5};
    char_dlnode *current = list->head;
    char_dlnode *next = NULL;
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

void test_pop_uchar_back_dlist(void **state) {
    (void) state;
    uchar_dl* list = init_dllist(dUChar)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    unsigned char a[5] = {3, 4, 5};
    uchar_dlnode *current = list->head;
    uchar_dlnode *next = NULL;
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

void test_pop_short_back_dlist(void **state) {
    (void) state;
    short_dl* list = init_dllist(dShort)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    short int a[5] = {3, 4, 5};
    short_dlnode *current = list->head;
    short_dlnode *next = NULL;
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

void test_pop_ushort_back_dlist(void **state) {
    (void) state;
    ushort_dl* list = init_dllist(dUShort)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    unsigned short int a[5] = {3, 4, 5};
    ushort_dlnode *current = list->head;
    ushort_dlnode *next = NULL;
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

void test_pop_int_back_dlist(void **state) {
    (void) state;
    int_dl* list = init_dllist(dInt)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    int a[5] = {3, 4, 5};
    int_dlnode *current = list->head;
    int_dlnode *next = NULL;
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

void test_pop_uint_back_dlist(void **state) {
    (void) state;
    uint_dl* list = init_dllist(dUInt)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    unsigned int a[5] = {3, 4, 5};
    uint_dlnode *current = list->head;
    uint_dlnode *next = NULL;
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

void test_pop_long_back_dlist(void **state) {
    (void) state;
    long_dl* list = init_dllist(dLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    long int a[5] = {3, 4, 5};
    long_dlnode *current = list->head;
    long_dlnode *next = NULL;
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

void test_pop_ulong_back_dlist(void **state) {
    (void) state;
    ulong_dl* list = init_dllist(dULong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    unsigned long a[5] = {3, 4, 5};
    ulong_dlnode *current = list->head;
    ulong_dlnode *next = NULL;
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

void test_pop_llong_back_dlist(void **state) {
    (void) state;
    llong_dl* list = init_dllist(dLongLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    long long int a[5] = {3, 4, 5};
    llong_dlnode *current = list->head;
    llong_dlnode *next = NULL;
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

void test_pop_ullong_back_dlist(void **state) {
    (void) state;
    ullong_dl* list = init_dllist(dULongLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    unsigned long long int a[5] = {3, 4, 5};
    ullong_dlnode *current = list->head;
    ullong_dlnode *next = NULL;
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

void test_pop_float_back_dlist(void **state) {
    (void) state;
    float_dl* list = init_dllist(dFloat)();
    insert_dllist(list, 5.f, 0);
    insert_dllist(list, 2.f, 1);
    insert_dllist(list, 3.f, 0);
    insert_dllist(list, 4.f, 1);
    insert_dllist(list, 5.f, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    float a[5] = {3.f, 4.f, 5.f};
    float_dlnode *current = list->head;
    float_dlnode *next = NULL;
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

void test_pop_double_back_dlist(void **state) {
    (void) state;
    double_dl* list = init_dllist(dDouble)();
    insert_dllist(list, 5., 0);
    insert_dllist(list, 2., 1);
    insert_dllist(list, 3., 0);
    insert_dllist(list, 4., 1);
    insert_dllist(list, 5., 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    double a[5] = {3., 4., 5.};
    double_dlnode *current = list->head;
    double_dlnode *next = NULL;
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

void test_pop_ldouble_back_dlist(void **state) {
    (void) state;
    ldouble_dl* list = init_dllist(dLDouble)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    long double a[5] = {3., 4., 5.};
    ldouble_dlnode *current = list->head;
    ldouble_dlnode *next = NULL;
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

void test_pop_bool_back_dlist(void **state) {
    (void) state;
    bool_dl* list = init_dllist(dBool)();
    insert_dllist(list, true, 0);
    insert_dllist(list, true, 1);
    insert_dllist(list, false, 0);
    insert_dllist(list, false, 1);
    insert_dllist(list, false, 2);
    pop_back_dllist(list);
    pop_back_dllist(list);
    bool a[5] = {false, false, false};
    bool_dlnode *current = list->head;
    bool_dlnode *next = NULL;
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

void test_pop_string_back_dlist(void **state) {
    (void) state;
    string_dl* list = init_dllist(dString)();
    insert_dllist(list, "Five", 0);
    insert_dllist(list, "Two", 1);
    insert_dllist(list, "Three", 0);
    insert_dllist(list, "Four", 1);
    str *val = init_string_nol("Five");
    insert_str_dllist(list, val, 2);
    str* one = pop_back_dllist(list);
    str* two = pop_back_dllist(list);
    char *a[5] = {"Three", "Four", "Five"};
    string_dlnode *current = list->head;
    string_dlnode *next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_string_equal(a[i], current->data->data);
        i++;
        current = next;
    }
    free_dllist(list);
    free_string(val);
    free_string(one);
    free_string(two);
}
// ================================================================================
// ================================================================================

void test_pop_char_at_dlist(void **state) {
    (void) state;
    char_dl* list = init_dllist(dChar)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    char a[5] = {4, 5};
    char_dlnode *current = list->head;
    char_dlnode *next = NULL;
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

void test_pop_uchar_at_dlist(void **state) {
    (void) state;
    uchar_dl* list = init_dllist(dUChar)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    unsigned char a[5] = {4, 5};
    uchar_dlnode *current = list->head;
    uchar_dlnode *next = NULL;
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

void test_pop_short_at_dlist(void **state) {
    (void) state;
    short_dl* list = init_dllist(dShort)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    short int a[5] = {4, 5};
    short_dlnode *current = list->head;
    short_dlnode *next = NULL;
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

void test_pop_ushort_at_dlist(void **state) {
    (void) state;
    ushort_dl* list = init_dllist(dUShort)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    unsigned short int a[5] = {4, 5};
    ushort_dlnode *current = list->head;
    ushort_dlnode *next = NULL;
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

void test_pop_int_at_dlist(void **state) {
    (void) state;
    int_dl* list = init_dllist(dInt)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    int a[5] = {4, 5};
    int_dlnode *current = list->head;
    int_dlnode *next = NULL;
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

void test_pop_uint_at_dlist(void **state) {
    (void) state;
    uint_dl* list = init_dllist(dUInt)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    unsigned int a[5] = {4, 5};
    uint_dlnode *current = list->head;
    uint_dlnode *next = NULL;
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

void test_pop_long_at_dlist(void **state) {
    (void) state;
    long_dl* list = init_dllist(dLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    long int a[5] = {4, 5};
    long_dlnode *current = list->head;
    long_dlnode *next = NULL;
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

void test_pop_ulong_at_dlist(void **state) {
    (void) state;
    ulong_dl* list = init_dllist(dULong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    unsigned long a[5] = {4, 5};
    ulong_dlnode *current = list->head;
    ulong_dlnode *next = NULL;
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

void test_pop_llong_at_dlist(void **state) {
    (void) state;
    llong_dl* list = init_dllist(dLongLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    long long int a[5] = {4, 5};
    llong_dlnode *current = list->head;
    llong_dlnode *next = NULL;
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

void test_pop_ullong_at_dlist(void **state) {
    (void) state;
    ullong_dl* list = init_dllist(dULongLong)();
    insert_dllist(list, 5, 0);
    insert_dllist(list, 2, 1);
    insert_dllist(list, 3, 0);
    insert_dllist(list, 4, 1);
    insert_dllist(list, 5, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    unsigned long long int a[5] = {4, 5};
    ullong_dlnode *current = list->head;
    ullong_dlnode *next = NULL;
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

void test_pop_float_at_dlist(void **state) {
    (void) state;
    float_dl* list = init_dllist(dFloat)();
    insert_dllist(list, 5.f, 0);
    insert_dllist(list, 2.f, 1);
    insert_dllist(list, 3.f, 0);
    insert_dllist(list, 4.f, 1);
    insert_dllist(list, 5.f, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    float a[5] = {4.f, 5.f};
    float_dlnode *current = list->head;
    float_dlnode *next = NULL;
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

void test_pop_double_at_dlist(void **state) {
    (void) state;
    double_dl* list = init_dllist(dDouble)();
    insert_dllist(list, 5., 0);
    insert_dllist(list, 2., 1);
    insert_dllist(list, 3., 0);
    insert_dllist(list, 4., 1);
    insert_dllist(list, 5., 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    double a[5] = {4., 5.};
    double_dlnode *current = list->head;
    double_dlnode *next = NULL;
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

void test_pop_ldouble_at_dlist(void **state) {
    (void) state;
    ldouble_dl* list = init_dllist(dLDouble)();
    insert_dllist(list, 5., 0);
    insert_dllist(list, 2., 1);
    insert_dllist(list, 3., 0);
    insert_dllist(list, 4., 1);
    insert_dllist(list, 5., 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    long double a[5] = {4., 5.};
    ldouble_dlnode *current = list->head;
    ldouble_dlnode *next = NULL;
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

void test_pop_bool_at_dlist(void **state) {
    (void) state;
    bool_dl* list = init_dllist(dBool)();
    insert_dllist(list, true, 0);
    insert_dllist(list, true, 1);
    insert_dllist(list, true, 0);
    insert_dllist(list, false, 1);
    insert_dllist(list, true, 2);
    pop_at_dllist(list, 0);
    pop_at_dllist(list, 2);
    pop_at_dllist(list, list->len - 1);
    bool a[5] = {false, true};
    bool_dlnode *current = list->head;
    bool_dlnode *next = NULL;
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

void test_pop_string_at_dlist(void **state) {
    (void) state;
    string_dl* list = init_dllist(dString)();
    insert_dllist(list, "Five", 0);
    insert_dllist(list, "Two", 1);
    insert_dllist(list, "Three", 0);
    insert_dllist(list, "Four", 1);
    insert_dllist(list, "Five", 2);
    str* one = pop_at_dllist(list, 0);
    str *two = pop_at_dllist(list, 2);
    str* three = pop_at_dllist(list, list->len - 1);
    char *a[5] = {"Four", "Five"};
    string_dlnode *current = list->head;
    string_dlnode *next = NULL;
    size_t i = 0;
    while (current != NULL) {
        next = current->next;
        assert_string_equal(a[i], current->data->data);
        i++;
        current = next;
    }
    free_dllist(list);
    free_string(one);
    free_string(two);
    free_string(three);
}
// ================================================================================ 
// ================================================================================

void test_get_char_dllist(void **state) {
    (void) state;
    char_dl* list = init_dllist(dChar)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    char val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_uchar_dllist(void **state) {
    (void) state;
    uchar_dl* list = init_dllist(dUChar)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned char val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_short_dllist(void **state) {
    (void) state;
    short_dl* list = init_dllist(dShort)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    short val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_ushort_dllist(void **state) {
    (void) state;
    ushort_dl* list = init_dllist(dUShort)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned short val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_int_dllist(void **state) {
    (void) state;
    int_dl* list = init_dllist(dInt)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    int val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_uint_dllist(void **state) {
    (void) state;
    uint_dl* list = init_dllist(dUInt)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned int val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_long_dllist(void **state) {
    (void) state;
    long_dl* list = init_dllist(dLong)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    long val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_ulong_dllist(void **state) {
    (void) state;
    ulong_dl* list = init_dllist(dULong)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned long val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_llong_dllist(void **state) {
    (void) state;
    llong_dl* list = init_dllist(dLongLong)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    long long val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_ullong_dllist(void **state) {
    (void) state;
    ullong_dl* list = init_dllist(dULongLong)();
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);
    push_back_dllist(list, 4);
    unsigned long long val = get_dllist(list, 1);
    assert_int_equal(val, 2);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_float_dllist(void **state) {
    (void) state;
    float_dl* list = init_dllist(dFloat)();
    push_back_dllist(list, 1.f);
    push_back_dllist(list, 2.f);
    push_back_dllist(list, 3.f);
    push_back_dllist(list, 4.f);
    float val = get_dllist(list, 1);
    assert_float_equal(val, 2.f, 1.0e-3);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_double_dllist(void **state) {
    (void) state;
    double_dl* list = init_dllist(dDouble)();
    push_back_dllist(list, 1.);
    push_back_dllist(list, 2.);
    push_back_dllist(list, 3.);
    push_back_dllist(list, 4.);
    double val = get_dllist(list, 1);
    assert_double_equal(val, 2., 1.0e-3);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_ldouble_dllist(void **state) {
    (void) state;
    ldouble_dl* list = init_dllist(dLDouble)();
    push_back_dllist(list, 1.);
    push_back_dllist(list, 2.);
    push_back_dllist(list, 3.);
    push_back_dllist(list, 4.);
    long double val = get_dllist(list, 1);
    assert_double_equal(val, 2., 1.0e-3);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_bool_dllist(void **state) {
    (void) state;
    bool_dl* list = init_dllist(dBool)();
    push_back_dllist(list, true);
    push_back_dllist(list,true);
    push_back_dllist(list, false);
    push_back_dllist(list, false);
    bool val = get_dllist(list, 1);
    assert_int_equal(val, true);
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_get_string_dllist(void **state) {
    (void) state;
    string_dl* list = init_dllist(dDouble)();
    push_back_dllist(list, "One");
    push_back_dllist(list, "Two");
    push_back_dllist(list, "Three");
    push_back_dllist(list, "Four");
    str* val = get_dllist(list, 1);
    assert_string_equal(val->data, "Two");
    free_dllist(list);
    free_string(val);
}
// ================================================================================
// ================================================================================

void test_char_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    char_dl* list = init_char_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    char_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    char_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// ================================================================================

void test_uchar_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    uchar_dl* list = init_uchar_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    uchar_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    uchar_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_short_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    short_dl* list = init_short_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    short_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    short_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_ushort_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    ushort_dl* list = init_ushort_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    ushort_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    ushort_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_int_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    int_dl* list = init_int_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    int_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    int_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_uint_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    uint_dl* list = init_uint_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    uint_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    uint_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_long_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    long_dl* list = init_long_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    long_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    long_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_ulong_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    ulong_dl* list = init_ulong_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    ulong_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    ulong_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_llong_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    llong_dl* list = init_llong_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    llong_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    llong_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_ullong_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    ullong_dl* list = init_ullong_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1);
    push_back_dllist(list, 2);
    push_back_dllist(list, 3);

    // Initialize the iterator
    ullong_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    ullong_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 2);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), 1);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), 3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_float_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    float_dl* list = init_float_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1.f);
    push_back_dllist(list, 2.f);
    push_back_dllist(list, 3.f);

    // Initialize the iterator
    float_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    float_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_float_equal(iter.get(node), 1.f, 1.0e-3);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_float_equal(iter.get(node), 2.f, 1.0e-3);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_float_equal(iter.get(node), 1.f, 1.0e-3);

    // Test end function
    node = iter.end(list);
    assert_float_equal(iter.get(node), 3.f, 1.0e-3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_double_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    double_dl* list = init_double_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1.);
    push_back_dllist(list, 2.);
    push_back_dllist(list, 3.);

    // Initialize the iterator
    double_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    double_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_double_equal(iter.get(node), 1., 1.0e-3);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_double_equal(iter.get(node), 2., 1.0e-3);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_double_equal(iter.get(node), 1., 1.0e-3);

    // Test end function
    node = iter.end(list);
    assert_double_equal(iter.get(node), 3., 1.0e-3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_ldouble_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    ldouble_dl* list = init_ldouble_dllist();
    assert_non_null(list);
    push_back_dllist(list, 1.);
    push_back_dllist(list, 2.);
    push_back_dllist(list, 3.);

    // Initialize the iterator
    ldouble_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    ldouble_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_double_equal(iter.get(node), 1., 1.0e-3);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_double_equal(iter.get(node), 2., 1.0e-3);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_double_equal(iter.get(node), 1., 1.0e-3);

    // Test end function
    node = iter.end(list);
    assert_double_equal(iter.get(node), 3., 1.0e-3); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bool_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    bool_dl* list = init_bool_dllist();
    assert_non_null(list);
    push_back_dllist(list, true);
    push_back_dllist(list, true);
    push_back_dllist(list, false);

    // Initialize the iterator
    bool_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    bool_dlnode* node = iter.begin(list);
    assert_non_null(node);
    assert_int_equal(iter.get(node), true);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), true);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    assert_int_equal(iter.get(node), true);

    // Test end function
    node = iter.end(list);
    assert_int_equal(iter.get(node), false); 

    // Cleanup
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_string_dllist_iterator(void **state) {
    (void) state; // Unused parameter

    // Initialize the list and populate it
    string_dl* list = init_string_dllist();
    assert_non_null(list);
    push_back_dllist(list, "One");
    push_back_dllist(list, "Two");
    push_back_dllist(list, "Three");

    // Initialize the iterator
    string_dl_iterator iter = dllist_iterator(list);
    
    // Test begin function
    string_dlnode* node = iter.begin(list);
    assert_non_null(node);
    str* one = iter.get(node);
    assert_string_equal(one->data, "One");
    free_string(one);
    
    // Test next function - move to 'b'
    iter.next(&node);
    assert_non_null(node);
    one = iter.get(node);
    assert_string_equal(one->data, "Two");
    free_string(one);

    // Test prev function - move back to 'a'
    iter.prev(&node);
    assert_non_null(node);
    one = iter.get(node);
    assert_string_equal(one->data, "One");
    free_string(one);

    // Test end function
    node = iter.end(list);
    one = iter.get(node);
    assert_string_equal(one->data, "Three"); 
    free_string(one);

    // Cleanup
    free_dllist(list);
}
// ================================================================================
// ================================================================================

void test_bubble_sort_char_dllist_forward(void **state) {
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    char a[7] = {1, 2, 3, 4, 5, 6, 7};
    char_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uchar_dllist_forward(void **state) {
    uchar_dl* list = init_dllist(dUChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    unsigned char a[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_short_dllist_forward(void **state) {
    short_dl* list = init_dllist(dShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    short a[7] = {1, 2, 3, 4, 5, 6, 7};
    short_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ushort_dllist_forward(void **state) {
    ushort_dl* list = init_dllist(dUShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    unsigned short a[7] = {1, 2, 3, 4, 5, 6, 7};
    ushort_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_int_dllist_forward(void **state) {
    int_dl* list = init_dllist(dInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    int_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uint_dllist_forward(void **state) {
    uint_dl* list = init_dllist(dUInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    unsigned int a[7] = {1, 2, 3, 4, 5, 6, 7};
    uint_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_long_dllist_forward(void **state) {
    long_dl* list = init_dllist(dLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    long_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ulong_dllist_forward(void **state) {
    ulong_dl* list = init_dllist(dULong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    unsigned long a[7] = {1, 2, 3, 4, 5, 6, 7};
    ulong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_llong_dllist_forward(void **state) {
    llong_dl* list = init_dllist(dLongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    llong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ullong_dllist_forward(void **state) {
    ullong_dl* list = init_dllist(dULongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, FORWARD);
    unsigned long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ullong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_float_dllist_forward(void **state) {
    float_dl* list = init_dllist(dFloat)();
    push_front_dllist(list, 5.f);
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 6.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 7.f);
    sort_dllist(list, BUBBLE, FORWARD);
    float a[7] = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f};
    float_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_double_dllist_forward(void **state) {
    double_dl* list = init_dllist(dDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, BUBBLE, FORWARD);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    double_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ldouble_dllist_forward(void **state) {
    ldouble_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, BUBBLE, FORWARD);
    long double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    ldouble_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_bool_dllist_forward(void **state) {
    bool_dl* list = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    sort_dllist(list, BUBBLE, FORWARD);
    bool a[7] = {false, false, false, true, true, true, true};
    bool_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_string_dllist_forward(void **state) {
    string_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    push_front_dllist(list, "Five");
    push_front_dllist(list, "Six");
    push_front_dllist(list, "Seven");
    sort_dllist(list, BUBBLE, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    string_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_dllist(list);
}
// ================================================================================
// ================================================================================

void test_selection_sort_char_dllist_forward(void **state) {
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    char a[7] = {1, 2, 3, 4, 5, 6, 7};
    char_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_uchar_dllist_forward(void **state) {
    uchar_dl* list = init_dllist(dUChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    unsigned char a[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_short_dllist_forward(void **state) {
    short_dl* list = init_dllist(dShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    short a[7] = {1, 2, 3, 4, 5, 6, 7};
    short_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ushort_dllist_forward(void **state) {
    ushort_dl* list = init_dllist(dUShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    unsigned short a[7] = {1, 2, 3, 4, 5, 6, 7};
    ushort_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_int_dllist_forward(void **state) {
    int_dl* list = init_dllist(dInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    int_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_uint_dllist_forward(void **state) {
    uint_dl* list = init_dllist(dUInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    unsigned int a[7] = {1, 2, 3, 4, 5, 6, 7};
    uint_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_long_dllist_forward(void **state) {
    long_dl* list = init_dllist(dLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    long_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ulong_dllist_forward(void **state) {
    ulong_dl* list = init_dllist(dULong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    unsigned long a[7] = {1, 2, 3, 4, 5, 6, 7};
    ulong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_llong_dllist_forward(void **state) {
    llong_dl* list = init_dllist(dLongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    llong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ullong_dllist_forward(void **state) {
    ullong_dl* list = init_dllist(dULongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, FORWARD);
    unsigned long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ullong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_float_dllist_forward(void **state) {
    float_dl* list = init_dllist(dFloat)();
    push_front_dllist(list, 5.f);
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 6.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 7.f);
    sort_dllist(list, SELECTION, FORWARD);
    float a[7] = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f};
    float_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_double_dllist_forward(void **state) {
    double_dl* list = init_dllist(dDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, SELECTION, FORWARD);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    double_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ldouble_dllist_forward(void **state) {
    ldouble_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, SELECTION, FORWARD);
    long double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    ldouble_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_bool_dllist_forward(void **state) {
    bool_dl* list = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    sort_dllist(list, SELECTION, FORWARD);
    bool a[7] = {false, false, false, true, true, true, true};
    bool_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_string_dllist_forward(void **state) {
    string_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    push_front_dllist(list, "Five");
    push_front_dllist(list, "Six");
    push_front_dllist(list, "Seven");
    sort_dllist(list, SELECTION, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    string_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_dllist(list);
}
// ================================================================================
// ================================================================================

void test_insertion_sort_char_dllist_forward(void **state) {
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    char a[7] = {1, 2, 3, 4, 5, 6, 7};
    char_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_uchar_dllist_forward(void **state) {
    uchar_dl* list = init_dllist(dUChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    unsigned char a[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_short_dllist_forward(void **state) {
    short_dl* list = init_dllist(dShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    short a[7] = {1, 2, 3, 4, 5, 6, 7};
    short_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_ushort_dllist_forward(void **state) {
    ushort_dl* list = init_dllist(dUShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    unsigned short a[7] = {1, 2, 3, 4, 5, 6, 7};
    ushort_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_int_dllist_forward(void **state) {
    int_dl* list = init_dllist(dInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    int_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_uint_dllist_forward(void **state) {
    uint_dl* list = init_dllist(dUInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    unsigned int a[7] = {1, 2, 3, 4, 5, 6, 7};
    uint_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_long_dllist_forward(void **state) {
    long_dl* list = init_dllist(dLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    long_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_ulong_dllist_forward(void **state) {
    ulong_dl* list = init_dllist(dULong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    unsigned long a[7] = {1, 2, 3, 4, 5, 6, 7};
    ulong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_llong_dllist_forward(void **state) {
    llong_dl* list = init_dllist(dLongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    llong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_ullong_dllist_forward(void **state) {
    ullong_dl* list = init_dllist(dULongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, FORWARD);
    unsigned long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ullong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_float_dllist_forward(void **state) {
    float_dl* list = init_dllist(dFloat)();
    push_front_dllist(list, 5.f);
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 6.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 7.f);
    sort_dllist(list, INSERT, FORWARD);
    float a[7] = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f};
    float_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_double_dllist_forward(void **state) {
    double_dl* list = init_dllist(dDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, INSERT, FORWARD);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    double_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_ldouble_dllist_forward(void **state) {
    ldouble_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, INSERT, FORWARD);
    long double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    ldouble_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_bool_dllist_forward(void **state) {
    bool_dl* list = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    sort_dllist(list, INSERT, FORWARD);
    bool a[7] = {false, false, false, true, true, true, true};
    bool_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_string_dllist_forward(void **state) {
    string_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    push_front_dllist(list, "Five");
    push_front_dllist(list, "Six");
    push_front_dllist(list, "Seven");
    sort_dllist(list, INSERT, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    string_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_dllist(list);
}
// ================================================================================
// ================================================================================

void test_merge_sort_char_dllist_forward(void **state) {
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    char a[7] = {1, 2, 3, 4, 5, 6, 7};
    char_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uchar_dllist_forward(void **state) {
    uchar_dl* list = init_dllist(dUChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    unsigned char a[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_short_dllist_forward(void **state) {
    short_dl* list = init_dllist(dShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    short a[7] = {1, 2, 3, 4, 5, 6, 7};
    short_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ushort_dllist_forward(void **state) {
    ushort_dl* list = init_dllist(dUShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    unsigned short a[7] = {1, 2, 3, 4, 5, 6, 7};
    ushort_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_int_dllist_forward(void **state) {
    int_dl* list = init_dllist(dInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    int_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uint_dllist_forward(void **state) {
    uint_dl* list = init_dllist(dUInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    unsigned int a[7] = {1, 2, 3, 4, 5, 6, 7};
    uint_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_long_dllist_forward(void **state) {
    long_dl* list = init_dllist(dLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    long_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ulong_dllist_forward(void **state) {
    ulong_dl* list = init_dllist(dULong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    unsigned long a[7] = {1, 2, 3, 4, 5, 6, 7};
    ulong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_llong_dllist_forward(void **state) {
    llong_dl* list = init_dllist(dLongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    llong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ullong_dllist_forward(void **state) {
    ullong_dl* list = init_dllist(dULongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, FORWARD);
    unsigned long long int a[7] = {1, 2, 3, 4, 5, 6, 7};
    ullong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_float_dllist_forward(void **state) {
    float_dl* list = init_dllist(dFloat)();
    push_front_dllist(list, 5.f);
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 6.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 7.f);
    sort_dllist(list, MERGE, FORWARD);
    float a[7] = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f};
    float_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_double_dllist_forward(void **state) {
    double_dl* list = init_dllist(dDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, MERGE, FORWARD);
    double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    double_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ldouble_dllist_forward(void **state) {
    ldouble_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, MERGE, FORWARD);
    long double a[7] = {1., 2., 3., 4., 5., 6., 7.};
    ldouble_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_bool_dllist_forward(void **state) {
    bool_dl* list = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    sort_dllist(list, MERGE, FORWARD);
    bool a[7] = {false, false, false, true, true, true, true};
    bool_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_string_dllist_forward(void **state) {
    string_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    push_front_dllist(list, "Five");
    push_front_dllist(list, "Six");
    push_front_dllist(list, "Seven");
    sort_dllist(list, MERGE, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    string_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_dllist(list);
}
// ================================================================================ 
// ================================================================================

void test_bubble_sort_char_dllist_reverse(void **state) {
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    char a[7] = {7, 6, 5, 4, 3, 2, 1};
    char_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uchar_dllist_reverse(void **state) {
    uchar_dl* list = init_dllist(dUChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    unsigned char a[7] = {7, 6, 5, 4, 3, 2, 1};
    uchar_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_short_dllist_reverse(void **state) {
    short_dl* list = init_dllist(dShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    short a[7] = {7, 6, 5, 4, 3, 2, 1};
    short_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ushort_dllist_reverse(void **state) {
    ushort_dl* list = init_dllist(dUShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    unsigned short a[7] = {7, 6, 5, 4, 3, 2, 1};
    ushort_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_int_dllist_reverse(void **state) {
    int_dl* list = init_dllist(dInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    int a[7] = {7, 6, 5, 4, 3, 2, 1};
    int_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uint_dllist_reverse(void **state) {
    uint_dl* list = init_dllist(dUInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    unsigned int a[7] = {7, 6, 5, 4, 3, 2, 1};
    uint_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_long_dllist_reverse(void **state) {
    long_dl* list = init_dllist(dLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    long_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ulong_dllist_reverse(void **state) {
    ulong_dl* list = init_dllist(dULong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    unsigned long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ulong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_llong_dllist_reverse(void **state) {
    llong_dl* list = init_dllist(dLongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    llong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ullong_dllist_reverse(void **state) {
    ullong_dl* list = init_dllist(dULongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, BUBBLE, REVERSE);
    unsigned long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ullong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_float_dllist_reverse(void **state) {
    float_dl* list = init_dllist(dFloat)();
    push_front_dllist(list, 5.f);
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 6.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 7.f);
    sort_dllist(list, BUBBLE, REVERSE);
    float a[7] = {7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f};
    float_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_double_dllist_reverse(void **state) {
    double_dl* list = init_dllist(dDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, BUBBLE, REVERSE);
    double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    double_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ldouble_dllist_reverse(void **state) {
    ldouble_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, BUBBLE, REVERSE);
    long double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    ldouble_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_bool_dllist_reverse(void **state) {
    bool_dl* list = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    sort_dllist(list, BUBBLE, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    bool_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_string_dllist_reverse(void **state) {
    string_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    push_front_dllist(list, "Five");
    push_front_dllist(list, "Six");
    push_front_dllist(list, "Seven");
    sort_dllist(list, BUBBLE, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    string_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_dllist(list);
}
// ================================================================================
// ================================================================================

void test_selection_sort_char_dllist_reverse(void **state) {
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    char a[7] = {7, 6, 5, 4, 3, 2, 1};
    char_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_uchar_dllist_reverse(void **state) {
    uchar_dl* list = init_dllist(dUChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    unsigned char a[7] = {7, 6, 5, 4, 3, 2, 1};
    uchar_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_short_dllist_reverse(void **state) {
    short_dl* list = init_dllist(dShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    short a[7] = {7, 6, 5, 4, 3, 2, 1};
    short_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ushort_dllist_reverse(void **state) {
    ushort_dl* list = init_dllist(dUShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    unsigned short a[7] = {7, 6, 5, 4, 3, 2, 1};
    ushort_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_int_dllist_reverse(void **state) {
    int_dl* list = init_dllist(dInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    int a[7] = {7, 6, 5, 4, 3, 2, 1};
    int_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_uint_dllist_reverse(void **state) {
    uint_dl* list = init_dllist(dUInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    unsigned int a[7] = {7, 6, 5, 4, 3, 2, 1};
    uint_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_long_dllist_reverse(void **state) {
    long_dl* list = init_dllist(dLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    long_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ulong_dllist_reverse(void **state) {
    ulong_dl* list = init_dllist(dULong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    unsigned long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ulong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_llong_dllist_reverse(void **state) {
    llong_dl* list = init_dllist(dLongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    llong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ullong_dllist_reverse(void **state) {
    ullong_dl* list = init_dllist(dULongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, SELECTION, REVERSE);
    unsigned long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ullong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_float_dllist_reverse(void **state) {
    float_dl* list = init_dllist(dFloat)();
    push_front_dllist(list, 5.f);
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 6.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 7.f);
    sort_dllist(list, SELECTION, REVERSE);
    float a[7] = {7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f};
    float_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_double_dllist_reverse(void **state) {
    double_dl* list = init_dllist(dDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, SELECTION, REVERSE);
    double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    double_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_ldouble_dllist_reverse(void **state) {
    ldouble_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, SELECTION, REVERSE);
    long double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    ldouble_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_bool_dllist_reverse(void **state) {
    bool_dl* list = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    sort_dllist(list, SELECTION, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    bool_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_selection_sort_string_dllist_reverse(void **state) {
    string_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    push_front_dllist(list, "Five");
    push_front_dllist(list, "Six");
    push_front_dllist(list, "Seven");
    sort_dllist(list, SELECTION, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    string_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_dllist(list);
}
// ================================================================================
// ================================================================================

void test_insertion_sort_char_dllist_reverse(void **state) {
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    char a[7] = {7, 6, 5, 4, 3, 2, 1};
    char_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_uchar_dllist_reverse(void **state) {
    uchar_dl* list = init_dllist(dUChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    unsigned char a[7] = {7, 6, 5, 4, 3, 2, 1};
    uchar_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_short_dllist_reverse(void **state) {
    short_dl* list = init_dllist(dShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    short a[7] = {7, 6, 5, 4, 3, 2, 1};
    short_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_ushort_dllist_reverse(void **state) {
    ushort_dl* list = init_dllist(dUShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    unsigned short a[7] = {7, 6, 5, 4, 3, 2, 1};
    ushort_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_int_dllist_reverse(void **state) {
    int_dl* list = init_dllist(dInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    int a[7] = {7, 6, 5, 4, 3, 2, 1};
    int_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_uint_dllist_reverse(void **state) {
    uint_dl* list = init_dllist(dUInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    unsigned int a[7] = {7, 6, 5, 4, 3, 2, 1};
    uint_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_long_dllist_reverse(void **state) {
    long_dl* list = init_dllist(dLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    long_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_ulong_dllist_reverse(void **state) {
    ulong_dl* list = init_dllist(dULong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    unsigned long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ulong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_llong_dllist_reverse(void **state) {
    llong_dl* list = init_dllist(dLongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    llong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_ullong_dllist_reverse(void **state) {
    ullong_dl* list = init_dllist(dULongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, INSERT, REVERSE);
    unsigned long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ullong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_float_dllist_reverse(void **state) {
    float_dl* list = init_dllist(dFloat)();
    push_front_dllist(list, 5.f);
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 6.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 7.f);
    sort_dllist(list, INSERT, REVERSE);
    float a[7] = {7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f};
    float_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_double_dllist_reverse(void **state) {
    double_dl* list = init_dllist(dDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, INSERT, REVERSE);
    double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    double_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_ldouble_dllist_reverse(void **state) {
    ldouble_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, INSERT, REVERSE);
    long double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    ldouble_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_bool_dllist_reverse(void **state) {
    bool_dl* list = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    sort_dllist(list, INSERT, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    bool_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_insertion_sort_string_dllist_reverse(void **state) {
    string_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    push_front_dllist(list, "Five");
    push_front_dllist(list, "Six");
    push_front_dllist(list, "Seven");
    sort_dllist(list, INSERT, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    string_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_dllist(list);
}
// ================================================================================
// ================================================================================

void test_merge_sort_char_dllist_reverse(void **state) {
    char_dl* list = init_dllist(dChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    char a[7] = {7, 6, 5, 4, 3, 2, 1};
    char_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uchar_dllist_reverse(void **state) {
    uchar_dl* list = init_dllist(dUChar)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    unsigned char a[7] = {7, 6, 5, 4, 3, 2, 1};
    uchar_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_short_dllist_reverse(void **state) {
    short_dl* list = init_dllist(dShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    short a[7] = {7, 6, 5, 4, 3, 2, 1};
    short_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ushort_dllist_reverse(void **state) {
    ushort_dl* list = init_dllist(dUShort)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    unsigned short a[7] = {7, 6, 5, 4, 3, 2, 1};
    ushort_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_int_dllist_reverse(void **state) {
    int_dl* list = init_dllist(dInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    int a[7] = {7, 6, 5, 4, 3, 2, 1};
    int_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uint_dllist_reverse(void **state) {
    uint_dl* list = init_dllist(dUInt)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    unsigned int a[7] = {7, 6, 5, 4, 3, 2, 1};
    uint_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_long_dllist_reverse(void **state) {
    long_dl* list = init_dllist(dLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    long_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ulong_dllist_reverse(void **state) {
    ulong_dl* list = init_dllist(dULong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    unsigned long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ulong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_llong_dllist_reverse(void **state) {
    llong_dl* list = init_dllist(dLongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    llong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ullong_dllist_reverse(void **state) {
    ullong_dl* list = init_dllist(dULongLong)();
    push_front_dllist(list, 5);
    push_front_dllist(list, 1);
    push_front_dllist(list, 6);
    push_front_dllist(list, 3);
    push_front_dllist(list, 2);
    push_front_dllist(list, 4);
    push_front_dllist(list, 7);
    sort_dllist(list, MERGE, REVERSE);
    unsigned long long int a[7] = {7, 6, 5, 4, 3, 2, 1};
    ullong_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_float_dllist_reverse(void **state) {
    float_dl* list = init_dllist(dFloat)();
    push_front_dllist(list, 5.f);
    push_front_dllist(list, 1.f);
    push_front_dllist(list, 6.f);
    push_front_dllist(list, 3.f);
    push_front_dllist(list, 2.f);
    push_front_dllist(list, 4.f);
    push_front_dllist(list, 7.f);
    sort_dllist(list, MERGE, REVERSE);
    float a[7] = {7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f};
    float_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_float_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_double_dllist_reverse(void **state) {
    double_dl* list = init_dllist(dDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, MERGE, REVERSE);
    double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    double_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ldouble_dllist_reverse(void **state) {
    ldouble_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, 5.);
    push_front_dllist(list, 1.);
    push_front_dllist(list, 6.);
    push_front_dllist(list, 3.);
    push_front_dllist(list, 2.);
    push_front_dllist(list, 4.);
    push_front_dllist(list, 7.);
    sort_dllist(list, MERGE, REVERSE);
    long double a[7] = {7., 6., 5., 4., 3., 2., 1.};
    ldouble_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_double_equal(a[i], result->data, 1.0e-3);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_bool_dllist_reverse(void **state) {
    bool_dl* list = init_dllist(dBool)();
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    push_front_dllist(list, false);
    push_front_dllist(list, true);
    sort_dllist(list, MERGE, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    bool_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_int_equal(a[i], result->data);
        result = result->next;
        i++;
    } 
    free_dllist(list);
}
// --------------------------------------------------------------------------------

void test_merge_sort_string_dllist_reverse(void **state) {
    string_dl* list = init_dllist(dLDouble)();
    push_front_dllist(list, "One");
    push_front_dllist(list, "Two");
    push_front_dllist(list, "Three");
    push_front_dllist(list, "Four");
    push_front_dllist(list, "Five");
    push_front_dllist(list, "Six");
    push_front_dllist(list, "Seven");
    sort_dllist(list, MERGE, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    string_dlnode* result = list->head;
    size_t i = 0;
    while (result != NULL) {
        assert_string_equal(a[i], result->data->data);
        result = result->next;
        i++;
    }
    free_dllist(list);
}
// ================================================================================
// ================================================================================
// eof
