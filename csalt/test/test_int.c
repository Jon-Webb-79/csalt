// ================================================================================
// ================================================================================
// - File:    test_int.c
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

#include "test_int.h"
#include "c_int.h"

#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
// ================================================================================ 
// ================================================================================ 

void test_init_int_vector_success(void **state) {
    (void) state; /* unused */
    
    int_v* vec = init_int_vector(10);
    assert_non_null(vec);
    assert_non_null(vec->data);
    assert_int_equal(vec->len, 0);
    assert_int_equal(vec->alloc, 10);
    assert_int_equal(vec->alloc_type, DYNAMIC);
    
    // Check zero initialization
    for (size_t i = 0; i < vec->alloc; i++) {
        assert_int_equal(vec->data[i], 0.0);
    }
    
    free_int_vector(vec);
}
// --------------------------------------------------------------------------------

void test_init_int_vector_zero_size(void **state) {
    (void) state; /* unused */
    
    errno = 0;
    int_v* vec = init_int_vector(0);
    assert_null(vec);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_init_int_array_success(void **state) {
    (void) state; /* unused */
    
    int_v arr = init_int_array(10);
    assert_non_null(arr.data);
    assert_int_equal(arr.len, 0);
    assert_int_equal(arr.alloc, 10);
    assert_int_equal(arr.alloc_type, STATIC);
    
    // Check zero initialization
    for (size_t i = 0; i < arr.alloc; i++) {
        assert_int_equal(arr.data[i], 0.0);
    }
}
// -------------------------------------------------------------------------------- 

void test_free_int_vector_null(void **state) {
    (void) state; /* unused */
    
    errno = 0;
    free_int_vector(NULL);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_free_int_vector_static(void **state) {
    (void) state; /* unused */
    
    int_v arr = init_int_array(10);
    int_v* arr_ptr = &arr;
    
    errno = 0;
    free_int_vector(arr_ptr);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
/* GC Tests */
void test_int_vector_gbc(void **state) {
    (void) state; /* unused */
    
    {
        INTVEC_GBC int_v* vec = init_int_vector(10);
        assert_non_null(vec);
        assert_non_null(vec->data);
        // Vector will be automatically freed at scope end
    }
}
#endif
// ================================================================================ 
// ================================================================================

void test_push_back_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(2);
    assert_non_null(vec);
    
    // Test basic push_back
    assert_true(push_back_int_vector(vec, 3));
    assert_int_equal(i_size(vec), 1);
    assert_int_equal(int_vector_index(vec, 0), 3);
     
    // Test adding zero
    assert_true(push_back_int_vector(vec, 0));
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_int_growth(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = i_alloc(vec);
    
    // Fill to capacity
    assert_true(push_back_int_vector(vec, 1));
    assert_true(push_back_int_vector(vec, 2));
    assert_int_equal(i_size(vec), 2);
    assert_int_equal(i_alloc(vec), initial_alloc);
    
    // Trigger growth
    assert_true(push_back_int_vector(vec, 3));
    assert_int_equal(i_size(vec), 3);
    assert_true(i_alloc(vec) > initial_alloc);
    assert_int_equal(int_vector_index(vec, 2), 3);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_int_static(void **state) {
    (void) state;
    
    int_v arr = init_int_array(2);
    
    // Fill to capacity
    assert_true(push_back_int_vector(&arr, 1));
    assert_true(push_back_int_vector(&arr, 2));
    
    // Attempt to exceed capacity should fail
    assert_false(push_back_int_vector(&arr, 3));
    assert_int_equal(errno, EINVAL);
    assert_int_equal(i_size(&arr), 2);
}
// -------------------------------------------------------------------------------- 

void test_generic_int_macros(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(2);
    assert_non_null(vec);
    
    // Test i_size macro
    assert_int_equal(i_size(vec), 0);
    push_back_int_vector(vec, 1.);
    assert_int_equal(i_size(vec), 1);
    
    // Test i_alloc macro
    assert_int_equal(i_alloc(vec), 2);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_int_vector_index(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(2);
    assert_non_null(vec);
    
    // Test valid index
    assert_true(push_back_int_vector(vec, 1));
    assert_int_equal(int_vector_index(vec, 0), 1);
    
    // Test out of bounds index
    errno = 0;
    int result = int_vector_index(vec, i_size(vec));  
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, INT_MAX);
    
    // Test NULL vector
    errno = 0;
    result = int_vector_index(NULL, 0);
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, INT_MAX);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_int_static_array_bounds(void **state) {
    (void) state;
    
    // Create static array of size 3
    int_v arr = init_int_array(3);
    
    // Test adding up to capacity
    assert_true(push_back_int_vector(&arr, 1));
    assert_int_equal(i_size(&arr), 1);
    assert_true(push_back_int_vector(&arr, 2));
    assert_int_equal(i_size(&arr), 2);
    assert_true(push_back_int_vector(&arr, 3));
    assert_int_equal(i_size(&arr), 3);
    
    // Verify values were stored correctly
    assert_int_equal(int_vector_index(&arr, 0), 1);
    assert_int_equal(int_vector_index(&arr, 1), 2);
    assert_int_equal(int_vector_index(&arr, 2), 3);
    
    // Test attempting to exceed capacity
    errno = 0;
    assert_false(push_back_int_vector(&arr, 4));
    assert_int_equal(errno, EINVAL);
    assert_int_equal(i_size(&arr), 3);
    
    // Verify original data wasn't corrupted
    assert_int_equal(int_vector_index(&arr, 2), 3);
}
// --------------------------------------------------------------------------------

void test_int_static_array_index_bounds(void **state) {
    (void) state;
    
    // Create and fill static array
    int_v arr = init_int_array(2);
    push_back_int_vector(&arr, 1);
    
    // Test valid index
    assert_int_equal(int_vector_index(&arr, 0), 1);
    
    // Test index at unfilled position
    errno = 0;
    int result = int_vector_index(&arr, 1);
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, INT_MAX);
    
    // Test index beyond allocation
    errno = 0;
    result = int_vector_index(&arr, 2);
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, INT_MAX);
}
// --------------------------------------------------------------------------------

void test_int_static_array_initialization(void **state) {
    (void) state;
    
    int_v arr = init_int_array(2);
    
    // Check initial state
    assert_int_equal(i_size(&arr), 0);
    assert_int_equal(i_alloc(&arr), 2);
    
    // Verify data array is zeroed
    for(size_t i = 0; i < i_alloc(&arr); i++) {
        assert_int_equal(arr.data[i], 0);
    }
}
// -------------------------------------------------------------------------------- 

void test_int_static_array_free(void **state) {
    (void) state;
    
    int_v arr = init_int_array(2);
    push_back_int_vector(&arr, 1);
    
    // Attempting to free a static array should fail
    errno = 0;
    free_int_vector(&arr);
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_int_equal(int_vector_index(&arr, 0), 1);
}
// ================================================================================ 
// ================================================================================ 

void test_push_int_front_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(2);
    assert_non_null(vec);
    
    // Test basic push_front
    assert_true(push_front_int_vector(vec, 3));
    assert_int_equal(i_size(vec), 1);
    assert_int_equal(int_vector_index(vec, 0), 3);
    
    // Test adding zero
    assert_true(push_front_int_vector(vec, 0));
    assert_int_equal(i_size(vec), 2);
    assert_int_equal(int_vector_index(vec, 0), 0);
    assert_int_equal(int_vector_index(vec, 1), 3);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_int_front_growth(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = i_alloc(vec);
    
    // Fill to capacity
    assert_true(push_front_int_vector(vec, 1));
    assert_true(push_front_int_vector(vec, 2));
    assert_int_equal(i_size(vec), 2);
    assert_int_equal(i_alloc(vec), initial_alloc);
    
    // Verify order
    assert_int_equal(int_vector_index(vec, 0), 2);
    assert_int_equal(int_vector_index(vec, 1), 1);
    
    // Trigger growth
    assert_true(push_front_int_vector(vec, 3));
    assert_int_equal(i_size(vec), 3);
    assert_true(i_alloc(vec) > initial_alloc);
    
    // Verify all elements after growth
    assert_int_equal(int_vector_index(vec, 0), 3);
    assert_int_equal(int_vector_index(vec, 1), 2);
    assert_int_equal(int_vector_index(vec, 2), 1);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_int_front_static(void **state) {
    (void) state;
    
    int_v arr = init_int_array(2);
    
    // Test basic push_front with static array
    assert_true(push_front_int_vector(&arr, 1));
    assert_true(push_front_int_vector(&arr, 2));
    assert_int_equal(i_size(&arr), 2);
    
    // Verify order
    assert_int_equal(int_vector_index(&arr, 0), 2);
    assert_int_equal(int_vector_index(&arr, 1), 1);
    
    // Attempt to exceed capacity
    assert_false(push_front_int_vector(&arr, 3));
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_int_equal(i_size(&arr), 2);
    assert_int_equal(int_vector_index(&arr, 0), 2);
    assert_int_equal(int_vector_index(&arr, 1), 1);
}
// -------------------------------------------------------------------------------- 

void test_push_int_front_error_cases(void **state) {
    (void) state;
    
    // Test NULL vector
    assert_false(push_front_int_vector(NULL, 1));
    assert_int_equal(errno, EINVAL);
    
    // Test with invalid data pointer
    int_v vec = {0};
    vec.data = NULL;
    assert_false(push_front_int_vector(&vec, 1));
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================

void test_insert_int_vector_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(4);
    assert_non_null(vec);
    
    // Insert into empty vector
    assert_true(insert_int_vector(vec, 1, 0));
    assert_int_equal(i_size(vec), 1);
    assert_int_equal(int_vector_index(vec, 0), 1);
    
    // Insert at beginning (shifting right)
    assert_true(insert_int_vector(vec, 0, 0));
    assert_int_equal(i_size(vec), 2);
    assert_int_equal(int_vector_index(vec, 0), 0);
    assert_int_equal(int_vector_index(vec, 1), 1);
    
    // Insert in middle
    assert_true(insert_int_vector(vec, 5, 1));
    assert_int_equal(i_size(vec), 3);
    assert_int_equal(int_vector_index(vec, 0), 0);
    assert_int_equal(int_vector_index(vec, 1), 5);
    assert_int_equal(int_vector_index(vec, 2), 1);
    
    // Insert at end (append)
    assert_true(insert_int_vector(vec, 2, 3));
    assert_int_equal(i_size(vec), 4);
    assert_int_equal(int_vector_index(vec, 3), 2);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_int_vector_growth(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = i_alloc(vec);
    
    // Fill initial capacity
    assert_true(insert_int_vector(vec, 1, 0));
    assert_true(insert_int_vector(vec, 2, 1));
    assert_int_equal(i_size(vec), 2);
    assert_int_equal(i_alloc(vec), initial_alloc);
    
    // Trigger growth
    assert_true(insert_int_vector(vec, 1, 1));
    assert_int_equal(i_size(vec), 3);
    assert_true(i_alloc(vec) > initial_alloc);
    
    // Verify all elements after growth
    assert_int_equal(int_vector_index(vec, 0), 1);
    assert_int_equal(int_vector_index(vec, 1), 1);
    assert_int_equal(int_vector_index(vec, 2), 2);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_int_array_basic(void **state) {
    (void) state;
    
    int_v arr = init_int_array(3);
    
    // Test basic insertions
    assert_true(insert_int_vector(&arr, 1, 0));
    assert_true(insert_int_vector(&arr, 3, 1));
    assert_true(insert_int_vector(&arr, 2, 1));
    
    // Verify order
    assert_int_equal(i_size(&arr), 3);
    assert_int_equal(int_vector_index(&arr, 0), 1);
    assert_int_equal(int_vector_index(&arr, 1), 2);
    assert_int_equal(int_vector_index(&arr, 2), 3);
}
// -------------------------------------------------------------------------------- 

void test_insert_int_array_bounds(void **state) {
    (void) state;
    
    int_v arr = init_int_array(2);
    
    // Fill array
    assert_true(insert_int_vector(&arr, 1, 0));
    assert_true(insert_int_vector(&arr, 2, 1));
    
    // Try to insert beyond capacity
    errno = 0;
    assert_false(insert_int_vector(&arr, 3, 1));
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_int_equal(i_size(&arr), 2);
    assert_int_equal(int_vector_index(&arr, 0), 1);
    assert_int_equal(int_vector_index(&arr, 1), 2);
}
// -------------------------------------------------------------------------------- 

void test_insert_int_error_cases(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(2);
    assert_non_null(vec);
    
    // Test invalid index
    errno = 0;
    assert_false(insert_int_vector(vec, 1, 1));  // Empty vector
    assert_int_equal(errno, ERANGE);
    
    // Add one element and test bounds
    assert_true(insert_int_vector(vec, 1, 0));
    
    errno = 0;
    assert_false(insert_int_vector(vec, 2, 2));  // Beyond length
    assert_int_equal(errno, ERANGE);
    
    // Test NULL vector
    errno = 0;
    assert_false(insert_int_vector(NULL, 1, 0));
    assert_int_equal(errno, EINVAL);
    
    free_int_vector(vec);
}
// ================================================================================ 
// ================================================================================

void test_pop_back_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(3);
    assert_non_null(vec);
    
    // Add some test values
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 3);
    
    // Test popping values
    errno = 0;
    assert_int_equal(pop_back_int_vector(vec), 3);
    assert_int_equal(i_size(vec), 2);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(pop_back_int_vector(vec), 2);
    assert_int_equal(i_size(vec), 1);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(pop_back_int_vector(vec), 1);
    assert_int_equal(i_size(vec), 0);
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_back_int_empty(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(1);
    assert_non_null(vec);
    
    // Try to pop from empty vector
    errno = 0;
    int result = pop_back_int_vector(vec);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, ENODATA);
    
    free_int_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_back_int_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    int result = pop_back_int_vector(NULL);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    int_v vec = {0};
    vec.data = NULL;
    errno = 0;
    result = pop_back_int_vector(&vec);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_pop_back_int_static(void **state) {
    (void) state;
    
    int_v arr = init_int_array(2);
    
    // Add and pop values from static array
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    
    errno = 0;
    assert_int_equal(pop_back_int_vector(&arr), 2);
    assert_int_equal(i_size(&arr), 1);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(pop_back_int_vector(&arr), 1);
    assert_int_equal(i_size(&arr), 0);
    assert_int_equal(errno, 0);
    
    // Try to pop from empty array
    errno = 0;
    int result = pop_back_int_vector(&arr);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, ENODATA);
}
// ================================================================================ 
// ================================================================================ 

void test_pop_front_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(3);
    assert_non_null(vec);
    
    // Add some test values
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 3);
    
    // Test popping values and check remaining elements
    errno = 0;
    assert_int_equal(pop_front_int_vector(vec), 1);
    assert_int_equal(i_size(vec), 2);
    assert_int_equal(int_vector_index(vec, 0), 2);
    assert_int_equal(int_vector_index(vec, 1), 3);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(pop_front_int_vector(vec), 2);
    assert_int_equal(i_size(vec), 1);
    assert_int_equal(int_vector_index(vec, 0), 3);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(pop_front_int_vector(vec), 3);
    assert_int_equal(i_size(vec), 0);
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_int_empty(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(1);
    assert_non_null(vec);
    
    // Try to pop from empty vector
    errno = 0;
    int result = pop_front_int_vector(vec);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, ENODATA);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_int_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    int result = pop_front_int_vector(NULL);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    int_v vec = {0};
    vec.data = NULL;
    errno = 0;
    result = pop_front_int_vector(&vec);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_int_static(void **state) {
    (void) state;
    
    int_v arr = init_int_array(2);
    
    // Add and pop values from static array
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    
    errno = 0;
    assert_int_equal(pop_front_int_vector(&arr), 1);
    assert_int_equal(i_size(&arr), 1);
    assert_int_equal(int_vector_index(&arr, 0), 2);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(pop_front_int_vector(&arr), 2);
    assert_int_equal(i_size(&arr), 0);
    assert_int_equal(errno, 0);
    
    // Try to pop from empty array
    errno = 0;
    int result = pop_front_int_vector(&arr);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, ENODATA);
}
// ================================================================================ 
// ================================================================================ 

void test_pop_any_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(4);
    assert_non_null(vec);
    
    // Add test values
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 3);
    push_back_int_vector(vec, 4);
    
    // Test popping from middle
    errno = 0;
    assert_int_equal(pop_any_int_vector(vec, 1), 2);
    assert_int_equal(i_size(vec), 3);
    assert_int_equal(int_vector_index(vec, 0), 1);
    assert_int_equal(int_vector_index(vec, 1), 3);
    assert_int_equal(int_vector_index(vec, 2), 4);
    assert_int_equal(errno, 0);
    
    // Test popping first element
    assert_int_equal(pop_any_int_vector(vec, 0), 1);
    assert_int_equal(i_size(vec), 2);
    assert_int_equal(int_vector_index(vec, 0), 3);
    assert_int_equal(int_vector_index(vec, 1), 4);
    
    // Test popping last element
    assert_int_equal(pop_any_int_vector(vec, 1), 4);
    assert_int_equal(i_size(vec), 1);
    assert_int_equal(int_vector_index(vec, 0), 3);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_int_errors(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(2);
    assert_non_null(vec);
    push_back_int_vector(vec, 1);
    
    // Test NULL vector
    errno = 0;
    int result = pop_any_int_vector(NULL, 0);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid index
    errno = 0;
    result = pop_any_int_vector(vec, 1);  // Index equals length
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, ERANGE);
    
    errno = 0;
    result = pop_any_int_vector(vec, 2);  // Index beyond length
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, ERANGE);
    
    // Test empty vector
    pop_any_int_vector(vec, 0);  // Remove the only element
    errno = 0;
    result = pop_any_int_vector(vec, 0);
    assert_int_equal(result, INT_MAX);
    assert_int_equal(errno, ENODATA);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_int_static(void **state) {
    (void) state;
    
    int_v arr = init_int_array(3);
    
    // Fill array
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    push_back_int_vector(&arr, 3);
    
    // Pop from middle
    errno = 0;
    int result = pop_any_int_vector(&arr, 1);
    assert_int_equal(result, 2);
    assert_int_equal(i_size(&arr), 2);
    assert_int_equal(int_vector_index(&arr, 0), 1);
    assert_int_equal(int_vector_index(&arr, 1), 3);
    assert_int_equal(errno, 0);
}
// ================================================================================ 
// ================================================================================

void test_reverse_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(4);
    assert_non_null(vec);
    
    // Test single element
    push_back_int_vector(vec, 1);
    reverse_int_vector(vec);
    assert_int_equal(i_size(vec), 1);
    assert_int_equal(int_vector_index(vec, 0), 1);
    
    // Test even number of elements
    push_back_int_vector(vec, 2.0f);
    reverse_int_vector(vec);
    assert_int_equal(i_size(vec), 2);
    assert_int_equal(int_vector_index(vec, 0), 2);
    assert_int_equal(int_vector_index(vec, 1), 1);
    
    // Test odd number of elements
    push_back_int_vector(vec, 3);
    reverse_int_vector(vec);
    assert_int_equal(i_size(vec), 3);
    assert_int_equal(int_vector_index(vec, 0), 3);
    assert_int_equal(int_vector_index(vec, 1), 1);
    assert_int_equal(int_vector_index(vec, 2), 2);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_reverse_int_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    reverse_int_vector(NULL);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    int_v vec = {0};
    vec.data = NULL;
    errno = 0;
    reverse_int_vector(&vec);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    int_v* empty_vec = init_int_vector(1);
    assert_non_null(empty_vec);
    errno = 0;
    reverse_int_vector(empty_vec);
    assert_int_equal(errno, ENODATA);
    
    free_int_vector(empty_vec);
}
// -------------------------------------------------------------------------------- 

void test_reverse_int_static(void **state) {
    (void) state;
    
    int_v arr = init_int_array(3);
    
    // Add test values
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    push_back_int_vector(&arr, 3);
    
    // Test reversal
    reverse_int_vector(&arr);
    assert_int_equal(i_size(&arr), 3);
    assert_int_equal(int_vector_index(&arr, 0), 3);
    assert_int_equal(int_vector_index(&arr, 1), 2);
    assert_int_equal(int_vector_index(&arr, 2), 1);
}
// ================================================================================
// ================================================================================ 

void test_sort_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(5);
    assert_non_null(vec);
    
    // Test ascending sort
    push_back_int_vector(vec, 5);
    push_back_int_vector(vec, 3);
    push_back_int_vector(vec, 4);
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 2);
    
    sort_int_vector(vec, FORWARD);
    
    // Verify ascending order
    for (size_t i = 0; i < i_size(vec) - 1; i++) {
        assert_true(int_vector_index(vec, i) <= int_vector_index(vec, i + 1));
    }
    
    // Test descending sort
    sort_int_vector(vec, REVERSE);
    
    // Verify descending order
    for (size_t i = 0; i < i_size(vec) - 1; i++) {
        assert_true(int_vector_index(vec, i) >= int_vector_index(vec, i + 1));
    }
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_int_edge_cases(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(10);
    assert_non_null(vec);
    
    // Test empty vector
    sort_int_vector(vec, FORWARD);
    assert_int_equal(i_size(vec), 0);
    
    // Test single element
    push_back_int_vector(vec, 1);
    sort_int_vector(vec, FORWARD);
    assert_int_equal(i_size(vec), 1);
    assert_int_equal(int_vector_index(vec, 0), 1);
    
    // Test two elements
    push_back_int_vector(vec, 0.0f);
    sort_int_vector(vec, FORWARD);
    assert_int_equal(int_vector_index(vec, 0), 0);
    assert_int_equal(int_vector_index(vec, 1), 1);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_int_duplicates(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(5);
    assert_non_null(vec);
    
    // Add duplicate values
    push_back_int_vector(vec, 3);
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 3);
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 2);
    
    sort_int_vector(vec, FORWARD);
    
    // Verify order with duplicates
    for (size_t i = 0; i < i_size(vec) - 1; i++) {
        assert_true(int_vector_index(vec, i) <= int_vector_index(vec, i + 1));
    }
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_static_int_array(void **state) {
    (void) state;
    
    int_v arr = init_int_array(5);
    
    // Add unsorted values
    push_back_int_vector(&arr, 5);
    push_back_int_vector(&arr, 3);
    push_back_int_vector(&arr, 4);
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    
    sort_int_vector(&arr, FORWARD);
    
    // Verify ascending order
    for (size_t i = 0; i < i_size(&arr) - 1; i++) {
        assert_true(int_vector_index(&arr, i) <= int_vector_index(&arr, i + 1));
    }
}
// -------------------------------------------------------------------------------- 

void test_sort_int_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    sort_int_vector(NULL, FORWARD);
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================ 

void test_trim_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(10);  // Start with capacity of 10
    assert_non_null(vec);
    
    // Add 5 elements
    for (int i = 0; i < 5; i++) {
        push_back_int_vector(vec, i);
    }
    
    size_t original_alloc = i_alloc(vec);
    assert_int_equal(original_alloc, 10);
    assert_int_equal(i_size(vec), 5);
    
    // Trim the vector
    errno = 0;
    trim_int_vector(vec);
    assert_int_equal(errno, 0);
    
    // Verify capacity matches size
    assert_int_equal(i_alloc(vec), 5);
    assert_int_equal(i_size(vec), 5);
    
    // Verify data is intact
    for (size_t i = 0; i < i_size(vec); i++) {
        assert_int_equal(int_vector_index(vec, i), i);
    }
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_empty_int_vector(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(5);  // Capacity of 5, but empty
    assert_non_null(vec);
    
    size_t original_alloc = i_alloc(vec);
    assert_int_equal(original_alloc, 5);
    assert_int_equal(i_size(vec), 0);
     
    // Trim the empty vector
    errno = 0;
    trim_int_vector(vec);
    assert_int_equal(errno, ENODATA);

    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_static_int_array(void **state) {
    (void) state;
    
    int_v arr = init_int_array(5);
    
    // Add some elements
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    
    size_t original_alloc = i_alloc(&arr);
    size_t original_size = i_size(&arr);
    
    // Try to trim static array
    errno = 0;
    trim_int_vector(&arr);
    assert_int_equal(errno, 0);  // Should not set error
    
    // Verify nothing changed
    assert_int_equal(i_alloc(&arr), original_alloc);
    assert_int_equal(i_size(&arr), original_size);
}
// -------------------------------------------------------------------------------- 

void test_trim_already_int_optimal(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(3);
    assert_non_null(vec);
    
    // Fill to capacity
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 3);
    
    assert_int_equal(i_alloc(vec), 3);
    assert_int_equal(i_size(vec), 3);
    
    // Try to trim an already optimal vector
    errno = 0;
    trim_int_vector(vec);
    assert_int_equal(errno, 0);
    
    // Verify nothing changed
    assert_int_equal(i_alloc(vec), 3);
    assert_int_equal(i_size(vec), 3);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_int_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    trim_int_vector(NULL);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    int_v vec = {0};
    vec.data = NULL;
    errno = 0;
    trim_int_vector(&vec);
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================ 

void test_binary_search_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(5);
    assert_non_null(vec);
    
    // Add values in order
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 3);
    push_back_int_vector(vec, 4);
    push_back_int_vector(vec, 5);
    
    // Test exact matches
    errno = 0;
    assert_int_equal(binary_search_int_vector(vec, 1, false), 0);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_int_vector(vec, 3, false), 2);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_int_vector(vec, 5, false), 4);
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_int_with_sort(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(5);
    assert_non_null(vec);
    
    // Add values out of order
    push_back_int_vector(vec, 5);
    push_back_int_vector(vec, 3);
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 4);
    push_back_int_vector(vec, 2);
    
    // Search with sort_first = true
    errno = 0;
    assert_int_equal(binary_search_int_vector(vec, 3, true), 2);
    assert_int_equal(errno, 0);
    
    // Verify the vector is now sorted
    for (size_t i = 0; i < i_size(vec) - 1; i++) {
        assert_true(int_vector_index(vec, i) <= int_vector_index(vec, i + 1));
    }
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_int_errors(void **state) {
    (void) state;
    // Test NULL vector
    errno = 0;
    assert_int_equal(binary_search_int_vector(NULL, 1, false), SIZE_MAX);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    int_v* vec = init_int_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_int_equal(binary_search_int_vector(vec, 1, false), SIZE_MAX);
    assert_int_equal(errno, ENODATA);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_int_static(void **state) {
    (void) state;
    
    int_v arr = init_int_array(5);
    
    // Add values
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    push_back_int_vector(&arr, 3);
    push_back_int_vector(&arr, 4);
    push_back_int_vector(&arr, 5);
    
    // Test search in static array
    errno = 0;
    assert_int_equal(binary_search_int_vector(&arr, 3, false), 2);
    assert_int_equal(errno, 0);
    
    // Test value not found
    errno = 0;
    assert_int_equal(binary_search_int_vector(&arr, 6, false), SIZE_MAX);
    assert_int_equal(errno, 0);
}
// ================================================================================ 
// ================================================================================ 

void test_update_int_vector_nominal(void **state) {
    (void) state;
    int_v arr = init_int_array(5);
    
    // Add values
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    push_back_int_vector(&arr, 3);
    push_back_int_vector(&arr, 4);
    push_back_int_vector(&arr, 5);

    update_int_vector(&arr, 2, 12);
    assert_int_equal(12, int_vector_index(&arr, 2));
}
// -------------------------------------------------------------------------------- 

void test_update_int_vector_null(void **state) {
    errno = 0;
    update_int_vector(NULL, 3, 8);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_update_int_vector_bad_index(void **state) {
    (void) state;
    int_v* vec = init_int_vector(3);
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 3);
    errno = 0;
    update_int_vector(vec, 5, 3);
    assert_int_equal(errno, ERANGE);
    free_int_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_min_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(5);
    assert_non_null(vec);
    
    // Test single element
    push_back_int_vector(vec, 1);
    errno = 0;
    assert_int_equal(min_int_vector(vec), 1);
    assert_int_equal(errno, 0);
    
    // Test multiple elements
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 4);
    push_back_int_vector(vec, 0);
    
    errno = 0;
    assert_int_equal(min_int_vector(vec), 0);
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_max_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(5);
    assert_non_null(vec);
    
    // Test single element
    push_back_int_vector(vec, 1.);
    errno = 0;
    assert_int_equal(max_int_vector(vec), 1);
    assert_int_equal(errno, 0);
    
    // Test multiple elements
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 12);
    push_back_int_vector(vec, 4);
    push_back_int_vector(vec, 0);
    
    errno = 0;
    assert_int_equal(max_int_vector(vec), 12);  // This will fail with current implementation
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_min_max_static_int_array(void **state) {
    (void) state;
    
    int_v arr = init_int_array(3);
    
    push_back_int_vector(&arr, 3);
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    
    errno = 0;
    assert_int_equal(min_int_vector(&arr), 1);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(max_int_vector(&arr), 3);  // This will fail with current implementation
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_min_max_int_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_int_equal(min_int_vector(NULL), INT_MAX);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_int_equal(max_int_vector(NULL), INT_MAX);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    int_v* vec = init_int_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_int_equal(min_int_vector(vec), INT_MAX);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_int_equal(max_int_vector(vec), INT_MAX);
    assert_int_equal(errno, EINVAL);
    
    free_int_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_sum_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(4);
    assert_non_null(vec);
    
    // Test single value
    push_back_int_vector(vec, 1);
    errno = 0;
    assert_int_equal(sum_int_vector(vec), 1);
    assert_int_equal(errno, 0);
    
    // Test multiple values
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 3);
    push_back_int_vector(vec, 4);
    
    errno = 0;
    assert_int_equal(sum_int_vector(vec), 10);
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_average_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(4);
    assert_non_null(vec);
    
    // Test single value
    push_back_int_vector(vec, 2);
    errno = 0;
    assert_int_equal(average_int_vector(vec), 2);
    assert_int_equal(errno, 0);
    
    // Test multiple values
    push_back_int_vector(vec, 4);
    push_back_int_vector(vec, 6);
    push_back_int_vector(vec, 8);
    
    errno = 0;
    assert_int_equal(average_int_vector(vec), 5);
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_int_negative(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(4);
    assert_non_null(vec);
    
    push_back_int_vector(vec, 12);
    push_back_int_vector(vec, 13);
    push_back_int_vector(vec, 5);
    push_back_int_vector(vec, 2);
    
    errno = 0;
    assert_int_equal(sum_int_vector(vec), 32);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(average_int_vector(vec), 8.0);
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_int_static(void **state) {
    (void) state;
    
    int_v arr = init_int_array(3);
    
    push_back_int_vector(&arr, 1);
    push_back_int_vector(&arr, 2);
    push_back_int_vector(&arr, 3);
    
    errno = 0;
    assert_int_equal(sum_int_vector(&arr), 6);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(average_int_vector(&arr), 2);
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_int_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_int_equal(sum_int_vector(NULL), INT_MAX);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(average_int_vector(NULL), FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    int_v* vec = init_int_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_int_equal(sum_int_vector(vec), INT_MAX);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(average_int_vector(vec), INT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    free_int_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_stdev_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(4);
    assert_non_null(vec);
    
    // Dataset with known standard deviation
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 4);
    push_back_int_vector(vec, 4);
    push_back_int_vector(vec, 6);
    
    // Mean = 4.0, variance = 2.0, stdev = sqrt(2.0)
    errno = 0;
    assert_float_equal(stdev_int_vector(vec), sqrt(2.0f), 0.0001f);
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_stdev_single_int_value(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(1);
    assert_non_null(vec);

    push_back_int_vector(vec, 2);

    errno = 0;
    float result = stdev_int_vector(vec);
    assert_float_equal(result, FLT_MAX, 0.0001f);  // Standard deviation of single value is 0
    // assert_int_equal(errno, ENODATA);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_stdev_same_int_values(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(3);
    assert_non_null(vec);
    
    // All same values should give stdev of 0
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 2);
    
    errno = 0;
    assert_float_equal(stdev_int_vector(vec), 0.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    free_int_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_cum_sum_int_basic(void **state) {
    (void) state;
    
    int_v* vec = init_int_vector(4);
    assert_non_null(vec);
    
    push_back_int_vector(vec, 1);
    push_back_int_vector(vec, 2);
    push_back_int_vector(vec, 3);
    push_back_int_vector(vec, 4);
    
    int_v* cum_sum = cum_sum_int_vector(vec);
    assert_non_null(cum_sum);
    assert_int_equal(i_size(cum_sum), 4);
    
    // Check cumulative sums: 1, 3, 6, 10
    assert_int_equal(int_vector_index(cum_sum, 0), 1);
    assert_int_equal(int_vector_index(cum_sum, 1), 3);
    assert_int_equal(int_vector_index(cum_sum, 2), 6);
    assert_int_equal(int_vector_index(cum_sum, 3), 10);
    
    free_int_vector(vec);
    free_int_vector(cum_sum);
}
// -------------------------------------------------------------------------------- 

void test_stdev_cum_sum_int_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_float_equal(stdev_int_vector(NULL), FLT_MAX, 0.0001f);
    assert_int_equal(errno, ENODATA);

    assert_null(cum_sum_int_vector(NULL));
    assert_int_equal(errno, EINVAL);

    // Test empty vector
    int_v* vec = init_int_vector(1);
    assert_non_null(vec);

    errno = 0;
    assert_float_equal(stdev_int_vector(vec), FLT_MAX, 0.0001f);
    assert_int_equal(errno, ENODATA);

    assert_null(cum_sum_int_vector(vec));
    assert_int_equal(errno, EINVAL);
     
    free_int_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

/* Setup and teardown functions */
static dict_i* test_dict = NULL;

int setup_int(void** state) {
    test_dict = init_int_dict();
    if (!test_dict) return -1;
    *state = test_dict;
    return 0;
}
// -------------------------------------------------------------------------------- 

int teardown_int(void** state) {
    free_int_dict(test_dict);
    return 0;
}
// -------------------------------------------------------------------------------- 

/* Test initialization */
void test_init_int_dict(void** state) {
    dict_i* dict = init_int_dict();
    assert_non_null(dict);
    assert_int_equal(int_dict_hash_size(dict), 0);
    assert_int_equal(int_dict_size(dict), 0);
    assert_true(int_dict_alloc(dict) > 0);
    free_int_dict(dict);
}
// -------------------------------------------------------------------------------- 

/* Test insertion */
void test_insert_int_dict_basic(void** state) {
    dict_i* dict = *state; 
    assert_true(insert_int_dict(dict, "test", 1));
    assert_int_equal(int_dict_hash_size(dict), 1);
    assert_int_equal(int_dict_size(dict), 1);

    int value = get_int_dict_value(dict, "test");
    assert_int_equal(value, 1);
}
// -------------------------------------------------------------------------------- 

void test_insert_int_dict_duplicate(void** state) {
    dict_i* dict = *state;
    assert_true(insert_int_dict(dict, "test", 1));
    assert_false(insert_int_dict(dict, "test", 2));
    
    int value = get_int_dict_value(dict, "test");
    assert_int_equal(value, 1);
}
// -------------------------------------------------------------------------------- 

void test_insert_int_dict_null(void** state) {
    dict_i* dict = *state;
    
    assert_false(insert_int_dict(NULL, "test", 1));
    assert_false(insert_int_dict(dict, NULL, 1));
}
// -------------------------------------------------------------------------------- 

/* Test retrieval */
void test_get_int_dict_value_basic(void** state) {
    dict_i* dict = *state;
    
    insert_int_dict(dict, "key1", 1);
    int value = get_int_dict_value(dict, "key1");
    assert_int_equal(value, 1.5);
}
// -------------------------------------------------------------------------------- 

void test_get_int_dict_value_missing(void** state) {
    dict_i* dict = *state;
    
    int value = get_int_dict_value(dict, "nonexistent");
    assert_int_equal(value, INT_MAX);
}
// -------------------------------------------------------------------------------- 

/* Test update */
void test_update_int_dict_basic(void** state) {
    dict_i* dict = *state;
    
    insert_int_dict(dict, "key1", 1);
    assert_true(update_int_dict(dict, "key1", 2));
    
    int value = get_int_dict_value(dict, "key1");
    assert_int_equal(value, 2);
}
// -------------------------------------------------------------------------------- 

void test_update_int_dict_missing(void** state) {
    dict_i* dict = *state;
    
    assert_false(update_int_dict(dict, "nonexistent", 1));
}
// -------------------------------------------------------------------------------- 

/* Test removal */
void test_pop_int_dict_basic(void** state) {
    dict_i* dict = *state;
    
    insert_int_dict(dict, "key1", 1);
    int value = pop_int_dict(dict, "key1");
    assert_int_equal(value, 1);
    assert_int_equal(int_dict_hash_size(dict), 0);
}
// -------------------------------------------------------------------------------- 

void test_pop_int_dict_missing(void** state) {
    dict_i* dict = *state;
    
    int value = pop_int_dict(dict, "nonexistent");
    assert_int_equal(value, INT_MAX);
}
// -------------------------------------------------------------------------------- 

/* Test resize behavior */
void test_resize_int_behavior(void** state) {
    dict_i* dict = *state;
    char key[20];
    size_t initial_alloc = int_dict_alloc(dict);
    
    // Insert enough items to trigger resize
    for (int i = 0; i < 100; i++) {
        sprintf(key, "key%d", i);
        assert_true(insert_int_dict(dict, key, i));
    }
    
    assert_true(int_dict_alloc(dict) > initial_alloc);
}
// -------------------------------------------------------------------------------- 

/* Test key/value extraction */
void test_get_keys_int_dict(void** state) {
    dict_i* dict = *state;
    
    insert_int_dict(dict, "key1", 1);
    insert_int_dict(dict, "key2", 2);
    
    string_v* keys = get_keys_int_dict(dict);
    assert_non_null(keys);
    assert_int_equal(int_dict_hash_size(dict), 2);
    
    free_str_vector(keys);
}
// -------------------------------------------------------------------------------- 

void test_get_values_int_dict(void** state) {
    dict_i* dict = *state;
    
    insert_int_dict(dict, "key1", 1);
    insert_int_dict(dict, "key2", 2);
    
    int_v* values = get_values_int_dict(dict);
    assert_non_null(values);
    assert_int_equal(int_dict_hash_size(dict), 2);
    
    free_int_vector(values);
}
// -------------------------------------------------------------------------------- 

/* Helper function for testing iterator */
static void count_entries(const char* key, int value, void* user_data) {
    size_t* count = (size_t*)user_data;
    (*count)++;
}

/* Helper function to sum values */
static void sum_values(const char* key, int value, void* user_data) {
    int* sum = (int*)user_data;
    *sum += value;
}
// -------------------------------------------------------------------------------- 

void test_foreach_int_dict_basic(void** state) {
    dict_i* dict = *state;
    size_t count = 0;
    
    // Add some test data
    assert_true(insert_int_dict(dict, "key1", 1));
    assert_true(insert_int_dict(dict, "key2", 2));
    assert_true(insert_int_dict(dict, "key3", 3));
    
    // Test iteration count
    assert_true(foreach_int_dict(dict, count_entries, &count));
    assert_int_equal(count, 3);
    assert_int_equal(count, int_dict_hash_size(dict));
    
    // Test value sum
    int sum = 0;
    assert_true(foreach_int_dict(dict, sum_values, &sum));
    assert_int_equal(sum, 6);
}
// -------------------------------------------------------------------------------- 

void test_foreach_int_dict_empty(void** state) {
    dict_i* dict = *state;
    size_t count = 0;
    
    assert_true(foreach_int_dict(dict, count_entries, &count));
    assert_int_equal(count, 0);
}
// -------------------------------------------------------------------------------- 

void test_foreach_int_dict_null(void** state) {
    dict_i* dict = *state;
    size_t count = 0;
    
    assert_false(foreach_int_dict(NULL, count_entries, &count));
    assert_false(foreach_int_dict(dict, NULL, &count));
}
// -------------------------------------------------------------------------------- 

void test_dictionary_int_gbc(void **state) {
    dict_i* newDict IDICT_GBC = init_int_dict();
    insert_int_dict(newDict, "Key1", 1);
}
// ================================================================================ 
// ================================================================================ 

void test_int_vector_dictionary(void **state) {
    dict_iv* dict = init_intv_dict();
    bool result;
    result = create_intv_dict(dict, "one", 3);
    assert_true(result);
    push_back_int_vector(return_intv_pointer(dict, "one"), 1);
    push_back_int_vector(return_intv_pointer(dict, "one"), 2);
    push_back_int_vector(return_intv_pointer(dict, "one"), 3);

    int test_one[3] = {1, 2, 3};
    int_v* vec1 = return_intv_pointer(dict, "one");
    for (size_t i = 0; i < int_vector_size(vec1); i++) {
        assert_int_equal(int_vector_index(vec1, i), test_one[i]);
    }

    result = create_intv_dict(dict, "two", 3);
    assert_true(result);
    push_back_int_vector(return_intv_pointer(dict, "two"), 4);
    push_back_int_vector(return_intv_pointer(dict, "two"), 5);
    push_back_int_vector(return_intv_pointer(dict, "two"), 6);

    int test_two[3] = {4, 5, 6};
    int_v* vec2 = return_intv_pointer(dict, "two");
    for (size_t i = 0; i < int_vector_size(vec2); i++) {
        assert_int_equal(int_vector_index(vec2, i), test_two[i]);
    }

    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_int_vector_dictionary_resize(void **state) {
    dict_iv* dict = init_intv_dict();
    bool result;
    result = create_intv_dict(dict, "one", 3);
    assert_true(result);
    push_back_int_vector(return_intv_pointer(dict, "one"), 1);
    push_back_int_vector(return_intv_pointer(dict, "one"), 2);
    push_back_int_vector(return_intv_pointer(dict, "one"), 3);
    push_back_int_vector(return_intv_pointer(dict, "one"), 4);
    push_back_int_vector(return_intv_pointer(dict, "one"), 5);

    int test_one[5] = {1, 2, 3, 4, 5};
    int_v* vec1 = return_intv_pointer(dict, "one");
    for (size_t i = 0; i < int_vector_size(vec1); i++) {
        assert_int_equal(int_vector_index(vec1, i), test_one[i]);
    }
    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_int_vector_dictionary_gbc(void **state) {
        IDICTV_GBC dict_iv* dict = init_intv_dict();
        bool result;
        result = create_intv_dict(dict, "one", 3);
        assert_true(result);
        push_back_int_vector(return_intv_pointer(dict, "one"), 1);
        push_back_int_vector(return_intv_pointer(dict, "one"), 2);
        push_back_int_vector(return_intv_pointer(dict, "one"), 3);
        push_back_int_vector(return_intv_pointer(dict, "one"), 4);
        push_back_int_vector(return_intv_pointer(dict, "one"), 5);

        int test_one[5] = {1, 2, 3, 4, 5};
        int_v* vec1 = return_intv_pointer(dict, "one");
        for (size_t i = 0; i < int_vector_size(vec1); i++) {
            assert_int_equal(int_vector_index(vec1, i), test_one[i]);
        }
    }
#endif
// --------------------------------------------------------------------------------

void test_pop_int_vector_dictionary(void **state) {
    dict_iv* dict = init_intv_dict();
    bool result;
    result = create_intv_dict(dict, "one", 3);
    assert_true(result);
    push_back_int_vector(return_intv_pointer(dict, "one"), 1);
    push_back_int_vector(return_intv_pointer(dict, "one"), 2);

    int test_one[3] = {1, 2, 3};
    int_v* vec1 = return_intv_pointer(dict, "one");
    for (size_t i = 0; i < int_vector_size(vec1); i++) {
        assert_int_equal(int_vector_index(vec1, i), test_one[i]);
    }

    result = create_intv_dict(dict, "two", 3);
    assert_true(result);
    push_back_int_vector(return_intv_pointer(dict, "two"), 4);
    push_back_int_vector(return_intv_pointer(dict, "two"), 5);

    int test_two[3] = {4, 5, 6};
    int_v* vec2 = return_intv_pointer(dict, "two");
    for (size_t i = 0; i < int_vector_size(vec2); i++) {
        assert_int_equal(int_vector_index(vec2, i), test_two[i]);
    }

    pop_intv_dict(dict, "two");
    assert_false(has_key_intv_dict(dict, "two"));
    assert_true(has_key_intv_dict(dict, "one"));

    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_insert_intv_dict_basic(void **state) {
    (void)state;  // unused

    dict_iv* dict = init_intv_dict();
    assert_non_null(dict);

    // Valid DYNAMIC vector
    int_v* vec1 = init_int_vector(3);
    assert_non_null(vec1);
    assert_int_equal(vec1->alloc_type, DYNAMIC);

    bool result = insert_intv_dict(dict, "key1", vec1);
    assert_true(result);

    // Confirm key exists
    assert_true(has_key_intv_dict(dict, "key1"));

    // Try inserting same key again — should fail
    int_v* vec2 = init_int_vector(2);
    assert_non_null(vec2);
    errno = 0;
    result = insert_intv_dict(dict, "key1", vec2);
    assert_false(result);
    assert_int_equal(errno, EEXIST);
    free_int_vector(vec2);  // Manual cleanup for rejected value

    // Try inserting STATIC vector — should fail
    int_v vec3 = init_int_array(2);  // This is heap-wrapped, but STATIC
    assert_int_equal(vec3.alloc_type, STATIC);
    errno = 0;
    result = insert_intv_dict(dict, "key_static", &vec3);
    assert_false(result);
    assert_int_equal(errno, EPERM);

    // NULL input tests
    errno = 0;
    assert_false(insert_intv_dict(NULL, "key", vec1));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(insert_intv_dict(dict, NULL, vec1));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(insert_intv_dict(dict, "keyX", NULL));
    assert_int_equal(errno, EINVAL);

    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_intv_size_macros(void **state) {
    dict_iv* dict = init_intv_dict();
    bool result;
    result = create_intv_dict(dict, "one", 3);
    assert_true(result);
    push_back_int_vector(return_intv_pointer(dict, "one"), 1);
    push_back_int_vector(return_intv_pointer(dict, "one"), 2);
    push_back_int_vector(return_intv_pointer(dict, "one"), 3);

    int test_one[3] = {1, 2, 3};
    int_v* vec1 = return_intv_pointer(dict, "one");
    for (size_t i = 0; i < int_vector_size(vec1); i++) {
        assert_int_equal(int_vector_index(vec1, i), test_one[i]);
    }

    result = create_intv_dict(dict, "two", 3);
    assert_true(result);
    push_back_int_vector(return_intv_pointer(dict, "two"), 4);
    push_back_int_vector(return_intv_pointer(dict, "two"), 5);
    push_back_int_vector(return_intv_pointer(dict, "two"), 6);

    int test_two[3] = {4, 5, 6};
    int_v* vec2 = return_intv_pointer(dict, "two");
    for (size_t i = 0; i < int_vector_size(vec2); i++) {
        assert_int_equal(int_vector_index(vec2, i), test_two[i]);
    }
    assert_int_equal(16, i_alloc(dict));
    assert_int_equal(2, i_size(dict));
    assert_int_equal(2, int_dictv_hash_size(dict));

    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_copy_intv_dict_success(void **state) {
    (void)state;

    dict_iv* original = init_intv_dict();
    int_v* v1 = init_int_vector(3);
    push_back_int_vector(v1, 1);
    push_back_int_vector(v1, 2);
    insert_intv_dict(original, "alpha", v1);

    dict_iv* copy = copy_intv_dict(original);
    assert_non_null(copy);
    assert_true(has_key_intv_dict(copy, "alpha"));

    int_v* copied = return_intv_pointer(copy, "alpha");
    assert_non_null(copied);
    assert_int_equal(int_vector_index(copied, 0), 1);
    assert_int_equal(int_vector_index(copied, 1), 2);

    // Ensure deep copy (modifying copy doesn't affect original)
    push_back_int_vector(copied, 999);
    assert_int_not_equal(int_vector_size(copied), int_vector_size(v1));

    free_intv_dict(original);
    free_intv_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_intv_dict_null_input(void **state) {
    (void)state;
    errno = 0;
    dict_iv* copy = copy_intv_dict(NULL);
    assert_null(copy);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_copy_intv_dict_static_vector(void **state) {
    (void)state;

    dict_iv* dict = init_intv_dict();
    int_v vec = init_int_array(2);
    push_back_int_vector(&vec, 42);
    insert_intv_dict(dict, "badkey", &vec);  // Insert unsafe manually
    
    errno = 0;
    dict_iv* copy = copy_intv_dict(dict);
    // assert_null(copy);
    // assert_int_equal(errno, EPERM);
    //
    free_intv_dict(dict);
    free_intv_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_intv_dict_multiple_entries(void **state) {
    (void)state;

    dict_iv* dict = init_intv_dict();
    int_v* v1 = init_int_vector(2);
    int_v* v2 = init_int_vector(2);
    push_back_int_vector(v1, 1);
    push_back_int_vector(v2, 2);
    insert_intv_dict(dict, "a", v1);
    insert_intv_dict(dict, "b", v2);

    dict_iv* copy = copy_intv_dict(dict);
    assert_non_null(copy);
    assert_true(has_key_intv_dict(copy, "a"));
    assert_true(has_key_intv_dict(copy, "b"));

    free_intv_dict(dict);
    free_intv_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_copy_intv_dict_independence(void **state) {
    (void)state;

    dict_iv* dict = init_intv_dict();
    int_v* v = init_int_vector(1);
    push_back_int_vector(v, 10);
    insert_intv_dict(dict, "x", v);

    dict_iv* copy = copy_intv_dict(dict);
    int_v* original = return_intv_pointer(dict, "x");
    int_v* copied = return_intv_pointer(copy, "x");

    push_back_int_vector(copied, 20);
    assert_int_not_equal(int_vector_size(original), int_vector_size(copied));

    free_intv_dict(dict);
    free_intv_dict(copy);
}
// -------------------------------------------------------------------------------- 

void test_merge_intv_dict_no_overwrite(void **state) {
    (void)state;
    dict_iv* dict1 = init_intv_dict();
    dict_iv* dict2 = init_intv_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    int_v* vec1 = init_int_vector(1); push_back_int_vector(vec1, 1);
    int_v* vec2 = init_int_vector(1); push_back_int_vector(vec2, 2);
    int_v* vec3 = init_int_vector(1); push_back_int_vector(vec3, 3);

    insert_intv_dict(dict1, "alpha", vec1);
    insert_intv_dict(dict1, "beta", vec2);
    insert_intv_dict(dict2, "beta", vec3);  // conflict
    insert_intv_dict(dict2, "gamma", init_int_vector(1));

    dict_iv* merged = merge_intv_dict(dict1, dict2, false);
    assert_non_null(merged);

    int_v* merged_beta = return_intv_pointer(merged, "beta");
    assert_int_equal(int_vector_index(merged_beta, 0), 2);  // from dict1

    free_intv_dict(dict1);
    free_intv_dict(dict2);
    free_intv_dict(merged);
}
// -------------------------------------------------------------------------------- 

void test_merge_intv_dict_overwrite(void **state) {
    (void)state;

    dict_iv* dict1 = init_intv_dict();
    dict_iv* dict2 = init_intv_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    int_v* vec1 = init_int_vector(1); push_back_int_vector(vec1, 1);
    int_v* vec2 = init_int_vector(1); push_back_int_vector(vec2, 2);
    int_v* vec3 = init_int_vector(1); push_back_int_vector(vec3, 99);

    insert_intv_dict(dict1, "beta", vec2);
    insert_intv_dict(dict2, "beta", vec3);  // conflict
    insert_intv_dict(dict1, "alpha", vec1);

    dict_iv* merged = merge_intv_dict(dict1, dict2, true);
    assert_non_null(merged);

    int_v* overwritten_beta = return_intv_pointer(merged, "beta");
    assert_int_equal(int_vector_index(overwritten_beta, 0), 99);  // from dict2

    free_intv_dict(dict1);
    free_intv_dict(dict2);
    free_intv_dict(merged);
}
// -------------------------------------------------------------------------------- 

void test_merge_intv_dict_reject_static(void **state) {
    (void)state;

    dict_iv* dict1 = init_intv_dict();
    dict_iv* dict2 = init_intv_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    int_v* vec1 = init_int_vector(1); push_back_int_vector(vec1, 1);
    insert_intv_dict(dict1, "alpha", vec1);

    int_v vec_static = init_int_array(2);  // stack + static
    push_back_int_vector(&vec_static, 42);
    insert_intv_dict(dict2, "static_key", &vec_static);

    errno = 0;
    dict_iv* merged = merge_intv_dict(dict1, dict2, true);

    free_intv_dict(dict1);
    free_intv_dict(dict2);
    free_intv_dict(merged);
}
// -------------------------------------------------------------------------------- 

void test_merge_intv_dict_null_inputs(void **state) {
    (void)state;

    dict_iv* valid = init_intv_dict();
    assert_non_null(valid);

    errno = 0;
    assert_null(merge_intv_dict(NULL, valid, true));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_null(merge_intv_dict(valid, NULL, false));
    assert_int_equal(errno, EINVAL);

    free_intv_dict(valid);
}
// -------------------------------------------------------------------------------- 

void test_merge_intv_dict_unique_keys(void **state) {
    (void)state;

    dict_iv* dict1 = init_intv_dict();
    dict_iv* dict2 = init_intv_dict();
    assert_non_null(dict1);
    assert_non_null(dict2);

    // Create unique int_v* instances for each dictionary
    int_v* vx = init_int_vector(1); push_back_int_vector(vx, 1);
    int_v* vy = init_int_vector(1); push_back_int_vector(vy, 2);
    int_v* vz = init_int_vector(1); push_back_int_vector(vz, 3);

    insert_intv_dict(dict1, "x", vx);
    insert_intv_dict(dict2, "y", vy);
    insert_intv_dict(dict2, "z", vz);

    dict_iv* merged = merge_intv_dict(dict1, dict2, false);
    assert_non_null(merged);
    assert_true(has_key_intv_dict(merged, "x"));
    assert_true(has_key_intv_dict(merged, "y"));
    assert_true(has_key_intv_dict(merged, "z"));

    // Cleanup only from merged dict
    free_intv_dict(merged);
    free_intv_dict(dict1);
    free_intv_dict(dict2);

    // Don't double-free dict1 and dict2 because their values were reused in merged
    // Alternative: clone data during merge to make them independent (deep copy)
}
// -------------------------------------------------------------------------------- 

void test_clear_intv_dict_basic(void **state) {
    (void)state;

    dict_iv* dict = init_intv_dict();
    assert_non_null(dict);

    create_intv_dict(dict, "a", 3);
    create_intv_dict(dict, "b", 2);

    assert_true(has_key_intv_dict(dict, "a"));
    assert_true(has_key_intv_dict(dict, "b"));
    assert_int_equal(int_dictv_size(dict), 2);
    assert_int_equal(int_dictv_hash_size(dict), 2);

    clear_intv_dict(dict);

    assert_false(has_key_intv_dict(dict, "a"));
    assert_false(has_key_intv_dict(dict, "b"));
    assert_int_equal(int_dictv_size(dict), 0);
    assert_int_equal(int_dictv_hash_size(dict), 0);

    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_clear_intv_dict_empty(void **state) {
    (void)state;

    dict_iv* dict = init_intv_dict();
    assert_non_null(dict);

    clear_intv_dict(dict);  // Should not crash
    assert_int_equal(int_dictv_size(dict), 0);
    assert_int_equal(int_dictv_hash_size(dict), 0);

    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_clear_intv_dict_reuse_after_clear(void **state) {
    (void)state;

    dict_iv* dict = init_intv_dict();
    assert_non_null(dict);

    create_intv_dict(dict, "temp1", 2);
    create_intv_dict(dict, "temp2", 2);
    clear_intv_dict(dict);

    // Insert again after clear
    create_intv_dict(dict, "new", 2);
    assert_true(has_key_intv_dict(dict, "new"));
    assert_int_equal(int_dictv_hash_size(dict), 1);

    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

static void key_counter(const char* key, const int_v* value, void* user_data) {
    (void)key; (void)value;
    int* counter = (int*)user_data;
    (*counter)++;
}

void test_foreach_intv_dict_counts_keys(void **state) {
    (void)state;

    dict_iv* dict = init_intv_dict();
    create_intv_dict(dict, "A", 2);
    create_intv_dict(dict, "B", 3);
    create_intv_dict(dict, "C", 1);

    int count = 0;
    bool result = foreach_intv_dict(dict, key_counter, &count);

    assert_true(result);
    assert_int_equal(count, 3);

    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_foreach_intv_dict_with_null_dict(void **state) {
    (void)state;
    errno = 0;

    bool result = foreach_intv_dict(NULL, key_counter, NULL);
    assert_false(result);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_foreach_intv_dict_with_null_callback(void **state) {
    (void)state;

    dict_iv* dict = init_intv_dict();
    create_intv_dict(dict, "temp", 1);

    errno = 0;
    bool result = foreach_intv_dict(dict, NULL, NULL);

    assert_false(result);
    assert_int_equal(errno, EINVAL);

    free_intv_dict(dict);
}
// -------------------------------------------------------------------------------- 

typedef struct {
    int sum;
    size_t count;
} accumulator;

static void accumulate_values(const char* key, const int_v* vec, void* user_data) {
    (void)key;
    accumulator* acc = (accumulator*)user_data;
    for (size_t i = 0; i < int_vector_size(vec); ++i) {
        acc->sum += int_vector_index(vec, i);
        acc->count++;
    }
}

void test_foreach_intv_dict_accumulates_sum(void **state) {
    (void)state;

    dict_iv* dict = init_intv_dict();
    create_intv_dict(dict, "sensor1", 3);
    create_intv_dict(dict, "sensor2", 2);

    push_back_int_vector(return_intv_pointer(dict, "sensor1"), 1);
    push_back_int_vector(return_intv_pointer(dict, "sensor1"), 2);
    push_back_int_vector(return_intv_pointer(dict, "sensor1"), 3);

    push_back_int_vector(return_intv_pointer(dict, "sensor2"), 4);
    push_back_int_vector(return_intv_pointer(dict, "sensor2"), 5);

    accumulator acc = {0.0f, 0};
    bool result = foreach_intv_dict(dict, accumulate_values, &acc);

    assert_true(result);
    assert_int_equal(acc.count, 5);
    assert_int_equal(acc.sum, 15);

    free_intv_dict(dict);
}
// ================================================================================
// ================================================================================
// eof
