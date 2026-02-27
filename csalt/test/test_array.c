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
#include "c_int8.h"
#include "c_uint16.h"
#include "c_int16.h"
#include "c_uint32.h"
#include "c_int32.h"
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
// Group 17: sort_uint8_array
// ================================================================================

static void test_sort_forward_basic_order(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, FORWARD), NO_ERROR);

    uint8_t expected[] = { 10, 20, 30 };
    uint8_t out = 0;
    for (int i = 0; i < 3; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_reverse_basic_order(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, REVERSE), NO_ERROR);

    uint8_t expected[] = { 30, 20, 10 };
    uint8_t out = 0;
    for (int i = 0; i < 3; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_forward_already_sorted(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, FORWARD), NO_ERROR);

    uint8_t expected[] = { 10, 20, 30 };
    uint8_t out = 0;
    for (int i = 0; i < 3; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_forward_reverse_sorted_input(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 50), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 40), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, FORWARD), NO_ERROR);

    uint8_t expected[] = { 10, 20, 30, 40, 50 };
    uint8_t out = 0;
    for (int i = 0; i < 5; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_does_not_change_len(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, FORWARD), NO_ERROR);

    assert_int_equal((int)uint8_array_size(arr), 3);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_forward_with_duplicates(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, FORWARD), NO_ERROR);

    uint8_t expected[] = { 10, 10, 20, 30, 30 };
    uint8_t out = 0;
    for (int i = 0; i < 5; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_reverse_with_duplicates(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, REVERSE), NO_ERROR);

    uint8_t expected[] = { 30, 30, 20, 10, 10 };
    uint8_t out = 0;
    for (int i = 0; i < 5; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_all_same_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    for (int i = 0; i < 5; i++) {
        assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);
    }
    assert_int_equal(sort_uint8_array(arr, FORWARD), NO_ERROR);

    uint8_t out = 0;
    for (int i = 0; i < 5; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, 42);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_forward_two_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, FORWARD), NO_ERROR);

    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 10);
    assert_int_equal(get_uint8_array_index(arr, 1, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_forward_min_max_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 255), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 128), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr,   0), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr,   1), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, FORWARD), NO_ERROR);

    uint8_t expected[] = { 0, 1, 128, 255 };
    uint8_t out = 0;
    for (int i = 0; i < 4; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, (int)expected[i]);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_forward_large_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    /* 64 elements forces the quicksort path — well above the insertion threshold */
    uint8_array_expect_t r = init_uint8_array(64, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* Push values 63 down to 0 (reverse order) */
    for (int i = 63; i >= 0; i--) {
        assert_int_equal(push_back_uint8_array(arr, (uint8_t)i), NO_ERROR);
    }
    assert_int_equal(sort_uint8_array(arr, FORWARD), NO_ERROR);

    uint8_t out = 0;
    for (int i = 0; i < 64; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, i);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_reverse_large_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(64, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* Push values 0 up to 63 (ascending order) */
    for (int i = 0; i < 64; i++) {
        assert_int_equal(push_back_uint8_array(arr, (uint8_t)i), NO_ERROR);
    }
    assert_int_equal(sort_uint8_array(arr, REVERSE), NO_ERROR);

    uint8_t out = 0;
    for (int i = 0; i < 64; i++) {
        assert_int_equal(get_uint8_array_index(arr, (size_t)i, &out), NO_ERROR);
        assert_int_equal((int)out, 63 - i);
    }

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_single_element_returns_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);
    assert_int_equal(sort_uint8_array(arr, FORWARD), EMPTY);

    /* Value must be untouched */
    uint8_t out = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &out), NO_ERROR);
    assert_int_equal((int)out, 42);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_sort_empty_array_returns_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(sort_uint8_array(arr, FORWARD), EMPTY);

    return_uint8_array(arr);
}
// -------------------------------------------------------------------------------- 

static void test_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_uint8_array(NULL, FORWARD), NULL_POINTER);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Group 18: uint8_array_binary_search
// ================================================================================

static void test_bsearch_finds_value_with_sort(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 40), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 30, true);
    assert_true(sr.has_value);
    /* After sort arr is [10,20,30,40]; 30 is at index 2 */
    assert_int_equal((int)sr.u.value, 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_finds_value_presorted(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 40), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 20, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_returns_first_of_duplicates(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* Push unsorted so sort flag is exercised */
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 10, true);
    assert_true(sr.has_value);
    /* After sort [10,10,20,30]; first 10 is at index 0 */
    assert_int_equal((int)sr.u.value, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_finds_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 10, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_finds_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 30, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_not_found_returns_not_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 99, false);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_sort_persists_after_call(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 50), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    uint8_array_binary_search(arr, 10, true);

    /* Verify the array is in sorted order afterward */
    uint8_t val = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &val), NO_ERROR);
    assert_int_equal((int)val, 10);
    assert_int_equal(get_uint8_array_index(arr, 1, &val), NO_ERROR);
    assert_int_equal((int)val, 30);
    assert_int_equal(get_uint8_array_index(arr, 2, &val), NO_ERROR);
    assert_int_equal((int)val, 50);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_min_value_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr,   0), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 128), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 255), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 0, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_max_value_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr,   0), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 128), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 255), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 255, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_single_element_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 42, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_single_element_not_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);

    size_expect_t sr = uint8_array_binary_search(arr, 99, false);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_empty_array_returns_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    size_expect_t sr = uint8_array_binary_search(arr, 10, false);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, EMPTY);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bsearch_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t sr = uint8_array_binary_search(NULL, 10, false);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NULL_POINTER);
}

// ================================================================================
// Group 19: uint8_array_binary_bracket
// ================================================================================

static void test_bracket_exact_match_lower_equals_upper(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 40), NO_ERROR);

    bracket_expect_t br = uint8_array_binary_bracket(arr, 30, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 2);
    assert_int_equal((int)br.u.value.upper, 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_exact_match_first_occurrence(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* [10, 20, 20, 30] — two copies of 20 */
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    bracket_expect_t br = uint8_array_binary_bracket(arr, 20, false);
    assert_true(br.has_value);
    /* First occurrence of 20 is at index 1; lower == upper == 1 */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_value_between_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 50), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 70), NO_ERROR);

    /* 40 sits between 30 (index 1) and 50 (index 2) */
    bracket_expect_t br = uint8_array_binary_bracket(arr, 40, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_lower_not_equal_upper_when_between(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 40), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 50), NO_ERROR);

    bracket_expect_t br = uint8_array_binary_bracket(arr, 30, false);
    assert_true(br.has_value);
    /* lower != upper confirms this is a gap result, not an exact match */
    assert_true(br.u.value.lower != br.u.value.upper);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_value_at_minimum(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    /* 10 is the minimum — exact match, lower == upper == 0 */
    bracket_expect_t br = uint8_array_binary_bracket(arr, 10, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 0);
    assert_int_equal((int)br.u.value.upper, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_value_at_maximum(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    /* 30 is the maximum — exact match, lower == upper == 2 */
    bracket_expect_t br = uint8_array_binary_bracket(arr, 30, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 2);
    assert_int_equal((int)br.u.value.upper, 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_below_minimum_returns_not_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    bracket_expect_t br = uint8_array_binary_bracket(arr, 5, false);
    assert_false(br.has_value);
    assert_int_equal(br.u.error, NOT_FOUND);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_above_maximum_returns_not_found(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    bracket_expect_t br = uint8_array_binary_bracket(arr, 99, false);
    assert_false(br.has_value);
    assert_int_equal(br.u.error, NOT_FOUND);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_with_sort_flag(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    /* Push unsorted */
    assert_int_equal(push_back_uint8_array(arr, 50), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 40), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20), NO_ERROR);

    /* 30 is between 20 and 40 after sorting */
    bracket_expect_t br = uint8_array_binary_bracket(arr, 30, true);
    assert_true(br.has_value);
    /* After sort [10,20,40,50]; 20 at index 1, 40 at index 2 */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_sort_persists_after_call(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(8, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 50), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 10), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 30), NO_ERROR);

    uint8_array_binary_bracket(arr, 20, true);

    /* Verify sorted order persists */
    uint8_t val = 0;
    assert_int_equal(get_uint8_array_index(arr, 0, &val), NO_ERROR);
    assert_int_equal((int)val, 10);
    assert_int_equal(get_uint8_array_index(arr, 1, &val), NO_ERROR);
    assert_int_equal((int)val, 30);
    assert_int_equal(get_uint8_array_index(arr, 2, &val), NO_ERROR);
    assert_int_equal((int)val, 50);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_single_element_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);

    bracket_expect_t br = uint8_array_binary_bracket(arr, 42, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 0);
    assert_int_equal((int)br.u.value.upper, 0);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_single_element_out_of_range(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint8_array(arr, 42), NO_ERROR);

    bracket_expect_t br = uint8_array_binary_bracket(arr, 99, false);
    assert_false(br.has_value);
    assert_int_equal(br.u.error, NOT_FOUND);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_empty_array_returns_empty(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(4, false, alloc);
    assert_true(r.has_value);
    uint8_array_t* arr = r.u.value;

    bracket_expect_t br = uint8_array_binary_bracket(arr, 10, false);
    assert_false(br.has_value);
    assert_int_equal(br.u.error, EMPTY);

    return_uint8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t br = uint8_array_binary_bracket(NULL, 10, false);
    assert_false(br.has_value);
    assert_int_equal(br.u.error, NULL_POINTER);
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

    /* Group 17: sort_uint8_array */
    cmocka_unit_test(test_sort_forward_basic_order),
    cmocka_unit_test(test_sort_reverse_basic_order),
    cmocka_unit_test(test_sort_forward_already_sorted),
    cmocka_unit_test(test_sort_forward_reverse_sorted_input),
    cmocka_unit_test(test_sort_does_not_change_len),
    cmocka_unit_test(test_sort_forward_with_duplicates),
    cmocka_unit_test(test_sort_reverse_with_duplicates),
    cmocka_unit_test(test_sort_all_same_values),
    cmocka_unit_test(test_sort_forward_two_elements),
    cmocka_unit_test(test_sort_forward_min_max_values),
    cmocka_unit_test(test_sort_forward_large_array),
    cmocka_unit_test(test_sort_reverse_large_array),
    cmocka_unit_test(test_sort_single_element_returns_empty),
    cmocka_unit_test(test_sort_empty_array_returns_empty),
    cmocka_unit_test(test_sort_null_array_returns_null_pointer),

    /* Group 18: uint8_array_binary_search */
    cmocka_unit_test(test_bsearch_finds_value_with_sort),
    cmocka_unit_test(test_bsearch_finds_value_presorted),
    cmocka_unit_test(test_bsearch_returns_first_of_duplicates),
    cmocka_unit_test(test_bsearch_finds_first_element),
    cmocka_unit_test(test_bsearch_finds_last_element),
    cmocka_unit_test(test_bsearch_not_found_returns_not_found),
    cmocka_unit_test(test_bsearch_sort_persists_after_call),
    cmocka_unit_test(test_bsearch_min_value_found),
    cmocka_unit_test(test_bsearch_max_value_found),
    cmocka_unit_test(test_bsearch_single_element_found),
    cmocka_unit_test(test_bsearch_single_element_not_found),
    cmocka_unit_test(test_bsearch_empty_array_returns_empty),
    cmocka_unit_test(test_bsearch_null_array_returns_null_pointer),

    /* Group 19: uint8_array_binary_bracket */
    cmocka_unit_test(test_bracket_exact_match_lower_equals_upper),
    cmocka_unit_test(test_bracket_exact_match_first_occurrence),
    cmocka_unit_test(test_bracket_value_between_elements),
    cmocka_unit_test(test_bracket_lower_not_equal_upper_when_between),
    cmocka_unit_test(test_bracket_value_at_minimum),
    cmocka_unit_test(test_bracket_value_at_maximum),
    cmocka_unit_test(test_bracket_below_minimum_returns_not_found),
    cmocka_unit_test(test_bracket_above_maximum_returns_not_found),
    cmocka_unit_test(test_bracket_with_sort_flag),
    cmocka_unit_test(test_bracket_sort_persists_after_call),
    cmocka_unit_test(test_bracket_single_element_exact_match),
    cmocka_unit_test(test_bracket_single_element_out_of_range),
    cmocka_unit_test(test_bracket_empty_array_returns_empty),
    cmocka_unit_test(test_bracket_null_array_returns_null_pointer),
};

const size_t test_uint8_array_count = sizeof(test_uint8_array) / sizeof(test_uint8_array[0]);
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Group 1: init_int8_array
// ================================================================================

// ================================================================================
// Group 1: init_int8_array
// ================================================================================

static void test_int8_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    int8_array_expect_t r = init_int8_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)int8_array_size(r.u.value), 0);
    assert_int_equal((int)int8_array_alloc(r.u.value), 8);
    return_int8_array(r.u.value);
}

// ================================================================================
// Group 2: return_int8_array
// ================================================================================

static void test_int8_return_null_is_safe(void** state) {
    (void)state;
    return_int8_array(NULL);   /* must not crash */
}

// --------------------------------------------------------------------------------

static void test_int8_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    return_int8_array(r.u.value);   /* must not crash */
}

// ================================================================================
// Group 3: push_back_int8_array
// ================================================================================

static void test_int8_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_int8_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    assert_int_equal(push_back_int8_array(arr, -10), NO_ERROR);
    assert_int_equal(push_back_int8_array(arr,   0), NO_ERROR);
    assert_int_equal(push_back_int8_array(arr,  10), NO_ERROR);
    assert_int_equal((int)int8_array_size(arr), 3);

    int8_t val = 0;
    get_int8_array_index(arr, 0, &val);  assert_int_equal((int)val, -10);
    get_int8_array_index(arr, 2, &val);  assert_int_equal((int)val,  10);

    return_int8_array(arr);
}

// ================================================================================
// Group 4: push_front_int8_array
// ================================================================================

static void test_int8_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_int8_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, 10);
    push_front_int8_array(arr, -10);
    /* arr is [-10, 10] */

    int8_t val = 0;
    get_int8_array_index(arr, 0, &val);
    assert_int_equal((int)val, -10);

    return_int8_array(arr);
}

// ================================================================================
// Group 5: push_at_int8_array
// ================================================================================

static void test_int8_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_int8_array(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,  10);
    push_at_int8_array(arr, 1, 0);
    /* arr is [-10, 0, 10] */

    int8_t val = 0;
    get_int8_array_index(arr, 1, &val);
    assert_int_equal((int)val, 0);

    return_int8_array(arr);
}

// ================================================================================
// Group 6: get_int8_array_index
// ================================================================================

static void test_int8_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    int8_t val = 0;
    assert_int_equal(get_int8_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, -128);
    push_back_int8_array(arr,    0);
    push_back_int8_array(arr,  127);

    int8_t val = 0;
    assert_int_equal(get_int8_array_index(arr, 0, &val), NO_ERROR);
    assert_int_equal((int)val, -128);
    assert_int_equal(get_int8_array_index(arr, 2, &val), NO_ERROR);
    assert_int_equal((int)val, 127);

    return_int8_array(arr);
}

// ================================================================================
// Group 7: pop_back_int8_array
// ================================================================================

static void test_int8_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_int8_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,  10);

    int8_t val = 0;
    assert_int_equal(pop_back_int8_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, 10);
    assert_int_equal((int)int8_array_size(arr), 1);

    return_int8_array(arr);
}

// ================================================================================
// Group 8: pop_front_int8_array
// ================================================================================

static void test_int8_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_int8_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,  10);

    int8_t val = 0;
    assert_int_equal(pop_front_int8_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, -10);
    assert_int_equal((int)int8_array_size(arr), 1);

    return_int8_array(arr);
}

// ================================================================================
// Group 9: pop_any_int8_array
// ================================================================================

static void test_int8_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_int8_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,   0);
    push_back_int8_array(arr,  10);

    int8_t val = 0;
    assert_int_equal(pop_any_int8_array(arr, &val, 1), NO_ERROR);
    assert_int_equal((int)val, 0);
    assert_int_equal((int)int8_array_size(arr), 2);

    return_int8_array(arr);
}

// ================================================================================
// Group 10: clear_int8_array
// ================================================================================

static void test_int8_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_int8_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,  10);
    assert_int_equal(clear_int8_array(arr), NO_ERROR);
    assert_int_equal((int)int8_array_size(arr), 0);
    assert_int_equal((int)int8_array_alloc(arr), 4);   /* capacity retained */

    return_int8_array(arr);
}

// ================================================================================
// Group 11: set_int8_array_index
// ================================================================================

static void test_int8_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_int8_array_index(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, 10);
    assert_int_equal(set_int8_array_index(arr, 0, -99), NO_ERROR);

    int8_t val = 0;
    get_int8_array_index(arr, 0, &val);
    assert_int_equal((int)val, -99);

    return_int8_array(arr);
}

// ================================================================================
// Group 12: copy_int8_array
// ================================================================================

static void test_int8_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = copy_int8_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* src = r.u.value;

    push_back_int8_array(src, -10);
    push_back_int8_array(src,  10);

    int8_array_expect_t cr = copy_int8_array(src, alloc);
    assert_true(cr.has_value);
    int8_array_t* dst = cr.u.value;

    /* Mutate src and confirm dst is unaffected */
    set_int8_array_index(src, 0, 99);
    int8_t val = 0;
    get_int8_array_index(dst, 0, &val);
    assert_int_equal((int)val, -10);

    return_int8_array(src);
    return_int8_array(dst);
}

// ================================================================================
// Group 13: concat_int8_array
// ================================================================================

static void test_int8_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* src = r.u.value;
    assert_int_equal(concat_int8_array(NULL, src), NULL_POINTER);
    return_int8_array(src);
}

// --------------------------------------------------------------------------------

static void test_int8_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r1 = init_int8_array(8, false, alloc);
    int8_array_expect_t r2 = init_int8_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    int8_array_t* dst = r1.u.value;
    int8_array_t* src = r2.u.value;

    push_back_int8_array(dst, -10);
    push_back_int8_array(src,  10);

    assert_int_equal(concat_int8_array(dst, src), NO_ERROR);
    assert_int_equal((int)int8_array_size(dst), 2);

    int8_t val = 0;
    get_int8_array_index(dst, 1, &val);
    assert_int_equal((int)val, 10);

    return_int8_array(dst);
    return_int8_array(src);
}

// ================================================================================
// Group 14: slice_int8_array
// ================================================================================

static void test_int8_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = slice_int8_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(8, false, alloc);
    assert_true(r.has_value);
    int8_array_t* src = r.u.value;

    push_back_int8_array(src, -20);
    push_back_int8_array(src, -10);
    push_back_int8_array(src,   0);
    push_back_int8_array(src,  10);
    push_back_int8_array(src,  20);

    int8_array_expect_t sr = slice_int8_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    int8_array_t* slc = sr.u.value;
    assert_int_equal((int)int8_array_size(slc), 3);

    int8_t val = 0;
    get_int8_array_index(slc, 0, &val);  assert_int_equal((int)val, -10);
    get_int8_array_index(slc, 1, &val);  assert_int_equal((int)val,   0);
    get_int8_array_index(slc, 2, &val);  assert_int_equal((int)val,  10);

    return_int8_array(src);
    return_int8_array(slc);
}

// ================================================================================
// Group 15: reverse_int8_array
// ================================================================================

static void test_int8_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_int8_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,   0);
    push_back_int8_array(arr,  10);

    assert_int_equal(reverse_int8_array(arr), NO_ERROR);

    int8_t val = 0;
    get_int8_array_index(arr, 0, &val);  assert_int_equal((int)val,  10);
    get_int8_array_index(arr, 2, &val);  assert_int_equal((int)val, -10);

    return_int8_array(arr);
}

// ================================================================================
// Group 16: sort_int8_array
// ================================================================================

static void test_int8_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_int8_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_sort_forward_orders_signed_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(8, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr,  10);
    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,   0);

    assert_int_equal(sort_int8_array(arr, FORWARD), NO_ERROR);

    int8_t val = 0;
    get_int8_array_index(arr, 0, &val);  assert_int_equal((int)val, -10);
    get_int8_array_index(arr, 1, &val);  assert_int_equal((int)val,   0);
    get_int8_array_index(arr, 2, &val);  assert_int_equal((int)val,  10);

    return_int8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int8_sort_reverse_orders_signed_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(8, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr,   0);
    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,  10);

    assert_int_equal(sort_int8_array(arr, REVERSE), NO_ERROR);

    int8_t val = 0;
    get_int8_array_index(arr, 0, &val);  assert_int_equal((int)val,  10);
    get_int8_array_index(arr, 1, &val);  assert_int_equal((int)val,   0);
    get_int8_array_index(arr, 2, &val);  assert_int_equal((int)val, -10);

    return_int8_array(arr);
}

// ================================================================================
// Group 17: int8_array_contains
// ================================================================================

static void test_int8_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = int8_array_contains(NULL, 0, 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_contains_finds_negative_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr,  10);
    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,   0);

    size_expect_t sr = int8_array_contains(arr, -10, 0, 3);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    return_int8_array(arr);
}

// ================================================================================
// Group 18: int8_array_binary_search
// ================================================================================

static void test_int8_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = int8_array_binary_search(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_binary_search_finds_value_with_sort(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(8, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr,  10);
    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,   0);
    push_back_int8_array(arr,  20);

    /* sort == true: arr becomes [-10, 0, 10, 20] */
    size_expect_t sr = int8_array_binary_search(arr, -10, true);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_int8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int8_binary_search_signed_comparator_correct(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(8, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    /* Already sorted: [-20, -10, 0, 10, 20] */
    push_back_int8_array(arr, -20);
    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,   0);
    push_back_int8_array(arr,  10);
    push_back_int8_array(arr,  20);

    size_expect_t sr = int8_array_binary_search(arr, -10, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = int8_array_binary_search(arr, 20, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 4);

    return_int8_array(arr);
}

// ================================================================================
// Group 19: int8_array_binary_bracket
// ================================================================================

static void test_int8_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = int8_array_binary_bracket(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(8, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, -20);
    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,   0);
    push_back_int8_array(arr,  10);

    bracket_expect_t br = int8_array_binary_bracket(arr, -10, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_int8_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int8_binary_bracket_signed_gap(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(8, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    /* [-20, -10, 10, 20] — gap between -10 and 10 */
    push_back_int8_array(arr, -20);
    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,  10);
    push_back_int8_array(arr,  20);

    /* 0 falls in the gap */
    bracket_expect_t br = int8_array_binary_bracket(arr, 0, false);
    assert_true(br.has_value);
    /* lower = first element >= 0 = index 2 (value 10)  */
    /* upper = last  element <= 0 = index 1 (value -10) */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_int8_array(arr);
}

// ================================================================================
// Group 20: int8_array_size
// ================================================================================

static void test_int8_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int8_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int8_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    assert_int_equal((int)int8_array_size(arr), 0);
    push_back_int8_array(arr, -1);
    push_back_int8_array(arr,  1);
    assert_int_equal((int)int8_array_size(arr), 2);

    return_int8_array(arr);
}

// ================================================================================
// Group 21: int8_array_alloc
// ================================================================================

static void test_int8_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int8_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int8_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)int8_array_alloc(r.u.value), 16);
    return_int8_array(r.u.value);
}

// ================================================================================
// Group 22: int8_array_data_size
// ================================================================================

static void test_int8_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int8_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int8_data_size_is_one(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)int8_array_data_size(r.u.value), 1);
    return_int8_array(r.u.value);
}

// ================================================================================
// Group 23: is_int8_array_empty
// ================================================================================

static void test_int8_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int8_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_int8_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(4, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    assert_true(is_int8_array_empty(arr));
    push_back_int8_array(arr, -1);
    assert_false(is_int8_array_empty(arr));

    return_int8_array(arr);
}

// ================================================================================
// Group 24: is_int8_array_full
// ================================================================================

static void test_int8_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int8_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_int8_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(2, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    assert_false(is_int8_array_full(arr));
    push_back_int8_array(arr, -1);
    push_back_int8_array(arr,  1);
    assert_true(is_int8_array_full(arr));

    return_int8_array(arr);
}

// ================================================================================
// Group 25: is_int8_array_ptr
// ================================================================================

static void test_int8_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    int8_t val = 0;
    assert_false(is_int8_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_int8_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_array_expect_t r = init_int8_array(8, false, alloc);
    assert_true(r.has_value);
    int8_array_t* arr = r.u.value;

    push_back_int8_array(arr, -10);
    push_back_int8_array(arr,   0);
    push_back_int8_array(arr,  10);

    const int8_t* first = (const int8_t*)arr->base.data;
    const int8_t* last  = first + 2;
    const int8_t* spare = first + 3;   /* beyond live region */

    assert_true (is_int8_array_ptr(arr, first));
    assert_true (is_int8_array_ptr(arr, last));
    assert_false(is_int8_array_ptr(arr, spare));
    assert_false(is_int8_array_ptr(arr, NULL));

    return_int8_array(arr);
}

// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_int8_array[] = {

    /* Group 1: init_int8_array */
    cmocka_unit_test(test_int8_init_null_allocate_fn_fails),
    cmocka_unit_test(test_int8_init_returns_valid_array),

    /* Group 2: return_int8_array */
    cmocka_unit_test(test_int8_return_null_is_safe),
    cmocka_unit_test(test_int8_return_valid_array_does_not_crash),

    /* Group 3: push_back_int8_array */
    cmocka_unit_test(test_int8_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_push_back_appends_value),

    /* Group 4: push_front_int8_array */
    cmocka_unit_test(test_int8_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_push_front_prepends_value),

    /* Group 5: push_at_int8_array */
    cmocka_unit_test(test_int8_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_push_at_inserts_at_index),

    /* Group 6: get_int8_array_index */
    cmocka_unit_test(test_int8_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_get_returns_correct_value),

    /* Group 7: pop_back_int8_array */
    cmocka_unit_test(test_int8_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_pop_back_removes_last_element),

    /* Group 8: pop_front_int8_array */
    cmocka_unit_test(test_int8_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_pop_front_removes_first_element),

    /* Group 9: pop_any_int8_array */
    cmocka_unit_test(test_int8_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_pop_any_removes_element_at_index),

    /* Group 10: clear_int8_array */
    cmocka_unit_test(test_int8_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_clear_resets_len_to_zero),

    /* Group 11: set_int8_array_index */
    cmocka_unit_test(test_int8_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_set_overwrites_element),

    /* Group 12: copy_int8_array */
    cmocka_unit_test(test_int8_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_int8_copy_produces_independent_array),

    /* Group 13: concat_int8_array */
    cmocka_unit_test(test_int8_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_int8_concat_appends_elements),

    /* Group 14: slice_int8_array */
    cmocka_unit_test(test_int8_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_int8_slice_returns_correct_subrange),

    /* Group 15: reverse_int8_array */
    cmocka_unit_test(test_int8_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_reverse_reverses_elements),

    /* Group 16: sort_int8_array */
    cmocka_unit_test(test_int8_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_sort_forward_orders_signed_values),
    cmocka_unit_test(test_int8_sort_reverse_orders_signed_values),

    /* Group 17: int8_array_contains */
    cmocka_unit_test(test_int8_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_contains_finds_negative_value),

    /* Group 18: int8_array_binary_search */
    cmocka_unit_test(test_int8_binary_search_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_binary_search_finds_value_with_sort),
    cmocka_unit_test(test_int8_binary_search_signed_comparator_correct),

    /* Group 19: int8_array_binary_bracket */
    cmocka_unit_test(test_int8_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_int8_binary_bracket_exact_match),
    cmocka_unit_test(test_int8_binary_bracket_signed_gap),

    /* Group 20: int8_array_size */
    cmocka_unit_test(test_int8_size_null_returns_zero),
    cmocka_unit_test(test_int8_size_reflects_push_count),

    /* Group 21: int8_array_alloc */
    cmocka_unit_test(test_int8_alloc_null_returns_zero),
    cmocka_unit_test(test_int8_alloc_matches_capacity),

    /* Group 22: int8_array_data_size */
    cmocka_unit_test(test_int8_data_size_null_returns_zero),
    cmocka_unit_test(test_int8_data_size_is_one),

    /* Group 23: is_int8_array_empty */
    cmocka_unit_test(test_int8_empty_null_returns_true),
    cmocka_unit_test(test_int8_empty_reflects_contents),

    /* Group 24: is_int8_array_full */
    cmocka_unit_test(test_int8_full_null_returns_true),
    cmocka_unit_test(test_int8_full_reflects_capacity),

    /* Group 25: is_int8_array_ptr */
    cmocka_unit_test(test_int8_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_int8_is_ptr_valid_and_invalid),
};
const size_t test_int8_array_count = sizeof(test_int8_array) / sizeof(test_int8_array[0]);
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Group 1: init_uint16_array
// ================================================================================

static void test_uint16_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    uint16_array_expect_t r = init_uint16_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)uint16_array_size(r.u.value), 0);
    assert_int_equal((int)uint16_array_alloc(r.u.value), 8);
    return_uint16_array(r.u.value);
}

// ================================================================================
// Group 2: return_uint16_array
// ================================================================================

static void test_uint16_return_null_is_safe(void** state) {
    (void)state;
    return_uint16_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_uint16_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    return_uint16_array(r.u.value);
}

// ================================================================================
// Group 3: push_back_uint16_array
// ================================================================================

static void test_uint16_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_uint16_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint16_array(arr, 100), NO_ERROR);
    assert_int_equal(push_back_uint16_array(arr, 200), NO_ERROR);
    assert_int_equal(push_back_uint16_array(arr, 300), NO_ERROR);
    assert_int_equal((int)uint16_array_size(arr), 3);

    uint16_t val = 0;
    get_uint16_array_index(arr, 0, &val);  assert_int_equal((int)val, 100);
    get_uint16_array_index(arr, 2, &val);  assert_int_equal((int)val, 300);

    return_uint16_array(arr);
}

// ================================================================================
// Group 4: push_front_uint16_array
// ================================================================================

static void test_uint16_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_uint16_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 200);
    push_front_uint16_array(arr, 100);
    /* arr is [100, 200] */

    uint16_t val = 0;
    get_uint16_array_index(arr, 0, &val);
    assert_int_equal((int)val, 100);

    return_uint16_array(arr);
}

// ================================================================================
// Group 5: push_at_uint16_array
// ================================================================================

static void test_uint16_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_uint16_array(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 300);
    push_at_uint16_array(arr, 1, 200);
    /* arr is [100, 200, 300] */

    uint16_t val = 0;
    get_uint16_array_index(arr, 1, &val);
    assert_int_equal((int)val, 200);

    return_uint16_array(arr);
}

// ================================================================================
// Group 6: get_uint16_array_index
// ================================================================================

static void test_uint16_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    uint16_t val = 0;
    assert_int_equal(get_uint16_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr,     0);
    push_back_uint16_array(arr, 32768);
    push_back_uint16_array(arr, 65535);

    uint16_t val = 0;
    assert_int_equal(get_uint16_array_index(arr, 0, &val), NO_ERROR);
    assert_int_equal((int)val, 0);
    assert_int_equal(get_uint16_array_index(arr, 2, &val), NO_ERROR);
    assert_int_equal((int)val, 65535);

    return_uint16_array(arr);
}

// ================================================================================
// Group 7: pop_back_uint16_array
// ================================================================================

static void test_uint16_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_uint16_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);

    uint16_t val = 0;
    assert_int_equal(pop_back_uint16_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, 200);
    assert_int_equal((int)uint16_array_size(arr), 1);

    return_uint16_array(arr);
}

// ================================================================================
// Group 8: pop_front_uint16_array
// ================================================================================

static void test_uint16_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_uint16_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);

    uint16_t val = 0;
    assert_int_equal(pop_front_uint16_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, 100);
    assert_int_equal((int)uint16_array_size(arr), 1);

    return_uint16_array(arr);
}

// ================================================================================
// Group 9: pop_any_uint16_array
// ================================================================================

static void test_uint16_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_uint16_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);
    push_back_uint16_array(arr, 300);

    uint16_t val = 0;
    assert_int_equal(pop_any_uint16_array(arr, &val, 1), NO_ERROR);
    assert_int_equal((int)val, 200);
    assert_int_equal((int)uint16_array_size(arr), 2);

    return_uint16_array(arr);
}

// ================================================================================
// Group 10: clear_uint16_array
// ================================================================================

static void test_uint16_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_uint16_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);
    assert_int_equal(clear_uint16_array(arr), NO_ERROR);
    assert_int_equal((int)uint16_array_size(arr), 0);
    assert_int_equal((int)uint16_array_alloc(arr), 4);   /* capacity retained */

    return_uint16_array(arr);
}

// ================================================================================
// Group 11: set_uint16_array_index
// ================================================================================

static void test_uint16_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_uint16_array_index(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    assert_int_equal(set_uint16_array_index(arr, 0, 65535), NO_ERROR);

    uint16_t val = 0;
    get_uint16_array_index(arr, 0, &val);
    assert_int_equal((int)val, 65535);

    return_uint16_array(arr);
}

// ================================================================================
// Group 12: copy_uint16_array
// ================================================================================

static void test_uint16_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = copy_uint16_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* src = r.u.value;

    push_back_uint16_array(src, 100);
    push_back_uint16_array(src, 200);

    uint16_array_expect_t cr = copy_uint16_array(src, alloc);
    assert_true(cr.has_value);
    uint16_array_t* dst = cr.u.value;

    set_uint16_array_index(src, 0, 999);
    uint16_t val = 0;
    get_uint16_array_index(dst, 0, &val);
    assert_int_equal((int)val, 100);

    return_uint16_array(src);
    return_uint16_array(dst);
}

// ================================================================================
// Group 13: concat_uint16_array
// ================================================================================

static void test_uint16_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* src = r.u.value;
    assert_int_equal(concat_uint16_array(NULL, src), NULL_POINTER);
    return_uint16_array(src);
}

// --------------------------------------------------------------------------------

static void test_uint16_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r1 = init_uint16_array(8, false, alloc);
    uint16_array_expect_t r2 = init_uint16_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    uint16_array_t* dst = r1.u.value;
    uint16_array_t* src = r2.u.value;

    push_back_uint16_array(dst, 100);
    push_back_uint16_array(src, 200);

    assert_int_equal(concat_uint16_array(dst, src), NO_ERROR);
    assert_int_equal((int)uint16_array_size(dst), 2);

    uint16_t val = 0;
    get_uint16_array_index(dst, 1, &val);
    assert_int_equal((int)val, 200);

    return_uint16_array(dst);
    return_uint16_array(src);
}

// ================================================================================
// Group 14: slice_uint16_array
// ================================================================================

static void test_uint16_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = slice_uint16_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(8, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* src = r.u.value;

    push_back_uint16_array(src, 100);
    push_back_uint16_array(src, 200);
    push_back_uint16_array(src, 300);
    push_back_uint16_array(src, 400);
    push_back_uint16_array(src, 500);

    uint16_array_expect_t sr = slice_uint16_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    uint16_array_t* slc = sr.u.value;
    assert_int_equal((int)uint16_array_size(slc), 3);

    uint16_t val = 0;
    get_uint16_array_index(slc, 0, &val);  assert_int_equal((int)val, 200);
    get_uint16_array_index(slc, 1, &val);  assert_int_equal((int)val, 300);
    get_uint16_array_index(slc, 2, &val);  assert_int_equal((int)val, 400);

    return_uint16_array(src);
    return_uint16_array(slc);
}

// ================================================================================
// Group 15: reverse_uint16_array
// ================================================================================

static void test_uint16_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_uint16_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);
    push_back_uint16_array(arr, 300);

    assert_int_equal(reverse_uint16_array(arr), NO_ERROR);

    uint16_t val = 0;
    get_uint16_array_index(arr, 0, &val);  assert_int_equal((int)val, 300);
    get_uint16_array_index(arr, 2, &val);  assert_int_equal((int)val, 100);

    return_uint16_array(arr);
}

// ================================================================================
// Group 16: sort_uint16_array
// ================================================================================

static void test_uint16_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_uint16_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_sort_forward_basic(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(8, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 300);
    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);

    assert_int_equal(sort_uint16_array(arr, FORWARD), NO_ERROR);

    uint16_t val = 0;
    get_uint16_array_index(arr, 0, &val);  assert_int_equal((int)val, 100);
    get_uint16_array_index(arr, 1, &val);  assert_int_equal((int)val, 200);
    get_uint16_array_index(arr, 2, &val);  assert_int_equal((int)val, 300);

    return_uint16_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint16_sort_comparator_safe_near_max(void** state) {
    (void)state;
    /*
     * A subtract-based comparator would compute (int)(1 - 65535) = -65534,
     * which is negative and would appear correct, but (int)(65535 - 1) = 65534
     * which is also correct. The real failure mode is on platforms where
     * unsigned subtraction wraps and the result is cast to a signed type that
     * cannot represent it. Using the pair (1, 65535) confirms the comparator
     * produces strictly negative for 1 < 65535 regardless of implementation.
     */
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 65535);
    push_back_uint16_array(arr,     1);
    push_back_uint16_array(arr, 32768);

    assert_int_equal(sort_uint16_array(arr, FORWARD), NO_ERROR);

    uint16_t val = 0;
    get_uint16_array_index(arr, 0, &val);  assert_int_equal((int)val,     1);
    get_uint16_array_index(arr, 1, &val);  assert_int_equal((int)val, 32768);
    get_uint16_array_index(arr, 2, &val);  assert_int_equal((int)val, 65535);

    return_uint16_array(arr);
}

// ================================================================================
// Group 17: uint16_array_contains
// ================================================================================

static void test_uint16_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = uint16_array_contains(NULL, 0, 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_contains_finds_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);
    push_back_uint16_array(arr, 300);

    size_expect_t sr = uint16_array_contains(arr, 200, 0, 3);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    return_uint16_array(arr);
}

// ================================================================================
// Group 18: uint16_array_binary_search
// ================================================================================

static void test_uint16_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = uint16_array_binary_search(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_binary_search_finds_value_with_sort(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(8, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 300);
    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);
    push_back_uint16_array(arr, 400);

    /* sort == true: arr becomes [100, 200, 300, 400] */
    size_expect_t sr = uint16_array_binary_search(arr, 300, true);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 2);

    return_uint16_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint16_binary_search_comparator_safe_near_max(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(8, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    /* Already sorted */
    push_back_uint16_array(arr,     1);
    push_back_uint16_array(arr, 32768);
    push_back_uint16_array(arr, 65534);
    push_back_uint16_array(arr, 65535);

    size_expect_t sr = uint16_array_binary_search(arr, 65535, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 3);

    sr = uint16_array_binary_search(arr, 1, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_uint16_array(arr);
}

// ================================================================================
// Group 19: uint16_array_binary_bracket
// ================================================================================

static void test_uint16_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = uint16_array_binary_bracket(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint16_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(8, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);
    push_back_uint16_array(arr, 300);
    push_back_uint16_array(arr, 400);

    bracket_expect_t br = uint16_array_binary_bracket(arr, 200, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_uint16_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint16_binary_bracket_comparator_safe_near_max(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(8, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    /* [1, 32768, 65535] — gap between 32768 and 65535 */
    push_back_uint16_array(arr,     1);
    push_back_uint16_array(arr, 32768);
    push_back_uint16_array(arr, 65535);

    /* 40000 falls in the gap */
    bracket_expect_t br = uint16_array_binary_bracket(arr, 40000, false);
    assert_true(br.has_value);
    /* lower = first element >= 40000 = index 2 (value 65535) */
    /* upper = last  element <= 40000 = index 1 (value 32768) */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_uint16_array(arr);
}

// ================================================================================
// Group 20: uint16_array_size
// ================================================================================

static void test_uint16_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint16_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_uint16_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    assert_int_equal((int)uint16_array_size(arr), 0);
    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);
    assert_int_equal((int)uint16_array_size(arr), 2);

    return_uint16_array(arr);
}

// ================================================================================
// Group 21: uint16_array_alloc
// ================================================================================

static void test_uint16_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint16_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_uint16_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)uint16_array_alloc(r.u.value), 16);
    return_uint16_array(r.u.value);
}

// ================================================================================
// Group 22: uint16_array_data_size
// ================================================================================

static void test_uint16_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint16_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_uint16_data_size_is_two(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)uint16_array_data_size(r.u.value), 2);
    return_uint16_array(r.u.value);
}

// ================================================================================
// Group 23: is_uint16_array_empty
// ================================================================================

static void test_uint16_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint16_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_uint16_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(4, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    assert_true(is_uint16_array_empty(arr));
    push_back_uint16_array(arr, 100);
    assert_false(is_uint16_array_empty(arr));

    return_uint16_array(arr);
}

// ================================================================================
// Group 24: is_uint16_array_full
// ================================================================================

static void test_uint16_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint16_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_uint16_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(2, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    assert_false(is_uint16_array_full(arr));
    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);
    assert_true(is_uint16_array_full(arr));

    return_uint16_array(arr);
}

// ================================================================================
// Group 25: is_uint16_array_ptr
// ================================================================================

static void test_uint16_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    uint16_t val = 0;
    assert_false(is_uint16_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_uint16_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_array_expect_t r = init_uint16_array(8, false, alloc);
    assert_true(r.has_value);
    uint16_array_t* arr = r.u.value;

    push_back_uint16_array(arr, 100);
    push_back_uint16_array(arr, 200);
    push_back_uint16_array(arr, 300);

    const uint16_t* first = (const uint16_t*)arr->base.data;
    const uint16_t* last  = first + 2;
    const uint16_t* spare = first + 3;   /* beyond live region */

    assert_true (is_uint16_array_ptr(arr, first));
    assert_true (is_uint16_array_ptr(arr, last));
    assert_false(is_uint16_array_ptr(arr, spare));
    assert_false(is_uint16_array_ptr(arr, NULL));

    return_uint16_array(arr);
}

// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_uint16_array[] = {

    /* Group 1: init_uint16_array */
    cmocka_unit_test(test_uint16_init_null_allocate_fn_fails),
    cmocka_unit_test(test_uint16_init_returns_valid_array),

    /* Group 2: return_uint16_array */
    cmocka_unit_test(test_uint16_return_null_is_safe),
    cmocka_unit_test(test_uint16_return_valid_array_does_not_crash),

    /* Group 3: push_back_uint16_array */
    cmocka_unit_test(test_uint16_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_push_back_appends_value),

    /* Group 4: push_front_uint16_array */
    cmocka_unit_test(test_uint16_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_push_front_prepends_value),

    /* Group 5: push_at_uint16_array */
    cmocka_unit_test(test_uint16_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_push_at_inserts_at_index),

    /* Group 6: get_uint16_array_index */
    cmocka_unit_test(test_uint16_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_get_returns_correct_value),

    /* Group 7: pop_back_uint16_array */
    cmocka_unit_test(test_uint16_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_pop_back_removes_last_element),

    /* Group 8: pop_front_uint16_array */
    cmocka_unit_test(test_uint16_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_pop_front_removes_first_element),

    /* Group 9: pop_any_uint16_array */
    cmocka_unit_test(test_uint16_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_pop_any_removes_element_at_index),

    /* Group 10: clear_uint16_array */
    cmocka_unit_test(test_uint16_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_clear_resets_len_to_zero),

    /* Group 11: set_uint16_array_index */
    cmocka_unit_test(test_uint16_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_set_overwrites_element),

    /* Group 12: copy_uint16_array */
    cmocka_unit_test(test_uint16_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_uint16_copy_produces_independent_array),

    /* Group 13: concat_uint16_array */
    cmocka_unit_test(test_uint16_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_uint16_concat_appends_elements),

    /* Group 14: slice_uint16_array */
    cmocka_unit_test(test_uint16_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_uint16_slice_returns_correct_subrange),

    /* Group 15: reverse_uint16_array */
    cmocka_unit_test(test_uint16_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_reverse_reverses_elements),

    /* Group 16: sort_uint16_array */
    cmocka_unit_test(test_uint16_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_sort_forward_basic),
    cmocka_unit_test(test_uint16_sort_comparator_safe_near_max),

    /* Group 17: uint16_array_contains */
    cmocka_unit_test(test_uint16_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_contains_finds_value),

    /* Group 18: uint16_array_binary_search */
    cmocka_unit_test(test_uint16_binary_search_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_binary_search_finds_value_with_sort),
    cmocka_unit_test(test_uint16_binary_search_comparator_safe_near_max),

    /* Group 19: uint16_array_binary_bracket */
    cmocka_unit_test(test_uint16_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint16_binary_bracket_exact_match),
    cmocka_unit_test(test_uint16_binary_bracket_comparator_safe_near_max),

    /* Group 20: uint16_array_size */
    cmocka_unit_test(test_uint16_size_null_returns_zero),
    cmocka_unit_test(test_uint16_size_reflects_push_count),

    /* Group 21: uint16_array_alloc */
    cmocka_unit_test(test_uint16_alloc_null_returns_zero),
    cmocka_unit_test(test_uint16_alloc_matches_capacity),

    /* Group 22: uint16_array_data_size */
    cmocka_unit_test(test_uint16_data_size_null_returns_zero),
    cmocka_unit_test(test_uint16_data_size_is_two),

    /* Group 23: is_uint16_array_empty */
    cmocka_unit_test(test_uint16_empty_null_returns_true),
    cmocka_unit_test(test_uint16_empty_reflects_contents),

    /* Group 24: is_uint16_array_full */
    cmocka_unit_test(test_uint16_full_null_returns_true),
    cmocka_unit_test(test_uint16_full_reflects_capacity),

    /* Group 25: is_uint16_array_ptr */
    cmocka_unit_test(test_uint16_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_uint16_is_ptr_valid_and_invalid),
};

const size_t test_uint16_array_count = sizeof(test_uint16_array) / sizeof(test_uint16_array[0]);
// ================================================================================ 
// ================================================================================ 
// ================================================================================
// Group 1: init_int16_array
// ================================================================================

static void test_int16_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    int16_array_expect_t r = init_int16_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)int16_array_size(r.u.value), 0);
    assert_int_equal((int)int16_array_alloc(r.u.value), 8);
    return_int16_array(r.u.value);
}

// ================================================================================
// Group 2: return_int16_array
// ================================================================================

static void test_int16_return_null_is_safe(void** state) {
    (void)state;
    return_int16_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_int16_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    return_int16_array(r.u.value);
}

// ================================================================================
// Group 3: push_back_int16_array
// ================================================================================

static void test_int16_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_int16_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    assert_int_equal(push_back_int16_array(arr, 100), NO_ERROR);
    assert_int_equal(push_back_int16_array(arr, 200), NO_ERROR);
    assert_int_equal(push_back_int16_array(arr, 300), NO_ERROR);
    assert_int_equal((int)int16_array_size(arr), 3);

    int16_t val = 0;
    get_int16_array_index(arr, 0, &val);  assert_int_equal((int)val, 100);
    get_int16_array_index(arr, 2, &val);  assert_int_equal((int)val, 300);

    return_int16_array(arr);
}

// ================================================================================
// Group 4: push_front_int16_array
// ================================================================================

static void test_int16_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_int16_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 200);
    push_front_int16_array(arr, 100);
    /* arr is [100, 200] */

    int16_t val = 0;
    get_int16_array_index(arr, 0, &val);
    assert_int_equal((int)val, 100);

    return_int16_array(arr);
}

// ================================================================================
// Group 5: push_at_int16_array
// ================================================================================

static void test_int16_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_int16_array(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 300);
    push_at_int16_array(arr, 1, 200);
    /* arr is [100, 200, 300] */

    int16_t val = 0;
    get_int16_array_index(arr, 1, &val);
    assert_int_equal((int)val, 200);

    return_int16_array(arr);
}

// ================================================================================
// Group 6: get_int16_array_index
// ================================================================================

static void test_int16_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    int16_t val = 0;
    assert_int_equal(get_int16_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, -32768);
    push_back_int16_array(arr,      0);
    push_back_int16_array(arr,  32767);

    int16_t val = 0;
    assert_int_equal(get_int16_array_index(arr, 0, &val), NO_ERROR);
    assert_int_equal((int)val, -32768);
    assert_int_equal(get_int16_array_index(arr, 2, &val), NO_ERROR);
    assert_int_equal((int)val, 32767);

    return_int16_array(arr);
}

// ================================================================================
// Group 7: pop_back_int16_array
// ================================================================================

static void test_int16_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_int16_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);

    int16_t val = 0;
    assert_int_equal(pop_back_int16_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, 200);
    assert_int_equal((int)int16_array_size(arr), 1);

    return_int16_array(arr);
}

// ================================================================================
// Group 8: pop_front_int16_array
// ================================================================================

static void test_int16_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_int16_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);

    int16_t val = 0;
    assert_int_equal(pop_front_int16_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, 100);
    assert_int_equal((int)int16_array_size(arr), 1);

    return_int16_array(arr);
}

// ================================================================================
// Group 9: pop_any_int16_array
// ================================================================================

static void test_int16_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_int16_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);
    push_back_int16_array(arr, 300);

    int16_t val = 0;
    assert_int_equal(pop_any_int16_array(arr, &val, 1), NO_ERROR);
    assert_int_equal((int)val, 200);
    assert_int_equal((int)int16_array_size(arr), 2);

    return_int16_array(arr);
}

// ================================================================================
// Group 10: clear_int16_array
// ================================================================================

static void test_int16_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_int16_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);
    assert_int_equal(clear_int16_array(arr), NO_ERROR);
    assert_int_equal((int)int16_array_size(arr), 0);
    assert_int_equal((int)int16_array_alloc(arr), 4);   /* capacity retained */

    return_int16_array(arr);
}

// ================================================================================
// Group 11: set_int16_array_index
// ================================================================================

static void test_int16_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_int16_array_index(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    assert_int_equal(set_int16_array_index(arr, 0, -32768), NO_ERROR);

    int16_t val = 0;
    get_int16_array_index(arr, 0, &val);
    assert_int_equal((int)val, -32768);

    return_int16_array(arr);
}

// ================================================================================
// Group 12: copy_int16_array
// ================================================================================

static void test_int16_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = copy_int16_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* src = r.u.value;

    push_back_int16_array(src, 100);
    push_back_int16_array(src, 200);

    int16_array_expect_t cr = copy_int16_array(src, alloc);
    assert_true(cr.has_value);
    int16_array_t* dst = cr.u.value;

    set_int16_array_index(src, 0, 999);
    int16_t val = 0;
    get_int16_array_index(dst, 0, &val);
    assert_int_equal((int)val, 100);

    return_int16_array(src);
    return_int16_array(dst);
}

// ================================================================================
// Group 13: concat_int16_array
// ================================================================================

static void test_int16_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* src = r.u.value;
    assert_int_equal(concat_int16_array(NULL, src), NULL_POINTER);
    return_int16_array(src);
}

// --------------------------------------------------------------------------------

static void test_int16_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r1 = init_int16_array(8, false, alloc);
    int16_array_expect_t r2 = init_int16_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    int16_array_t* dst = r1.u.value;
    int16_array_t* src = r2.u.value;

    push_back_int16_array(dst, 100);
    push_back_int16_array(src, 200);

    assert_int_equal(concat_int16_array(dst, src), NO_ERROR);
    assert_int_equal((int)int16_array_size(dst), 2);

    int16_t val = 0;
    get_int16_array_index(dst, 1, &val);
    assert_int_equal((int)val, 200);

    return_int16_array(dst);
    return_int16_array(src);
}

// ================================================================================
// Group 14: slice_int16_array
// ================================================================================

static void test_int16_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = slice_int16_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(8, false, alloc);
    assert_true(r.has_value);
    int16_array_t* src = r.u.value;

    push_back_int16_array(src, 100);
    push_back_int16_array(src, 200);
    push_back_int16_array(src, 300);
    push_back_int16_array(src, 400);
    push_back_int16_array(src, 500);

    int16_array_expect_t sr = slice_int16_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    int16_array_t* slc = sr.u.value;
    assert_int_equal((int)int16_array_size(slc), 3);

    int16_t val = 0;
    get_int16_array_index(slc, 0, &val);  assert_int_equal((int)val, 200);
    get_int16_array_index(slc, 1, &val);  assert_int_equal((int)val, 300);
    get_int16_array_index(slc, 2, &val);  assert_int_equal((int)val, 400);

    return_int16_array(src);
    return_int16_array(slc);
}

// ================================================================================
// Group 15: reverse_int16_array
// ================================================================================

static void test_int16_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_int16_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);
    push_back_int16_array(arr, 300);

    assert_int_equal(reverse_int16_array(arr), NO_ERROR);

    int16_t val = 0;
    get_int16_array_index(arr, 0, &val);  assert_int_equal((int)val, 300);
    get_int16_array_index(arr, 2, &val);  assert_int_equal((int)val, 100);

    return_int16_array(arr);
}

// ================================================================================
// Group 16: sort_int16_array
// ================================================================================

static void test_int16_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_int16_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_sort_forward_basic(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(8, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 300);
    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);

    assert_int_equal(sort_int16_array(arr, FORWARD), NO_ERROR);

    int16_t val = 0;
    get_int16_array_index(arr, 0, &val);  assert_int_equal((int)val, 100);
    get_int16_array_index(arr, 1, &val);  assert_int_equal((int)val, 200);
    get_int16_array_index(arr, 2, &val);  assert_int_equal((int)val, 300);

    return_int16_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int16_sort_comparator_correct_signed(void** state) {
    (void)state;
    /*
     * An unsigned three-way comparator would treat -1 (stored as 0xFFFF)
     * as greater than 1, producing the wrong order. This test confirms
     * the signed comparator places negative values before positive ones.
     */
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr,  32767);
    push_back_int16_array(arr,     -1);
    push_back_int16_array(arr, -32768);

    assert_int_equal(sort_int16_array(arr, FORWARD), NO_ERROR);

    int16_t val = 0;
    get_int16_array_index(arr, 0, &val);  assert_int_equal((int)val, -32768);
    get_int16_array_index(arr, 1, &val);  assert_int_equal((int)val,     -1);
    get_int16_array_index(arr, 2, &val);  assert_int_equal((int)val,  32767);

    return_int16_array(arr);
}

// ================================================================================
// Group 17: int16_array_contains
// ================================================================================

static void test_int16_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = int16_array_contains(NULL, 0, 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_contains_finds_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);
    push_back_int16_array(arr, 300);

    size_expect_t sr = int16_array_contains(arr, 200, 0, 3);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    return_int16_array(arr);
}

// ================================================================================
// Group 18: int16_array_binary_search
// ================================================================================

static void test_int16_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = int16_array_binary_search(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_binary_search_finds_value_with_sort(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(8, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 300);
    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);
    push_back_int16_array(arr, 400);

    /* sort == true: arr becomes [100, 200, 300, 400] */
    size_expect_t sr = int16_array_binary_search(arr, 300, true);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 2);

    return_int16_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int16_binary_search_comparator_correct_signed(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(8, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    /* Already sorted across the signed range */
    push_back_int16_array(arr, -32768);
    push_back_int16_array(arr,    -1);
    push_back_int16_array(arr,     0);
    push_back_int16_array(arr, 32767);

    size_expect_t sr = int16_array_binary_search(arr, -32768, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    sr = int16_array_binary_search(arr, 32767, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 3);

    return_int16_array(arr);
}

// ================================================================================
// Group 19: int16_array_binary_bracket
// ================================================================================

static void test_int16_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = int16_array_binary_bracket(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(8, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);
    push_back_int16_array(arr, 300);
    push_back_int16_array(arr, 400);

    bracket_expect_t br = int16_array_binary_bracket(arr, 200, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_int16_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int16_binary_bracket_comparator_correct_signed(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(8, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    /* [-32768, -100, 100, 32767] — gap between -100 and 100 */
    push_back_int16_array(arr, -32768);
    push_back_int16_array(arr,   -100);
    push_back_int16_array(arr,    100);
    push_back_int16_array(arr,  32767);

    /* 0 falls in the gap */
    bracket_expect_t br = int16_array_binary_bracket(arr, 0, false);
    assert_true(br.has_value);
    /* lower = first element >= 0 = index 2 (value  100) */
    /* upper = last  element <= 0 = index 1 (value -100) */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_int16_array(arr);
}

// ================================================================================
// Group 20: int16_array_size
// ================================================================================

static void test_int16_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int16_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int16_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    assert_int_equal((int)int16_array_size(arr), 0);
    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);
    assert_int_equal((int)int16_array_size(arr), 2);

    return_int16_array(arr);
}

// ================================================================================
// Group 21: int16_array_alloc
// ================================================================================

static void test_int16_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int16_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int16_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)int16_array_alloc(r.u.value), 16);
    return_int16_array(r.u.value);
}

// ================================================================================
// Group 22: int16_array_data_size
// ================================================================================

static void test_int16_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int16_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int16_data_size_is_two(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)int16_array_data_size(r.u.value), 2);
    return_int16_array(r.u.value);
}

// ================================================================================
// Group 23: is_int16_array_empty
// ================================================================================

static void test_int16_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int16_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_int16_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(4, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    assert_true(is_int16_array_empty(arr));
    push_back_int16_array(arr, 100);
    assert_false(is_int16_array_empty(arr));

    return_int16_array(arr);
}

// ================================================================================
// Group 24: is_int16_array_full
// ================================================================================

static void test_int16_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int16_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_int16_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(2, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    assert_false(is_int16_array_full(arr));
    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);
    assert_true(is_int16_array_full(arr));

    return_int16_array(arr);
}

// ================================================================================
// Group 25: is_int16_array_ptr
// ================================================================================

static void test_int16_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    int16_t val = 0;
    assert_false(is_int16_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_int16_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_array_expect_t r = init_int16_array(8, false, alloc);
    assert_true(r.has_value);
    int16_array_t* arr = r.u.value;

    push_back_int16_array(arr, 100);
    push_back_int16_array(arr, 200);
    push_back_int16_array(arr, 300);

    const int16_t* first = (const int16_t*)arr->base.data;
    const int16_t* last  = first + 2;
    const int16_t* spare = first + 3;   /* beyond live region */

    assert_true (is_int16_array_ptr(arr, first));
    assert_true (is_int16_array_ptr(arr, last));
    assert_false(is_int16_array_ptr(arr, spare));
    assert_false(is_int16_array_ptr(arr, NULL));

    return_int16_array(arr);
}

// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_int16_array[] = {

    /* Group 1: init_int16_array */
    cmocka_unit_test(test_int16_init_null_allocate_fn_fails),
    cmocka_unit_test(test_int16_init_returns_valid_array),

    /* Group 2: return_int16_array */
    cmocka_unit_test(test_int16_return_null_is_safe),
    cmocka_unit_test(test_int16_return_valid_array_does_not_crash),

    /* Group 3: push_back_int16_array */
    cmocka_unit_test(test_int16_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_push_back_appends_value),

    /* Group 4: push_front_int16_array */
    cmocka_unit_test(test_int16_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_push_front_prepends_value),

    /* Group 5: push_at_int16_array */
    cmocka_unit_test(test_int16_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_push_at_inserts_at_index),

    /* Group 6: get_int16_array_index */
    cmocka_unit_test(test_int16_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_get_returns_correct_value),

    /* Group 7: pop_back_int16_array */
    cmocka_unit_test(test_int16_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_pop_back_removes_last_element),

    /* Group 8: pop_front_int16_array */
    cmocka_unit_test(test_int16_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_pop_front_removes_first_element),

    /* Group 9: pop_any_int16_array */
    cmocka_unit_test(test_int16_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_pop_any_removes_element_at_index),

    /* Group 10: clear_int16_array */
    cmocka_unit_test(test_int16_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_clear_resets_len_to_zero),

    /* Group 11: set_int16_array_index */
    cmocka_unit_test(test_int16_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_set_overwrites_element),

    /* Group 12: copy_int16_array */
    cmocka_unit_test(test_int16_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_int16_copy_produces_independent_array),

    /* Group 13: concat_int16_array */
    cmocka_unit_test(test_int16_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_int16_concat_appends_elements),

    /* Group 14: slice_int16_array */
    cmocka_unit_test(test_int16_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_int16_slice_returns_correct_subrange),

    /* Group 15: reverse_int16_array */
    cmocka_unit_test(test_int16_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_reverse_reverses_elements),

    /* Group 16: sort_int16_array */
    cmocka_unit_test(test_int16_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_sort_forward_basic),
    cmocka_unit_test(test_int16_sort_comparator_correct_signed),

    /* Group 17: int16_array_contains */
    cmocka_unit_test(test_int16_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_contains_finds_value),

    /* Group 18: int16_array_binary_search */
    cmocka_unit_test(test_int16_binary_search_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_binary_search_finds_value_with_sort),
    cmocka_unit_test(test_int16_binary_search_comparator_correct_signed),

    /* Group 19: int16_array_binary_bracket */
    cmocka_unit_test(test_int16_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_int16_binary_bracket_exact_match),
    cmocka_unit_test(test_int16_binary_bracket_comparator_correct_signed),

    /* Group 20: int16_array_size */
    cmocka_unit_test(test_int16_size_null_returns_zero),
    cmocka_unit_test(test_int16_size_reflects_push_count),

    /* Group 21: int16_array_alloc */
    cmocka_unit_test(test_int16_alloc_null_returns_zero),
    cmocka_unit_test(test_int16_alloc_matches_capacity),

    /* Group 22: int16_array_data_size */
    cmocka_unit_test(test_int16_data_size_null_returns_zero),
    cmocka_unit_test(test_int16_data_size_is_two),

    /* Group 23: is_int16_array_empty */
    cmocka_unit_test(test_int16_empty_null_returns_true),
    cmocka_unit_test(test_int16_empty_reflects_contents),

    /* Group 24: is_int16_array_full */
    cmocka_unit_test(test_int16_full_null_returns_true),
    cmocka_unit_test(test_int16_full_reflects_capacity),

    /* Group 25: is_int16_array_ptr */
    cmocka_unit_test(test_int16_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_int16_is_ptr_valid_and_invalid),
};
const size_t test_int16_array_count = sizeof(test_int16_array) / sizeof(test_int16_array[0]);
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Group 1: init_uint32_array
// ================================================================================

static void test_uint32_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    uint32_array_expect_t r = init_uint32_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)uint32_array_size(r.u.value), 0);
    assert_int_equal((int)uint32_array_alloc(r.u.value), 8);
    return_uint32_array(r.u.value);
}

// ================================================================================
// Group 2: return_uint32_array
// ================================================================================

static void test_uint32_return_null_is_safe(void** state) {
    (void)state;
    return_uint32_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_uint32_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    return_uint32_array(r.u.value);
}

// ================================================================================
// Group 3: push_back_uint32_array
// ================================================================================

static void test_uint32_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_uint32_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint32_array(arr, 1000), NO_ERROR);
    assert_int_equal(push_back_uint32_array(arr, 2000), NO_ERROR);
    assert_int_equal(push_back_uint32_array(arr, 3000), NO_ERROR);
    assert_int_equal((int)uint32_array_size(arr), 3);

    uint32_t val = 0;
    get_uint32_array_index(arr, 0, &val);  assert_int_equal((int)val, 1000);
    get_uint32_array_index(arr, 2, &val);  assert_int_equal((int)val, 3000);

    return_uint32_array(arr);
}

// ================================================================================
// Group 4: push_front_uint32_array
// ================================================================================

static void test_uint32_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_uint32_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 2000);
    push_front_uint32_array(arr, 1000);
    /* arr is [1000, 2000] */

    uint32_t val = 0;
    get_uint32_array_index(arr, 0, &val);
    assert_int_equal((int)val, 1000);

    return_uint32_array(arr);
}

// ================================================================================
// Group 5: push_at_uint32_array
// ================================================================================

static void test_uint32_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_uint32_array(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 3000);
    push_at_uint32_array(arr, 1, 2000);
    /* arr is [1000, 2000, 3000] */

    uint32_t val = 0;
    get_uint32_array_index(arr, 1, &val);
    assert_int_equal((int)val, 2000);

    return_uint32_array(arr);
}

// ================================================================================
// Group 6: get_uint32_array_index
// ================================================================================

static void test_uint32_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    uint32_t val = 0;
    assert_int_equal(get_uint32_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr,          0u);
    push_back_uint32_array(arr, 2147483648u);
    push_back_uint32_array(arr, 4294967295u);

    uint32_t val = 0;
    assert_int_equal(get_uint32_array_index(arr, 0, &val), NO_ERROR);
    assert_true(val == 0u);
    assert_int_equal(get_uint32_array_index(arr, 2, &val), NO_ERROR);
    assert_true(val == 4294967295u);

    return_uint32_array(arr);
}

// ================================================================================
// Group 7: pop_back_uint32_array
// ================================================================================

static void test_uint32_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_uint32_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);

    uint32_t val = 0;
    assert_int_equal(pop_back_uint32_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, 2000);
    assert_int_equal((int)uint32_array_size(arr), 1);

    return_uint32_array(arr);
}

// ================================================================================
// Group 8: pop_front_uint32_array
// ================================================================================

static void test_uint32_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_uint32_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);

    uint32_t val = 0;
    assert_int_equal(pop_front_uint32_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, 1000);
    assert_int_equal((int)uint32_array_size(arr), 1);

    return_uint32_array(arr);
}

// ================================================================================
// Group 9: pop_any_uint32_array
// ================================================================================

static void test_uint32_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_uint32_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);
    push_back_uint32_array(arr, 3000);

    uint32_t val = 0;
    assert_int_equal(pop_any_uint32_array(arr, &val, 1), NO_ERROR);
    assert_int_equal((int)val, 2000);
    assert_int_equal((int)uint32_array_size(arr), 2);

    return_uint32_array(arr);
}

// ================================================================================
// Group 10: clear_uint32_array
// ================================================================================

static void test_uint32_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_uint32_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);
    assert_int_equal(clear_uint32_array(arr), NO_ERROR);
    assert_int_equal((int)uint32_array_size(arr), 0);
    assert_int_equal((int)uint32_array_alloc(arr), 4);   /* capacity retained */

    return_uint32_array(arr);
}

// ================================================================================
// Group 11: set_uint32_array_index
// ================================================================================

static void test_uint32_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_uint32_array_index(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000u);
    assert_int_equal(set_uint32_array_index(arr, 0, 4294967295u), NO_ERROR);

    uint32_t val = 0;
    get_uint32_array_index(arr, 0, &val);
    assert_true(val == 4294967295u);

    return_uint32_array(arr);
}

// ================================================================================
// Group 12: copy_uint32_array
// ================================================================================

static void test_uint32_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = copy_uint32_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* src = r.u.value;

    push_back_uint32_array(src, 1000);
    push_back_uint32_array(src, 2000);

    uint32_array_expect_t cr = copy_uint32_array(src, alloc);
    assert_true(cr.has_value);
    uint32_array_t* dst = cr.u.value;

    set_uint32_array_index(src, 0, 999);
    uint32_t val = 0;
    get_uint32_array_index(dst, 0, &val);
    assert_int_equal((int)val, 1000);

    return_uint32_array(src);
    return_uint32_array(dst);
}

// ================================================================================
// Group 13: concat_uint32_array
// ================================================================================

static void test_uint32_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* src = r.u.value;
    assert_int_equal(concat_uint32_array(NULL, src), NULL_POINTER);
    return_uint32_array(src);
}

// --------------------------------------------------------------------------------

static void test_uint32_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r1 = init_uint32_array(8, false, alloc);
    uint32_array_expect_t r2 = init_uint32_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    uint32_array_t* dst = r1.u.value;
    uint32_array_t* src = r2.u.value;

    push_back_uint32_array(dst, 1000);
    push_back_uint32_array(src, 2000);

    assert_int_equal(concat_uint32_array(dst, src), NO_ERROR);
    assert_int_equal((int)uint32_array_size(dst), 2);

    uint32_t val = 0;
    get_uint32_array_index(dst, 1, &val);
    assert_int_equal((int)val, 2000);

    return_uint32_array(dst);
    return_uint32_array(src);
}

// ================================================================================
// Group 14: slice_uint32_array
// ================================================================================

static void test_uint32_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = slice_uint32_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(8, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* src = r.u.value;

    push_back_uint32_array(src, 1000);
    push_back_uint32_array(src, 2000);
    push_back_uint32_array(src, 3000);
    push_back_uint32_array(src, 4000);
    push_back_uint32_array(src, 5000);

    uint32_array_expect_t sr = slice_uint32_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    uint32_array_t* slc = sr.u.value;
    assert_int_equal((int)uint32_array_size(slc), 3);

    uint32_t val = 0;
    get_uint32_array_index(slc, 0, &val);  assert_int_equal((int)val, 2000);
    get_uint32_array_index(slc, 1, &val);  assert_int_equal((int)val, 3000);
    get_uint32_array_index(slc, 2, &val);  assert_int_equal((int)val, 4000);

    return_uint32_array(src);
    return_uint32_array(slc);
}

// ================================================================================
// Group 15: reverse_uint32_array
// ================================================================================

static void test_uint32_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_uint32_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);
    push_back_uint32_array(arr, 3000);

    assert_int_equal(reverse_uint32_array(arr), NO_ERROR);

    uint32_t val = 0;
    get_uint32_array_index(arr, 0, &val);  assert_int_equal((int)val, 3000);
    get_uint32_array_index(arr, 2, &val);  assert_int_equal((int)val, 1000);

    return_uint32_array(arr);
}

// ================================================================================
// Group 16: sort_uint32_array
// ================================================================================

static void test_uint32_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_uint32_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_sort_forward_basic(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(8, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 3000);
    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);

    assert_int_equal(sort_uint32_array(arr, FORWARD), NO_ERROR);

    uint32_t val = 0;
    get_uint32_array_index(arr, 0, &val);  assert_int_equal((int)val, 1000);
    get_uint32_array_index(arr, 1, &val);  assert_int_equal((int)val, 2000);
    get_uint32_array_index(arr, 2, &val);  assert_int_equal((int)val, 3000);

    return_uint32_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint32_sort_comparator_safe_near_max(void** state) {
    (void)state;
    /*
     * A subtract-based comparator (int)(a - b) is broken for uint32_t: the
     * difference (4294967295 - 1) = 4294967294, which overflows int on any
     * platform where int is 32 bits, producing an undefined or wrong result.
     * The three-way comparison (va > vb) - (va < vb) is always correct.
     */
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 4294967295u);
    push_back_uint32_array(arr,          1u);
    push_back_uint32_array(arr, 2147483648u);

    assert_int_equal(sort_uint32_array(arr, FORWARD), NO_ERROR);

    uint32_t val = 0;
    get_uint32_array_index(arr, 0, &val);  assert_true(val ==          1u);
    get_uint32_array_index(arr, 1, &val);  assert_true(val == 2147483648u);
    get_uint32_array_index(arr, 2, &val);  assert_true(val == 4294967295u);

    return_uint32_array(arr);
}

// ================================================================================
// Group 17: uint32_array_contains
// ================================================================================

static void test_uint32_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = uint32_array_contains(NULL, 0, 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_contains_finds_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);
    push_back_uint32_array(arr, 3000);

    size_expect_t sr = uint32_array_contains(arr, 2000, 0, 3);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    return_uint32_array(arr);
}

// ================================================================================
// Group 18: uint32_array_binary_search
// ================================================================================

static void test_uint32_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = uint32_array_binary_search(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_binary_search_finds_value_with_sort(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(8, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 3000);
    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);
    push_back_uint32_array(arr, 4000);

    /* sort == true: arr becomes [1000, 2000, 3000, 4000] */
    size_expect_t sr = uint32_array_binary_search(arr, 3000, true);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 2);

    return_uint32_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint32_binary_search_comparator_safe_near_max(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(8, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    /* Already sorted */
    push_back_uint32_array(arr,          1u);
    push_back_uint32_array(arr, 2147483648u);
    push_back_uint32_array(arr, 4294967294u);
    push_back_uint32_array(arr, 4294967295u);

    size_expect_t sr = uint32_array_binary_search(arr, 4294967295u, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 3);

    sr = uint32_array_binary_search(arr, 1u, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_uint32_array(arr);
}

// ================================================================================
// Group 19: uint32_array_binary_bracket
// ================================================================================

static void test_uint32_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = uint32_array_binary_bracket(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(8, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);
    push_back_uint32_array(arr, 3000);
    push_back_uint32_array(arr, 4000);

    bracket_expect_t br = uint32_array_binary_bracket(arr, 2000, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_uint32_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint32_binary_bracket_comparator_safe_near_max(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(8, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    /* [1, 2147483648u, 4294967295u] — gap between 2147483648u and 4294967295u */
    push_back_uint32_array(arr,          1u);
    push_back_uint32_array(arr, 2147483648u);
    push_back_uint32_array(arr, 4294967295u);

    /* 3000000000u falls in the gap */
    bracket_expect_t br = uint32_array_binary_bracket(arr, 3000000000u, false);
    assert_true(br.has_value);
    /* lower = first element >= 3000000000u = index 2 (value 4294967295u) */
    /* upper = last  element <= 3000000000u = index 1 (value 2147483648u) */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_uint32_array(arr);
}

// ================================================================================
// Group 20: uint32_array_size
// ================================================================================

static void test_uint32_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint32_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_uint32_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    assert_int_equal((int)uint32_array_size(arr), 0);
    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);
    assert_int_equal((int)uint32_array_size(arr), 2);

    return_uint32_array(arr);
}

// ================================================================================
// Group 21: uint32_array_alloc
// ================================================================================

static void test_uint32_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint32_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_uint32_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)uint32_array_alloc(r.u.value), 16);
    return_uint32_array(r.u.value);
}

// ================================================================================
// Group 22: uint32_array_data_size
// ================================================================================

static void test_uint32_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint32_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_uint32_data_size_is_four(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)uint32_array_data_size(r.u.value), 4);
    return_uint32_array(r.u.value);
}

// ================================================================================
// Group 23: is_uint32_array_empty
// ================================================================================

static void test_uint32_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint32_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_uint32_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    assert_true(is_uint32_array_empty(arr));
    push_back_uint32_array(arr, 1000);
    assert_false(is_uint32_array_empty(arr));

    return_uint32_array(arr);
}

// ================================================================================
// Group 24: is_uint32_array_full
// ================================================================================

static void test_uint32_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint32_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_uint32_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(2, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    assert_false(is_uint32_array_full(arr));
    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);
    assert_true(is_uint32_array_full(arr));

    return_uint32_array(arr);
}

// ================================================================================
// Group 25: is_uint32_array_ptr
// ================================================================================

static void test_uint32_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    uint32_t val = 0;
    assert_false(is_uint32_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_uint32_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(8, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* arr = r.u.value;

    push_back_uint32_array(arr, 1000);
    push_back_uint32_array(arr, 2000);
    push_back_uint32_array(arr, 3000);

    const uint32_t* first = (const uint32_t*)arr->base.data;
    const uint32_t* last  = first + 2;
    const uint32_t* spare = first + 3;   /* beyond live region */

    assert_true (is_uint32_array_ptr(arr, first));
    assert_true (is_uint32_array_ptr(arr, last));
    assert_false(is_uint32_array_ptr(arr, spare));
    assert_false(is_uint32_array_ptr(arr, NULL));

    return_uint32_array(arr);
}

// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_uint32_array[] = {

    /* Group 1: init_uint32_array */
    cmocka_unit_test(test_uint32_init_null_allocate_fn_fails),
    cmocka_unit_test(test_uint32_init_returns_valid_array),

    /* Group 2: return_uint32_array */
    cmocka_unit_test(test_uint32_return_null_is_safe),
    cmocka_unit_test(test_uint32_return_valid_array_does_not_crash),

    /* Group 3: push_back_uint32_array */
    cmocka_unit_test(test_uint32_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_push_back_appends_value),

    /* Group 4: push_front_uint32_array */
    cmocka_unit_test(test_uint32_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_push_front_prepends_value),

    /* Group 5: push_at_uint32_array */
    cmocka_unit_test(test_uint32_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_push_at_inserts_at_index),

    /* Group 6: get_uint32_array_index */
    cmocka_unit_test(test_uint32_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_get_returns_correct_value),

    /* Group 7: pop_back_uint32_array */
    cmocka_unit_test(test_uint32_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_pop_back_removes_last_element),

    /* Group 8: pop_front_uint32_array */
    cmocka_unit_test(test_uint32_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_pop_front_removes_first_element),

    /* Group 9: pop_any_uint32_array */
    cmocka_unit_test(test_uint32_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_pop_any_removes_element_at_index),

    /* Group 10: clear_uint32_array */
    cmocka_unit_test(test_uint32_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_clear_resets_len_to_zero),

    /* Group 11: set_uint32_array_index */
    cmocka_unit_test(test_uint32_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_set_overwrites_element),

    /* Group 12: copy_uint32_array */
    cmocka_unit_test(test_uint32_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_uint32_copy_produces_independent_array),

    /* Group 13: concat_uint32_array */
    cmocka_unit_test(test_uint32_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_uint32_concat_appends_elements),

    /* Group 14: slice_uint32_array */
    cmocka_unit_test(test_uint32_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_uint32_slice_returns_correct_subrange),

    /* Group 15: reverse_uint32_array */
    cmocka_unit_test(test_uint32_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_reverse_reverses_elements),

    /* Group 16: sort_uint32_array */
    cmocka_unit_test(test_uint32_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_sort_forward_basic),
    cmocka_unit_test(test_uint32_sort_comparator_safe_near_max),

    /* Group 17: uint32_array_contains */
    cmocka_unit_test(test_uint32_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_contains_finds_value),

    /* Group 18: uint32_array_binary_search */
    cmocka_unit_test(test_uint32_binary_search_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_binary_search_finds_value_with_sort),
    cmocka_unit_test(test_uint32_binary_search_comparator_safe_near_max),

    /* Group 19: uint32_array_binary_bracket */
    cmocka_unit_test(test_uint32_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_binary_bracket_exact_match),
    cmocka_unit_test(test_uint32_binary_bracket_comparator_safe_near_max),

    /* Group 20: uint32_array_size */
    cmocka_unit_test(test_uint32_size_null_returns_zero),
    cmocka_unit_test(test_uint32_size_reflects_push_count),

    /* Group 21: uint32_array_alloc */
    cmocka_unit_test(test_uint32_alloc_null_returns_zero),
    cmocka_unit_test(test_uint32_alloc_matches_capacity),

    /* Group 22: uint32_array_data_size */
    cmocka_unit_test(test_uint32_data_size_null_returns_zero),
    cmocka_unit_test(test_uint32_data_size_is_four),

    /* Group 23: is_uint32_array_empty */
    cmocka_unit_test(test_uint32_empty_null_returns_true),
    cmocka_unit_test(test_uint32_empty_reflects_contents),

    /* Group 24: is_uint32_array_full */
    cmocka_unit_test(test_uint32_full_null_returns_true),
    cmocka_unit_test(test_uint32_full_reflects_capacity),

    /* Group 25: is_uint32_array_ptr */
    cmocka_unit_test(test_uint32_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_uint32_is_ptr_valid_and_invalid),
};
const size_t test_uint32_array_count = sizeof(test_uint32_array) / sizeof(test_uint32_array[0]);
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Group 1: init_int32_array
// ================================================================================

static void test_int32_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    int32_array_expect_t r = init_int32_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)int32_array_size(r.u.value), 0);
    assert_int_equal((int)int32_array_alloc(r.u.value), 8);
    return_int32_array(r.u.value);
}

// ================================================================================
// Group 2: return_int32_array
// ================================================================================

static void test_int32_return_null_is_safe(void** state) {
    (void)state;
    return_int32_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_int32_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    return_int32_array(r.u.value);
}

// ================================================================================
// Group 3: push_back_int32_array
// ================================================================================

static void test_int32_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_int32_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    assert_int_equal(push_back_int32_array(arr, 1000), NO_ERROR);
    assert_int_equal(push_back_int32_array(arr, 2000), NO_ERROR);
    assert_int_equal(push_back_int32_array(arr, 3000), NO_ERROR);
    assert_int_equal((int)int32_array_size(arr), 3);

    int32_t val = 0;
    get_int32_array_index(arr, 0, &val);  assert_int_equal((int)val, 1000);
    get_int32_array_index(arr, 2, &val);  assert_int_equal((int)val, 3000);

    return_int32_array(arr);
}

// ================================================================================
// Group 4: push_front_int32_array
// ================================================================================

static void test_int32_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_int32_array(NULL, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 2000);
    push_front_int32_array(arr, 1000);
    /* arr is [1000, 2000] */

    int32_t val = 0;
    get_int32_array_index(arr, 0, &val);
    assert_int_equal((int)val, 1000);

    return_int32_array(arr);
}

// ================================================================================
// Group 5: push_at_int32_array
// ================================================================================

static void test_int32_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_int32_array(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 3000);
    push_at_int32_array(arr, 1, 2000);
    /* arr is [1000, 2000, 3000] */

    int32_t val = 0;
    get_int32_array_index(arr, 1, &val);
    assert_int_equal((int)val, 2000);

    return_int32_array(arr);
}

// ================================================================================
// Group 6: get_int32_array_index
// ================================================================================

static void test_int32_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    int32_t val = 0;
    assert_int_equal(get_int32_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, INT32_MIN);
    push_back_int32_array(arr,         0);
    push_back_int32_array(arr, INT32_MAX);

    int32_t val = 0;
    assert_int_equal(get_int32_array_index(arr, 0, &val), NO_ERROR);
    assert_true(val == INT32_MIN);
    assert_int_equal(get_int32_array_index(arr, 2, &val), NO_ERROR);
    assert_true(val == INT32_MAX);

    return_int32_array(arr);
}

// ================================================================================
// Group 7: pop_back_int32_array
// ================================================================================

static void test_int32_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_int32_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);

    int32_t val = 0;
    assert_int_equal(pop_back_int32_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, 2000);
    assert_int_equal((int)int32_array_size(arr), 1);

    return_int32_array(arr);
}

// ================================================================================
// Group 8: pop_front_int32_array
// ================================================================================

static void test_int32_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_int32_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);

    int32_t val = 0;
    assert_int_equal(pop_front_int32_array(arr, &val), NO_ERROR);
    assert_int_equal((int)val, 1000);
    assert_int_equal((int)int32_array_size(arr), 1);

    return_int32_array(arr);
}

// ================================================================================
// Group 9: pop_any_int32_array
// ================================================================================

static void test_int32_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_int32_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);
    push_back_int32_array(arr, 3000);

    int32_t val = 0;
    assert_int_equal(pop_any_int32_array(arr, &val, 1), NO_ERROR);
    assert_int_equal((int)val, 2000);
    assert_int_equal((int)int32_array_size(arr), 2);

    return_int32_array(arr);
}

// ================================================================================
// Group 10: clear_int32_array
// ================================================================================

static void test_int32_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_int32_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);
    assert_int_equal(clear_int32_array(arr), NO_ERROR);
    assert_int_equal((int)int32_array_size(arr), 0);
    assert_int_equal((int)int32_array_alloc(arr), 4);   /* capacity retained */

    return_int32_array(arr);
}

// ================================================================================
// Group 11: set_int32_array_index
// ================================================================================

static void test_int32_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_int32_array_index(NULL, 0, 1), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    assert_int_equal(set_int32_array_index(arr, 0, INT32_MIN), NO_ERROR);

    int32_t val = 0;
    get_int32_array_index(arr, 0, &val);
    assert_true(val == INT32_MIN);

    return_int32_array(arr);
}

// ================================================================================
// Group 12: copy_int32_array
// ================================================================================

static void test_int32_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = copy_int32_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* src = r.u.value;

    push_back_int32_array(src, 1000);
    push_back_int32_array(src, 2000);

    int32_array_expect_t cr = copy_int32_array(src, alloc);
    assert_true(cr.has_value);
    int32_array_t* dst = cr.u.value;

    set_int32_array_index(src, 0, 999);
    int32_t val = 0;
    get_int32_array_index(dst, 0, &val);
    assert_int_equal((int)val, 1000);

    return_int32_array(src);
    return_int32_array(dst);
}

// ================================================================================
// Group 13: concat_int32_array
// ================================================================================

static void test_int32_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* src = r.u.value;
    assert_int_equal(concat_int32_array(NULL, src), NULL_POINTER);
    return_int32_array(src);
}

// --------------------------------------------------------------------------------

static void test_int32_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r1 = init_int32_array(8, false, alloc);
    int32_array_expect_t r2 = init_int32_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    int32_array_t* dst = r1.u.value;
    int32_array_t* src = r2.u.value;

    push_back_int32_array(dst, 1000);
    push_back_int32_array(src, 2000);

    assert_int_equal(concat_int32_array(dst, src), NO_ERROR);
    assert_int_equal((int)int32_array_size(dst), 2);

    int32_t val = 0;
    get_int32_array_index(dst, 1, &val);
    assert_int_equal((int)val, 2000);

    return_int32_array(dst);
    return_int32_array(src);
}

// ================================================================================
// Group 14: slice_int32_array
// ================================================================================

static void test_int32_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = slice_int32_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* src = r.u.value;

    push_back_int32_array(src, 1000);
    push_back_int32_array(src, 2000);
    push_back_int32_array(src, 3000);
    push_back_int32_array(src, 4000);
    push_back_int32_array(src, 5000);

    int32_array_expect_t sr = slice_int32_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    int32_array_t* slc = sr.u.value;
    assert_int_equal((int)int32_array_size(slc), 3);

    int32_t val = 0;
    get_int32_array_index(slc, 0, &val);  assert_int_equal((int)val, 2000);
    get_int32_array_index(slc, 1, &val);  assert_int_equal((int)val, 3000);
    get_int32_array_index(slc, 2, &val);  assert_int_equal((int)val, 4000);

    return_int32_array(src);
    return_int32_array(slc);
}

// ================================================================================
// Group 15: reverse_int32_array
// ================================================================================

static void test_int32_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_int32_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);
    push_back_int32_array(arr, 3000);

    assert_int_equal(reverse_int32_array(arr), NO_ERROR);

    int32_t val = 0;
    get_int32_array_index(arr, 0, &val);  assert_int_equal((int)val, 3000);
    get_int32_array_index(arr, 2, &val);  assert_int_equal((int)val, 1000);

    return_int32_array(arr);
}

// ================================================================================
// Group 16: sort_int32_array
// ================================================================================

static void test_int32_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_int32_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_sort_forward_basic(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 3000);
    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);

    assert_int_equal(sort_int32_array(arr, FORWARD), NO_ERROR);

    int32_t val = 0;
    get_int32_array_index(arr, 0, &val);  assert_int_equal((int)val, 1000);
    get_int32_array_index(arr, 1, &val);  assert_int_equal((int)val, 2000);
    get_int32_array_index(arr, 2, &val);  assert_int_equal((int)val, 3000);

    return_int32_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int32_sort_comparator_correct_signed(void** state) {
    (void)state;
    /*
     * A subtract-based comparator overflows for int32_t: the difference
     * INT32_MIN - INT32_MAX = -4294967295, which is outside the range of
     * int32_t and produces undefined behaviour. The three-way comparison
     * (va > vb) - (va < vb) is always correct regardless of magnitude.
     */
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, INT32_MAX);
    push_back_int32_array(arr,         0);
    push_back_int32_array(arr, INT32_MIN);

    assert_int_equal(sort_int32_array(arr, FORWARD), NO_ERROR);

    int32_t val = 0;
    get_int32_array_index(arr, 0, &val);  assert_true(val == INT32_MIN);
    get_int32_array_index(arr, 1, &val);  assert_true(val == 0);
    get_int32_array_index(arr, 2, &val);  assert_true(val == INT32_MAX);

    return_int32_array(arr);
}

// ================================================================================
// Group 17: int32_array_contains
// ================================================================================

static void test_int32_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = int32_array_contains(NULL, 0, 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_contains_finds_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);
    push_back_int32_array(arr, 3000);

    size_expect_t sr = int32_array_contains(arr, 2000, 0, 3);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    return_int32_array(arr);
}

// ================================================================================
// Group 18: int32_array_binary_search
// ================================================================================

static void test_int32_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = int32_array_binary_search(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_binary_search_finds_value_with_sort(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 3000);
    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);
    push_back_int32_array(arr, 4000);

    /* sort == true: arr becomes [1000, 2000, 3000, 4000] */
    size_expect_t sr = int32_array_binary_search(arr, 3000, true);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 2);

    return_int32_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int32_binary_search_comparator_correct_signed(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    /* Already sorted across the full signed range */
    push_back_int32_array(arr, INT32_MIN);
    push_back_int32_array(arr,        -1);
    push_back_int32_array(arr,         0);
    push_back_int32_array(arr, INT32_MAX);

    size_expect_t sr = int32_array_binary_search(arr, INT32_MIN, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    sr = int32_array_binary_search(arr, INT32_MAX, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 3);

    return_int32_array(arr);
}

// ================================================================================
// Group 19: int32_array_binary_bracket
// ================================================================================

static void test_int32_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = int32_array_binary_bracket(NULL, 0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);
    push_back_int32_array(arr, 3000);
    push_back_int32_array(arr, 4000);

    bracket_expect_t br = int32_array_binary_bracket(arr, 2000, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_int32_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int32_binary_bracket_comparator_correct_signed(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    /* [INT32_MIN, -1000, 1000, INT32_MAX] — gap between -1000 and 1000 */
    push_back_int32_array(arr, INT32_MIN);
    push_back_int32_array(arr,    -1000);
    push_back_int32_array(arr,     1000);
    push_back_int32_array(arr, INT32_MAX);

    /* 0 falls in the gap */
    bracket_expect_t br = int32_array_binary_bracket(arr, 0, false);
    assert_true(br.has_value);
    /* lower = first element >= 0 = index 2 (value  1000) */
    /* upper = last  element <= 0 = index 1 (value -1000) */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_int32_array(arr);
}

// ================================================================================
// Group 20: int32_array_size
// ================================================================================

static void test_int32_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int32_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int32_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    assert_int_equal((int)int32_array_size(arr), 0);
    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);
    assert_int_equal((int)int32_array_size(arr), 2);

    return_int32_array(arr);
}

// ================================================================================
// Group 21: int32_array_alloc
// ================================================================================

static void test_int32_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int32_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int32_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)int32_array_alloc(r.u.value), 16);
    return_int32_array(r.u.value);
}

// ================================================================================
// Group 22: int32_array_data_size
// ================================================================================

static void test_int32_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int32_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int32_data_size_is_four(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)int32_array_data_size(r.u.value), 4);
    return_int32_array(r.u.value);
}

// ================================================================================
// Group 23: is_int32_array_empty
// ================================================================================

static void test_int32_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int32_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_int32_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(4, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    assert_true(is_int32_array_empty(arr));
    push_back_int32_array(arr, 1000);
    assert_false(is_int32_array_empty(arr));

    return_int32_array(arr);
}

// ================================================================================
// Group 24: is_int32_array_full
// ================================================================================

static void test_int32_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int32_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_int32_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(2, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    assert_false(is_int32_array_full(arr));
    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);
    assert_true(is_int32_array_full(arr));

    return_int32_array(arr);
}

// ================================================================================
// Group 25: is_int32_array_ptr
// ================================================================================

static void test_int32_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    int32_t val = 0;
    assert_false(is_int32_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_int32_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 1000);
    push_back_int32_array(arr, 2000);
    push_back_int32_array(arr, 3000);

    const int32_t* first = (const int32_t*)arr->base.data;
    const int32_t* last  = first + 2;
    const int32_t* spare = first + 3;   /* beyond live region */

    assert_true (is_int32_array_ptr(arr, first));
    assert_true (is_int32_array_ptr(arr, last));
    assert_false(is_int32_array_ptr(arr, spare));
    assert_false(is_int32_array_ptr(arr, NULL));

    return_int32_array(arr);
}

// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_int32_array[] = {

    /* Group 1: init_int32_array */
    cmocka_unit_test(test_int32_init_null_allocate_fn_fails),
    cmocka_unit_test(test_int32_init_returns_valid_array),

    /* Group 2: return_int32_array */
    cmocka_unit_test(test_int32_return_null_is_safe),
    cmocka_unit_test(test_int32_return_valid_array_does_not_crash),

    /* Group 3: push_back_int32_array */
    cmocka_unit_test(test_int32_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_push_back_appends_value),

    /* Group 4: push_front_int32_array */
    cmocka_unit_test(test_int32_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_push_front_prepends_value),

    /* Group 5: push_at_int32_array */
    cmocka_unit_test(test_int32_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_push_at_inserts_at_index),

    /* Group 6: get_int32_array_index */
    cmocka_unit_test(test_int32_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_get_returns_correct_value),

    /* Group 7: pop_back_int32_array */
    cmocka_unit_test(test_int32_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_pop_back_removes_last_element),

    /* Group 8: pop_front_int32_array */
    cmocka_unit_test(test_int32_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_pop_front_removes_first_element),

    /* Group 9: pop_any_int32_array */
    cmocka_unit_test(test_int32_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_pop_any_removes_element_at_index),

    /* Group 10: clear_int32_array */
    cmocka_unit_test(test_int32_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_clear_resets_len_to_zero),

    /* Group 11: set_int32_array_index */
    cmocka_unit_test(test_int32_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_set_overwrites_element),

    /* Group 12: copy_int32_array */
    cmocka_unit_test(test_int32_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_int32_copy_produces_independent_array),

    /* Group 13: concat_int32_array */
    cmocka_unit_test(test_int32_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_int32_concat_appends_elements),

    /* Group 14: slice_int32_array */
    cmocka_unit_test(test_int32_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_int32_slice_returns_correct_subrange),

    /* Group 15: reverse_int32_array */
    cmocka_unit_test(test_int32_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_reverse_reverses_elements),

    /* Group 16: sort_int32_array */
    cmocka_unit_test(test_int32_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_sort_forward_basic),
    cmocka_unit_test(test_int32_sort_comparator_correct_signed),

    /* Group 17: int32_array_contains */
    cmocka_unit_test(test_int32_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_contains_finds_value),

    /* Group 18: int32_array_binary_search */
    cmocka_unit_test(test_int32_binary_search_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_binary_search_finds_value_with_sort),
    cmocka_unit_test(test_int32_binary_search_comparator_correct_signed),

    /* Group 19: int32_array_binary_bracket */
    cmocka_unit_test(test_int32_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_binary_bracket_exact_match),
    cmocka_unit_test(test_int32_binary_bracket_comparator_correct_signed),

    /* Group 20: int32_array_size */
    cmocka_unit_test(test_int32_size_null_returns_zero),
    cmocka_unit_test(test_int32_size_reflects_push_count),

    /* Group 21: int32_array_alloc */
    cmocka_unit_test(test_int32_alloc_null_returns_zero),
    cmocka_unit_test(test_int32_alloc_matches_capacity),

    /* Group 22: int32_array_data_size */
    cmocka_unit_test(test_int32_data_size_null_returns_zero),
    cmocka_unit_test(test_int32_data_size_is_four),

    /* Group 23: is_int32_array_empty */
    cmocka_unit_test(test_int32_empty_null_returns_true),
    cmocka_unit_test(test_int32_empty_reflects_contents),

    /* Group 24: is_int32_array_full */
    cmocka_unit_test(test_int32_full_null_returns_true),
    cmocka_unit_test(test_int32_full_reflects_capacity),

    /* Group 25: is_int32_array_ptr */
    cmocka_unit_test(test_int32_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_int32_is_ptr_valid_and_invalid),
};
const size_t test_int32_array_count = sizeof(test_int32_array) / sizeof(test_int32_array[0]);
// ================================================================================
// ================================================================================
// eof
