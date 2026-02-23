// ================================================================================
// ================================================================================
// - File:    test_uint8_array.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================

/**
 * @file test_uint8_array.c
 * @brief Unit tests for uint8_array_t: init, return, and push_back operations.
 *
 * All tests use heap_allocator() as the backing allocator. Tests are grouped
 * and run in order matching the logical lifecycle of the array.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>

#include "c_allocator.h"
#include "c_dtypes.h"
#include "c_uint8.h"

// ================================================================================
// Group 1: init_uint8_array
// ================================================================================

static void test_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);

    assert_true(result.has_value);
    assert_non_null(result.u.value);

    return_uint8_array(result.u.value);
}

// --------------------------------------------------------------------------------

static void test_init_len_is_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);

    assert_true(result.has_value);
    assert_int_equal((int)uint8_array_size(result.u.value), 0);

    return_uint8_array(result.u.value);
}

// --------------------------------------------------------------------------------

static void test_init_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(16, false, alloc);

    assert_true(result.has_value);
    assert_int_equal((int)uint8_array_alloc(result.u.value), 16);

    return_uint8_array(result.u.value);
}

// --------------------------------------------------------------------------------

static void test_init_data_size_is_one(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);

    assert_true(result.has_value);
    assert_int_equal((int)uint8_array_data_size(result.u.value), 1);

    return_uint8_array(result.u.value);
}

// --------------------------------------------------------------------------------

static void test_init_array_is_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);

    assert_true(result.has_value);
    assert_true(is_uint8_array_empty(result.u.value));

    return_uint8_array(result.u.value);
}

// --------------------------------------------------------------------------------

static void test_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(0, false, alloc);

    assert_false(result.has_value);
    assert_int_equal(result.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    alloc.allocate = NULL;

    uint8_array_expect_t result = init_uint8_array(8, false, alloc);

    assert_false(result.has_value);
    assert_int_equal(result.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_init_growth_true_stored_on_struct(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, true, alloc);

    assert_true(result.has_value);
    assert_true(result.u.value->base.growth);

    return_uint8_array(result.u.value);
}

// --------------------------------------------------------------------------------

static void test_init_growth_false_stored_on_struct(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);

    assert_true(result.has_value);
    assert_false(result.u.value->base.growth);

    return_uint8_array(result.u.value);
}

// ================================================================================
// Group 2: return_uint8_array
// ================================================================================

static void test_return_null_is_safe(void** state) {
    (void)state;
    /* Must not crash or assert */
    return_uint8_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);

    assert_true(result.has_value);
    /* Simply returning without crashing is the contract */
    return_uint8_array(result.u.value);
}

// --------------------------------------------------------------------------------

static void test_return_after_pushes_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);

    uint8_array_t* arr = result.u.value;
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);

    return_uint8_array(arr);
}

// ================================================================================
// Group 3: push_back_uint8_array
// ================================================================================

static void test_push_back_increments_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 1);

    assert_int_equal(push_back_uint8_array(arr, 99), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_back_value_is_retrievable(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 123), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 123);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_back_multiple_values_in_order(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    uint8_t values[] = { 10, 20, 30, 40, 50 };
    for (int i = 0; i < 5; i++) {
        assert_int_equal(push_back_uint8_array(arr, values[i]), NO_ERROR);
    }

    uint8_t out = 0;
    for (int i = 0; i < 5; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)values[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_uint8_array(NULL, 42), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_push_back_fills_to_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 1), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 2), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 3), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 4), NO_ERROR);

    assert_true(is_uint8_array_full(arr));
    assert_int_equal((int)uint8_array_size(arr), 4);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_back_beyond_capacity_no_growth_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(2, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 1), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 2), NO_ERROR);
    /* Array is now full and growth is false */
    assert_int_equal(push_back_uint8_array(arr, 3), CAPACITY_OVERFLOW);
    /* len must be unchanged */
    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_back_beyond_capacity_with_growth_succeeds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(2, true, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 1), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 2), NO_ERROR);
    /* Growth kicks in here */
    assert_int_equal(push_back_uint8_array(arr, 3), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 3);
    /* Capacity must have grown */
    assert_true((int)uint8_array_alloc(arr) >= 3);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 2, &out), NO_ERROR);
    assert_int_equal((int)out, 3);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_back_zero_value_stored_correctly(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 0), NO_ERROR);

    uint8_t out = 255;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_back_max_value_stored_correctly(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 255), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 255);

    return_uint8_array(arr);
}

// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_uint8_array[] = {
    /* Group 1: init */
    cmocka_unit_test(test_init_returns_valid_array),
    cmocka_unit_test(test_init_len_is_zero),
    cmocka_unit_test(test_init_alloc_matches_capacity),
    cmocka_unit_test(test_init_data_size_is_one),
    cmocka_unit_test(test_init_array_is_empty),
    cmocka_unit_test(test_init_zero_capacity_fails),
    cmocka_unit_test(test_init_null_allocate_fn_fails),
    cmocka_unit_test(test_init_growth_true_stored_on_struct),
    cmocka_unit_test(test_init_growth_false_stored_on_struct),

    /* Group 2: return */
    cmocka_unit_test(test_return_null_is_safe),
    cmocka_unit_test(test_return_valid_array_does_not_crash),
    cmocka_unit_test(test_return_after_pushes_does_not_crash),

    /* Group 3: push_back */
    cmocka_unit_test(test_push_back_increments_len),
    cmocka_unit_test(test_push_back_value_is_retrievable),
    cmocka_unit_test(test_push_back_multiple_values_in_order),
    cmocka_unit_test(test_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_push_back_fills_to_capacity),
    cmocka_unit_test(test_push_back_beyond_capacity_no_growth_fails),
    cmocka_unit_test(test_push_back_beyond_capacity_with_growth_succeeds),
    cmocka_unit_test(test_push_back_zero_value_stored_correctly),
    cmocka_unit_test(test_push_back_max_value_stored_correctly),
};

const size_t test_uint8_array_count = sizeof(test_uint8_array) / sizeof(test_uint8_array[0]);
// ================================================================================
// ================================================================================
// eof
