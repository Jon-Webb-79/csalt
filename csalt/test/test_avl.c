// ================================================================================
// ================================================================================
// - File:    test_avl.c
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

#include "test_avl.h"
// ================================================================================
// ================================================================================

void test_init_char_avl(void **state) {
	(void) state;
    charAVLTree* tree = init_avltree(dChar)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_char_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// --------------------------------------------------------------------------------

void test_init_uchar_avl(void **state) {
	(void) state;
    ucharAVLTree* tree = init_avltree(dUChar)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_uchar_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// --------------------------------------------------------------------------------

void test_init_short_avl(void **state) {
	(void) state;
    shortAVLTree* tree = init_avltree(dShort)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_short_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// --------------------------------------------------------------------------------

void test_init_ushort_avl(void **state) {
	(void) state;
    ushortAVLTree* tree = init_avltree(dUShort)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_ushort_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// -------------------------------------------------------------------------------- 

void test_init_int_avl(void **state) {
	(void) state;
    intAVLTree* tree = init_avltree(dInt)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_int_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// --------------------------------------------------------------------------------

void test_init_uint_avl(void **state) {
	(void) state;
    uintAVLTree* tree = init_avltree(dUInt)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_uint_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// -------------------------------------------------------------------------------- 

void test_init_long_avl(void **state) {
	(void) state;
    longAVLTree* tree = init_avltree(dLong)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_long_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// -------------------------------------------------------------------------------- 

void test_init_ulong_avl(void **state) {
	(void) state;
    ulongAVLTree* tree = init_avltree(dULong)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_ulong_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// -------------------------------------------------------------------------------- 

void test_init_llong_avl(void **state) {
	(void) state;
    llongAVLTree* tree = init_avltree(dLongLong)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_llong_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// -------------------------------------------------------------------------------- 

void test_init_ullong_avl(void **state) {
	(void) state;
    ullongAVLTree* tree = init_avltree(dULongLong)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_ullong_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// -------------------------------------------------------------------------------- 

void test_init_float_avl(void **state) {
	(void) state;
    floatAVLTree* tree = init_avltree(dFloat)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_float_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// --------------------------------------------------------------------------------

void test_init_double_avl(void **state) {
	(void) state;
    doubleAVLTree* tree = init_avltree(dDouble)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_double_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// -------------------------------------------------------------------------------- 

void test_init_ldouble_avl(void **state) {
	(void) state;
    ldoubleAVLTree* tree = init_avltree(dLDouble)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_ldouble_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// -------------------------------------------------------------------------------- 

void test_init_bool_avl(void **state) {
	(void) state;
    boolAVLTree* tree = init_avltree(dBool)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_bool_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// --------------------------------------------------------------------------------

void test_init_string_avl(void **state) {
	(void) state;
    stringAVLTree* tree = init_avltree(dString)(false);
    assert_non_null(tree);
    assert_int_equal(tree->len, 0);
    assert_null(tree->root);
    assert_false(tree->allow_duplicates);

    free_avltree(tree);  // Clean up

    tree = init_string_avltree(true);
    assert_non_null(tree);
    assert_true(tree->allow_duplicates);

    free_avltree(tree);  // Clean up
}
// ================================================================================
// ================================================================================

void test_insert_char_avl_no_duplicates(void **state) {
    charAVLTree* tree = init_avltree(dChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_char_avl_duplicates(void **state) {
    charAVLTree* tree = init_avltree(dChar)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_uchar_avl_no_duplicates(void **state) {
    ucharAVLTree* tree = init_avltree(dUChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_uchar_avl_duplicates(void **state) {
    ucharAVLTree* tree = init_avltree(dUChar)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_short_avl_no_duplicates(void **state) {
    shortAVLTree* tree = init_avltree(dShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_short_avl_duplicates(void **state) {
    shortAVLTree* tree = init_avltree(dShort)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_ushort_avl_no_duplicates(void **state) {
    ushortAVLTree* tree = init_avltree(dUShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_ushort_avl_duplicates(void **state) {
    ushortAVLTree* tree = init_avltree(dUShort)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_int_avl_no_duplicates(void **state) {
    intAVLTree* tree = init_avltree(dInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_int_avl_duplicates(void **state) {
    intAVLTree* tree = init_avltree(dInt)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_uint_avl_no_duplicates(void **state) {
    uintAVLTree* tree = init_avltree(dUInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_uint_avl_duplicates(void **state) {
    uintAVLTree* tree = init_avltree(dUInt)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_long_avl_no_duplicates(void **state) {
    longAVLTree* tree = init_avltree(dLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_long_avl_duplicates(void **state) {
    longAVLTree* tree = init_avltree(dLong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// -------------------------------------------------------------------------------- 

void test_insert_ulong_avl_no_duplicates(void **state) {
    ulongAVLTree* tree = init_avltree(dULong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_ulong_avl_duplicates(void **state) {
    ulongAVLTree* tree = init_avltree(dULong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_llong_avl_no_duplicates(void **state) {
    llongAVLTree* tree = init_avltree(dLongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_llong_avl_duplicates(void **state) {
    llongAVLTree* tree = init_avltree(dLongLong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_ullong_avl_no_duplicates(void **state) {
    ullongAVLTree* tree = init_avltree(dULongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_ullong_avl_duplicates(void **state) {
    ullongAVLTree* tree = init_avltree(dULongLong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_float_avl_no_duplicates(void **state) {
    floatAVLTree* tree = init_avltree(dFloat)(false);
    insert_avltree(tree, 1.f);
    insert_avltree(tree, 1.f);
    insert_avltree(tree, 2.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 1.f);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1.f);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2.f);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3.f);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_double_avl_duplicates(void **state) {
    doubleAVLTree* tree = init_avltree(dDouble)(true);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 1.);
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, 1.);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1.);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2.);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3.);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_ldouble_avl_no_duplicates(void **state) {
    ldoubleAVLTree* tree = init_avltree(dLDouble)(false);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 1.);
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, 1.);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, 2.);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, 3.);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_bool_avl_no_duplicates(void **state) {
    boolAVLTree* tree = init_avltree(dBool)(false);
    insert_avltree(tree, true);
    insert_avltree(tree, false);
    insert_avltree(tree, true);
    insert_avltree(tree, false);
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, true);
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, false);
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_string_avl_no_duplicates(void **state) {
    stringAVLTree* tree = init_avltree(dString)(false);
    insert_avltree(tree, "Hello");
    insert_avltree(tree, "Hello");
    insert_avltree(tree, "Goodbye");
    insert_avltree(tree, "Again");
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, "Hello");
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, "Goodbye");
    assert_int_equal(avltree_size(tree), 1);
    remove_avltree(tree, "Again");
    assert_int_equal(avltree_size(tree), 0);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_insert_string_avl_duplicates(void **state) {
    stringAVLTree* tree = init_avltree(dString)(true);
    insert_avltree(tree, "Hello");
    insert_avltree(tree, "Hello");
    insert_avltree(tree, "Goodbye");
    insert_avltree(tree, "Again");
    assert_int_equal(avltree_size(tree), 4);
    remove_avltree(tree, "Hello");
    assert_int_equal(avltree_size(tree), 3);
    remove_avltree(tree, "Again");
    assert_int_equal(avltree_size(tree), 2);
    remove_avltree(tree, "Hello");
    assert_int_equal(avltree_size(tree), 1);
    free_avltree(tree);
}
// ================================================================================ 
// ================================================================================ 
// eof
