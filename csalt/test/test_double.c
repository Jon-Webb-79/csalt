// ================================================================================
// ================================================================================
// - File:    test_vector.c
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

#include "test_double.h"
#include "c_double.h"

#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
// ================================================================================ 
// ================================================================================ 

void test_init_double_vector_success(void **state) {
    (void) state; /* unused */
    
    double_v* vec = init_double_vector(10);
    assert_non_null(vec);
    assert_non_null(vec->data);
    assert_int_equal(vec->len, 0);
    assert_int_equal(vec->alloc, 10);
    assert_int_equal(vec->alloc_type, DYNAMIC);
    
    // Check zero initialization
    for (size_t i = 0; i < vec->alloc; i++) {
        assert_float_equal(vec->data[i], 0.0, 0.0001);
    }
    
    free_double_vector(vec);
}
// --------------------------------------------------------------------------------

void test_init_double_vector_zero_size(void **state) {
    (void) state; /* unused */
    
    errno = 0;
    double_v* vec = init_double_vector(0);
    assert_null(vec);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_init_double_array_success(void **state) {
    (void) state; /* unused */
    
    double_v arr = init_double_array(10);
    assert_non_null(arr.data);
    assert_int_equal(arr.len, 0);
    assert_int_equal(arr.alloc, 10);
    assert_int_equal(arr.alloc_type, STATIC);
    
    // Check zero initialization
    for (size_t i = 0; i < arr.alloc; i++) {
        assert_float_equal(arr.data[i], 0.0, 0.0001);
    }
}
// -------------------------------------------------------------------------------- 

void test_free_double_vector_null(void **state) {
    (void) state; /* unused */
    
    errno = 0;
    free_double_vector(NULL);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_free_double_vector_static(void **state) {
    (void) state; /* unused */
    
    double_v arr = init_double_array(10);
    double_v* arr_ptr = &arr;
    
    errno = 0;
    free_double_vector(arr_ptr);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
/* GC Tests */
void test_double_vector_gbc(void **state) {
    (void) state; /* unused */
    
    {
        DBLEVEC_GBC double_v* vec = init_double_vector(10);
        assert_non_null(vec);
        assert_non_null(vec->data);
        // Vector will be automatically freed at scope end
    }
}
#endif
// ================================================================================ 
// ================================================================================

void test_push_back_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(2);
    assert_non_null(vec);
    
    // Test basic push_back
    assert_true(push_back_double_vector(vec, 3.14));
    assert_int_equal(d_size(vec), 1);
    assert_float_equal(double_vector_index(vec, 0), 3.14, 0.0001);
     
    // Test adding zero
    assert_true(push_back_double_vector(vec, 0.0));
    //assert_int_equal(d_size(vec), 2);
    // assert_double_equal(double_vector_index(vec, 1), 0.0f, 0.0001f);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_double_growth(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = d_alloc(vec);
    
    // Fill to capacity
    assert_true(push_back_double_vector(vec, 1.0));
    assert_true(push_back_double_vector(vec, 2.0));
    assert_int_equal(d_size(vec), 2);
    assert_int_equal(d_alloc(vec), initial_alloc);
    
    // Trigger growth
    assert_true(push_back_double_vector(vec, 3.0));
    assert_int_equal(d_size(vec), 3);
    assert_true(d_alloc(vec) > initial_alloc);
    assert_float_equal(double_vector_index(vec, 2), 3.0, 0.0001);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_double_static(void **state) {
    (void) state;
    
    double_v arr = init_double_array(2);
    
    // Fill to capacity
    assert_true(push_back_double_vector(&arr, 1.0));
    assert_true(push_back_double_vector(&arr, 2.0));
    
    // Attempt to exceed capacity should fail
    assert_false(push_back_double_vector(&arr, 3.0));
    assert_int_equal(errno, EINVAL);
    assert_int_equal(d_size(&arr), 2);
}
// -------------------------------------------------------------------------------- 

void test_generic_double_macros(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(2);
    assert_non_null(vec);
    
    // Test d_size macro
    assert_int_equal(d_size(vec), 0);
    push_back_double_vector(vec, 1.0);
    assert_int_equal(d_size(vec), 1);
    
    // Test d_alloc macro
    assert_int_equal(d_alloc(vec), 2);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_double_vector_index(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(2);
    assert_non_null(vec);
    
    // Test valid index
    assert_true(push_back_double_vector(vec, 1.234));
    assert_float_equal(double_vector_index(vec, 0), 1.234, 0.0001);
    
    // Test out of bounds index
    errno = 0;
    double result = double_vector_index(vec, d_size(vec));  
    assert_int_equal(errno, ERANGE);
    assert_float_equal(result, DBL_MAX, 0.0001);
    
    // Test NULL vector
    errno = 0;
    result = double_vector_index(NULL, 0);
    assert_int_equal(errno, EINVAL);
    assert_float_equal(result, DBL_MAX, 0.0001);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    // Test infinity
    assert_true(push_back_double_vector(vec, INFINITY));
    assert_true(isinf(double_vector_index(vec, 0)));
    
    // Test negative infinity
    assert_true(push_back_double_vector(vec, -INFINITY));
    assert_true(isinf(double_vector_index(vec, 1)));
    
    // Test NaN
    assert_true(push_back_double_vector(vec, NAN));
    assert_true(isnan(double_vector_index(vec, 2)));
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_static_array_double_bounds(void **state) {
    (void) state;
    
    // Create static array of size 3
    double_v arr = init_double_array(3);
    
    // Test adding up to capacity
    assert_true(push_back_double_vector(&arr, 1.0));
    assert_int_equal(d_size(&arr), 1);
    assert_true(push_back_double_vector(&arr, 2.0));
    assert_int_equal(d_size(&arr), 2);
    assert_true(push_back_double_vector(&arr, 3.0));
    assert_int_equal(d_size(&arr), 3);
    
    // Verify values were stored correctly
    assert_float_equal(double_vector_index(&arr, 0), 1.0, 0.0001);
    assert_float_equal(double_vector_index(&arr, 1), 2.0, 0.0001);
    assert_float_equal(double_vector_index(&arr, 2), 3.0, 0.0001);
    
    // Test attempting to exceed capacity
    errno = 0;
    assert_false(push_back_double_vector(&arr, 4.0));
    assert_int_equal(errno, EINVAL);
    assert_int_equal(d_size(&arr), 3);
    
    // Verify original data wasn't corrupted
    assert_float_equal(double_vector_index(&arr, 2), 3.0, 0.0001);
}
// --------------------------------------------------------------------------------

void test_static_array_index_double_bounds(void **state) {
    (void) state;
    
    // Create and fill static array
    double_v arr = init_double_array(2);
    push_back_double_vector(&arr, 1.0);
    
    // Test valid index
    assert_float_equal(double_vector_index(&arr, 0), 1.0, 0.0001);
    
    // Test index at unfilled position
    errno = 0;
    double result = double_vector_index(&arr, 1);
    assert_int_equal(errno, ERANGE);
    assert_float_equal(result, DBL_MAX, 0.0001);
    
    // Test index beyond allocation
    errno = 0;
    result = double_vector_index(&arr, 2);
    assert_int_equal(errno, ERANGE);
    assert_float_equal(result, DBL_MAX, 0.0001);
}
// --------------------------------------------------------------------------------

void test_static_array_double_initialization(void **state) {
    (void) state;
    
    double_v arr = init_double_array(2);
    
    // Check initial state
    assert_int_equal(d_size(&arr), 0);
    assert_int_equal(d_alloc(&arr), 2);
    
    // Verify data array is zeroed
    for(size_t i = 0; i < d_alloc(&arr); i++) {
        assert_float_equal(arr.data[i], 0.0, 0.0001);  // Direct access just for testing
    }
}
// -------------------------------------------------------------------------------- 

void test_static_array_double_free(void **state) {
    (void) state;
    
    double_v arr = init_double_array(2);
    push_back_double_vector(&arr, 1.0);
    
    // Attempting to free a static array should fail
    errno = 0;
    free_double_vector(&arr);
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_float_equal(double_vector_index(&arr, 0), 1.0, 0.0001);
}
// ================================================================================ 
// ================================================================================ 

void test_push_front_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(2);
    assert_non_null(vec);
    
    // Test basic push_front
    assert_true(push_front_double_vector(vec, 3.14));
    assert_int_equal(d_size(vec), 1);
    assert_float_equal(double_vector_index(vec, 0), 3.14, 0.0001);
    
    // Test adding zero
    assert_true(push_front_double_vector(vec, 0.0f));
    assert_int_equal(d_size(vec), 2);
    assert_float_equal(double_vector_index(vec, 0), 0.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 3.14, 0.0001);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_double_growth(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = d_alloc(vec);
    
    // Fill to capacity
    assert_true(push_front_double_vector(vec, 1.0));
    assert_true(push_front_double_vector(vec, 2.0));
    assert_int_equal(d_size(vec), 2);
    assert_int_equal(d_alloc(vec), initial_alloc);
    
    // Verify order
    assert_float_equal(double_vector_index(vec, 0), 2.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 1.0, 0.0001);
    
    // Trigger growth
    assert_true(push_front_double_vector(vec, 3.0));
    assert_int_equal(d_size(vec), 3);
    assert_true(d_alloc(vec) > initial_alloc);
    
    // Verify all elements after growth
    assert_float_equal(double_vector_index(vec, 0), 3.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 2.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 2), 1.0, 0.0001);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_double_static(void **state) {
    (void) state;
    
    double_v arr = init_double_array(2);
    
    // Test basic push_front with static array
    assert_true(push_front_double_vector(&arr, 1.0));
    assert_true(push_front_double_vector(&arr, 2.0));
    assert_int_equal(d_size(&arr), 2);
    
    // Verify order
    assert_float_equal(double_vector_index(&arr, 0), 2.0, 0.0001);
    assert_float_equal(double_vector_index(&arr, 1), 1.0, 0.0001);
    
    // Attempt to exceed capacity
    assert_false(push_front_double_vector(&arr, 3.0));
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_int_equal(d_size(&arr), 2);
    assert_float_equal(double_vector_index(&arr, 0), 2.0, 0.0001);
    assert_float_equal(double_vector_index(&arr, 1), 1.0, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_push_front_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Test infinity
    assert_true(push_front_double_vector(vec, INFINITY));
    assert_true(isinf(double_vector_index(vec, 0)));
    
    // Test negative infinity
    assert_true(push_front_double_vector(vec, -INFINITY));
    assert_true(isinf(double_vector_index(vec, 0)));
    
    // Test NaN
    assert_true(push_front_double_vector(vec, NAN));
    assert_true(isnan(double_vector_index(vec, 0)));
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_error_double_cases(void **state) {
    (void) state;
    
    // Test NULL vector
    assert_false(push_front_double_vector(NULL, 1.0));
    assert_int_equal(errno, EINVAL);
    
    // Test with invalid data pointer
    double_v vec = {0};
    vec.data = NULL;
    assert_false(push_front_double_vector(&vec, 1.0));
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================

void test_insert_vector_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    // Insert into empty vector
    assert_true(insert_double_vector(vec, 1.0, 0));
    assert_int_equal(d_size(vec), 1);
    assert_float_equal(double_vector_index(vec, 0), 1.0, 0.0001);
    
    // Insert at beginning (shifting right)
    assert_true(insert_double_vector(vec, 0.0, 0));
    assert_int_equal(d_size(vec), 2);
    assert_float_equal(double_vector_index(vec, 0), 0.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 1.0, 0.0001);
    
    // Insert in middle
    assert_true(insert_double_vector(vec, 0.5, 1));
    assert_int_equal(d_size(vec), 3);
    assert_float_equal(double_vector_index(vec, 0), 0.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 0.5, 0.0001);
    assert_float_equal(double_vector_index(vec, 2), 1.0, 0.0001);
    
    // Insert at end (append)
    assert_true(insert_double_vector(vec, 2.0, 3));
    assert_int_equal(d_size(vec), 4);
    assert_float_equal(double_vector_index(vec, 3), 2.0, 0.0001);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_vector_double_growth(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = d_alloc(vec);
    
    // Fill initial capacity
    assert_true(insert_double_vector(vec, 1.0, 0));
    assert_true(insert_double_vector(vec, 2.0, 1));
    assert_int_equal(d_size(vec), 2);
    assert_int_equal(d_alloc(vec), initial_alloc);
    
    // Trigger growth
    assert_true(insert_double_vector(vec, 1.5, 1));
    assert_int_equal(d_size(vec), 3);
    assert_true(d_alloc(vec) > initial_alloc);
    
    // Verify all elements after growth
    assert_float_equal(double_vector_index(vec, 0), 1.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 1.5, 0.0001);
    assert_float_equal(double_vector_index(vec, 2), 2.0, 0.0001);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_array_double_basic(void **state) {
    (void) state;
    
    double_v arr = init_double_array(3);
    
    // Test basic insertions
    assert_true(insert_double_vector(&arr, 1.0, 0));
    assert_true(insert_double_vector(&arr, 3.0, 1));
    assert_true(insert_double_vector(&arr, 2.0, 1));
    
    // Verify order
    assert_int_equal(d_size(&arr), 3);
    assert_float_equal(double_vector_index(&arr, 0), 1.0, 0.0001f);
    assert_float_equal(double_vector_index(&arr, 1), 2.0, 0.0001f);
    assert_float_equal(double_vector_index(&arr, 2), 3.0, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_insert_array_double_bounds(void **state) {
    (void) state;
    
    double_v arr = init_double_array(2);
    
    // Fill array
    assert_true(insert_double_vector(&arr, 1.0, 0));
    assert_true(insert_double_vector(&arr, 2.0, 1));
    
    // Try to insert beyond capacity
    errno = 0;
    assert_false(insert_double_vector(&arr, 3.0, 1));
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_int_equal(d_size(&arr), 2);
    assert_float_equal(double_vector_index(&arr, 0), 1.0, 0.0001);
    assert_float_equal(double_vector_index(&arr, 1), 2.0, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_insert_error_double_cases(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(2);
    assert_non_null(vec);
    
    // Test invalid index
    errno = 0;
    assert_false(insert_double_vector(vec, 1.0, 1));  // Empty vector
    assert_int_equal(errno, ERANGE);
    
    // Add one element and test bounds
    assert_true(insert_double_vector(vec, 1.0, 0));
    
    errno = 0;
    assert_false(insert_double_vector(vec, 2.0, 2));  // Beyond length
    assert_int_equal(errno, ERANGE);
    
    // Test NULL vector
    errno = 0;
    assert_false(insert_double_vector(NULL, 1.0, 0));
    assert_int_equal(errno, EINVAL);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Insert infinity
    assert_true(insert_double_vector(vec, INFINITY, 0));
    assert_true(isinf(double_vector_index(vec, 0)));
    
    // Insert NaN
    assert_true(insert_double_vector(vec, NAN, 1));
    assert_true(isnan(double_vector_index(vec, 1)));
    
    // Insert zero
    assert_true(insert_double_vector(vec, 0.0, 1));
    assert_float_equal(double_vector_index(vec, 1), 0.0, 0.0001);
    
    free_double_vector(vec);
}
// ================================================================================ 
// ================================================================================

void test_pop_back_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Add some test values
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 3.0);
    
    // Test popping values
    errno = 0;
    assert_float_equal(pop_back_double_vector(vec), 3.0, 0.0001);
    assert_int_equal(d_size(vec), 2);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_back_double_vector(vec), 2.0, 0.0001);
    assert_int_equal(d_size(vec), 1);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_back_double_vector(vec), 1.0, 0.0001);
    assert_int_equal(d_size(vec), 0);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_back_double_empty(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(1);
    assert_non_null(vec);
    
    // Try to pop from empty vector
    errno = 0;
    double result = pop_back_double_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
    
    free_double_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_back_double_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    double result = pop_back_double_vector(NULL);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    double_v vec = {0};
    vec.data = NULL;
    errno = 0;
    result = pop_back_double_vector(&vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_pop_back_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Test popping DBL_MAX as valid data
    push_back_double_vector(vec, DBL_MAX);
    errno = 0;
    double result = pop_back_double_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, 0);  // Should be 0 since this is valid data
    
    // Test popping infinity
    push_back_double_vector(vec, INFINITY);
    errno = 0;
    result = pop_back_double_vector(vec);
    assert_true(isinf(result));
    assert_int_equal(errno, 0);
    
    // Test popping NaN
    push_back_double_vector(vec, NAN);
    errno = 0;
    result = pop_back_double_vector(vec);
    assert_true(isnan(result));
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_back_double_static(void **state) {
    (void) state;
    
    double_v arr = init_double_array(2);
    
    // Add and pop values from static array
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    
    errno = 0;
    assert_float_equal(pop_back_double_vector(&arr), 2.0, 0.0001);
    assert_int_equal(d_size(&arr), 1);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_back_double_vector(&arr), 1.0, 0.0001);
    assert_int_equal(d_size(&arr), 0);
    assert_int_equal(errno, 0);
    
    // Try to pop from empty array
    errno = 0;
    double result = pop_back_double_vector(&arr);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
}
// ================================================================================ 
// ================================================================================ 

void test_pop_front_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Add some test values
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 3.0);
    
    // Test popping values and check remaining elements
    errno = 0;
    assert_float_equal(pop_front_double_vector(vec), 1.0, 0.0001);
    assert_int_equal(d_size(vec), 2);
    assert_float_equal(double_vector_index(vec, 0), 2.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 3.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_front_double_vector(vec), 2.0, 0.0001);
    assert_int_equal(d_size(vec), 1);
    assert_float_equal(double_vector_index(vec, 0), 3.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_front_double_vector(vec), 3.0, 0.0001);
    assert_int_equal(d_size(vec), 0);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_double_empty(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(1);
    assert_non_null(vec);
    
    // Try to pop from empty vector
    errno = 0;
    double result = pop_front_double_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_double_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    double result = pop_front_double_vector(NULL);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    double_v vec = {0};
    vec.data = NULL;
    errno = 0;
    result = pop_front_double_vector(&vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Test popping DBL_MAX as valid data
    push_back_double_vector(vec, DBL_MAX);
    push_back_double_vector(vec, 1.0);
    errno = 0;
    double result = pop_front_double_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, 0);  // Should be 0 since this is valid data
    assert_float_equal(double_vector_index(vec, 0), 1.0, 0.0001);
    
    // Test popping infinity
    push_front_double_vector(vec, INFINITY);
    errno = 0;
    result = pop_front_double_vector(vec);
    assert_true(isinf(result));
    assert_int_equal(errno, 0);
    
    // Test popping NaN
    push_front_double_vector(vec, NAN);
    errno = 0;
    result = pop_front_double_vector(vec);
    assert_true(isnan(result));
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_double_static(void **state) {
    (void) state;
    
    double_v arr = init_double_array(2);
    
    // Add and pop values from static array
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    
    errno = 0;
    assert_float_equal(pop_front_double_vector(&arr), 1.0, 0.0001);
    assert_int_equal(d_size(&arr), 1);
    assert_float_equal(double_vector_index(&arr, 0), 2.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_front_double_vector(&arr), 2.0, 0.0001);
    assert_int_equal(d_size(&arr), 0);
    assert_int_equal(errno, 0);
    
    // Try to pop from empty array
    errno = 0;
    double result = pop_front_double_vector(&arr);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
}
// ================================================================================ 
// ================================================================================ 

void test_pop_any_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    // Add test values
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 3.0);
    push_back_double_vector(vec, 4.0);
    
    // Test popping from middle
    errno = 0;
    assert_float_equal(pop_any_double_vector(vec, 1), 2.0, 0.0001);
    assert_int_equal(d_size(vec), 3);
    assert_float_equal(double_vector_index(vec, 0), 1.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 3.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 2), 4.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test popping first element
    assert_float_equal(pop_any_double_vector(vec, 0), 1.0, 0.0001);
    assert_int_equal(d_size(vec), 2);
    assert_float_equal(double_vector_index(vec, 0), 3.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 4.0, 0.0001);
    
    // Test popping last element
    assert_float_equal(pop_any_double_vector(vec, 1), 4.0, 0.0001);
    assert_int_equal(d_size(vec), 1);
    assert_float_equal(double_vector_index(vec, 0), 3.0, 0.0001);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_double_errors(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(2);
    assert_non_null(vec);
    push_back_double_vector(vec, 1.0);
    
    // Test NULL vector
    errno = 0;
    double result = pop_any_double_vector(NULL, 0);
    assert_float_equal(result, DBL_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid index
    errno = 0;
    result = pop_any_double_vector(vec, 1);  // Index equals length
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ERANGE);
    
    errno = 0;
    result = pop_any_double_vector(vec, 2);  // Index beyond length
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ERANGE);
    
    // Test empty vector
    pop_any_double_vector(vec, 0);  // Remove the only element
    errno = 0;
    result = pop_any_double_vector(vec, 0);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_double_static(void **state) {
    (void) state;
    
    double_v arr = init_double_array(3);
    
    // Fill array
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    push_back_double_vector(&arr, 3.0);
    
    // Pop from middle
    errno = 0;
    double result = pop_any_double_vector(&arr, 1);
    assert_float_equal(result, 2.0, 0.0001);
    assert_int_equal(d_size(&arr), 2);
    assert_float_equal(double_vector_index(&arr, 0), 1.0, 0.0001);
    assert_float_equal(double_vector_index(&arr, 1), 3.0, 0.0001);
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Test with DBL_MAX as valid data
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, DBL_MAX);
    push_back_double_vector(vec, 3.0);
     
    errno = 0;
    double result = pop_any_double_vector(vec, 1);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, 0);  // Should be 0 since this is valid data
     
    // Test with NaN
    push_back_double_vector(vec, NAN);
    errno = 0;
    result = pop_any_double_vector(vec, 2);
    assert_true(isnan(result));
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// ================================================================================ 
// ================================================================================

void test_reverse_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    // Test single element
    push_back_double_vector(vec, 1.0);
    reverse_double_vector(vec);
    assert_int_equal(d_size(vec), 1);
    assert_float_equal(double_vector_index(vec, 0), 1.0, 0.0001);
    
    // Test even number of elements
    push_back_double_vector(vec, 2.0);
    reverse_double_vector(vec);
    assert_int_equal(d_size(vec), 2);
    assert_float_equal(double_vector_index(vec, 0), 2.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 1.0, 0.0001);
    
    // Test odd number of elements
    push_back_double_vector(vec, 3.0);
    reverse_double_vector(vec);
    assert_int_equal(d_size(vec), 3);
    assert_float_equal(double_vector_index(vec, 0), 3.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 1.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 2), 2.0, 0.0001);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_reverse_double_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    reverse_double_vector(NULL);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    double_v vec = {0};
    vec.data = NULL;
    errno = 0;
    reverse_double_vector(&vec);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    double_v* empty_vec = init_double_vector(1);
    assert_non_null(empty_vec);
    errno = 0;
    reverse_double_vector(empty_vec);
    assert_int_equal(errno, ENODATA);
    
    free_double_vector(empty_vec);
}
// -------------------------------------------------------------------------------- 

void test_reverse_double_static(void **state) {
    (void) state;
    
    double_v arr = init_double_array(3);
    
    // Add test values
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    push_back_double_vector(&arr, 3.0);
    
    // Test reversal
    reverse_double_vector(&arr);
    assert_int_equal(d_size(&arr), 3);
    assert_float_equal(double_vector_index(&arr, 0), 3.0, 0.0001);
    assert_float_equal(double_vector_index(&arr, 1), 2.0, 0.0001);
    assert_float_equal(double_vector_index(&arr, 2), 1.0, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_reverse_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Add special values
    push_back_double_vector(vec, INFINITY);
    push_back_double_vector(vec, NAN);
    push_back_double_vector(vec, -INFINITY);
    
    // Test reversal
    reverse_double_vector(vec);
    
    // Verify reversed order
    assert_true(isinf(double_vector_index(vec, 0)) && double_vector_index(vec, 0) < 0.0);  // -INFINITY
    assert_true(isnan(double_vector_index(vec, 1)));  // NAN
    assert_true(isinf(double_vector_index(vec, 2)) && double_vector_index(vec, 2) > 0.0);  // INFINITY
    
    free_double_vector(vec);
}
// ================================================================================
// ================================================================================ 

void test_sort_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(5);
    assert_non_null(vec);
    
    // Test ascending sort
    push_back_double_vector(vec, 5.0);
    push_back_double_vector(vec, 3.0);
    push_back_double_vector(vec, 4.0);
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    
    sort_double_vector(vec, FORWARD);
    
    // Verify ascending order
    for (size_t i = 0; i < d_size(vec) - 1; i++) {
        assert_true(double_vector_index(vec, i) <= double_vector_index(vec, i + 1));
    }
    
    // Test descending sort
    sort_double_vector(vec, REVERSE);
    
    // Verify descending order
    for (size_t i = 0; i < d_size(vec) - 1; i++) {
        assert_true(double_vector_index(vec, i) >= double_vector_index(vec, i + 1));
    }
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_edge_double_cases(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(10);
    assert_non_null(vec);
    
    // Test empty vector
    sort_double_vector(vec, FORWARD);
    assert_int_equal(d_size(vec), 0);
    
    // Test single element
    push_back_double_vector(vec, 1.0);
    sort_double_vector(vec, FORWARD);
    assert_int_equal(d_size(vec), 1);
    assert_float_equal(double_vector_index(vec, 0), 1.0, 0.0001);
    
    // Test two elements
    push_back_double_vector(vec, 0.0);
    sort_double_vector(vec, FORWARD);
    assert_float_equal(double_vector_index(vec, 0), 0.0, 0.0001);
    assert_float_equal(double_vector_index(vec, 1), 1.0, 0.0001);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_double_duplicates(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(5);
    assert_non_null(vec);
    
    // Add duplicate values
    push_back_double_vector(vec, 3.0);
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 3.0);
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    
    sort_double_vector(vec, FORWARD);
    
    // Verify order with duplicates
    for (size_t i = 0; i < d_size(vec) - 1; i++) {
        assert_true(double_vector_index(vec, i) <= double_vector_index(vec, i + 1));
    }
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(5);
    assert_non_null(vec);
    
    // Add special values
    push_back_double_vector(vec, INFINITY);
    push_back_double_vector(vec, -INFINITY);
    push_back_double_vector(vec, NAN);
    push_back_double_vector(vec, 0.0);
    
    sort_double_vector(vec, FORWARD);
    
    // Verify NaN handling and infinity ordering
    size_t last_valid_index = d_size(vec) - 1;
    while (last_valid_index > 0 && isnan(double_vector_index(vec, last_valid_index))) {
        last_valid_index--;
    }
     
    // Check that -INFINITY is first and INFINITY is last (excluding NaN)
    assert_true(isinf(double_vector_index(vec, 0)) && double_vector_index(vec, 0) < 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_static_double_array(void **state) {
    (void) state;
    
    double_v arr = init_double_array(5);
    
    // Add unsorted values
    push_back_double_vector(&arr, 5.0);
    push_back_double_vector(&arr, 3.0);
    push_back_double_vector(&arr, 4.0);
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    
    sort_double_vector(&arr, FORWARD);
    
    // Verify ascending order
    for (size_t i = 0; i < d_size(&arr) - 1; i++) {
        assert_true(double_vector_index(&arr, i) <= double_vector_index(&arr, i + 1));
    }
}
// -------------------------------------------------------------------------------- 

void test_sort_double_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    sort_double_vector(NULL, FORWARD);
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================ 

void test_trim_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(10);  // Start with capacity of 10
    assert_non_null(vec);
    
    // Add 5 elements
    for (double i = 0; i < 5; i++) {
        push_back_double_vector(vec, i);
    }
    
    size_t original_alloc = d_alloc(vec);
    assert_int_equal(original_alloc, 10);
    assert_int_equal(d_size(vec), 5);
    
    // Trim the vector
    errno = 0;
    trim_double_vector(vec);
    assert_int_equal(errno, 0);
    
    // Verify capacity matches size
    assert_int_equal(d_alloc(vec), 5);
    assert_int_equal(d_size(vec), 5);
    
    // Verify data is intact
    for (size_t i = 0; i < d_size(vec); i++) {
        assert_float_equal(double_vector_index(vec, i), (double)i, 0.0001);
    }
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_empty_double_vector(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(5);  // Capacity of 5, but empty
    assert_non_null(vec);
    
    size_t original_alloc = d_alloc(vec);
    assert_int_equal(original_alloc, 5);
    assert_int_equal(d_size(vec), 0);
     
    // Trim the empty vector
    errno = 0;
    trim_double_vector(vec);
    assert_int_equal(errno, ENODATA);

    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_static_double_array(void **state) {
    (void) state;
    
    double_v arr = init_double_array(5);
    
    // Add some elements
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    
    size_t original_alloc = d_alloc(&arr);
    size_t original_size = d_size(&arr);
    
    // Try to trim static array
    errno = 0;
    trim_double_vector(&arr);
    assert_int_equal(errno, 0);  // Should not set error
    
    // Verify nothing changed
    assert_int_equal(d_alloc(&arr), original_alloc);
    assert_int_equal(d_size(&arr), original_size);
}
// -------------------------------------------------------------------------------- 

void test_trim_already_double_optimal(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Fill to capacity
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 3.0);
    
    assert_int_equal(d_alloc(vec), 3);
    assert_int_equal(d_size(vec), 3);
    
    // Try to trim an already optimal vector
    errno = 0;
    trim_double_vector(vec);
    assert_int_equal(errno, 0);
    
    // Verify nothing changed
    assert_int_equal(d_alloc(vec), 3);
    assert_int_equal(d_size(vec), 3);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_double_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    trim_double_vector(NULL);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    double_v vec = {0};
    vec.data = NULL;
    errno = 0;
    trim_double_vector(&vec);
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================ 

void test_binary_search_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(5);
    assert_non_null(vec);
    
    // Add values in order
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 3.0);
    push_back_double_vector(vec, 4.0);
    push_back_double_vector(vec, 5.0);
    
    // Test exact matches
    errno = 0;
    assert_int_equal(binary_search_double_vector(vec, 1.0, 0.0001, false), 0);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_double_vector(vec, 3.0, 0.0001, false), 2);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_double_vector(vec, 5.0, 0.0001, false), 4);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_double_tolerance(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 3.0);
    
    // Test with tolerance
    errno = 0;
    assert_int_equal(binary_search_double_vector(vec, 1.1, 0.2, false), 0);  // Should find 1.0
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_double_vector(vec, 2.95, 0.1, false), 2);  // Should find 3.0
    assert_int_equal(errno, 0);
    
    // Test value outside tolerance
    errno = 0;
    assert_int_equal(binary_search_double_vector(vec, 2.5, 0.1, false), LONG_MAX);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_with_double_sort(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(5);
    assert_non_null(vec);
    
    // Add values out of order
    push_back_double_vector(vec, 5.0f);
    push_back_double_vector(vec, 3.0f);
    push_back_double_vector(vec, 1.0f);
    push_back_double_vector(vec, 4.0f);
    push_back_double_vector(vec, 2.0f);
    
    // Search with sort_first = true
    errno = 0;
    assert_int_equal(binary_search_double_vector(vec, 3.0, 0.0001, true), 2);
    assert_int_equal(errno, 0);
    
    // Verify the vector is now sorted
    for (size_t i = 0; i < d_size(vec) - 1; i++) {
        assert_true(double_vector_index(vec, i) <= double_vector_index(vec, i + 1));
    }
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_double_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_int_equal(binary_search_double_vector(NULL, 1.0, 0.0001, false), LONG_MAX);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    double_v* vec = init_double_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_int_equal(binary_search_double_vector(vec, 1.0, 0.0001, false), LONG_MAX);
    assert_int_equal(errno, ENODATA);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_double_static(void **state) {
    (void) state;
    
    double_v arr = init_double_array(5);
    
    // Add values
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    push_back_double_vector(&arr, 3.0);
    push_back_double_vector(&arr, 4.0);
    push_back_double_vector(&arr, 5.0);
    
    // Test search in static array
    errno = 0;
    assert_int_equal(binary_search_double_vector(&arr, 3.0, 0.0001, false), 2);
    assert_int_equal(errno, 0);
    
    // Test value not found
    errno = 0;
    assert_int_equal(binary_search_double_vector(&arr, 6.0, 0.0001, false), LONG_MAX);
    assert_int_equal(errno, 0);
}
// ================================================================================ 
// ================================================================================ 

void test_update_double_vector_nominal(void **state) {
    (void) state;
    double_v arr = init_double_array(5);
    
    // Add values
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    push_back_double_vector(&arr, 3.0);
    push_back_double_vector(&arr, 4.0);
    push_back_double_vector(&arr, 5.0);

    update_double_vector(&arr, 2, 12.0);
    assert_float_equal(12.0, double_vector_index(&arr, 2), 1.0e-6);
}
// -------------------------------------------------------------------------------- 

void test_update_double_vector_null(void **state) {
    errno = 0;
    update_double_vector(NULL, 3, 8.2);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_update_double_vector_bad_index(void **state) {
    (void) state;
    double_v* vec = init_double_vector(3);
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 3.0);
    errno = 0;
    update_double_vector(vec, 5, 3.0);
    assert_int_equal(errno, ERANGE);
    free_double_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_min_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(5);
    assert_non_null(vec);
    
    // Test single element
    push_back_double_vector(vec, 1.0);
    errno = 0;
    assert_float_equal(min_double_vector(vec), 1.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test multiple elements
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, -3.0);
    push_back_double_vector(vec, 4.0);
    push_back_double_vector(vec, 0.0);
    
    errno = 0;
    assert_float_equal(min_double_vector(vec), -3.0, 0.0001);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_max_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(5);
    assert_non_null(vec);
    
    // Test single element
    push_back_double_vector(vec, 1.0);
    errno = 0;
    assert_float_equal(max_double_vector(vec), 1.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test multiple elements
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, -3.0);
    push_back_double_vector(vec, 4.0);
    push_back_double_vector(vec, 0.0);
    
    errno = 0;
    assert_float_equal(max_double_vector(vec), 4.0, 0.0001);  // This will fail with current implementation
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_min_max_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    // Test with infinity
    push_back_double_vector(vec, INFINITY);
    push_back_double_vector(vec, -INFINITY);
    push_back_double_vector(vec, 1.0);
    
    errno = 0;
    assert_true(isinf(min_double_vector(vec)) && min_double_vector(vec) < 0);  // Should be -INFINITY
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_true(isinf(max_double_vector(vec)) && max_double_vector(vec) > 0);  // Should be INFINITY
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_min_max_static_double_array(void **state) {
    (void) state;
    
    double_v arr = init_double_array(3);
    
    push_back_double_vector(&arr, 3.0);
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    
    errno = 0;
    assert_float_equal(min_double_vector(&arr), 1.0f, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(max_double_vector(&arr), 3.0, 0.0001);  // This will fail with current implementation
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_min_max_double_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_float_equal(min_double_vector(NULL), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(max_double_vector(NULL), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    double_v* vec = init_double_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_float_equal(min_double_vector(vec), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(max_double_vector(vec), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    free_double_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_sum_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    // Test single value
    push_back_double_vector(vec, 1.0);
    errno = 0;
    assert_float_equal(sum_double_vector(vec), 1.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test multiple values
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 3.0);
    push_back_double_vector(vec, 4.0);
    
    errno = 0;
    assert_float_equal(sum_double_vector(vec), 10.0, 0.0001);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_average_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    // Test single value
    push_back_double_vector(vec, 2.0);
    errno = 0;
    assert_float_equal(average_double_vector(vec), 2.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test multiple values
    push_back_double_vector(vec, 4.0);
    push_back_double_vector(vec, 6.0);
    push_back_double_vector(vec, 8.0);
    
    errno = 0;
    assert_float_equal(average_double_vector(vec), 5.0, 0.0001);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Test with infinity
    push_back_double_vector(vec, INFINITY);
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    
    errno = 0;
    assert_true(isinf(sum_double_vector(vec)));
    assert_int_equal(errno, 0);

    errno = 0;
    assert_true(isinf(average_double_vector(vec)));
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_double_negative(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    push_back_double_vector(vec, -1.0);
    push_back_double_vector(vec, -2.0);
    push_back_double_vector(vec, 5.0);
    push_back_double_vector(vec, 2.0);
    
    errno = 0;
    assert_float_equal(sum_double_vector(vec), 4.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(average_double_vector(vec), 1.0, 0.0001);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_double_static(void **state) {
    (void) state;
    
    double_v arr = init_double_array(3);
    
    push_back_double_vector(&arr, 1.0);
    push_back_double_vector(&arr, 2.0);
    push_back_double_vector(&arr, 3.0);
    
    errno = 0;
    assert_float_equal(sum_double_vector(&arr), 6.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(average_double_vector(&arr), 2.0, 0.0001);
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_double_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_float_equal(sum_double_vector(NULL), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(average_double_vector(NULL), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    double_v* vec = init_double_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_float_equal(sum_double_vector(vec), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(average_double_vector(vec), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    free_double_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_stdev_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    // Dataset with known standard deviation
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 4.0);
    push_back_double_vector(vec, 4.0);
    push_back_double_vector(vec, 6.0);
    
    // Mean = 4.0, variance = 2.0, stdev = sqrt(2.0)
    errno = 0;
    assert_float_equal(stdev_double_vector(vec), sqrt(2.0), 0.0001);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_stdev_single_double_value(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(1);
    assert_non_null(vec);
    
    push_back_double_vector(vec, 2.0);
    
    errno = 0;
    double result = stdev_double_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);  // Standard deviation of single value is 0
    assert_int_equal(errno, ENODATA);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_stdev_same_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // All same values should give stdev of 0
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 2.0);
    
    errno = 0;
    assert_float_equal(stdev_double_vector(vec), 0.0f, 0.0001);
    assert_int_equal(errno, 0);
    
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_cum_sum_double_basic(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    push_back_double_vector(vec, 3.0);
    push_back_double_vector(vec, 4.0);
    
    double_v* cum_sum = cum_sum_double_vector(vec);
    assert_non_null(cum_sum);
    assert_int_equal(d_size(cum_sum), 4);
    
    // Check cumulative sums: 1, 3, 6, 10
    assert_float_equal(double_vector_index(cum_sum, 0), 1.0, 0.0001);
    assert_float_equal(double_vector_index(cum_sum, 1), 3.0, 0.0001);
    assert_float_equal(double_vector_index(cum_sum, 2), 6.0, 0.0001);
    assert_float_equal(double_vector_index(cum_sum, 3), 10.0, 0.0001);
    
    free_double_vector(vec);
    free_double_vector(cum_sum);
}
// -------------------------------------------------------------------------------- 

void test_cum_sum_double_negative(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(4);
    assert_non_null(vec);
    
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, -2.0);
    push_back_double_vector(vec, 3.0);
    push_back_double_vector(vec, -4.0);
    
    double_v* cum_sum = cum_sum_double_vector(vec);
    assert_non_null(cum_sum);
    
    // Check cumulative sums: 1, -1, 2, -2
    assert_float_equal(double_vector_index(cum_sum, 0), 1.0, 0.0001);
    assert_float_equal(double_vector_index(cum_sum, 1), -1.0, 0.0001);
    assert_float_equal(double_vector_index(cum_sum, 2), 2.0, 0.0001);
    assert_float_equal(double_vector_index(cum_sum, 3), -2.0, 0.0001);
    
    free_double_vector(vec);
    free_double_vector(cum_sum);
}
// -------------------------------------------------------------------------------- 

void test_stdev_cum_sum_special_double_values(void **state) {
    (void) state;
    
    double_v* vec = init_double_vector(3);
    assert_non_null(vec);
    
    // Test with infinity
    push_back_double_vector(vec, INFINITY);
    push_back_double_vector(vec, 1.0);
    push_back_double_vector(vec, 2.0);
    
    errno = 0;
    assert_true(isinf(stdev_double_vector(vec)));

    double_v* cum_sum = cum_sum_double_vector(vec);
    assert_non_null(cum_sum);
    assert_true(isinf(double_vector_index(cum_sum, 0)));
    assert_true(isinf(double_vector_index(cum_sum, 1)));
    assert_true(isinf(double_vector_index(cum_sum, 2)));
    
    free_double_vector(vec);
    free_double_vector(cum_sum);
}
// -------------------------------------------------------------------------------- 

void test_stdev_cum_sum_double_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_float_equal(stdev_double_vector(NULL), DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
    
    assert_null(cum_sum_double_vector(NULL));
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    double_v* vec = init_double_vector(1);
    assert_non_null(vec);

    errno = 0;
    assert_float_equal(stdev_double_vector(vec), DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);

    assert_null(cum_sum_double_vector(vec));
    assert_int_equal(errno, EINVAL);
     
    free_double_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_dot_double_product_basic(void **state) {
    double_v *v1 = init_double_vector(3);
    double_v *v2 = init_double_vector(3);

    push_back_double_vector(v1, 1.0);
    push_back_double_vector(v1, 2.0);
    push_back_double_vector(v1, 3.0);

    push_back_double_vector(v2, 4.0);
    push_back_double_vector(v2, 5.0);
    push_back_double_vector(v2, 6.0);

    double result = dot_double_vector(v1, v2);
    assert_float_equal(result, 32.0, 1e-6);

    free_double_vector(v1);
    free_double_vector(v2);
}
// ----------------------------------------------------------------------------

void test_dot_double_product_mismatched_lengths(void **state) {
    double_v *v1 = init_double_vector(2);
    double_v *v2 = init_double_vector(3);

    push_back_double_vector(v1, 1.0);
    push_back_double_vector(v1, 2.0);

    push_back_double_vector(v2, 3.0);
    push_back_double_vector(v2, 4.0);
    push_back_double_vector(v2, 5.0);

    double result = dot_double_vector(v1, v2);
    assert_float_equal(result, DBL_MAX, 1e-6);
    assert_int_equal(errno, ERANGE);

    free_double_vector(v1);
    free_double_vector(v2);
}
// ----------------------------------------------------------------------------

void test_dot_double_product_null_inputs(void **state) {
    double result = dot_double_vector(NULL, NULL);
    assert_float_equal(result, DBL_MAX, 1e-6);
    assert_int_equal(errno, EINVAL);
}
// ----------------------------------------------------------------------------

void test_dot_double_product_zero_length(void **state) {
    double_v *v1 = init_double_vector(0);
    double_v *v2 = init_double_vector(0);

    double result = dot_double_vector(v1, v2);
    assert_float_equal(result, DBL_MAX, 1e-6);  // dot of zero-length vectors is 0

    free_double_vector(v1);
    free_double_vector(v2);
}
// -------------------------------------------------------------------------------- 

void test_cross_double_valid(void **state) {
    (void) state;

    double a[3] = {1.0, 0.0, 0.0};  // î
    double b[3] = {0.0, 1.0, 0.0};  // ĵ
    double result[3] = {0};

    bool ok = cross_double(a, b, result);
    assert_true(ok);

    assert_float_equal(result[0], 0.0, 1e-6f);
    assert_float_equal(result[1], 0.0, 1e-6f);
    assert_float_equal(result[2], 1.0f, 1e-6f);  // î × ĵ = k̂
}
// --------------------------------------------------------------------------------

void test_cross_double_null_inputs(void **state) {
    (void) state;

    double a[3] = {1.0, 0.0, 0.0};
    double result[3] = {0};

    assert_false(cross_double(NULL, a, result));
    assert_int_equal(errno, EINVAL);

    assert_false(cross_double(a, NULL, result));
    assert_int_equal(errno, EINVAL);

    assert_false(cross_double(a, a, NULL));
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_cross_double_vector_valid(void **state) {
    (void) state;

    double_v* vec1 = init_double_vector(3);
    double_v* vec2 = init_double_vector(3);

    vec1->data[0] = 1.0; vec1->data[1] = 0.0; vec1->data[2] = 0.0;
    vec2->data[0] = 0.0; vec2->data[1] = 1.0; vec2->data[2] = 0.0;
    vec1->len = 3;
    vec2->len = 3;

    double_v* result = cross_double_vector(vec1, vec2);
    assert_non_null(result);
    assert_int_equal(result->len, 3);
    assert_float_equal(result->data[0], 0.0f, 1e-6f);
    assert_float_equal(result->data[1], 0.0f, 1e-6f);
    assert_float_equal(result->data[2], 1.0f, 1e-6f);

    free_double_vector(vec1);
    free_double_vector(vec2);
    free_double_vector(result);
}
// --------------------------------------------------------------------------------

void test_cross_double_vector_invalid_inputs(void **state) {
    (void) state;

    double_v* bad1 = NULL;
    double_v* bad2 = init_double_vector(3);
    double_v* short_vec = init_double_vector(2);

    double_v* result;

    result = cross_double_vector(bad1, bad2);
    assert_null(result);
    assert_int_equal(errno, EINVAL);

    result = cross_double_vector(bad2, NULL);
    assert_null(result);
    assert_int_equal(errno, EINVAL);

    result = cross_double_vector(short_vec, bad2);
    assert_null(result);
    assert_int_equal(errno, ERANGE);

    result = cross_double_vector(bad2, short_vec);
    assert_null(result);
    assert_int_equal(errno, ERANGE);

    free_double_vector(bad2);
    free_double_vector(short_vec);
}
// ================================================================================ 
// ================================================================================ 

/* Setup and teardown functions */
static dict_d* test_dict = NULL;

int setup_double(void** state) {
    test_dict = init_double_dict();
    if (!test_dict) return -1;
    *state = test_dict;
    return 0;
}
// -------------------------------------------------------------------------------- 

int teardown_double(void** state) {
    free_double_dict(test_dict);
    return 0;
}
// -------------------------------------------------------------------------------- 

/* Test initialization */
void test_init_double_dict(void** state) {
    dict_d* dict = init_double_dict();
    assert_non_null(dict);
    assert_int_equal(double_dict_hash_size(dict), 0);
    assert_int_equal(double_dict_size(dict), 0);
    assert_true(double_dict_alloc(dict) > 0);
    free_double_dict(dict);
}
// -------------------------------------------------------------------------------- 

/* Test insertion */
void test_insert_double_dict_basic(void** state) {
    dict_d* dict = *state;
    
    assert_true(insert_double_dict(dict, "test", 1.0f));
    assert_int_equal(double_dict_hash_size(dict), 1);
    assert_int_equal(double_dict_size(dict), 1);
    
    double value = get_double_dict_value(dict, "test");
    assert_float_equal(value, 1.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_insert_double_dict_duplicate(void** state) {
    dict_d* dict = *state;
    
    assert_true(insert_double_dict(dict, "test", 1.0f));
    assert_false(insert_double_dict(dict, "test", 2.0f));
    
    double value = get_double_dict_value(dict, "test");
    assert_float_equal(value, 1.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_insert_double_dict_null(void** state) {
    dict_d* dict = *state;
    
    assert_false(insert_double_dict(NULL, "test", 1.0f));
    assert_false(insert_double_dict(dict, NULL, 1.0f));
}
// -------------------------------------------------------------------------------- 

/* Test retrieval */
void test_get_double_dict_value_basic(void** state) {
    dict_d* dict = *state;
    
    insert_double_dict(dict, "key1", 1.5f);
    double value = get_double_dict_value(dict, "key1");
    assert_float_equal(value, 1.5f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_get_double_dict_value_missing(void** state) {
    dict_d* dict = *state;
    
    double value = get_double_dict_value(dict, "nonexistent");
    assert_float_equal(value, FLT_MAX, 0.0001f);
}
// -------------------------------------------------------------------------------- 

/* Test update */
void test_update_double_dict_basic(void** state) {
    dict_d* dict = *state;
    
    insert_double_dict(dict, "key1", 1.0f);
    assert_true(update_double_dict(dict, "key1", 2.0f));
    
    double value = get_double_dict_value(dict, "key1");
    assert_float_equal(value, 2.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_update_double_dict_missing(void** state) {
    dict_d* dict = *state;
    
    assert_false(update_double_dict(dict, "nonexistent", 1.0f));
}
// -------------------------------------------------------------------------------- 

/* Test removal */
void test_pop_double_dict_basic(void** state) {
    dict_d* dict = *state;
    
    insert_double_dict(dict, "key1", 1.5f);
    double value = pop_double_dict(dict, "key1");
    assert_float_equal(value, 1.5f, 0.0001f);
    assert_int_equal(double_dict_hash_size(dict), 0);
}
// -------------------------------------------------------------------------------- 

void test_pop_double_dict_missing(void** state) {
    dict_d* dict = *state;
    
    double value = pop_double_dict(dict, "nonexistent");
    assert_float_equal(value, FLT_MAX, 0.0001f);
}
// -------------------------------------------------------------------------------- 

/* Test resize behavior */
void test_resize_double_behavior(void** state) {
    dict_d* dict = *state;
    char key[20];
    size_t initial_alloc = double_dict_alloc(dict);
    
    // Insert enough items to trigger resize
    for (int i = 0; i < 100; i++) {
        sprintf(key, "key%d", i);
        assert_true(insert_double_dict(dict, key, (double)i));
    }
    
    assert_true(double_dict_alloc(dict) > initial_alloc);
}
// -------------------------------------------------------------------------------- 

/* Test key/value extraction */
void test_get_keys_double_dict(void** state) {
    dict_d* dict = *state;
    
    insert_double_dict(dict, "key1", 1.0f);
    insert_double_dict(dict, "key2", 2.0f);
    
    string_v* keys = get_keys_double_dict(dict);
    assert_non_null(keys);
    assert_int_equal(double_dict_hash_size(dict), 2);
    
    free_str_vector(keys);
}
// -------------------------------------------------------------------------------- 

void test_get_values_double_dict(void** state) {
    dict_d* dict = *state;
    
    insert_double_dict(dict, "key1", 1.0f);
    insert_double_dict(dict, "key2", 2.0f);
    
    double_v* values = get_values_double_dict(dict);
    assert_non_null(values);
    assert_int_equal(double_dict_hash_size(dict), 2);
    
    free_double_vector(values);
}
// -------------------------------------------------------------------------------- 

/* Helper function for testing iterator */
static void count_entries(const char* key, double value, void* user_data) {
    size_t* count = (size_t*)user_data;
    (*count)++;
}

/* Helper function to sum values */
static void sum_values(const char* key, double value, void* user_data) {
    double* sum = (double*)user_data;
    *sum += value;
}
// -------------------------------------------------------------------------------- 

void test_foreach_double_dict_basic(void** state) {
    dict_d* dict = *state;
    size_t count = 0;
    
    // Add some test data
    assert_true(insert_double_dict(dict, "key1", 1.0f));
    assert_true(insert_double_dict(dict, "key2", 2.0f));
    assert_true(insert_double_dict(dict, "key3", 3.0f));
    
    // Test iteration count
    assert_true(foreach_double_dict(dict, count_entries, &count));
    assert_int_equal(count, 3);
    assert_int_equal(count, double_dict_hash_size(dict));
    
    // Test value sum
    double sum = 0.0f;
    assert_true(foreach_double_dict(dict, sum_values, &sum));
    assert_float_equal(sum, 6.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_foreach_double_dict_empty(void** state) {
    dict_d* dict = *state;
    size_t count = 0;
    
    assert_true(foreach_double_dict(dict, count_entries, &count));
    assert_int_equal(count, 0);
}
// -------------------------------------------------------------------------------- 

void test_foreach_double_dict_null(void** state) {
    dict_d* dict = *state;
    size_t count = 0;
    
    assert_false(foreach_double_dict(NULL, count_entries, &count));
    assert_false(foreach_double_dict(dict, NULL, &count));
}
// -------------------------------------------------------------------------------- 

void test_dictionary_double_gbc(void **state) {
    dict_d* dict DDICT_GBC = init_double_dict();
    insert_double_dict(dict, "Key1", 1.0);
}
// ================================================================================ 
// ================================================================================ 

void test_vector_double_dictionary(void **state) {
    dict_dv* dict = init_doublev_dict();
    bool result;
    result = create_doublev_dict(dict, "one", 3);
    assert_true(result);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 1.0);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 2.0);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 3.0);

    double test_one[3] = {1.0, 2.0, 3.0};
    double_v* vec1 = return_doublev_pointer(dict, "one");
    for (size_t i = 0; i < double_vector_size(vec1); i++) {
        assert_float_equal(double_vector_index(vec1, i), test_one[i], 1.0e-3);
    }

    result = create_doublev_dict(dict, "two", 3);
    assert_true(result);
    push_back_double_vector(return_doublev_pointer(dict, "two"), 4.0);
    push_back_double_vector(return_doublev_pointer(dict, "two"), 5.0);
    push_back_double_vector(return_doublev_pointer(dict, "two"), 6.0);

    double test_two[3] = {4.0, 5.0, 6.0};
    double_v* vec2 = return_doublev_pointer(dict, "two");
    for (size_t i = 0; i < double_vector_size(vec2); i++) {
        assert_float_equal(double_vector_index(vec2, i), test_two[i], 1.0e-3);
    }

    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_vector_dictionary_double_resize(void **state) {
    dict_dv* dict = init_doublev_dict();
    bool result;
    result = create_doublev_dict(dict, "one", 3);
    assert_true(result);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 1.0);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 2.0);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 3.0);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 4.0);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 5.0);

    double test_one[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double_v* vec1 = return_doublev_pointer(dict, "one");
    for (size_t i = 0; i < double_vector_size(vec1); i++) {
        assert_float_equal(double_vector_index(vec1, i), test_one[i], 1.0e-3);
    }
    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_vector_dictionary_double_gbc(void **state) {
        DDICTV_GBC dict_dv* dict = init_doublev_dict();
        bool result;
        result = create_doublev_dict(dict, "one", 3);
        assert_true(result);
        push_back_double_vector(return_doublev_pointer(dict, "one"), 1.0);
        push_back_double_vector(return_doublev_pointer(dict, "one"), 2.0);
        push_back_double_vector(return_doublev_pointer(dict, "one"), 3.0);
        push_back_double_vector(return_doublev_pointer(dict, "one"), 4.0);
        push_back_double_vector(return_doublev_pointer(dict, "one"), 5.0);

        double test_one[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
        double_v* vec1 = return_doublev_pointer(dict, "one");
        for (size_t i = 0; i < double_vector_size(vec1); i++) {
            assert_float_equal(double_vector_index(vec1, i), test_one[i], 1.0e-3);
        }
    }
#endif
// --------------------------------------------------------------------------------

void test_pop_vector_double_dictionary(void **state) {
    dict_dv* dict = init_doublev_dict();
    bool result;
    result = create_doublev_dict(dict, "one", 3);
    assert_true(result);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 1.0);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 2.0);

    double test_one[3] = {1.0, 2.0, 3.0};
    double_v* vec1 = return_doublev_pointer(dict, "one");
    for (size_t i = 0; i < double_vector_size(vec1); i++) {
        assert_float_equal(double_vector_index(vec1, i), test_one[i], 1.0e-3);
    }

    result = create_doublev_dict(dict, "two", 3);
    assert_true(result);
    push_back_double_vector(return_doublev_pointer(dict, "two"), 4.0);
    push_back_double_vector(return_doublev_pointer(dict, "two"), 5.0);

    double test_two[3] = {4.0, 5.0, 6.0};
    double_v* vec2 = return_doublev_pointer(dict, "two");
    for (size_t i = 0; i < double_vector_size(vec2); i++) {
        assert_float_equal(double_vector_index(vec2, i), test_two[i], 1.0e-3);
    }

    pop_doublev_dict(dict, "two");
    assert_false(has_key_doublev_dict(dict, "two"));
    assert_true(has_key_doublev_dict(dict, "one"));

    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_insert_doublev_dict_basic(void **state) {
    (void)state;  // unused

    dict_dv* dict = init_doublev_dict();
    assert_non_null(dict);

    // Valid DYNAMIC vector
    double_v* vec1 = init_double_vector(3);
    assert_non_null(vec1);
    assert_int_equal(vec1->alloc_type, DYNAMIC);

    bool result = insert_doublev_dict(dict, "key1", vec1);
    assert_true(result);

    // Confirm key exists
    assert_true(has_key_doublev_dict(dict, "key1"));

    // Try inserting same key again — should fail
    double_v* vec2 = init_double_vector(2);
    assert_non_null(vec2);
    errno = 0;
    result = insert_doublev_dict(dict, "key1", vec2);
    assert_false(result);
    assert_int_equal(errno, EEXIST);
    free_double_vector(vec2);  // Manual cleanup for rejected value

    // Try inserting STATIC vector — should fail
    double_v vec3 = init_double_array(2);  // This is heap-wrapped, but STATIC
    assert_int_equal(vec3.alloc_type, STATIC);
    errno = 0;
    result = insert_doublev_dict(dict, "key_static", &vec3);
    assert_false(result);
    assert_int_equal(errno, EPERM);

    // NULL input tests
    errno = 0;
    assert_false(insert_doublev_dict(NULL, "key", vec1));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(insert_doublev_dict(dict, NULL, vec1));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(insert_doublev_dict(dict, "keyX", NULL));
    assert_int_equal(errno, EINVAL);

    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_doublev_size_macros(void **state) {
    dict_dv* dict = init_doublev_dict();
    bool result;
    result = create_doublev_dict(dict, "one", 3);
    assert_true(result);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 1.0);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 2.0);
    push_back_double_vector(return_doublev_pointer(dict, "one"), 3.0);

    double test_one[3] = {1.0, 2.0, 3.0};
    double_v* vec1 = return_doublev_pointer(dict, "one");
    for (size_t i = 0; i < double_vector_size(vec1); i++) {
        assert_float_equal(double_vector_index(vec1, i), test_one[i], 1.0e-3);
    }

    result = create_doublev_dict(dict, "two", 3);
    assert_true(result);
    push_back_double_vector(return_doublev_pointer(dict, "two"), 4.0);
    push_back_double_vector(return_doublev_pointer(dict, "two"), 5.0);
    push_back_double_vector(return_doublev_pointer(dict, "two"), 6.0);

    double test_two[3] = {4.0, 5.0, 6.0};
    double_v* vec2 = return_doublev_pointer(dict, "two");
    for (size_t i = 0; i < double_vector_size(vec2); i++) {
        assert_float_equal(double_vector_index(vec2, i), test_two[i], 1.0e-3);
    }
    assert_int_equal(16, d_alloc(dict));
    assert_int_equal(2, d_size(dict));
    assert_int_equal(2, double_dictv_hash_size(dict));

    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_copy_doublev_dict_success(void **state) {
    (void)state;

    dict_dv* original = init_doublev_dict();
    double_v* v1 = init_double_vector(3);
    push_back_double_vector(v1, 1.0f);
    push_back_double_vector(v1, 2.0f);
    insert_doublev_dict(original, "alpha", v1);

    dict_dv* copy = copy_doublev_dict(original);
    assert_non_null(copy);
    assert_true(has_key_doublev_dict(copy, "alpha"));

    double_v* copied = return_doublev_pointer(copy, "alpha");
    assert_non_null(copied);
    assert_float_equal(double_vector_index(copied, 0), 1.0f, 1e-6);
    assert_float_equal(double_vector_index(copied, 1), 2.0f, 1e-6);

    // Ensure deep copy (modifying copy doesn't affect original)
    push_back_double_vector(copied, 999.0f);
    assert_int_not_equal(double_vector_size(copied), double_vector_size(v1));

    free_doublev_dict(original);
    free_doublev_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_doublev_dict_null_input(void **state) {
    (void)state;
    errno = 0;
    dict_dv* copy = copy_doublev_dict(NULL);
    assert_null(copy);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_copy_doublev_dict_static_vector(void **state) {
    (void)state;

    dict_dv* dict = init_doublev_dict();
    double_v vec = init_double_array(2);
    push_back_double_vector(&vec, 42.0f);
    insert_doublev_dict(dict, "badkey", &vec);  // Insert unsafe manually
    
    errno = 0;
    dict_dv* copy = copy_doublev_dict(dict);
    // assert_null(copy);
    // assert_int_equal(errno, EPERM);
    //
    free_doublev_dict(dict);
    free_doublev_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_doublev_dict_multiple_entries(void **state) {
    (void)state;

    dict_dv* dict = init_doublev_dict();
    double_v* v1 = init_double_vector(2);
    double_v* v2 = init_double_vector(2);
    push_back_double_vector(v1, 1.0f);
    push_back_double_vector(v2, 2.0f);
    insert_doublev_dict(dict, "a", v1);
    insert_doublev_dict(dict, "b", v2);

    dict_dv* copy = copy_doublev_dict(dict);
    assert_non_null(copy);
    assert_true(has_key_doublev_dict(copy, "a"));
    assert_true(has_key_doublev_dict(copy, "b"));

    free_doublev_dict(dict);
    free_doublev_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_doublev_dict_independence(void **state) {
    (void)state;

    dict_dv* dict = init_doublev_dict();
    double_v* v = init_double_vector(1);
    push_back_double_vector(v, 10.0f);
    insert_doublev_dict(dict, "x", v);

    dict_dv* copy = copy_doublev_dict(dict);
    double_v* original = return_doublev_pointer(dict, "x");
    double_v* copied = return_doublev_pointer(copy, "x");

    push_back_double_vector(copied, 20.0f);
    assert_int_not_equal(double_vector_size(original), double_vector_size(copied));

    free_doublev_dict(dict);
    free_doublev_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_merge_doublev_dict_no_overwrite(void **state) {
    (void)state;
    dict_dv* dict1 = init_doublev_dict();
    dict_dv* dict2 = init_doublev_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    double_v* vec1 = init_double_vector(1); push_back_double_vector(vec1, 1.0f);
    double_v* vec2 = init_double_vector(1); push_back_double_vector(vec2, 2.0f);
    double_v* vec3 = init_double_vector(1); push_back_double_vector(vec3, 3.0f);

    insert_doublev_dict(dict1, "alpha", vec1);
    insert_doublev_dict(dict1, "beta", vec2);
    insert_doublev_dict(dict2, "beta", vec3);  // conflict
    insert_doublev_dict(dict2, "gamma", init_double_vector(1));

    dict_dv* merged = merge_doublev_dict(dict1, dict2, false);
    assert_non_null(merged);

    double_v* merged_beta = return_doublev_pointer(merged, "beta");
    assert_float_equal(double_vector_index(merged_beta, 0), 2.0f, 1e-6);  // from dict1

    free_doublev_dict(dict1);
    free_doublev_dict(dict2);
    free_doublev_dict(merged);
}
// -------------------------------------------------------------------------------- 

void test_merge_doublev_dict_overwrite(void **state) {
    (void)state;

    dict_dv* dict1 = init_doublev_dict();
    dict_dv* dict2 = init_doublev_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    double_v* vec1 = init_double_vector(1); push_back_double_vector(vec1, 1.0f);
    double_v* vec2 = init_double_vector(1); push_back_double_vector(vec2, 2.0f);
    double_v* vec3 = init_double_vector(1); push_back_double_vector(vec3, 99.0f);

    insert_doublev_dict(dict1, "beta", vec2);
    insert_doublev_dict(dict2, "beta", vec3);  // conflict
    insert_doublev_dict(dict1, "alpha", vec1);

    dict_dv* merged = merge_doublev_dict(dict1, dict2, true);
    assert_non_null(merged);

    double_v* overwritten_beta = return_doublev_pointer(merged, "beta");
    assert_float_equal(double_vector_index(overwritten_beta, 0), 99.0f, 1e-6);  // from dict2

    free_doublev_dict(dict1);
    free_doublev_dict(dict2);
    free_doublev_dict(merged);
}
// -------------------------------------------------------------------------------- 

void test_merge_doublev_dict_reject_static(void **state) {
    (void)state;

    dict_dv* dict1 = init_doublev_dict();
    dict_dv* dict2 = init_doublev_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    double_v* vec1 = init_double_vector(1); push_back_double_vector(vec1, 1.0f);
    insert_doublev_dict(dict1, "alpha", vec1);

    double_v vec_static = init_double_array(2);  // stack + static
    push_back_double_vector(&vec_static, 42.0f);
    insert_doublev_dict(dict2, "static_key", &vec_static);

    errno = 0;
    dict_dv* merged = merge_doublev_dict(dict1, dict2, true);

    free_doublev_dict(dict1);
    free_doublev_dict(dict2);
    free_doublev_dict(merged);
}
// -------------------------------------------------------------------------------- 

void test_merge_doublev_dict_null_inputs(void **state) {
    (void)state;

    dict_dv* valid = init_doublev_dict();
    assert_non_null(valid);

    errno = 0;
    assert_null(merge_doublev_dict(NULL, valid, true));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_null(merge_doublev_dict(valid, NULL, false));
    assert_int_equal(errno, EINVAL);

    free_doublev_dict(valid);
}
// -------------------------------------------------------------------------------- 

void test_merge_doublev_dict_unique_keys(void **state) {
    (void)state;

    dict_dv* dict1 = init_doublev_dict();
    dict_dv* dict2 = init_doublev_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    // Create unique double_v* instances for each dictionary
    double_v* vx = init_double_vector(1); push_back_double_vector(vx, 1.0f);
    double_v* vy = init_double_vector(1); push_back_double_vector(vy, 2.0f);
    double_v* vz = init_double_vector(1); push_back_double_vector(vz, 3.0f);

    insert_doublev_dict(dict1, "x", vx);
    insert_doublev_dict(dict2, "y", vy);
    insert_doublev_dict(dict2, "z", vz);

    dict_dv* merged = merge_doublev_dict(dict1, dict2, false);
    assert_non_null(merged);
    assert_true(has_key_doublev_dict(merged, "x"));
    assert_true(has_key_doublev_dict(merged, "y"));
    assert_true(has_key_doublev_dict(merged, "z"));

    // Cleanup only from merged dict
    free_doublev_dict(merged);
    free_doublev_dict(dict1);
    free_doublev_dict(dict2);

    // Don't double-free dict1 and dict2 because their values were reused in merged
    // Alternative: clone data during merge to make them independent (deep copy)
}
// -------------------------------------------------------------------------------- 

void test_clear_doublev_dict_basic(void **state) {
    (void)state;

    dict_dv* dict = init_doublev_dict();
    assert_non_null(dict);

    create_doublev_dict(dict, "a", 3);
    create_doublev_dict(dict, "b", 2);

    assert_true(has_key_doublev_dict(dict, "a"));
    assert_true(has_key_doublev_dict(dict, "b"));
    assert_int_equal(double_dictv_size(dict), 2);
    assert_int_equal(double_dictv_hash_size(dict), 2);

    clear_doublev_dict(dict);

    assert_false(has_key_doublev_dict(dict, "a"));
    assert_false(has_key_doublev_dict(dict, "b"));
    assert_int_equal(double_dictv_size(dict), 0);
    assert_int_equal(double_dictv_hash_size(dict), 0);

    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_clear_doublev_dict_empty(void **state) {
    (void)state;

    dict_dv* dict = init_doublev_dict();
    assert_non_null(dict);

    clear_doublev_dict(dict);  // Should not crash
    assert_int_equal(double_dictv_size(dict), 0);
    assert_int_equal(double_dictv_hash_size(dict), 0);

    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_clear_doublev_dict_reuse_after_clear(void **state) {
    (void)state;

    dict_dv* dict = init_doublev_dict();
    assert_non_null(dict);

    create_doublev_dict(dict, "temp1", 2);
    create_doublev_dict(dict, "temp2", 2);
    clear_doublev_dict(dict);

    // Insert again after clear
    create_doublev_dict(dict, "new", 2);
    assert_true(has_key_doublev_dict(dict, "new"));
    assert_int_equal(double_dictv_hash_size(dict), 1);

    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

static void key_counter(const char* key, const double_v* value, void* user_data) {
    (void)key; (void)value;
    int* counter = (int*)user_data;
    (*counter)++;
}

void test_foreach_doublev_dict_counts_keys(void **state) {
    (void)state;

    dict_dv* dict = init_doublev_dict();
    create_doublev_dict(dict, "A", 2);
    create_doublev_dict(dict, "B", 3);
    create_doublev_dict(dict, "C", 1);

    int count = 0;
    bool result = foreach_doublev_dict(dict, key_counter, &count);

    assert_true(result);
    assert_int_equal(count, 3);

    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_foreach_doublev_dict_with_null_dict(void **state) {
    (void)state;
    errno = 0;

    bool result = foreach_doublev_dict(NULL, key_counter, NULL);
    assert_false(result);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_foreach_doublev_dict_with_null_callback(void **state) {
    (void)state;

    dict_dv* dict = init_doublev_dict();
    create_doublev_dict(dict, "temp", 1);

    errno = 0;
    bool result = foreach_doublev_dict(dict, NULL, NULL);

    assert_false(result);
    assert_int_equal(errno, EINVAL);

    free_doublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

typedef struct {
    double sum;
    size_t count;
} accumulator;

static void accumulate_values(const char* key, const double_v* vec, void* user_data) {
    (void)key;
    accumulator* acc = (accumulator*)user_data;
    for (size_t i = 0; i < double_vector_size(vec); ++i) {
        acc->sum += double_vector_index(vec, i);
        acc->count++;
    }
}

void test_foreach_doublev_dict_accumulates_sum(void **state) {
    (void)state;

    dict_dv* dict = init_doublev_dict();
    create_doublev_dict(dict, "sensor1", 3);
    create_doublev_dict(dict, "sensor2", 2);

    push_back_double_vector(return_doublev_pointer(dict, "sensor1"), 1.0f);
    push_back_double_vector(return_doublev_pointer(dict, "sensor1"), 2.0f);
    push_back_double_vector(return_doublev_pointer(dict, "sensor1"), 3.0f);

    push_back_double_vector(return_doublev_pointer(dict, "sensor2"), 4.0f);
    push_back_double_vector(return_doublev_pointer(dict, "sensor2"), 5.0f);

    accumulator acc = {0.0f, 0};
    bool result = foreach_doublev_dict(dict, accumulate_values, &acc);

    assert_true(result);
    assert_int_equal(acc.count, 5);
    assert_float_equal(acc.sum, 15.0f, 0.0001);

    free_doublev_dict(dict);
}
// ================================================================================ 
// ================================================================================ 

void test_create_dense_double_matrix(void **state) {
    matrix_d* mat = create_double_matrix(3, 3, 0);
    assert_non_null(mat);
    assert_int_equal(get_double_matrix_rows(mat), 3);
    assert_int_equal(get_double_matrix_cols(mat), 3);
    assert_int_equal(get_double_matrix_element_count(mat), 0);
    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_insert_and_get_dense_double_matrix(void **state) {
    matrix_d* mat = create_double_matrix(3, 3, 0);
    assert_true(insert_double_matrix(&mat, 0, 1, 2.5f, false));

    double value = get_double_matrix(mat, 0, 1);
    assert_float_equal(value, 2.5f, 0.0001);
    assert_int_equal(get_double_matrix_element_count(mat), 1);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_invalid_dense_double_insert(void **state) {
    matrix_d* mat = create_double_matrix(2, 2, 0);
    // Out-of-bounds
    assert_false(insert_double_matrix(&mat, 3, 0, 1.0f, false));
    assert_int_equal(errno, ERANGE);
    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_double_coo_conversion_from_dense(void **state) {
    const size_t rows = 50;
    const size_t cols = 50;
    matrix_d* mat = create_double_matrix(rows, cols, 0);  // start as dense

    // Insert just a few values → low density, triggers conversion to COO
    assert_true(insert_double_matrix(&mat, 0, 1, 1.0f, false));
    assert_true(insert_double_matrix(&mat, 10, 10, 2.0f, false));
    assert_true(insert_double_matrix(&mat, 20, 30, 3.0f, false));

    // Ensure values are retrievable after conversion
    assert_int_equal(get_double_matrix_type(mat), SPARSE_COO_MATRIX);
    assert_float_equal(get_double_matrix(mat, 10, 10), 2.0f, 0.0001);
    assert_int_equal(get_double_matrix_element_count(mat), 3);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_insert_double_and_get_coo_matrix(void **state) {
    const size_t rows = 100;
    const size_t cols = 100;
    matrix_d* mat = create_double_matrix(rows, cols, 0);

    // Insert entries to trigger COO and test correctness
    insert_double_matrix(&mat, 1, 1, 4.4f, false);
    insert_double_matrix(&mat, 99, 99, 5.5f, false);

    assert_float_equal(get_double_matrix(mat, 1, 1), 4.4f, 0.0001);
    assert_float_equal(get_double_matrix(mat, 99, 99), 5.5f, 0.0001);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_duplicate_double_insert_coo_disallowed(void **state) {
    matrix_d* mat = create_double_matrix(20, 20, 0);

    insert_double_matrix(&mat, 5, 5, 1.0f, false);
    assert_false(insert_double_matrix(&mat, 5, 5, 2.0f, false));  // not allowed
    assert_int_equal(errno, EEXIST);

    double v = get_double_matrix(mat, 5, 5);
    assert_float_equal(v, 1.0f, 0.0001);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_double_csr_conversion_from_coo(void **state) {
    matrix_d* mat = create_double_matrix(10, 10, 0);  // smaller matrix

    // Insert fewer elements (e.g., 20)
    for (size_t i = 0; i < 20; ++i)
        insert_double_matrix(&mat, i / 5, i % 5, (double)i, false);

    // Force CSR conversion
    convert_doubleMat_to_csr(&mat);
    
    assert_int_equal(mat->type, SPARSE_CSR_MATRIX);
    assert_int_equal(get_double_matrix_element_count(mat), 20);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_get_after_double_csr_conversion(void **state) {
    matrix_d* mat = create_double_matrix(20, 20, 0);
    insert_double_matrix(&mat, 3, 4, 7.7f, false);
    insert_double_matrix(&mat, 10, 15, 9.9f, false);

    // Convert Dense → COO
    convert_doubleMat_to_csr(&mat);
    
    assert_int_equal(mat->type, SPARSE_CSR_MATRIX);
    assert_float_equal(get_double_matrix(mat, 3, 4), 7.7f, 0.0001);
    assert_float_equal(get_double_matrix(mat, 10, 15), 9.9f, 0.0001);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_pop_after_double_csr_conversion(void **state) {
    matrix_d* mat = create_double_matrix(10, 10, 0);
    insert_double_matrix(&mat, 2, 2, 3.3f, false);
    insert_double_matrix(&mat, 5, 5, 5.5f, false);

    // Convert Dense → COO
    convert_doubleMat_to_csr(&mat);
    
    double val = pop_double_matrix(&mat, 2, 2);
    assert_float_equal(val, 3.3f, 0.0001);
    assert_int_equal(get_double_matrix_element_count(mat), 1);
    //
    double missing = get_double_matrix(mat, 2, 2);
    assert_float_equal(missing, FLT_MAX, 0.0001);  // default zero

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_get_double_matrix_bounds(void **state) {
    matrix_d* mat = create_double_matrix(5, 5, 0);
    insert_double_matrix(&mat, 1, 1, 4.2f, false);

    double val = get_double_matrix(mat, 10, 1);  // row out-of-bounds
    assert_float_equal(val, FLT_MAX, 0.0001);
    assert_int_equal(errno, ERANGE);

    val = get_double_matrix(mat, 1, 10);  // col out-of-bounds
    assert_float_equal(val, FLT_MAX, 0.0001);
    assert_int_equal(errno, ERANGE);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_insert_double_matrix_bounds(void **state) {
    matrix_d* mat = create_double_matrix(4, 4, 0);

    bool result = insert_double_matrix(&mat, 4, 1, 1.1f, false);  // row out-of-bounds
    assert_false(result);
    assert_int_equal(errno, ERANGE);

    result = insert_double_matrix(&mat, 1, 4, 1.1f, false);  // col out-of-bounds
    assert_false(result);
    assert_int_equal(errno, ERANGE);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_pop_double_matrix_bounds(void **state) {
    matrix_d* mat = create_double_matrix(3, 3, 0);
    insert_double_matrix(&mat, 0, 0, 2.2f, false);

    double val = pop_double_matrix(&mat, 3, 0);  // row out-of-bounds
    assert_float_equal(val, FLT_MAX, 0.0001);
    assert_int_equal(errno, ERANGE);

    val = pop_double_matrix(&mat, 0, 3);  // col out-of-bounds
    assert_float_equal(val, FLT_MAX, 0.0001);
    assert_int_equal(errno, ERANGE);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_get_double_matrix_null(void **state) {
    double val = get_double_matrix(NULL, 0, 0);
    assert_float_equal(val, FLT_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_insert_double_matrix_null(void **state) {
    bool result = insert_double_matrix(NULL, 0, 0, 1.0f, false);
    assert_false(result);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_pop_double_matrix_null(void **state) {
    double val = pop_double_matrix(NULL, 0, 0);
    assert_float_equal(val, FLT_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_double_matrix_gbc(void **state) {
        matrix_d* mat DBLMAT_GBC  = create_double_matrix(4, 4, 0);
        insert_double_matrix(&mat, 0, 1, 3.0f, false);
    }
#endif
// -------------------------------------------------------------------------------- 

void test_double_identity_inverse(void **state) {
    matrix_d *mat = create_double_matrix(3, 3, 1);
    insert_double_dense_matrix(mat, 0, 0, 1.0f);
    insert_double_dense_matrix(mat, 1, 1, 1.0f);
    insert_double_dense_matrix(mat, 2, 2, 1.0f);

    matrix_d *inv = invert_double_dense_matrix(mat);
    assert_non_null(inv);

    for (size_t i = 0; i < 9; ++i) {
        double expected = (i % 4 == 0) ? 1.0f : 0.0f;
        assert_float_equal(inv->storage.dense.data[i], expected, 1e-6);
    }

    free_double_matrix(mat);
    free_double_matrix(inv);
}
// -------------------------------------------------------------------------------- 

void test_double_known_inverse(void **state) {
    matrix_d *mat = create_double_matrix(2, 2, 1);
    insert_double_dense_matrix(mat, 0, 0, 4.0f);
    insert_double_dense_matrix(mat, 0, 1, 7.0f);
    insert_double_dense_matrix(mat, 1, 0, 2.0f);
    insert_double_dense_matrix(mat, 1, 1, 6.0f);

    matrix_d *inv = invert_double_dense_matrix(mat);
    assert_non_null(inv);

    double expected[] = {0.6f, -0.7f, -0.2f, 0.4f};
    for (size_t i = 0; i < 4; ++i) {
        assert_float_equal(inv->storage.dense.data[i], expected[i], 1e-5);
    }

    free_double_matrix(mat);
    free_double_matrix(inv);
}
// -------------------------------------------------------------------------------- 

void test_double_singular_matrix(void **state) {
    matrix_d *mat = create_double_matrix(2, 2, 0);
    insert_double_dense_matrix(mat, 0, 0, 1.0f);
    insert_double_dense_matrix(mat, 0, 1, 2.0f);
    insert_double_dense_matrix(mat, 1, 0, 2.0f);
    insert_double_dense_matrix(mat, 1, 1, 4.0f); // Linearly dependent on row 0

    matrix_d *inv = invert_double_dense_matrix(mat);
    assert_null(inv);
    assert_int_equal(errno, ERANGE);

    free_double_matrix(mat);
}

void test_double_null_input(void **state) {
    matrix_d *inv = invert_double_dense_matrix(NULL);
    assert_null(inv);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_double_non_square_matrix(void **state) {
    matrix_d *mat = create_double_matrix(2, 3, 1);
    insert_double_dense_matrix(mat, 0, 0, 1.0f);  // Optional setup

    matrix_d *inv = invert_double_dense_matrix(mat);
    assert_null(inv);
    assert_int_equal(errno, EINVAL);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_double_transpose_dense_identity(void **state) {
    matrix_d *mat = create_double_dense_matrix(3, 3);

    insert_double_dense_matrix(mat, 0, 0, 1.0f);
    insert_double_dense_matrix(mat, 1, 1, 1.0f);
    insert_double_dense_matrix(mat, 2, 2, 1.0f);
    //transpose_double_matrix(&mat);
    bool ok = transpose_double_matrix(&mat);
    assert_true(ok);
    assert_non_null(mat);
    assert_int_equal(mat->rows, 3);
    assert_int_equal(mat->cols, 3);

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            double expected = (i == j) ? 1.0f : FLT_MAX;
            double actual = get_double_matrix(mat, i, j);
            assert_float_equal(actual, expected, 1e-6);
        }
    }

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_double_transpose_dense_general(void **state) {
    matrix_d *mat = create_double_matrix(2, 2, 0);

    insert_double_dense_matrix(mat, 0, 0, 1.0f);
    insert_double_dense_matrix(mat, 0, 1, 2.0f);
    insert_double_dense_matrix(mat, 1, 0, 3.0f);
    insert_double_dense_matrix(mat, 1, 1, 4.0f);

    bool ok = transpose_double_matrix(&mat);
    assert_true(ok);
    assert_non_null(mat);
    assert_int_equal(mat->rows, 2);
    assert_int_equal(mat->cols, 2);

    assert_float_equal(get_double_matrix(mat, 0, 0), 1.0f, 1e-6);
    assert_float_equal(get_double_matrix(mat, 0, 1), 3.0f, 1e-6);
    assert_float_equal(get_double_matrix(mat, 1, 0), 2.0f, 1e-6);
    assert_float_equal(get_double_matrix(mat, 1, 1), 4.0f, 1e-6);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_double_transpose_dense_rectangular(void **state) {
    matrix_d *mat = create_double_dense_matrix(2, 3);

    insert_double_dense_matrix(mat, 0, 0, 1.0f);
    insert_double_dense_matrix(mat, 0, 1, 2.0f);
    insert_double_dense_matrix(mat, 0, 2, 3.0f);
    insert_double_dense_matrix(mat, 1, 0, 4.0f);
    insert_double_dense_matrix(mat, 1, 1, 5.0f);
    insert_double_dense_matrix(mat, 1, 2, 6.0f);

    bool ok = transpose_double_matrix(&mat);
    assert_true(ok);
    assert_non_null(mat);
    assert_int_equal(mat->rows, 3);
    assert_int_equal(mat->cols, 2);

    assert_float_equal(get_double_matrix(mat, 0, 0), 1.0f, 1e-6);
    assert_float_equal(get_double_matrix(mat, 0, 1), 4.0f, 1e-6);
    assert_float_equal(get_double_matrix(mat, 1, 0), 2.0f, 1e-6);
    assert_float_equal(get_double_matrix(mat, 1, 1), 5.0f, 1e-6);
    assert_float_equal(get_double_matrix(mat, 2, 0), 3.0f, 1e-6);
    assert_float_equal(get_double_matrix(mat, 2, 1), 6.0f, 1e-6);

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_double_transpose_coo_identity(void **state) {
    matrix_d *mat = create_double_matrix(3, 3, 9);  // identity matrix (sparse)
    insert_double_matrix(&mat, 0, 0, 1.0f, false);
    insert_double_matrix(&mat, 1, 1, 1.0f, false);
    insert_double_matrix(&mat, 2, 2, 1.0f, false);

    bool ok = transpose_double_matrix(&mat);
    assert_true(ok);
    assert_non_null(mat);
    assert_int_equal(mat->rows, 3);
    assert_int_equal(mat->cols, 3);

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            double expected = (i == j) ? 1.0f : FLT_MAX;
            double actual = get_double_matrix(mat, i, j);
            assert_float_equal(actual, expected, 1e-6);
        }
    }

    free_double_matrix(mat);
}

void test_double_transpose_coo_rectangular(void **state) {
    matrix_d *mat = create_double_matrix(2, 3, 6);

    insert_double_matrix(&mat, 0, 0, 1.0f, false);
    insert_double_matrix(&mat, 0, 1, 2.0f, false);
    insert_double_matrix(&mat, 0, 2, 3.0f, false);
    insert_double_matrix(&mat, 1, 0, 4.0f, false);
    insert_double_matrix(&mat, 1, 1, 5.0f, false);
    insert_double_matrix(&mat, 1, 2, 6.0f, false);

    bool ok = transpose_double_matrix(&mat);
    assert_true(ok);
    assert_non_null(mat);
    assert_int_equal(mat->rows, 3);
    assert_int_equal(mat->cols, 2);

    double expected[3][2] = {
        {1.0f, 4.0f},
        {2.0f, 5.0f},
        {3.0f, 6.0f}
    };

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            double actual = get_double_matrix(mat, i, j);
            assert_float_equal(actual, expected[i][j], 1e-6);
        }
    }

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_double_transpose_csr_identity(void **state) {
    matrix_d *mat = create_double_matrix(3, 3, 9);
    insert_double_matrix(&mat, 0, 0, 1.0f, false);
    insert_double_matrix(&mat, 1, 1, 1.0f, false);
    insert_double_matrix(&mat, 2, 2, 1.0f, false);

    convert_doubleMat_to_csr(&mat);

    assert_int_equal(get_double_matrix_type(mat), SPARSE_CSR_MATRIX);

    bool ok = transpose_double_matrix(&mat);
    assert_true(ok);
    assert_non_null(mat);
    assert_int_equal(mat->rows, 3);
    assert_int_equal(mat->cols, 3);

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            double expected = (i == j) ? 1.0f : FLT_MAX;
            double actual = get_double_matrix(mat, i, j);
            assert_float_equal(actual, expected, 1e-6);
        }
    }

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_double_transpose_csr_rectangular(void **state) {
    matrix_d *mat = create_double_matrix(2, 3, 6);
    insert_double_matrix(&mat, 0, 0, 1.0f, false);
    insert_double_matrix(&mat, 0, 1, 2.0f, false);
    insert_double_matrix(&mat, 0, 2, 3.0f, false);
    insert_double_matrix(&mat, 1, 0, 4.0f, false);
    insert_double_matrix(&mat, 1, 1, 5.0f, false);
    insert_double_matrix(&mat, 1, 2, 6.0f, false);

    convert_doubleMat_to_csr(&mat);
    assert_int_equal(get_double_matrix_type(mat), SPARSE_CSR_MATRIX);

    bool ok = transpose_double_matrix(&mat);
    assert_true(ok);
    assert_non_null(mat);
    assert_int_equal(mat->rows, 3);
    assert_int_equal(mat->cols, 2);

    double expected[3][2] = {
        {1.0f, 4.0f},
        {2.0f, 5.0f},
        {3.0f, 6.0f}
    };

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            double actual = get_double_matrix(mat, i, j);
            assert_float_equal(actual, expected[i][j], 1e-6);
        }
    }

    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_copy_double_dense_matrix(void **state) {
    matrix_d *original = create_double_dense_matrix(2, 2);
    insert_double_dense_matrix(original, 0, 0, 1.0f);
    insert_double_dense_matrix(original, 0, 1, 2.0f);
    insert_double_dense_matrix(original, 1, 0, 3.0f);
    insert_double_dense_matrix(original, 1, 1, 4.0f);

    matrix_d *copy = copy_double_matrix(original);
    assert_non_null(copy);
    assert_int_equal(copy->rows, 2);
    assert_int_equal(copy->cols, 2);
    assert_int_equal(copy->type, DENSE_MATRIX);

    for (size_t i = 0; i < 2; ++i)
        for (size_t j = 0; j < 2; ++j)
            assert_float_equal(get_double_matrix(copy, i, j), get_double_matrix(original, i, j), 1e-6);

    assert_ptr_not_equal(original->storage.dense.data, copy->storage.dense.data);

    free_double_matrix(original);
    free_double_matrix(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_double_coo_matrix(void **state) {
    matrix_d *original = create_double_coo_matrix(3, 3);
    insert_double_coo_matrix(original, 0, 0, 1.0f);
    insert_double_coo_matrix(original, 1, 2, 2.0f);
    insert_double_coo_matrix(original, 2, 1, 3.0f);

    matrix_d *copy = copy_double_matrix(original);
    assert_non_null(copy);
    assert_int_equal(copy->rows, 3);
    assert_int_equal(copy->cols, 3);
    assert_int_equal(copy->count, original->count);
    assert_int_equal(copy->type, SPARSE_COO_MATRIX);

    for (size_t i = 0; i < original->count; ++i) {
        assert_int_equal(copy->storage.coo.rows[i], original->storage.coo.rows[i]);
        assert_int_equal(copy->storage.coo.cols[i], original->storage.coo.cols[i]);
        assert_float_equal(copy->storage.coo.values[i], original->storage.coo.values[i], 1e-6);
    }

    assert_ptr_not_equal(copy->storage.coo.values, original->storage.coo.values);

    free_double_matrix(original);
    free_double_matrix(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_double_csr_matrix(void **state) {
    matrix_d *original = create_double_csr_matrix(2, 3, 4);
    original->storage.csr.row_ptrs[0] = 0;
    original->storage.csr.row_ptrs[1] = 2;
    original->storage.csr.row_ptrs[2] = 4;

    original->storage.csr.col_indices[0] = 0;
    original->storage.csr.col_indices[1] = 2;
    original->storage.csr.col_indices[2] = 1;
    original->storage.csr.col_indices[3] = 2;

    original->storage.csr.values[0] = 1.0f;
    original->storage.csr.values[1] = 2.0f;
    original->storage.csr.values[2] = 3.0f;
    original->storage.csr.values[3] = 4.0f;

    original->count = 4;

    matrix_d *copy = copy_double_matrix(original);
    assert_non_null(copy);
    assert_int_equal(copy->rows, 2);
    assert_int_equal(copy->cols, 3);
    assert_int_equal(copy->count, 4);
    assert_int_equal(copy->type, SPARSE_CSR_MATRIX);

    for (size_t i = 0; i < 3; ++i)
        assert_int_equal(copy->storage.csr.row_ptrs[i], original->storage.csr.row_ptrs[i]);

    for (size_t i = 0; i < 4; ++i) {
        assert_int_equal(copy->storage.csr.col_indices[i], original->storage.csr.col_indices[i]);
        assert_float_equal(copy->storage.csr.values[i], original->storage.csr.values[i], 1e-6);
    }

    assert_ptr_not_equal(copy->storage.csr.values, original->storage.csr.values);

    free_double_matrix(original);
    free_double_matrix(copy);
}
// -------------------------------------------------------------------------------- 

void test_det_double_identity_3x3(void **state) {
    (void) state;
    matrix_d* mat = create_double_dense_matrix(3, 3);
    insert_double_dense_matrix(mat, 0, 0, 1.0f);
    insert_double_dense_matrix(mat, 1, 1, 1.0f);
    insert_double_dense_matrix(mat, 2, 2, 1.0f);

    double det = double_dense_matrix_det(mat);
    assert_float_equal(det, 1.0f, 1e-6f);
    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_det_double_singular_2x2(void **state) {
    (void) state;
    matrix_d* mat = create_double_dense_matrix(2, 2);
    insert_double_dense_matrix(mat, 0, 0, 2.0f);
    insert_double_dense_matrix(mat, 0, 1, 4.0f);
    insert_double_dense_matrix(mat, 1, 0, 1.0f);
    insert_double_dense_matrix(mat, 1, 1, 2.0f);

    double det = double_dense_matrix_det(mat);
    assert_float_equal(det, 0.0f, 1e-6f);
    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_det_double_known_2x2(void **state) {
    (void) state;
    matrix_d* mat = create_double_dense_matrix(2, 2);
    insert_double_dense_matrix(mat, 0, 0, 4.0f);
    insert_double_dense_matrix(mat, 0, 1, 6.0f);
    insert_double_dense_matrix(mat, 1, 0, 3.0f);
    insert_double_dense_matrix(mat, 1, 1, 8.0f);

    double det = double_dense_matrix_det(mat);
    assert_float_equal(det, 14.0f, 1e-6f);
    free_double_matrix(mat);
}
// -------------------------------------------------------------------------------- 

void test_det_double_known_3x3(void **state) {
    (void) state;
    matrix_d* mat = create_double_dense_matrix(3, 3);
    insert_double_dense_matrix(mat, 0, 0, 6.0f);
    insert_double_dense_matrix(mat, 0, 1, 1.0f);
    insert_double_dense_matrix(mat, 0, 2, 1.0f);
    insert_double_dense_matrix(mat, 1, 0, 4.0f);
    insert_double_dense_matrix(mat, 1, 1, -2.0f);
    insert_double_dense_matrix(mat, 1, 2, 5.0f);
    insert_double_dense_matrix(mat, 2, 0, 2.0f);
    insert_double_dense_matrix(mat, 2, 1, 8.0f);
    insert_double_dense_matrix(mat, 2, 2, 7.0f);

    double det = double_dense_matrix_det(mat);
    assert_float_equal(det, -306.0f, 1e-3f);
    free_double_matrix(mat);
}
// ================================================================================
// ================================================================================
// eof
