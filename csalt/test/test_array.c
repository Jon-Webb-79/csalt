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
#include <math.h>

#include "c_allocator.h"
#include "c_dtypes.h"
#include "c_uint8.h"
#include "c_int8.h"
#include "c_uint16.h"
#include "c_int16.h"
#include "c_uint32.h"
#include "c_int32.h"
#include "c_uint64.h"
#include "c_int64.h"
#include "c_float.h"
#include "c_double.h"
#include "c_ldouble.h"
#include "c_string.h"
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
// -------------------------------------------------------------------------------- 

static uint8_array_t* _make_uint8_array(size_t capacity, bool growth) {
    allocator_vtable_t a = heap_allocator();
    uint8_array_expect_t r = init_uint8_array(capacity, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// --------------------------------------------------------------------------------

static uint8_array_t* _make_sample_uint8_array(void) {
    uint8_array_t* array = _make_uint8_array(8u, true);

    assert_int_equal(push_back_uint8_array(array, 1u), NO_ERROR);
    assert_int_equal(push_back_uint8_array(array, 2u), NO_ERROR);
    assert_int_equal(push_back_uint8_array(array, 3u), NO_ERROR);
    assert_int_equal(push_back_uint8_array(array, 4u), NO_ERROR);
    assert_int_equal(push_back_uint8_array(array, 5u), NO_ERROR);

    return array;
}
// -------------------------------------------------------------------------------- 

static void test_uint8_add_scalar_array_null_returns_null_pointer(void** state) {
    (void)state;

    assert_int_equal(uint8_add_scalar_array(NULL, 5u), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint8_add_scalar_array_empty_returns_empty(void** state) {
    (void)state;

    uint8_array_t* array = _make_uint8_array(4u, true);

    assert_int_equal(uint8_add_scalar_array(array, 5u), EMPTY);

    return_uint8_array(array);
}

// --------------------------------------------------------------------------------

static void test_uint8_add_scalar_array_add_zero_leaves_values_unchanged(void** state) {
    (void)state;

    uint8_array_t* array = _make_sample_uint8_array();

    assert_int_equal(uint8_add_scalar_array(array, 0u), NO_ERROR);

    uint8_t out = 0u;

    assert_int_equal(get_uint8_array_index(array, 0u, &out), NO_ERROR);
    assert_int_equal(out, 1u);

    assert_int_equal(get_uint8_array_index(array, 1u, &out), NO_ERROR);
    assert_int_equal(out, 2u);

    assert_int_equal(get_uint8_array_index(array, 2u, &out), NO_ERROR);
    assert_int_equal(out, 3u);

    assert_int_equal(get_uint8_array_index(array, 3u, &out), NO_ERROR);
    assert_int_equal(out, 4u);

    assert_int_equal(get_uint8_array_index(array, 4u, &out), NO_ERROR);
    assert_int_equal(out, 5u);

    return_uint8_array(array);
}

// --------------------------------------------------------------------------------

static void test_uint8_add_scalar_array_single_element_updates_value(void** state) {
    (void)state;

    uint8_array_t* array = _make_uint8_array(4u, true);
    assert_int_equal(push_back_uint8_array(array, 10u), NO_ERROR);

    assert_int_equal(uint8_add_scalar_array(array, 7u), NO_ERROR);

    uint8_t out = 0u;
    assert_int_equal(get_uint8_array_index(array, 0u, &out), NO_ERROR);
    assert_int_equal(out, 17u);

    return_uint8_array(array);
}

// --------------------------------------------------------------------------------

static void test_uint8_add_scalar_array_updates_all_elements(void** state) {
    (void)state;

    uint8_array_t* array = _make_sample_uint8_array();

    assert_int_equal(uint8_add_scalar_array(array, 10u), NO_ERROR);

    uint8_t out = 0u;

    assert_int_equal(get_uint8_array_index(array, 0u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    assert_int_equal(get_uint8_array_index(array, 1u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    assert_int_equal(get_uint8_array_index(array, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    assert_int_equal(get_uint8_array_index(array, 3u, &out), NO_ERROR);
    assert_int_equal(out, 14u);

    assert_int_equal(get_uint8_array_index(array, 4u, &out), NO_ERROR);
    assert_int_equal(out, 15u);

    return_uint8_array(array);
}

// --------------------------------------------------------------------------------

static void test_uint8_add_scalar_array_preserves_length(void** state) {
    (void)state;

    uint8_array_t* array = _make_sample_uint8_array();
    size_t before = uint8_array_size(array);

    assert_int_equal(uint8_add_scalar_array(array, 3u), NO_ERROR);

    assert_int_equal(uint8_array_size(array), before);

    return_uint8_array(array);
}

// --------------------------------------------------------------------------------

static void test_uint8_add_scalar_array_multiple_calls_accumulate(void** state) {
    (void)state;

    uint8_array_t* array = _make_sample_uint8_array();

    assert_int_equal(uint8_add_scalar_array(array, 2u), NO_ERROR);
    assert_int_equal(uint8_add_scalar_array(array, 3u), NO_ERROR);

    uint8_t out = 0u;

    assert_int_equal(get_uint8_array_index(array, 0u, &out), NO_ERROR);
    assert_int_equal(out, 6u);

    assert_int_equal(get_uint8_array_index(array, 1u, &out), NO_ERROR);
    assert_int_equal(out, 7u);

    assert_int_equal(get_uint8_array_index(array, 2u, &out), NO_ERROR);
    assert_int_equal(out, 8u);

    assert_int_equal(get_uint8_array_index(array, 3u, &out), NO_ERROR);
    assert_int_equal(out, 9u);

    assert_int_equal(get_uint8_array_index(array, 4u, &out), NO_ERROR);
    assert_int_equal(out, 10u);

    return_uint8_array(array);
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

    cmocka_unit_test(test_uint8_add_scalar_array_null_returns_null_pointer),
    cmocka_unit_test(test_uint8_add_scalar_array_empty_returns_empty),
    cmocka_unit_test(test_uint8_add_scalar_array_add_zero_leaves_values_unchanged),
    cmocka_unit_test(test_uint8_add_scalar_array_single_element_updates_value),
    cmocka_unit_test(test_uint8_add_scalar_array_updates_all_elements),
    cmocka_unit_test(test_uint8_add_scalar_array_preserves_length),
    cmocka_unit_test(test_uint8_add_scalar_array_multiple_calls_accumulate),
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
// -------------------------------------------------------------------------------- 

static int8_array_t* _make_int8_array(size_t capacity, bool growth) {
    allocator_vtable_t a = heap_allocator();
    int8_array_expect_t r = init_int8_array(capacity, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// --------------------------------------------------------------------------------

static int8_array_t* _make_sample_int8_array(void) {
    int8_array_t* array = _make_int8_array(8u, true);

    assert_int_equal(push_back_int8_array(array, 1u), NO_ERROR);
    assert_int_equal(push_back_int8_array(array, 2u), NO_ERROR);
    assert_int_equal(push_back_int8_array(array, 3u), NO_ERROR);
    assert_int_equal(push_back_int8_array(array, 4u), NO_ERROR);
    assert_int_equal(push_back_int8_array(array, 5u), NO_ERROR);

    return array;
}
// -------------------------------------------------------------------------------- 

static void test_int8_add_scalar_array_null_returns_null_pointer(void** state) {
    (void)state;

    assert_int_equal(int8_add_scalar_array(NULL, 5u), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_add_scalar_array_empty_returns_empty(void** state) {
    (void)state;

    int8_array_t* array = _make_int8_array(4u, true);

    assert_int_equal(int8_add_scalar_array(array, 5u), EMPTY);

    return_int8_array(array);
}

// --------------------------------------------------------------------------------

static void test_int8_add_scalar_array_add_zero_leaves_values_unchanged(void** state) {
    (void)state;

    int8_array_t* array = _make_sample_int8_array();

    assert_int_equal(int8_add_scalar_array(array, 0u), NO_ERROR);

    int8_t out = 0u;

    assert_int_equal(get_int8_array_index(array, 0u, &out), NO_ERROR);
    assert_int_equal(out, 1u);

    assert_int_equal(get_int8_array_index(array, 1u, &out), NO_ERROR);
    assert_int_equal(out, 2u);

    assert_int_equal(get_int8_array_index(array, 2u, &out), NO_ERROR);
    assert_int_equal(out, 3u);

    assert_int_equal(get_int8_array_index(array, 3u, &out), NO_ERROR);
    assert_int_equal(out, 4u);

    assert_int_equal(get_int8_array_index(array, 4u, &out), NO_ERROR);
    assert_int_equal(out, 5u);

    return_int8_array(array);
}

// --------------------------------------------------------------------------------

static void test_int8_add_scalar_array_single_element_updates_value(void** state) {
    (void)state;

    int8_array_t* array = _make_int8_array(4u, true);
    assert_int_equal(push_back_int8_array(array, 10u), NO_ERROR);

    assert_int_equal(int8_add_scalar_array(array, 7u), NO_ERROR);

    int8_t out = 0u;
    assert_int_equal(get_int8_array_index(array, 0u, &out), NO_ERROR);
    assert_int_equal(out, 17u);

    return_int8_array(array);
}

// --------------------------------------------------------------------------------

static void test_int8_add_scalar_array_updates_all_elements(void** state) {
    (void)state;

    int8_array_t* array = _make_sample_int8_array();

    assert_int_equal(int8_add_scalar_array(array, 10u), NO_ERROR);

    int8_t out = 0u;

    assert_int_equal(get_int8_array_index(array, 0u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    assert_int_equal(get_int8_array_index(array, 1u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    assert_int_equal(get_int8_array_index(array, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    assert_int_equal(get_int8_array_index(array, 3u, &out), NO_ERROR);
    assert_int_equal(out, 14u);

    assert_int_equal(get_int8_array_index(array, 4u, &out), NO_ERROR);
    assert_int_equal(out, 15u);

    return_int8_array(array);
}

// --------------------------------------------------------------------------------

static void test_int8_add_scalar_array_preserves_length(void** state) {
    (void)state;

    int8_array_t* array = _make_sample_int8_array();
    size_t before = int8_array_size(array);

    assert_int_equal(int8_add_scalar_array(array, 3u), NO_ERROR);

    assert_int_equal(int8_array_size(array), before);

    return_int8_array(array);
}

// --------------------------------------------------------------------------------

static void test_int8_add_scalar_array_multiple_calls_accumulate(void** state) {
    (void)state;

    int8_array_t* array = _make_sample_int8_array();

    assert_int_equal(int8_add_scalar_array(array, 2u), NO_ERROR);
    assert_int_equal(int8_add_scalar_array(array, 3u), NO_ERROR);

    int8_t out = 0u;

    assert_int_equal(get_int8_array_index(array, 0u, &out), NO_ERROR);
    assert_int_equal(out, 6u);

    assert_int_equal(get_int8_array_index(array, 1u, &out), NO_ERROR);
    assert_int_equal(out, 7u);

    assert_int_equal(get_int8_array_index(array, 2u, &out), NO_ERROR);
    assert_int_equal(out, 8u);

    assert_int_equal(get_int8_array_index(array, 3u, &out), NO_ERROR);
    assert_int_equal(out, 9u);

    assert_int_equal(get_int8_array_index(array, 4u, &out), NO_ERROR);
    assert_int_equal(out, 10u);

    return_int8_array(array);
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

    cmocka_unit_test(test_int8_add_scalar_array_null_returns_null_pointer),
    cmocka_unit_test(test_int8_add_scalar_array_empty_returns_empty),
    cmocka_unit_test(test_int8_add_scalar_array_add_zero_leaves_values_unchanged),
    cmocka_unit_test(test_int8_add_scalar_array_single_element_updates_value),
    cmocka_unit_test(test_int8_add_scalar_array_updates_all_elements),
    cmocka_unit_test(test_int8_add_scalar_array_preserves_length),
    cmocka_unit_test(test_int8_add_scalar_array_multiple_calls_accumulate),
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
// -------------------------------------------------------------------------------- 

// static uint16_array_t* _make_uint16_array(size_t capacity, bool growth) {
//     allocator_vtable_t a = heap_allocator();
//     uint16_array_expect_t r = init_uint16_array(capacity, growth, a);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     return r.u.value;
// }
//
// // --------------------------------------------------------------------------------
//
// static uint16_array_t* _make_sample_uint16_array(void) {
//     uint16_array_t* array = _make_uint16_array(8u, true);
//
//     assert_int_equal(push_back_uint16_array(array, 1u), NO_ERROR);
//     assert_int_equal(push_back_uint16_array(array, 2u), NO_ERROR);
//     assert_int_equal(push_back_uint16_array(array, 3u), NO_ERROR);
//     assert_int_equal(push_back_uint16_array(array, 4u), NO_ERROR);
//     assert_int_equal(push_back_uint16_array(array, 5u), NO_ERROR);
//
//     return array;
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_uint16_add_scalar_array_null_returns_null_pointer(void** state) {
//     (void)state;
//
//     assert_int_equal(uint16_add_scalar_array(NULL, 5u), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint16_add_scalar_array_empty_returns_empty(void** state) {
//     (void)state;
//
//     uint16_array_t* array = _make_uint16_array(4u, true);
//
//     assert_int_equal(uint16_add_scalar_array(array, 5u), EMPTY);
//
//     return_uint16_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint16_add_scalar_array_add_zero_leaves_values_unchanged(void** state) {
//     (void)state;
//
//     uint16_array_t* array = _make_sample_uint16_array();
//
//     assert_int_equal(uint16_add_scalar_array(array, 0u), NO_ERROR);
//
//     uint16_t out = 0u;
//
//     assert_int_equal(get_uint16_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 1u);
//
//     assert_int_equal(get_uint16_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 2u);
//
//     assert_int_equal(get_uint16_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 3u);
//
//     assert_int_equal(get_uint16_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 4u);
//
//     assert_int_equal(get_uint16_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 5u);
//
//     return_uint16_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint16_add_scalar_array_single_element_updates_value(void** state) {
//     (void)state;
//
//     uint16_array_t* array = _make_uint16_array(4u, true);
//     assert_int_equal(push_back_uint16_array(array, 10u), NO_ERROR);
//
//     assert_int_equal(uint16_add_scalar_array(array, 7u), NO_ERROR);
//
//     uint16_t out = 0u;
//     assert_int_equal(get_uint16_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 17u);
//
//     return_uint16_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint16_add_scalar_array_updates_all_elements(void** state) {
//     (void)state;
//
//     uint16_array_t* array = _make_sample_uint16_array();
//
//     assert_int_equal(uint16_add_scalar_array(array, 10u), NO_ERROR);
//
//     uint16_t out = 0u;
//
//     assert_int_equal(get_uint16_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 11u);
//
//     assert_int_equal(get_uint16_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 12u);
//
//     assert_int_equal(get_uint16_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 13u);
//
//     assert_int_equal(get_uint16_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 14u);
//
//     assert_int_equal(get_uint16_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 15u);
//
//     return_uint16_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint16_add_scalar_array_preserves_length(void** state) {
//     (void)state;
//
//     uint16_array_t* array = _make_sample_uint16_array();
//     size_t before = uint16_array_size(array);
//
//     assert_int_equal(uint16_add_scalar_array(array, 3u), NO_ERROR);
//
//     assert_int_equal(uint16_array_size(array), before);
//
//     return_uint16_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint16_add_scalar_array_multiple_calls_accumulate(void** state) {
//     (void)state;
//
//     uint16_array_t* array = _make_sample_uint16_array();
//
//     assert_int_equal(uint16_add_scalar_array(array, 2u), NO_ERROR);
//     assert_int_equal(uint16_add_scalar_array(array, 3u), NO_ERROR);
//
//     uint16_t out = 0u;
//
//     assert_int_equal(get_uint16_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 6u);
//
//     assert_int_equal(get_uint16_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 7u);
//
//     assert_int_equal(get_uint16_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 8u);
//
//     assert_int_equal(get_uint16_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 9u);
//
//     assert_int_equal(get_uint16_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 10u);
//
//     return_uint16_array(array);
// }
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

    // cmocka_unit_test(test_uint16_add_scalar_array_null_returns_null_pointer),
    // cmocka_unit_test(test_uint16_add_scalar_array_empty_returns_empty),
    // cmocka_unit_test(test_uint16_add_scalar_array_add_zero_leaves_values_unchanged),
    // cmocka_unit_test(test_uint16_add_scalar_array_single_element_updates_value),
    // cmocka_unit_test(test_uint16_add_scalar_array_updates_all_elements),
    // cmocka_unit_test(test_uint16_add_scalar_array_preserves_length),
    // cmocka_unit_test(test_uint16_add_scalar_array_multiple_calls_accumulate),
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
// -------------------------------------------------------------------------------- 

// static int16_array_t* _make_int16_array(size_t capacity, bool growth) {
//     allocator_vtable_t a = heap_allocator();
//     int16_array_expect_t r = init_int16_array(capacity, growth, a);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     return r.u.value;
// }
//
// // --------------------------------------------------------------------------------
//
// static int16_array_t* _make_sample_int16_array(void) {
//     int16_array_t* array = _make_int16_array(8u, true);
//
//     assert_int_equal(push_back_int16_array(array, 1u), NO_ERROR);
//     assert_int_equal(push_back_int16_array(array, 2u), NO_ERROR);
//     assert_int_equal(push_back_int16_array(array, 3u), NO_ERROR);
//     assert_int_equal(push_back_int16_array(array, 4u), NO_ERROR);
//     assert_int_equal(push_back_int16_array(array, 5u), NO_ERROR);
//
//     return array;
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_int16_add_scalar_array_null_returns_null_pointer(void** state) {
//     (void)state;
//
//     assert_int_equal(int16_add_scalar_array(NULL, 5u), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int16_add_scalar_array_empty_returns_empty(void** state) {
//     (void)state;
//
//     int16_array_t* array = _make_int16_array(4u, true);
//
//     assert_int_equal(int16_add_scalar_array(array, 5u), EMPTY);
//
//     return_int16_array(array);
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_int16_add_scalar_array_add_zero_leaves_values_unchanged(void** state) {
//     (void)state;
//
//     int16_array_t* array = _make_sample_int16_array();
//
//     assert_int_equal(int16_add_scalar_array(array, 0u), NO_ERROR);
//
//     int16_t out = 0u;
//
//     assert_int_equal(get_int16_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 1u);
//
//     assert_int_equal(get_int16_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 2u);
//
//     assert_int_equal(get_int16_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 3u);
//
//     assert_int_equal(get_int16_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 4u);
//
//     assert_int_equal(get_int16_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 5u);
//
//     return_int16_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int16_add_scalar_array_single_element_updates_value(void** state) {
//     (void)state;
//
//     int16_array_t* array = _make_int16_array(4u, true);
//     assert_int_equal(push_back_int16_array(array, 10u), NO_ERROR);
//
//     assert_int_equal(int16_add_scalar_array(array, 7u), NO_ERROR);
//
//     int16_t out = 0u;
//     assert_int_equal(get_int16_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 17u);
//
//     return_int16_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int16_add_scalar_array_updates_all_elements(void** state) {
//     (void)state;
//
//     int16_array_t* array = _make_sample_int16_array();
//
//     assert_int_equal(int16_add_scalar_array(array, 10u), NO_ERROR);
//
//     int16_t out = 0u;
//
//     assert_int_equal(get_int16_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 11u);
//
//     assert_int_equal(get_int16_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 12u);
//
//     assert_int_equal(get_int16_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 13u);
//
//     assert_int_equal(get_int16_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 14u);
//
//     assert_int_equal(get_int16_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 15u);
//
//     return_int16_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int16_add_scalar_array_preserves_length(void** state) {
//     (void)state;
//
//     int16_array_t* array = _make_sample_int16_array();
//     size_t before = int16_array_size(array);
//
//     assert_int_equal(int16_add_scalar_array(array, 3u), NO_ERROR);
//
//     assert_int_equal(int16_array_size(array), before);
//
//     return_int16_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int16_add_scalar_array_multiple_calls_accumulate(void** state) {
//     (void)state;
//
//     int16_array_t* array = _make_sample_int16_array();
//
//     assert_int_equal(int16_add_scalar_array(array, 2u), NO_ERROR);
//     assert_int_equal(int16_add_scalar_array(array, 3u), NO_ERROR);
//
//     int16_t out = 0u;
//
//     assert_int_equal(get_int16_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 6u);
//
//     assert_int_equal(get_int16_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 7u);
//
//     assert_int_equal(get_int16_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 8u);
//
//     assert_int_equal(get_int16_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 9u);
//
//     assert_int_equal(get_int16_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 10u);
//
//     return_int16_array(array);
// }
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

    // cmocka_unit_test(test_int16_add_scalar_array_null_returns_null_pointer),
    // cmocka_unit_test(test_int16_add_scalar_array_empty_returns_empty),
    // cmocka_unit_test(test_int16_add_scalar_array_add_zero_leaves_values_unchanged),
    // cmocka_unit_test(test_int16_add_scalar_array_single_element_updates_value),
    // cmocka_unit_test(test_int16_add_scalar_array_updates_all_elements),
    // cmocka_unit_test(test_int16_add_scalar_array_preserves_length),
    // cmocka_unit_test(test_int16_add_scalar_array_multiple_calls_accumulate),

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
// -------------------------------------------------------------------------------- 

static void test_uint32_cumulative_null_array_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = cumulative_uint32_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint32_cumulative_produces_prefix_sum(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(8, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* src = r.u.value;

    push_back_uint32_array(src, 1000u);
    push_back_uint32_array(src, 2000u);
    push_back_uint32_array(src, 3000u);
    push_back_uint32_array(src, 4000u);
    /* src: [1000, 2000, 3000, 4000]
     * expected output: [1000, 3000, 6000, 10000] */

    uint32_array_expect_t cr = cumulative_uint32_array(src, alloc);
    assert_true(cr.has_value);
    uint32_array_t* dst = cr.u.value;
    assert_int_equal((int)uint32_array_size(dst), 4);

    uint32_t val = 0;
    get_uint32_array_index(dst, 0, &val);  assert_true(val ==  1000u);
    get_uint32_array_index(dst, 1, &val);  assert_true(val ==  3000u);
    get_uint32_array_index(dst, 2, &val);  assert_true(val ==  6000u);
    get_uint32_array_index(dst, 3, &val);  assert_true(val == 10000u);

    return_uint32_array(src);
    return_uint32_array(dst);
}

// --------------------------------------------------------------------------------

static void test_uint32_cumulative_single_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_array_expect_t r = init_uint32_array(4, false, alloc);
    assert_true(r.has_value);
    uint32_array_t* src = r.u.value;

    push_back_uint32_array(src, 42000u);
    /* src: [42000] — output must equal input verbatim */

    uint32_array_expect_t cr = cumulative_uint32_array(src, alloc);
    assert_true(cr.has_value);
    uint32_array_t* dst = cr.u.value;
    assert_int_equal((int)uint32_array_size(dst), 1);

    uint32_t val = 0;
    get_uint32_array_index(dst, 0, &val);  assert_true(val == 42000u);

    return_uint32_array(src);
    return_uint32_array(dst);
}
// -------------------------------------------------------------------------------- 

// static uint32_array_t* _make_uint32_array(size_t capacity, bool growth) {
//     allocator_vtable_t a = heap_allocator();
//     uint32_array_expect_t r = init_uint32_array(capacity, growth, a);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     return r.u.value;
// }
//
// // --------------------------------------------------------------------------------
//
// static uint32_array_t* _make_sample_uint32_array(void) {
//     uint32_array_t* array = _make_uint32_array(8u, true);
//
//     assert_int_equal(push_back_uint32_array(array, 1u), NO_ERROR);
//     assert_int_equal(push_back_uint32_array(array, 2u), NO_ERROR);
//     assert_int_equal(push_back_uint32_array(array, 3u), NO_ERROR);
//     assert_int_equal(push_back_uint32_array(array, 4u), NO_ERROR);
//     assert_int_equal(push_back_uint32_array(array, 5u), NO_ERROR);
//
//     return array;
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_uint32_add_scalar_array_null_returns_null_pointer(void** state) {
//     (void)state;
//
//     assert_int_equal(uint32_add_scalar_array(NULL, 5u), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint32_add_scalar_array_empty_returns_empty(void** state) {
//     (void)state;
//
//     uint32_array_t* array = _make_uint32_array(4u, true);
//
//     assert_int_equal(uint32_add_scalar_array(array, 5u), EMPTY);
//
//     return_uint32_array(array);
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_uint32_add_scalar_array_add_zero_leaves_values_unchanged(void** state) {
//     (void)state;
//
//     uint32_array_t* array = _make_sample_uint32_array();
//
//     assert_int_equal(uint32_add_scalar_array(array, 0u), NO_ERROR);
//
//     uint32_t out = 0u;
//
//     assert_int_equal(get_uint32_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 1u);
//
//     assert_int_equal(get_uint32_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 2u);
//
//     assert_int_equal(get_uint32_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 3u);
//
//     assert_int_equal(get_uint32_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 4u);
//
//     assert_int_equal(get_uint32_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 5u);
//
//     return_uint32_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint32_add_scalar_array_single_element_updates_value(void** state) {
//     (void)state;
//
//     uint32_array_t* array = _make_uint32_array(4u, true);
//     assert_int_equal(push_back_uint32_array(array, 10u), NO_ERROR);
//
//     assert_int_equal(uint32_add_scalar_array(array, 7u), NO_ERROR);
//
//     uint32_t out = 0u;
//     assert_int_equal(get_uint32_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 17u);
//
//     return_uint32_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint32_add_scalar_array_updates_all_elements(void** state) {
//     (void)state;
//
//     uint32_array_t* array = _make_sample_uint32_array();
//
//     assert_int_equal(uint32_add_scalar_array(array, 10u), NO_ERROR);
//
//     uint32_t out = 0u;
//
//     assert_int_equal(get_uint32_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 11u);
//
//     assert_int_equal(get_uint32_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 12u);
//
//     assert_int_equal(get_uint32_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 13u);
//
//     assert_int_equal(get_uint32_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 14u);
//
//     assert_int_equal(get_uint32_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 15u);
//
//     return_uint32_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint32_add_scalar_array_preserves_length(void** state) {
//     (void)state;
//
//     uint32_array_t* array = _make_sample_uint32_array();
//     size_t before = uint32_array_size(array);
//
//     assert_int_equal(uint32_add_scalar_array(array, 3u), NO_ERROR);
//
//     assert_int_equal(uint32_array_size(array), before);
//
//     return_uint32_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint32_add_scalar_array_multiple_calls_accumulate(void** state) {
//     (void)state;
//
//     uint32_array_t* array = _make_sample_uint32_array();
//
//     assert_int_equal(uint32_add_scalar_array(array, 2u), NO_ERROR);
//     assert_int_equal(uint32_add_scalar_array(array, 3u), NO_ERROR);
//
//     uint32_t out = 0u;
//
//     assert_int_equal(get_uint32_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 6u);
//
//     assert_int_equal(get_uint32_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 7u);
//
//     assert_int_equal(get_uint32_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 8u);
//
//     assert_int_equal(get_uint32_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 9u);
//
//     assert_int_equal(get_uint32_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 10u);
//
//     return_uint32_array(array);
// }
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

    /* Group 26: cumulative_uint32_array */
    cmocka_unit_test(test_uint32_cumulative_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint32_cumulative_produces_prefix_sum),
    cmocka_unit_test(test_uint32_cumulative_single_element),

    // cmocka_unit_test(test_uint32_add_scalar_array_null_returns_null_pointer),
    // cmocka_unit_test(test_uint32_add_scalar_array_empty_returns_empty),
    // cmocka_unit_test(test_uint32_add_scalar_array_add_zero_leaves_values_unchanged),
    // cmocka_unit_test(test_uint32_add_scalar_array_single_element_updates_value),
    // cmocka_unit_test(test_uint32_add_scalar_array_updates_all_elements),
    // cmocka_unit_test(test_uint32_add_scalar_array_preserves_length),
    // cmocka_unit_test(test_uint32_add_scalar_array_multiple_calls_accumulate),
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
// -------------------------------------------------------------------------------- 

// ================================================================================
// Group 26: int32_array_min
// ================================================================================

static void test_int32_min_null_array_returns_null_pointer(void** state) {
    (void)state;
    int32_expect_t r = int32_array_min(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_min_returns_index_of_smallest(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 3000);
    push_back_int32_array(arr, 1000);   /* minimum at index 1 */
    push_back_int32_array(arr, 2000);
    /* arr: [3000, 1000, 2000] */

    int32_expect_t result = int32_array_min(arr);
    assert_true(result.has_value);
    assert_int_equal((int)result.u.value, 1000);

    return_int32_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int32_min_negative_values_and_first_occurrence(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    /* Span the full signed range; INT32_MIN is the minimum. */
    push_back_int32_array(arr,         0);
    push_back_int32_array(arr,  INT32_MAX);
    push_back_int32_array(arr,  INT32_MIN);  /* minimum at index 2 */
    push_back_int32_array(arr,        -1);
    push_back_int32_array(arr,  INT32_MIN);  /* duplicate — first occurrence wins */
    /* arr: [0, INT32_MAX, INT32_MIN, -1, INT32_MIN] */

    int32_expect_t result = int32_array_min(arr);
    assert_true(result.has_value);
    assert_int_equal((int)result.u.value, INT32_MIN);  /* first INT32_MIN is at index 2 */

    return_int32_array(arr);
}

// ================================================================================
// Group 27: int32_array_max
// ================================================================================

static void test_int32_max_null_array_returns_null_pointer(void** state) {
    (void)state;
    int32_expect_t r = int32_array_max(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_max_returns_largest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    push_back_int32_array(arr, 2000);
    push_back_int32_array(arr, 3000);
    push_back_int32_array(arr, 1000);
    /* arr: [2000, 3000, 1000] — maximum value is 3000 */

    int32_expect_t result = int32_array_max(arr);
    assert_true(result.has_value);
    assert_true(result.u.value == 3000);

    return_int32_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int32_max_negative_values_and_boundary(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* arr = r.u.value;

    /* Span the full signed range; INT32_MAX must be returned. */
    push_back_int32_array(arr,        -1);
    push_back_int32_array(arr,  INT32_MIN);
    push_back_int32_array(arr,  INT32_MAX);
    push_back_int32_array(arr,         0);
    /* arr: [-1, INT32_MIN, INT32_MAX, 0] */

    int32_expect_t result = int32_array_max(arr);
    assert_true(result.has_value);
    assert_true(result.u.value == INT32_MAX);

    return_int32_array(arr);
}

// ================================================================================
// Group 28: int32_array_sum
// ================================================================================

// static void test_int32_sum_null_array_returns_null_pointer(void** state) {
//     (void)state;
//     int32_expect_t r = int32_array_sum(NULL);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int32_sum_returns_correct_total(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     int32_array_expect_t r = init_int32_array(8, false, alloc);
//     assert_true(r.has_value);
//     int32_array_t* arr = r.u.value;
//
//     push_back_int32_array(arr, 10000);
//     push_back_int32_array(arr, 20000);
//     push_back_int32_array(arr, 30000);
//     /* arr: [10000, 20000, 30000], sum == 60000 */
//
//     int32_expect_t result = int32_array_sum(arr);
//     assert_true(result.has_value);
//     assert_true(result.u.value == 60000);
//
//     return_int32_array(arr);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int32_sum_mixed_signs_cancel(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     int32_array_expect_t r = init_int32_array(8, false, alloc);
//     assert_true(r.has_value);
//     int32_array_t* arr = r.u.value;
//
//     push_back_int32_array(arr,  100000);
//     push_back_int32_array(arr, -100000);
//     push_back_int32_array(arr,   50000);
//     push_back_int32_array(arr,  -50000);
//     /* arr: [100000, -100000, 50000, -50000], sum == 0 */
//
//     int32_expect_t result = int32_array_sum(arr);
//     assert_true(result.has_value);
//     assert_true(result.u.value == 0);
//
//     return_int32_array(arr);
// }
// // -------------------------------------------------------------------------------- 

static void test_int32_cumulative_null_array_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = cumulative_int32_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_cumulative_produces_prefix_sum(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* src = r.u.value;

    push_back_int32_array(src, 1000);
    push_back_int32_array(src, 2000);
    push_back_int32_array(src, 3000);
    push_back_int32_array(src, 4000);
    /* src: [1000, 2000, 3000, 4000]
     * expected output: [1000, 3000, 6000, 10000] */

    int32_array_expect_t cr = cumulative_int32_array(src, alloc);
    assert_true(cr.has_value);
    int32_array_t* dst = cr.u.value;
    assert_int_equal((int)int32_array_size(dst), 4);

    int32_t val = 0;
    get_int32_array_index(dst, 0, &val);  assert_true(val ==  1000);
    get_int32_array_index(dst, 1, &val);  assert_true(val ==  3000);
    get_int32_array_index(dst, 2, &val);  assert_true(val ==  6000);
    get_int32_array_index(dst, 3, &val);  assert_true(val == 10000);

    return_int32_array(src);
    return_int32_array(dst);
}

// --------------------------------------------------------------------------------

static void test_int32_cumulative_mixed_signs(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_array_expect_t r = init_int32_array(8, false, alloc);
    assert_true(r.has_value);
    int32_array_t* src = r.u.value;

    push_back_int32_array(src,  5000);
    push_back_int32_array(src, -3000);
    push_back_int32_array(src,  2000);
    push_back_int32_array(src, -1000);
    /* src: [5000, -3000, 2000, -1000]
     * expected output: [5000, 2000, 4000, 3000] */

    int32_array_expect_t cr = cumulative_int32_array(src, alloc);
    assert_true(cr.has_value);
    int32_array_t* dst = cr.u.value;
    assert_int_equal((int)int32_array_size(dst), 4);

    int32_t val = 0;
    get_int32_array_index(dst, 0, &val);  assert_true(val == 5000);
    get_int32_array_index(dst, 1, &val);  assert_true(val == 2000);
    get_int32_array_index(dst, 2, &val);  assert_true(val == 4000);
    get_int32_array_index(dst, 3, &val);  assert_true(val == 3000);

    return_int32_array(src);
    return_int32_array(dst);
}
// -------------------------------------------------------------------------------- 

// static int32_array_t* _make_int32_array(size_t capacity, bool growth) {
//     allocator_vtable_t a = heap_allocator();
//     int32_array_expect_t r = init_int32_array(capacity, growth, a);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     return r.u.value;
// }
//
// // --------------------------------------------------------------------------------
//
// static int32_array_t* _make_sample_int32_array(void) {
//     int32_array_t* array = _make_int32_array(8u, true);
//
//     assert_int_equal(push_back_int32_array(array, 1u), NO_ERROR);
//     assert_int_equal(push_back_int32_array(array, 2u), NO_ERROR);
//     assert_int_equal(push_back_int32_array(array, 3u), NO_ERROR);
//     assert_int_equal(push_back_int32_array(array, 4u), NO_ERROR);
//     assert_int_equal(push_back_int32_array(array, 5u), NO_ERROR);
//
//     return array;
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_int32_add_scalar_array_null_returns_null_pointer(void** state) {
//     (void)state;
//
//     assert_int_equal(int32_add_scalar_array(NULL, 5u), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int32_add_scalar_array_empty_returns_empty(void** state) {
//     (void)state;
//
//     int32_array_t* array = _make_int32_array(4u, true);
//
//     assert_int_equal(int32_add_scalar_array(array, 5u), EMPTY);
//
//     return_int32_array(array);
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_int32_add_scalar_array_add_zero_leaves_values_unchanged(void** state) {
//     (void)state;
//
//     int32_array_t* array = _make_sample_int32_array();
//
//     assert_int_equal(int32_add_scalar_array(array, 0u), NO_ERROR);
//
//     int32_t out = 0u;
//
//     assert_int_equal(get_int32_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 1u);
//
//     assert_int_equal(get_int32_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 2u);
//
//     assert_int_equal(get_int32_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 3u);
//
//     assert_int_equal(get_int32_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 4u);
//
//     assert_int_equal(get_int32_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 5u);
//
//     return_int32_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int32_add_scalar_array_single_element_updates_value(void** state) {
//     (void)state;
//
//     int32_array_t* array = _make_int32_array(4u, true);
//     assert_int_equal(push_back_int32_array(array, 10u), NO_ERROR);
//
//     assert_int_equal(int32_add_scalar_array(array, 7u), NO_ERROR);
//
//     int32_t out = 0u;
//     assert_int_equal(get_int32_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 17u);
//
//     return_int32_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int32_add_scalar_array_updates_all_elements(void** state) {
//     (void)state;
//
//     int32_array_t* array = _make_sample_int32_array();
//
//     assert_int_equal(int32_add_scalar_array(array, 10u), NO_ERROR);
//
//     int32_t out = 0u;
//
//     assert_int_equal(get_int32_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 11u);
//
//     assert_int_equal(get_int32_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 12u);
//
//     assert_int_equal(get_int32_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 13u);
//
//     assert_int_equal(get_int32_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 14u);
//
//     assert_int_equal(get_int32_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 15u);
//
//     return_int32_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int32_add_scalar_array_preserves_length(void** state) {
//     (void)state;
//
//     int32_array_t* array = _make_sample_int32_array();
//     size_t before = int32_array_size(array);
//
//     assert_int_equal(int32_add_scalar_array(array, 3u), NO_ERROR);
//
//     assert_int_equal(int32_array_size(array), before);
//
//     return_int32_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int32_add_scalar_array_multiple_calls_accumulate(void** state) {
//     (void)state;
//
//     int32_array_t* array = _make_sample_int32_array();
//
//     assert_int_equal(int32_add_scalar_array(array, 2u), NO_ERROR);
//     assert_int_equal(int32_add_scalar_array(array, 3u), NO_ERROR);
//
//     int32_t out = 0u;
//
//     assert_int_equal(get_int32_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 6u);
//
//     assert_int_equal(get_int32_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 7u);
//
//     assert_int_equal(get_int32_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 8u);
//
//     assert_int_equal(get_int32_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 9u);
//
//     assert_int_equal(get_int32_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 10u);
//
//     return_int32_array(array);
// }
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

    cmocka_unit_test(test_int32_min_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_min_returns_index_of_smallest),
    cmocka_unit_test(test_int32_min_negative_values_and_first_occurrence),
   
    /* Group 27: int32_array_max */
    cmocka_unit_test(test_int32_max_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_max_returns_largest_value),
    cmocka_unit_test(test_int32_max_negative_values_and_boundary),

    /* Group 28: int32_array_sum */
    // cmocka_unit_test(test_int32_sum_null_array_returns_null_pointer),
    // cmocka_unit_test(test_int32_sum_returns_correct_total),
    // cmocka_unit_test(test_int32_sum_mixed_signs_cancel), 

    /* Group 29: cumulative_int32_array */
    cmocka_unit_test(test_int32_cumulative_null_array_returns_null_pointer),
    cmocka_unit_test(test_int32_cumulative_produces_prefix_sum),
    cmocka_unit_test(test_int32_cumulative_mixed_signs),

    // cmocka_unit_test(test_int32_add_scalar_array_null_returns_null_pointer),
    // cmocka_unit_test(test_int32_add_scalar_array_empty_returns_empty),
    // cmocka_unit_test(test_int32_add_scalar_array_add_zero_leaves_values_unchanged),
    // cmocka_unit_test(test_int32_add_scalar_array_single_element_updates_value),
    // cmocka_unit_test(test_int32_add_scalar_array_updates_all_elements),
    // cmocka_unit_test(test_int32_add_scalar_array_preserves_length),
    // cmocka_unit_test(test_int32_add_scalar_array_multiple_calls_accumulate),


};
const size_t test_int32_array_count = sizeof(test_int32_array) / sizeof(test_int32_array[0]);
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Group 1: init_uint64_array
// ================================================================================

static void test_uint64_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    uint64_array_expect_t r = init_uint64_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)uint64_array_size(r.u.value), 0);
    assert_int_equal((int)uint64_array_alloc(r.u.value), 8);
    return_uint64_array(r.u.value);
}

// ================================================================================
// Group 2: return_uint64_array
// ================================================================================

static void test_uint64_return_null_is_safe(void** state) {
    (void)state;
    return_uint64_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_uint64_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    return_uint64_array(r.u.value);
}

// ================================================================================
// Group 3: push_back_uint64_array
// ================================================================================

static void test_uint64_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_uint64_array(NULL, 1u), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    assert_int_equal(push_back_uint64_array(arr, 1000u), NO_ERROR);
    assert_int_equal(push_back_uint64_array(arr, 2000u), NO_ERROR);
    assert_int_equal(push_back_uint64_array(arr, 3000u), NO_ERROR);
    assert_int_equal((int)uint64_array_size(arr), 3);

    uint64_t val = 0;
    get_uint64_array_index(arr, 0, &val);  assert_true(val == 1000u);
    get_uint64_array_index(arr, 2, &val);  assert_true(val == 3000u);

    return_uint64_array(arr);
}

// ================================================================================
// Group 4: push_front_uint64_array
// ================================================================================

static void test_uint64_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_uint64_array(NULL, 1u), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 2000u);
    push_front_uint64_array(arr, 1000u);
    /* arr is [1000, 2000] */

    uint64_t val = 0;
    get_uint64_array_index(arr, 0, &val);
    assert_true(val == 1000u);

    return_uint64_array(arr);
}

// ================================================================================
// Group 5: push_at_uint64_array
// ================================================================================

static void test_uint64_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_uint64_array(NULL, 0, 1u), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 3000u);
    push_at_uint64_array(arr, 1, 2000u);
    /* arr is [1000, 2000, 3000] */

    uint64_t val = 0;
    get_uint64_array_index(arr, 1, &val);
    assert_true(val == 2000u);

    return_uint64_array(arr);
}

// ================================================================================
// Group 6: get_uint64_array_index
// ================================================================================

static void test_uint64_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    uint64_t val = 0;
    assert_int_equal(get_uint64_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr,                    0u);
    push_back_uint64_array(arr,  9223372036854775808u);  /* 2^63 */
    push_back_uint64_array(arr, 18446744073709551615u);  /* UINT64_MAX */

    uint64_t val = 0;
    assert_int_equal(get_uint64_array_index(arr, 0, &val), NO_ERROR);
    assert_true(val == 0u);
    assert_int_equal(get_uint64_array_index(arr, 2, &val), NO_ERROR);
    assert_true(val == 18446744073709551615u);

    return_uint64_array(arr);
}

// ================================================================================
// Group 7: pop_back_uint64_array
// ================================================================================

static void test_uint64_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_uint64_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);

    uint64_t val = 0;
    assert_int_equal(pop_back_uint64_array(arr, &val), NO_ERROR);
    assert_true(val == 2000u);
    assert_int_equal((int)uint64_array_size(arr), 1);

    return_uint64_array(arr);
}

// ================================================================================
// Group 8: pop_front_uint64_array
// ================================================================================

static void test_uint64_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_uint64_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);

    uint64_t val = 0;
    assert_int_equal(pop_front_uint64_array(arr, &val), NO_ERROR);
    assert_true(val == 1000u);
    assert_int_equal((int)uint64_array_size(arr), 1);

    return_uint64_array(arr);
}

// ================================================================================
// Group 9: pop_any_uint64_array
// ================================================================================

static void test_uint64_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_uint64_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);
    push_back_uint64_array(arr, 3000u);

    uint64_t val = 0;
    assert_int_equal(pop_any_uint64_array(arr, &val, 1), NO_ERROR);
    assert_true(val == 2000u);
    assert_int_equal((int)uint64_array_size(arr), 2);

    return_uint64_array(arr);
}

// ================================================================================
// Group 10: clear_uint64_array
// ================================================================================

static void test_uint64_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_uint64_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);
    assert_int_equal(clear_uint64_array(arr), NO_ERROR);
    assert_int_equal((int)uint64_array_size(arr), 0);
    assert_int_equal((int)uint64_array_alloc(arr), 4);   /* capacity retained */

    return_uint64_array(arr);
}

// ================================================================================
// Group 11: set_uint64_array_index
// ================================================================================

static void test_uint64_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_uint64_array_index(NULL, 0, 1u), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    assert_int_equal(set_uint64_array_index(arr, 0, 18446744073709551615u), NO_ERROR);

    uint64_t val = 0;
    get_uint64_array_index(arr, 0, &val);
    assert_true(val == 18446744073709551615u);

    return_uint64_array(arr);
}

// ================================================================================
// Group 12: copy_uint64_array
// ================================================================================

static void test_uint64_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = copy_uint64_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* src = r.u.value;

    push_back_uint64_array(src, 1000u);
    push_back_uint64_array(src, 2000u);

    uint64_array_expect_t cr = copy_uint64_array(src, alloc);
    assert_true(cr.has_value);
    uint64_array_t* dst = cr.u.value;

    /* Mutating src must not affect the copy */
    set_uint64_array_index(src, 0, 999u);
    uint64_t val = 0;
    get_uint64_array_index(dst, 0, &val);
    assert_true(val == 1000u);

    return_uint64_array(src);
    return_uint64_array(dst);
}

// ================================================================================
// Group 13: concat_uint64_array
// ================================================================================

static void test_uint64_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* src = r.u.value;
    assert_int_equal(concat_uint64_array(NULL, src), NULL_POINTER);
    return_uint64_array(src);
}

// --------------------------------------------------------------------------------

static void test_uint64_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r1 = init_uint64_array(8, false, alloc);
    uint64_array_expect_t r2 = init_uint64_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    uint64_array_t* dst = r1.u.value;
    uint64_array_t* src = r2.u.value;

    push_back_uint64_array(dst, 1000u);
    push_back_uint64_array(src, 2000u);

    assert_int_equal(concat_uint64_array(dst, src), NO_ERROR);
    assert_int_equal((int)uint64_array_size(dst), 2);

    uint64_t val = 0;
    get_uint64_array_index(dst, 1, &val);
    assert_true(val == 2000u);

    return_uint64_array(dst);
    return_uint64_array(src);
}

// ================================================================================
// Group 14: slice_uint64_array
// ================================================================================

static void test_uint64_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = slice_uint64_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(8, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* src = r.u.value;

    push_back_uint64_array(src, 1000u);
    push_back_uint64_array(src, 2000u);
    push_back_uint64_array(src, 3000u);
    push_back_uint64_array(src, 4000u);
    push_back_uint64_array(src, 5000u);

    uint64_array_expect_t sr = slice_uint64_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    uint64_array_t* slc = sr.u.value;
    assert_int_equal((int)uint64_array_size(slc), 3);

    uint64_t val = 0;
    get_uint64_array_index(slc, 0, &val);  assert_true(val == 2000u);
    get_uint64_array_index(slc, 1, &val);  assert_true(val == 3000u);
    get_uint64_array_index(slc, 2, &val);  assert_true(val == 4000u);

    return_uint64_array(src);
    return_uint64_array(slc);
}

// ================================================================================
// Group 15: reverse_uint64_array
// ================================================================================

static void test_uint64_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_uint64_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);
    push_back_uint64_array(arr, 3000u);

    assert_int_equal(reverse_uint64_array(arr), NO_ERROR);

    uint64_t val = 0;
    get_uint64_array_index(arr, 0, &val);  assert_true(val == 3000u);
    get_uint64_array_index(arr, 2, &val);  assert_true(val == 1000u);

    return_uint64_array(arr);
}

// ================================================================================
// Group 16: sort_uint64_array
// ================================================================================

static void test_uint64_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_uint64_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_sort_forward_basic(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(8, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 3000u);
    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);

    assert_int_equal(sort_uint64_array(arr, FORWARD), NO_ERROR);

    uint64_t val = 0;
    get_uint64_array_index(arr, 0, &val);  assert_true(val == 1000u);
    get_uint64_array_index(arr, 1, &val);  assert_true(val == 2000u);
    get_uint64_array_index(arr, 2, &val);  assert_true(val == 3000u);

    return_uint64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint64_sort_comparator_safe_near_max(void** state) {
    (void)state;
    /*
     * A subtract-based comparator is broken for uint64_t: the difference
     * (18446744073709551615u - 1u) = 18446744073709551614u overflows int64_t,
     * producing an undefined or wrong sign. The three-way comparison
     * (va > vb) - (va < vb) is always correct regardless of magnitude.
     */
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 18446744073709551615u);  /* UINT64_MAX */
    push_back_uint64_array(arr,                    1u);
    push_back_uint64_array(arr,  9223372036854775808u);  /* 2^63        */

    assert_int_equal(sort_uint64_array(arr, FORWARD), NO_ERROR);

    uint64_t val = 0;
    get_uint64_array_index(arr, 0, &val);  assert_true(val ==                    1u);
    get_uint64_array_index(arr, 1, &val);  assert_true(val ==  9223372036854775808u);
    get_uint64_array_index(arr, 2, &val);  assert_true(val == 18446744073709551615u);

    return_uint64_array(arr);
}

// ================================================================================
// Group 17: uint64_array_contains
// ================================================================================

static void test_uint64_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = uint64_array_contains(NULL, 0u, 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_contains_finds_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);
    push_back_uint64_array(arr, 3000u);

    size_expect_t sr = uint64_array_contains(arr, 2000u, 0, 3);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    return_uint64_array(arr);
}

// ================================================================================
// Group 18: uint64_array_binary_search
// ================================================================================

static void test_uint64_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = uint64_array_binary_search(NULL, 0u, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_binary_search_finds_value_with_sort(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(8, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 3000u);
    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);
    push_back_uint64_array(arr, 4000u);

    /* sort == true: arr becomes [1000, 2000, 3000, 4000] */
    size_expect_t sr = uint64_array_binary_search(arr, 3000u, true);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 2);

    return_uint64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint64_binary_search_comparator_safe_near_max(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(8, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    /* Already sorted ascending */
    push_back_uint64_array(arr,                    1u);
    push_back_uint64_array(arr,  9223372036854775808u);  /* 2^63        */
    push_back_uint64_array(arr, 18446744073709551614u);  /* UINT64_MAX - 1 */
    push_back_uint64_array(arr, 18446744073709551615u);  /* UINT64_MAX     */

    size_expect_t sr = uint64_array_binary_search(arr, 18446744073709551615u, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 3);

    sr = uint64_array_binary_search(arr, 1u, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_uint64_array(arr);
}

// ================================================================================
// Group 19: uint64_array_binary_bracket
// ================================================================================

static void test_uint64_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = uint64_array_binary_bracket(NULL, 0u, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(8, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);
    push_back_uint64_array(arr, 3000u);
    push_back_uint64_array(arr, 4000u);

    bracket_expect_t br = uint64_array_binary_bracket(arr, 2000u, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_uint64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint64_binary_bracket_comparator_safe_near_max(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(8, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    /* [1, 9223372036854775808u, 18446744073709551615u] — gap between 2^63 and UINT64_MAX */
    push_back_uint64_array(arr,                    1u);
    push_back_uint64_array(arr,  9223372036854775808u);
    push_back_uint64_array(arr, 18446744073709551615u);

    /* 12000000000000000000u falls in the gap */
    bracket_expect_t br = uint64_array_binary_bracket(arr, 12000000000000000000u, false);
    assert_true(br.has_value);
    /* lower = first element >= 12000000000000000000u = index 2 (UINT64_MAX) */
    /* upper = last  element <= 12000000000000000000u = index 1 (2^63)       */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_uint64_array(arr);
}

// ================================================================================
// Group 20: uint64_array_size
// ================================================================================

static void test_uint64_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint64_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_uint64_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    assert_int_equal((int)uint64_array_size(arr), 0);
    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);
    assert_int_equal((int)uint64_array_size(arr), 2);

    return_uint64_array(arr);
}

// ================================================================================
// Group 21: uint64_array_alloc
// ================================================================================

static void test_uint64_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint64_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_uint64_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)uint64_array_alloc(r.u.value), 16);
    return_uint64_array(r.u.value);
}

// ================================================================================
// Group 22: uint64_array_data_size
// ================================================================================

static void test_uint64_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint64_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_uint64_data_size_is_eight(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)uint64_array_data_size(r.u.value), 8);
    return_uint64_array(r.u.value);
}

// ================================================================================
// Group 23: is_uint64_array_empty
// ================================================================================

static void test_uint64_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint64_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_uint64_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    assert_true(is_uint64_array_empty(arr));
    push_back_uint64_array(arr, 1000u);
    assert_false(is_uint64_array_empty(arr));

    return_uint64_array(arr);
}

// ================================================================================
// Group 24: is_uint64_array_full
// ================================================================================

static void test_uint64_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint64_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_uint64_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(2, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    assert_false(is_uint64_array_full(arr));
    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);
    assert_true(is_uint64_array_full(arr));

    return_uint64_array(arr);
}

// ================================================================================
// Group 25: is_uint64_array_ptr
// ================================================================================

static void test_uint64_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    uint64_t val = 0;
    assert_false(is_uint64_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_uint64_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(8, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 1000u);
    push_back_uint64_array(arr, 2000u);
    push_back_uint64_array(arr, 3000u);
    /* arr contains [1000, 2000, 3000], len == 3, alloc == 8 */

    const uint64_t* first = (const uint64_t*)arr->base.data;
    const uint64_t* last  = first + 2;
    const uint64_t* spare = first + 3;   /* beyond live region */

    assert_true (is_uint64_array_ptr(arr, first));
    assert_true (is_uint64_array_ptr(arr, last));
    assert_false(is_uint64_array_ptr(arr, spare));
    assert_false(is_uint64_array_ptr(arr, NULL));

    return_uint64_array(arr);
}

// ================================================================================
// Group 26: uint64_array_min
// ================================================================================

static void test_uint64_min_null_array_returns_null_pointer(void** state) {
    (void)state;
    uint64_expect_t r = uint64_array_min(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_min_returns_smallest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 3000000u);
    push_back_uint64_array(arr, 1000000u);
    push_back_uint64_array(arr, 2000000u);

    uint64_expect_t mr = uint64_array_min(arr);
    assert_true(mr.has_value);
    assert_true(mr.u.value == 1000000u);

    return_uint64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint64_min_boundary_values(void** state) {
    (void)state;
    /*
     * Include 0 and UINT64_MAX to confirm the three-way comparator selects
     * 0 as the minimum without overflowing when computing (0 - UINT64_MAX).
     */
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr,                    0u);
    push_back_uint64_array(arr, 18446744073709551615u);  /* UINT64_MAX */
    push_back_uint64_array(arr,  9223372036854775808u);  /* 2^63       */
    push_back_uint64_array(arr,                    1u);

    uint64_expect_t mr = uint64_array_min(arr);
    assert_true(mr.has_value);
    assert_true(mr.u.value == 0u);

    return_uint64_array(arr);
}

// ================================================================================
// Group 27: uint64_array_max
// ================================================================================

static void test_uint64_max_null_array_returns_null_pointer(void** state) {
    (void)state;
    uint64_expect_t r = uint64_array_max(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_max_returns_largest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr, 2000000u);
    push_back_uint64_array(arr, 3000000u);
    push_back_uint64_array(arr, 1000000u);

    uint64_expect_t mr = uint64_array_max(arr);
    assert_true(mr.has_value);
    assert_true(mr.u.value == 3000000u);

    return_uint64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_uint64_max_boundary_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* arr = r.u.value;

    push_back_uint64_array(arr,                    1u);
    push_back_uint64_array(arr,                    0u);
    push_back_uint64_array(arr, 18446744073709551615u);  /* UINT64_MAX */
    push_back_uint64_array(arr,  9223372036854775808u);  /* 2^63       */

    uint64_expect_t mr = uint64_array_max(arr);
    assert_true(mr.has_value);
    assert_true(mr.u.value == 18446744073709551615u);

    return_uint64_array(arr);
}

// ================================================================================
// Group 28: uint64_array_sum
// ================================================================================

// static void test_uint64_sum_null_array_returns_null_pointer(void** state) {
//     (void)state;
//     uint64_expect_t r = uint64_array_sum(NULL);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint64_sum_returns_correct_total(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint64_array_expect_t r = init_uint64_array(4, false, alloc);
//     assert_true(r.has_value);
//     uint64_array_t* arr = r.u.value;
//
//     push_back_uint64_array(arr, 1000000000u);
//     push_back_uint64_array(arr, 2000000000u);
//     push_back_uint64_array(arr, 3000000000u);
//
//     uint64_expect_t sr = uint64_array_sum(arr);
//     assert_true(sr.has_value);
//     assert_true(sr.u.value == 6000000000u);
//
//     return_uint64_array(arr);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint64_sum_wraps_on_overflow(void** state) {
//     (void)state;
//     /*
//      * UINT64_MAX + 1 wraps to 0 mod 2^64. Pushing UINT64_MAX then 1
//      * must produce 0 — verifying the accumulator type is genuinely
//      * uint64_t and not silently widened.
//      */
//     allocator_vtable_t alloc = heap_allocator();
//     uint64_array_expect_t r = init_uint64_array(4, false, alloc);
//     assert_true(r.has_value);
//     uint64_array_t* arr = r.u.value;
//
//     push_back_uint64_array(arr, 18446744073709551615u);  /* UINT64_MAX */
//     push_back_uint64_array(arr,                    1u);
//
//     uint64_expect_t sr = uint64_array_sum(arr);
//     assert_true(sr.has_value);
//     assert_true(sr.u.value == 0u);
//
//     return_uint64_array(arr);
// }

// ================================================================================
// Group 29: cumulative_uint64_array
// ================================================================================

static void test_uint64_cumulative_null_array_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = cumulative_uint64_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_uint64_cumulative_produces_prefix_sum(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(8, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* src = r.u.value;

    push_back_uint64_array(src, 1000u);
    push_back_uint64_array(src, 2000u);
    push_back_uint64_array(src, 3000u);
    push_back_uint64_array(src, 4000u);
    /* src: [1000, 2000, 3000, 4000]
     * expected output: [1000, 3000, 6000, 10000] */

    uint64_array_expect_t cr = cumulative_uint64_array(src, alloc);
    assert_true(cr.has_value);
    uint64_array_t* dst = cr.u.value;
    assert_int_equal((int)uint64_array_size(dst), 4);

    uint64_t val = 0;
    get_uint64_array_index(dst, 0, &val);  assert_true(val ==  1000u);
    get_uint64_array_index(dst, 1, &val);  assert_true(val ==  3000u);
    get_uint64_array_index(dst, 2, &val);  assert_true(val ==  6000u);
    get_uint64_array_index(dst, 3, &val);  assert_true(val == 10000u);

    return_uint64_array(src);
    return_uint64_array(dst);
}

// --------------------------------------------------------------------------------

static void test_uint64_cumulative_single_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_array_expect_t r = init_uint64_array(4, false, alloc);
    assert_true(r.has_value);
    uint64_array_t* src = r.u.value;

    push_back_uint64_array(src, 9999999999u);
    /* src: [9999999999] — output must equal input verbatim */

    uint64_array_expect_t cr = cumulative_uint64_array(src, alloc);
    assert_true(cr.has_value);
    uint64_array_t* dst = cr.u.value;
    assert_int_equal((int)uint64_array_size(dst), 1);

    uint64_t val = 0;
    get_uint64_array_index(dst, 0, &val);  assert_true(val == 9999999999u);

    return_uint64_array(src);
    return_uint64_array(dst);
}
// -------------------------------------------------------------------------------- 

// static uint64_array_t* _make_uint64_array(size_t capacity, bool growth) {
//     allocator_vtable_t a = heap_allocator();
//     uint64_array_expect_t r = init_uint64_array(capacity, growth, a);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     return r.u.value;
// }
//
// // --------------------------------------------------------------------------------
//
// static uint64_array_t* _make_sample_uint64_array(void) {
//     uint64_array_t* array = _make_uint64_array(8u, true);
//
//     assert_int_equal(push_back_uint64_array(array, 1u), NO_ERROR);
//     assert_int_equal(push_back_uint64_array(array, 2u), NO_ERROR);
//     assert_int_equal(push_back_uint64_array(array, 3u), NO_ERROR);
//     assert_int_equal(push_back_uint64_array(array, 4u), NO_ERROR);
//     assert_int_equal(push_back_uint64_array(array, 5u), NO_ERROR);
//
//     return array;
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_uint64_add_scalar_array_null_returns_null_pointer(void** state) {
//     (void)state;
//
//     assert_int_equal(uint64_add_scalar_array(NULL, 5u), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint64_add_scalar_array_empty_returns_empty(void** state) {
//     (void)state;
//
//     uint64_array_t* array = _make_uint64_array(4u, true);
//
//     assert_int_equal(uint64_add_scalar_array(array, 5u), EMPTY);
//
//     return_uint64_array(array);
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_uint64_add_scalar_array_add_zero_leaves_values_unchanged(void** state) {
//     (void)state;
//
//     uint64_array_t* array = _make_sample_uint64_array();
//
//     assert_int_equal(uint64_add_scalar_array(array, 0u), NO_ERROR);
//
//     uint64_t out = 0u;
//
//     assert_int_equal(get_uint64_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 1u);
//
//     assert_int_equal(get_uint64_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 2u);
//
//     assert_int_equal(get_uint64_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 3u);
//
//     assert_int_equal(get_uint64_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 4u);
//
//     assert_int_equal(get_uint64_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 5u);
//
//     return_uint64_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint64_add_scalar_array_single_element_updates_value(void** state) {
//     (void)state;
//
//     uint64_array_t* array = _make_uint64_array(4u, true);
//     assert_int_equal(push_back_uint64_array(array, 10u), NO_ERROR);
//
//     assert_int_equal(uint64_add_scalar_array(array, 7u), NO_ERROR);
//
//     uint64_t out = 0u;
//     assert_int_equal(get_uint64_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 17u);
//
//     return_uint64_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint64_add_scalar_array_updates_all_elements(void** state) {
//     (void)state;
//
//     uint64_array_t* array = _make_sample_uint64_array();
//
//     assert_int_equal(uint64_add_scalar_array(array, 10u), NO_ERROR);
//
//     uint64_t out = 0u;
//
//     assert_int_equal(get_uint64_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 11u);
//
//     assert_int_equal(get_uint64_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 12u);
//
//     assert_int_equal(get_uint64_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 13u);
//
//     assert_int_equal(get_uint64_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 14u);
//
//     assert_int_equal(get_uint64_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 15u);
//
//     return_uint64_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint64_add_scalar_array_preserves_length(void** state) {
//     (void)state;
//
//     uint64_array_t* array = _make_sample_uint64_array();
//     size_t before = uint64_array_size(array);
//
//     assert_int_equal(uint64_add_scalar_array(array, 3u), NO_ERROR);
//
//     assert_int_equal(uint64_array_size(array), before);
//
//     return_uint64_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint64_add_scalar_array_multiple_calls_accumulate(void** state) {
//     (void)state;
//
//     uint64_array_t* array = _make_sample_uint64_array();
//
//     assert_int_equal(uint64_add_scalar_array(array, 2u), NO_ERROR);
//     assert_int_equal(uint64_add_scalar_array(array, 3u), NO_ERROR);
//
//     uint64_t out = 0u;
//
//     assert_int_equal(get_uint64_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 6u);
//
//     assert_int_equal(get_uint64_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 7u);
//
//     assert_int_equal(get_uint64_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 8u);
//
//     assert_int_equal(get_uint64_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 9u);
//
//     assert_int_equal(get_uint64_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 10u);
//
//     return_uint64_array(array);
// }
// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_uint64_array[] = {

    /* Group 1: init_uint64_array */
    cmocka_unit_test(test_uint64_init_null_allocate_fn_fails),
    cmocka_unit_test(test_uint64_init_returns_valid_array),

    /* Group 2: return_uint64_array */
    cmocka_unit_test(test_uint64_return_null_is_safe),
    cmocka_unit_test(test_uint64_return_valid_array_does_not_crash),

    /* Group 3: push_back_uint64_array */
    cmocka_unit_test(test_uint64_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_push_back_appends_value),

    /* Group 4: push_front_uint64_array */
    cmocka_unit_test(test_uint64_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_push_front_prepends_value),

    /* Group 5: push_at_uint64_array */
    cmocka_unit_test(test_uint64_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_push_at_inserts_at_index),

    /* Group 6: get_uint64_array_index */
    cmocka_unit_test(test_uint64_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_get_returns_correct_value),

    /* Group 7: pop_back_uint64_array */
    cmocka_unit_test(test_uint64_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_pop_back_removes_last_element),

    /* Group 8: pop_front_uint64_array */
    cmocka_unit_test(test_uint64_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_pop_front_removes_first_element),

    /* Group 9: pop_any_uint64_array */
    cmocka_unit_test(test_uint64_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_pop_any_removes_element_at_index),

    /* Group 10: clear_uint64_array */
    cmocka_unit_test(test_uint64_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_clear_resets_len_to_zero),

    /* Group 11: set_uint64_array_index */
    cmocka_unit_test(test_uint64_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_set_overwrites_element),

    /* Group 12: copy_uint64_array */
    cmocka_unit_test(test_uint64_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_uint64_copy_produces_independent_array),

    /* Group 13: concat_uint64_array */
    cmocka_unit_test(test_uint64_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_uint64_concat_appends_elements),

    /* Group 14: slice_uint64_array */
    cmocka_unit_test(test_uint64_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_uint64_slice_returns_correct_subrange),

    /* Group 15: reverse_uint64_array */
    cmocka_unit_test(test_uint64_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_reverse_reverses_elements),

    /* Group 16: sort_uint64_array */
    cmocka_unit_test(test_uint64_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_sort_forward_basic),
    cmocka_unit_test(test_uint64_sort_comparator_safe_near_max),

    /* Group 17: uint64_array_contains */
    cmocka_unit_test(test_uint64_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_contains_finds_value),

    /* Group 18: uint64_array_binary_search */
    cmocka_unit_test(test_uint64_binary_search_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_binary_search_finds_value_with_sort),
    cmocka_unit_test(test_uint64_binary_search_comparator_safe_near_max),

    /* Group 19: uint64_array_binary_bracket */
    cmocka_unit_test(test_uint64_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_binary_bracket_exact_match),
    cmocka_unit_test(test_uint64_binary_bracket_comparator_safe_near_max),

    /* Group 20: uint64_array_size */
    cmocka_unit_test(test_uint64_size_null_returns_zero),
    cmocka_unit_test(test_uint64_size_reflects_push_count),

    /* Group 21: uint64_array_alloc */
    cmocka_unit_test(test_uint64_alloc_null_returns_zero),
    cmocka_unit_test(test_uint64_alloc_matches_capacity),

    /* Group 22: uint64_array_data_size */
    cmocka_unit_test(test_uint64_data_size_null_returns_zero),
    cmocka_unit_test(test_uint64_data_size_is_eight),

    /* Group 23: is_uint64_array_empty */
    cmocka_unit_test(test_uint64_empty_null_returns_true),
    cmocka_unit_test(test_uint64_empty_reflects_contents),

    /* Group 24: is_uint64_array_full */
    cmocka_unit_test(test_uint64_full_null_returns_true),
    cmocka_unit_test(test_uint64_full_reflects_capacity),

    /* Group 25: is_uint64_array_ptr */
    cmocka_unit_test(test_uint64_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_uint64_is_ptr_valid_and_invalid),

    /* Group 26: uint64_array_min */
    cmocka_unit_test(test_uint64_min_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_min_returns_smallest_value),
    cmocka_unit_test(test_uint64_min_boundary_values),

    /* Group 27: uint64_array_max */
    cmocka_unit_test(test_uint64_max_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_max_returns_largest_value),
    cmocka_unit_test(test_uint64_max_boundary_values),

    /* Group 28: uint64_array_sum */
    // cmocka_unit_test(test_uint64_sum_null_array_returns_null_pointer),
    // cmocka_unit_test(test_uint64_sum_returns_correct_total),
    // cmocka_unit_test(test_uint64_sum_wraps_on_overflow),

    /* Group 29: cumulative_uint64_array */
    cmocka_unit_test(test_uint64_cumulative_null_array_returns_null_pointer),
    cmocka_unit_test(test_uint64_cumulative_produces_prefix_sum),
    cmocka_unit_test(test_uint64_cumulative_single_element),

    // cmocka_unit_test(test_uint64_add_scalar_array_null_returns_null_pointer),
    // cmocka_unit_test(test_uint64_add_scalar_array_empty_returns_empty),
    // cmocka_unit_test(test_uint64_add_scalar_array_add_zero_leaves_values_unchanged),
    // cmocka_unit_test(test_uint64_add_scalar_array_single_element_updates_value),
    // cmocka_unit_test(test_uint64_add_scalar_array_updates_all_elements),
    // cmocka_unit_test(test_uint64_add_scalar_array_preserves_length),
    // cmocka_unit_test(test_uint64_add_scalar_array_multiple_calls_accumulate),

};
const size_t test_uint64_array_count = sizeof(test_uint64_array) / sizeof(test_uint64_array[0]);
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Group 1: init_int64_array
// ================================================================================

static void test_int64_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    int64_array_expect_t r = init_int64_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)int64_array_size(r.u.value), 0);
    assert_int_equal((int)int64_array_alloc(r.u.value), 8);
    return_int64_array(r.u.value);
}

// ================================================================================
// Group 2: return_int64_array
// ================================================================================

static void test_int64_return_null_is_safe(void** state) {
    (void)state;
    return_int64_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_int64_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    return_int64_array(r.u.value);
}

// ================================================================================
// Group 3: push_back_int64_array
// ================================================================================

static void test_int64_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_int64_array(NULL, 1LL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    assert_int_equal(push_back_int64_array(arr, -1000LL), NO_ERROR);
    assert_int_equal(push_back_int64_array(arr,     0LL), NO_ERROR);
    assert_int_equal(push_back_int64_array(arr,  1000LL), NO_ERROR);
    assert_int_equal((int)int64_array_size(arr), 3);

    int64_t val = 0;
    get_int64_array_index(arr, 0, &val);  assert_true(val == -1000LL);
    get_int64_array_index(arr, 2, &val);  assert_true(val ==  1000LL);

    return_int64_array(arr);
}

// ================================================================================
// Group 4: push_front_int64_array
// ================================================================================

static void test_int64_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_int64_array(NULL, 1LL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr,  2000LL);
    push_front_int64_array(arr, 1000LL);
    /* arr is [1000, 2000] */

    int64_t val = 0;
    get_int64_array_index(arr, 0, &val);
    assert_true(val == 1000LL);

    return_int64_array(arr);
}

// ================================================================================
// Group 5: push_at_int64_array
// ================================================================================

static void test_int64_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_int64_array(NULL, 0, 1LL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, 1000LL);
    push_back_int64_array(arr, 3000LL);
    push_at_int64_array(arr, 1, 2000LL);
    /* arr is [1000, 2000, 3000] */

    int64_t val = 0;
    get_int64_array_index(arr, 1, &val);
    assert_true(val == 2000LL);

    return_int64_array(arr);
}

// ================================================================================
// Group 6: get_int64_array_index
// ================================================================================

static void test_int64_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    int64_t val = 0;
    assert_int_equal(get_int64_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, INT64_MIN);
    push_back_int64_array(arr,       0LL);
    push_back_int64_array(arr, INT64_MAX);

    int64_t val = 0;
    assert_int_equal(get_int64_array_index(arr, 0, &val), NO_ERROR);
    assert_true(val == INT64_MIN);
    assert_int_equal(get_int64_array_index(arr, 2, &val), NO_ERROR);
    assert_true(val == INT64_MAX);

    return_int64_array(arr);
}

// ================================================================================
// Group 7: pop_back_int64_array
// ================================================================================

static void test_int64_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_int64_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, 1000LL);
    push_back_int64_array(arr, 2000LL);

    int64_t val = 0;
    assert_int_equal(pop_back_int64_array(arr, &val), NO_ERROR);
    assert_true(val == 2000LL);
    assert_int_equal((int)int64_array_size(arr), 1);

    return_int64_array(arr);
}

// ================================================================================
// Group 8: pop_front_int64_array
// ================================================================================

static void test_int64_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_int64_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, 1000LL);
    push_back_int64_array(arr, 2000LL);

    int64_t val = 0;
    assert_int_equal(pop_front_int64_array(arr, &val), NO_ERROR);
    assert_true(val == 1000LL);
    assert_int_equal((int)int64_array_size(arr), 1);

    return_int64_array(arr);
}

// ================================================================================
// Group 9: pop_any_int64_array
// ================================================================================

static void test_int64_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_int64_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, 1000LL);
    push_back_int64_array(arr, 2000LL);
    push_back_int64_array(arr, 3000LL);

    int64_t val = 0;
    assert_int_equal(pop_any_int64_array(arr, &val, 1), NO_ERROR);
    assert_true(val == 2000LL);
    assert_int_equal((int)int64_array_size(arr), 2);

    return_int64_array(arr);
}

// ================================================================================
// Group 10: clear_int64_array
// ================================================================================

static void test_int64_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_int64_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, 1000LL);
    push_back_int64_array(arr, 2000LL);
    assert_int_equal(clear_int64_array(arr), NO_ERROR);
    assert_int_equal((int)int64_array_size(arr), 0);
    assert_int_equal((int)int64_array_alloc(arr), 4);   /* capacity retained */

    return_int64_array(arr);
}

// ================================================================================
// Group 11: set_int64_array_index
// ================================================================================

static void test_int64_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_int64_array_index(NULL, 0, 1LL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, 1000LL);
    assert_int_equal(set_int64_array_index(arr, 0, INT64_MIN), NO_ERROR);

    int64_t val = 0;
    get_int64_array_index(arr, 0, &val);
    assert_true(val == INT64_MIN);

    return_int64_array(arr);
}

// ================================================================================
// Group 12: copy_int64_array
// ================================================================================

static void test_int64_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = copy_int64_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* src = r.u.value;

    push_back_int64_array(src, 1000LL);
    push_back_int64_array(src, 2000LL);

    int64_array_expect_t cr = copy_int64_array(src, alloc);
    assert_true(cr.has_value);
    int64_array_t* dst = cr.u.value;

    /* Mutating src must not affect the copy */
    set_int64_array_index(src, 0, 999LL);
    int64_t val = 0;
    get_int64_array_index(dst, 0, &val);
    assert_true(val == 1000LL);

    return_int64_array(src);
    return_int64_array(dst);
}

// ================================================================================
// Group 13: concat_int64_array
// ================================================================================

static void test_int64_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* src = r.u.value;
    assert_int_equal(concat_int64_array(NULL, src), NULL_POINTER);
    return_int64_array(src);
}

// --------------------------------------------------------------------------------

static void test_int64_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r1 = init_int64_array(8, false, alloc);
    int64_array_expect_t r2 = init_int64_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    int64_array_t* dst = r1.u.value;
    int64_array_t* src = r2.u.value;

    push_back_int64_array(dst, 1000LL);
    push_back_int64_array(src, 2000LL);

    assert_int_equal(concat_int64_array(dst, src), NO_ERROR);
    assert_int_equal((int)int64_array_size(dst), 2);

    int64_t val = 0;
    get_int64_array_index(dst, 1, &val);
    assert_true(val == 2000LL);

    return_int64_array(dst);
    return_int64_array(src);
}

// ================================================================================
// Group 14: slice_int64_array
// ================================================================================

static void test_int64_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = slice_int64_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* src = r.u.value;

    push_back_int64_array(src, 1000LL);
    push_back_int64_array(src, 2000LL);
    push_back_int64_array(src, 3000LL);
    push_back_int64_array(src, 4000LL);
    push_back_int64_array(src, 5000LL);

    int64_array_expect_t sr = slice_int64_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    int64_array_t* slc = sr.u.value;
    assert_int_equal((int)int64_array_size(slc), 3);

    int64_t val = 0;
    get_int64_array_index(slc, 0, &val);  assert_true(val == 2000LL);
    get_int64_array_index(slc, 1, &val);  assert_true(val == 3000LL);
    get_int64_array_index(slc, 2, &val);  assert_true(val == 4000LL);

    return_int64_array(src);
    return_int64_array(slc);
}

// ================================================================================
// Group 15: reverse_int64_array
// ================================================================================

static void test_int64_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_int64_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, 1000LL);
    push_back_int64_array(arr, 2000LL);
    push_back_int64_array(arr, 3000LL);

    assert_int_equal(reverse_int64_array(arr), NO_ERROR);

    int64_t val = 0;
    get_int64_array_index(arr, 0, &val);  assert_true(val == 3000LL);
    get_int64_array_index(arr, 2, &val);  assert_true(val == 1000LL);

    return_int64_array(arr);
}

// ================================================================================
// Group 16: sort_int64_array
// ================================================================================

static void test_int64_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_int64_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_sort_forward_basic(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr,  3000LL);
    push_back_int64_array(arr, -1000LL);
    push_back_int64_array(arr,  2000LL);

    assert_int_equal(sort_int64_array(arr, FORWARD), NO_ERROR);

    int64_t val = 0;
    get_int64_array_index(arr, 0, &val);  assert_true(val == -1000LL);
    get_int64_array_index(arr, 1, &val);  assert_true(val ==  2000LL);
    get_int64_array_index(arr, 2, &val);  assert_true(val ==  3000LL);

    return_int64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int64_sort_comparator_correct_signed(void** state) {
    (void)state;
    /*
     * A subtract-based comparator overflows for int64_t: the difference
     * INT64_MIN - INT64_MAX is outside the range of int64_t and produces
     * undefined behaviour. The three-way comparison (va > vb) - (va < vb)
     * is always correct regardless of magnitude.
     */
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, INT64_MAX);
    push_back_int64_array(arr,      0LL);
    push_back_int64_array(arr, INT64_MIN);

    assert_int_equal(sort_int64_array(arr, FORWARD), NO_ERROR);

    int64_t val = 0;
    get_int64_array_index(arr, 0, &val);  assert_true(val == INT64_MIN);
    get_int64_array_index(arr, 1, &val);  assert_true(val == 0LL);
    get_int64_array_index(arr, 2, &val);  assert_true(val == INT64_MAX);

    return_int64_array(arr);
}

// ================================================================================
// Group 17: int64_array_contains
// ================================================================================

static void test_int64_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = int64_array_contains(NULL, 0LL, 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_contains_finds_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, -1000LL);
    push_back_int64_array(arr,  2000LL);
    push_back_int64_array(arr, -3000LL);

    size_expect_t sr = int64_array_contains(arr, 2000LL, 0, 3);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    return_int64_array(arr);
}

// ================================================================================
// Group 18: int64_array_binary_search
// ================================================================================

static void test_int64_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = int64_array_binary_search(NULL, 0LL, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_binary_search_finds_value_with_sort(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr,  3000LL);
    push_back_int64_array(arr, -1000LL);
    push_back_int64_array(arr,  2000LL);
    push_back_int64_array(arr,  4000LL);

    /* sort == true: arr becomes [-1000, 2000, 3000, 4000] */
    size_expect_t sr = int64_array_binary_search(arr, 3000LL, true);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 2);

    return_int64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int64_binary_search_comparator_correct_signed(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    /* Already sorted across the full signed range */
    push_back_int64_array(arr, INT64_MIN);
    push_back_int64_array(arr,      -1LL);
    push_back_int64_array(arr,       0LL);
    push_back_int64_array(arr, INT64_MAX);

    size_expect_t sr = int64_array_binary_search(arr, INT64_MIN, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    sr = int64_array_binary_search(arr, INT64_MAX, false);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 3);

    return_int64_array(arr);
}

// ================================================================================
// Group 19: int64_array_binary_bracket
// ================================================================================

static void test_int64_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = int64_array_binary_bracket(NULL, 0LL, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, -2000LL);
    push_back_int64_array(arr, -1000LL);
    push_back_int64_array(arr,  1000LL);
    push_back_int64_array(arr,  2000LL);

    bracket_expect_t br = int64_array_binary_bracket(arr, -1000LL, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_int64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int64_binary_bracket_comparator_correct_signed(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    /* [INT64_MIN, -1000, 1000, INT64_MAX] — gap between -1000 and 1000 */
    push_back_int64_array(arr, INT64_MIN);
    push_back_int64_array(arr,   -1000LL);
    push_back_int64_array(arr,    1000LL);
    push_back_int64_array(arr, INT64_MAX);

    /* 0 falls in the gap */
    bracket_expect_t br = int64_array_binary_bracket(arr, 0LL, false);
    assert_true(br.has_value);
    /* lower = first element >= 0 = index 2 (value  1000) */
    /* upper = last  element <= 0 = index 1 (value -1000) */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_int64_array(arr);
}

// ================================================================================
// Group 20: int64_array_size
// ================================================================================

static void test_int64_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int64_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int64_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    assert_int_equal((int)int64_array_size(arr), 0);
    push_back_int64_array(arr, -1000LL);
    push_back_int64_array(arr,  2000LL);
    assert_int_equal((int)int64_array_size(arr), 2);

    return_int64_array(arr);
}

// ================================================================================
// Group 21: int64_array_alloc
// ================================================================================

static void test_int64_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int64_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int64_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)int64_array_alloc(r.u.value), 16);
    return_int64_array(r.u.value);
}

// ================================================================================
// Group 22: int64_array_data_size
// ================================================================================

static void test_int64_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int64_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_int64_data_size_is_eight(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)int64_array_data_size(r.u.value), 8);
    return_int64_array(r.u.value);
}

// ================================================================================
// Group 23: is_int64_array_empty
// ================================================================================

static void test_int64_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int64_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_int64_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(4, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    assert_true(is_int64_array_empty(arr));
    push_back_int64_array(arr, -1000LL);
    assert_false(is_int64_array_empty(arr));

    return_int64_array(arr);
}

// ================================================================================
// Group 24: is_int64_array_full
// ================================================================================

static void test_int64_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int64_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_int64_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(2, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    assert_false(is_int64_array_full(arr));
    push_back_int64_array(arr, -1000LL);
    push_back_int64_array(arr,  2000LL);
    assert_true(is_int64_array_full(arr));

    return_int64_array(arr);
}

// ================================================================================
// Group 25: is_int64_array_ptr
// ================================================================================

static void test_int64_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    int64_t val = 0;
    assert_false(is_int64_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_int64_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, 1000LL);
    push_back_int64_array(arr, 2000LL);
    push_back_int64_array(arr, 3000LL);
    /* arr contains [1000, 2000, 3000], len == 3, alloc == 8 */

    const int64_t* first = (const int64_t*)arr->base.data;
    const int64_t* last  = first + 2;
    const int64_t* spare = first + 3;   /* beyond live region */

    assert_true (is_int64_array_ptr(arr, first));
    assert_true (is_int64_array_ptr(arr, last));
    assert_false(is_int64_array_ptr(arr, spare));
    assert_false(is_int64_array_ptr(arr, NULL));

    return_int64_array(arr);
}

// ================================================================================
// Group 26: int64_array_min
// ================================================================================

static void test_int64_min_null_array_returns_null_pointer(void** state) {
    (void)state;
    int64_expect_t r = int64_array_min(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_min_returns_smallest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr,  3000000LL);
    push_back_int64_array(arr, -1000000LL);
    push_back_int64_array(arr,  2000000LL);
    /* arr: [3000000, -1000000, 2000000] — minimum is -1000000 */

    int64_expect_t result = int64_array_min(arr);
    assert_true(result.has_value);
    assert_true(result.u.value == -1000000LL);

    return_int64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int64_min_negative_values_and_boundary(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    /* Span the full signed range; INT64_MIN must be returned. */
    push_back_int64_array(arr,       0LL);
    push_back_int64_array(arr, INT64_MAX);
    push_back_int64_array(arr, INT64_MIN);
    push_back_int64_array(arr,      -1LL);
    /* arr: [0, INT64_MAX, INT64_MIN, -1] */

    int64_expect_t result = int64_array_min(arr);
    assert_true(result.has_value);
    assert_true(result.u.value == INT64_MIN);

    return_int64_array(arr);
}

// ================================================================================
// Group 27: int64_array_max
// ================================================================================

static void test_int64_max_null_array_returns_null_pointer(void** state) {
    (void)state;
    int64_expect_t r = int64_array_max(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_max_returns_largest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    push_back_int64_array(arr, -2000000LL);
    push_back_int64_array(arr,  3000000LL);
    push_back_int64_array(arr,  1000000LL);
    /* arr: [-2000000, 3000000, 1000000] — maximum is 3000000 */

    int64_expect_t result = int64_array_max(arr);
    assert_true(result.has_value);
    assert_true(result.u.value == 3000000LL);

    return_int64_array(arr);
}

// --------------------------------------------------------------------------------

static void test_int64_max_negative_values_and_boundary(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* arr = r.u.value;

    /* Span the full signed range; INT64_MAX must be returned. */
    push_back_int64_array(arr,      -1LL);
    push_back_int64_array(arr, INT64_MIN);
    push_back_int64_array(arr, INT64_MAX);
    push_back_int64_array(arr,       0LL);
    /* arr: [-1, INT64_MIN, INT64_MAX, 0] */

    int64_expect_t result = int64_array_max(arr);
    assert_true(result.has_value);
    assert_true(result.u.value == INT64_MAX);

    return_int64_array(arr);
}

// ================================================================================
// Group 28: int64_array_sum
// ================================================================================

// static void test_int64_sum_null_array_returns_null_pointer(void** state) {
//     (void)state;
//     int64_expect_t r = int64_array_sum(NULL);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int64_sum_returns_correct_total(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     int64_array_expect_t r = init_int64_array(8, false, alloc);
//     assert_true(r.has_value);
//     int64_array_t* arr = r.u.value;
//
//     push_back_int64_array(arr,  1000000000LL);
//     push_back_int64_array(arr, -2000000000LL);
//     push_back_int64_array(arr,  3000000000LL);
//     /* arr: [1000000000, -2000000000, 3000000000], sum == 2000000000 */
//
//     int64_expect_t result = int64_array_sum(arr);
//     assert_true(result.has_value);
//     assert_true(result.u.value == 2000000000LL);
//
//     return_int64_array(arr);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int64_sum_mixed_signs_cancel(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     int64_array_expect_t r = init_int64_array(8, false, alloc);
//     assert_true(r.has_value);
//     int64_array_t* arr = r.u.value;
//
//     push_back_int64_array(arr,  100000000000LL);
//     push_back_int64_array(arr, -100000000000LL);
//     push_back_int64_array(arr,   50000000000LL);
//     push_back_int64_array(arr,  -50000000000LL);
//     /* arr: [1e11, -1e11, 5e10, -5e10], sum == 0 */
//
//     int64_expect_t result = int64_array_sum(arr);
//     assert_true(result.has_value);
//     assert_true(result.u.value == 0LL);
//
//     return_int64_array(arr);
// }

// ================================================================================
// Group 29: cumulative_int64_array
// ================================================================================

static void test_int64_cumulative_null_array_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = cumulative_int64_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int64_cumulative_produces_prefix_sum(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* src = r.u.value;

    push_back_int64_array(src, 1000LL);
    push_back_int64_array(src, 2000LL);
    push_back_int64_array(src, 3000LL);
    push_back_int64_array(src, 4000LL);
    /* src: [1000, 2000, 3000, 4000]
     * expected output: [1000, 3000, 6000, 10000] */

    int64_array_expect_t cr = cumulative_int64_array(src, alloc);
    assert_true(cr.has_value);
    int64_array_t* dst = cr.u.value;
    assert_int_equal((int)int64_array_size(dst), 4);

    int64_t val = 0;
    get_int64_array_index(dst, 0, &val);  assert_true(val ==  1000LL);
    get_int64_array_index(dst, 1, &val);  assert_true(val ==  3000LL);
    get_int64_array_index(dst, 2, &val);  assert_true(val ==  6000LL);
    get_int64_array_index(dst, 3, &val);  assert_true(val == 10000LL);

    return_int64_array(src);
    return_int64_array(dst);
}

// --------------------------------------------------------------------------------

static void test_int64_cumulative_mixed_signs(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_array_expect_t r = init_int64_array(8, false, alloc);
    assert_true(r.has_value);
    int64_array_t* src = r.u.value;

    push_back_int64_array(src,  5000000000LL);
    push_back_int64_array(src, -3000000000LL);
    push_back_int64_array(src,  2000000000LL);
    push_back_int64_array(src, -1000000000LL);
    /* src: [5e9, -3e9, 2e9, -1e9]
     * expected output: [5e9, 2e9, 4e9, 3e9] */

    int64_array_expect_t cr = cumulative_int64_array(src, alloc);
    assert_true(cr.has_value);
    int64_array_t* dst = cr.u.value;
    assert_int_equal((int)int64_array_size(dst), 4);

    int64_t val = 0;
    get_int64_array_index(dst, 0, &val);  assert_true(val ==  5000000000LL);
    get_int64_array_index(dst, 1, &val);  assert_true(val ==  2000000000LL);
    get_int64_array_index(dst, 2, &val);  assert_true(val ==  4000000000LL);
    get_int64_array_index(dst, 3, &val);  assert_true(val ==  3000000000LL);

    return_int64_array(src);
    return_int64_array(dst);
}
// -------------------------------------------------------------------------------- 

// static int64_array_t* _make_int64_array(size_t capacity, bool growth) {
//     allocator_vtable_t a = heap_allocator();
//     int64_array_expect_t r = init_int64_array(capacity, growth, a);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     return r.u.value;
// }
//
// // --------------------------------------------------------------------------------
//
// static int64_array_t* _make_sample_int64_array(void) {
//     int64_array_t* array = _make_int64_array(8u, true);
//
//     assert_int_equal(push_back_int64_array(array, 1u), NO_ERROR);
//     assert_int_equal(push_back_int64_array(array, 2u), NO_ERROR);
//     assert_int_equal(push_back_int64_array(array, 3u), NO_ERROR);
//     assert_int_equal(push_back_int64_array(array, 4u), NO_ERROR);
//     assert_int_equal(push_back_int64_array(array, 5u), NO_ERROR);
//
//     return array;
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_int64_add_scalar_array_null_returns_null_pointer(void** state) {
//     (void)state;
//
//     assert_int_equal(int64_add_scalar_array(NULL, 5u), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int64_add_scalar_array_empty_returns_empty(void** state) {
//     (void)state;
//
//     int64_array_t* array = _make_int64_array(4u, true);
//
//     assert_int_equal(int64_add_scalar_array(array, 5u), EMPTY);
//
//     return_int64_array(array);
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_int64_add_scalar_array_add_zero_leaves_values_unchanged(void** state) {
//     (void)state;
//
//     int64_array_t* array = _make_sample_int64_array();
//
//     assert_int_equal(int64_add_scalar_array(array, 0u), NO_ERROR);
//
//     int64_t out = 0u;
//
//     assert_int_equal(get_int64_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 1u);
//
//     assert_int_equal(get_int64_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 2u);
//
//     assert_int_equal(get_int64_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 3u);
//
//     assert_int_equal(get_int64_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 4u);
//
//     assert_int_equal(get_int64_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 5u);
//
//     return_int64_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int64_add_scalar_array_single_element_updates_value(void** state) {
//     (void)state;
//
//     int64_array_t* array = _make_int64_array(4u, true);
//     assert_int_equal(push_back_int64_array(array, 10u), NO_ERROR);
//
//     assert_int_equal(int64_add_scalar_array(array, 7u), NO_ERROR);
//
//     int64_t out = 0u;
//     assert_int_equal(get_int64_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 17u);
//
//     return_int64_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int64_add_scalar_array_updates_all_elements(void** state) {
//     (void)state;
//
//     int64_array_t* array = _make_sample_int64_array();
//
//     assert_int_equal(int64_add_scalar_array(array, 10u), NO_ERROR);
//
//     int64_t out = 0u;
//
//     assert_int_equal(get_int64_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 11u);
//
//     assert_int_equal(get_int64_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 12u);
//
//     assert_int_equal(get_int64_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 13u);
//
//     assert_int_equal(get_int64_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 14u);
//
//     assert_int_equal(get_int64_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 15u);
//
//     return_int64_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int64_add_scalar_array_preserves_length(void** state) {
//     (void)state;
//
//     int64_array_t* array = _make_sample_int64_array();
//     size_t before = int64_array_size(array);
//
//     assert_int_equal(int64_add_scalar_array(array, 3u), NO_ERROR);
//
//     assert_int_equal(int64_array_size(array), before);
//
//     return_int64_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_int64_add_scalar_array_multiple_calls_accumulate(void** state) {
//     (void)state;
//
//     int64_array_t* array = _make_sample_int64_array();
//
//     assert_int_equal(int64_add_scalar_array(array, 2u), NO_ERROR);
//     assert_int_equal(int64_add_scalar_array(array, 3u), NO_ERROR);
//
//     int64_t out = 0u;
//
//     assert_int_equal(get_int64_array_index(array, 0u, &out), NO_ERROR);
//     assert_int_equal(out, 6u);
//
//     assert_int_equal(get_int64_array_index(array, 1u, &out), NO_ERROR);
//     assert_int_equal(out, 7u);
//
//     assert_int_equal(get_int64_array_index(array, 2u, &out), NO_ERROR);
//     assert_int_equal(out, 8u);
//
//     assert_int_equal(get_int64_array_index(array, 3u, &out), NO_ERROR);
//     assert_int_equal(out, 9u);
//
//     assert_int_equal(get_int64_array_index(array, 4u, &out), NO_ERROR);
//     assert_int_equal(out, 10u);
//
//     return_int64_array(array);
// }
// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_int64_array[] = {

    /* Group 1: init_int64_array */
    cmocka_unit_test(test_int64_init_null_allocate_fn_fails),
    cmocka_unit_test(test_int64_init_returns_valid_array),

    /* Group 2: return_int64_array */
    cmocka_unit_test(test_int64_return_null_is_safe),
    cmocka_unit_test(test_int64_return_valid_array_does_not_crash),

    /* Group 3: push_back_int64_array */
    cmocka_unit_test(test_int64_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_push_back_appends_value),

    /* Group 4: push_front_int64_array */
    cmocka_unit_test(test_int64_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_push_front_prepends_value),

    /* Group 5: push_at_int64_array */
    cmocka_unit_test(test_int64_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_push_at_inserts_at_index),

    /* Group 6: get_int64_array_index */
    cmocka_unit_test(test_int64_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_get_returns_correct_value),

    /* Group 7: pop_back_int64_array */
    cmocka_unit_test(test_int64_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_pop_back_removes_last_element),

    /* Group 8: pop_front_int64_array */
    cmocka_unit_test(test_int64_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_pop_front_removes_first_element),

    /* Group 9: pop_any_int64_array */
    cmocka_unit_test(test_int64_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_pop_any_removes_element_at_index),

    /* Group 10: clear_int64_array */
    cmocka_unit_test(test_int64_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_clear_resets_len_to_zero),

    /* Group 11: set_int64_array_index */
    cmocka_unit_test(test_int64_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_set_overwrites_element),

    /* Group 12: copy_int64_array */
    cmocka_unit_test(test_int64_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_int64_copy_produces_independent_array),

    /* Group 13: concat_int64_array */
    cmocka_unit_test(test_int64_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_int64_concat_appends_elements),

    /* Group 14: slice_int64_array */
    cmocka_unit_test(test_int64_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_int64_slice_returns_correct_subrange),

    /* Group 15: reverse_int64_array */
    cmocka_unit_test(test_int64_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_reverse_reverses_elements),

    /* Group 16: sort_int64_array */
    cmocka_unit_test(test_int64_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_sort_forward_basic),
    cmocka_unit_test(test_int64_sort_comparator_correct_signed),

    /* Group 17: int64_array_contains */
    cmocka_unit_test(test_int64_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_contains_finds_value),

    /* Group 18: int64_array_binary_search */
    cmocka_unit_test(test_int64_binary_search_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_binary_search_finds_value_with_sort),
    cmocka_unit_test(test_int64_binary_search_comparator_correct_signed),

    /* Group 19: int64_array_binary_bracket */
    cmocka_unit_test(test_int64_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_binary_bracket_exact_match),
    cmocka_unit_test(test_int64_binary_bracket_comparator_correct_signed),

    /* Group 20: int64_array_size */
    cmocka_unit_test(test_int64_size_null_returns_zero),
    cmocka_unit_test(test_int64_size_reflects_push_count),

    /* Group 21: int64_array_alloc */
    cmocka_unit_test(test_int64_alloc_null_returns_zero),
    cmocka_unit_test(test_int64_alloc_matches_capacity),

    /* Group 22: int64_array_data_size */
    cmocka_unit_test(test_int64_data_size_null_returns_zero),
    cmocka_unit_test(test_int64_data_size_is_eight),

    /* Group 23: is_int64_array_empty */
    cmocka_unit_test(test_int64_empty_null_returns_true),
    cmocka_unit_test(test_int64_empty_reflects_contents),

    /* Group 24: is_int64_array_full */
    cmocka_unit_test(test_int64_full_null_returns_true),
    cmocka_unit_test(test_int64_full_reflects_capacity),

    /* Group 25: is_int64_array_ptr */
    cmocka_unit_test(test_int64_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_int64_is_ptr_valid_and_invalid),

    /* Group 26: int64_array_min */
    cmocka_unit_test(test_int64_min_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_min_returns_smallest_value),
    cmocka_unit_test(test_int64_min_negative_values_and_boundary),

    /* Group 27: int64_array_max */
    cmocka_unit_test(test_int64_max_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_max_returns_largest_value),
    cmocka_unit_test(test_int64_max_negative_values_and_boundary),

    /* Group 28: int64_array_sum */
    // cmocka_unit_test(test_int64_sum_null_array_returns_null_pointer),
    // cmocka_unit_test(test_int64_sum_returns_correct_total),
    // cmocka_unit_test(test_int64_sum_mixed_signs_cancel),

    /* Group 29: cumulative_int64_array */
    cmocka_unit_test(test_int64_cumulative_null_array_returns_null_pointer),
    cmocka_unit_test(test_int64_cumulative_produces_prefix_sum),
    cmocka_unit_test(test_int64_cumulative_mixed_signs),

    // cmocka_unit_test(test_int64_add_scalar_array_null_returns_null_pointer),
    // cmocka_unit_test(test_int64_add_scalar_array_empty_returns_empty),
    // cmocka_unit_test(test_int64_add_scalar_array_add_zero_leaves_values_unchanged),
    // cmocka_unit_test(test_int64_add_scalar_array_single_element_updates_value),
    // cmocka_unit_test(test_int64_add_scalar_array_updates_all_elements),
    // cmocka_unit_test(test_int64_add_scalar_array_preserves_length),
    // cmocka_unit_test(test_int64_add_scalar_array_multiple_calls_accumulate),
};
const size_t test_int64_array_count = sizeof(test_int64_array) / sizeof(test_int64_array[0]);
// ================================================================================ 
// ================================================================================ 

/* Tolerance-free equality for exact binary fractions used in tests. */
#define ASSERT_FLOAT_EXACT(a, b) assert_true((a) == (b))

// ================================================================================
// Group 1: init_float_array
// ================================================================================

static void test_float_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    float_array_expect_t r = init_float_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)float_array_size(r.u.value), 0);
    assert_int_equal((int)float_array_alloc(r.u.value), 8);
    return_float_array(r.u.value);
}

// ================================================================================
// Group 2: return_float_array
// ================================================================================

static void test_float_return_null_is_safe(void** state) {
    (void)state;
    return_float_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_float_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    return_float_array(r.u.value);
}

// ================================================================================
// Group 3: push_back_float_array
// ================================================================================

static void test_float_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_float_array(NULL, 1.0f), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    assert_int_equal(push_back_float_array(arr, -1.0f), NO_ERROR);
    assert_int_equal(push_back_float_array(arr,  0.0f), NO_ERROR);
    assert_int_equal(push_back_float_array(arr,  1.0f), NO_ERROR);
    assert_int_equal((int)float_array_size(arr), 3);

    float val = 0.0f;
    get_float_array_index(arr, 0, &val);  ASSERT_FLOAT_EXACT(val, -1.0f);
    get_float_array_index(arr, 2, &val);  ASSERT_FLOAT_EXACT(val,  1.0f);

    return_float_array(arr);
}

// ================================================================================
// Group 4: push_front_float_array
// ================================================================================

static void test_float_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_float_array(NULL, 1.0f), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,  2.0f);
    push_front_float_array(arr, 1.0f);
    /* arr is [1.0, 2.0] */

    float val = 0.0f;
    get_float_array_index(arr, 0, &val);
    ASSERT_FLOAT_EXACT(val, 1.0f);

    return_float_array(arr);
}

// ================================================================================
// Group 5: push_at_float_array
// ================================================================================

static void test_float_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_float_array(NULL, 0, 1.0f), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 3.0f);
    push_at_float_array(arr, 1, 2.0f);
    /* arr is [1.0, 2.0, 3.0] */

    float val = 0.0f;
    get_float_array_index(arr, 1, &val);
    ASSERT_FLOAT_EXACT(val, 2.0f);

    return_float_array(arr);
}

// ================================================================================
// Group 6: get_float_array_index
// ================================================================================

static void test_float_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    float val = 0.0f;
    assert_int_equal(get_float_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, -1.5f);
    push_back_float_array(arr,  0.0f);
    push_back_float_array(arr,  1.5f);

    float val = 0.0f;
    assert_int_equal(get_float_array_index(arr, 0, &val), NO_ERROR);
    ASSERT_FLOAT_EXACT(val, -1.5f);
    assert_int_equal(get_float_array_index(arr, 2, &val), NO_ERROR);
    ASSERT_FLOAT_EXACT(val,  1.5f);

    return_float_array(arr);
}

// ================================================================================
// Group 7: pop_back_float_array
// ================================================================================

static void test_float_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_float_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);

    float val = 0.0f;
    assert_int_equal(pop_back_float_array(arr, &val), NO_ERROR);
    ASSERT_FLOAT_EXACT(val, 2.0f);
    assert_int_equal((int)float_array_size(arr), 1);

    return_float_array(arr);
}

// ================================================================================
// Group 8: pop_front_float_array
// ================================================================================

static void test_float_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_float_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);

    float val = 0.0f;
    assert_int_equal(pop_front_float_array(arr, &val), NO_ERROR);
    ASSERT_FLOAT_EXACT(val, 1.0f);
    assert_int_equal((int)float_array_size(arr), 1);

    return_float_array(arr);
}

// ================================================================================
// Group 9: pop_any_float_array
// ================================================================================

static void test_float_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_float_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);
    push_back_float_array(arr, 3.0f);

    float val = 0.0f;
    assert_int_equal(pop_any_float_array(arr, &val, 1), NO_ERROR);
    ASSERT_FLOAT_EXACT(val, 2.0f);
    assert_int_equal((int)float_array_size(arr), 2);

    return_float_array(arr);
}

// ================================================================================
// Group 10: clear_float_array
// ================================================================================

static void test_float_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_float_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);
    assert_int_equal(clear_float_array(arr), NO_ERROR);
    assert_int_equal((int)float_array_size(arr), 0);
    assert_int_equal((int)float_array_alloc(arr), 4);   /* capacity retained */

    return_float_array(arr);
}

// ================================================================================
// Group 11: set_float_array_index
// ================================================================================

static void test_float_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_float_array_index(NULL, 0, 1.0f), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 1.0f);
    assert_int_equal(set_float_array_index(arr, 0, -99.5f), NO_ERROR);

    float val = 0.0f;
    get_float_array_index(arr, 0, &val);
    ASSERT_FLOAT_EXACT(val, -99.5f);

    return_float_array(arr);
}

// ================================================================================
// Group 12: copy_float_array
// ================================================================================

static void test_float_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = copy_float_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* src = r.u.value;

    push_back_float_array(src, 1.0f);
    push_back_float_array(src, 2.0f);

    float_array_expect_t cr = copy_float_array(src, alloc);
    assert_true(cr.has_value);
    float_array_t* dst = cr.u.value;

    /* Mutating src must not affect the copy */
    set_float_array_index(src, 0, 999.0f);
    float val = 0.0f;
    get_float_array_index(dst, 0, &val);
    ASSERT_FLOAT_EXACT(val, 1.0f);

    return_float_array(src);
    return_float_array(dst);
}

// ================================================================================
// Group 13: concat_float_array
// ================================================================================

static void test_float_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* src = r.u.value;
    assert_int_equal(concat_float_array(NULL, src), NULL_POINTER);
    return_float_array(src);
}

// --------------------------------------------------------------------------------

static void test_float_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r1 = init_float_array(8, false, alloc);
    float_array_expect_t r2 = init_float_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    float_array_t* dst = r1.u.value;
    float_array_t* src = r2.u.value;

    push_back_float_array(dst, 1.0f);
    push_back_float_array(src, 2.0f);

    assert_int_equal(concat_float_array(dst, src), NO_ERROR);
    assert_int_equal((int)float_array_size(dst), 2);

    float val = 0.0f;
    get_float_array_index(dst, 1, &val);
    ASSERT_FLOAT_EXACT(val, 2.0f);

    return_float_array(dst);
    return_float_array(src);
}

// ================================================================================
// Group 14: slice_float_array
// ================================================================================

static void test_float_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = slice_float_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* src = r.u.value;

    push_back_float_array(src, 1.0f);
    push_back_float_array(src, 2.0f);
    push_back_float_array(src, 3.0f);
    push_back_float_array(src, 4.0f);
    push_back_float_array(src, 5.0f);

    float_array_expect_t sr = slice_float_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    float_array_t* slc = sr.u.value;
    assert_int_equal((int)float_array_size(slc), 3);

    float val = 0.0f;
    get_float_array_index(slc, 0, &val);  ASSERT_FLOAT_EXACT(val, 2.0f);
    get_float_array_index(slc, 1, &val);  ASSERT_FLOAT_EXACT(val, 3.0f);
    get_float_array_index(slc, 2, &val);  ASSERT_FLOAT_EXACT(val, 4.0f);

    return_float_array(src);
    return_float_array(slc);
}

// ================================================================================
// Group 15: reverse_float_array
// ================================================================================

static void test_float_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_float_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);
    push_back_float_array(arr, 3.0f);

    assert_int_equal(reverse_float_array(arr), NO_ERROR);

    float val = 0.0f;
    get_float_array_index(arr, 0, &val);  ASSERT_FLOAT_EXACT(val, 3.0f);
    get_float_array_index(arr, 2, &val);  ASSERT_FLOAT_EXACT(val, 1.0f);

    return_float_array(arr);
}

// ================================================================================
// Group 16: sort_float_array
// ================================================================================

static void test_float_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_float_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_sort_forward_basic(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,  3.0f);
    push_back_float_array(arr, -1.5f);
    push_back_float_array(arr,  2.5f);

    assert_int_equal(sort_float_array(arr, FORWARD), NO_ERROR);

    float val = 0.0f;
    get_float_array_index(arr, 0, &val);  ASSERT_FLOAT_EXACT(val, -1.5f);
    get_float_array_index(arr, 1, &val);  ASSERT_FLOAT_EXACT(val,  2.5f);
    get_float_array_index(arr, 2, &val);  ASSERT_FLOAT_EXACT(val,  3.0f);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_sort_negative_values_and_infinity(void** state) {
    (void)state;
    /*
     * IEEE 754 total order for finite floats and infinities:
     *   -INFINITY < negative finite values < 0.0 < positive finite values < INFINITY
     *
     * The three-way comparator (va > vb) - (va < vb) handles infinities
     * correctly because the relational operators are defined for infinities
     * by IEEE 754.
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,  1.0f);
    push_back_float_array(arr, -INFINITY);
    push_back_float_array(arr, -1.0f);
    push_back_float_array(arr,  INFINITY);
    push_back_float_array(arr,  0.0f);

    assert_int_equal(sort_float_array(arr, FORWARD), NO_ERROR);

    float val = 0.0f;
    get_float_array_index(arr, 0, &val);  assert_true(isinf(val) && val < 0.0f);
    get_float_array_index(arr, 1, &val);  ASSERT_FLOAT_EXACT(val, -1.0f);
    get_float_array_index(arr, 2, &val);  ASSERT_FLOAT_EXACT(val,  0.0f);
    get_float_array_index(arr, 3, &val);  ASSERT_FLOAT_EXACT(val,  1.0f);
    get_float_array_index(arr, 4, &val);  assert_true(isinf(val) && val > 0.0f);

    return_float_array(arr);
}

// ================================================================================
// Group 17: float_array_contains
// ================================================================================

static void test_float_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = float_array_contains(NULL, 0.0f, 0, 1, 0.0f, 0.0f);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_contains_negative_tolerance_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;
    push_back_float_array(arr, 1.0f);

    size_expect_t sr = float_array_contains(arr, 1.0f, 0, 1, -0.1f, 0.0f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    sr = float_array_contains(arr, 1.0f, 0, 1, 0.0f, -0.1f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_contains_exact_mode_finds_value(void** state) {
    (void)state;
    /*
     * Exact mode (both tolerances 0.0f): bitwise equality.
     * -0.0f and +0.0f have different bit patterns so they do NOT match.
     * NaN != NaN so a stored NaN is never found.
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, -1.0f);
    push_back_float_array(arr,  2.5f);
    push_back_float_array(arr, -3.0f);
    push_back_float_array(arr,  0.0f);  /* +0.0f */

    /* Value present — found at correct index. */
    size_expect_t sr = float_array_contains(arr, 2.5f, 0, 4, 0.0f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    /* Searching for -0.0f must NOT find +0.0f at index 3. */
    sr = float_array_contains(arr, -0.0f, 0, 4, 0.0f, 0.0f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    /* Searching for +0.0f must find it at index 3. */
    sr = float_array_contains(arr, 0.0f, 0, 4, 0.0f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 3);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_contains_absolute_tolerance(void** state) {
    (void)state;
    /*
     * Absolute mode (rel_tol == 0.0f): match when |element - needle| <= abs_tol.
     * arr: [0.0, 1.0, 2.0, 3.0]
     * Searching for 1.05 with abs_tol 0.1 should find index 1 (|1.0 - 1.05| = 0.05).
     * Searching for 1.5  with abs_tol 0.1 should NOT find anything (gap is 0.5).
     * -0.0f and +0.0f now match each other because |(-0.0f) - 0.0f| = 0.0 <= tol.
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 0.0f);
    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);
    push_back_float_array(arr, 3.0f);

    size_expect_t sr = float_array_contains(arr, 1.05f, 0, 4, 0.1f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = float_array_contains(arr, 1.5f, 0, 4, 0.1f, 0.0f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    /* -0.0f vs +0.0f: diff == 0.0 <= abs_tol, so they match in tolerance mode. */
    sr = float_array_contains(arr, -0.0f, 0, 1, 1e-9f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_contains_relative_tolerance(void** state) {
    (void)state;
    /*
     * Relative mode (abs_tol == 0.0f): match when
     *   |element - needle| <= rel_tol * max(|element|, |needle|).
     *
     * arr: [100.0, 1000.0, 10000.0]
     * rel_tol 0.01 (1%):
     *   needle 1005.0 vs 1000.0: |diff|=5, threshold=0.01*max(1000,1005)=10.05 — match
     *   needle  110.0 vs  100.0: |diff|=10, threshold=0.01*max(100,110)=1.1    — no match
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,   100.0f);
    push_back_float_array(arr,  1000.0f);
    push_back_float_array(arr, 10000.0f);

    size_expect_t sr = float_array_contains(arr, 1005.0f, 0, 3, 0.0f, 0.01f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = float_array_contains(arr, 110.0f, 0, 3, 0.0f, 0.01f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_contains_combined_tolerance(void** state) {
    (void)state;
    /*
     * Combined mode (both > 0.0f): threshold = max(abs_tol, rel_tol * max(|e|,|n|)).
     *
     * This ensures abs_tol acts as a floor near zero where pure relative
     * tolerance collapses.
     *
     * arr: [0.0, 1000.0]
     * needle = 0.0, abs_tol = 1e-4, rel_tol = 0.01
     *   For element 0.0:   diff=0, threshold=max(1e-4, 0.01*0)=1e-4 — match
     *   For element 1000:  diff=1000, threshold=max(1e-4, 0.01*1000)=10 — no match
     *
     * Confirm abs_tol floor: needle = 1e-7 (tiny), abs_tol = 1e-4
     *   element 0.0: diff=1e-7, threshold >= 1e-4 — match (abs_tol rescues near zero)
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,    0.0f);
    push_back_float_array(arr, 1000.0f);

    /* Combined — element 0.0 matches needle 0.0 */
    size_expect_t sr = float_array_contains(arr, 0.0f, 0, 2, 1e-4f, 0.01f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    /* abs_tol floor: needle 1e-7 is within abs_tol of element 0.0 */
    sr = float_array_contains(arr, 1e-7f, 0, 1, 1e-4f, 0.01f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    /* Pure relative alone would miss this — confirm abs_tol is doing the work */
    sr = float_array_contains(arr, 1e-7f, 0, 1, 0.0f, 0.01f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_float_array(arr);
}


// ================================================================================
// Group 18: float_array_binary_search
// ================================================================================

static void test_float_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = float_array_binary_search(NULL, 0.0f, false, 0.0f, 0.0f);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_binary_search_negative_tolerance_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;
    push_back_float_array(arr, 1.0f);

    size_expect_t sr = float_array_binary_search(arr, 1.0f, false, -0.1f, 0.0f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    sr = float_array_binary_search(arr, 1.0f, false, 0.0f, -0.1f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_binary_search_exact_finds_value_with_sort(void** state) {
    (void)state;
    /*
     * Exact mode (both tolerances 0.0f): confirms the sort path and that the
     * first occurrence of a duplicate is returned.
     * arr (unsorted): [3.0, -1.5, 2.5, 2.5, 4.0]
     * After sort:     [-1.5, 2.5, 2.5, 3.0, 4.0]
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,  3.0f);
    push_back_float_array(arr, -1.5f);
    push_back_float_array(arr,  2.5f);
    push_back_float_array(arr,  2.5f);
    push_back_float_array(arr,  4.0f);

    size_expect_t sr = float_array_binary_search(arr, 2.5f, true, 0.0f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);   /* first of the two 2.5 entries */

    sr = float_array_binary_search(arr, 99.0f, false, 0.0f, 0.0f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_binary_search_exact_negative_and_infinity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    /* Already sorted: [-INFINITY, -1.0, 0.0, 1.0, INFINITY] */
    push_back_float_array(arr, -INFINITY);
    push_back_float_array(arr,    -1.0f);
    push_back_float_array(arr,     0.0f);
    push_back_float_array(arr,     1.0f);
    push_back_float_array(arr,  INFINITY);

    size_expect_t sr = float_array_binary_search(arr, -INFINITY, false, 0.0f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    sr = float_array_binary_search(arr, INFINITY, false, 0.0f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 4);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_binary_search_absolute_tolerance(void** state) {
    (void)state;
    /*
     * Absolute tolerance: match when |element - needle| <= abs_tol.
     *
     * arr (sorted): [0.0, 1.0, 2.0, 3.0, 4.0]
     *
     * needle=1.3, abs_tol=0.5: elements 1.0 (diff=0.3) and 2.0 (diff=0.7)
     *   — only 1.0 qualifies; first (lowest) match is index 1.
     *
     * needle=1.5, abs_tol=0.5: elements 1.0 (diff=0.5) and 2.0 (diff=0.5)
     *   — both qualify; lowest index is 1.
     *
     * needle=5.0, abs_tol=0.1: nothing within range → NOT_FOUND.
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 0.0f);
    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);
    push_back_float_array(arr, 3.0f);
    push_back_float_array(arr, 4.0f);

    size_expect_t sr = float_array_binary_search(arr, 1.3f, false, 0.5f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = float_array_binary_search(arr, 1.5f, false, 0.5f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = float_array_binary_search(arr, 5.0f, false, 0.1f, 0.0f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_binary_search_relative_tolerance(void** state) {
    (void)state;
    /*
     * Relative tolerance: match when |element - needle| <=
     *   rel_tol * max(|element|, |needle|).
     *
     * arr (sorted): [100.0, 1000.0, 10000.0]
     * rel_tol = 0.01 (1%):
     *   needle=1005.0: threshold = 0.01 * max(1000, 1005) = 10.05
     *     1000.0: diff=5   <= 10.05 → match at index 1
     *   needle=115.0:  threshold = 0.01 * max(100, 115)  = 1.15
     *     100.0:  diff=15  >  1.15  → no match → NOT_FOUND
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,   100.0f);
    push_back_float_array(arr,  1000.0f);
    push_back_float_array(arr, 10000.0f);

    size_expect_t sr = float_array_binary_search(arr, 1005.0f, false, 0.0f, 0.01f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = float_array_binary_search(arr, 115.0f, false, 0.0f, 0.01f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_binary_search_combined_tolerance(void** state) {
    (void)state;
    /*
     * Combined tolerance: threshold = max(abs_tol,
     *                                     rel_tol * max(|element|, |needle|)).
     * abs_tol acts as a floor so the threshold doesn't collapse near zero.
     *
     * arr (sorted): [0.0, 0.001, 1000.0]
     * abs_tol=1e-3, rel_tol=0.01:
     *   needle=0.0: threshold = max(1e-3, 0.01*0) = 1e-3
     *     0.0:   diff=0    <= 1e-3 → match at index 0
     *     0.001: diff=1e-3 <= 1e-3 → also within tolerance; lowest is index 0
     *
     * needle=1e-6 (tiny): threshold >= abs_tol=1e-3 (floor saves near zero)
     *     0.0:   diff=1e-6 <= 1e-3 → match at index 0
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,    0.0f);
    push_back_float_array(arr,    0.001f);
    push_back_float_array(arr, 1000.0f);

    size_expect_t sr = float_array_binary_search(arr, 0.0f, false, 1e-3f, 0.01f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    sr = float_array_binary_search(arr, 1e-6f, false, 1e-3f, 0.01f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_binary_search_fanout_lands_between_values(void** state) {
    (void)state;
    /*
     * The bisection may land at a position where no exact match exists but a
     * tolerant match is nearby.  This test places needle between two elements
     * that are both within abs_tol, verifying the fan-out finds the lower one.
     *
     * arr (sorted): [1.0, 2.0, 3.0, 4.0]
     * needle=2.4, abs_tol=0.5:
     *   bisection lands at index 2 (3.0), diff=0.6 > 0.5 — no exact match
     *   fan-out left: index 1 (2.0), diff=0.4 <= 0.5 → match, best=1
     *                 index 0 (1.0), diff=1.4 >  0.5 → stop
     *   lowest match is index 1.
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);
    push_back_float_array(arr, 3.0f);
    push_back_float_array(arr, 4.0f);

    size_expect_t sr = float_array_binary_search(arr, 2.4f, false, 0.5f, 0.0f);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    /* needle far outside range → NOT_FOUND even with generous tolerance */
    sr = float_array_binary_search(arr, 100.0f, false, 0.5f, 0.0f);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_float_array(arr);
}

// ================================================================================
// Group 19: float_array_binary_bracket
// ================================================================================

static void test_float_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = float_array_binary_bracket(NULL, 0.0f, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, -2.0f);
    push_back_float_array(arr, -1.0f);
    push_back_float_array(arr,  1.0f);
    push_back_float_array(arr,  2.0f);

    bracket_expect_t br = float_array_binary_bracket(arr, -1.0f, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_binary_bracket_gap_between_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    /* [-2.0, -1.0, 1.0, 2.0] — gap between -1.0 and 1.0 */
    push_back_float_array(arr, -2.0f);
    push_back_float_array(arr, -1.0f);
    push_back_float_array(arr,  1.0f);
    push_back_float_array(arr,  2.0f);

    /* 0.0 falls in the gap */
    bracket_expect_t br = float_array_binary_bracket(arr, 0.0f, false);
    assert_true(br.has_value);
    /* lower = first element >= 0.0 = index 2 (value  1.0) */
    /* upper = last  element <= 0.0 = index 1 (value -1.0) */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_float_array(arr);
}

// ================================================================================
// Group 20: float_array_size
// ================================================================================

static void test_float_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)float_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_float_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    assert_int_equal((int)float_array_size(arr), 0);
    push_back_float_array(arr, -1.0f);
    push_back_float_array(arr,  2.0f);
    assert_int_equal((int)float_array_size(arr), 2);

    return_float_array(arr);
}

// ================================================================================
// Group 21: float_array_alloc
// ================================================================================

static void test_float_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)float_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_float_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)float_array_alloc(r.u.value), 16);
    return_float_array(r.u.value);
}

// ================================================================================
// Group 22: float_array_data_size
// ================================================================================

static void test_float_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)float_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_float_data_size_is_four(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)float_array_data_size(r.u.value), 4);
    return_float_array(r.u.value);
}

// ================================================================================
// Group 23: is_float_array_empty
// ================================================================================

static void test_float_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_float_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_float_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(4, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    assert_true(is_float_array_empty(arr));
    push_back_float_array(arr, 1.0f);
    assert_false(is_float_array_empty(arr));

    return_float_array(arr);
}

// ================================================================================
// Group 24: is_float_array_full
// ================================================================================

static void test_float_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_float_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_float_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(2, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    assert_false(is_float_array_full(arr));
    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);
    assert_true(is_float_array_full(arr));

    return_float_array(arr);
}

// ================================================================================
// Group 25: is_float_array_ptr
// ================================================================================

static void test_float_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    float val = 0.0f;
    assert_false(is_float_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_float_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);
    push_back_float_array(arr, 3.0f);

    const float* first = (const float*)arr->base.data;
    const float* last  = first + 2;
    const float* spare = first + 3;   /* beyond live region */

    assert_true (is_float_array_ptr(arr, first));
    assert_true (is_float_array_ptr(arr, last));
    assert_false(is_float_array_ptr(arr, spare));
    assert_false(is_float_array_ptr(arr, NULL));

    return_float_array(arr);
}

// ================================================================================
// Group 26: float_array_min
// ================================================================================

static void test_float_min_null_array_returns_null_pointer(void** state) {
    (void)state;
    float_expect_t r = float_array_min(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_min_returns_smallest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,  3.0f);
    push_back_float_array(arr, -1.5f);
    push_back_float_array(arr,  2.5f);
    /* arr: [3.0, -1.5, 2.5] — minimum is -1.5 */

    float_expect_t result = float_array_min(arr);
    assert_true(result.has_value);
    ASSERT_FLOAT_EXACT(result.u.value, -1.5f);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_min_negative_infinity_wins(void** state) {
    (void)state;
    /*
     * -INFINITY is less than every finite float under IEEE 754.
     * The comparator (va > vb) - (va < vb) handles this correctly because
     * the relational operators are defined for infinities by the standard.
     */
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,       0.0f);
    push_back_float_array(arr,    1000.0f);
    push_back_float_array(arr, -INFINITY);
    push_back_float_array(arr,      -1.0f);
    /* arr: [0.0, 1000.0, -INFINITY, -1.0] */

    float_expect_t result = float_array_min(arr);
    assert_true(result.has_value);
    assert_true(isinf(result.u.value) && result.u.value < 0.0f);

    return_float_array(arr);
}

// ================================================================================
// Group 27: float_array_max
// ================================================================================

static void test_float_max_null_array_returns_null_pointer(void** state) {
    (void)state;
    float_expect_t r = float_array_max(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_max_returns_largest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr, -2.0f);
    push_back_float_array(arr,  3.5f);
    push_back_float_array(arr,  1.0f);
    /* arr: [-2.0, 3.5, 1.0] — maximum is 3.5 */

    float_expect_t result = float_array_max(arr);
    assert_true(result.has_value);
    ASSERT_FLOAT_EXACT(result.u.value, 3.5f);

    return_float_array(arr);
}

// --------------------------------------------------------------------------------

static void test_float_max_positive_infinity_wins(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* arr = r.u.value;

    push_back_float_array(arr,      -1.0f);
    push_back_float_array(arr, -INFINITY);
    push_back_float_array(arr,  INFINITY);
    push_back_float_array(arr,       0.0f);
    /* arr: [-1.0, -INFINITY, INFINITY, 0.0] */

    float_expect_t result = float_array_max(arr);
    assert_true(result.has_value);
    assert_true(isinf(result.u.value) && result.u.value > 0.0f);

    return_float_array(arr);
}

// ================================================================================
// Group 28: float_array_sum
// ================================================================================

// static void test_float_sum_null_array_returns_null_pointer(void** state) {
//     (void)state;
//     float_expect_t r = float_array_sum(NULL);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_float_sum_returns_correct_total(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_array_expect_t r = init_float_array(8, false, alloc);
//     assert_true(r.has_value);
//     float_array_t* arr = r.u.value;
//
//     push_back_float_array(arr,  1.0f);
//     push_back_float_array(arr, -2.5f);
//     push_back_float_array(arr,  3.5f);
//     /* arr: [1.0, -2.5, 3.5], sum == 2.0 (exact in binary) */
//
//     float_expect_t result = float_array_sum(arr);
//     assert_true(result.has_value);
//     ASSERT_FLOAT_EXACT(result.u.value, 2.0f);
//
//     return_float_array(arr);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_float_sum_cancelling_values(void** state) {
//     (void)state;
//     /*
//      * All values are exact binary fractions so the sum is exact (0.0f).
//      * This confirms the accumulator is genuinely a float and that
//      * negative contributions are handled correctly.
//      */
//     allocator_vtable_t alloc = heap_allocator();
//     float_array_expect_t r = init_float_array(8, false, alloc);
//     assert_true(r.has_value);
//     float_array_t* arr = r.u.value;
//
//     push_back_float_array(arr,  4.0f);
//     push_back_float_array(arr, -4.0f);
//     push_back_float_array(arr,  0.5f);
//     push_back_float_array(arr, -0.5f);
//     /* arr: [4.0, -4.0, 0.5, -0.5], sum == 0.0 */
//
//     float_expect_t result = float_array_sum(arr);
//     assert_true(result.has_value);
//     ASSERT_FLOAT_EXACT(result.u.value, 0.0f);
//
//     return_float_array(arr);
// }

// ================================================================================
// Group 29: cumulative_float_array
// ================================================================================

static void test_float_cumulative_null_array_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = cumulative_float_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_float_cumulative_produces_prefix_sum(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* src = r.u.value;

    push_back_float_array(src, 1.0f);
    push_back_float_array(src, 2.0f);
    push_back_float_array(src, 3.0f);
    push_back_float_array(src, 4.0f);
    /* src: [1.0, 2.0, 3.0, 4.0]
     * expected output: [1.0, 3.0, 6.0, 10.0] — all exact binary fractions */

    float_array_expect_t cr = cumulative_float_array(src, alloc);
    assert_true(cr.has_value);
    float_array_t* dst = cr.u.value;
    assert_int_equal((int)float_array_size(dst), 4);

    float val = 0.0f;
    get_float_array_index(dst, 0, &val);  ASSERT_FLOAT_EXACT(val,  1.0f);
    get_float_array_index(dst, 1, &val);  ASSERT_FLOAT_EXACT(val,  3.0f);
    get_float_array_index(dst, 2, &val);  ASSERT_FLOAT_EXACT(val,  6.0f);
    get_float_array_index(dst, 3, &val);  ASSERT_FLOAT_EXACT(val, 10.0f);

    return_float_array(src);
    return_float_array(dst);
}

// --------------------------------------------------------------------------------

static void test_float_cumulative_mixed_signs(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_array_expect_t r = init_float_array(8, false, alloc);
    assert_true(r.has_value);
    float_array_t* src = r.u.value;

    push_back_float_array(src,  4.0f);
    push_back_float_array(src, -2.0f);
    push_back_float_array(src,  1.0f);
    push_back_float_array(src, -0.5f);
    /* src: [4.0, -2.0, 1.0, -0.5]
     * expected output: [4.0, 2.0, 3.0, 2.5] — all exact binary fractions */

    float_array_expect_t cr = cumulative_float_array(src, alloc);
    assert_true(cr.has_value);
    float_array_t* dst = cr.u.value;
    assert_int_equal((int)float_array_size(dst), 4);

    float val = 0.0f;
    get_float_array_index(dst, 0, &val);  ASSERT_FLOAT_EXACT(val, 4.0f);
    get_float_array_index(dst, 1, &val);  ASSERT_FLOAT_EXACT(val, 2.0f);
    get_float_array_index(dst, 2, &val);  ASSERT_FLOAT_EXACT(val, 3.0f);
    get_float_array_index(dst, 3, &val);  ASSERT_FLOAT_EXACT(val, 2.5f);
    return_float_array(src);
    return_float_array(dst);
}
// -------------------------------------------------------------------------------- 

// static float_array_t* _make_float_array(size_t capacity, bool growth) {
//     allocator_vtable_t a = heap_allocator();
//     float_array_expect_t r = init_float_array(capacity, growth, a);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     return r.u.value;
// }
//
// // --------------------------------------------------------------------------------
//
// static float_array_t* _make_sample_float_array(void) {
//     float_array_t* array = _make_float_array(8u, true);
//
//     assert_int_equal(push_back_float_array(array, 1.0), NO_ERROR);
//     assert_int_equal(push_back_float_array(array, 2.0), NO_ERROR);
//     assert_int_equal(push_back_float_array(array, 3.0), NO_ERROR);
//     assert_int_equal(push_back_float_array(array, 4.0), NO_ERROR);
//     assert_int_equal(push_back_float_array(array, 5.0), NO_ERROR);
//
//     return array;
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_float_add_scalar_array_null_returns_null_pointer(void** state) {
//     (void)state;
//
//     assert_int_equal(float_add_scalar_array(NULL, 5.0), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_float_add_scalar_array_empty_returns_empty(void** state) {
//     (void)state;
//
//     float_array_t* array = _make_float_array(4u, true);
//
//     assert_int_equal(float_add_scalar_array(array, 5.0), EMPTY);
//
//     return_float_array(array);
// }
// // -------------------------------------------------------------------------------- 
//
// static void test_float_add_scalar_array_add_zero_leaves_values_unchanged(void** state) {
//     (void)state;
//
//     float_array_t* array = _make_sample_float_array();
//
//     assert_int_equal(float_add_scalar_array(array, 0.0), NO_ERROR);
//
//     float out = 0.0;
//
//     assert_int_equal(get_float_array_index(array, 0u, &out), NO_ERROR);
//     assert_float_equal(out, 1.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 1u, &out), NO_ERROR);
//     assert_float_equal(out, 2.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 2u, &out), NO_ERROR);
//     assert_float_equal(out, 3.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 3u, &out), NO_ERROR);
//     assert_float_equal(out, 4.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 4u, &out), NO_ERROR);
//     assert_float_equal(out, 5.0, 1.0e-3);
//
//     return_float_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_float_add_scalar_array_single_element_updates_value(void** state) {
//     (void)state;
//
//     float_array_t* array = _make_float_array(4u, true);
//     assert_int_equal(push_back_float_array(array, 10.0), NO_ERROR);
//     print_float_array(array, stdout);
//     float out = 0.0;
//     get_float_array_index(array, 0, &out);
//     printf("%f\n", out);
//     assert_int_equal(float_add_scalar_array(array, 7.0), NO_ERROR);
//     get_float_array_index(array, 0, &out);
//     printf("%f\n", out);
//     // assert_int_equal(get_float_array_index(array, 0u, &out), NO_ERROR);
//     // assert_float_equal(out, 17.0, 1.0e-3);
//
//     return_float_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_float_add_scalar_array_updates_all_elements(void** state) {
//     (void)state;
//
//     float_array_t* array = _make_sample_float_array();
//
//     assert_int_equal(float_add_scalar_array(array, 10.0), NO_ERROR);
//
//     float out = 0.0;
//
//     assert_int_equal(get_float_array_index(array, 0u, &out), NO_ERROR);
//     assert_float_equal(out, 11.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 1u, &out), NO_ERROR);
//     assert_float_equal(out, 12.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 2u, &out), NO_ERROR);
//     assert_float_equal(out, 13.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 3u, &out), NO_ERROR);
//     assert_float_equal(out, 14.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 4u, &out), NO_ERROR);
//     assert_float_equal(out, 15.0, 1.0e-3);
//
//     return_float_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_float_add_scalar_array_preserves_length(void** state) {
//     (void)state;
//
//     float_array_t* array = _make_sample_float_array();
//     size_t before = float_array_size(array);
//
//     assert_int_equal(float_add_scalar_array(array, 3.0), NO_ERROR);
//
//     assert_int_equal(float_array_size(array), before);
//
//     return_float_array(array);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_float_add_scalar_array_multiple_calls_accumulate(void** state) {
//     (void)state;
//
//     float_array_t* array = _make_sample_float_array();
//
//     assert_int_equal(float_add_scalar_array(array, 2.0), NO_ERROR);
//     assert_int_equal(float_add_scalar_array(array, 3.0), NO_ERROR);
//
//     float out = 0.0;
//
//     assert_int_equal(get_float_array_index(array, 0u, &out), NO_ERROR);
//     assert_float_equal(out, 6.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 1u, &out), NO_ERROR);
//     assert_float_equal(out, 7.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 2u, &out), NO_ERROR);
//     assert_float_equal(out, 8.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 3u, &out), NO_ERROR);
//     assert_float_equal(out, 9.0, 1.0e-3);
//
//     assert_int_equal(get_float_array_index(array, 4u, &out), NO_ERROR);
//     assert_float_equal(out, 10.0, 1.0e-3);
//
//     return_float_array(array);
// }
// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_float_array[] = {

    /* Group 1: init_float_array */
    cmocka_unit_test(test_float_init_null_allocate_fn_fails),
    cmocka_unit_test(test_float_init_returns_valid_array),

    /* Group 2: return_float_array */
    cmocka_unit_test(test_float_return_null_is_safe),
    cmocka_unit_test(test_float_return_valid_array_does_not_crash),

    /* Group 3: push_back_float_array */
    cmocka_unit_test(test_float_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_push_back_appends_value),

    /* Group 4: push_front_float_array */
    cmocka_unit_test(test_float_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_push_front_prepends_value),

    /* Group 5: push_at_float_array */
    cmocka_unit_test(test_float_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_push_at_inserts_at_index),

    /* Group 6: get_float_array_index */
    cmocka_unit_test(test_float_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_get_returns_correct_value),

    /* Group 7: pop_back_float_array */
    cmocka_unit_test(test_float_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_pop_back_removes_last_element),

    /* Group 8: pop_front_float_array */
    cmocka_unit_test(test_float_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_pop_front_removes_first_element),

    /* Group 9: pop_any_float_array */
    cmocka_unit_test(test_float_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_pop_any_removes_element_at_index),

    /* Group 10: clear_float_array */
    cmocka_unit_test(test_float_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_clear_resets_len_to_zero),

    /* Group 11: set_float_array_index */
    cmocka_unit_test(test_float_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_set_overwrites_element),

    /* Group 12: copy_float_array */
    cmocka_unit_test(test_float_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_float_copy_produces_independent_array),

    /* Group 13: concat_float_array */
    cmocka_unit_test(test_float_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_float_concat_appends_elements),

    /* Group 14: slice_float_array */
    cmocka_unit_test(test_float_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_float_slice_returns_correct_subrange),

    /* Group 15: reverse_float_array */
    cmocka_unit_test(test_float_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_reverse_reverses_elements),

    /* Group 16: sort_float_array */
    cmocka_unit_test(test_float_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_sort_forward_basic),
    cmocka_unit_test(test_float_sort_negative_values_and_infinity),

    /* Group 17: float_array_contains */
    cmocka_unit_test(test_float_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_contains_negative_tolerance_returns_invalid_arg),
    cmocka_unit_test(test_float_contains_exact_mode_finds_value),
    cmocka_unit_test(test_float_contains_absolute_tolerance),
    cmocka_unit_test(test_float_contains_relative_tolerance),
    cmocka_unit_test(test_float_contains_combined_tolerance),

    /* Group 18: float_array_binary_search */
    cmocka_unit_test(test_float_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_binary_bracket_exact_match),
    cmocka_unit_test(test_float_binary_bracket_gap_between_values),
   
    /* Group 19: float_array_binary_bracket */
    cmocka_unit_test(test_float_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_binary_bracket_exact_match),
    cmocka_unit_test(test_float_binary_bracket_gap_between_values),

    /* Group 20: float_array_size */
    cmocka_unit_test(test_float_size_null_returns_zero),
    cmocka_unit_test(test_float_size_reflects_push_count),

    /* Group 21: float_array_alloc */
    cmocka_unit_test(test_float_alloc_null_returns_zero),
    cmocka_unit_test(test_float_alloc_matches_capacity),

    /* Group 22: float_array_data_size */
    cmocka_unit_test(test_float_data_size_null_returns_zero),
    cmocka_unit_test(test_float_data_size_is_four),

    /* Group 23: is_float_array_empty */
    cmocka_unit_test(test_float_empty_null_returns_true),
    cmocka_unit_test(test_float_empty_reflects_contents),

    /* Group 24: is_float_array_full */
    cmocka_unit_test(test_float_full_null_returns_true),
    cmocka_unit_test(test_float_full_reflects_capacity),

    /* Group 25: is_float_array_ptr */
    cmocka_unit_test(test_float_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_float_is_ptr_valid_and_invalid),

    /* Group 26: float_array_min */
    cmocka_unit_test(test_float_min_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_min_returns_smallest_value),
    cmocka_unit_test(test_float_min_negative_infinity_wins),

    /* Group 27: float_array_max */
    cmocka_unit_test(test_float_max_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_max_returns_largest_value),
    cmocka_unit_test(test_float_max_positive_infinity_wins),

    /* Group 28: float_array_sum */
    // cmocka_unit_test(test_float_sum_null_array_returns_null_pointer),
    // cmocka_unit_test(test_float_sum_returns_correct_total),
    // cmocka_unit_test(test_float_sum_cancelling_values),

    /* Group 29: cumulative_float_array */
    cmocka_unit_test(test_float_cumulative_null_array_returns_null_pointer),
    cmocka_unit_test(test_float_cumulative_produces_prefix_sum),
    cmocka_unit_test(test_float_cumulative_mixed_signs),

    // cmocka_unit_test(test_float_add_scalar_array_null_returns_null_pointer),
    // cmocka_unit_test(test_float_add_scalar_array_empty_returns_empty),
    // cmocka_unit_test(test_float_add_scalar_array_add_zero_leaves_values_unchanged),
    // cmocka_unit_test(test_float_add_scalar_array_single_element_updates_value),
    // cmocka_unit_test(test_float_add_scalar_array_updates_all_elements),
    // cmocka_unit_test(test_float_add_scalar_array_preserves_length),
    // cmocka_unit_test(test_float_add_scalar_array_multiple_calls_accumulate),
};
const size_t test_float_array_count = sizeof(test_float_array) / sizeof(test_float_array[0]);
// ================================================================================ 
// ================================================================================ 

/* Tolerance-free equality for exact binary fractions used in tests. */
#define ASSERT_DOUBLE_EXACT(a, b) assert_true((a) == (b))

// ================================================================================
// Group 1: init_double_array
// ================================================================================

static void test_double_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    double_array_expect_t r = init_double_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)double_array_size(r.u.value), 0);
    assert_int_equal((int)double_array_alloc(r.u.value), 8);
    return_double_array(r.u.value);
}

// ================================================================================
// Group 2: return_double_array
// ================================================================================

static void test_double_return_null_is_safe(void** state) {
    (void)state;
    return_double_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_double_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    return_double_array(r.u.value);
}

// ================================================================================
// Group 3: push_back_double_array
// ================================================================================

static void test_double_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_double_array(NULL, 1.0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    assert_int_equal(push_back_double_array(arr, -1.0), NO_ERROR);
    assert_int_equal(push_back_double_array(arr,  0.0), NO_ERROR);
    assert_int_equal(push_back_double_array(arr,  1.0), NO_ERROR);
    assert_int_equal((int)double_array_size(arr), 3);

    double val = 0.0;
    get_double_array_index(arr, 0, &val);  ASSERT_DOUBLE_EXACT(val, -1.0);
    get_double_array_index(arr, 2, &val);  ASSERT_DOUBLE_EXACT(val,  1.0);

    return_double_array(arr);
}

// ================================================================================
// Group 4: push_front_double_array
// ================================================================================

static void test_double_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_double_array(NULL, 1.0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,  2.0);
    push_front_double_array(arr, 1.0);
    /* arr is [1.0, 2.0] */

    double val = 0.0;
    get_double_array_index(arr, 0, &val);
    ASSERT_DOUBLE_EXACT(val, 1.0);

    return_double_array(arr);
}

// ================================================================================
// Group 5: push_at_double_array
// ================================================================================

static void test_double_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_double_array(NULL, 0, 1.0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 3.0);
    push_at_double_array(arr, 1, 2.0);
    /* arr is [1.0, 2.0, 3.0] */

    double val = 0.0;
    get_double_array_index(arr, 1, &val);
    ASSERT_DOUBLE_EXACT(val, 2.0);

    return_double_array(arr);
}

// ================================================================================
// Group 6: get_double_array_index
// ================================================================================

static void test_double_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    double val = 0.0;
    assert_int_equal(get_double_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, -1.5);
    push_back_double_array(arr,  0.0);
    push_back_double_array(arr,  1.5);

    double val = 0.0;
    assert_int_equal(get_double_array_index(arr, 0, &val), NO_ERROR);
    ASSERT_DOUBLE_EXACT(val, -1.5);
    assert_int_equal(get_double_array_index(arr, 2, &val), NO_ERROR);
    ASSERT_DOUBLE_EXACT(val,  1.5);

    return_double_array(arr);
}

// ================================================================================
// Group 7: pop_back_double_array
// ================================================================================

static void test_double_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_double_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);

    double val = 0.0;
    assert_int_equal(pop_back_double_array(arr, &val), NO_ERROR);
    ASSERT_DOUBLE_EXACT(val, 2.0);
    assert_int_equal((int)double_array_size(arr), 1);

    return_double_array(arr);
}

// ================================================================================
// Group 8: pop_front_double_array
// ================================================================================

static void test_double_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_double_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);

    double val = 0.0;
    assert_int_equal(pop_front_double_array(arr, &val), NO_ERROR);
    ASSERT_DOUBLE_EXACT(val, 1.0);
    assert_int_equal((int)double_array_size(arr), 1);

    return_double_array(arr);
}

// ================================================================================
// Group 9: pop_any_double_array
// ================================================================================

static void test_double_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_double_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);
    push_back_double_array(arr, 3.0);

    double val = 0.0;
    assert_int_equal(pop_any_double_array(arr, &val, 1), NO_ERROR);
    ASSERT_DOUBLE_EXACT(val, 2.0);
    assert_int_equal((int)double_array_size(arr), 2);

    return_double_array(arr);
}

// ================================================================================
// Group 10: clear_double_array
// ================================================================================

static void test_double_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_double_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);
    assert_int_equal(clear_double_array(arr), NO_ERROR);
    assert_int_equal((int)double_array_size(arr), 0);
    assert_int_equal((int)double_array_alloc(arr), 4);   /* capacity retained */

    return_double_array(arr);
}

// ================================================================================
// Group 11: set_double_array_index
// ================================================================================

static void test_double_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_double_array_index(NULL, 0, 1.0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 1.0);
    assert_int_equal(set_double_array_index(arr, 0, -99.5), NO_ERROR);

    double val = 0.0;
    get_double_array_index(arr, 0, &val);
    ASSERT_DOUBLE_EXACT(val, -99.5);

    return_double_array(arr);
}

// ================================================================================
// Group 12: copy_double_array
// ================================================================================

static void test_double_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = copy_double_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* src = r.u.value;

    push_back_double_array(src, 1.0);
    push_back_double_array(src, 2.0);

    double_array_expect_t cr = copy_double_array(src, alloc);
    assert_true(cr.has_value);
    double_array_t* dst = cr.u.value;

    /* Mutating src must not affect the copy */
    set_double_array_index(src, 0, 999.0);
    double val = 0.0;
    get_double_array_index(dst, 0, &val);
    ASSERT_DOUBLE_EXACT(val, 1.0);

    return_double_array(src);
    return_double_array(dst);
}

// ================================================================================
// Group 13: concat_double_array
// ================================================================================

static void test_double_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* src = r.u.value;
    assert_int_equal(concat_double_array(NULL, src), NULL_POINTER);
    return_double_array(src);
}

// --------------------------------------------------------------------------------

static void test_double_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r1 = init_double_array(8, false, alloc);
    double_array_expect_t r2 = init_double_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    double_array_t* dst = r1.u.value;
    double_array_t* src = r2.u.value;

    push_back_double_array(dst, 1.0);
    push_back_double_array(src, 2.0);

    assert_int_equal(concat_double_array(dst, src), NO_ERROR);
    assert_int_equal((int)double_array_size(dst), 2);

    double val = 0.0;
    get_double_array_index(dst, 1, &val);
    ASSERT_DOUBLE_EXACT(val, 2.0);

    return_double_array(dst);
    return_double_array(src);
}

// ================================================================================
// Group 14: slice_double_array
// ================================================================================

static void test_double_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = slice_double_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* src = r.u.value;

    push_back_double_array(src, 1.0);
    push_back_double_array(src, 2.0);
    push_back_double_array(src, 3.0);
    push_back_double_array(src, 4.0);
    push_back_double_array(src, 5.0);

    double_array_expect_t sr = slice_double_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    double_array_t* slc = sr.u.value;
    assert_int_equal((int)double_array_size(slc), 3);

    double val = 0.0;
    get_double_array_index(slc, 0, &val);  ASSERT_DOUBLE_EXACT(val, 2.0);
    get_double_array_index(slc, 1, &val);  ASSERT_DOUBLE_EXACT(val, 3.0);
    get_double_array_index(slc, 2, &val);  ASSERT_DOUBLE_EXACT(val, 4.0);

    return_double_array(src);
    return_double_array(slc);
}

// ================================================================================
// Group 15: reverse_double_array
// ================================================================================

static void test_double_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_double_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);
    push_back_double_array(arr, 3.0);

    assert_int_equal(reverse_double_array(arr), NO_ERROR);

    double val = 0.0;
    get_double_array_index(arr, 0, &val);  ASSERT_DOUBLE_EXACT(val, 3.0);
    get_double_array_index(arr, 2, &val);  ASSERT_DOUBLE_EXACT(val, 1.0);

    return_double_array(arr);
}

// ================================================================================
// Group 16: sort_double_array
// ================================================================================

static void test_double_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_double_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_sort_forward_basic(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,  3.0);
    push_back_double_array(arr, -1.5);
    push_back_double_array(arr,  2.5);

    assert_int_equal(sort_double_array(arr, FORWARD), NO_ERROR);

    double val = 0.0;
    get_double_array_index(arr, 0, &val);  ASSERT_DOUBLE_EXACT(val, -1.5);
    get_double_array_index(arr, 1, &val);  ASSERT_DOUBLE_EXACT(val,  2.5);
    get_double_array_index(arr, 2, &val);  ASSERT_DOUBLE_EXACT(val,  3.0);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_sort_negative_values_and_infinity(void** state) {
    (void)state;
    /*
     * IEEE 754 total order for finite doubles and infinities:
     *   -INFINITY < negative finite values < 0.0 < positive finite values < INFINITY
     *
     * The three-way comparator (va > vb) - (va < vb) handles infinities
     * correctly because the relational operators are defined for infinities
     * by IEEE 754.
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,  1.0);
    push_back_double_array(arr, -INFINITY);
    push_back_double_array(arr, -1.0);
    push_back_double_array(arr,  INFINITY);
    push_back_double_array(arr,  0.0);

    assert_int_equal(sort_double_array(arr, FORWARD), NO_ERROR);

    double val = 0.0;
    get_double_array_index(arr, 0, &val);  assert_true(isinf(val) && val < 0.0);
    get_double_array_index(arr, 1, &val);  ASSERT_DOUBLE_EXACT(val, -1.0);
    get_double_array_index(arr, 2, &val);  ASSERT_DOUBLE_EXACT(val,  0.0);
    get_double_array_index(arr, 3, &val);  ASSERT_DOUBLE_EXACT(val,  1.0);
    get_double_array_index(arr, 4, &val);  assert_true(isinf(val) && val > 0.0);

    return_double_array(arr);
}

// ================================================================================
// Group 17: double_array_contains
// ================================================================================

static void test_double_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = double_array_contains(NULL, 0.0, 0, 1, 0.0, 0.0);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_contains_negative_tolerance_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;
    push_back_double_array(arr, 1.0);

    size_expect_t sr = double_array_contains(arr, 1.0, 0, 1, -0.1, 0.0);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    sr = double_array_contains(arr, 1.0, 0, 1, 0.0, -0.1);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_contains_exact_mode_finds_value(void** state) {
    (void)state;
    /*
     * Exact mode (both tolerances 0.0): bitwise equality.
     * -0.0 and +0.0 have different bit patterns so they do NOT match.
     * NaN != NaN so a stored NaN is never found.
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, -1.0);
    push_back_double_array(arr,  2.5);
    push_back_double_array(arr, -3.0);
    push_back_double_array(arr,  0.0);  /* +0.0 */

    /* Value present — found at correct index. */
    size_expect_t sr = double_array_contains(arr, 2.5, 0, 4, 0.0, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    /* Searching for -0.0 must NOT find +0.0 at index 3. */
    sr = double_array_contains(arr, -0.0, 0, 4, 0.0, 0.0);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    /* Searching for +0.0 must find it at index 3. */
    sr = double_array_contains(arr, 0.0, 0, 4, 0.0, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 3);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_contains_absolute_tolerance(void** state) {
    (void)state;
    /*
     * Absolute mode (rel_tol == 0.0): match when |element - needle| <= abs_tol.
     * arr: [0.0, 1.0, 2.0, 3.0]
     * needle=1.05, abs_tol=0.1: |1.0 - 1.05| = 0.05 <= 0.1 → match at index 1.
     * needle=1.5,  abs_tol=0.1: nearest elements differ by 0.5 > 0.1 → NOT_FOUND.
     * -0.0 and +0.0 match under tolerance because |(-0.0) - 0.0| = 0.0 <= tol.
     *
     * Double-specific: use a tighter abs_tol (1e-9) near zero to confirm
     * the extra precision is handled correctly.
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 0.0);
    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);
    push_back_double_array(arr, 3.0);

    size_expect_t sr = double_array_contains(arr, 1.05, 0, 4, 0.1, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = double_array_contains(arr, 1.5, 0, 4, 0.1, 0.0);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    /* -0.0 vs +0.0 with double-precision floor tolerance. */
    sr = double_array_contains(arr, -0.0, 0, 1, 1e-9, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_contains_relative_tolerance(void** state) {
    (void)state;
    /*
     * Relative mode (abs_tol == 0.0): match when
     *   |element - needle| <= rel_tol * max(|element|, |needle|).
     *
     * arr: [100.0, 1000.0, 10000.0]
     * rel_tol 0.01 (1%):
     *   needle 1005.0 vs 1000.0: diff=5, threshold=0.01*1005=10.05 — match at 1
     *   needle  110.0 vs  100.0: diff=10, threshold=0.01*110=1.1   — no match
     *
     * Double-specific: also verify a tight 1e-10 relative tolerance that would
     * fail for float but succeeds for double.
     * arr: [1.0000000001] (needs double precision to represent)
     * needle=1.0, rel_tol=1e-9: diff=1e-10 <= 1e-9*1 = 1e-9 — match
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,   100.0);
    push_back_double_array(arr,  1000.0);
    push_back_double_array(arr, 10000.0);

    size_expect_t sr = double_array_contains(arr, 1005.0, 0, 3, 0.0, 0.01);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = double_array_contains(arr, 110.0, 0, 3, 0.0, 0.01);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    /* Double-precision relative tolerance test. */
    return_double_array(arr);

    double_array_expect_t r2 = init_double_array(4, false, alloc);
    assert_true(r2.has_value);
    double_array_t* arr2 = r2.u.value;
    push_back_double_array(arr2, 1.0000000001);   /* distinct from 1.0 in double */

    sr = double_array_contains(arr2, 1.0, 0, 1, 0.0, 1e-9);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_double_array(arr2);
}

// --------------------------------------------------------------------------------

static void test_double_contains_combined_tolerance(void** state) {
    (void)state;
    /*
     * Combined mode: threshold = max(abs_tol, rel_tol * max(|e|, |n|)).
     * abs_tol acts as a floor so near zero the threshold doesn't collapse.
     *
     * arr: [0.0, 1000.0]
     * abs_tol=1e-9, rel_tol=0.01:
     *   needle=0.0:   threshold=max(1e-9, 0.0)=1e-9 → 0.0 matches at index 0
     *   needle=1e-12: diff=1e-12 <= 1e-9 (abs_tol floor) → match at index 0
     *
     * Confirm pure relative alone misses: needle=1e-12, abs_tol=0, rel_tol=0.01
     *   threshold=0.01*max(0, 1e-12)=1e-14 < diff=1e-12 → NOT_FOUND
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,    0.0);
    push_back_double_array(arr, 1000.0);

    size_expect_t sr = double_array_contains(arr, 0.0, 0, 2, 1e-9, 0.01);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    /* abs_tol floor rescues sub-1e-9 needle near zero */
    sr = double_array_contains(arr, 1e-12, 0, 1, 1e-9, 0.01);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    /* Pure relative alone collapses near zero — no match */
    sr = double_array_contains(arr, 1e-12, 0, 1, 0.0, 0.01);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_double_array(arr);
}

// ================================================================================
// Group 18: double_array_binary_search
// ================================================================================

static void test_double_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = double_array_binary_search(NULL, 0.0, false, 0.0, 0.0);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_binary_search_negative_tolerance_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;
    push_back_double_array(arr, 1.0);

    size_expect_t sr = double_array_binary_search(arr, 1.0, false, -0.1, 0.0);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    sr = double_array_binary_search(arr, 1.0, false, 0.0, -0.1);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_binary_search_exact_finds_value_with_sort(void** state) {
    (void)state;
    /*
     * Exact mode (both tolerances 0.0): confirms the sort path and that the
     * first occurrence of a duplicate is returned.
     * arr (unsorted): [3.0, -1.5, 2.5, 2.5, 4.0]
     * After sort:     [-1.5, 2.5, 2.5, 3.0, 4.0]
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,  3.0);
    push_back_double_array(arr, -1.5);
    push_back_double_array(arr,  2.5);
    push_back_double_array(arr,  2.5);
    push_back_double_array(arr,  4.0);

    size_expect_t sr = double_array_binary_search(arr, 2.5, true, 0.0, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);   /* first of the two 2.5 entries */

    sr = double_array_binary_search(arr, 99.0, false, 0.0, 0.0);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_binary_search_exact_negative_and_infinity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    /* Already sorted: [-INFINITY, -1.0, 0.0, 1.0, INFINITY] */
    push_back_double_array(arr, -INFINITY);
    push_back_double_array(arr,    -1.0);
    push_back_double_array(arr,     0.0);
    push_back_double_array(arr,     1.0);
    push_back_double_array(arr,  INFINITY);

    size_expect_t sr = double_array_binary_search(arr, -INFINITY, false, 0.0, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    sr = double_array_binary_search(arr, INFINITY, false, 0.0, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 4);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_binary_search_absolute_tolerance(void** state) {
    (void)state;
    /*
     * Absolute tolerance: match when |element - needle| <= abs_tol.
     *
     * arr (sorted): [0.0, 1.0, 2.0, 3.0, 4.0]
     *
     * needle=1.3, abs_tol=0.5: element 1.0 (diff=0.3 <= 0.5) — match at 1.
     * needle=1.5, abs_tol=0.5: elements 1.0 (diff=0.5) and 2.0 (diff=0.5)
     *   both qualify; lowest index is 1.
     * needle=5.0, abs_tol=0.1: nothing in range → NOT_FOUND.
     *
     * Double-specific: use a sub-float-precision abs_tol (1e-10) to confirm
     * the extra precision is honoured.
     * arr2 (sorted): [1.0, 1.0 + 5e-11]
     * needle=1.0, abs_tol=1e-10: diff(1.0)=0 → match; also diff(1.0+5e-11)=5e-11 <= 1e-10
     * → lowest match is index 0.
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 0.0);
    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);
    push_back_double_array(arr, 3.0);
    push_back_double_array(arr, 4.0);

    size_expect_t sr = double_array_binary_search(arr, 1.3, false, 0.5, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);
    
    sr = double_array_binary_search(arr, 1.5, false, 0.5, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);
    
    sr = double_array_binary_search(arr, 5.0, false, 0.1, 0.0);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    /* Double-precision sub-float abs_tol. */
    return_double_array(arr);

    double_array_expect_t r2 = init_double_array(4, false, alloc);
    assert_true(r2.has_value);
    double_array_t* arr2 = r2.u.value;
    push_back_double_array(arr2, 1.0);
    push_back_double_array(arr2, 1.0 + 5e-11);   /* indistinguishable as float */

    sr = double_array_binary_search(arr2, 1.0, false, 1e-10, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_double_array(arr2);
}

// --------------------------------------------------------------------------------

static void test_double_binary_search_relative_tolerance(void** state) {
    (void)state;
    /*
     * Relative tolerance: match when |element - needle| <=
     *   rel_tol * max(|element|, |needle|).
     *
     * arr (sorted): [100.0, 1000.0, 10000.0]
     * rel_tol=0.01:
     *   needle=1005.0: threshold=0.01*1005=10.05; 1000.0 diff=5 → match at 1.
     *   needle=115.0:  threshold=0.01*115=1.15;   100.0  diff=15 → NOT_FOUND.
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,   100.0);
    push_back_double_array(arr,  1000.0);
    push_back_double_array(arr, 10000.0);

    size_expect_t sr = double_array_binary_search(arr, 1005.0, false, 0.0, 0.01);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = double_array_binary_search(arr, 115.0, false, 0.0, 0.01);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_binary_search_combined_tolerance(void** state) {
    (void)state;
    /*
     * Combined tolerance: threshold = max(abs_tol,
     *                                     rel_tol * max(|element|, |needle|)).
     * abs_tol acts as a floor so the threshold doesn't collapse near zero.
     *
     * arr (sorted): [0.0, 0.001, 1000.0]
     * abs_tol=1e-3, rel_tol=0.01:
     *   needle=0.0: threshold=max(1e-3, 0)=1e-3
     *     0.0   diff=0    <= 1e-3 → match
     *     0.001 diff=1e-3 <= 1e-3 → also within; lowest is index 0.
     *
     *   needle=1e-6: threshold >= abs_tol=1e-3 (abs_tol floor saves near zero)
     *     0.0   diff=1e-6 <= 1e-3 → match at index 0.
     *
     * Double-specific: use 1e-12 abs_tol floor with 1e-9 needle — exercises
     * sub-float-precision floor behaviour unique to double.
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,    0.0);
    push_back_double_array(arr,    0.001);
    push_back_double_array(arr, 1000.0);

    size_expect_t sr = double_array_binary_search(arr, 0.0, false, 1e-3, 0.01);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    sr = double_array_binary_search(arr, 1e-6, false, 1e-3, 0.01);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    /* Double-precision sub-float abs_tol floor. */
    return_double_array(arr);

    double_array_expect_t r2 = init_double_array(4, false, alloc);
    assert_true(r2.has_value);
    double_array_t* arr2 = r2.u.value;
    push_back_double_array(arr2, 0.0);

    sr = double_array_binary_search(arr2, 1e-9, false, 1e-8, 0.0);
    assert_true(sr.has_value);   /* diff=1e-9 <= abs_tol=1e-8 */
    assert_int_equal((int)sr.u.value, 0);

    return_double_array(arr2);
}

// --------------------------------------------------------------------------------

static void test_double_binary_search_fanout_lands_between_values(void** state) {
    (void)state;
    /*
     * The bisection may land at a position where no exact match exists but a
     * tolerant match is nearby.  The fan-out must find the lower-index match.
     *
     * arr (sorted): [1.0, 2.0, 3.0, 4.0]
     * needle=2.4, abs_tol=0.5:
     *   bisection lands at index 2 (3.0), diff=0.6 > 0.5 — no match.
     *   fan-out left: index 1 (2.0), diff=0.4 <= 0.5 → best=1.
     *                 index 0 (1.0), diff=1.4 >  0.5 → stop.
     *   lowest match is index 1.
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);
    push_back_double_array(arr, 3.0);
    push_back_double_array(arr, 4.0);

    size_expect_t sr = double_array_binary_search(arr, 2.4, false, 0.5, 0.0);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    /* needle far outside range → NOT_FOUND even with generous tolerance */
    sr = double_array_binary_search(arr, 100.0, false, 0.5, 0.0);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_double_array(arr);
}

// ================================================================================
// Group 19: double_array_binary_bracket
// ================================================================================

static void test_double_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = double_array_binary_bracket(NULL, 0.0, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, -2.0);
    push_back_double_array(arr, -1.0);
    push_back_double_array(arr,  1.0);
    push_back_double_array(arr,  2.0);

    bracket_expect_t br = double_array_binary_bracket(arr, -1.0, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_binary_bracket_gap_between_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    /* [-2.0, -1.0, 1.0, 2.0] — gap between -1.0 and 1.0 */
    push_back_double_array(arr, -2.0);
    push_back_double_array(arr, -1.0);
    push_back_double_array(arr,  1.0);
    push_back_double_array(arr,  2.0);

    /* 0.0 falls in the gap */
    bracket_expect_t br = double_array_binary_bracket(arr, 0.0, false);
    assert_true(br.has_value);
    /* lower = first element >= 0.0 = index 2 (value  1.0) */
    /* upper = last  element <= 0.0 = index 1 (value -1.0) */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_double_array(arr);
}

// ================================================================================
// Group 20: double_array_size
// ================================================================================

static void test_double_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)double_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_double_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    assert_int_equal((int)double_array_size(arr), 0);
    push_back_double_array(arr, -1.0);
    push_back_double_array(arr,  2.0);
    assert_int_equal((int)double_array_size(arr), 2);

    return_double_array(arr);
}

// ================================================================================
// Group 21: double_array_alloc
// ================================================================================

static void test_double_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)double_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_double_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)double_array_alloc(r.u.value), 16);
    return_double_array(r.u.value);
}

// ================================================================================
// Group 22: double_array_data_size
// ================================================================================

static void test_double_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)double_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_double_data_size_is_eight(void** state) {
    (void)state;
    /*
     * sizeof(double) == 8 on all IEEE 754 platforms. This differs from
     * float_array_data_size which always returns 4.
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)double_array_data_size(r.u.value), 8);
    return_double_array(r.u.value);
}

// ================================================================================
// Group 23: is_double_array_empty
// ================================================================================

static void test_double_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_double_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_double_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(4, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    assert_true(is_double_array_empty(arr));
    push_back_double_array(arr, 1.0);
    assert_false(is_double_array_empty(arr));

    return_double_array(arr);
}

// ================================================================================
// Group 24: is_double_array_full
// ================================================================================

static void test_double_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_double_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_double_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(2, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    assert_false(is_double_array_full(arr));
    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);
    assert_true(is_double_array_full(arr));

    return_double_array(arr);
}

// ================================================================================
// Group 25: is_double_array_ptr
// ================================================================================

static void test_double_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    double val = 0.0;
    assert_false(is_double_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_double_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, 1.0);
    push_back_double_array(arr, 2.0);
    push_back_double_array(arr, 3.0);

    const double* first = (const double*)arr->base.data;
    const double* last  = first + 2;
    const double* spare = first + 3;   /* beyond live region */

    assert_true (is_double_array_ptr(arr, first));
    assert_true (is_double_array_ptr(arr, last));
    assert_false(is_double_array_ptr(arr, spare));
    assert_false(is_double_array_ptr(arr, NULL));

    return_double_array(arr);
}

// ================================================================================
// Group 26: double_array_min
// ================================================================================

static void test_double_min_null_array_returns_null_pointer(void** state) {
    (void)state;
    double_expect_t r = double_array_min(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_min_returns_smallest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,  3.0);
    push_back_double_array(arr, -1.5);
    push_back_double_array(arr,  2.5);
    /* arr: [3.0, -1.5, 2.5] — minimum is -1.5 */

    double_expect_t result = double_array_min(arr);
    assert_true(result.has_value);
    ASSERT_DOUBLE_EXACT(result.u.value, -1.5);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_min_negative_infinity_wins(void** state) {
    (void)state;
    /*
     * -INFINITY is less than every finite double under IEEE 754.
     */
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,       0.0);
    push_back_double_array(arr,    1000.0);
    push_back_double_array(arr, -INFINITY);
    push_back_double_array(arr,      -1.0);

    double_expect_t result = double_array_min(arr);
    assert_true(result.has_value);
    assert_true(isinf(result.u.value) && result.u.value < 0.0);

    return_double_array(arr);
}

// ================================================================================
// Group 27: double_array_max
// ================================================================================

static void test_double_max_null_array_returns_null_pointer(void** state) {
    (void)state;
    double_expect_t r = double_array_max(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_max_returns_largest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr, -2.0);
    push_back_double_array(arr,  3.5);
    push_back_double_array(arr,  1.0);
    /* arr: [-2.0, 3.5, 1.0] — maximum is 3.5 */

    double_expect_t result = double_array_max(arr);
    assert_true(result.has_value);
    ASSERT_DOUBLE_EXACT(result.u.value, 3.5);

    return_double_array(arr);
}

// --------------------------------------------------------------------------------

static void test_double_max_positive_infinity_wins(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* arr = r.u.value;

    push_back_double_array(arr,      -1.0);
    push_back_double_array(arr, -INFINITY);
    push_back_double_array(arr,  INFINITY);
    push_back_double_array(arr,       0.0);

    double_expect_t result = double_array_max(arr);
    assert_true(result.has_value);
    assert_true(isinf(result.u.value) && result.u.value > 0.0);

    return_double_array(arr);
}

// ================================================================================
// Group 28: double_array_sum
// ================================================================================

// static void test_double_sum_null_array_returns_null_pointer(void** state) {
//     (void)state;
//     double_expect_t r = double_array_sum(NULL);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_double_sum_returns_correct_total(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_array_expect_t r = init_double_array(8, false, alloc);
//     assert_true(r.has_value);
//     double_array_t* arr = r.u.value;
//
//     push_back_double_array(arr,  1.0);
//     push_back_double_array(arr, -2.5);
//     push_back_double_array(arr,  3.5);
//     /* arr: [1.0, -2.5, 3.5], sum == 2.0 (exact in binary) */
//
//     double_expect_t result = double_array_sum(arr);
//     assert_true(result.has_value);
//     ASSERT_DOUBLE_EXACT(result.u.value, 2.0);
//
//     return_double_array(arr);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_double_sum_cancelling_values(void** state) {
//     (void)state;
//     /*
//      * All values are exact binary fractions so the sum is exactly 0.0.
//      */
//     allocator_vtable_t alloc = heap_allocator();
//     double_array_expect_t r = init_double_array(8, false, alloc);
//     assert_true(r.has_value);
//     double_array_t* arr = r.u.value;
//
//     push_back_double_array(arr,  4.0);
//     push_back_double_array(arr, -4.0);
//     push_back_double_array(arr,  0.5);
//     push_back_double_array(arr, -0.5);
//     /* arr: [4.0, -4.0, 0.5, -0.5], sum == 0.0 */
//
//     double_expect_t result = double_array_sum(arr);
//     assert_true(result.has_value);
//     ASSERT_DOUBLE_EXACT(result.u.value, 0.0);
//
//     return_double_array(arr);
// }

// ================================================================================
// Group 29: cumulative_double_array
// ================================================================================

static void test_double_cumulative_null_array_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = cumulative_double_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_double_cumulative_produces_prefix_sum(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* src = r.u.value;

    push_back_double_array(src, 1.0);
    push_back_double_array(src, 2.0);
    push_back_double_array(src, 3.0);
    push_back_double_array(src, 4.0);
    /* src: [1.0, 2.0, 3.0, 4.0]
     * expected: [1.0, 3.0, 6.0, 10.0] — all exact binary fractions */

    double_array_expect_t cr = cumulative_double_array(src, alloc);
    assert_true(cr.has_value);
    double_array_t* dst = cr.u.value;
    assert_int_equal((int)double_array_size(dst), 4);

    double val = 0.0;
    get_double_array_index(dst, 0, &val);  ASSERT_DOUBLE_EXACT(val,  1.0);
    get_double_array_index(dst, 1, &val);  ASSERT_DOUBLE_EXACT(val,  3.0);
    get_double_array_index(dst, 2, &val);  ASSERT_DOUBLE_EXACT(val,  6.0);
    get_double_array_index(dst, 3, &val);  ASSERT_DOUBLE_EXACT(val, 10.0);

    return_double_array(src);
    return_double_array(dst);
}

// --------------------------------------------------------------------------------

static void test_double_cumulative_mixed_signs(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_array_expect_t r = init_double_array(8, false, alloc);
    assert_true(r.has_value);
    double_array_t* src = r.u.value;

    push_back_double_array(src,  4.0);
    push_back_double_array(src, -2.0);
    push_back_double_array(src,  1.0);
    push_back_double_array(src, -0.5);
    /* src: [4.0, -2.0, 1.0, -0.5]
     * expected: [4.0, 2.0, 3.0, 2.5] — all exact binary fractions */

    double_array_expect_t cr = cumulative_double_array(src, alloc);
    assert_true(cr.has_value);
    double_array_t* dst = cr.u.value;
    assert_int_equal((int)double_array_size(dst), 4);

    double val = 0.0;
    get_double_array_index(dst, 0, &val);  ASSERT_DOUBLE_EXACT(val, 4.0);
    get_double_array_index(dst, 1, &val);  ASSERT_DOUBLE_EXACT(val, 2.0);
    get_double_array_index(dst, 2, &val);  ASSERT_DOUBLE_EXACT(val, 3.0);
    get_double_array_index(dst, 3, &val);  ASSERT_DOUBLE_EXACT(val, 2.5);

    return_double_array(src);
    return_double_array(dst);
}


// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_double_array[] = {

    /* Group 1: init_double_array */
    cmocka_unit_test(test_double_init_null_allocate_fn_fails),
    cmocka_unit_test(test_double_init_returns_valid_array),

    /* Group 2: return_double_array */
    cmocka_unit_test(test_double_return_null_is_safe),
    cmocka_unit_test(test_double_return_valid_array_does_not_crash),

    /* Group 3: push_back_double_array */
    cmocka_unit_test(test_double_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_push_back_appends_value),

    /* Group 4: push_front_double_array */
    cmocka_unit_test(test_double_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_push_front_prepends_value),

    /* Group 5: push_at_double_array */
    cmocka_unit_test(test_double_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_push_at_inserts_at_index),

    /* Group 6: get_double_array_index */
    cmocka_unit_test(test_double_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_get_returns_correct_value),

    /* Group 7: pop_back_double_array */
    cmocka_unit_test(test_double_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_pop_back_removes_last_element),

    /* Group 8: pop_front_double_array */
    cmocka_unit_test(test_double_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_pop_front_removes_first_element),

    /* Group 9: pop_any_double_array */
    cmocka_unit_test(test_double_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_pop_any_removes_element_at_index),

    /* Group 10: clear_double_array */
    cmocka_unit_test(test_double_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_clear_resets_len_to_zero),

    /* Group 11: set_double_array_index */
    cmocka_unit_test(test_double_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_set_overwrites_element),

    /* Group 12: copy_double_array */
    cmocka_unit_test(test_double_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_double_copy_produces_independent_array),

    /* Group 13: concat_double_array */
    cmocka_unit_test(test_double_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_double_concat_appends_elements),

    /* Group 14: slice_double_array */
    cmocka_unit_test(test_double_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_double_slice_returns_correct_subrange),

    /* Group 15: reverse_double_array */
    cmocka_unit_test(test_double_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_reverse_reverses_elements),

    /* Group 16: sort_double_array */
    cmocka_unit_test(test_double_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_sort_forward_basic),
    cmocka_unit_test(test_double_sort_negative_values_and_infinity),

    /* Group 17: double_array_contains */
    cmocka_unit_test(test_double_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_contains_negative_tolerance_returns_invalid_arg),
    cmocka_unit_test(test_double_contains_exact_mode_finds_value),
    cmocka_unit_test(test_double_contains_absolute_tolerance),
    cmocka_unit_test(test_double_contains_relative_tolerance),
    cmocka_unit_test(test_double_contains_combined_tolerance),

    /* Group 18: double_array_binary_search */
    cmocka_unit_test(test_double_binary_search_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_binary_search_negative_tolerance_returns_invalid_arg),
    cmocka_unit_test(test_double_binary_search_exact_finds_value_with_sort),
    cmocka_unit_test(test_double_binary_search_exact_negative_and_infinity),
    cmocka_unit_test(test_double_binary_search_absolute_tolerance),
    cmocka_unit_test(test_double_binary_search_relative_tolerance),
    cmocka_unit_test(test_double_binary_search_combined_tolerance),
    cmocka_unit_test(test_double_binary_search_fanout_lands_between_values),

    /* Group 19: double_array_binary_bracket */
    cmocka_unit_test(test_double_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_binary_bracket_exact_match),
    cmocka_unit_test(test_double_binary_bracket_gap_between_values),

    /* Group 20: double_array_size */
    cmocka_unit_test(test_double_size_null_returns_zero),
    cmocka_unit_test(test_double_size_reflects_push_count),

    /* Group 21: double_array_alloc */
    cmocka_unit_test(test_double_alloc_null_returns_zero),
    cmocka_unit_test(test_double_alloc_matches_capacity),

    /* Group 22: double_array_data_size */
    cmocka_unit_test(test_double_data_size_null_returns_zero),
    cmocka_unit_test(test_double_data_size_is_eight),

    /* Group 23: is_double_array_empty */
    cmocka_unit_test(test_double_empty_null_returns_true),
    cmocka_unit_test(test_double_empty_reflects_contents),

    /* Group 24: is_double_array_full */
    cmocka_unit_test(test_double_full_null_returns_true),
    cmocka_unit_test(test_double_full_reflects_capacity),

    /* Group 25: is_double_array_ptr */
    cmocka_unit_test(test_double_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_double_is_ptr_valid_and_invalid),

    /* Group 26: double_array_min */
    cmocka_unit_test(test_double_min_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_min_returns_smallest_value),
    cmocka_unit_test(test_double_min_negative_infinity_wins),

    /* Group 27: double_array_max */
    cmocka_unit_test(test_double_max_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_max_returns_largest_value),
    cmocka_unit_test(test_double_max_positive_infinity_wins),

    /* Group 28: double_array_sum */
    // cmocka_unit_test(test_double_sum_null_array_returns_null_pointer),
    // cmocka_unit_test(test_double_sum_returns_correct_total),
    // cmocka_unit_test(test_double_sum_cancelling_values),

    /* Group 29: cumulative_double_array */
    cmocka_unit_test(test_double_cumulative_null_array_returns_null_pointer),
    cmocka_unit_test(test_double_cumulative_produces_prefix_sum),
    cmocka_unit_test(test_double_cumulative_mixed_signs),
};

const size_t test_double_array_count = sizeof(test_double_array) / sizeof(test_double_array[0]);
// ================================================================================ 
// ================================================================================ 

#define ASSERT_LDOUBLE_EXACT(a, b) assert_true((a) == (b))

// ================================================================================
// Group 1: init_ldouble_array
// ================================================================================

static void test_ldouble_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    ldouble_array_expect_t r = init_ldouble_array(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)ldouble_array_size(r.u.value), 0);
    assert_int_equal((int)ldouble_array_alloc(r.u.value), 8);
    return_ldouble_array(r.u.value);
}

// ================================================================================
// Group 2: return_ldouble_array
// ================================================================================

static void test_ldouble_return_null_is_safe(void** state) {
    (void)state;
    return_ldouble_array(NULL);
}

// --------------------------------------------------------------------------------

static void test_ldouble_return_valid_array_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    return_ldouble_array(r.u.value);
}

// ================================================================================
// Group 3: push_back_ldouble_array
// ================================================================================

static void test_ldouble_push_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_ldouble_array(NULL, 1.0L), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_push_back_appends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    assert_int_equal(push_back_ldouble_array(arr, -1.0L), NO_ERROR);
    assert_int_equal(push_back_ldouble_array(arr,  0.0L), NO_ERROR);
    assert_int_equal(push_back_ldouble_array(arr,  1.0L), NO_ERROR);
    assert_int_equal((int)ldouble_array_size(arr), 3);

    long double val = 0.0L;
    get_ldouble_array_index(arr, 0, &val);  ASSERT_LDOUBLE_EXACT(val, -1.0L);
    get_ldouble_array_index(arr, 2, &val);  ASSERT_LDOUBLE_EXACT(val,  1.0L);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 4: push_front_ldouble_array
// ================================================================================

static void test_ldouble_push_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_ldouble_array(NULL, 1.0L), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_push_front_prepends_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,  2.0L);
    push_front_ldouble_array(arr, 1.0L);
    /* arr is [1.0L, 2.0L] */

    long double val = 0.0L;
    get_ldouble_array_index(arr, 0, &val);
    ASSERT_LDOUBLE_EXACT(val, 1.0L);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 5: push_at_ldouble_array
// ================================================================================

static void test_ldouble_push_at_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_ldouble_array(NULL, 0, 1.0L), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_push_at_inserts_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 3.0L);
    push_at_ldouble_array(arr, 1, 2.0L);
    /* arr is [1.0L, 2.0L, 3.0L] */

    long double val = 0.0L;
    get_ldouble_array_index(arr, 1, &val);
    ASSERT_LDOUBLE_EXACT(val, 2.0L);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 6: get_ldouble_array_index
// ================================================================================

static void test_ldouble_get_null_array_returns_null_pointer(void** state) {
    (void)state;
    long double val = 0.0L;
    assert_int_equal(get_ldouble_array_index(NULL, 0, &val), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_get_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, -1.5L);
    push_back_ldouble_array(arr,  0.0L);
    push_back_ldouble_array(arr,  1.5L);

    long double val = 0.0L;
    assert_int_equal(get_ldouble_array_index(arr, 0, &val), NO_ERROR);
    ASSERT_LDOUBLE_EXACT(val, -1.5L);
    assert_int_equal(get_ldouble_array_index(arr, 2, &val), NO_ERROR);
    ASSERT_LDOUBLE_EXACT(val,  1.5L);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 7: pop_back_ldouble_array
// ================================================================================

static void test_ldouble_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_ldouble_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_pop_back_removes_last_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);

    long double val = 0.0L;
    assert_int_equal(pop_back_ldouble_array(arr, &val), NO_ERROR);
    ASSERT_LDOUBLE_EXACT(val, 2.0L);
    assert_int_equal((int)ldouble_array_size(arr), 1);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 8: pop_front_ldouble_array
// ================================================================================

static void test_ldouble_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_ldouble_array(NULL, NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_pop_front_removes_first_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);

    long double val = 0.0L;
    assert_int_equal(pop_front_ldouble_array(arr, &val), NO_ERROR);
    ASSERT_LDOUBLE_EXACT(val, 1.0L);
    assert_int_equal((int)ldouble_array_size(arr), 1);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 9: pop_any_ldouble_array
// ================================================================================

static void test_ldouble_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_ldouble_array(NULL, NULL, 0), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_pop_any_removes_element_at_index(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);
    push_back_ldouble_array(arr, 3.0L);

    long double val = 0.0L;
    assert_int_equal(pop_any_ldouble_array(arr, &val, 1), NO_ERROR);
    ASSERT_LDOUBLE_EXACT(val, 2.0L);
    assert_int_equal((int)ldouble_array_size(arr), 2);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 10: clear_ldouble_array
// ================================================================================

static void test_ldouble_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_ldouble_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_clear_resets_len_to_zero(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);
    assert_int_equal(clear_ldouble_array(arr), NO_ERROR);
    assert_int_equal((int)ldouble_array_size(arr), 0);
    assert_int_equal((int)ldouble_array_alloc(arr), 4);   /* capacity retained */

    return_ldouble_array(arr);
}

// ================================================================================
// Group 11: set_ldouble_array_index
// ================================================================================

static void test_ldouble_set_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_ldouble_array_index(NULL, 0, 1.0L), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_set_overwrites_element(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 1.0L);
    assert_int_equal(set_ldouble_array_index(arr, 0, -99.5L), NO_ERROR);

    long double val = 0.0L;
    get_ldouble_array_index(arr, 0, &val);
    ASSERT_LDOUBLE_EXACT(val, -99.5L);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 12: copy_ldouble_array
// ================================================================================

static void test_ldouble_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = copy_ldouble_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* src = r.u.value;

    push_back_ldouble_array(src, 1.0L);
    push_back_ldouble_array(src, 2.0L);

    ldouble_array_expect_t cr = copy_ldouble_array(src, alloc);
    assert_true(cr.has_value);
    ldouble_array_t* dst = cr.u.value;

    /* Mutating src must not affect the copy. */
    set_ldouble_array_index(src, 0, 999.0L);
    long double val = 0.0L;
    get_ldouble_array_index(dst, 0, &val);
    ASSERT_LDOUBLE_EXACT(val, 1.0L);

    return_ldouble_array(src);
    return_ldouble_array(dst);
}

// ================================================================================
// Group 13: concat_ldouble_array
// ================================================================================

static void test_ldouble_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* src = r.u.value;
    assert_int_equal(concat_ldouble_array(NULL, src), NULL_POINTER);
    return_ldouble_array(src);
}

// --------------------------------------------------------------------------------

static void test_ldouble_concat_appends_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r1 = init_ldouble_array(8, false, alloc);
    ldouble_array_expect_t r2 = init_ldouble_array(4, false, alloc);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
    ldouble_array_t* dst = r1.u.value;
    ldouble_array_t* src = r2.u.value;

    push_back_ldouble_array(dst, 1.0L);
    push_back_ldouble_array(src, 2.0L);

    assert_int_equal(concat_ldouble_array(dst, src), NO_ERROR);
    assert_int_equal((int)ldouble_array_size(dst), 2);

    long double val = 0.0L;
    get_ldouble_array_index(dst, 1, &val);
    ASSERT_LDOUBLE_EXACT(val, 2.0L);

    return_ldouble_array(dst);
    return_ldouble_array(src);
}

// ================================================================================
// Group 14: slice_ldouble_array
// ================================================================================

static void test_ldouble_slice_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = slice_ldouble_array(NULL, 0, 1, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_slice_returns_correct_subrange(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* src = r.u.value;

    push_back_ldouble_array(src, 1.0L);
    push_back_ldouble_array(src, 2.0L);
    push_back_ldouble_array(src, 3.0L);
    push_back_ldouble_array(src, 4.0L);
    push_back_ldouble_array(src, 5.0L);

    ldouble_array_expect_t sr = slice_ldouble_array(src, 1, 4, alloc);
    assert_true(sr.has_value);
    ldouble_array_t* slc = sr.u.value;
    assert_int_equal((int)ldouble_array_size(slc), 3);

    long double val = 0.0L;
    get_ldouble_array_index(slc, 0, &val);  ASSERT_LDOUBLE_EXACT(val, 2.0L);
    get_ldouble_array_index(slc, 1, &val);  ASSERT_LDOUBLE_EXACT(val, 3.0L);
    get_ldouble_array_index(slc, 2, &val);  ASSERT_LDOUBLE_EXACT(val, 4.0L);

    return_ldouble_array(src);
    return_ldouble_array(slc);
}

// ================================================================================
// Group 15: reverse_ldouble_array
// ================================================================================

static void test_ldouble_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_ldouble_array(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_reverse_reverses_elements(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);
    push_back_ldouble_array(arr, 3.0L);

    assert_int_equal(reverse_ldouble_array(arr), NO_ERROR);

    long double val = 0.0L;
    
    get_ldouble_array_index(arr, 0, &val);  ASSERT_LDOUBLE_EXACT(val, 3.0L);
    //get_ldouble_array_index(arr, 2, &val);  ASSERT_LDOUBLE_EXACT(val, 1.0L);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 16: sort_ldouble_array
// ================================================================================

static void test_ldouble_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_ldouble_array(NULL, FORWARD), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_sort_forward_basic(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,  3.0L);
    push_back_ldouble_array(arr, -1.5L);
    push_back_ldouble_array(arr,  2.5L);

    assert_int_equal(sort_ldouble_array(arr, FORWARD), NO_ERROR);

    long double val = 0.0L;
    get_ldouble_array_index(arr, 0, &val);  ASSERT_LDOUBLE_EXACT(val, -1.5L);
    get_ldouble_array_index(arr, 1, &val);  ASSERT_LDOUBLE_EXACT(val,  2.5L);
    get_ldouble_array_index(arr, 2, &val);  ASSERT_LDOUBLE_EXACT(val,  3.0L);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_sort_negative_values_and_infinity(void** state) {
    (void)state;
    /*
     * IEEE 754 total order for finite long doubles and infinities:
     *   -INFINITY < negative finite values < 0.0L < positive finite values < INFINITY
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,  1.0L);
    push_back_ldouble_array(arr, -INFINITY);
    push_back_ldouble_array(arr, -1.0L);
    push_back_ldouble_array(arr,  INFINITY);
    push_back_ldouble_array(arr,  0.0L);

    assert_int_equal(sort_ldouble_array(arr, FORWARD), NO_ERROR);

    long double val = 0.0L;
    get_ldouble_array_index(arr, 0, &val);  assert_true(isinf((double)val) && val < 0.0L);
    get_ldouble_array_index(arr, 1, &val);  ASSERT_LDOUBLE_EXACT(val, -1.0L);
    get_ldouble_array_index(arr, 2, &val);  ASSERT_LDOUBLE_EXACT(val,  0.0L);
    get_ldouble_array_index(arr, 3, &val);  ASSERT_LDOUBLE_EXACT(val,  1.0L);
    get_ldouble_array_index(arr, 4, &val);  assert_true(isinf((double)val) && val > 0.0L);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 17: ldouble_array_contains
// ================================================================================

static void test_ldouble_contains_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = ldouble_array_contains(NULL, 0.0L, 0, 1, 0.0L, 0.0L);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_contains_negative_tolerance_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;
    push_back_ldouble_array(arr, 1.0L);

    size_expect_t sr = ldouble_array_contains(arr, 1.0L, 0, 1, -0.1L, 0.0L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    sr = ldouble_array_contains(arr, 1.0L, 0, 1, 0.0L, -0.1L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_contains_exact_mode_finds_value(void** state) {
    (void)state;
    /*
     * Exact mode (both tolerances 0.0L): bitwise equality.
     * -0.0L and +0.0L have different bit patterns so they do NOT match.
     * NaN != NaN so a stored NaN is never found.
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, -1.0L);
    push_back_ldouble_array(arr,  2.5L);
    push_back_ldouble_array(arr, -3.0L);
    push_back_ldouble_array(arr,  0.0L);  /* +0.0L */

    /* Value present — found at correct index. */
    size_expect_t sr = ldouble_array_contains(arr, 2.5L, 0, 4, 0.0L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    /* Searching for -0.0L must NOT find +0.0L at index 3. */
    sr = ldouble_array_contains(arr, -0.0L, 0, 4, 0.0L, 0.0L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    /* Searching for +0.0L must find it at index 3. */
    sr = ldouble_array_contains(arr, 0.0L, 0, 4, 0.0L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 3);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_contains_absolute_tolerance(void** state) {
    (void)state;
    /*
     * Absolute mode (rel_tol == 0.0L): match when |element - needle| <= abs_tol.
     * arr: [0.0L, 1.0L, 2.0L, 3.0L]
     * needle=1.05L, abs_tol=0.1L: |1.0L - 1.05L| = 0.05L <= 0.1L → match at 1.
     * needle=1.5L,  abs_tol=0.1L: nearest elements differ by 0.5L > 0.1L → NOT_FOUND.
     *
     * Long-double-specific: use a sub-double-precision abs_tol (1e-17L) to
     * confirm the extra mantissa bits are honoured.
     * -0.0L and +0.0L match under tolerance because |(-0.0L) - 0.0L| = 0.0L.
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 0.0L);
    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);
    push_back_ldouble_array(arr, 3.0L);

    size_expect_t sr = ldouble_array_contains(arr, 1.05L, 0, 4, 0.1L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = ldouble_array_contains(arr, 1.5L, 0, 4, 0.1L, 0.0L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    /* -0.0L vs +0.0L with sub-double-precision floor tolerance. */
    sr = ldouble_array_contains(arr, -0.0L, 0, 1, 1e-17L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_contains_relative_tolerance(void** state) {
    (void)state;
    /*
     * Relative mode (abs_tol == 0.0L): match when
     *   |element - needle| <= rel_tol * max(|element|, |needle|).
     *
     * arr: [100.0L, 1000.0L, 10000.0L]
     * rel_tol 0.01L (1%):
     *   needle 1005.0L vs 1000.0L: diff=5, threshold=0.01L*1005=10.05L — match at 1
     *   needle  110.0L vs  100.0L: diff=10, threshold=0.01L*110=1.1L   — no match
     *
     * Long-double-specific: verify a 1e-17L relative tolerance that would
     * be indistinguishable in double but is meaningful in extended precision.
     * arr2: [1.0L + 1e-18L] (representable as long double but not as double)
     * needle=1.0L, rel_tol=1e-17L: diff=1e-18L <= 1e-17L*1 = 1e-17L — match
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,   100.0L);
    push_back_ldouble_array(arr,  1000.0L);
    push_back_ldouble_array(arr, 10000.0L);

    size_expect_t sr = ldouble_array_contains(arr, 1005.0L, 0, 3, 0.0L, 0.01L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = ldouble_array_contains(arr, 110.0L, 0, 3, 0.0L, 0.01L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_ldouble_array(arr);

    /* Extended-precision relative tolerance. */
    ldouble_array_expect_t r2 = init_ldouble_array(4, false, alloc);
    assert_true(r2.has_value);
    ldouble_array_t* arr2 = r2.u.value;
    push_back_ldouble_array(arr2, 1.0L + 1e-18L);  /* distinct from 1.0L */

    sr = ldouble_array_contains(arr2, 1.0L, 0, 1, 0.0L, 1e-17L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_ldouble_array(arr2);
}

// --------------------------------------------------------------------------------

static void test_ldouble_contains_combined_tolerance(void** state) {
    (void)state;
    /*
     * Combined mode: threshold = max(abs_tol, rel_tol * max(|e|, |n|)).
     * abs_tol acts as a floor so near zero the threshold doesn't collapse.
     *
     * arr: [0.0L, 1000.0L]
     * abs_tol=1e-15L, rel_tol=0.01L:
     *   needle=0.0L:   threshold=max(1e-15L, 0.0L)=1e-15L → match at index 0
     *   needle=1e-18L: diff=1e-18L <= 1e-15L (abs_tol floor) → match at index 0
     *
     * Pure relative alone misses: needle=1e-18L, abs_tol=0, rel_tol=0.01L
     *   threshold=0.01L*1e-18L=1e-20L < 1e-18L → NOT_FOUND
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,    0.0L);
    push_back_ldouble_array(arr, 1000.0L);

    size_expect_t sr = ldouble_array_contains(arr, 0.0L, 0, 2, 1e-15L, 0.01L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    /* abs_tol floor rescues sub-1e-15L needle near zero */
    sr = ldouble_array_contains(arr, 1e-18L, 0, 1, 1e-15L, 0.01L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    /* Pure relative alone collapses near zero — no match */
    sr = ldouble_array_contains(arr, 1e-18L, 0, 1, 0.0L, 0.01L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 18: ldouble_array_binary_search
// ================================================================================

static void test_ldouble_binary_search_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = ldouble_array_binary_search(NULL, 0.0L, false, 0.0L, 0.0L);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_binary_search_negative_tolerance_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;
    push_back_ldouble_array(arr, 1.0L);

    size_expect_t sr = ldouble_array_binary_search(arr, 1.0L, false, -0.1L, 0.0L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    sr = ldouble_array_binary_search(arr, 1.0L, false, 0.0L, -0.1L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, INVALID_ARG);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_binary_search_exact_finds_value_with_sort(void** state) {
    (void)state;
    /*
     * Exact mode (both tolerances 0.0L): confirms the sort path and that the
     * first occurrence of a duplicate is returned.
     * arr (unsorted): [3.0L, -1.5L, 2.5L, 2.5L, 4.0L]
     * After sort:     [-1.5L, 2.5L, 2.5L, 3.0L, 4.0L]
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,  3.0L);
    push_back_ldouble_array(arr, -1.5L);
    push_back_ldouble_array(arr,  2.5L);
    push_back_ldouble_array(arr,  2.5L);
    push_back_ldouble_array(arr,  4.0L);

    size_expect_t sr = ldouble_array_binary_search(arr, 2.5L, true, 0.0L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);   /* first of the two 2.5L entries */

    sr = ldouble_array_binary_search(arr, 99.0L, false, 0.0L, 0.0L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_binary_search_exact_negative_and_infinity(void** state) {
    (void)state;
    /*
     * Exact mode must find -INFINITY and +INFINITY without producing NaN
     * from (inf - inf). The _biteq_ldouble path handles this correctly.
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    /* Already sorted: [-INFINITY, -1.0L, 0.0L, 1.0L, INFINITY] */
    push_back_ldouble_array(arr, -INFINITY);
    push_back_ldouble_array(arr,    -1.0L);
    push_back_ldouble_array(arr,     0.0L);
    push_back_ldouble_array(arr,     1.0L);
    push_back_ldouble_array(arr,  INFINITY);

    size_expect_t sr = ldouble_array_binary_search(arr, -INFINITY, false, 0.0L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    sr = ldouble_array_binary_search(arr, INFINITY, false, 0.0L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 4);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_binary_search_absolute_tolerance(void** state) {
    (void)state;
    /*
     * Absolute tolerance: match when |element - needle| <= abs_tol.
     *
     * arr (sorted): [0.0L, 1.0L, 2.0L, 3.0L, 4.0L]
     *
     * needle=1.3L, abs_tol=0.5L: element 1.0L (diff=0.3L <= 0.5L) — match at 1.
     * needle=1.5L, abs_tol=0.5L: elements 1.0L and 2.0L both qualify;
     *   lowest index is 1.
     * needle=5.0L, abs_tol=0.1L: nothing in range → NOT_FOUND.
     *
     * Long-double-specific: use a sub-double-precision abs_tol (1e-17L).
     * arr2: [1.0L, 1.0L + 5e-18L]
     * needle=1.0L, abs_tol=1e-17L: both elements qualify; lowest is index 0.
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 0.0L);
    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);
    push_back_ldouble_array(arr, 3.0L);
    push_back_ldouble_array(arr, 4.0L);

    size_expect_t sr = ldouble_array_binary_search(arr, 1.3L, false, 0.5L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = ldouble_array_binary_search(arr, 1.5L, false, 0.5L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = ldouble_array_binary_search(arr, 5.0L, false, 0.1L, 0.0L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_ldouble_array(arr);

    /* Long-double sub-double-precision abs_tol. */
    ldouble_array_expect_t r2 = init_ldouble_array(4, false, alloc);
    assert_true(r2.has_value);
    ldouble_array_t* arr2 = r2.u.value;
    push_back_ldouble_array(arr2, 1.0L);
    push_back_ldouble_array(arr2, 1.0L + 5e-18L);

    sr = ldouble_array_binary_search(arr2, 1.0L, false, 1e-17L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    return_ldouble_array(arr2);
}

// --------------------------------------------------------------------------------

static void test_ldouble_binary_search_relative_tolerance(void** state) {
    (void)state;
    /*
     * Relative tolerance: match when |element - needle| <=
     *   rel_tol * max(|element|, |needle|).
     *
     * arr (sorted): [100.0L, 1000.0L, 10000.0L]
     * rel_tol=0.01L:
     *   needle=1005.0L: threshold=0.01L*1005=10.05L; 1000.0L diff=5 → match at 1.
     *   needle=115.0L:  threshold=0.01L*115=1.15L;   100.0L  diff=15 → NOT_FOUND.
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,   100.0L);
    push_back_ldouble_array(arr,  1000.0L);
    push_back_ldouble_array(arr, 10000.0L);

    size_expect_t sr = ldouble_array_binary_search(arr, 1005.0L, false, 0.0L, 0.01L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    sr = ldouble_array_binary_search(arr, 115.0L, false, 0.0L, 0.01L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_binary_search_combined_tolerance(void** state) {
    (void)state;
    /*
     * Combined tolerance: threshold = max(abs_tol,
     *                                     rel_tol * max(|element|, |needle|)).
     *
     * arr (sorted): [0.0L, 0.001L, 1000.0L]
     * abs_tol=1e-3L, rel_tol=0.01L:
     *   needle=0.0L: threshold=max(1e-3L, 0)=1e-3L
     *     0.0L   diff=0      <= 1e-3L → match
     *     0.001L diff=1e-3L  <= 1e-3L → also within; lowest is index 0.
     *
     *   needle=1e-6L: abs_tol floor (1e-3L >= 1e-6L) → match at index 0.
     *
     * Long-double-specific: abs_tol=1e-15L floor with needle=1e-16L.
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,    0.0L);
    push_back_ldouble_array(arr,    0.001L);
    push_back_ldouble_array(arr, 1000.0L);

    size_expect_t sr = ldouble_array_binary_search(arr, 0.0L, false, 1e-3L, 0.01L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    sr = ldouble_array_binary_search(arr, 1e-6L, false, 1e-3L, 0.01L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 0);

    /* Sub-double abs_tol floor. */
    return_ldouble_array(arr);

    ldouble_array_expect_t r2 = init_ldouble_array(4, false, alloc);
    assert_true(r2.has_value);
    ldouble_array_t* arr2 = r2.u.value;
    push_back_ldouble_array(arr2, 0.0L);

    sr = ldouble_array_binary_search(arr2, 1e-16L, false, 1e-15L, 0.0L);
    assert_true(sr.has_value);   /* diff=1e-16L <= abs_tol=1e-15L */
    assert_int_equal((int)sr.u.value, 0);

    return_ldouble_array(arr2);
}

// --------------------------------------------------------------------------------

static void test_ldouble_binary_search_fanout_lands_between_values(void** state) {
    (void)state;
    /*
     * Bisection lands at a position outside tolerance; the fan-out must find
     * the lower-index match.
     *
     * arr (sorted): [1.0L, 2.0L, 3.0L, 4.0L]
     * needle=2.4L, abs_tol=0.5L:
     *   bisection lands at index 2 (3.0L), diff=0.6L > 0.5L — no match.
     *   fan-out left: index 1 (2.0L), diff=0.4L <= 0.5L → best=1.
     *                 index 0 (1.0L), diff=1.4L > 0.5L  → stop.
     *   lowest match is index 1.
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);
    push_back_ldouble_array(arr, 3.0L);
    push_back_ldouble_array(arr, 4.0L);

    size_expect_t sr = ldouble_array_binary_search(arr, 2.4L, false, 0.5L, 0.0L);
    assert_true(sr.has_value);
    assert_int_equal((int)sr.u.value, 1);

    /* needle far outside range → NOT_FOUND even with generous tolerance */
    sr = ldouble_array_binary_search(arr, 100.0L, false, 0.5L, 0.0L);
    assert_false(sr.has_value);
    assert_int_equal(sr.u.error, NOT_FOUND);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 19: ldouble_array_binary_bracket
// ================================================================================

static void test_ldouble_binary_bracket_null_array_returns_null_pointer(void** state) {
    (void)state;
    bracket_expect_t r = ldouble_array_binary_bracket(NULL, 0.0L, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_binary_bracket_exact_match(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, -2.0L);
    push_back_ldouble_array(arr, -1.0L);
    push_back_ldouble_array(arr,  1.0L);
    push_back_ldouble_array(arr,  2.0L);

    bracket_expect_t br = ldouble_array_binary_bracket(arr, -1.0L, false);
    assert_true(br.has_value);
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 1);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_binary_bracket_gap_between_values(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    /* [-2.0L, -1.0L, 1.0L, 2.0L] — gap between -1.0L and 1.0L */
    push_back_ldouble_array(arr, -2.0L);
    push_back_ldouble_array(arr, -1.0L);
    push_back_ldouble_array(arr,  1.0L);
    push_back_ldouble_array(arr,  2.0L);

    /* 0.0L falls in the gap */
    bracket_expect_t br = ldouble_array_binary_bracket(arr, 0.0L, false);
    assert_true(br.has_value);
    /* lower = first index >= 0.0L = index 2 (value  1.0L) */
    /* upper = last  index <= 0.0L = index 1 (value -1.0L) */
    assert_int_equal((int)br.u.value.lower, 1);
    assert_int_equal((int)br.u.value.upper, 2);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 20: ldouble_array_size
// ================================================================================

static void test_ldouble_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)ldouble_array_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_ldouble_size_reflects_push_count(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    assert_int_equal((int)ldouble_array_size(arr), 0);
    push_back_ldouble_array(arr, -1.0L);
    push_back_ldouble_array(arr,  2.0L);
    assert_int_equal((int)ldouble_array_size(arr), 2);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 21: ldouble_array_alloc
// ================================================================================

static void test_ldouble_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)ldouble_array_alloc(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_ldouble_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(16, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)ldouble_array_alloc(r.u.value), 16);
    return_ldouble_array(r.u.value);
}

// ================================================================================
// Group 22: ldouble_array_data_size
// ================================================================================

static void test_ldouble_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)ldouble_array_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_ldouble_data_size_matches_sizeof(void** state) {
    (void)state;
    /*
     * sizeof(long double) is platform-dependent: 12 or 16 on x86-64
     * (depending on ABI/compiler), 16 on ARM64. We assert against the
     * compile-time constant rather than a hardcoded number so the test is
     * correct on all supported platforms.
     */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    assert_int_equal((int)ldouble_array_data_size(r.u.value),
                     (int)sizeof(long double));
    return_ldouble_array(r.u.value);
}

// ================================================================================
// Group 23: is_ldouble_array_empty
// ================================================================================

static void test_ldouble_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_ldouble_array_empty(NULL));
}

// --------------------------------------------------------------------------------

static void test_ldouble_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(4, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    assert_true(is_ldouble_array_empty(arr));
    push_back_ldouble_array(arr, 1.0L);
    assert_false(is_ldouble_array_empty(arr));

    return_ldouble_array(arr);
}

// ================================================================================
// Group 24: is_ldouble_array_full
// ================================================================================

static void test_ldouble_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_ldouble_array_full(NULL));
}

// --------------------------------------------------------------------------------

static void test_ldouble_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(2, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    assert_false(is_ldouble_array_full(arr));
    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);
    assert_true(is_ldouble_array_full(arr));

    return_ldouble_array(arr);
}

// ================================================================================
// Group 25: is_ldouble_array_ptr
// ================================================================================

static void test_ldouble_is_ptr_null_array_returns_false(void** state) {
    (void)state;
    long double val = 0.0L;
    assert_false(is_ldouble_array_ptr(NULL, &val));
}

// --------------------------------------------------------------------------------

static void test_ldouble_is_ptr_valid_and_invalid(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, 1.0L);
    push_back_ldouble_array(arr, 2.0L);
    push_back_ldouble_array(arr, 3.0L);

    const long double* first = (const long double*)arr->base.data;
    const long double* last  = first + 2;
    const long double* spare = first + 3;   /* beyond live region */

    assert_true (is_ldouble_array_ptr(arr, first));
    assert_true (is_ldouble_array_ptr(arr, last));
    assert_false(is_ldouble_array_ptr(arr, spare));
    assert_false(is_ldouble_array_ptr(arr, NULL));

    return_ldouble_array(arr);
}

// ================================================================================
// Group 26: ldouble_array_min
// ================================================================================

static void test_ldouble_min_null_array_returns_null_pointer(void** state) {
    (void)state;
    ldouble_expect_t r = ldouble_array_min(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_min_returns_smallest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,  3.0L);
    push_back_ldouble_array(arr, -1.5L);
    push_back_ldouble_array(arr,  2.5L);

    ldouble_expect_t result = ldouble_array_min(arr);
    assert_true(result.has_value);
    ASSERT_LDOUBLE_EXACT(result.u.value, -1.5L);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_min_negative_infinity_wins(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,       0.0L);
    push_back_ldouble_array(arr,    1000.0L);
    push_back_ldouble_array(arr, -INFINITY);
    push_back_ldouble_array(arr,      -1.0L);

    ldouble_expect_t result = ldouble_array_min(arr);
    assert_true(result.has_value);
    assert_true(isinf((double)result.u.value) && result.u.value < 0.0L);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 27: ldouble_array_max
// ================================================================================

static void test_ldouble_max_null_array_returns_null_pointer(void** state) {
    (void)state;
    ldouble_expect_t r = ldouble_array_max(NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_max_returns_largest_value(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr, -2.0L);
    push_back_ldouble_array(arr,  3.5L);
    push_back_ldouble_array(arr,  1.0L);

    ldouble_expect_t result = ldouble_array_max(arr);
    assert_true(result.has_value);
    ASSERT_LDOUBLE_EXACT(result.u.value, 3.5L);

    return_ldouble_array(arr);
}

// --------------------------------------------------------------------------------

static void test_ldouble_max_positive_infinity_wins(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* arr = r.u.value;

    push_back_ldouble_array(arr,      -1.0L);
    push_back_ldouble_array(arr, -INFINITY);
    push_back_ldouble_array(arr,  INFINITY);
    push_back_ldouble_array(arr,       0.0L);

    ldouble_expect_t result = ldouble_array_max(arr);
    assert_true(result.has_value);
    assert_true(isinf((double)result.u.value) && result.u.value > 0.0L);

    return_ldouble_array(arr);
}

// ================================================================================
// Group 28: ldouble_array_sum
// ================================================================================

// static void test_ldouble_sum_null_array_returns_null_pointer(void** state) {
//     (void)state;
//     ldouble_expect_t r = ldouble_array_sum(NULL);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_ldouble_sum_returns_correct_total(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
//     assert_true(r.has_value);
//     ldouble_array_t* arr = r.u.value;
//
//     push_back_ldouble_array(arr,  1.0L);
//     push_back_ldouble_array(arr, -2.5L);
//     push_back_ldouble_array(arr,  3.5L);
//     /* arr: [1.0L, -2.5L, 3.5L], sum == 2.0L (exact in binary) */
//
//     ldouble_expect_t result = ldouble_array_sum(arr);
//     assert_true(result.has_value);
//     ASSERT_LDOUBLE_EXACT(result.u.value, 2.0L);
//
//     return_ldouble_array(arr);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_ldouble_sum_cancelling_values(void** state) {
//     (void)state;
//     /*
//      * All values are exact binary fractions so the sum is exactly 0.0L.
//      */
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
//     assert_true(r.has_value);
//     ldouble_array_t* arr = r.u.value;
//
//     push_back_ldouble_array(arr,  4.0L);
//     push_back_ldouble_array(arr, -4.0L);
//     push_back_ldouble_array(arr,  0.5L);
//     push_back_ldouble_array(arr, -0.5L);
//
//     ldouble_expect_t result = ldouble_array_sum(arr);
//     assert_true(result.has_value);
//     ASSERT_LDOUBLE_EXACT(result.u.value, 0.0L);
//
//     return_ldouble_array(arr);
// }

// ================================================================================
// Group 29: cumulative_ldouble_array
// ================================================================================

static void test_ldouble_cumulative_null_array_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = cumulative_ldouble_array(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_ldouble_cumulative_produces_prefix_sum(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* src = r.u.value;

    push_back_ldouble_array(src, 1.0L);
    push_back_ldouble_array(src, 2.0L);
    push_back_ldouble_array(src, 3.0L);
    push_back_ldouble_array(src, 4.0L);
    /* expected: [1.0L, 3.0L, 6.0L, 10.0L] — all exact binary fractions */

    ldouble_array_expect_t cr = cumulative_ldouble_array(src, alloc);
    assert_true(cr.has_value);
    ldouble_array_t* dst = cr.u.value;
    assert_int_equal((int)ldouble_array_size(dst), 4);

    long double val = 0.0L;
    get_ldouble_array_index(dst, 0, &val);  ASSERT_LDOUBLE_EXACT(val,  1.0L);
    get_ldouble_array_index(dst, 1, &val);  ASSERT_LDOUBLE_EXACT(val,  3.0L);
    get_ldouble_array_index(dst, 2, &val);  ASSERT_LDOUBLE_EXACT(val,  6.0L);
    get_ldouble_array_index(dst, 3, &val);  ASSERT_LDOUBLE_EXACT(val, 10.0L);

    return_ldouble_array(src);
    return_ldouble_array(dst);
}

// --------------------------------------------------------------------------------

static void test_ldouble_cumulative_mixed_signs(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_array_expect_t r = init_ldouble_array(8, false, alloc);
    assert_true(r.has_value);
    ldouble_array_t* src = r.u.value;

    push_back_ldouble_array(src,  4.0L);
    push_back_ldouble_array(src, -2.0L);
    push_back_ldouble_array(src,  1.0L);
    push_back_ldouble_array(src, -0.5L);
    /* expected: [4.0L, 2.0L, 3.0L, 2.5L] — all exact binary fractions */

    ldouble_array_expect_t cr = cumulative_ldouble_array(src, alloc);
    assert_true(cr.has_value);
    ldouble_array_t* dst = cr.u.value;
    assert_int_equal((int)ldouble_array_size(dst), 4);

    long double val = 0.0L;
    get_ldouble_array_index(dst, 0, &val);  ASSERT_LDOUBLE_EXACT(val, 4.0L);
    get_ldouble_array_index(dst, 1, &val);  ASSERT_LDOUBLE_EXACT(val, 2.0L);
    get_ldouble_array_index(dst, 2, &val);  ASSERT_LDOUBLE_EXACT(val, 3.0L);
    get_ldouble_array_index(dst, 3, &val);  ASSERT_LDOUBLE_EXACT(val, 2.5L);

    return_ldouble_array(src);
    return_ldouble_array(dst);
}

// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_ldouble_array[] = {

    /* Group 1: init_ldouble_array */
    cmocka_unit_test(test_ldouble_init_null_allocate_fn_fails),
    cmocka_unit_test(test_ldouble_init_returns_valid_array),

    /* Group 2: return_ldouble_array */
    cmocka_unit_test(test_ldouble_return_null_is_safe),
    cmocka_unit_test(test_ldouble_return_valid_array_does_not_crash),

    /* Group 3: push_back_ldouble_array */
    cmocka_unit_test(test_ldouble_push_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_push_back_appends_value),

    /* Group 4: push_front_ldouble_array */
    cmocka_unit_test(test_ldouble_push_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_push_front_prepends_value),

    /* Group 5: push_at_ldouble_array */
    cmocka_unit_test(test_ldouble_push_at_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_push_at_inserts_at_index),

    /* Group 6: get_ldouble_array_index */
    cmocka_unit_test(test_ldouble_get_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_get_returns_correct_value),

    /* Group 7: pop_back_ldouble_array */
    cmocka_unit_test(test_ldouble_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_pop_back_removes_last_element),

    /* Group 8: pop_front_ldouble_array */
    cmocka_unit_test(test_ldouble_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_pop_front_removes_first_element),

    /* Group 9: pop_any_ldouble_array */
    cmocka_unit_test(test_ldouble_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_pop_any_removes_element_at_index),

    /* Group 10: clear_ldouble_array */
    cmocka_unit_test(test_ldouble_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_clear_resets_len_to_zero),

    /* Group 11: set_ldouble_array_index */
    cmocka_unit_test(test_ldouble_set_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_set_overwrites_element),

    /* Group 12: copy_ldouble_array */
    cmocka_unit_test(test_ldouble_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_ldouble_copy_produces_independent_array),

    /* Group 13: concat_ldouble_array */
    cmocka_unit_test(test_ldouble_concat_null_dst_returns_null_pointer),
    cmocka_unit_test(test_ldouble_concat_appends_elements),

    /* Group 14: slice_ldouble_array */
    cmocka_unit_test(test_ldouble_slice_null_src_returns_null_pointer),
    cmocka_unit_test(test_ldouble_slice_returns_correct_subrange),

    /* Group 15: reverse_ldouble_array */
    cmocka_unit_test(test_ldouble_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_reverse_reverses_elements),

    /* Group 16: sort_ldouble_array */
    cmocka_unit_test(test_ldouble_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_sort_forward_basic),
    cmocka_unit_test(test_ldouble_sort_negative_values_and_infinity),

    /* Group 17: ldouble_array_contains */
    cmocka_unit_test(test_ldouble_contains_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_contains_negative_tolerance_returns_invalid_arg),
    cmocka_unit_test(test_ldouble_contains_exact_mode_finds_value),
    cmocka_unit_test(test_ldouble_contains_absolute_tolerance),
    cmocka_unit_test(test_ldouble_contains_relative_tolerance),
    cmocka_unit_test(test_ldouble_contains_combined_tolerance),

    /* Group 18: ldouble_array_binary_search */
    cmocka_unit_test(test_ldouble_binary_search_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_binary_search_negative_tolerance_returns_invalid_arg),
    cmocka_unit_test(test_ldouble_binary_search_exact_finds_value_with_sort),
    cmocka_unit_test(test_ldouble_binary_search_exact_negative_and_infinity),
    cmocka_unit_test(test_ldouble_binary_search_absolute_tolerance),
    cmocka_unit_test(test_ldouble_binary_search_relative_tolerance),
    cmocka_unit_test(test_ldouble_binary_search_combined_tolerance),
    cmocka_unit_test(test_ldouble_binary_search_fanout_lands_between_values),

    /* Group 19: ldouble_array_binary_bracket */
    cmocka_unit_test(test_ldouble_binary_bracket_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_binary_bracket_exact_match),
    cmocka_unit_test(test_ldouble_binary_bracket_gap_between_values),

    /* Group 20: ldouble_array_size */
    cmocka_unit_test(test_ldouble_size_null_returns_zero),
    cmocka_unit_test(test_ldouble_size_reflects_push_count),

    /* Group 21: ldouble_array_alloc */
    cmocka_unit_test(test_ldouble_alloc_null_returns_zero),
    cmocka_unit_test(test_ldouble_alloc_matches_capacity),

    /* Group 22: ldouble_array_data_size */
    cmocka_unit_test(test_ldouble_data_size_null_returns_zero),
    cmocka_unit_test(test_ldouble_data_size_matches_sizeof),

    /* Group 23: is_ldouble_array_empty */
    cmocka_unit_test(test_ldouble_empty_null_returns_true),
    cmocka_unit_test(test_ldouble_empty_reflects_contents),

    /* Group 24: is_ldouble_array_full */
    cmocka_unit_test(test_ldouble_full_null_returns_true),
    cmocka_unit_test(test_ldouble_full_reflects_capacity),

    /* Group 25: is_ldouble_array_ptr */
    cmocka_unit_test(test_ldouble_is_ptr_null_array_returns_false),
    cmocka_unit_test(test_ldouble_is_ptr_valid_and_invalid),

    /* Group 26: ldouble_array_min */
    cmocka_unit_test(test_ldouble_min_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_min_returns_smallest_value),
    cmocka_unit_test(test_ldouble_min_negative_infinity_wins),

    /* Group 27: ldouble_array_max */
    cmocka_unit_test(test_ldouble_max_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_max_returns_largest_value),
    cmocka_unit_test(test_ldouble_max_positive_infinity_wins),

    /* Group 28: ldouble_array_sum */
    // cmocka_unit_test(test_ldouble_sum_null_array_returns_null_pointer),
    // cmocka_unit_test(test_ldouble_sum_returns_correct_total),
    // cmocka_unit_test(test_ldouble_sum_cancelling_values),

    /* Group 29: cumulative_ldouble_array */
    cmocka_unit_test(test_ldouble_cumulative_null_array_returns_null_pointer),
    cmocka_unit_test(test_ldouble_cumulative_produces_prefix_sum),
    cmocka_unit_test(test_ldouble_cumulative_mixed_signs),
};

const size_t test_ldouble_array_count = sizeof(test_ldouble_array) / sizeof(test_ldouble_array[0]);
// ================================================================================ 
// ================================================================================ 

/* Build a heap str_array_t with growth enabled. */
static str_array_t* _make_array(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    str_array_expect_t r = init_str_array(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
/* Build a string_t* from a literal using the heap allocator. */
static string_t* _make_str(const char* lit) {
    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string(lit, 0u, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
// ================================================================================
// Group 1: init_str_array
// ================================================================================
 
static void test_str_init_null_allocate_fn_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    str_array_expect_t r = init_str_array(8, false, bad);
    assert_false(r.has_value);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_init_returns_valid_array(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    str_array_expect_t r = init_str_array(8, false, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)str_array_size(r.u.value), 0);
    assert_int_equal((int)str_array_alloc(r.u.value), 8);
    return_str_array(r.u.value);
}
 
// ================================================================================
// Group 2: return_str_array
// ================================================================================
 
static void test_str_return_null_is_safe(void** state) {
    (void)state;
    return_str_array(NULL);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_return_releases_owned_strings(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "alpha"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "beta"),  NO_ERROR);
    return_str_array(arr);   /* must not crash or leak */
}
 
// ================================================================================
// Group 3: push_back_str
// ================================================================================
 
static void test_str_push_back_str_null_array_returns_null_pointer(void** state) {
    (void)state;
    string_t* s = _make_str("x");
    assert_int_equal(push_back_str(NULL, s), NULL_POINTER);
    return_string(s);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_back_str_null_src_returns_null_pointer(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_str(arr, NULL), NULL_POINTER);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_back_str_deep_copy_independent(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_t*    s   = _make_str("hello");
 
    assert_int_equal(push_back_str(arr, s), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 1);
 
    /* Mutate the original — stored copy must be unaffected. */
    str_concat(s, " world");
 
    string_expect_t g = get_str_array_index(arr, 0);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "hello");
    return_string(g.u.value);
 
    return_string(s);
    return_str_array(arr);
}
 
// ================================================================================
// Group 4: push_front_str
// ================================================================================
 
static void test_str_push_front_str_null_array_returns_null_pointer(void** state) {
    (void)state;
    string_t* s = _make_str("x");
    assert_int_equal(push_front_str(NULL, s), NULL_POINTER);
    return_string(s);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_front_str_prepends_correctly(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_t*    a   = _make_str("alpha");
    string_t*    b   = _make_str("beta");
 
    assert_int_equal(push_back_str(arr, a),  NO_ERROR);
    assert_int_equal(push_front_str(arr, b), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 2);
 
    string_expect_t g0 = get_str_array_index(arr, 0);
    string_expect_t g1 = get_str_array_index(arr, 1);
    assert_true(g0.has_value);
    assert_true(g1.has_value);
    assert_string_equal(const_string(g0.u.value), "beta");
    assert_string_equal(const_string(g1.u.value), "alpha");
    return_string(g0.u.value);
    return_string(g1.u.value);
 
    return_string(a);
    return_string(b);
    return_str_array(arr);
}
 
// ================================================================================
// Group 5: push_at_str
// ================================================================================
 
static void test_str_push_at_str_null_array_returns_null_pointer(void** state) {
    (void)state;
    string_t* s = _make_str("x");
    assert_int_equal(push_at_str(NULL, 0, s), NULL_POINTER);
    return_string(s);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_at_str_out_of_bounds_returns_error(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_t*    s   = _make_str("x");
    assert_int_equal(push_at_str(arr, 5, s), OUT_OF_BOUNDS);
    return_string(s);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_at_str_inserts_at_correct_position(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_t*    a   = _make_str("alpha");
    string_t*    b   = _make_str("beta");
    string_t*    c   = _make_str("gamma");
 
    assert_int_equal(push_back_str(arr, a), NO_ERROR);
    assert_int_equal(push_back_str(arr, c), NO_ERROR);
    assert_int_equal(push_at_str(arr, 1, b), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 3);
 
    string_expect_t g1 = get_str_array_index(arr, 1);
    assert_true(g1.has_value);
    assert_string_equal(const_string(g1.u.value), "beta");
    return_string(g1.u.value);
 
    return_string(a);
    return_string(b);
    return_string(c);
    return_str_array(arr);
}
 
// ================================================================================
// Group 6: push_back_lit
// ================================================================================
 
static void test_str_push_back_lit_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_back_lit(NULL, "x"), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_back_lit_null_lit_returns_null_pointer(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, NULL), NULL_POINTER);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_back_lit_stores_correct_value(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "one"),   NO_ERROR);
    assert_int_equal(push_back_lit(arr, "two"),   NO_ERROR);
    assert_int_equal(push_back_lit(arr, "three"), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 3);
 
    string_expect_t g = get_str_array_index(arr, 1);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "two");
    return_string(g.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 7: push_front_lit
// ================================================================================
 
static void test_str_push_front_lit_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_front_lit(NULL, "x"), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_front_lit_prepends_correctly(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr,  "second"), NO_ERROR);
    assert_int_equal(push_front_lit(arr, "first"),  NO_ERROR);
 
    string_expect_t g = get_str_array_index(arr, 0);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "first");
    return_string(g.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 8: push_at_lit
// ================================================================================
 
static void test_str_push_at_lit_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(push_at_lit(NULL, 0, "x"), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_at_lit_out_of_bounds_returns_error(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_at_lit(arr, 9, "x"), OUT_OF_BOUNDS);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_push_at_lit_inserts_at_correct_position(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "a"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "c"), NO_ERROR);
    assert_int_equal(push_at_lit(arr, 1, "b"), NO_ERROR);
 
    string_expect_t g0 = get_str_array_index(arr, 0);
    string_expect_t g1 = get_str_array_index(arr, 1);
    string_expect_t g2 = get_str_array_index(arr, 2);
    assert_true(g0.has_value && g1.has_value && g2.has_value);
    assert_string_equal(const_string(g0.u.value), "a");
    assert_string_equal(const_string(g1.u.value), "b");
    assert_string_equal(const_string(g2.u.value), "c");
    return_string(g0.u.value);
    return_string(g1.u.value);
    return_string(g2.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 9: get_str_array_index (owned deep copy)
// ================================================================================
 
static void test_str_get_index_null_array_returns_null_pointer(void** state) {
    (void)state;
    string_expect_t r = get_str_array_index(NULL, 0);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_get_index_out_of_bounds_returns_error(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_expect_t r = get_str_array_index(arr, 0);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, OUT_OF_BOUNDS);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_get_index_returns_independent_copy(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "original"), NO_ERROR);
 
    string_expect_t g = get_str_array_index(arr, 0);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "original");
 
    /* Mutate the copy — array's stored value must be unaffected. */
    str_concat(g.u.value, "-modified");
 
    string_expect_t g2 = get_str_array_ptr(arr, 0);
    assert_true(g2.has_value);
    assert_string_equal(const_string(g2.u.value), "original");
 
    return_string(g.u.value);
    return_str_array(arr);
}
 
// ================================================================================
// Group 10: get_str_array_ptr (borrowed pointer)
// ================================================================================
 
static void test_str_get_ptr_null_array_returns_null_pointer(void** state) {
    (void)state;
    string_expect_t r = get_str_array_ptr(NULL, 0);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_get_ptr_out_of_bounds_returns_error(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_expect_t r = get_str_array_ptr(arr, 0);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, OUT_OF_BOUNDS);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_get_ptr_returns_correct_value_without_copy(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "alpha"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "beta"),  NO_ERROR);
 
    string_expect_t g = get_str_array_ptr(arr, 1);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "beta");
    /* do NOT call return_string on g.u.value */
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 11: pop_back_str_array
// ================================================================================
 
static void test_str_pop_back_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_back_str_array(NULL), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_pop_back_empty_array_returns_empty(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(pop_back_str_array(arr), EMPTY);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_pop_back_removes_last_element(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "one"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "two"), NO_ERROR);
    assert_int_equal(pop_back_str_array(arr), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 1);
 
    string_expect_t g = get_str_array_index(arr, 0);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "one");
    return_string(g.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 12: pop_front_str_array
// ================================================================================
 
static void test_str_pop_front_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_str_array(NULL), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_pop_front_empty_array_returns_empty(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(pop_front_str_array(arr), EMPTY);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_pop_front_removes_first_element(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "alpha"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "beta"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "gamma"), NO_ERROR);
    assert_int_equal(pop_front_str_array(arr), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 2);
 
    string_expect_t g = get_str_array_index(arr, 0);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "beta");
    return_string(g.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 13: pop_any_str_array
// ================================================================================
 
static void test_str_pop_any_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_any_str_array(NULL, 0), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_pop_any_out_of_bounds_returns_error(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "x"), NO_ERROR);
    assert_int_equal(pop_any_str_array(arr, 5), OUT_OF_BOUNDS);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_pop_any_removes_middle_element(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "a"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "b"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "c"), NO_ERROR);
    assert_int_equal(pop_any_str_array(arr, 1), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 2);
 
    string_expect_t g0 = get_str_array_index(arr, 0);
    string_expect_t g1 = get_str_array_index(arr, 1);
    assert_true(g0.has_value && g1.has_value);
    assert_string_equal(const_string(g0.u.value), "a");
    assert_string_equal(const_string(g1.u.value), "c");
    return_string(g0.u.value);
    return_string(g1.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 14: clear_str_array
// ================================================================================
 
static void test_str_clear_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_str_array(NULL), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_clear_releases_elements_and_resets_len(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "x"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "y"), NO_ERROR);
    assert_int_equal(clear_str_array(arr), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 0);
 
    /* Buffer retained — can push again immediately. */
    assert_int_equal(push_back_lit(arr, "z"), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 1);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 15: set_str_array_index_str
// ================================================================================
 
static void test_str_set_index_str_null_array_returns_null_pointer(void** state) {
    (void)state;
    string_t* s = _make_str("x");
    assert_int_equal(set_str_array_index_str(NULL, 0, s), NULL_POINTER);
    return_string(s);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_set_index_str_out_of_bounds_returns_error(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_t*    s   = _make_str("x");
    assert_int_equal(set_str_array_index_str(arr, 0, s), OUT_OF_BOUNDS);
    return_string(s);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_set_index_str_replaces_value(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_t*    s   = _make_str("replacement");
    assert_int_equal(push_back_lit(arr, "original"), NO_ERROR);
    assert_int_equal(set_str_array_index_str(arr, 0, s), NO_ERROR);
 
    string_expect_t g = get_str_array_index(arr, 0);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "replacement");
    return_string(g.u.value);
 
    return_string(s);
    return_str_array(arr);
}
 
// ================================================================================
// Group 16: set_str_array_index_lit
// ================================================================================
 
static void test_str_set_index_lit_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_str_array_index_lit(NULL, 0, "x"), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_set_index_lit_out_of_bounds_returns_error(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(set_str_array_index_lit(arr, 0, "x"), OUT_OF_BOUNDS);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_set_index_lit_replaces_value(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "old"), NO_ERROR);
    assert_int_equal(set_str_array_index_lit(arr, 0, "new"), NO_ERROR);
 
    string_expect_t g = get_str_array_index(arr, 0);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "new");
    return_string(g.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 17: copy_str_array
// ================================================================================
 
static void test_str_copy_null_src_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    str_array_expect_t r = copy_str_array(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_copy_produces_independent_array(void** state) {
    (void)state;
    allocator_vtable_t a   = heap_allocator();
    str_array_t*       src = _make_array(4);
    assert_int_equal(push_back_lit(src, "apple"),  NO_ERROR);
    assert_int_equal(push_back_lit(src, "banana"), NO_ERROR);
 
    str_array_expect_t cr = copy_str_array(src, a);
    assert_true(cr.has_value);
    str_array_t* dst = cr.u.value;
 
    assert_int_equal((int)str_array_size(dst), 2);
 
    /* Modify the source — copy must be unaffected. */
    assert_int_equal(set_str_array_index_lit(src, 0, "cherry"), NO_ERROR);
 
    string_expect_t g = get_str_array_index(dst, 0);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "apple");
    return_string(g.u.value);
 
    return_str_array(src);
    return_str_array(dst);
}
 
// ================================================================================
// Group 18: reverse_str_array
// ================================================================================
 
static void test_str_reverse_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_str_array(NULL), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_reverse_single_element_is_no_op(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "only"), NO_ERROR);
    assert_int_equal(reverse_str_array(arr), NO_ERROR);
 
    string_expect_t g = get_str_array_index(arr, 0);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "only");
    return_string(g.u.value);
 
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_reverse_inverts_order(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "first"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "second"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "third"),  NO_ERROR);
    assert_int_equal(reverse_str_array(arr), NO_ERROR);
 
    string_expect_t g0 = get_str_array_index(arr, 0);
    string_expect_t g2 = get_str_array_index(arr, 2);
    assert_true(g0.has_value && g2.has_value);
    assert_string_equal(const_string(g0.u.value), "third");
    assert_string_equal(const_string(g2.u.value), "first");
    return_string(g0.u.value);
    return_string(g2.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 19: sort_str_array
// ================================================================================
 
static void test_str_sort_null_array_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(sort_str_array(NULL, FORWARD), NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_sort_forward_ascending(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "zebra"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "apple"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "mango"),  NO_ERROR);
    assert_int_equal(sort_str_array(arr, FORWARD), NO_ERROR);
 
    string_expect_t g0 = get_str_array_index(arr, 0);
    string_expect_t g2 = get_str_array_index(arr, 2);
    assert_true(g0.has_value && g2.has_value);
    assert_string_equal(const_string(g0.u.value), "apple");
    assert_string_equal(const_string(g2.u.value), "zebra");
    return_string(g0.u.value);
    return_string(g2.u.value);
 
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_sort_reverse_descending(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "apple"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "zebra"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "mango"),  NO_ERROR);
    assert_int_equal(sort_str_array(arr, REVERSE), NO_ERROR);
 
    string_expect_t g0 = get_str_array_index(arr, 0);
    string_expect_t g2 = get_str_array_index(arr, 2);
    assert_true(g0.has_value && g2.has_value);
    assert_string_equal(const_string(g0.u.value), "zebra");
    assert_string_equal(const_string(g2.u.value), "apple");
    return_string(g0.u.value);
    return_string(g2.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 20: str_array_contains_str
// ================================================================================
 
static void test_str_contains_str_null_array_returns_null_pointer(void** state) {
    (void)state;
    string_t*     s = _make_str("x");
    size_expect_t r = str_array_contains_str(NULL, s, 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_string(s);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_contains_str_finds_existing_value(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_t*    s   = _make_str("beta");
    assert_int_equal(push_back_lit(arr, "alpha"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "beta"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "gamma"), NO_ERROR);
 
    size_expect_t r = str_array_contains_str(arr, s, 0, 3);
    assert_true(r.has_value);
    assert_int_equal((int)r.u.value, 1);
 
    return_string(s);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_contains_str_not_found_returns_error(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    string_t*    s   = _make_str("delta");
    assert_int_equal(push_back_lit(arr, "alpha"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "beta"),  NO_ERROR);
 
    size_expect_t r = str_array_contains_str(arr, s, 0, 2);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NOT_FOUND);
 
    return_string(s);
    return_str_array(arr);
}
 
// ================================================================================
// Group 21: str_array_contains_lit
// ================================================================================
 
static void test_str_contains_lit_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = str_array_contains_lit(NULL, "x", 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_contains_lit_case_sensitive_no_match(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "Hello"), NO_ERROR);
 
    /* Lowercase "hello" must NOT match "Hello". */
    size_expect_t r = str_array_contains_lit(arr, "hello", 0, 1);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NOT_FOUND);
 
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_contains_lit_finds_value(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "one"),   NO_ERROR);
    assert_int_equal(push_back_lit(arr, "two"),   NO_ERROR);
    assert_int_equal(push_back_lit(arr, "three"), NO_ERROR);
 
    size_expect_t r = str_array_contains_lit(arr, "three", 0, 3);
    assert_true(r.has_value);
    assert_int_equal((int)r.u.value, 2);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 22: str_array_binary_search_str
// ================================================================================
 
static void test_str_bsearch_str_null_array_returns_null_pointer(void** state) {
    (void)state;
    string_t*     s = _make_str("x");
    size_expect_t r = str_array_binary_search_str(NULL, s, false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_string(s);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_bsearch_str_finds_value_with_sort(void** state) {
    (void)state;
    str_array_t* arr = _make_array(8);
    string_t*    s   = _make_str("mango");
    assert_int_equal(push_back_lit(arr, "zebra"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "apple"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "mango"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "banana"), NO_ERROR);
 
    size_expect_t r = str_array_binary_search_str(arr, s, true);
    assert_true(r.has_value);
 
    /* Confirm the element at the returned index is correct. */
    string_expect_t g = get_str_array_index(arr, r.u.value);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "mango");
    return_string(g.u.value);
 
    return_string(s);
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_bsearch_str_returns_lowest_index_for_duplicates(void** state) {
    (void)state;
    str_array_t* arr = _make_array(8);
    string_t*    s   = _make_str("beta");
    assert_int_equal(push_back_lit(arr, "alpha"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "beta"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "beta"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "gamma"), NO_ERROR);
 
    /* Array is already sorted — no sort needed. */
    size_expect_t r = str_array_binary_search_str(arr, s, false);
    assert_true(r.has_value);
    assert_int_equal((int)r.u.value, 1);
 
    return_string(s);
    return_str_array(arr);
}
 
// ================================================================================
// Group 23: str_array_binary_search_lit
// ================================================================================
 
static void test_str_bsearch_lit_null_array_returns_null_pointer(void** state) {
    (void)state;
    size_expect_t r = str_array_binary_search_lit(NULL, "x", false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_bsearch_lit_not_found_returns_error(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "alpha"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "beta"),  NO_ERROR);
 
    size_expect_t r = str_array_binary_search_lit(arr, "delta", false);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NOT_FOUND);
 
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_bsearch_lit_finds_with_sort(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal(push_back_lit(arr, "cherry"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "apple"),  NO_ERROR);
    assert_int_equal(push_back_lit(arr, "banana"), NO_ERROR);
 
    size_expect_t r = str_array_binary_search_lit(arr, "banana", true);
    assert_true(r.has_value);
 
    string_expect_t g = get_str_array_index(arr, r.u.value);
    assert_true(g.has_value);
    assert_string_equal(const_string(g.u.value), "banana");
    return_string(g.u.value);
 
    return_str_array(arr);
}
 
// ================================================================================
// Group 24: str_array_size
// ================================================================================
 
static void test_str_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)str_array_size(NULL), 0);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_size_reflects_push_count(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal((int)str_array_size(arr), 0);
    assert_int_equal(push_back_lit(arr, "a"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "b"), NO_ERROR);
    assert_int_equal((int)str_array_size(arr), 2);
    return_str_array(arr);
}
 
// ================================================================================
// Group 25: str_array_alloc
// ================================================================================
 
static void test_str_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)str_array_alloc(NULL), 0);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_alloc_matches_capacity(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    str_array_expect_t r = init_str_array(16, false, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_alloc(r.u.value), 16);
    return_str_array(r.u.value);
}
 
// ================================================================================
// Group 26: str_array_data_size
// ================================================================================
 
static void test_str_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)str_array_data_size(NULL), 0);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_data_size_is_pointer_size(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_int_equal((int)str_array_data_size(arr), (int)sizeof(string_t*));
    return_str_array(arr);
}
 
// ================================================================================
// Group 27: is_str_array_empty / is_str_array_full
// ================================================================================
 
static void test_str_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_str_array_empty(NULL));
}
 
// --------------------------------------------------------------------------------
 
static void test_str_empty_reflects_contents(void** state) {
    (void)state;
    str_array_t* arr = _make_array(4);
    assert_true(is_str_array_empty(arr));
    assert_int_equal(push_back_lit(arr, "x"), NO_ERROR);
    assert_false(is_str_array_empty(arr));
    return_str_array(arr);
}
 
// --------------------------------------------------------------------------------
 
static void test_str_full_null_returns_true(void** state) {
    (void)state;
    assert_true(is_str_array_full(NULL));
}
 
// --------------------------------------------------------------------------------
 
static void test_str_full_reflects_capacity(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    str_array_expect_t r = init_str_array(2, false, a);
    assert_true(r.has_value);
    str_array_t* arr = r.u.value;
 
    assert_false(is_str_array_full(arr));
    assert_int_equal(push_back_lit(arr, "a"), NO_ERROR);
    assert_int_equal(push_back_lit(arr, "b"), NO_ERROR);
    assert_true(is_str_array_full(arr));
 
    return_str_array(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Group 28: string_delim_array — macro dispatches to _lit for const char*
// ================================================================================
 
/* Basic split on two distinct delimiter characters */
static void test_delim_macro_lit_basic(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string("one,two;three", 0u, a);
    assert_true(rs.has_value);
 
    str_array_expect_t r = string_delim_array(rs.u.value, ",;", NULL, NULL, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 3);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    string_expect_t e2 = get_str_array_index(r.u.value, 2u);
    assert_string_equal(const_string(e0.u.value), "one");
    assert_string_equal(const_string(e1.u.value), "two");
    assert_string_equal(const_string(e2.u.value), "three");
    return_string(e0.u.value); return_string(e1.u.value); return_string(e2.u.value);
    return_str_array(r.u.value);
    return_string(rs.u.value);
}
 
/* Single delimiter character — same behaviour as strtok with one separator */
static void test_delim_macro_lit_single_char(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string("a:b:c", 0u, a);
    assert_true(rs.has_value);
 
    str_array_expect_t r = string_delim_array(rs.u.value, ":", NULL, NULL, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 3);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    string_expect_t e2 = get_str_array_index(r.u.value, 2u);
    assert_string_equal(const_string(e0.u.value), "a");
    assert_string_equal(const_string(e1.u.value), "b");
    assert_string_equal(const_string(e2.u.value), "c");
    return_string(e0.u.value); return_string(e1.u.value); return_string(e2.u.value);
    return_str_array(r.u.value);
    return_string(rs.u.value);
}
 
/* Consecutive delimiters produce an empty string between them */
static void test_delim_macro_lit_consecutive_delimiters(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string("a,,b", 0u, a);
    assert_true(rs.has_value);
 
    str_array_expect_t r = string_delim_array(rs.u.value, ",", NULL, NULL, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 3);

    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    assert_string_equal(const_string(e1.u.value), "");
    return_string(e1.u.value);
    return_str_array(r.u.value);
    return_string(rs.u.value);
}
 
/* Leading delimiter produces empty string as first element */
static void test_delim_macro_lit_leading_delimiter(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string(",hello", 0u, a);
    assert_true(rs.has_value);
 
    str_array_expect_t r = string_delim_array(rs.u.value, ",", NULL, NULL, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 2);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    assert_string_equal(const_string(e0.u.value), "");
    assert_string_equal(const_string(e1.u.value), "hello");
    return_string(e0.u.value); return_string(e1.u.value);
    return_str_array(r.u.value);
    return_string(rs.u.value);
}
 
/* Trailing delimiter produces empty string as last element */
static void test_delim_macro_lit_trailing_delimiter(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string("hello,", 0u, a);
    assert_true(rs.has_value);
 
    str_array_expect_t r = string_delim_array(rs.u.value, ",", NULL, NULL, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 2);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    assert_string_equal(const_string(e0.u.value), "hello");
    assert_string_equal(const_string(e1.u.value), "");
    return_string(e0.u.value); return_string(e1.u.value);
    return_str_array(r.u.value);
    return_string(rs.u.value);
}
 
/* No delimiter found — entire string returned as single element */
static void test_delim_macro_lit_no_match(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string("noddelimiters", 0u, a);
    assert_true(rs.has_value);
 
    str_array_expect_t r = string_delim_array(rs.u.value, ",;", NULL, NULL, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 1);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    assert_string_equal(const_string(e0.u.value), "noddelimiters");
    return_string(e0.u.value);
    return_str_array(r.u.value);
    return_string(rs.u.value);
}
 
/* NULL source returns NULL_POINTER error */
static void test_delim_macro_lit_null_source_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    str_array_expect_t r = string_delim_array((string_t*)NULL, ",", NULL, NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
/* Empty delimiter set returns INVALID_ARG */
static void test_delim_macro_lit_empty_set_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string("hello", 0u, a);
    assert_true(rs.has_value);
    str_array_expect_t r = string_delim_array(rs.u.value, "", NULL, NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
    return_string(rs.u.value);
}
 
// ================================================================================
// Group 29: string_delim_array — macro dispatches to _str for string_t*
// ================================================================================
 
/* Basic split using string_t delimiter set */
static void test_delim_macro_str_basic(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs  = init_string("x:y-z", 0u, a);
    string_expect_t rds = init_string(":-", 0u, a);
    assert_true(rs.has_value); assert_true(rds.has_value);
 
    str_array_expect_t r = string_delim_array(rs.u.value, rds.u.value, NULL, NULL, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 3);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    string_expect_t e2 = get_str_array_index(r.u.value, 2u);
    assert_string_equal(const_string(e0.u.value), "x");
    assert_string_equal(const_string(e1.u.value), "y");
    assert_string_equal(const_string(e2.u.value), "z");
    return_string(e0.u.value); return_string(e1.u.value); return_string(e2.u.value);
    return_str_array(r.u.value);
    return_string(rds.u.value); return_string(rs.u.value);
}
 
/* Results are independent deep copies — mutating source does not affect array */
static void test_delim_macro_str_elements_are_deep_copies(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs  = init_string("hello,world", 0u, a);
    string_expect_t rds = init_string(",", 0u, a);
    assert_true(rs.has_value); assert_true(rds.has_value);
 
    str_array_expect_t r = string_delim_array(rs.u.value, rds.u.value, NULL, NULL, a);
    assert_true(r.has_value);
 
    /* Overwrite source string buffer */
    memset(rs.u.value->str, 'X', rs.u.value->len);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    assert_string_equal(const_string(e0.u.value), "hello");
    assert_string_equal(const_string(e1.u.value), "world");
    return_string(e0.u.value); return_string(e1.u.value);
    return_str_array(r.u.value);
    return_string(rds.u.value); return_string(rs.u.value);
}
 
/* NULL string_t delimiter returns NULL_POINTER */
static void test_delim_macro_str_null_delim_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string("hello", 0u, a);
    assert_true(rs.has_value);
    str_array_expect_t r = string_delim_array(rs.u.value, (string_t*)NULL,
                                              NULL, NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_string(rs.u.value);
}
 
// ================================================================================
// Group 30: string_delim_array — windowed
// ================================================================================
 
/* begin window trims the left side */
static void test_delim_macro_windowed_begin(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    /* "SKIP,one,two" — start window 5 bytes in, past "SKIP," */
    string_expect_t rs = init_string("SKIP,one,two", 0u, a);
    assert_true(rs.has_value);
    const uint8_t* begin = (const uint8_t*)rs.u.value->str + 5u;
 
    str_array_expect_t r = string_delim_array(rs.u.value, ",", begin, NULL, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 2);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    assert_string_equal(const_string(e0.u.value), "one");
    assert_string_equal(const_string(e1.u.value), "two");
    return_string(e0.u.value); return_string(e1.u.value);
    return_str_array(r.u.value);
    return_string(rs.u.value);
}
 
/* end window trims the right side */
static void test_delim_macro_windowed_end(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    /* "one,two,SKIP" — end window 7 bytes in, before ",SKIP" */
    string_expect_t rs = init_string("one,two,SKIP", 0u, a);
    assert_true(rs.has_value);
    const uint8_t* end = (const uint8_t*)rs.u.value->str + 7u;
 
    str_array_expect_t r = string_delim_array(rs.u.value, ",", NULL, end, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 2);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    assert_string_equal(const_string(e0.u.value), "one");
    assert_string_equal(const_string(e1.u.value), "two");
    return_string(e0.u.value); return_string(e1.u.value);
    return_str_array(r.u.value);
    return_string(rs.u.value);
}
 
/* begin and end together isolate a middle segment */
static void test_delim_macro_windowed_both(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    /* "SKIP,one,two,SKIP" — window [5, 12): "one,two" */
    string_expect_t rs = init_string("SKIP,one,two,SKIP", 0u, a);
    assert_true(rs.has_value);
    const uint8_t* begin = (const uint8_t*)rs.u.value->str + 5u;
    const uint8_t* end   = (const uint8_t*)rs.u.value->str + 12u;
 
    str_array_expect_t r = string_delim_array(rs.u.value, ",", begin, end, a);
    assert_true(r.has_value);
    assert_int_equal((int)str_array_size(r.u.value), 2);
 
    string_expect_t e0 = get_str_array_index(r.u.value, 0u);
    string_expect_t e1 = get_str_array_index(r.u.value, 1u);
    assert_string_equal(const_string(e0.u.value), "one");
    assert_string_equal(const_string(e1.u.value), "two");
    return_string(e0.u.value); return_string(e1.u.value);
    return_str_array(r.u.value);
    return_string(rs.u.value);
}
 
/* begin > end is an invalid window — returns INVALID_ARG */
static void test_delim_macro_windowed_invalid_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string("hello", 0u, a);
    assert_true(rs.has_value);
    const uint8_t* b = (const uint8_t*)rs.u.value->str + 4u;
    const uint8_t* e = (const uint8_t*)rs.u.value->str + 1u;
    str_array_expect_t r = string_delim_array(rs.u.value, ",", b, e, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
    return_string(rs.u.value);
}
 
/* Pointer outside string bounds — returns INVALID_ARG */
static void test_delim_macro_windowed_out_of_bounds_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    string_expect_t rs = init_string("hello", 0u, a);
    assert_true(rs.has_value);
    /* Point one byte before the string buffer */
    const uint8_t* b = (const uint8_t*)rs.u.value->str - 1u;
    str_array_expect_t r = string_delim_array(rs.u.value, ",", b, NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
    return_string(rs.u.value);
}
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_string_array[] = {

    /* Group 1: init_str_array */
    cmocka_unit_test(test_str_init_null_allocate_fn_fails),
    cmocka_unit_test(test_str_init_returns_valid_array),

    /* Group 2: return_str_array */
    cmocka_unit_test(test_str_return_null_is_safe),
    cmocka_unit_test(test_str_return_releases_owned_strings),

    /* Group 3: push_back_str */
    cmocka_unit_test(test_str_push_back_str_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_push_back_str_null_src_returns_null_pointer),
    cmocka_unit_test(test_str_push_back_str_deep_copy_independent),

    /* Group 4: push_front_str */
    cmocka_unit_test(test_str_push_front_str_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_push_front_str_prepends_correctly),

    /* Group 5: push_at_str */
    cmocka_unit_test(test_str_push_at_str_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_push_at_str_out_of_bounds_returns_error),
    cmocka_unit_test(test_str_push_at_str_inserts_at_correct_position),

    /* Group 6: push_back_lit */
    cmocka_unit_test(test_str_push_back_lit_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_push_back_lit_null_lit_returns_null_pointer),
    cmocka_unit_test(test_str_push_back_lit_stores_correct_value),

    /* Group 7: push_front_lit */
    cmocka_unit_test(test_str_push_front_lit_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_push_front_lit_prepends_correctly),

    /* Group 8: push_at_lit */
    cmocka_unit_test(test_str_push_at_lit_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_push_at_lit_out_of_bounds_returns_error),
    cmocka_unit_test(test_str_push_at_lit_inserts_at_correct_position),

    /* Group 9: get_str_array_index */
    cmocka_unit_test(test_str_get_index_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_get_index_out_of_bounds_returns_error),
    cmocka_unit_test(test_str_get_index_returns_independent_copy),

    /* Group 10: get_str_array_ptr */
    cmocka_unit_test(test_str_get_ptr_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_get_ptr_out_of_bounds_returns_error),
    cmocka_unit_test(test_str_get_ptr_returns_correct_value_without_copy),

    /* Group 11: pop_back_str_array */
    cmocka_unit_test(test_str_pop_back_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_pop_back_empty_array_returns_empty),
    cmocka_unit_test(test_str_pop_back_removes_last_element),

    /* Group 12: pop_front_str_array */
    cmocka_unit_test(test_str_pop_front_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_pop_front_empty_array_returns_empty),
    cmocka_unit_test(test_str_pop_front_removes_first_element),

    /* Group 13: pop_any_str_array */
    cmocka_unit_test(test_str_pop_any_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_pop_any_out_of_bounds_returns_error),
    cmocka_unit_test(test_str_pop_any_removes_middle_element),

    /* Group 14: clear_str_array */
    cmocka_unit_test(test_str_clear_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_clear_releases_elements_and_resets_len),

    /* Group 15: set_str_array_index_str */
    cmocka_unit_test(test_str_set_index_str_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_set_index_str_out_of_bounds_returns_error),
    cmocka_unit_test(test_str_set_index_str_replaces_value),

    /* Group 16: set_str_array_index_lit */
    cmocka_unit_test(test_str_set_index_lit_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_set_index_lit_out_of_bounds_returns_error),
    cmocka_unit_test(test_str_set_index_lit_replaces_value),

    /* Group 17: copy_str_array */
    cmocka_unit_test(test_str_copy_null_src_returns_null_pointer),
    cmocka_unit_test(test_str_copy_produces_independent_array),

    /* Group 18: reverse_str_array */
    cmocka_unit_test(test_str_reverse_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_reverse_single_element_is_no_op),
    cmocka_unit_test(test_str_reverse_inverts_order),

    /* Group 19: sort_str_array */
    cmocka_unit_test(test_str_sort_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_sort_forward_ascending),
    cmocka_unit_test(test_str_sort_reverse_descending),

    /* Group 20: str_array_contains_str */
    cmocka_unit_test(test_str_contains_str_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_contains_str_finds_existing_value),
    cmocka_unit_test(test_str_contains_str_not_found_returns_error),

    /* Group 21: str_array_contains_lit */
    cmocka_unit_test(test_str_contains_lit_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_contains_lit_case_sensitive_no_match),
    cmocka_unit_test(test_str_contains_lit_finds_value),

    /* Group 22: str_array_binary_search_str */
    cmocka_unit_test(test_str_bsearch_str_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_bsearch_str_finds_value_with_sort),
    cmocka_unit_test(test_str_bsearch_str_returns_lowest_index_for_duplicates),

    /* Group 23: str_array_binary_search_lit */
    cmocka_unit_test(test_str_bsearch_lit_null_array_returns_null_pointer),
    cmocka_unit_test(test_str_bsearch_lit_not_found_returns_error),
    cmocka_unit_test(test_str_bsearch_lit_finds_with_sort),

    /* Group 24: str_array_size */
    cmocka_unit_test(test_str_size_null_returns_zero),
    cmocka_unit_test(test_str_size_reflects_push_count),

    /* Group 25: str_array_alloc */
    cmocka_unit_test(test_str_alloc_null_returns_zero),
    cmocka_unit_test(test_str_alloc_matches_capacity),

    /* Group 26: str_array_data_size */
    cmocka_unit_test(test_str_data_size_null_returns_zero),
    cmocka_unit_test(test_str_data_size_is_pointer_size),

    /* Group 27: is_str_array_empty / is_str_array_full */
    cmocka_unit_test(test_str_empty_null_returns_true),
    cmocka_unit_test(test_str_empty_reflects_contents),
    cmocka_unit_test(test_str_full_null_returns_true),
    cmocka_unit_test(test_str_full_reflects_capacity),

    /* Group 28: string_delim_array macro — _lit dispatch */
    cmocka_unit_test(test_delim_macro_lit_basic),
    cmocka_unit_test(test_delim_macro_lit_single_char),
    cmocka_unit_test(test_delim_macro_lit_consecutive_delimiters),
    cmocka_unit_test(test_delim_macro_lit_leading_delimiter),
    cmocka_unit_test(test_delim_macro_lit_trailing_delimiter),
    cmocka_unit_test(test_delim_macro_lit_no_match),
    cmocka_unit_test(test_delim_macro_lit_null_source_fails),
    cmocka_unit_test(test_delim_macro_lit_empty_set_fails),

    /* Group 29: string_delim_array macro — _str dispatch */
    cmocka_unit_test(test_delim_macro_str_basic),
    cmocka_unit_test(test_delim_macro_str_elements_are_deep_copies),
    cmocka_unit_test(test_delim_macro_str_null_delim_fails),

    /* Group 30: string_delim_array macro — windowed */
    cmocka_unit_test(test_delim_macro_windowed_begin),
    cmocka_unit_test(test_delim_macro_windowed_end),
    cmocka_unit_test(test_delim_macro_windowed_both),
    cmocka_unit_test(test_delim_macro_windowed_invalid_fails),
    cmocka_unit_test(test_delim_macro_windowed_out_of_bounds_fails),
};
const size_t test_string_array_count = sizeof(test_string_array) / sizeof(test_string_array[0]);
// ================================================================================
// ================================================================================
// eof
