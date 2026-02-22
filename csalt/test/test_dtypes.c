// ================================================================================
// ================================================================================
// - File:    test_dtypes.c
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

/**
 * @file test_dtypes.c
 * @brief Unit tests for c_dtypes.h / c_dtypes.c using the cmocka framework.
 *
 * Build instructions (handled by CMake):
 *   - c_dtypes.c must be compiled with -DDTYPE_TESTING so internal registry
 *     variables are exposed with external linkage for the reset function.
 *   - Link against cmocka.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>
#include "c_dtypes.h"
#include <stdio.h>
// ================================================================================
// User-defined type constants
//
// Each test that registers a user type uses a unique ID so tests do not
// interfere with each other across the shared registry lifetime.
// ================================================================================

#define TEST_VEC3_TYPE    (USER_BASE_TYPE + 1u)
#define TEST_COMPLEX_TYPE (USER_BASE_TYPE + 2u)
#define TEST_MAT4_TYPE    (USER_BASE_TYPE + 3u)
#define TEST_DUP_TYPE     (USER_BASE_TYPE + 4u)
#define TEST_ZERO_TYPE    (USER_BASE_TYPE + 5u)
#define TEST_SLOTS_TYPE   (USER_BASE_TYPE + 6u)

typedef struct { float x; float y; float z; } vec3_t;
typedef struct { double real; double imag;  } complex_t;

// ================================================================================
// Group 1: init_dtype_registry
// ================================================================================

static void test_init_returns_true_on_first_call(void** state) {
    (void)state;
    bool dat = init_dtype_registry();
    assert_true(dat);
}

static void test_init_returns_true_on_repeated_calls(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    assert_true(init_dtype_registry());
    assert_true(init_dtype_registry());
}

static void test_init_registers_all_builtin_types(void** state) {
    (void)state;
    assert_true(init_dtype_registry());

    assert_non_null(lookup_dtype(FLOAT_TYPE));
    assert_non_null(lookup_dtype(DOUBLE_TYPE));
    assert_non_null(lookup_dtype(LDOUBLE_TYPE));
    assert_non_null(lookup_dtype(CHAR_TYPE));
    assert_non_null(lookup_dtype(UCHAR_TYPE));
    assert_non_null(lookup_dtype(INT8_TYPE));
    assert_non_null(lookup_dtype(UINT8_TYPE));
    assert_non_null(lookup_dtype(INT16_TYPE));
    assert_non_null(lookup_dtype(UINT16_TYPE));
    assert_non_null(lookup_dtype(INT32_TYPE));
    assert_non_null(lookup_dtype(UINT32_TYPE));
    assert_non_null(lookup_dtype(INT64_TYPE));
    assert_non_null(lookup_dtype(UINT64_TYPE));
    assert_non_null(lookup_dtype(BOOL_TYPE));
    assert_non_null(lookup_dtype(SIZE_T_TYPE));
}

static void test_init_builtin_sizes_match_sizeof(void** state) {
    (void)state;
    assert_true(init_dtype_registry());

    assert_int_equal((int)sizeof(float),         (int)lookup_dtype(FLOAT_TYPE)->data_size);
    assert_int_equal((int)sizeof(double),        (int)lookup_dtype(DOUBLE_TYPE)->data_size);
    assert_int_equal((int)sizeof(long double),   (int)lookup_dtype(LDOUBLE_TYPE)->data_size);
    assert_int_equal((int)sizeof(char),          (int)lookup_dtype(CHAR_TYPE)->data_size);
    assert_int_equal((int)sizeof(unsigned char), (int)lookup_dtype(UCHAR_TYPE)->data_size);
    assert_int_equal((int)sizeof(int8_t),        (int)lookup_dtype(INT8_TYPE)->data_size);
    assert_int_equal((int)sizeof(uint8_t),       (int)lookup_dtype(UINT8_TYPE)->data_size);
    assert_int_equal((int)sizeof(int16_t),       (int)lookup_dtype(INT16_TYPE)->data_size);
    assert_int_equal((int)sizeof(uint16_t),      (int)lookup_dtype(UINT16_TYPE)->data_size);
    assert_int_equal((int)sizeof(int32_t),       (int)lookup_dtype(INT32_TYPE)->data_size);
    assert_int_equal((int)sizeof(uint32_t),      (int)lookup_dtype(UINT32_TYPE)->data_size);
    assert_int_equal((int)sizeof(int64_t),       (int)lookup_dtype(INT64_TYPE)->data_size);
    assert_int_equal((int)sizeof(uint64_t),      (int)lookup_dtype(UINT64_TYPE)->data_size);
    assert_int_equal((int)sizeof(bool),          (int)lookup_dtype(BOOL_TYPE)->data_size);
    assert_int_equal((int)sizeof(size_t),        (int)lookup_dtype(SIZE_T_TYPE)->data_size);
}

static void test_init_builtin_names_are_not_null(void** state) {
    (void)state;
    assert_true(init_dtype_registry());

    assert_non_null(lookup_dtype(FLOAT_TYPE)->name);
    assert_non_null(lookup_dtype(DOUBLE_TYPE)->name);
    assert_non_null(lookup_dtype(LDOUBLE_TYPE)->name);
    assert_non_null(lookup_dtype(CHAR_TYPE)->name);
    assert_non_null(lookup_dtype(UCHAR_TYPE)->name);
    assert_non_null(lookup_dtype(INT8_TYPE)->name);
    assert_non_null(lookup_dtype(UINT8_TYPE)->name);
    assert_non_null(lookup_dtype(INT16_TYPE)->name);
    assert_non_null(lookup_dtype(UINT16_TYPE)->name);
    assert_non_null(lookup_dtype(INT32_TYPE)->name);
    assert_non_null(lookup_dtype(UINT32_TYPE)->name);
    assert_non_null(lookup_dtype(INT64_TYPE)->name);
    assert_non_null(lookup_dtype(UINT64_TYPE)->name);
    assert_non_null(lookup_dtype(BOOL_TYPE)->name);
    assert_non_null(lookup_dtype(SIZE_T_TYPE)->name);
}

static void test_init_does_not_register_unknown_type(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    assert_null(lookup_dtype(UNKNOWN_TYPE));
}

// ================================================================================
// Group 2: register_dtype
// ================================================================================

static void test_register_valid_user_type_returns_true(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    /* Only register if not already present from a prior run */
    if (lookup_dtype(TEST_VEC3_TYPE) == NULL) {
        static const dtype_t desc = { TEST_VEC3_TYPE, sizeof(vec3_t), "vec3" };
        assert_true(register_dtype(&desc));
    }
    assert_non_null(lookup_dtype(TEST_VEC3_TYPE));
}

static void test_register_null_descriptor_returns_false(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    assert_false(register_dtype(NULL));
}

static void test_register_unknown_type_id_returns_false(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    static const dtype_t desc = { UNKNOWN_TYPE, 4u, "bad" };
    assert_false(register_dtype(&desc));
}

static void test_register_zero_data_size_returns_false(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    static const dtype_t desc = { TEST_ZERO_TYPE, 0u, "bad" };
    assert_false(register_dtype(&desc));
}

static void test_register_duplicate_id_returns_false(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    /* Ensure the type is registered first, then attempt a duplicate */
    if (lookup_dtype(TEST_DUP_TYPE) == NULL) {
        static const dtype_t first = { TEST_DUP_TYPE, 12u, "first" };
        assert_true(register_dtype(&first));
    }
    static const dtype_t second = { TEST_DUP_TYPE, 16u, "second" };
    assert_false(register_dtype(&second));
}

static void test_register_duplicate_builtin_id_returns_false(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    static const dtype_t desc = { FLOAT_TYPE, sizeof(float), "float_again" };
    assert_false(register_dtype(&desc));
}

static void test_register_multiple_distinct_user_types(void** state) {
    (void)state;
    assert_true(init_dtype_registry());

    if (lookup_dtype(TEST_VEC3_TYPE) == NULL) {
        static const dtype_t vec3 = { TEST_VEC3_TYPE, sizeof(vec3_t), "vec3" };
        assert_true(register_dtype(&vec3));
    }
    if (lookup_dtype(TEST_COMPLEX_TYPE) == NULL) {
        static const dtype_t cmplx = { TEST_COMPLEX_TYPE, sizeof(complex_t), "complex" };
        assert_true(register_dtype(&cmplx));
    }
    if (lookup_dtype(TEST_MAT4_TYPE) == NULL) {
        static const dtype_t mat4 = { TEST_MAT4_TYPE, 64u, "mat4" };
        assert_true(register_dtype(&mat4));
    }

    assert_non_null(lookup_dtype(TEST_VEC3_TYPE));
    assert_non_null(lookup_dtype(TEST_COMPLEX_TYPE));
    assert_non_null(lookup_dtype(TEST_MAT4_TYPE));
}

static void test_register_failed_does_not_decrement_slots(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    size_t slots_before = available_dtype_slots();
    static const dtype_t bad = { TEST_ZERO_TYPE, 0u, "bad" };
    assert_false(register_dtype(&bad));
    assert_int_equal((int)slots_before, (int)available_dtype_slots());
}

// ================================================================================
// Group 3: lookup_dtype
// ================================================================================

static void test_lookup_builtin_returns_correct_id(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    const dtype_t* desc = lookup_dtype(FLOAT_TYPE);
    assert_non_null(desc);
    assert_int_equal((int)FLOAT_TYPE, (int)desc->id);
}

static void test_lookup_builtin_returns_correct_size(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    const dtype_t* desc = lookup_dtype(FLOAT_TYPE);
    assert_non_null(desc);
    assert_int_equal((int)sizeof(float), (int)desc->data_size);
}

static void test_lookup_builtin_returns_correct_name(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    const dtype_t* desc = lookup_dtype(FLOAT_TYPE);
    assert_non_null(desc);
    assert_string_equal("float", desc->name);
}

static void test_lookup_unregistered_id_returns_null(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    /* Use an ID well outside the test range that we never register */
    assert_null(lookup_dtype(USER_BASE_TYPE + 999u));
}

static void test_lookup_unknown_type_returns_null(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    assert_null(lookup_dtype(UNKNOWN_TYPE));
}

static void test_lookup_reserved_range_returns_null(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    assert_null(lookup_dtype(16u));
    assert_null(lookup_dtype(500u));
    assert_null(lookup_dtype(999u));
}

static void test_lookup_user_type_after_registration(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    if (lookup_dtype(TEST_VEC3_TYPE) == NULL) {
        static const dtype_t desc = { TEST_VEC3_TYPE, sizeof(vec3_t), "vec3" };
        assert_true(register_dtype(&desc));
    }
    const dtype_t* found = lookup_dtype(TEST_VEC3_TYPE);
    assert_non_null(found);
    assert_int_equal((int)TEST_VEC3_TYPE,  (int)found->id);
    assert_int_equal((int)sizeof(vec3_t),  (int)found->data_size);
    assert_string_equal("vec3", found->name);
}

static void test_lookup_returns_stable_pointer(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    const dtype_t* first  = lookup_dtype(FLOAT_TYPE);
    const dtype_t* second = lookup_dtype(FLOAT_TYPE);
    assert_ptr_equal(first, second);
}

// ================================================================================
// Group 4: ensure_dtype_registered
// ================================================================================

static void test_ensure_null_descriptor_returns_false(void** state) {
    (void)state;
    assert_false(ensure_dtype_registered(NULL));
}

static void test_ensure_new_type_registers_successfully(void** state) {
    (void)state;
    static const dtype_t desc = { TEST_VEC3_TYPE, sizeof(vec3_t), "vec3" };
    assert_true(ensure_dtype_registered(&desc));
    assert_non_null(lookup_dtype(TEST_VEC3_TYPE));
}

static void test_ensure_idempotent_on_repeated_calls(void** state) {
    (void)state;
    static const dtype_t desc = { TEST_VEC3_TYPE, sizeof(vec3_t), "vec3" };
    assert_true(ensure_dtype_registered(&desc));
    assert_true(ensure_dtype_registered(&desc));
    assert_true(ensure_dtype_registered(&desc));
}

static void test_ensure_initializes_registry_implicitly(void** state) {
    (void)state;
    static const dtype_t desc = { TEST_VEC3_TYPE, sizeof(vec3_t), "vec3" };
    assert_true(ensure_dtype_registered(&desc));
    assert_non_null(lookup_dtype(FLOAT_TYPE));
    assert_non_null(lookup_dtype(INT32_TYPE));
}

static void test_ensure_builtin_already_present_returns_true(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    static const dtype_t desc = { FLOAT_TYPE, sizeof(float), "float" };
    assert_true(ensure_dtype_registered(&desc));
}

static void test_ensure_does_not_consume_extra_slot_on_repeat(void** state) {
    (void)state;
    static const dtype_t desc = { TEST_SLOTS_TYPE, sizeof(vec3_t), "slots_test" };
    assert_true(ensure_dtype_registered(&desc));
    size_t slots_after_first = available_dtype_slots();
    assert_true(ensure_dtype_registered(&desc));
    assert_int_equal((int)slots_after_first, (int)available_dtype_slots());
}

// ================================================================================
// Group 5: available_dtype_slots
// ================================================================================

static void test_slots_never_exceed_max(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    assert_true((int)available_dtype_slots() <= (int)MAX_DTYPES);
}

static void test_slots_unchanged_on_failed_registration(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    size_t slots_before = available_dtype_slots();
    static const dtype_t bad = { TEST_ZERO_TYPE, 0u, "bad" };
    assert_false(register_dtype(&bad));
    assert_int_equal((int)slots_before, (int)available_dtype_slots());
}

static void test_slots_decrements_on_successful_registration(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    /* Only meaningful if this type is not yet registered */
    if (lookup_dtype(TEST_SLOTS_TYPE) == NULL) {
        size_t slots_before = available_dtype_slots();
        static const dtype_t desc = { TEST_SLOTS_TYPE, 4u, "slots" };
        assert_true(register_dtype(&desc));
        assert_int_equal((int)(slots_before - 1u), (int)available_dtype_slots());
    }
}

// ================================================================================
// Group 6: ID boundary conditions
// ================================================================================

static void test_user_base_type_itself_is_valid(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    if (lookup_dtype(USER_BASE_TYPE) == NULL) {
        static const dtype_t desc = { USER_BASE_TYPE, 4u, "base" };
        assert_true(register_dtype(&desc));
    }
    assert_non_null(lookup_dtype(USER_BASE_TYPE));
}

static void test_large_user_type_id_is_valid(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    if (lookup_dtype(0xFFFFFFFEu) == NULL) {
        static const dtype_t desc = { 0xFFFFFFFEu, 4u, "max_id" };
        assert_true(register_dtype(&desc));
    }
    assert_non_null(lookup_dtype(0xFFFFFFFEu));
}

static void test_reserved_range_not_auto_registered(void** state) {
    (void)state;
    assert_true(init_dtype_registry());
    assert_null(lookup_dtype(16u));
    assert_null(lookup_dtype(500u));
    assert_null(lookup_dtype(999u));
}

// ================================================================================
// Test table - consumed by unit_test.c via extern declaration in test_suite.h
// ================================================================================

const struct CMUnitTest test_dtypes[] = {
    cmocka_unit_test(test_init_returns_true_on_first_call),
    cmocka_unit_test(test_init_returns_true_on_repeated_calls),
    cmocka_unit_test(test_init_registers_all_builtin_types),
    cmocka_unit_test(test_init_builtin_sizes_match_sizeof),
    cmocka_unit_test(test_init_builtin_names_are_not_null),
    cmocka_unit_test(test_init_does_not_register_unknown_type),

    cmocka_unit_test(test_register_valid_user_type_returns_true),
    cmocka_unit_test(test_register_null_descriptor_returns_false),
    cmocka_unit_test(test_register_unknown_type_id_returns_false),
    cmocka_unit_test(test_register_zero_data_size_returns_false),
    cmocka_unit_test(test_register_duplicate_id_returns_false),
    cmocka_unit_test(test_register_duplicate_builtin_id_returns_false),
    cmocka_unit_test(test_register_multiple_distinct_user_types),
    cmocka_unit_test(test_register_failed_does_not_decrement_slots),

    cmocka_unit_test(test_lookup_builtin_returns_correct_id),
    cmocka_unit_test(test_lookup_builtin_returns_correct_size),
    cmocka_unit_test(test_lookup_builtin_returns_correct_name),
    cmocka_unit_test(test_lookup_unregistered_id_returns_null),
    cmocka_unit_test(test_lookup_unknown_type_returns_null),
    cmocka_unit_test(test_lookup_reserved_range_returns_null),
    cmocka_unit_test(test_lookup_user_type_after_registration),
    cmocka_unit_test(test_lookup_returns_stable_pointer),

    cmocka_unit_test(test_ensure_null_descriptor_returns_false),
    cmocka_unit_test(test_ensure_new_type_registers_successfully),
    cmocka_unit_test(test_ensure_idempotent_on_repeated_calls),
    cmocka_unit_test(test_ensure_initializes_registry_implicitly),
    cmocka_unit_test(test_ensure_builtin_already_present_returns_true),
    cmocka_unit_test(test_ensure_does_not_consume_extra_slot_on_repeat),

    cmocka_unit_test(test_slots_never_exceed_max),
    cmocka_unit_test(test_slots_unchanged_on_failed_registration),
    cmocka_unit_test(test_slots_decrements_on_successful_registration),

    cmocka_unit_test(test_user_base_type_itself_is_valid),
    cmocka_unit_test(test_large_user_type_id_is_valid),
    cmocka_unit_test(test_reserved_range_not_auto_registered),
};

const size_t test_dtypes_count = sizeof(test_dtypes) / sizeof(test_dtypes[0]);
// ================================================================================
// ================================================================================
// eof
