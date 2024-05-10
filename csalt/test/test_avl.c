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

void test_char_avltree_gbc(void **state) {
    charAVLTree* tree gbc_char_avltree = init_avltree(dChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_uchar_avltree_gbc(void **state) {
    ucharAVLTree* tree gbc_uchar_avltree = init_avltree(dUChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_short_avltree_gbc(void **state) {
    shortAVLTree* tree gbc_short_avltree = init_avltree(dShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_ushort_avltree_gbc(void **state) {
    ushortAVLTree* tree gbc_ushort_avltree = init_avltree(dUShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_int_avltree_gbc(void **state) {
    intAVLTree* tree gbc_int_avltree = init_avltree(dInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_uint_avltree_gbc(void **state) {
    uintAVLTree* tree gbc_uint_avltree = init_avltree(dUInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_long_avltree_gbc(void **state) {
    longAVLTree* tree gbc_long_avltree = init_avltree(dLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_ulong_avltree_gbc(void **state) {
    ulongAVLTree* tree gbc_ulong_avltree = init_avltree(dULong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_llong_avltree_gbc(void **state) {
    llongAVLTree* tree gbc_llong_avltree = init_avltree(dLongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_ullong_avltree_gbc(void **state) {
    ullongAVLTree* tree gbc_ullong_avltree = init_avltree(dULongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
}
// --------------------------------------------------------------------------------

void test_float_avltree_gbc(void **state) {
    floatAVLTree* tree gbc_float_avltree = init_avltree(dFloat)(false);
    insert_avltree(tree, 1.f);
    insert_avltree(tree, 2.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 4.f);
}
// -------------------------------------------------------------------------------- 

void test_double_avltree_gbc(void **state) {
    doubleAVLTree* tree gbc_double_avltree = init_avltree(dDouble)(false);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
}
// --------------------------------------------------------------------------------

void test_ldouble_avltree_gbc(void **state) {
    ldoubleAVLTree* tree gbc_ldouble_avltree = init_avltree(dLDouble)(false);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
}
// --------------------------------------------------------------------------------

void test_bool_avltree_gbc(void **state) {
    boolAVLTree* tree gbc_bool_avltree = init_avltree(dBool)(false);
    insert_avltree(tree, true);
    insert_avltree(tree, false);
}
// --------------------------------------------------------------------------------

void test_string_avltree_gbc(void **state) {
    stringAVLTree* tree gbc_string_avltree = init_avltree(dString)(false);
    insert_avltree(tree, "One");
    insert_avltree(tree, "Two");
}
// ================================================================================ 
// ================================================================================

void test_min_char_avltree(void **state) {
    charAVLTree* tree = init_avltree(dChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    char minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_uchar_avltree(void **state) {
    ucharAVLTree* tree = init_avltree(dUChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned char minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_short_avltree(void **state) {
    shortAVLTree* tree = init_avltree(dShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    short minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_ushort_avltree(void **state) {
    ushortAVLTree* tree = init_avltree(dUShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned short int minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_int_avltree(void **state) {
    intAVLTree* tree = init_avltree(dInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    int minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_uint_avltree(void **state) {
    uintAVLTree* tree = init_avltree(dUInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned int minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_long_avltree(void **state) {
    longAVLTree* tree = init_avltree(dLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    long int minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_ulong_avltree(void **state) {
    ulongAVLTree* tree = init_avltree(dULong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned long int minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_llong_avltree(void **state) {
    llongAVLTree* tree = init_avltree(dLongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    long long int minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_ullong_avltree(void **state) {
    ullongAVLTree* tree = init_avltree(dULongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned long long int minVal = min_avltree(tree);
    assert_int_equal(minVal, 1);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_float_avltree(void **state) {
    floatAVLTree* tree = init_avltree(dFloat)(false);
    insert_avltree(tree, 1.f);
    insert_avltree(tree, 2.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 4.f);
    insert_avltree(tree, 5.f);
    float minVal = min_avltree(tree);
    assert_float_equal(minVal, 1.f, 1.0e-3);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_double_avltree(void **state) {
    doubleAVLTree* tree = init_avltree(dDouble)(false);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    double minVal = min_avltree(tree);
    assert_double_equal(minVal, 1., 1.0e-3);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_ldouble_avltree(void **state) {
    ldoubleAVLTree* tree = init_avltree(dLDouble)(false);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    long double minVal = min_avltree(tree);
    assert_double_equal(minVal, 1., 1.0e-3);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_bool_avltree(void **state) {
    boolAVLTree* tree = init_avltree(dBool)(false);
    insert_avltree(tree, true);
    insert_avltree(tree, false);
    bool minVal = min_avltree(tree);
    assert_int_equal(minVal, false);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_min_string_avltree(void **state) {
    stringAVLTree* tree = init_avltree(dString)(false);
    insert_avltree(tree, "One");
    insert_avltree(tree, "Two");
    insert_avltree(tree, "Three");
    insert_avltree(tree, "Five");
    str* minVal = min_avltree(tree);
    assert_string_equal("Five", minVal->data);
    free_avltree(tree);
}
// ================================================================================ 
// ================================================================================

void test_max_char_avltree(void **state) {
    charAVLTree* tree = init_avltree(dChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    char maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_uchar_avltree(void **state) {
    ucharAVLTree* tree = init_avltree(dUChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned char maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_short_avltree(void **state) {
    shortAVLTree* tree = init_avltree(dShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    short maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_ushort_avltree(void **state) {
    ushortAVLTree* tree = init_avltree(dUShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned short int maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_int_avltree(void **state) {
    intAVLTree* tree = init_avltree(dInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    int maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_uint_avltree(void **state) {
    uintAVLTree* tree = init_avltree(dUInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned int maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_long_avltree(void **state) {
    longAVLTree* tree = init_avltree(dLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    long int maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_ulong_avltree(void **state) {
    ulongAVLTree* tree = init_avltree(dULong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned long int maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_llong_avltree(void **state) {
    llongAVLTree* tree = init_avltree(dLongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    long long int maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_ullong_avltree(void **state) {
    ullongAVLTree* tree = init_avltree(dULongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    unsigned long long int maxVal = max_avltree(tree);
    assert_int_equal(maxVal, 5);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_float_avltree(void **state) {
    floatAVLTree* tree = init_avltree(dFloat)(false);
    insert_avltree(tree, 1.f);
    insert_avltree(tree, 2.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 4.f);
    insert_avltree(tree, 5.f);
    float maxVal = max_avltree(tree);
    assert_float_equal(maxVal, 5.f, 1.0e-3);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_double_avltree(void **state) {
    doubleAVLTree* tree = init_avltree(dDouble)(false);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    double maxVal = max_avltree(tree);
    assert_double_equal(maxVal, 5., 1.0e-3);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_ldouble_avltree(void **state) {
    ldoubleAVLTree* tree = init_avltree(dLDouble)(false);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    long double maxVal = max_avltree(tree);
    assert_double_equal(maxVal, 5., 1.0e-3);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_bool_avltree(void **state) {
    boolAVLTree* tree = init_avltree(dBool)(false);
    insert_avltree(tree, true);
    insert_avltree(tree, false);
    bool maxVal = max_avltree(tree);
    assert_int_equal(maxVal, true);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_max_string_avltree(void **state) {
    stringAVLTree* tree = init_avltree(dString)(false);
    insert_avltree(tree, "One");
    insert_avltree(tree, "Two");
    insert_avltree(tree, "Three");
    insert_avltree(tree, "Five");
    str* maxVal = max_avltree(tree);
    assert_string_equal("Two", maxVal->data);
    free_avltree(tree);
}
// ================================================================================
// ================================================================================

void test_all_char_avltree(void **state) {
    charAVLTree* tree = init_avltree(dChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    char_v* vec = avltree_values(tree, 0, ALL);
    char a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_uchar_avltree(void **state) {
    ucharAVLTree* tree = init_avltree(dUChar)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    uchar_v* vec = avltree_values(tree, 0, ALL);
    unsigned char a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_short_avltree(void **state) {
    shortAVLTree* tree = init_avltree(dShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    short_v* vec = avltree_values(tree, 0, ALL);
    short int a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_ushort_avltree(void **state) {
    ushortAVLTree* tree = init_avltree(dUShort)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    ushort_v* vec = avltree_values(tree, 0, ALL);
    unsigned short a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_int_avltree(void **state) {
    intAVLTree* tree = init_avltree(dInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    int_v* vec = avltree_values(tree, 0, ALL);
    int a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_uint_avltree(void **state) {
    uintAVLTree* tree = init_avltree(dUInt)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    uint_v* vec = avltree_values(tree, 0, ALL);
    unsigned int a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_long_avltree(void **state) {
    longAVLTree* tree = init_avltree(dLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    long_v* vec = avltree_values(tree, 0, ALL);
    long int a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// -------------------------------------------------------------------------------- 

void test_all_ulong_avltree(void **state) {
    ulongAVLTree* tree = init_avltree(dULong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    ulong_v* vec = avltree_values(tree, 0, ALL);
    unsigned long int a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_llong_avltree(void **state) {
    llongAVLTree* tree = init_avltree(dLongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    llong_v* vec = avltree_values(tree, 0, ALL);
    long long int a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_ullong_avltree(void **state) {
    ullongAVLTree* tree = init_avltree(dULongLong)(false);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    ullong_v* vec = avltree_values(tree, 0, ALL);
    unsigned long long int a[6] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_float_avltree(void **state) {
    floatAVLTree* tree = init_avltree(dFloat)(false);
    insert_avltree(tree, 1.f);
    insert_avltree(tree, 2.f);
    insert_avltree(tree, 2.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 4.f);
    insert_avltree(tree, 5.f);
    insert_avltree(tree, 6.f);
    float_v* vec = avltree_values(tree, 0, ALL);
    float a[6] = {1., 2., 3., 4., 5., 6.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_double_avltree(void **state) {
    doubleAVLTree* tree = init_avltree(dDouble)(false);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    insert_avltree(tree, 6.);
    double_v* vec = avltree_values(tree, 0, ALL);
    double a[6] = {1., 2., 3., 4., 5., 6.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_ldouble_avltree(void **state) {
    ldoubleAVLTree* tree = init_avltree(dLDouble)(false);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    insert_avltree(tree, 6.);
    ldouble_v* vec = avltree_values(tree, 0, ALL);
    long double a[6] = {1., 2., 3., 4., 5., 6.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_bool_avltree(void **state) {
    boolAVLTree* tree = init_avltree(dBool)(false);
    insert_avltree(tree, true);
    insert_avltree(tree, false);
    insert_avltree(tree, true);
    insert_avltree(tree, false);
    insert_avltree(tree, true);
    insert_avltree(tree, false);
    insert_avltree(tree, true);
    bool_v* vec = avltree_values(tree, 0, ALL);
    float a[2] = {false, true};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
    free_avltree(tree);
}
// --------------------------------------------------------------------------------

void test_all_string_avltree(void **state) {
    stringAVLTree* tree = init_avltree(dString)(false);
    insert_avltree(tree, "Apple");
    insert_avltree(tree, "Banana");
    insert_avltree(tree, "Cucumber");
    insert_avltree(tree, "Diode");
    insert_avltree(tree, "Echo");
    insert_avltree(tree, "Foxtrot");
    insert_avltree(tree, "Golf");
    string_v* vec = avltree_values(tree, 0, ALL);
    char *a[7] = {"Apple", "Banana", "Cucumber", "Diode", "Echo", "Foxtrot", "Golf"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
    free_avltree(tree);
}
// ================================================================================
// ================================================================================ 

void test_gte_char_avltree(void **state) {
    charAVLTree* tree = init_avltree(dChar)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    char_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    char a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_uchar_avltree(void **state) {
    ucharAVLTree* tree = init_avltree(dUChar)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    uchar_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    unsigned char a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_short_avltree(void **state) {
    shortAVLTree* tree = init_avltree(dShort)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    short_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    short int a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_ushort_avltree(void **state) {
    ushortAVLTree* tree = init_avltree(dUShort)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    ushort_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    unsigned short int a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_int_avltree(void **state) {
    intAVLTree* tree = init_avltree(dInt)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    int_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    int a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_uint_avltree(void **state) {
    uintAVLTree* tree = init_avltree(dUInt)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    uint_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    unsigned int a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_long_avltree(void **state) {
    longAVLTree* tree = init_avltree(dLong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    long_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    long int a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_ulong_avltree(void **state) {
    ulongAVLTree* tree = init_avltree(dULong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    ulong_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    unsigned long int a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_llong_avltree(void **state) {
    llongAVLTree* tree = init_avltree(dLongLong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    llong_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    long long int a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_ullong_avltree(void **state) {
    ullongAVLTree* tree = init_avltree(dULongLong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    ullong_v* vec = avltree_values(tree, 3, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    unsigned long long int a[7] = {3, 3, 3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_float_avltree(void **state) {
    floatAVLTree* tree = init_avltree(dFloat)(true);
    insert_avltree(tree, 1.f);
    insert_avltree(tree, 2.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 4.f);
    insert_avltree(tree, 5.f);
    insert_avltree(tree, 6.f);
    float_v* vec = avltree_values(tree, 3.f, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    float a[7] = {3., 3., 3., 3., 4., 5., 6.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_double_avltree(void **state) {
    doubleAVLTree* tree = init_avltree(dDouble)(true);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    insert_avltree(tree, 6.);
    double_v* vec = avltree_values(tree, 3., GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    double a[7] = {3., 3., 3., 3., 4., 5., 6.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_ldouble_avltree(void **state) {
    ldoubleAVLTree* tree = init_avltree(dLDouble)(true);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    insert_avltree(tree, 6.);
    ldouble_v* vec = avltree_values(tree, 3., GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    long double a[7] = {3., 3., 3., 3., 4., 5., 6.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_bool_avltree(void **state) {
    boolAVLTree* tree = init_avltree(dBool)(true);
    insert_avltree(tree, true);
    insert_avltree(tree, true);
    insert_avltree(tree, true);
    insert_avltree(tree, false);
    insert_avltree(tree, false);
    insert_avltree(tree, false);
    insert_avltree(tree, false);
    insert_avltree(tree, false);
    insert_avltree(tree, false);
    bool_v* vec = avltree_values(tree, false, GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    bool a[9] = {false, false, false, false, false, false, true, true, true};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_gte_string_avltree(void **state) {
    stringAVLTree* tree = init_avltree(dString)(true);
    insert_avltree(tree, "Apple");
    insert_avltree(tree, "Banana");
    insert_avltree(tree, "Cucumber");
    insert_avltree(tree, "Cucumber");
    insert_avltree(tree, "Cucumber");
    insert_avltree(tree, "Fox");
    insert_avltree(tree, "Echo");
    insert_avltree(tree, "Golf");
    insert_avltree(tree, "Hotel");
    string_v* vec = avltree_values(tree, "Cucumber", GTE);
    sort_vector(vec, BUBBLE, FORWARD);
    char *a[7] = {"Cucumber", "Cucumber", "Cucumber", "Echo", "Fox", "Golf", "Hotel"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_avltree(tree);
    free_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_and_char_avltree(void **state) {
    charAVLTree* tree = init_avltree(dChar)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    char_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    char b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    print(vec);
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_uchar_avltree(void **state) {
    ucharAVLTree* tree = init_avltree(dUChar)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    uchar_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    unsigned char b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_short_avltree(void **state) {
    shortAVLTree* tree = init_avltree(dShort)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    short_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    short int b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_ushort_avltree(void **state) {
    ushortAVLTree* tree = init_avltree(dUShort)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    ushort_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    unsigned short int b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_int_avltree(void **state) {
    intAVLTree* tree = init_avltree(dInt)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    int_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    int b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_uint_avltree(void **state) {
    uintAVLTree* tree = init_avltree(dUInt)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    uint_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    unsigned int b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_long_avltree(void **state) {
    longAVLTree* tree = init_avltree(dLong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    long_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    long int b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_ulong_avltree(void **state) {
    ulongAVLTree* tree = init_avltree(dULong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    ulong_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    unsigned long int b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_llong_avltree(void **state) {
    llongAVLTree* tree = init_avltree(dLongLong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    llong_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    long long int b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_ullong_avltree(void **state) {
    ullongAVLTree* tree = init_avltree(dULongLong)(true);
    insert_avltree(tree, 1);
    insert_avltree(tree, 2);
    insert_avltree(tree, 3);
    insert_avltree(tree, 3);
    insert_avltree(tree, 4);
    insert_avltree(tree, 5);
    insert_avltree(tree, 6);
    insert_avltree(tree, 7);
    insert_avltree(tree, 7);
    ullong_v* vec = avltree_and_or(tree, 3, 7, GTE_AND_LT);
    unsigned long long int b[5] = {3, 3, 4, 5, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], vec->data[i]);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_float_avltree(void **state) {
    floatAVLTree* tree = init_avltree(dFloat)(true);
    insert_avltree(tree, 1.f);
    insert_avltree(tree, 2.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 3.f);
    insert_avltree(tree, 4.f);
    insert_avltree(tree, 5.f);
    insert_avltree(tree, 6.f);
    insert_avltree(tree, 7.f);
    insert_avltree(tree, 7.f);
    float_v* vec = avltree_and_or(tree, 3.f, 7.f, GTE_AND_LT);
    float b[5] = {3., 3., 4., 5., 6.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(b[i], vec->data[i], 1.0e-3);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_double_avltree(void **state) {
    doubleAVLTree* tree = init_avltree(dDouble)(true);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    insert_avltree(tree, 6.);
    insert_avltree(tree, 7.);
    insert_avltree(tree, 7.);
    double_v* vec = avltree_and_or(tree, 3., 7., GTE_AND_LT);
    double b[5] = {3., 3., 4., 5., 6.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(b[i], vec->data[i], 1.0e-3);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_ldouble_avltree(void **state) {
    ldoubleAVLTree* tree = init_avltree(dLDouble)(true);
    insert_avltree(tree, 1.);
    insert_avltree(tree, 2.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 3.);
    insert_avltree(tree, 4.);
    insert_avltree(tree, 5.);
    insert_avltree(tree, 6.);
    insert_avltree(tree, 7.);
    insert_avltree(tree, 7.);
    ldouble_v* vec = avltree_and_or(tree, 3., 7., GTE_AND_LT);
    long double b[5] = {3., 3., 4., 5., 6.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(b[i], vec->data[i], 1.0e-3);
    }
    free_avltree(tree);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_and_string_avltree(void **state) {
    stringAVLTree* tree = init_avltree(dString)(true);
    insert_avltree(tree, "Alpha");
    insert_avltree(tree, "Bravo");
    insert_avltree(tree, "Charlie");
    insert_avltree(tree, "Charlie");
    insert_avltree(tree, "Delta");
    insert_avltree(tree, "Echo");
    insert_avltree(tree, "Foxtrot");
    insert_avltree(tree, "Golf");
    insert_avltree(tree, "Golf");
    string_v* vec = avltree_and_or(tree, "Charlie", "Golf", GTE_AND_LT);
    char *b[6] = {"Charlie", "Charlie", "Delta", "Echo", "Foxtrot"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(b[i], vec->data[i].data);
    }
    free_avltree(tree);
    free_vector(vec);
}
// ================================================================================ 
// ================================================================================ 
// eof
