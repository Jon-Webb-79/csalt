// ================================================================================
// ================================================================================
// - File:    test_hash.c
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

#include "test_hash.h"

void test_char_hash_map(void **state)
{
	(void) state;
    charHashTable* table = init_hash_map(dChar)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_uchar_hash_map(void **state)
{
	(void) state;
    ucharHashTable* table = init_hash_map(dUChar)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_short_hash_map(void **state)
{
	(void) state;
    shortHashTable* table = init_hash_map(dShort)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_ushort_hash_map(void **state)
{
	(void) state;
    ushortHashTable* table = init_hash_map(dUShort)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_int_hash_map(void **state)
{
	(void) state;
    intHashTable* table = init_hash_map(dInt)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_uint_hash_map(void **state)
{
	(void) state;
    uintHashTable* table = init_hash_map(dUInt)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_long_hash_map(void **state)
{
	(void) state;
    longHashTable* table = init_hash_map(dChar)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_ulong_hash_map(void **state)
{
	(void) state;
    ulongHashTable* table = init_hash_map(dULong)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_llong_hash_map(void **state)
{
	(void) state;
    llongHashTable* table = init_hash_map(dLongLong)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_ullong_hash_map(void **state)
{
	(void) state;
    ullongHashTable* table = init_hash_map(dULongLong)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_float_hash_map(void **state)
{
	(void) state;
    floatHashTable* table = init_hash_map(dFloat)(20);
    insert_hash_map(table, "One", 1.f);
    insert_hash_map(table, "Two", 2.f);
    insert_hash_map(table, "Three", 3.f);
    insert_hash_map(table, "Four", 4.f);
    insert_hash_map(table, "Five", 5.f);
    assert_float_equal(get_hash_value(table, "One"), 1.f, 1.0e-3);
    assert_float_equal(get_hash_value(table, "Two"), 2.f, 1.0e-3);
    assert_float_equal(get_hash_value(table, "Three"), 3.f, 1.0e-3);
    assert_float_equal(get_hash_value(table, "Four"), 4.f, 1.0e-3);
    assert_float_equal(get_hash_value(table, "Five"), 5.f, 1.0e-3);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_double_hash_map(void **state)
{
	(void) state;
    doubleHashTable* table = init_hash_map(dDouble)(20);
    insert_hash_map(table, "One", 1.);
    insert_hash_map(table, "Two", 2.);
    insert_hash_map(table, "Three", 3.);
    insert_hash_map(table, "Four", 4.);
    insert_hash_map(table, "Five", 5.);
    assert_double_equal(get_hash_value(table, "One"), 1.0, 1.0e-3);
    assert_double_equal(get_hash_value(table, "Two"), 2., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Three"), 3, 1.0e-3);
    assert_double_equal(get_hash_value(table, "Four"), 4., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Five"), 5., 1.0e-3);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_ldouble_hash_map(void **state)
{
	(void) state;
    ldoubleHashTable* table = init_hash_map(dLDouble)(20);
    insert_hash_map(table, "One", 1.);
    insert_hash_map(table, "Two", 2.);
    insert_hash_map(table, "Three", 3.);
    insert_hash_map(table, "Four", 4.);
    insert_hash_map(table, "Five", 5.);
    assert_double_equal(get_hash_value(table, "One"), 1., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Two"), 2., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Three"), 3., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Four"), 4., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Five"), 5., 1.0e-3);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_bool_hash_map(void **state)
{
	(void) state;
    boolHashTable* table = init_hash_map(dBool)(20);
    insert_hash_map(table, "One", true);
    insert_hash_map(table, "Two", true);
    insert_hash_map(table, "Three", true);
    insert_hash_map(table, "Four", false);
    insert_hash_map(table, "Five", false);
    assert_int_equal(get_hash_value(table, "One"), true);
    assert_int_equal(get_hash_value(table, "Two"), true);
    assert_int_equal(get_hash_value(table, "Three"), true);
    assert_int_equal(get_hash_value(table, "Four"), false);
    assert_int_equal(get_hash_value(table, "Five"), false);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// --------------------------------------------------------------------------------

void test_string_hash_map(void **state)
{
	(void) state;
    stringHashTable* table = init_hash_map(dString)(20);
    insert_hash_map(table, "One", "One");
    insert_hash_map(table, "Two", "Two");
    insert_hash_map(table, "Three", "Three");
    insert_hash_map(table, "Four", "Four");
    insert_hash_map(table, "Five", "Five");
    assert_string_equal(get_hash_value(table, "One")->data, "One");
    assert_string_equal(get_hash_value(table, "Two")->data, "Two");
    assert_string_equal(get_hash_value(table, "Three")->data, "Three");
    assert_string_equal(get_hash_value(table, "Four")->data, "Four");
    assert_string_equal(get_hash_value(table, "Five")->data, "Five");
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
    free_hash_map(table);
}
// ================================================================================
// ================================================================================

void test_char_hash_map_gbc(void **state)
{
	(void) state;
    charHashTable* table gbc_char_map = init_hash_map(dChar)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_uchar_hash_map_gbc(void **state)
{
	(void) state;
    ucharHashTable* table gbc_uchar_map = init_hash_map(dUChar)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_short_hash_map_gbc(void **state)
{
	(void) state;
    shortHashTable* table gbc_short_map = init_hash_map(dShort)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_ushort_hash_map_gbc(void **state)
{
	(void) state;
    ushortHashTable* table gbc_ushort_map = init_hash_map(dUShort)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_int_hash_map_gbc(void **state)
{
	(void) state;
    intHashTable* table gbc_int_map = init_hash_map(dInt)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_uint_hash_map_gbc(void **state)
{
	(void) state;
    uintHashTable* table gbc_uint_map = init_hash_map(dUInt)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_long_hash_map_gbc(void **state)
{
	(void) state;
    longHashTable* table gbc_long_map = init_hash_map(dChar)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_ulong_hash_map_gbc(void **state)
{
	(void) state;
    ulongHashTable* table gbc_ulong_map = init_hash_map(dULong)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_llong_hash_map_gbc(void **state)
{
	(void) state;
    llongHashTable* table gbc_llong_map = init_hash_map(dLongLong)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_ullong_hash_map_gbc(void **state)
{
	(void) state;
    ullongHashTable* table gbc_ullong_map = init_hash_map(dULongLong)(20);
    insert_hash_map(table, "One", 1);
    insert_hash_map(table, "Two", 2);
    insert_hash_map(table, "Three", 3);
    insert_hash_map(table, "Four", 4);
    insert_hash_map(table, "Five", 5);
    assert_int_equal(get_hash_value(table, "One"), 1);
    assert_int_equal(get_hash_value(table, "Two"), 2);
    assert_int_equal(get_hash_value(table, "Three"), 3);
    assert_int_equal(get_hash_value(table, "Four"), 4);
    assert_int_equal(get_hash_value(table, "Five"), 5);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_float_hash_map_gbc(void **state)
{
	(void) state;
    floatHashTable* table gbc_float_map = init_hash_map(dFloat)(20);
    insert_hash_map(table, "One", 1.f);
    insert_hash_map(table, "Two", 2.f);
    insert_hash_map(table, "Three", 3.f);
    insert_hash_map(table, "Four", 4.f);
    insert_hash_map(table, "Five", 5.f);
    assert_float_equal(get_hash_value(table, "One"), 1.f, 1.0e-3);
    assert_float_equal(get_hash_value(table, "Two"), 2.f, 1.0e-3);
    assert_float_equal(get_hash_value(table, "Three"), 3.f, 1.0e-3);
    assert_float_equal(get_hash_value(table, "Four"), 4.f, 1.0e-3);
    assert_float_equal(get_hash_value(table, "Five"), 5.f, 1.0e-3);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_double_hash_map_gbc(void **state)
{
	(void) state;
    doubleHashTable* table gbc_double_map = init_hash_map(dDouble)(20);
    insert_hash_map(table, "One", 1.);
    insert_hash_map(table, "Two", 2.);
    insert_hash_map(table, "Three", 3.);
    insert_hash_map(table, "Four", 4.);
    insert_hash_map(table, "Five", 5.);
    assert_double_equal(get_hash_value(table, "One"), 1.0, 1.0e-3);
    assert_double_equal(get_hash_value(table, "Two"), 2., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Three"), 3, 1.0e-3);
    assert_double_equal(get_hash_value(table, "Four"), 4., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Five"), 5., 1.0e-3);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_ldouble_hash_map_gbc(void **state)
{
	(void) state;
    ldoubleHashTable* table gbc_ldouble_map = init_hash_map(dLDouble)(20);
    insert_hash_map(table, "One", 1.);
    insert_hash_map(table, "Two", 2.);
    insert_hash_map(table, "Three", 3.);
    insert_hash_map(table, "Four", 4.);
    insert_hash_map(table, "Five", 5.);
    assert_double_equal(get_hash_value(table, "One"), 1., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Two"), 2., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Three"), 3., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Four"), 4., 1.0e-3);
    assert_double_equal(get_hash_value(table, "Five"), 5., 1.0e-3);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_bool_hash_map_gbc(void **state)
{
	(void) state;
    boolHashTable* table gbc_bool_map = init_hash_map(dBool)(20);
    insert_hash_map(table, "One", true);
    insert_hash_map(table, "Two", true);
    insert_hash_map(table, "Three", true);
    insert_hash_map(table, "Four", false);
    insert_hash_map(table, "Five", false);
    assert_int_equal(get_hash_value(table, "One"), true);
    assert_int_equal(get_hash_value(table, "Two"), true);
    assert_int_equal(get_hash_value(table, "Three"), true);
    assert_int_equal(get_hash_value(table, "Four"), false);
    assert_int_equal(get_hash_value(table, "Five"), false);
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// --------------------------------------------------------------------------------

void test_string_hash_map_gbc(void **state)
{
	(void) state;
    stringHashTable* table gbc_string_map = init_hash_map(dString)(20);
    insert_hash_map(table, "One", "One");
    insert_hash_map(table, "Two", "Two");
    insert_hash_map(table, "Three", "Three");
    insert_hash_map(table, "Four", "Four");
    insert_hash_map(table, "Five", "Five");
    assert_string_equal(get_hash_value(table, "One")->data, "One");
    assert_string_equal(get_hash_value(table, "Two")->data, "Two");
    assert_string_equal(get_hash_value(table, "Three")->data, "Three");
    assert_string_equal(get_hash_value(table, "Four")->data, "Four");
    assert_string_equal(get_hash_value(table, "Five")->data, "Five");
    assert_int_equal(hash_map_alloc(table), 8);
    assert_int_equal(hash_map_size(table), 4);
    assert_int_equal(hash_map_hashSize(table), 5);
}
// ================================================================================
// ================================================================================
// eof
