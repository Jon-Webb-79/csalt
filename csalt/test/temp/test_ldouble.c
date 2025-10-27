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

#include "test_ldouble.h"
#include "c_ldouble.h"

#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
// ================================================================================ 
// ================================================================================ 

void test_init_ldouble_vector_success(void **state) {
    (void) state; /* unused */
    
    ldouble_v* vec = init_ldouble_vector(10);
    assert_non_null(vec);
    assert_non_null(vec->data);
    assert_int_equal(vec->len, 0);
    assert_int_equal(vec->alloc, 10);
    assert_int_equal(vec->alloc_type, DYNAMIC);
    
    // Check zero initialization
    for (size_t i = 0; i < vec->alloc; i++) {
        assert_float_equal(vec->data[i], 0.0, 0.0001);
    }
    
    free_ldouble_vector(vec);
}
// --------------------------------------------------------------------------------

void test_init_ldouble_vector_zero_size(void **state) {
    (void) state; /* unused */
    
    errno = 0;
    ldouble_v* vec = init_ldouble_vector(0);
    assert_null(vec);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_init_ldouble_array_success(void **state) {
    (void) state; /* unused */
    
    ldouble_v arr = init_ldouble_array(10);
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
void test_free_ldouble_vector_null(void **state) {
    (void) state; /* unused */
    
    errno = 0;
    free_ldouble_vector(NULL);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_free_ldouble_vector_static(void **state) {
    (void) state; /* unused */
    
    ldouble_v arr = init_ldouble_array(10);
    ldouble_v* arr_ptr = &arr;
    
    errno = 0;
    free_ldouble_vector(arr_ptr);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    /* GC Tests */
    void test_ldouble_vector_gbc(void **state) {
        (void) state; /* unused */
    
        {
            LDBLEVEC_GBC ldouble_v* vec = init_ldouble_vector(10);
            assert_non_null(vec);
        assert_non_null(vec->data);
            // Vector will be automatically freed at scope end
        }
    }
#endif
// ================================================================================ 
// ================================================================================

void test_push_back_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(2);
    assert_non_null(vec);
    
    // Test basic push_back
    assert_true(push_back_ldouble_vector(vec, 3.14));
    assert_int_equal(ld_size(vec), 1);
    assert_float_equal(ldouble_vector_index(vec, 0), 3.14, 0.0001);
     
    // Test adding zero
    assert_true(push_back_ldouble_vector(vec, 0.0));
    //assert_int_equal(ld_size(vec), 2);
    // assert_ldouble_equal(ldouble_vector_index(vec, 1), 0.0f, 0.0001f);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_ldouble_growth(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = ld_alloc(vec);
    
    // Fill to capacity
    assert_true(push_back_ldouble_vector(vec, 1.0));
    assert_true(push_back_ldouble_vector(vec, 2.0));
    assert_int_equal(ld_size(vec), 2);
    assert_int_equal(ld_alloc(vec), initial_alloc);
    
    // Trigger growth
    assert_true(push_back_ldouble_vector(vec, 3.0));
    assert_int_equal(ld_size(vec), 3);
    assert_true(ld_alloc(vec) > initial_alloc);
    assert_float_equal(ldouble_vector_index(vec, 2), 3.0, 0.0001);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_ldouble_static(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(2);
    
    // Fill to capacity
    assert_true(push_back_ldouble_vector(&arr, 1.0));
    assert_true(push_back_ldouble_vector(&arr, 2.0));
    
    // Attempt to exceed capacity should fail
    assert_false(push_back_ldouble_vector(&arr, 3.0));
    assert_int_equal(errno, EINVAL);
    assert_int_equal(ld_size(&arr), 2);
}
// -------------------------------------------------------------------------------- 

void test_generic_ldouble_macros(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(2);
    assert_non_null(vec);
    
    // Test ld_size macro
    assert_int_equal(ld_size(vec), 0);
    push_back_ldouble_vector(vec, 1.0);
    assert_int_equal(ld_size(vec), 1);
    
    // Test ld_alloc macro
    assert_int_equal(ld_alloc(vec), 2);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_vector_ldouble_index(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(2);
    assert_non_null(vec);
    
    // Test valid index
    assert_true(push_back_ldouble_vector(vec, 1.234));
    assert_float_equal(ldouble_vector_index(vec, 0), 1.234, 0.0001);
    
    // Test out of bounds index
    errno = 0;
    long double result = ldouble_vector_index(vec, ld_size(vec));  
    assert_int_equal(errno, ERANGE);
    assert_float_equal(result, DBL_MAX, 0.0001);
    
    // Test NULL vector
    errno = 0;
    result = ldouble_vector_index(NULL, 0);
    assert_int_equal(errno, EINVAL);
    assert_float_equal(result, DBL_MAX, 0.0001);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    // Test infinity
    assert_true(push_back_ldouble_vector(vec, (long double)INFINITY));
    assert_true(isinf((double)ldouble_vector_index(vec, 0)));
     
    // Test negative infinity
    assert_true(push_back_ldouble_vector(vec, (long double)-INFINITY));
    assert_true(isinf((double)ldouble_vector_index(vec, 1)));

    // Test NaN
    assert_true(push_back_ldouble_vector(vec, (long double)NAN));
    assert_true(isnan((double)ldouble_vector_index(vec, 2)));
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_static_array_ldouble_bounds(void **state) {
    (void) state;
    
    // Create static array of size 3
    ldouble_v arr = init_ldouble_array(3);
    
    // Test adding up to capacity
    assert_true(push_back_ldouble_vector(&arr, 1.0));
    assert_int_equal(ld_size(&arr), 1);
    assert_true(push_back_ldouble_vector(&arr, 2.0));
    assert_int_equal(ld_size(&arr), 2);
    assert_true(push_back_ldouble_vector(&arr, 3.0));
    assert_int_equal(ld_size(&arr), 3);
    
    // Verify values were stored correctly
    assert_float_equal(ldouble_vector_index(&arr, 0), 1.0, 0.0001);
    assert_float_equal(ldouble_vector_index(&arr, 1), 2.0, 0.0001);
    assert_float_equal(ldouble_vector_index(&arr, 2), 3.0, 0.0001);
    
    // Test attempting to exceed capacity
    errno = 0;
    assert_false(push_back_ldouble_vector(&arr, 4.0));
    assert_int_equal(errno, EINVAL);
    assert_int_equal(ld_size(&arr), 3);
    
    // Verify original data wasn't corrupted
    assert_float_equal(ldouble_vector_index(&arr, 2), 3.0, 0.0001);
}
// --------------------------------------------------------------------------------

void test_static_array_index_ldouble_bounds(void **state) {
    (void) state;
    
    // Create and fill static array
    ldouble_v arr = init_ldouble_array(2);
    push_back_ldouble_vector(&arr, 1.0);
    
    // Test valid index
    assert_float_equal(ldouble_vector_index(&arr, 0), 1.0, 0.0001);
    
    // Test index at unfilled position
    errno = 0;
    long double result = ldouble_vector_index(&arr, 1);
    assert_int_equal(errno, ERANGE);
    assert_float_equal(result, DBL_MAX, 0.0001);
    
    // Test index beyond allocation
    errno = 0;
    result = ldouble_vector_index(&arr, 2);
    assert_int_equal(errno, ERANGE);
    assert_float_equal(result, DBL_MAX, 0.0001);
}
// --------------------------------------------------------------------------------

void test_static_array_ldouble_initialization(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(2);
    
    // Check initial state
    assert_int_equal(ld_size(&arr), 0);
    assert_int_equal(ld_alloc(&arr), 2);
    
    // Verify data array is zeroed
    for(size_t i = 0; i < ld_alloc(&arr); i++) {
        assert_float_equal(arr.data[i], 0.0, 0.0001);  // Direct access just for testing
    }
}
// -------------------------------------------------------------------------------- 

void test_static_array_ldouble_free(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(2);
    push_back_ldouble_vector(&arr, 1.0);
    
    // Attempting to free a static array should fail
    errno = 0;
    free_ldouble_vector(&arr);
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_float_equal(ldouble_vector_index(&arr, 0), 1.0, 0.0001);
}
// ================================================================================ 
// ================================================================================ 

void test_push_front_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(2);
    assert_non_null(vec);
    
    // Test basic push_front
    assert_true(push_front_ldouble_vector(vec, 3.14));
    assert_int_equal(ld_size(vec), 1);
    assert_float_equal(ldouble_vector_index(vec, 0), 3.14, 0.0001);
    
    // Test adding zero
    assert_true(push_front_ldouble_vector(vec, 0.0f));
    assert_int_equal(ld_size(vec), 2);
    assert_float_equal(ldouble_vector_index(vec, 0), 0.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 3.14, 0.0001);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_ldouble_growth(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = ld_alloc(vec);
    
    // Fill to capacity
    assert_true(push_front_ldouble_vector(vec, 1.0));
    assert_true(push_front_ldouble_vector(vec, 2.0));
    assert_int_equal(ld_size(vec), 2);
    assert_int_equal(ld_alloc(vec), initial_alloc);
    
    // Verify order
    assert_float_equal(ldouble_vector_index(vec, 0), 2.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 1.0, 0.0001);
    
    // Trigger growth
    assert_true(push_front_ldouble_vector(vec, 3.0));
    assert_int_equal(ld_size(vec), 3);
    assert_true(ld_alloc(vec) > initial_alloc);
    
    // Verify all elements after growth
    assert_float_equal(ldouble_vector_index(vec, 0), 3.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 2.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 2), 1.0, 0.0001);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_ldouble_static(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(2);
    
    // Test basic push_front with static array
    assert_true(push_front_ldouble_vector(&arr, 1.0));
    assert_true(push_front_ldouble_vector(&arr, 2.0));
    assert_int_equal(ld_size(&arr), 2);
    
    // Verify order
    assert_float_equal(ldouble_vector_index(&arr, 0), 2.0, 0.0001);
    assert_float_equal(ldouble_vector_index(&arr, 1), 1.0, 0.0001);
    
    // Attempt to exceed capacity
    assert_false(push_front_ldouble_vector(&arr, 3.0));
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_int_equal(ld_size(&arr), 2);
    assert_float_equal(ldouble_vector_index(&arr, 0), 2.0, 0.0001);
    assert_float_equal(ldouble_vector_index(&arr, 1), 1.0, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_push_front_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Test infinity
    assert_true(push_front_ldouble_vector(vec, INFINITY));
    assert_true(isinf((double)ldouble_vector_index(vec, 0)));
    
    // Test negative infinity
    assert_true(push_front_ldouble_vector(vec, -INFINITY));
    assert_true(isinf((double)ldouble_vector_index(vec, 0)));
    
    // Test NaN
    assert_true(push_front_ldouble_vector(vec, NAN));
    assert_true(isnan((double)ldouble_vector_index(vec, 0)));
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_error_ldouble_cases(void **state) {
    (void) state;
    
    // Test NULL vector
    assert_false(push_front_ldouble_vector(NULL, 1.0));
    assert_int_equal(errno, EINVAL);
    
    // Test with invalid data pointer
    ldouble_v vec = {0};
    vec.data = NULL;
    assert_false(push_front_ldouble_vector(&vec, 1.0));
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================

void test_insert_vector_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    // Insert into empty vector
    assert_true(insert_ldouble_vector(vec, 1.0, 0));
    assert_int_equal(ld_size(vec), 1);
    assert_float_equal(ldouble_vector_index(vec, 0), 1.0, 0.0001);
    
    // Insert at beginning (shifting right)
    assert_true(insert_ldouble_vector(vec, 0.0, 0));
    assert_int_equal(ld_size(vec), 2);
    assert_float_equal(ldouble_vector_index(vec, 0), 0.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 1.0, 0.0001);
    
    // Insert in middle
    assert_true(insert_ldouble_vector(vec, 0.5, 1));
    assert_int_equal(ld_size(vec), 3);
    assert_float_equal(ldouble_vector_index(vec, 0), 0.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 0.5, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 2), 1.0, 0.0001);
    
    // Insert at end (append)
    assert_true(insert_ldouble_vector(vec, 2.0, 3));
    assert_int_equal(ld_size(vec), 4);
    assert_float_equal(ldouble_vector_index(vec, 3), 2.0, 0.0001);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_vector_ldouble_growth(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = ld_alloc(vec);
    
    // Fill initial capacity
    assert_true(insert_ldouble_vector(vec, 1.0, 0));
    assert_true(insert_ldouble_vector(vec, 2.0, 1));
    assert_int_equal(ld_size(vec), 2);
    assert_int_equal(ld_alloc(vec), initial_alloc);
    
    // Trigger growth
    assert_true(insert_ldouble_vector(vec, 1.5, 1));
    assert_int_equal(ld_size(vec), 3);
    assert_true(ld_alloc(vec) > initial_alloc);
    
    // Verify all elements after growth
    assert_float_equal(ldouble_vector_index(vec, 0), 1.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 1.5, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 2), 2.0, 0.0001);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_array_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(3);
    
    // Test basic insertions
    assert_true(insert_ldouble_vector(&arr, 1.0, 0));
    assert_true(insert_ldouble_vector(&arr, 3.0, 1));
    assert_true(insert_ldouble_vector(&arr, 2.0, 1));
    
    // Verify order
    assert_int_equal(ld_size(&arr), 3);
    assert_float_equal(ldouble_vector_index(&arr, 0), 1.0, 0.0001f);
    assert_float_equal(ldouble_vector_index(&arr, 1), 2.0, 0.0001f);
    assert_float_equal(ldouble_vector_index(&arr, 2), 3.0, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_insert_array_ldouble_bounds(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(2);
    
    // Fill array
    assert_true(insert_ldouble_vector(&arr, 1.0, 0));
    assert_true(insert_ldouble_vector(&arr, 2.0, 1));
    
    // Try to insert beyond capacity
    errno = 0;
    assert_false(insert_ldouble_vector(&arr, 3.0, 1));
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_int_equal(ld_size(&arr), 2);
    assert_float_equal(ldouble_vector_index(&arr, 0), 1.0, 0.0001);
    assert_float_equal(ldouble_vector_index(&arr, 1), 2.0, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_insert_error_ldouble_cases(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(2);
    assert_non_null(vec);
    
    // Test invalid index
    errno = 0;
    assert_false(insert_ldouble_vector(vec, 1.0, 1));  // Empty vector
    assert_int_equal(errno, ERANGE);
    
    // Add one element and test bounds
    assert_true(insert_ldouble_vector(vec, 1.0, 0));
    
    errno = 0;
    assert_false(insert_ldouble_vector(vec, 2.0, 2));  // Beyond length
    assert_int_equal(errno, ERANGE);
    
    // Test NULL vector
    errno = 0;
    assert_false(insert_ldouble_vector(NULL, 1.0, 0));
    assert_int_equal(errno, EINVAL);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Insert infinity
    assert_true(insert_ldouble_vector(vec, INFINITY, 0));
    assert_true(isinf((double)ldouble_vector_index(vec, 0)));
    
    // Insert NaN
    assert_true(insert_ldouble_vector(vec, NAN, 1));
    assert_true(isnan((double)ldouble_vector_index(vec, 1)));
    
    // Insert zero
    assert_true(insert_ldouble_vector(vec, 0.0, 1));
    assert_float_equal(ldouble_vector_index(vec, 1), 0.0, 0.0001);
    
    free_ldouble_vector(vec);
}
// ================================================================================ 
// ================================================================================

void test_pop_back_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Add some test values
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 3.0);
    
    // Test popping values
    errno = 0;
    assert_float_equal(pop_back_ldouble_vector(vec), 3.0, 0.0001);
    assert_int_equal(ld_size(vec), 2);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_back_ldouble_vector(vec), 2.0, 0.0001);
    assert_int_equal(ld_size(vec), 1);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_back_ldouble_vector(vec), 1.0, 0.0001);
    assert_int_equal(ld_size(vec), 0);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_back_ldouble_empty(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(1);
    assert_non_null(vec);
    
    // Try to pop from empty vector
    errno = 0;
    long double result = pop_back_ldouble_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
    
    free_ldouble_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_back_ldouble_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    long double result = pop_back_ldouble_vector(NULL);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    ldouble_v vec = {0};
    vec.data = NULL;
    errno = 0;
    result = pop_back_ldouble_vector(&vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_pop_back_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Test popping DBL_MAX as valid data
    push_back_ldouble_vector(vec, DBL_MAX);
    errno = 0;
    long double result = pop_back_ldouble_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, 0);  // Should be 0 since this is valid data

    // Test popping infinity
    push_back_ldouble_vector(vec, INFINITY);
    errno = 0;
    result = pop_back_ldouble_vector(vec);
    assert_true(isinf((double)result));
    assert_int_equal(errno, 0);

    // Test popping NaN
    push_back_ldouble_vector(vec, NAN);
    errno = 0;
    result = pop_back_ldouble_vector(vec);
    assert_true(isnan((double)result));
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_back_ldouble_static(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(2);
    
    // Add and pop values from static array
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    
    errno = 0;
    assert_float_equal(pop_back_ldouble_vector(&arr), 2.0, 0.0001);
    assert_int_equal(ld_size(&arr), 1);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_back_ldouble_vector(&arr), 1.0, 0.0001);
    assert_int_equal(ld_size(&arr), 0);
    assert_int_equal(errno, 0);
    
    // Try to pop from empty array
    errno = 0;
    long double result = pop_back_ldouble_vector(&arr);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
}
// ================================================================================ 
// ================================================================================ 

void test_pop_front_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Add some test values
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 3.0);
    
    // Test popping values and check remaining elements
    errno = 0;
    assert_float_equal(pop_front_ldouble_vector(vec), 1.0, 0.0001);
    assert_int_equal(ld_size(vec), 2);
    assert_float_equal(ldouble_vector_index(vec, 0), 2.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 3.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_front_ldouble_vector(vec), 2.0, 0.0001);
    assert_int_equal(ld_size(vec), 1);
    assert_float_equal(ldouble_vector_index(vec, 0), 3.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_front_ldouble_vector(vec), 3.0, 0.0001);
    assert_int_equal(ld_size(vec), 0);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_ldouble_empty(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(1);
    assert_non_null(vec);
    
    // Try to pop from empty vector
    errno = 0;
    long double result = pop_front_ldouble_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_ldouble_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    long double result = pop_front_ldouble_vector(NULL);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    ldouble_v vec = {0};
    vec.data = NULL;
    errno = 0;
    result = pop_front_ldouble_vector(&vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Test popping DBL_MAX as valid data
    push_back_ldouble_vector(vec, DBL_MAX);
    push_back_ldouble_vector(vec, 1.0);
    errno = 0;
    long double result = pop_front_ldouble_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, 0);  // Should be 0 since this is valid data
    assert_float_equal(ldouble_vector_index(vec, 0), 1.0, 0.0001);
    
    // Test popping infinity
    push_front_ldouble_vector(vec, INFINITY);
    errno = 0;
    result = pop_front_ldouble_vector(vec);
    assert_true(isinf((double)result));
    assert_int_equal(errno, 0);
    
    // Test popping NaN
    push_front_ldouble_vector(vec, NAN);
    errno = 0;
    result = pop_front_ldouble_vector(vec);
    assert_true(isnan((double)result));
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_ldouble_static(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(2);
    
    // Add and pop values from static array
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    
    errno = 0;
    assert_float_equal(pop_front_ldouble_vector(&arr), 1.0, 0.0001);
    assert_int_equal(ld_size(&arr), 1);
    assert_float_equal(ldouble_vector_index(&arr, 0), 2.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_front_ldouble_vector(&arr), 2.0, 0.0001);
    assert_int_equal(ld_size(&arr), 0);
    assert_int_equal(errno, 0);
    
    // Try to pop from empty array
    errno = 0;
    long double result = pop_front_ldouble_vector(&arr);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
}
// ================================================================================ 
// ================================================================================ 

void test_pop_any_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    // Add test values
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 3.0);
    push_back_ldouble_vector(vec, 4.0);
    
    // Test popping from middle
    errno = 0;
    assert_float_equal(pop_any_ldouble_vector(vec, 1), 2.0, 0.0001);
    assert_int_equal(ld_size(vec), 3);
    assert_float_equal(ldouble_vector_index(vec, 0), 1.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 3.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 2), 4.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test popping first element
    assert_float_equal(pop_any_ldouble_vector(vec, 0), 1.0, 0.0001);
    assert_int_equal(ld_size(vec), 2);
    assert_float_equal(ldouble_vector_index(vec, 0), 3.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 4.0, 0.0001);
    
    // Test popping last element
    assert_float_equal(pop_any_ldouble_vector(vec, 1), 4.0, 0.0001);
    assert_int_equal(ld_size(vec), 1);
    assert_float_equal(ldouble_vector_index(vec, 0), 3.0, 0.0001);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_ldouble_errors(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(2);
    assert_non_null(vec);
    push_back_ldouble_vector(vec, 1.0);
    
    // Test NULL vector
    errno = 0;
    long double result = pop_any_ldouble_vector(NULL, 0);
    assert_float_equal(result, DBL_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid index
    errno = 0;
    result = pop_any_ldouble_vector(vec, 1);  // Index equals length
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ERANGE);
    
    errno = 0;
    result = pop_any_ldouble_vector(vec, 2);  // Index beyond length
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ERANGE);
    
    // Test empty vector
    pop_any_ldouble_vector(vec, 0);  // Remove the only element
    errno = 0;
    result = pop_any_ldouble_vector(vec, 0);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_ldouble_static(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(3);
    
    // Fill array
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    push_back_ldouble_vector(&arr, 3.0);
    
    // Pop from middle
    errno = 0;
    long double result = pop_any_ldouble_vector(&arr, 1);
    assert_float_equal(result, 2.0, 0.0001);
    assert_int_equal(ld_size(&arr), 2);
    assert_float_equal(ldouble_vector_index(&arr, 0), 1.0, 0.0001);
    assert_float_equal(ldouble_vector_index(&arr, 1), 3.0, 0.0001);
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Test with DBL_MAX as valid data
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, DBL_MAX);
    push_back_ldouble_vector(vec, 3.0);
     
    errno = 0;
    long double result = pop_any_ldouble_vector(vec, 1);
    assert_float_equal(result, DBL_MAX, 0.0001);
    assert_int_equal(errno, 0);  // Should be 0 since this is valid data
     
    // Test with NaN
    push_back_ldouble_vector(vec, NAN);
    errno = 0;
    result = pop_any_ldouble_vector(vec, 2);
    assert_true(isnan(result));
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// ================================================================================ 
// ================================================================================

void test_reverse_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    // Test single element
    push_back_ldouble_vector(vec, 1.0);
    reverse_ldouble_vector(vec);
    assert_int_equal(ld_size(vec), 1);
    assert_float_equal(ldouble_vector_index(vec, 0), 1.0, 0.0001);
    
    // Test even number of elements
    push_back_ldouble_vector(vec, 2.0);
    reverse_ldouble_vector(vec);
    assert_int_equal(ld_size(vec), 2);
    assert_float_equal(ldouble_vector_index(vec, 0), 2.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 1.0, 0.0001);
    
    // Test odd number of elements
    push_back_ldouble_vector(vec, 3.0);
    reverse_ldouble_vector(vec);
    assert_int_equal(ld_size(vec), 3);
    assert_float_equal(ldouble_vector_index(vec, 0), 3.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 1.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 2), 2.0, 0.0001);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_reverse_ldouble_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    reverse_ldouble_vector(NULL);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    ldouble_v vec = {0};
    vec.data = NULL;
    errno = 0;
    reverse_ldouble_vector(&vec);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    ldouble_v* empty_vec = init_ldouble_vector(1);
    assert_non_null(empty_vec);
    errno = 0;
    reverse_ldouble_vector(empty_vec);
    assert_int_equal(errno, ENODATA);
    
    free_ldouble_vector(empty_vec);
}
// -------------------------------------------------------------------------------- 

void test_reverse_ldouble_static(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(3);
    
    // Add test values
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    push_back_ldouble_vector(&arr, 3.0);
    
    // Test reversal
    reverse_ldouble_vector(&arr);
    assert_int_equal(ld_size(&arr), 3);
    assert_float_equal(ldouble_vector_index(&arr, 0), 3.0, 0.0001);
    assert_float_equal(ldouble_vector_index(&arr, 1), 2.0, 0.0001);
    assert_float_equal(ldouble_vector_index(&arr, 2), 1.0, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_reverse_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Add special values
    push_back_ldouble_vector(vec, INFINITY);
    push_back_ldouble_vector(vec, NAN);
    push_back_ldouble_vector(vec, -INFINITY);
    
    // Test reversal
    reverse_ldouble_vector(vec);
    
    // Verify reversed order
    assert_true(isinf((double)ldouble_vector_index(vec, 0)) && ldouble_vector_index(vec, 0) < 0.0);  // -INFINITY
    assert_true(isnan((double)ldouble_vector_index(vec, 1)));  // NAN
    assert_true(isinf((double)ldouble_vector_index(vec, 2)) && ldouble_vector_index(vec, 2) > 0.0);  // INFINITY
    
    free_ldouble_vector(vec);
}
// ================================================================================
// ================================================================================ 

void test_sort_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(5);
    assert_non_null(vec);
    
    // Test ascending sort
    push_back_ldouble_vector(vec, 5.0);
    push_back_ldouble_vector(vec, 3.0);
    push_back_ldouble_vector(vec, 4.0);
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    
    sort_ldouble_vector(vec, FORWARD);
    
    // Verify ascending order
    for (size_t i = 0; i < ld_size(vec) - 1; i++) {
        assert_true(ldouble_vector_index(vec, i) <= ldouble_vector_index(vec, i + 1));
    }
    
    // Test descending sort
    sort_ldouble_vector(vec, REVERSE);
    
    // Verify descending order
    for (size_t i = 0; i < ld_size(vec) - 1; i++) {
        assert_true(ldouble_vector_index(vec, i) >= ldouble_vector_index(vec, i + 1));
    }
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_edge_ldouble_cases(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(10);
    assert_non_null(vec);
    
    // Test empty vector
    sort_ldouble_vector(vec, FORWARD);
    assert_int_equal(ld_size(vec), 0);
    
    // Test single element
    push_back_ldouble_vector(vec, 1.0);
    sort_ldouble_vector(vec, FORWARD);
    assert_int_equal(ld_size(vec), 1);
    assert_float_equal(ldouble_vector_index(vec, 0), 1.0, 0.0001);
    
    // Test two elements
    push_back_ldouble_vector(vec, 0.0);
    sort_ldouble_vector(vec, FORWARD);
    assert_float_equal(ldouble_vector_index(vec, 0), 0.0, 0.0001);
    assert_float_equal(ldouble_vector_index(vec, 1), 1.0, 0.0001);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_ldouble_duplicates(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(5);
    assert_non_null(vec);
    
    // Add duplicate values
    push_back_ldouble_vector(vec, 3.0);
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 3.0);
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    
    sort_ldouble_vector(vec, FORWARD);
    
    // Verify order with duplicates
    for (size_t i = 0; i < ld_size(vec) - 1; i++) {
        assert_true(ldouble_vector_index(vec, i) <= ldouble_vector_index(vec, i + 1));
    }
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(5);
    assert_non_null(vec);
    
    // Add special values
    push_back_ldouble_vector(vec, INFINITY);
    push_back_ldouble_vector(vec, -INFINITY);
    push_back_ldouble_vector(vec, NAN);
    push_back_ldouble_vector(vec, 0.0);
    
    sort_ldouble_vector(vec, FORWARD);
    
    // Verify NaN handling and infinity ordering
    size_t last_valid_index = ld_size(vec) - 1;
    while (last_valid_index > 0 && isnan((double)ldouble_vector_index(vec, last_valid_index))) {
        last_valid_index--;
    }
     
    // Check that -INFINITY is first and INFINITY is last (excluding NaN)
    assert_true(isinf((double)ldouble_vector_index(vec, 0)) && ldouble_vector_index(vec, 0) < 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_static_ldouble_array(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(5);
    
    // Add unsorted values
    push_back_ldouble_vector(&arr, 5.0);
    push_back_ldouble_vector(&arr, 3.0);
    push_back_ldouble_vector(&arr, 4.0);
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    
    sort_ldouble_vector(&arr, FORWARD);
    
    // Verify ascending order
    for (size_t i = 0; i < ld_size(&arr) - 1; i++) {
        assert_true(ldouble_vector_index(&arr, i) <= ldouble_vector_index(&arr, i + 1));
    }
}
// -------------------------------------------------------------------------------- 

void test_sort_ldouble_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    sort_ldouble_vector(NULL, FORWARD);
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================ 

void test_trim_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(10);  // Start with capacity of 10
    assert_non_null(vec);
    
    // Add 5 elements
    for (long double i = 0; i < 5; i++) {
        push_back_ldouble_vector(vec, i);
    }
    
    size_t original_alloc = ld_alloc(vec);
    assert_int_equal(original_alloc, 10);
    assert_int_equal(ld_size(vec), 5);
    
    // Trim the vector
    errno = 0;
    trim_ldouble_vector(vec);
    assert_int_equal(errno, 0);
    
    // Verify capacity matches size
    assert_int_equal(ld_alloc(vec), 5);
    assert_int_equal(ld_size(vec), 5);
    
    // Verify data is intact
    for (size_t i = 0; i < ld_size(vec); i++) {
        assert_float_equal(ldouble_vector_index(vec, i), (long double)i, 0.0001);
    }
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_empty_ldouble_vector(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(5);  // Capacity of 5, but empty
    assert_non_null(vec);
    
    size_t original_alloc = ld_alloc(vec);
    assert_int_equal(original_alloc, 5);
    assert_int_equal(ld_size(vec), 0);
     
    // Trim the empty vector
    errno = 0;
    trim_ldouble_vector(vec);
    assert_int_equal(errno, ENODATA);

    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_static_ldouble_array(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(5);
    
    // Add some elements
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    
    size_t original_alloc = ld_alloc(&arr);
    size_t original_size = ld_size(&arr);
    
    // Try to trim static array
    errno = 0;
    trim_ldouble_vector(&arr);
    assert_int_equal(errno, 0);  // Should not set error
    
    // Verify nothing changed
    assert_int_equal(ld_alloc(&arr), original_alloc);
    assert_int_equal(ld_size(&arr), original_size);
}
// -------------------------------------------------------------------------------- 

void test_trim_already_ldouble_optimal(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Fill to capacity
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 3.0);
    
    assert_int_equal(ld_alloc(vec), 3);
    assert_int_equal(ld_size(vec), 3);
    
    // Try to trim an already optimal vector
    errno = 0;
    trim_ldouble_vector(vec);
    assert_int_equal(errno, 0);
    
    // Verify nothing changed
    assert_int_equal(ld_alloc(vec), 3);
    assert_int_equal(ld_size(vec), 3);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_ldouble_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    trim_ldouble_vector(NULL);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    ldouble_v vec = {0};
    vec.data = NULL;
    errno = 0;
    trim_ldouble_vector(&vec);
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================ 

void test_binary_search_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(5);
    assert_non_null(vec);
    
    // Add values in order
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 3.0);
    push_back_ldouble_vector(vec, 4.0);
    push_back_ldouble_vector(vec, 5.0);
    
    // Test exact matches
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(vec, 1.0, 0.0001, false), 0);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(vec, 3.0, 0.0001, false), 2);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(vec, 5.0, 0.0001, false), 4);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_ldouble_tolerance(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 3.0);
    
    // Test with tolerance
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(vec, 1.1, 0.2, false), 0);  // Should find 1.0
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(vec, 2.95, 0.1, false), 2);  // Should find 3.0
    assert_int_equal(errno, 0);
    
    // Test value outside tolerance
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(vec, 2.5, 0.1, false), LONG_MAX);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_with_ldouble_sort(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(5);
    assert_non_null(vec);
    
    // Add values out of order
    push_back_ldouble_vector(vec, 5.0f);
    push_back_ldouble_vector(vec, 3.0f);
    push_back_ldouble_vector(vec, 1.0f);
    push_back_ldouble_vector(vec, 4.0f);
    push_back_ldouble_vector(vec, 2.0f);
    
    // Search with sort_first = true
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(vec, 3.0, 0.0001, true), 2);
    assert_int_equal(errno, 0);
    
    // Verify the vector is now sorted
    for (size_t i = 0; i < ld_size(vec) - 1; i++) {
        assert_true(ldouble_vector_index(vec, i) <= ldouble_vector_index(vec, i + 1));
    }
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_ldouble_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(NULL, 1.0, 0.0001, false), LONG_MAX);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    ldouble_v* vec = init_ldouble_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(vec, 1.0, 0.0001, false), LONG_MAX);
    assert_int_equal(errno, ENODATA);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_ldouble_static(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(5);
    
    // Add values
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    push_back_ldouble_vector(&arr, 3.0);
    push_back_ldouble_vector(&arr, 4.0);
    push_back_ldouble_vector(&arr, 5.0);
    
    // Test search in static array
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(&arr, 3.0, 0.0001, false), 2);
    assert_int_equal(errno, 0);
    
    // Test value not found
    errno = 0;
    assert_int_equal(binary_search_ldouble_vector(&arr, 6.0, 0.0001, false), LONG_MAX);
    assert_int_equal(errno, 0);
}
// ================================================================================ 
// ================================================================================ 

void test_update_ldouble_vector_nominal(void **state) {
    (void) state;
    ldouble_v arr = init_ldouble_array(5);
    
    // Add values
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    push_back_ldouble_vector(&arr, 3.0);
    push_back_ldouble_vector(&arr, 4.0);
    push_back_ldouble_vector(&arr, 5.0);

    update_ldouble_vector(&arr, 2, 12.0);
    assert_float_equal(12.0, ldouble_vector_index(&arr, 2), 1.0e-6);
}
// -------------------------------------------------------------------------------- 

void test_update_ldouble_vector_null(void **state) {
    errno = 0;
    update_ldouble_vector(NULL, 3, 8.2);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_update_ldouble_vector_bad_index(void **state) {
    (void) state;
    ldouble_v* vec = init_ldouble_vector(3);
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 3.0);
    errno = 0;
    update_ldouble_vector(vec, 5, 3.0);
    assert_int_equal(errno, ERANGE);
    free_ldouble_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_min_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(5);
    assert_non_null(vec);
    
    // Test single element
    push_back_ldouble_vector(vec, 1.0);
    errno = 0;
    assert_float_equal(min_ldouble_vector(vec), 1.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test multiple elements
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, -3.0);
    push_back_ldouble_vector(vec, 4.0);
    push_back_ldouble_vector(vec, 0.0);
    
    errno = 0;
    assert_float_equal(min_ldouble_vector(vec), -3.0, 0.0001);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_max_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(5);
    assert_non_null(vec);
    
    // Test single element
    push_back_ldouble_vector(vec, 1.0);
    errno = 0;
    assert_float_equal(max_ldouble_vector(vec), 1.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test multiple elements
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, -3.0);
    push_back_ldouble_vector(vec, 4.0);
    push_back_ldouble_vector(vec, 0.0);
    
    errno = 0;
    assert_float_equal(max_ldouble_vector(vec), 4.0, 0.0001);  // This will fail with current implementation
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_min_max_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    // Test with infinity
    push_back_ldouble_vector(vec, INFINITY);
    push_back_ldouble_vector(vec, -INFINITY);
    push_back_ldouble_vector(vec, 1.0);
    
    errno = 0;
    assert_true(isinf((double)min_ldouble_vector(vec)) && min_ldouble_vector(vec) < 0);  // Should be -INFINITY
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_true(isinf((double)max_ldouble_vector(vec)) && max_ldouble_vector(vec) > 0);  // Should be INFINITY
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_min_max_static_ldouble_array(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(3);
    
    push_back_ldouble_vector(&arr, 3.0);
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    
    errno = 0;
    assert_float_equal(min_ldouble_vector(&arr), 1.0f, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(max_ldouble_vector(&arr), 3.0, 0.0001);  // This will fail with current implementation
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_min_max_ldouble_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_float_equal(min_ldouble_vector(NULL), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(max_ldouble_vector(NULL), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    ldouble_v* vec = init_ldouble_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_float_equal(min_ldouble_vector(vec), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(max_ldouble_vector(vec), DBL_MAX, 0.0001);
    assert_int_equal(errno, EINVAL);
    
    free_ldouble_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_sum_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    // Test single value
    push_back_ldouble_vector(vec, 1.0);
    errno = 0;
    assert_float_equal(sum_ldouble_vector(vec), 1.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test multiple values
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 3.0);
    push_back_ldouble_vector(vec, 4.0);
    
    errno = 0;
    assert_float_equal(sum_ldouble_vector(vec), 10.0, 0.0001);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_average_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    // Test single value
    push_back_ldouble_vector(vec, 2.0);
    errno = 0;
    assert_float_equal(average_ldouble_vector(vec), 2.0, 0.0001);
    assert_int_equal(errno, 0);
    
    // Test multiple values
    push_back_ldouble_vector(vec, 4.0);
    push_back_ldouble_vector(vec, 6.0);
    push_back_ldouble_vector(vec, 8.0);
    
    errno = 0;
    assert_float_equal(average_ldouble_vector(vec), 5.0, 0.0001);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_special_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // Test with infinity
    push_back_ldouble_vector(vec, INFINITY);
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    
    errno = 0;
    assert_true(isinf((double)sum_ldouble_vector(vec)));
    assert_int_equal(errno, 0);

    errno = 0;
    assert_true(isinf((double)average_ldouble_vector(vec)));
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_ldouble_negative(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    push_back_ldouble_vector(vec, -1.0);
    push_back_ldouble_vector(vec, -2.0);
    push_back_ldouble_vector(vec, 5.0);
    push_back_ldouble_vector(vec, 2.0);
    
    errno = 0;
    assert_float_equal(sum_ldouble_vector(vec), 4.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(average_ldouble_vector(vec), 1.0, 0.0001);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_ldouble_static(void **state) {
    (void) state;
    
    ldouble_v arr = init_ldouble_array(3);
    
    push_back_ldouble_vector(&arr, 1.0);
    push_back_ldouble_vector(&arr, 2.0);
    push_back_ldouble_vector(&arr, 3.0);
    
    errno = 0;
    assert_float_equal(sum_ldouble_vector(&arr), 6.0, 0.0001);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(average_ldouble_vector(&arr), 2.0, 0.0001);
    assert_int_equal(errno, 0);
}
// ================================================================================ 
// ================================================================================ 

void test_stdev_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    // Dataset with known standard deviation
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 4.0);
    push_back_ldouble_vector(vec, 4.0);
    push_back_ldouble_vector(vec, 6.0);
    
    // Mean = 4.0, variance = 2.0, stdev = sqrt(2.0)
    errno = 0;
    assert_float_equal(stdev_ldouble_vector(vec), sqrt(2.0), 0.0001);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_stdev_single_ldouble_value(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(1);
    assert_non_null(vec);
    
    push_back_ldouble_vector(vec, 2.0);
    
    errno = 0;
    long double result = stdev_ldouble_vector(vec);
    assert_float_equal(result, DBL_MAX, 0.0001);  // Standard deviation of single value is 0
    assert_int_equal(errno, ENODATA);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_stdev_same_ldouble_values(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(3);
    assert_non_null(vec);
    
    // All same values should give stdev of 0
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 2.0);
    
    errno = 0;
    assert_float_equal(stdev_ldouble_vector(vec), 0.0f, 0.0001);
    assert_int_equal(errno, 0);
    
    free_ldouble_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_cum_sum_ldouble_basic(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, 2.0);
    push_back_ldouble_vector(vec, 3.0);
    push_back_ldouble_vector(vec, 4.0);
    
    ldouble_v* cum_sum = cum_sum_ldouble_vector(vec);
    assert_non_null(cum_sum);
    assert_int_equal(ld_size(cum_sum), 4);
    
    // Check cumulative sums: 1, 3, 6, 10
    assert_float_equal(ldouble_vector_index(cum_sum, 0), 1.0, 0.0001);
    assert_float_equal(ldouble_vector_index(cum_sum, 1), 3.0, 0.0001);
    assert_float_equal(ldouble_vector_index(cum_sum, 2), 6.0, 0.0001);
    assert_float_equal(ldouble_vector_index(cum_sum, 3), 10.0, 0.0001);
    
    free_ldouble_vector(vec);
    free_ldouble_vector(cum_sum);
}
// -------------------------------------------------------------------------------- 

void test_cum_sum_ldouble_negative(void **state) {
    (void) state;
    
    ldouble_v* vec = init_ldouble_vector(4);
    assert_non_null(vec);
    
    push_back_ldouble_vector(vec, 1.0);
    push_back_ldouble_vector(vec, -2.0);
    push_back_ldouble_vector(vec, 3.0);
    push_back_ldouble_vector(vec, -4.0);
    
    ldouble_v* cum_sum = cum_sum_ldouble_vector(vec);
    assert_non_null(cum_sum);
    
    // Check cumulative sums: 1, -1, 2, -2
    assert_float_equal(ldouble_vector_index(cum_sum, 0), 1.0, 0.0001);
    assert_float_equal(ldouble_vector_index(cum_sum, 1), -1.0, 0.0001);
    assert_float_equal(ldouble_vector_index(cum_sum, 2), 2.0, 0.0001);
    assert_float_equal(ldouble_vector_index(cum_sum, 3), -2.0, 0.0001);
    
    free_ldouble_vector(vec);
    free_ldouble_vector(cum_sum);
}
// -------------------------------------------------------------------------------- 

void test_stdev_cum_sum_ldouble_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_float_equal(stdev_ldouble_vector(NULL), DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);
    
    assert_null(cum_sum_ldouble_vector(NULL));
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    ldouble_v* vec = init_ldouble_vector(1);
    assert_non_null(vec);

    errno = 0;
    assert_float_equal(stdev_ldouble_vector(vec), DBL_MAX, 0.0001);
    assert_int_equal(errno, ENODATA);

    assert_null(cum_sum_ldouble_vector(vec));
    assert_int_equal(errno, EINVAL);
     
    free_ldouble_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

/* Setup and teardown functions */
static dict_ld* test_dict = NULL;

int setup_ldouble(void** state) {
    test_dict = init_ldouble_dict();
    if (!test_dict) return -1;
    *state = test_dict;
    return 0;
}
// -------------------------------------------------------------------------------- 

int teardown_ldouble(void** state) {
    free_ldouble_dict(test_dict);
    return 0;
}
// -------------------------------------------------------------------------------- 

/* Test initialization */
void test_init_ldouble_dict(void** state) {
    dict_ld* dict = init_ldouble_dict();
    assert_non_null(dict);
    assert_int_equal(ldouble_dict_hash_size(dict), 0);
    assert_int_equal(ldouble_dict_size(dict), 0);
    assert_true(ldouble_dict_alloc(dict) > 0);
    free_ldouble_dict(dict);
}
// -------------------------------------------------------------------------------- 

/* Test insertion */
void test_insert_ldouble_dict_basic(void** state) {
    dict_ld* dict = *state;
    
    assert_true(insert_ldouble_dict(dict, "test", 1.0));
    assert_int_equal(ldouble_dict_hash_size(dict), 1);
    assert_int_equal(ldouble_dict_size(dict), 1);
    
    long double value = get_ldouble_dict_value(dict, "test");
    assert_float_equal(value, 1.0f, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_insert_ldouble_dict_duplicate(void** state) {
    dict_ld* dict = *state;
    
    assert_true(insert_ldouble_dict(dict, "test", 1.0));
    assert_false(insert_ldouble_dict(dict, "test", 2.0));
    
    long double value = get_ldouble_dict_value(dict, "test");
    assert_float_equal(value, 1.0, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_insert_ldouble_dict_null(void** state) {
    dict_ld* dict = *state;
    
    assert_false(insert_ldouble_dict(NULL, "test", 1.0));
    assert_false(insert_ldouble_dict(dict, NULL, 1.0));
}
// -------------------------------------------------------------------------------- 

/* Test retrieval */
void test_get_ldouble_dict_value_basic(void** state) {
    dict_ld* dict = *state;
    
    insert_ldouble_dict(dict, "key1", 1.5);
    long double value = get_ldouble_dict_value(dict, "key1");
    assert_float_equal(value, 1.5, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_get_ldouble_dict_value_missing(void** state) {
    dict_ld* dict = *state;
    
    long double value = get_ldouble_dict_value(dict, "nonexistent");
    assert_float_equal(value, LDBL_MAX, 0.0001);
}
// -------------------------------------------------------------------------------- 

/* Test update */
void test_update_ldouble_dict_basic(void** state) {
    dict_ld* dict = *state;
    
    insert_ldouble_dict(dict, "key1", 1.0);
    assert_true(update_ldouble_dict(dict, "key1", 2.0));
    
    long double value = get_ldouble_dict_value(dict, "key1");
    assert_float_equal(value, 2.0, 0.0001);
}
// -------------------------------------------------------------------------------- 

void test_update_ldouble_dict_missing(void** state) {
    dict_ld* dict = *state;
    
    assert_false(update_ldouble_dict(dict, "nonexistent", 1.0));
}
// -------------------------------------------------------------------------------- 

/* Test removal */
void test_pop_ldouble_dict_basic(void** state) {
    dict_ld* dict = *state;
    
    insert_ldouble_dict(dict, "key1", 1.5);
    long double value = pop_ldouble_dict(dict, "key1");
    assert_float_equal(value, 1.5, 0.0001);
    assert_int_equal(ldouble_dict_hash_size(dict), 0);
}
// -------------------------------------------------------------------------------- 

void test_pop_ldouble_dict_missing(void** state) {
    dict_ld* dict = *state;
    
    long double value = pop_ldouble_dict(dict, "nonexistent");
    assert_float_equal(value, LDBL_MAX, 0.0001);
}
// -------------------------------------------------------------------------------- 

/* Test resize behavior */
void test_resize_ldouble_behavior(void** state) {
    dict_ld* dict = *state;
    char key[20];
    size_t initial_alloc = ldouble_dict_alloc(dict);
    
    // Insert enough items to trigger resize
    for (int i = 0; i < 100; i++) {
        sprintf(key, "key%d", i);
        assert_true(insert_ldouble_dict(dict, key, (long double)i));
    }
    
    assert_true(ldouble_dict_alloc(dict) > initial_alloc);
}
// -------------------------------------------------------------------------------- 

/* Test key/value extraction */
void test_get_keys_ldouble_dict(void** state) {
    dict_ld* dict = *state;
    
    insert_ldouble_dict(dict, "key1", 1.0);
    insert_ldouble_dict(dict, "key2", 2.0);
    
    string_v* keys = get_keys_ldouble_dict(dict);
    assert_non_null(keys);
    assert_int_equal(ldouble_dict_hash_size(dict), 2);
    
    free_str_vector(keys);
}
// -------------------------------------------------------------------------------- 

void test_get_values_ldouble_dict(void** state) {
    dict_ld* dict = *state;
    
    insert_ldouble_dict(dict, "key1", 1.0);
    insert_ldouble_dict(dict, "key2", 2.0);
    
    ldouble_v* values = get_values_ldouble_dict(dict);
    assert_non_null(values);
    assert_int_equal(ldouble_dict_hash_size(dict), 2);
    
    free_ldouble_vector(values);
}
// -------------------------------------------------------------------------------- 

/* Helper function for testing iterator */
static void count_entries(const char* key, long double value, void* user_data) {
    size_t* count = (size_t*)user_data;
    (*count)++;
}

/* Helper function to sum values */
static void sum_values(const char* key, long double value, void* user_data) {
    long double* sum = (long double*)user_data;
    *sum += value;
}
// -------------------------------------------------------------------------------- 

void test_foreach_ldouble_dict_basic(void** state) {
    dict_ld* dict = *state;
    size_t count = 0;
    
    // Add some test data
    assert_true(insert_ldouble_dict(dict, "key1", 1.0));
    assert_true(insert_ldouble_dict(dict, "key2", 2.0));
    assert_true(insert_ldouble_dict(dict, "key3", 3.0));
    
    // Test iteration count
    assert_true(foreach_ldouble_dict(dict, count_entries, &count));
    assert_int_equal(count, 3);
    assert_int_equal(count, ldouble_dict_hash_size(dict));
    
    // Test value sum
    long double sum = 0.0;
    assert_true(foreach_ldouble_dict(dict, sum_values, &sum));
    assert_float_equal(sum, 6.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_foreach_ldouble_dict_empty(void** state) {
    dict_ld* dict = *state;
    size_t count = 0;
    
    assert_true(foreach_ldouble_dict(dict, count_entries, &count));
    assert_int_equal(count, 0);
}
// -------------------------------------------------------------------------------- 

void test_foreach_ldouble_dict_null(void** state) {
    dict_ld* dict = *state;
    size_t count = 0;
    
    assert_false(foreach_ldouble_dict(NULL, count_entries, &count));
    assert_false(foreach_ldouble_dict(dict, NULL, &count));
}
// ================================================================================ 
// ================================================================================ 

void test_vector_ldouble_dictionary(void **state) {
    dict_ldv* dict = init_ldoublev_dict();
    bool result;
    result = create_ldoublev_dict(dict, "one", 3);
    assert_true(result);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 1.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 2.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 3.0);

    long double test_one[3] = {1.0, 2.0, 3.0};
    ldouble_v* vec1 = return_ldoublev_pointer(dict, "one");
    for (size_t i = 0; i < ldouble_vector_size(vec1); i++) {
        assert_float_equal(ldouble_vector_index(vec1, i), test_one[i], 1.0e-3);
    }

    result = create_ldoublev_dict(dict, "two", 3);
    assert_true(result);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "two"), 4.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "two"), 5.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "two"), 6.0);

    long double test_two[3] = {4.0, 5.0, 6.0};
    ldouble_v* vec2 = return_ldoublev_pointer(dict, "two");
    for (size_t i = 0; i < ldouble_vector_size(vec2); i++) {
        assert_float_equal(ldouble_vector_index(vec2, i), test_two[i], 1.0e-3);
    }

    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_vector_dictionary_ldouble_resize(void **state) {
    dict_ldv* dict = init_ldoublev_dict();
    bool result;
    result = create_ldoublev_dict(dict, "one", 3);
    assert_true(result);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 1.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 2.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 3.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 4.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 5.0);

    long double test_one[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    ldouble_v* vec1 = return_ldoublev_pointer(dict, "one");
    for (size_t i = 0; i < ldouble_vector_size(vec1); i++) {
        assert_float_equal(ldouble_vector_index(vec1, i), test_one[i], 1.0e-3);
    }
    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_vector_dictionary_ldouble_gbc(void **state) {
        LDDICTV_GBC dict_ldv* dict = init_ldoublev_dict();
        bool result;
        result = create_ldoublev_dict(dict, "one", 3);
        assert_true(result);
        push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 1.0);
        push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 2.0);
        push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 3.0);
        push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 4.0);
        push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 5.0);

        long double test_one[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
        ldouble_v* vec1 = return_ldoublev_pointer(dict, "one");
        for (size_t i = 0; i < ldouble_vector_size(vec1); i++) {
            assert_float_equal(ldouble_vector_index(vec1, i), test_one[i], 1.0e-3);
        }
    }
#endif
// --------------------------------------------------------------------------------

void test_pop_vector_ldouble_dictionary(void **state) {
    dict_ldv* dict = init_ldoublev_dict();
    bool result;
    result = create_ldoublev_dict(dict, "one", 3);
    assert_true(result);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 1.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 2.0);

    long double test_one[3] = {1.0, 2.0, 3.0};
    ldouble_v* vec1 = return_ldoublev_pointer(dict, "one");
    for (size_t i = 0; i < ldouble_vector_size(vec1); i++) {
        assert_float_equal(ldouble_vector_index(vec1, i), test_one[i], 1.0e-3);
    }

    result = create_ldoublev_dict(dict, "two", 3);
    assert_true(result);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "two"), 4.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "two"), 5.0);

    long double test_two[3] = {4.0, 5.0, 6.0};
    ldouble_v* vec2 = return_ldoublev_pointer(dict, "two");
    for (size_t i = 0; i < ldouble_vector_size(vec2); i++) {
        assert_float_equal(ldouble_vector_index(vec2, i), test_two[i], 1.0e-3);
    }

    pop_ldoublev_dict(dict, "two");
    assert_false(has_key_ldoublev_dict(dict, "two"));
    assert_true(has_key_ldoublev_dict(dict, "one"));

    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_insert_ldoublev_dict_basic(void **state) {
    (void)state;  // unused

    dict_ldv* dict = init_ldoublev_dict();
    assert_non_null(dict);

    // Valid DYNAMIC vector
    ldouble_v* vec1 = init_ldouble_vector(3);
    assert_non_null(vec1);
    assert_int_equal(vec1->alloc_type, DYNAMIC);

    bool result = insert_ldoublev_dict(dict, "key1", vec1);
    assert_true(result);

    // Confirm key exists
    assert_true(has_key_ldoublev_dict(dict, "key1"));

    // Try inserting same key again — should fail
    ldouble_v* vec2 = init_ldouble_vector(2);
    assert_non_null(vec2);
    errno = 0;
    result = insert_ldoublev_dict(dict, "key1", vec2);
    assert_false(result);
    assert_int_equal(errno, EEXIST);
    free_ldouble_vector(vec2);  // Manual cleanup for rejected value

    // Try inserting STATIC vector — should fail
    ldouble_v vec3 = init_ldouble_array(2);  // This is heap-wrapped, but STATIC
    assert_int_equal(vec3.alloc_type, STATIC);
    errno = 0;
    result = insert_ldoublev_dict(dict, "key_static", &vec3);
    assert_false(result);
    assert_int_equal(errno, EPERM);

    // NULL input tests
    errno = 0;
    assert_false(insert_ldoublev_dict(NULL, "key", vec1));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(insert_ldoublev_dict(dict, NULL, vec1));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(insert_ldoublev_dict(dict, "keyX", NULL));
    assert_int_equal(errno, EINVAL);

    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_ldoublev_size_macros(void **state) {
    dict_ldv* dict = init_ldoublev_dict();
    bool result;
    result = create_ldoublev_dict(dict, "one", 3);
    assert_true(result);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 1.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 2.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "one"), 3.0);

    long double test_one[3] = {1.0, 2.0, 3.0};
    ldouble_v* vec1 = return_ldoublev_pointer(dict, "one");
    for (size_t i = 0; i < ldouble_vector_size(vec1); i++) {
        assert_float_equal(ldouble_vector_index(vec1, i), test_one[i], 1.0e-3);
    }

    result = create_ldoublev_dict(dict, "two", 3);
    assert_true(result);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "two"), 4.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "two"), 5.0);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "two"), 6.0);

    long double test_two[3] = {4.0, 5.0, 6.0};
    ldouble_v* vec2 = return_ldoublev_pointer(dict, "two");
    for (size_t i = 0; i < ldouble_vector_size(vec2); i++) {
        assert_float_equal(ldouble_vector_index(vec2, i), test_two[i], 1.0e-3);
    }
    assert_int_equal(16, ld_alloc(dict));
    assert_int_equal(2, ld_size(dict));
    assert_int_equal(2, ldouble_dictv_hash_size(dict));

    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_copy_ldoublev_dict_success(void **state) {
    (void)state;

    dict_ldv* original = init_ldoublev_dict();
    ldouble_v* v1 = init_ldouble_vector(3);
    push_back_ldouble_vector(v1, 1.0f);
    push_back_ldouble_vector(v1, 2.0f);
    insert_ldoublev_dict(original, "alpha", v1);

    dict_ldv* copy = copy_ldoublev_dict(original);
    assert_non_null(copy);
    assert_true(has_key_ldoublev_dict(copy, "alpha"));

    ldouble_v* copied = return_ldoublev_pointer(copy, "alpha");
    assert_non_null(copied);
    assert_float_equal(ldouble_vector_index(copied, 0), 1.0f, 1e-6);
    assert_float_equal(ldouble_vector_index(copied, 1), 2.0f, 1e-6);

    // Ensure deep copy (modifying copy doesn't affect original)
    push_back_ldouble_vector(copied, 999.0f);
    assert_int_not_equal(ldouble_vector_size(copied), ldouble_vector_size(v1));

    free_ldoublev_dict(original);
    free_ldoublev_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_ldoublev_dict_null_input(void **state) {
    (void)state;
    errno = 0;
    dict_ldv* copy = copy_ldoublev_dict(NULL);
    assert_null(copy);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_copy_ldoublev_dict_static_vector(void **state) {
    (void)state;

    dict_ldv* dict = init_ldoublev_dict();
    ldouble_v vec = init_ldouble_array(2);
    push_back_ldouble_vector(&vec, 42.0f);
    insert_ldoublev_dict(dict, "badkey", &vec);  // Insert unsafe manually
    
    errno = 0;
    dict_ldv* copy = copy_ldoublev_dict(dict);
    // assert_null(copy);
    // assert_int_equal(errno, EPERM);
    //
    free_ldoublev_dict(dict);
    free_ldoublev_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_ldoublev_dict_multiple_entries(void **state) {
    (void)state;

    dict_ldv* dict = init_ldoublev_dict();
    ldouble_v* v1 = init_ldouble_vector(2);
    ldouble_v* v2 = init_ldouble_vector(2);
    push_back_ldouble_vector(v1, 1.0);
    push_back_ldouble_vector(v2, 2.0);
    insert_ldoublev_dict(dict, "a", v1);
    insert_ldoublev_dict(dict, "b", v2);

    dict_ldv* copy = copy_ldoublev_dict(dict);
    assert_non_null(copy);
    assert_true(has_key_ldoublev_dict(copy, "a"));
    assert_true(has_key_ldoublev_dict(copy, "b"));

    free_ldoublev_dict(dict);
    free_ldoublev_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_ldoublev_dict_independence(void **state) {
    (void)state;

    dict_ldv* dict = init_ldoublev_dict();
    ldouble_v* v = init_ldouble_vector(1);
    push_back_ldouble_vector(v, 10.0f);
    insert_ldoublev_dict(dict, "x", v);

    dict_ldv* copy = copy_ldoublev_dict(dict);
    ldouble_v* original = return_ldoublev_pointer(dict, "x");
    ldouble_v* copied = return_ldoublev_pointer(copy, "x");

    push_back_ldouble_vector(copied, 20.0f);
    assert_int_not_equal(ldouble_vector_size(original), ldouble_vector_size(copied));

    free_ldoublev_dict(dict);
    free_ldoublev_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_merge_ldoublev_dict_no_overwrite(void **state) {
    (void)state;
    dict_ldv* dict1 = init_ldoublev_dict();
    dict_ldv* dict2 = init_ldoublev_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    ldouble_v* vec1 = init_ldouble_vector(1); push_back_ldouble_vector(vec1, 1.0f);
    ldouble_v* vec2 = init_ldouble_vector(1); push_back_ldouble_vector(vec2, 2.0f);
    ldouble_v* vec3 = init_ldouble_vector(1); push_back_ldouble_vector(vec3, 3.0f);

    insert_ldoublev_dict(dict1, "alpha", vec1);
    insert_ldoublev_dict(dict1, "beta", vec2);
    insert_ldoublev_dict(dict2, "beta", vec3);  // conflict
    insert_ldoublev_dict(dict2, "gamma", init_ldouble_vector(1));

    dict_ldv* merged = merge_ldoublev_dict(dict1, dict2, false);
    assert_non_null(merged);

    ldouble_v* merged_beta = return_ldoublev_pointer(merged, "beta");
    assert_float_equal(ldouble_vector_index(merged_beta, 0), 2.0f, 1e-6);  // from dict1

    free_ldoublev_dict(dict1);
    free_ldoublev_dict(dict2);
    free_ldoublev_dict(merged);
}
// -------------------------------------------------------------------------------- 

void test_merge_ldoublev_dict_overwrite(void **state) {
    (void)state;

    dict_ldv* dict1 = init_ldoublev_dict();
    dict_ldv* dict2 = init_ldoublev_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    ldouble_v* vec1 = init_ldouble_vector(1); push_back_ldouble_vector(vec1, 1.0f);
    ldouble_v* vec2 = init_ldouble_vector(1); push_back_ldouble_vector(vec2, 2.0f);
    ldouble_v* vec3 = init_ldouble_vector(1); push_back_ldouble_vector(vec3, 99.0f);

    insert_ldoublev_dict(dict1, "beta", vec2);
    insert_ldoublev_dict(dict2, "beta", vec3);  // conflict
    insert_ldoublev_dict(dict1, "alpha", vec1);

    dict_ldv* merged = merge_ldoublev_dict(dict1, dict2, true);
    assert_non_null(merged);

    ldouble_v* overwritten_beta = return_ldoublev_pointer(merged, "beta");
    assert_float_equal(ldouble_vector_index(overwritten_beta, 0), 99.0f, 1e-6);  // from dict2

    free_ldoublev_dict(dict1);
    free_ldoublev_dict(dict2);
    free_ldoublev_dict(merged);
}
// -------------------------------------------------------------------------------- 

void test_merge_ldoublev_dict_reject_static(void **state) {
    (void)state;

    dict_ldv* dict1 = init_ldoublev_dict();
    dict_ldv* dict2 = init_ldoublev_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    ldouble_v* vec1 = init_ldouble_vector(1); push_back_ldouble_vector(vec1, 1.0f);
    insert_ldoublev_dict(dict1, "alpha", vec1);

    ldouble_v vec_static = init_ldouble_array(2);  // stack + static
    push_back_ldouble_vector(&vec_static, 42.0f);
    insert_ldoublev_dict(dict2, "static_key", &vec_static);

    errno = 0;
    dict_ldv* merged = merge_ldoublev_dict(dict1, dict2, true);

    free_ldoublev_dict(dict1);
    free_ldoublev_dict(dict2);
    free_ldoublev_dict(merged);
}
// -------------------------------------------------------------------------------- 

void test_merge_ldoublev_dict_null_inputs(void **state) {
    (void)state;

    dict_ldv* valid = init_ldoublev_dict();
    assert_non_null(valid);

    errno = 0;
    assert_null(merge_ldoublev_dict(NULL, valid, true));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_null(merge_ldoublev_dict(valid, NULL, false));
    assert_int_equal(errno, EINVAL);

    free_ldoublev_dict(valid);
}
// -------------------------------------------------------------------------------- 

void test_merge_ldoublev_dict_unique_keys(void **state) {
    (void)state;

    dict_ldv* dict1 = init_ldoublev_dict();
    dict_ldv* dict2 = init_ldoublev_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    // Create unique ldouble_v* instances for each dictionary
    ldouble_v* vx = init_ldouble_vector(1); push_back_ldouble_vector(vx, 1.0f);
    ldouble_v* vy = init_ldouble_vector(1); push_back_ldouble_vector(vy, 2.0f);
    ldouble_v* vz = init_ldouble_vector(1); push_back_ldouble_vector(vz, 3.0f);

    insert_ldoublev_dict(dict1, "x", vx);
    insert_ldoublev_dict(dict2, "y", vy);
    insert_ldoublev_dict(dict2, "z", vz);

    dict_ldv* merged = merge_ldoublev_dict(dict1, dict2, false);
    assert_non_null(merged);
    assert_true(has_key_ldoublev_dict(merged, "x"));
    assert_true(has_key_ldoublev_dict(merged, "y"));
    assert_true(has_key_ldoublev_dict(merged, "z"));

    // Cleanup only from merged dict
    free_ldoublev_dict(merged);
    free_ldoublev_dict(dict1);
    free_ldoublev_dict(dict2);

    // Don't ldouble-free dict1 and dict2 because their values were reused in merged
    // Alternative: clone data during merge to make them independent (deep copy)
}
// -------------------------------------------------------------------------------- 

void test_clear_ldoublev_dict_basic(void **state) {
    (void)state;

    dict_ldv* dict = init_ldoublev_dict();
    assert_non_null(dict);

    create_ldoublev_dict(dict, "a", 3);
    create_ldoublev_dict(dict, "b", 2);

    assert_true(has_key_ldoublev_dict(dict, "a"));
    assert_true(has_key_ldoublev_dict(dict, "b"));
    assert_int_equal(ldouble_dictv_size(dict), 2);
    assert_int_equal(ldouble_dictv_hash_size(dict), 2);

    clear_ldoublev_dict(dict);

    assert_false(has_key_ldoublev_dict(dict, "a"));
    assert_false(has_key_ldoublev_dict(dict, "b"));
    assert_int_equal(ldouble_dictv_size(dict), 0);
    assert_int_equal(ldouble_dictv_hash_size(dict), 0);

    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_clear_ldoublev_dict_empty(void **state) {
    (void)state;

    dict_ldv* dict = init_ldoublev_dict();
    assert_non_null(dict);

    clear_ldoublev_dict(dict);  // Should not crash
    assert_int_equal(ldouble_dictv_size(dict), 0);
    assert_int_equal(ldouble_dictv_hash_size(dict), 0);

    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_clear_ldoublev_dict_reuse_after_clear(void **state) {
    (void)state;

    dict_ldv* dict = init_ldoublev_dict();
    assert_non_null(dict);

    create_ldoublev_dict(dict, "temp1", 2);
    create_ldoublev_dict(dict, "temp2", 2);
    clear_ldoublev_dict(dict);

    // Insert again after clear
    create_ldoublev_dict(dict, "new", 2);
    assert_true(has_key_ldoublev_dict(dict, "new"));
    assert_int_equal(ldouble_dictv_hash_size(dict), 1);

    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

static void key_counter(const char* key, const ldouble_v* value, void* user_data) {
    (void)key; (void)value;
    int* counter = (int*)user_data;
    (*counter)++;
}

void test_foreach_ldoublev_dict_counts_keys(void **state) {
    (void)state;

    dict_ldv* dict = init_ldoublev_dict();
    create_ldoublev_dict(dict, "A", 2);
    create_ldoublev_dict(dict, "B", 3);
    create_ldoublev_dict(dict, "C", 1);

    int count = 0;
    bool result = foreach_ldoublev_dict(dict, key_counter, &count);

    assert_true(result);
    assert_int_equal(count, 3);

    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_foreach_ldoublev_dict_with_null_dict(void **state) {
    (void)state;
    errno = 0;

    bool result = foreach_ldoublev_dict(NULL, key_counter, NULL);
    assert_false(result);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_foreach_ldoublev_dict_with_null_callback(void **state) {
    (void)state;

    dict_ldv* dict = init_ldoublev_dict();
    create_ldoublev_dict(dict, "temp", 1);

    errno = 0;
    bool result = foreach_ldoublev_dict(dict, NULL, NULL);

    assert_false(result);
    assert_int_equal(errno, EINVAL);

    free_ldoublev_dict(dict);
}
// -------------------------------------------------------------------------------- 

typedef struct {
    long double sum;
    size_t count;
} accumulator;

static void accumulate_values(const char* key, const ldouble_v* vec, void* user_data) {
    (void)key;
    accumulator* acc = (accumulator*)user_data;
    for (size_t i = 0; i < ldouble_vector_size(vec); ++i) {
        acc->sum += ldouble_vector_index(vec, i);
        acc->count++;
    }
}

void test_foreach_ldoublev_dict_accumulates_sum(void **state) {
    (void)state;

    dict_ldv* dict = init_ldoublev_dict();
    create_ldoublev_dict(dict, "sensor1", 3);
    create_ldoublev_dict(dict, "sensor2", 2);

    push_back_ldouble_vector(return_ldoublev_pointer(dict, "sensor1"), 1.0f);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "sensor1"), 2.0f);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "sensor1"), 3.0f);

    push_back_ldouble_vector(return_ldoublev_pointer(dict, "sensor2"), 4.0f);
    push_back_ldouble_vector(return_ldoublev_pointer(dict, "sensor2"), 5.0f);

    accumulator acc = {0.0f, 0};
    bool result = foreach_ldoublev_dict(dict, accumulate_values, &acc);

    assert_true(result);
    assert_int_equal(acc.count, 5);
    assert_float_equal(acc.sum, 15.0f, 0.0001);

    free_ldoublev_dict(dict);
}
// ================================================================================ 
// ================================================================================ 

static inline ldouble_v make_vec(long double *buf, size_t n) {
    ldouble_v v; v.data = buf; v.len = n; return v;
}

static inline void assert_bounds(bin_dat bd, size_t lower, size_t upper) {
    assert_int_equal(bd.lower, lower);
    assert_int_equal(bd.upper, upper);
}
// -------------------------------------------------------------------------------- 

void test_ldouble_bin_bounds_exact_middle(void **state) {
    (void)state;
    long double a[] = {1.f, 2.f, 3.f, 4.f};
    ldouble_v v = make_vec(a, 4);
    errno = 0;
    bin_dat bd = binary_search_bounds_ldouble_vector(&v, 3., 0., false);
    assert_int_equal(errno, 0);
    assert_bounds(bd, 2, 2);
}
// -------------------------------------------------------------------------------- 

void test_ldouble_bin_bounds_exact_first(void **state) {
    (void)state;
    long double a[] = {1.f, 2.f, 3.f, 4.f};
    ldouble_v v = make_vec(a, 4);
    bin_dat bd = binary_search_bounds_ldouble_vector(&v, 1.f, 0.f, false);
    assert_bounds(bd, 0, 0);
}
// -------------------------------------------------------------------------------- 

void test_ldouble_bin_bounds_exact_last(void **state) {
    (void)state;
    long double a[] = {1.f, 2.f, 3.f, 4.f};
    ldouble_v v = make_vec(a, 4);
    bin_dat bd = binary_search_bounds_ldouble_vector(&v, 4.f, 0.f, false);
    assert_bounds(bd, 3, 3);
}
// -------------------------------------------------------------------------------- 

void test_ldouble_bin_bounds_near_hit_within_tol(void **state) {
    (void)state;
    long double a[] = {1.f, 2.f, 3.f, 4.f};
    ldouble_v v = make_vec(a, 4);
    bin_dat bd = binary_search_bounds_ldouble_vector(&v, 3.0004f, 0.001f, false);
    assert_bounds(bd, 2, 2);
}
// -------------------------------------------------------------------------------- 

void test_ldouble_bin_bounds_near_miss_outside_tol(void **state) {
    (void)state;
    long double a[] = {1.f, 2.f, 3.f, 4.f};
    ldouble_v v = make_vec(a, 4);
    bin_dat bd = binary_search_bounds_ldouble_vector(&v, 3.002f, 0.001f, false);
    assert_bounds(bd, 2, 3);
}
// -------------------------------------------------------------------------------- 

void test_ldouble_bin_bounds_inrange_miss(void **state) {
    (void)state;
    long double a[] = {1.f, 2.f, 3.f, 4.f};
    ldouble_v v = make_vec(a, 4);
    bin_dat bd = binary_search_bounds_ldouble_vector(&v, 2.5f, 0.f, false);
    assert_bounds(bd, 1, 2);
}
// -------------------------------------------------------------------------------- 

void test_ldouble_bin_bounds_below_range(void **state) {
    (void)state;
    long double a[] = {1.f, 2.f, 3.f, 4.f};
    ldouble_v v = make_vec(a, 4);
    bin_dat bd = binary_search_bounds_ldouble_vector(&v, -10.f, 0.f, false);
    assert_bounds(bd, SIZE_MAX, 0);
}
// -------------------------------------------------------------------------------- 

void test_ldouble_bin_bounds_above_range(void **state) {
    (void)state;
    long double a[] = {1.f, 2.f, 3.f, 4.f};
    ldouble_v v = make_vec(a, 4);
    bin_dat bd = binary_search_bounds_ldouble_vector(&v, 10.f, 0.f, false);
    assert_bounds(bd, 3, SIZE_MAX);
}
// ================================================================================
// ================================================================================
// eof
