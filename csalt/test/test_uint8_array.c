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
// ================================================================================ 

// ================================================================================
// Group 4: push_front_uint8_array
// ================================================================================

static void test_push_front_increments_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_front_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 1);

    assert_int_equal(push_front_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_front_value_is_at_index_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_front_uint8_array(arr, 42), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 42);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_front_shifts_existing_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    /* Build [10, 20, 30] then prepend 5 -> [5, 10, 20, 30] */
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_front_uint8_array(arr, 5),  NO_ERROR);

    uint8_t expected[] = { 5, 10, 20, 30 };
    uint8_t out = 0;
    for (int i = 0; i < 4; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_front_multiple_reverses_insertion_order(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    /* Prepend 1, 2, 3 in order -> array should be [3, 2, 1] */
    assert_int_equal(push_front_uint8_array(arr, 1), NO_ERROR);
    assert_int_equal(push_front_uint8_array(arr, 2), NO_ERROR);
    assert_int_equal(push_front_uint8_array(arr, 3), NO_ERROR);

    uint8_t expected[] = { 3, 2, 1 };
    uint8_t out = 0;
    for (int i = 0; i < 3; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_uint8_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_push_front_beyond_capacity_no_growth_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(2, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_front_uint8_array(arr, 1), NO_ERROR);
    assert_int_equal(push_front_uint8_array(arr, 2), NO_ERROR);
    assert_int_equal(push_front_uint8_array(arr, 3), CAPACITY_OVERFLOW);
    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_front_beyond_capacity_with_growth_succeeds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(2, true, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_front_uint8_array(arr, 1), NO_ERROR);
    assert_int_equal(push_front_uint8_array(arr, 2), NO_ERROR);
    assert_int_equal(push_front_uint8_array(arr, 3), NO_ERROR);

    assert_int_equal((int)uint8_array_size(arr), 3);

    /* Verify order: [3, 2, 1] */
    uint8_t expected[] = { 3, 2, 1 };
    uint8_t out = 0;
    for (int i = 0; i < 3; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// ================================================================================
// Group 5: push_at_uint8_array
// ================================================================================

static void test_push_at_increments_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_at_uint8_array(arr, 1, 20), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 3);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_at_middle_shifts_correctly(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    /* Build [10, 30] then insert 20 at index 1 -> [10, 20, 30] */
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_at_uint8_array(arr, 1, 20), NO_ERROR);

    uint8_t expected[] = { 10, 20, 30 };
    uint8_t out = 0;
    for (int i = 0; i < 3; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_at_index_zero_behaves_like_push_front(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_at_uint8_array(arr, 0, 10), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_at_index_len_behaves_like_push_back(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    /* Inserting at index == len appends */
    assert_int_equal(push_at_uint8_array(arr, 2, 30), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 2, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_uint8_array(NULL, 0, 42), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_push_at_out_of_bounds_index_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    /* len is 1, so index 2 is out of bounds */
    assert_int_equal(push_at_uint8_array(arr, 2, 99), OUT_OF_BOUNDS);
    assert_int_equal((int)uint8_array_size(arr), 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_at_beyond_capacity_no_growth_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(2, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    /* Full, no growth */
    assert_int_equal(push_at_uint8_array(arr, 1, 20), CAPACITY_OVERFLOW);
    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_at_beyond_capacity_with_growth_succeeds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(2, true, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    /* Full but growth allowed */
    assert_int_equal(push_at_uint8_array(arr, 1, 20), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 3);

    uint8_t expected[] = { 10, 20, 30 };
    uint8_t out = 0;
    for (int i = 0; i < 3; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_push_at_empty_array_at_index_zero_succeeds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    /* Inserting into empty array at index 0 is valid */
    assert_int_equal(push_at_uint8_array(arr, 0, 77), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 1);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 77);

    return_uint8_array(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Group 6: get_uint8_array_index
// ================================================================================

static void test_get_returns_correct_value_at_index_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 77), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 77);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_get_returns_correct_value_at_last_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 2, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_get_returns_correct_value_at_middle_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 55), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 1, &out), NO_ERROR);
    assert_int_equal((int)out, 55);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_get_does_not_modify_array_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 43), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal(get_uint8_array_index(arr, 1, &out), NO_ERROR);

    /* len must remain unchanged after reads */
    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_get_all_values_in_sequence(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    uint8_t values[] = { 1, 2, 3, 4, 5, 6 };
    for (int i = 0; i < 6; i++) {
        assert_int_equal(push_back_uint8_array(arr, values[i]), NO_ERROR);
    }

    uint8_t out = 0;
    for (int i = 0; i < 6; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)values[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(NULL, 0, &out), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(get_uint8_array_index(arr, 0, NULL), NULL_POINTER);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_get_out_of_bounds_index_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);

    uint8_t out = 0;
    /* len is 1, index 1 is out of bounds */
    assert_int_equal(get_uint8_array_index(arr, 1, &out), OUT_OF_BOUNDS);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_get_on_empty_array_returns_out_of_bounds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), OUT_OF_BOUNDS);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_get_zero_value_retrieved_correctly(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 0), NO_ERROR);

    uint8_t out = 255;  /* pre-poison to verify overwrite */
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_get_max_value_retrieved_correctly(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 255), NO_ERROR);

    uint8_t out = 0;  /* pre-poison to verify overwrite */
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 255);

    return_uint8_array(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Group 7: pop_back_uint8_array
// ================================================================================

static void test_pop_back_decrements_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(pop_back_uint8_array(arr, NULL), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_back_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 99), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(pop_back_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal((int)out, 99);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_back_null_out_discards_silently(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);
    assert_int_equal(pop_back_uint8_array(arr, NULL), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_back_leaves_remaining_elements_intact(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(pop_back_uint8_array(arr, NULL), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 10);
    assert_int_equal(get_uint8_array_index(arr, 1, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_back_until_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 1), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 2), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 3), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(pop_back_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal((int)out, 3);
    assert_int_equal(pop_back_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal((int)out, 2);
    assert_int_equal(pop_back_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_true(is_uint8_array_empty(arr));

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_back_on_empty_returns_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(pop_back_uint8_array(arr, NULL), EMPTY);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_uint8_array(NULL, NULL), NULL_POINTER);
}

// ================================================================================
// Group 8: pop_front_uint8_array
// ================================================================================

static void test_pop_front_decrements_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(pop_front_uint8_array(arr, NULL), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_front_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 55), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 66), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(pop_front_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal((int)out, 55);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_front_null_out_discards_silently(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 77), NO_ERROR);
    assert_int_equal(pop_front_uint8_array(arr, NULL), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_front_shifts_remaining_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(pop_front_uint8_array(arr, NULL), NO_ERROR);

    /* Remaining should be [20, 30] */
    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 20);
    assert_int_equal(get_uint8_array_index(arr, 1, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_front_until_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 1), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 2), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 3), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(pop_front_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal((int)out, 1);
    assert_int_equal(pop_front_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal((int)out, 2);
    assert_int_equal(pop_front_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal((int)out, 3);

    assert_true(is_uint8_array_empty(arr));

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_front_on_empty_returns_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(pop_front_uint8_array(arr, NULL), EMPTY);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_uint8_array(NULL, NULL), NULL_POINTER);
}

// ================================================================================
// Group 9: pop_any_uint8_array
// ================================================================================

static void test_pop_any_decrements_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(pop_any_uint8_array(arr, NULL, 1), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_any_middle_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 55), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(pop_any_uint8_array(arr, &out, 1), NO_ERROR);
    assert_int_equal((int)out, 55);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_any_middle_shifts_remaining_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    /* [10, 20, 30, 40] -> pop index 1 -> [10, 30, 40] */
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 40), NO_ERROR);
    assert_int_equal(pop_any_uint8_array(arr, NULL, 1), NO_ERROR);

    uint8_t expected[] = { 10, 30, 40 };
    uint8_t out = 0;
    for (int i = 0; i < 3; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_any_index_zero_behaves_like_pop_front(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(pop_any_uint8_array(arr, &out, 0), NO_ERROR);
    assert_int_equal((int)out, 10);

    /* Remaining: [20, 30] */
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_any_last_index_behaves_like_pop_back(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(pop_any_uint8_array(arr, &out, 2), NO_ERROR);
    assert_int_equal((int)out, 30);
    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_any_null_out_discards_silently(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(pop_any_uint8_array(arr, NULL, 0), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_uint8_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_pop_any_on_empty_returns_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(pop_any_uint8_array(arr, NULL, 0), EMPTY);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_pop_any_out_of_bounds_index_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);

    /* len is 2, index 2 is out of bounds */
    assert_int_equal(pop_any_uint8_array(arr, NULL, 2), OUT_OF_BOUNDS);
    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Group 10: clear_uint8_array
// ================================================================================

static void test_clear_sets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(clear_uint8_array(arr), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_clear_preserves_alloc_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(clear_uint8_array(arr), NO_ERROR);

    /* Buffer should still be there at full capacity */
    assert_int_equal((int)uint8_array_alloc(arr), 8);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_clear_array_is_empty_after_clear(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);
    assert_int_equal(clear_uint8_array(arr), NO_ERROR);
    assert_true(is_uint8_array_empty(arr));

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_clear_allows_reuse_after_clear(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(clear_uint8_array(arr), NO_ERROR);

    /* Array should be fully usable again from scratch */
    assert_int_equal(push_back_uint8_array(arr, 99), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 1);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 99);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_clear_on_empty_array_succeeds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    /* Clearing an already empty array must succeed without error */
    assert_int_equal(clear_uint8_array(arr), NO_ERROR);
    assert_int_equal((int)uint8_array_size(arr), 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_uint8_array(NULL), NULL_POINTER);
}

// ================================================================================
// Group 11: set_uint8_array_index
// ================================================================================

static void test_set_overwrites_value_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(set_uint8_array_index(arr, 0, 99), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 99);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_set_does_not_change_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(set_uint8_array_index(arr, 0, 55), NO_ERROR);

    assert_int_equal((int)uint8_array_size(arr), 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_set_does_not_affect_other_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(set_uint8_array_index(arr, 1, 99), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 10);
    assert_int_equal(get_uint8_array_index(arr, 2, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_set_at_last_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(set_uint8_array_index(arr, 2, 77), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 2, &out), NO_ERROR);
    assert_int_equal((int)out, 77);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_set_out_of_bounds_index_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    /* len is 1, index 1 is out of bounds */
    assert_int_equal(set_uint8_array_index(arr, 1, 99), OUT_OF_BOUNDS);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_uint8_array_index(NULL, 0, 42), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_set_zero_value_stored_correctly(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 255), NO_ERROR);
    assert_int_equal(set_uint8_array_index(arr, 0, 0), NO_ERROR);

    uint8_t out = 255;  /* pre-poison */
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_set_max_value_stored_correctly(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* arr = result.u.value;

    assert_int_equal(push_back_uint8_array(arr, 0), NO_ERROR);
    assert_int_equal(set_uint8_array_index(arr, 0, 255), NO_ERROR);

    uint8_t out = 0;  /* pre-poison */
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 255);

    return_uint8_array(arr);
}

// ================================================================================
// Group 12: copy_uint8_array
// ================================================================================

static void test_copy_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* src = result.u.value;

    assert_int_equal(push_back_uint8_array(src, 10), NO_ERROR);

    uint8_array_expect_t copy_result = copy_uint8_array(src, alloc);
    assert_true(copy_result.has_value);
    assert_non_null(copy_result.u.value);

    return_uint8_array(src);
    return_uint8_array(copy_result.u.value);
}

// --------------------------------------------------------------------------------

static void test_copy_has_same_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* src = result.u.value;

    assert_int_equal(push_back_uint8_array(src, 1), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 2), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 3), NO_ERROR);

    uint8_array_expect_t copy_result = copy_uint8_array(src, alloc);
    assert_true(copy_result.has_value);
    assert_int_equal((int)uint8_array_size(copy_result.u.value),
                     (int)uint8_array_size(src));

    return_uint8_array(src);
    return_uint8_array(copy_result.u.value);
}

// --------------------------------------------------------------------------------

static void test_copy_contains_same_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* src = result.u.value;

    uint8_t values[] = { 10, 20, 30, 40, 50 };
    for (int i = 0; i < 5; i++) {
        assert_int_equal(push_back_uint8_array(src, values[i]), NO_ERROR);
    }

    uint8_array_expect_t copy_result = copy_uint8_array(src, alloc);
    assert_true(copy_result.has_value);
    uint8_array_t* dst = copy_result.u.value;

    uint8_t out = 0;
    for (int i = 0; i < 5; i++) {
        assert_int_equal(get_uint8_array_index(dst, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)values[i]);
    }

    return_uint8_array(src);
    return_uint8_array(dst);
}

// --------------------------------------------------------------------------------

static void test_copy_is_independent_from_src(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* src = result.u.value;

    assert_int_equal(push_back_uint8_array(src, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 20), NO_ERROR);

    uint8_array_expect_t copy_result = copy_uint8_array(src, alloc);
    assert_true(copy_result.has_value);
    uint8_array_t* dst = copy_result.u.value;

    /* Modify src — dst must be unaffected */
    assert_int_equal(set_uint8_array_index(src, 0, 99), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(dst, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    return_uint8_array(src);
    return_uint8_array(dst);
}

// --------------------------------------------------------------------------------

static void test_copy_is_independent_when_dst_modified(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(8, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* src = result.u.value;

    assert_int_equal(push_back_uint8_array(src, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 20), NO_ERROR);

    uint8_array_expect_t copy_result = copy_uint8_array(src, alloc);
    assert_true(copy_result.has_value);
    uint8_array_t* dst = copy_result.u.value;

    /* Modify dst — src must be unaffected */
    assert_int_equal(set_uint8_array_index(dst, 0, 77), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(src, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    return_uint8_array(src);
    return_uint8_array(dst);
}

// --------------------------------------------------------------------------------

static void test_copy_of_empty_array_succeeds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t result = init_uint8_array(4, false, alloc);
    assert_true(result.has_value);
    uint8_array_t* src = result.u.value;

    uint8_array_expect_t copy_result = copy_uint8_array(src, alloc);
    assert_true(copy_result.has_value);
    assert_true(is_uint8_array_empty(copy_result.u.value));

    return_uint8_array(src);
    return_uint8_array(copy_result.u.value);
}

// --------------------------------------------------------------------------------

static void test_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t copy_result = copy_uint8_array(NULL, alloc);
    assert_false(copy_result.has_value);
    assert_int_equal(copy_result.u.error, NULL_POINTER);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Group 13: concat_uint8_array
// ================================================================================

static void test_concat_appends_all_src_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();

    uint8_array_expect_t r1 = init_uint8_array(8, false, alloc);
    uint8_array_expect_t r2 = init_uint8_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    uint8_array_t* dst = r1.u.value;
    uint8_array_t* src = r2.u.value;

    assert_int_equal(push_back_uint8_array(dst, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(dst, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 40), NO_ERROR);

    assert_int_equal(concat_uint8_array(dst, src), NO_ERROR);

    uint8_t expected[] = { 10, 20, 30, 40 };
    uint8_t out = 0;
    for (int i = 0; i < 4; i++) {
        assert_int_equal(get_uint8_array_index(dst, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(dst);
    return_uint8_array(src);
}

// --------------------------------------------------------------------------------

static void test_concat_increases_dst_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();

    uint8_array_expect_t r1 = init_uint8_array(8, false, alloc);
    uint8_array_expect_t r2 = init_uint8_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    uint8_array_t* dst = r1.u.value;
    uint8_array_t* src = r2.u.value;

    assert_int_equal(push_back_uint8_array(dst, 1), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 2), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 3), NO_ERROR);

    assert_int_equal(concat_uint8_array(dst, src), NO_ERROR);
    assert_int_equal((int)uint8_array_size(dst), 3);

    return_uint8_array(dst);
    return_uint8_array(src);
}

// --------------------------------------------------------------------------------

static void test_concat_src_is_unmodified(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();

    uint8_array_expect_t r1 = init_uint8_array(8, false, alloc);
    uint8_array_expect_t r2 = init_uint8_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    uint8_array_t* dst = r1.u.value;
    uint8_array_t* src = r2.u.value;

    assert_int_equal(push_back_uint8_array(src, 55), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 66), NO_ERROR);
    assert_int_equal(concat_uint8_array(dst, src), NO_ERROR);

    assert_int_equal((int)uint8_array_size(src), 2);
    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(src, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 55);

    return_uint8_array(dst);
    return_uint8_array(src);
}

// --------------------------------------------------------------------------------

static void test_concat_empty_src_leaves_dst_unchanged(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();

    uint8_array_expect_t r1 = init_uint8_array(4, false, alloc);
    uint8_array_expect_t r2 = init_uint8_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    uint8_array_t* dst = r1.u.value;
    uint8_array_t* src = r2.u.value;

    assert_int_equal(push_back_uint8_array(dst, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(dst, 20), NO_ERROR);

    assert_int_equal(concat_uint8_array(dst, src), NO_ERROR);
    assert_int_equal((int)uint8_array_size(dst), 2);

    return_uint8_array(dst);
    return_uint8_array(src);
}

// --------------------------------------------------------------------------------

static void test_concat_with_growth_succeeds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();

    /* dst capacity 2, growth true — src has 3 elements, must trigger growth */
    uint8_array_expect_t r1 = init_uint8_array(2, true, alloc);
    uint8_array_expect_t r2 = init_uint8_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    uint8_array_t* dst = r1.u.value;
    uint8_array_t* src = r2.u.value;

    assert_int_equal(push_back_uint8_array(src, 1), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 2), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 3), NO_ERROR);

    assert_int_equal(concat_uint8_array(dst, src), NO_ERROR);
    assert_int_equal((int)uint8_array_size(dst), 3);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(dst, 2, &out), NO_ERROR);
    assert_int_equal((int)out, 3);

    return_uint8_array(dst);
    return_uint8_array(src);
}

// --------------------------------------------------------------------------------

static void test_concat_without_growth_fails_when_full(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();

    uint8_array_expect_t r1 = init_uint8_array(2, false, alloc);
    uint8_array_expect_t r2 = init_uint8_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    uint8_array_t* dst = r1.u.value;
    uint8_array_t* src = r2.u.value;

    assert_int_equal(push_back_uint8_array(dst, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 40), NO_ERROR);

    assert_int_equal(concat_uint8_array(dst, src), CAPACITY_OVERFLOW);
    /* dst must be unmodified */
    assert_int_equal((int)uint8_array_size(dst), 1);

    return_uint8_array(dst);
    return_uint8_array(src);
}

// --------------------------------------------------------------------------------

static void test_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);

    assert_int_equal(concat_uint8_array(NULL, r.u.value), NULL_POINTER);

    return_uint8_array(r.u.value);
}

// --------------------------------------------------------------------------------

static void test_concat_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);

    assert_int_equal(concat_uint8_array(r.u.value, NULL), NULL_POINTER);

    return_uint8_array(r.u.value);
}

// ================================================================================
// Group 14: slice_uint8_array
// ================================================================================

static void test_slice_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* src = r.u.value;

    for (uint8_t i = 1; i <= 5; i++) {
        assert_int_equal(push_back_uint8_array(src, i), NO_ERROR);
    }

    uint8_array_expect_t slice = slice_uint8_array(src, 1, 4, alloc);
    assert_true(slice.has_value);
    assert_non_null(slice.u.value);

    return_uint8_array(src);
    return_uint8_array(slice.u.value);
}

// --------------------------------------------------------------------------------

static void test_slice_has_correct_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* src = r.u.value;

    for (uint8_t i = 1; i <= 5; i++) {
        assert_int_equal(push_back_uint8_array(src, i), NO_ERROR);
    }

    /* slice [1, 4) -> 3 elements */
    uint8_array_expect_t slice = slice_uint8_array(src, 1, 4, alloc);
    assert_true(slice.has_value);
    assert_int_equal((int)uint8_array_size(slice.u.value), 3);

    return_uint8_array(src);
    return_uint8_array(slice.u.value);
}

// --------------------------------------------------------------------------------

static void test_slice_contains_correct_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* src = r.u.value;

    /* src = [10, 20, 30, 40, 50] */
    uint8_t values[] = { 10, 20, 30, 40, 50 };
    for (int i = 0; i < 5; i++) {
        assert_int_equal(push_back_uint8_array(src, values[i]), NO_ERROR);
    }

    /* slice [1, 4) -> [20, 30, 40] */
    uint8_array_expect_t slice = slice_uint8_array(src, 1, 4, alloc);
    assert_true(slice.has_value);

    uint8_t expected[] = { 20, 30, 40 };
    uint8_t out = 0;
    for (int i = 0; i < 3; i++) {
        assert_int_equal(get_uint8_array_index(slice.u.value, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(src);
    return_uint8_array(slice.u.value);
}

// --------------------------------------------------------------------------------

static void test_slice_from_index_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* src = r.u.value;

    uint8_t values[] = { 10, 20, 30, 40 };
    for (int i = 0; i < 4; i++) {
        assert_int_equal(push_back_uint8_array(src, values[i]), NO_ERROR);
    }

    /* slice [0, 2) -> [10, 20] */
    uint8_array_expect_t slice = slice_uint8_array(src, 0, 2, alloc);
    assert_true(slice.has_value);
    assert_int_equal((int)uint8_array_size(slice.u.value), 2);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(slice.u.value, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 10);
    assert_int_equal(get_uint8_array_index(slice.u.value, 1, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_uint8_array(src);
    return_uint8_array(slice.u.value);
}

// --------------------------------------------------------------------------------

static void test_slice_to_end_of_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* src = r.u.value;

    uint8_t values[] = { 10, 20, 30, 40 };
    for (int i = 0; i < 4; i++) {
        assert_int_equal(push_back_uint8_array(src, values[i]), NO_ERROR);
    }

    /* slice [2, 4) -> [30, 40] */
    uint8_array_expect_t slice = slice_uint8_array(src, 2, 4, alloc);
    assert_true(slice.has_value);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(slice.u.value, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 30);
    assert_int_equal(get_uint8_array_index(slice.u.value, 1, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint8_array(src);
    return_uint8_array(slice.u.value);
}

// --------------------------------------------------------------------------------

static void test_slice_is_independent_from_src(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* src = r.u.value;

    uint8_t values[] = { 10, 20, 30 };
    for (int i = 0; i < 3; i++) {
        assert_int_equal(push_back_uint8_array(src, values[i]), NO_ERROR);
    }

    uint8_array_expect_t slice = slice_uint8_array(src, 0, 3, alloc);
    assert_true(slice.has_value);

    /* Modify src — slice must be unaffected */
    assert_int_equal(set_uint8_array_index(src, 0, 99), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(slice.u.value, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    return_uint8_array(src);
    return_uint8_array(slice.u.value);
}

// --------------------------------------------------------------------------------

static void test_slice_start_equal_end_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* src = r.u.value;

    assert_int_equal(push_back_uint8_array(src, 10), NO_ERROR);

    uint8_array_expect_t slice = slice_uint8_array(src, 1, 1, alloc);
    assert_false(slice.has_value);
    assert_int_equal(slice.u.error, INVALID_ARG);

    return_uint8_array(src);
}

// --------------------------------------------------------------------------------

static void test_slice_end_out_of_bounds_fails(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* src = r.u.value;

    assert_int_equal(push_back_uint8_array(src, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(src, 20), NO_ERROR);

    /* end > len */
    uint8_array_expect_t slice = slice_uint8_array(src, 0, 3, alloc);
    assert_false(slice.has_value);
    assert_int_equal(slice.u.error, OUT_OF_BOUNDS);

    return_uint8_array(src);
}

// --------------------------------------------------------------------------------

static void test_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t slice = slice_uint8_array(NULL, 0, 1, alloc);
    assert_false(slice.has_value);
    assert_int_equal(slice.u.error, NULL_POINTER);
}

// ================================================================================
// Group 15: reverse_uint8_array
// ================================================================================

static void test_reverse_two_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(reverse_uint8_array(arr), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 20);
    assert_int_equal(get_uint8_array_index(arr, 1, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_reverse_odd_number_of_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* [10, 20, 30, 40, 50] -> [50, 40, 30, 20, 10] */
    uint8_t values[] = { 10, 20, 30, 40, 50 };
    for (int i = 0; i < 5; i++) {
        assert_int_equal(push_back_uint8_array(arr, values[i]), NO_ERROR);
    }
    assert_int_equal(reverse_uint8_array(arr), NO_ERROR);

    uint8_t expected[] = { 50, 40, 30, 20, 10 };
    uint8_t out = 0;
    for (int i = 0; i < 5; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_reverse_even_number_of_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* [1, 2, 3, 4] -> [4, 3, 2, 1] */
    uint8_t values[] = { 1, 2, 3, 4 };
    for (int i = 0; i < 4; i++) {
        assert_int_equal(push_back_uint8_array(arr, values[i]), NO_ERROR);
    }
    assert_int_equal(reverse_uint8_array(arr), NO_ERROR);

    uint8_t expected[] = { 4, 3, 2, 1 };
    uint8_t out = 0;
    for (int i = 0; i < 4; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_reverse_single_element_succeeds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);
    /* single element: EMPTY is returned since len < 2 */
    assert_int_equal(reverse_uint8_array(arr), EMPTY);

    /* Value must be untouched */
    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 42);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_reverse_twice_restores_original_order(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    uint8_t values[] = { 10, 20, 30, 40 };
    for (int i = 0; i < 4; i++) {
        assert_int_equal(push_back_uint8_array(arr, values[i]), NO_ERROR);
    }

    assert_int_equal(reverse_uint8_array(arr), NO_ERROR);
    assert_int_equal(reverse_uint8_array(arr), NO_ERROR);

    uint8_t out = 0;
    for (int i = 0; i < 4; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)values[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_reverse_does_not_change_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 1), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 2), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 3), NO_ERROR);
    assert_int_equal(reverse_uint8_array(arr), NO_ERROR);

    assert_int_equal((int)uint8_array_size(arr), 3);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_reverse_empty_array_returns_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(reverse_uint8_array(arr), EMPTY);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_uint8_array(NULL), NULL_POINTER);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Group 16: uint8_array_contains
// ================================================================================

static void test_contains_finds_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 10, 0, 3);
    assert_true(result.has_value);
    assert_int_equal((int)result.u.value, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_finds_middle_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 55), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 55, 0, 3);
    assert_true(result.has_value);
    assert_int_equal((int)result.u.value, 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_finds_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 99), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 99, 0, 3);
    assert_true(result.has_value);
    assert_int_equal((int)result.u.value, 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_returns_first_of_duplicates(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* Value 42 appears at indices 1 and 3 — must return 1 */
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 42, 0, 4);
    assert_true(result.has_value);
    assert_int_equal((int)result.u.value, 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_not_found_returns_not_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 99, 0, 3);
    assert_false(result.has_value);
    assert_int_equal(result.u.error, NOT_FOUND);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_respects_start_boundary(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* Value 10 is at index 0 — searching from index 1 should not find it */
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 10, 1, 3);
    assert_false(result.has_value);
    assert_int_equal(result.u.error, NOT_FOUND);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_respects_end_boundary(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* Value 30 is at index 2 — searching [0, 2) should not find it */
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 30, 0, 2);
    assert_false(result.has_value);
    assert_int_equal(result.u.error, NOT_FOUND);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_single_element_range_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    /* Search range [1, 2) — single element window containing 20 */
    size_expect_t result = uint8_array_contains(arr, 20, 1, 2);
    assert_true(result.has_value);
    assert_int_equal((int)result.u.value, 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_single_element_range_no_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    /* Search range [1, 2) — single element window, looking for 99 */
    size_expect_t result = uint8_array_contains(arr, 99, 1, 2);
    assert_false(result.has_value);
    assert_int_equal(result.u.error, NOT_FOUND);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_zero_value_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr,  0), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 0, 0, 3);
    assert_true(result.has_value);
    assert_int_equal((int)result.u.value, 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_max_value_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr,  10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 255), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr,  30), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 255, 0, 3);
    assert_true(result.has_value);
    assert_int_equal((int)result.u.value, 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t result = uint8_array_contains(NULL, 42, 0, 1);
    assert_false(result.has_value);
    assert_int_equal(result.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_contains_start_equal_end_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);

    size_expect_t result = uint8_array_contains(arr, 10, 1, 1);
    assert_false(result.has_value);
    assert_int_equal(result.u.error, INVALID_ARG);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_contains_end_out_of_bounds_returns_out_of_bounds(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);

    /* end > len */
    size_expect_t result = uint8_array_contains(arr, 10, 0, 3);
    assert_false(result.has_value);
    assert_int_equal(result.u.error, OUT_OF_BOUNDS);

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

    /* Group 4: push_front */
    cmocka_unit_test(test_push_front_increments_len),
    cmocka_unit_test(test_push_front_value_is_at_index_zero),
    cmocka_unit_test(test_push_front_shifts_existing_elements),
    cmocka_unit_test(test_push_front_multiple_reverses_insertion_order),
    cmocka_unit_test(test_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_push_front_beyond_capacity_no_growth_fails),
    cmocka_unit_test(test_push_front_beyond_capacity_with_growth_succeeds),

    /* Group 5: push_at */
    cmocka_unit_test(test_push_at_increments_len),
    cmocka_unit_test(test_push_at_middle_shifts_correctly),
    cmocka_unit_test(test_push_at_index_zero_behaves_like_push_front),
    cmocka_unit_test(test_push_at_index_len_behaves_like_push_back),
    cmocka_unit_test(test_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_push_at_out_of_bounds_index_fails),
    cmocka_unit_test(test_push_at_beyond_capacity_no_growth_fails),
    cmocka_unit_test(test_push_at_beyond_capacity_with_growth_succeeds),
    cmocka_unit_test(test_push_at_empty_array_at_index_zero_succeeds),

  /* Group 6: get_uint8_array_index */
    cmocka_unit_test(test_get_returns_correct_value_at_index_zero),
    cmocka_unit_test(test_get_returns_correct_value_at_last_index),
    cmocka_unit_test(test_get_returns_correct_value_at_middle_index),
    cmocka_unit_test(test_get_does_not_modify_array_len),
    cmocka_unit_test(test_get_all_values_in_sequence),
    cmocka_unit_test(test_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_get_out_of_bounds_index_fails),
    cmocka_unit_test(test_get_on_empty_array_returns_out_of_bounds),
    cmocka_unit_test(test_get_zero_value_retrieved_correctly),
    cmocka_unit_test(test_get_max_value_retrieved_correctly),

    /* Group 7: pop_back */
    cmocka_unit_test(test_pop_back_decrements_len),
    cmocka_unit_test(test_pop_back_returns_correct_value),
    cmocka_unit_test(test_pop_back_null_out_discards_silently),
    cmocka_unit_test(test_pop_back_leaves_remaining_elements_intact),
    cmocka_unit_test(test_pop_back_until_empty),
    cmocka_unit_test(test_pop_back_on_empty_returns_empty),
    cmocka_unit_test(test_pop_back_null_array_returns_null_pointer),

    /* Group 8: pop_front */
    cmocka_unit_test(test_pop_front_decrements_len),
    cmocka_unit_test(test_pop_front_returns_correct_value),
    cmocka_unit_test(test_pop_front_null_out_discards_silently),
    cmocka_unit_test(test_pop_front_shifts_remaining_elements),
    cmocka_unit_test(test_pop_front_until_empty),
    cmocka_unit_test(test_pop_front_on_empty_returns_empty),
    cmocka_unit_test(test_pop_front_null_array_returns_null_pointer),

    /* Group 9: pop_any */
    cmocka_unit_test(test_pop_any_decrements_len),
    cmocka_unit_test(test_pop_any_middle_returns_correct_value),
    cmocka_unit_test(test_pop_any_middle_shifts_remaining_elements),
    cmocka_unit_test(test_pop_any_index_zero_behaves_like_pop_front),
    cmocka_unit_test(test_pop_any_last_index_behaves_like_pop_back),
    cmocka_unit_test(test_pop_any_null_out_discards_silently),
    cmocka_unit_test(test_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_pop_any_on_empty_returns_empty),
    cmocka_unit_test(test_pop_any_out_of_bounds_index_fails),

    /* Group 10: clear_uint8_array */
    cmocka_unit_test(test_clear_sets_len_to_zero),
    cmocka_unit_test(test_clear_preserves_alloc_capacity),
    cmocka_unit_test(test_clear_array_is_empty_after_clear),
    cmocka_unit_test(test_clear_allows_reuse_after_clear),
    cmocka_unit_test(test_clear_on_empty_array_succeeds),
    cmocka_unit_test(test_clear_null_array_returns_null_pointer),

    /* Group 11: set_uint8_array_index */
    cmocka_unit_test(test_set_overwrites_value_at_index),
    cmocka_unit_test(test_set_does_not_change_len),
    cmocka_unit_test(test_set_does_not_affect_other_elements),
    cmocka_unit_test(test_set_at_last_index),
    cmocka_unit_test(test_set_out_of_bounds_index_fails),
    cmocka_unit_test(test_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_set_zero_value_stored_correctly),
    cmocka_unit_test(test_set_max_value_stored_correctly),

    /* Group 12: copy_uint8_array */
    cmocka_unit_test(test_copy_returns_valid_array),
    cmocka_unit_test(test_copy_has_same_len),
    cmocka_unit_test(test_copy_contains_same_values),
    cmocka_unit_test(test_copy_is_independent_from_src),
    cmocka_unit_test(test_copy_is_independent_when_dst_modified),
    cmocka_unit_test(test_copy_of_empty_array_succeeds),
    cmocka_unit_test(test_copy_null_src_returns_null_pointer),

    /* Group 13: concat_uint8_array */
    cmocka_unit_test(test_concat_appends_all_src_elements),
    cmocka_unit_test(test_concat_increases_dst_len),
    cmocka_unit_test(test_concat_src_is_unmodified),
    cmocka_unit_test(test_concat_empty_src_leaves_dst_unchanged),
    cmocka_unit_test(test_concat_with_growth_succeeds),
    cmocka_unit_test(test_concat_without_growth_fails_when_full),
    cmocka_unit_test(test_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_concat_null_src_returns_null_pointer),

    /* Group 14: slice_uint8_array */
    cmocka_unit_test(test_slice_returns_valid_array),
    cmocka_unit_test(test_slice_has_correct_len),
    cmocka_unit_test(test_slice_contains_correct_values),
    cmocka_unit_test(test_slice_from_index_zero),
    cmocka_unit_test(test_slice_to_end_of_array),
    cmocka_unit_test(test_slice_is_independent_from_src),
    cmocka_unit_test(test_slice_start_equal_end_fails),
    cmocka_unit_test(test_slice_end_out_of_bounds_fails),
    cmocka_unit_test(test_slice_null_src_returns_null_pointer),

    /* Group 15: reverse_uint8_array */
    cmocka_unit_test(test_reverse_two_elements),
    cmocka_unit_test(test_reverse_odd_number_of_elements),
    cmocka_unit_test(test_reverse_even_number_of_elements),
    cmocka_unit_test(test_reverse_single_element_succeeds),
    cmocka_unit_test(test_reverse_twice_restores_original_order),
    cmocka_unit_test(test_reverse_does_not_change_len),
    cmocka_unit_test(test_reverse_empty_array_returns_empty),
    cmocka_unit_test(test_reverse_null_array_returns_null_pointer),

    /* Group 16: uint8_array_contains */
    cmocka_unit_test(test_contains_finds_first_element),
    cmocka_unit_test(test_contains_finds_middle_element),
    cmocka_unit_test(test_contains_finds_last_element),
    cmocka_unit_test(test_contains_returns_first_of_duplicates),
    cmocka_unit_test(test_contains_not_found_returns_not_found),
    cmocka_unit_test(test_contains_respects_start_boundary),
    cmocka_unit_test(test_contains_respects_end_boundary),
    cmocka_unit_test(test_contains_single_element_range_match),
    cmocka_unit_test(test_contains_single_element_range_no_match),
    cmocka_unit_test(test_contains_zero_value_found),
    cmocka_unit_test(test_contains_max_value_found),
    cmocka_unit_test(test_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_contains_start_equal_end_returns_invalid_arg),
    cmocka_unit_test(test_contains_end_out_of_bounds_returns_out_of_bounds),
};

const size_t test_uint8_array_count = sizeof(test_uint8_array) / sizeof(test_uint8_array[0]);
// ================================================================================
// ================================================================================
// eof
