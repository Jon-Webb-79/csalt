// ================================================================================
// ================================================================================
// - File:    hash.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 13, 2024
// - Version: 1.0
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef hash_H
#define hash_H

#include <stdlib.h>

#include "str.h"
#include "admin.h"
// ================================================================================
// ================================================================================

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================

#define DEFINE_NODE_TYPE(TYPE, TYPENAME)        \
    typedef struct TYPENAME##Node {             \
        char* key;                              \
        TYPE value;                             \
        struct TYPENAME##Node* next;            \
    } TYPENAME##Node;
// --------------------------------------------------------------------------------

DEFINE_NODE_TYPE(char, char)
DEFINE_NODE_TYPE(unsigned char, uchar)
DEFINE_NODE_TYPE(short int, short)
DEFINE_NODE_TYPE(unsigned short int, ushort)
DEFINE_NODE_TYPE(int, int)
DEFINE_NODE_TYPE(unsigned int, uint)
DEFINE_NODE_TYPE(long int, long)
DEFINE_NODE_TYPE(unsigned long int, ulong)
DEFINE_NODE_TYPE(long long int, llong)
DEFINE_NODE_TYPE(unsigned long long int, ullong)
DEFINE_NODE_TYPE(float, float)
DEFINE_NODE_TYPE(double, double)
DEFINE_NODE_TYPE(long double, ldouble)
DEFINE_NODE_TYPE(bool, bool)
DEFINE_NODE_TYPE(str*, string)
// ================================================================================
// ================================================================================

#define DEFINE_HASH_TABLE_TYPE(TYPENAME)        \
    typedef struct {                            \
        TYPENAME##Node* keyValues;              \
        size_t hash_size;                       \
        size_t size;                            \
        size_t alloc;                           \
    } TYPENAME##HashTable;
// --------------------------------------------------------------------------------

DEFINE_HASH_TABLE_TYPE(char)
DEFINE_HASH_TABLE_TYPE(uchar)
DEFINE_HASH_TABLE_TYPE(short)
DEFINE_HASH_TABLE_TYPE(ushort)
DEFINE_HASH_TABLE_TYPE(int)
DEFINE_HASH_TABLE_TYPE(uint)
DEFINE_HASH_TABLE_TYPE(long)
DEFINE_HASH_TABLE_TYPE(ulong)
DEFINE_HASH_TABLE_TYPE(llong)
DEFINE_HASH_TABLE_TYPE(ullong)
DEFINE_HASH_TABLE_TYPE(float)
DEFINE_HASH_TABLE_TYPE(double)
DEFINE_HASH_TABLE_TYPE(ldouble)
DEFINE_HASH_TABLE_TYPE(bool)
DEFINE_HASH_TABLE_TYPE(string)
// ================================================================================
// ================================================================================

/**
 * @brief Function pointer to create a seemingly type agnostic function
 *
 * @param dat_type An enum of type dtype from admin.h file
 */
typedef void* (*InitHashFunc)(bool);
InitHashFunc init_hash_map(dtype dat_type);


/**
 * @brief functions to create and initialize a hash map data structure
 *
 * @param size A user based estimate for the size of the initial hash map array
 * @return A Hash map data structure of the appropriate type
 */
charHashTable* init_char_hash_map(size_t size);
ucharHashTable* init_uchar_hash_map(size_t size);
shortHashTable* init_short_hash_map(size_t size);
ushortHashTable* init_ushort_hash_map(size_t size);
intHashTable* init_int_hash_map(size_t size);
uintHashTable* init_uint_hash_map(size_t size);
longHashTable* init_long_hash_map(size_t size);
ulongHashTable* init_ulong_hash_map(size_t size);
llongHashTable* init_llong_hash_map(size_t size);
ullongHashTable* init_ullong_hash_map(size_t size);
floatHashTable* init_float_hash_map(size_t size);
doubleHashTable* init_double_hash_map(size_t size);
ldoubleHashTable* init_ldouble_hash_map(size_t size);
stringHashTable* init_string_hash_map(size_t size);
// ================================================================================
// ================================================================================

/**
 * @brief This function inserts data into a hash table 
 *
 * @param table A hash table data structure 
 * @param key The key, must be a string literal 
 * @param value The value associated with a key
 */
void insert_char_hash_map(charHashTable* table, char* key, char value);
void insert_uchar_hash_map(ucharHashTable* table, char* key, unsigned char value);
void insert_short_hash_map(shortHashTable* table, char* key, short int value);
void insert_ushort_hash_map(ushortHashTable* table, char* key, unsigned short int value);
void insert_int_hash_map(intHashTable* table, char* key, int value);
void insert_uint_hash_map(uintHashTable* table, char* key, unsigned int value);
void insert_long_hash_map(longHashTable* table, char* key, long int value);
void insert_ulong_hash_map(ulongHashTable* table, char* key, unsigned long int value);
void insert_llong_hash_map(llongHashTable* table, char* key, long long int value);
void insert_ullong_hash_map(ullongHashTable* table, char* key, unsigned long long int value);
void insert_float_hash_map(floatHashTable* table, char* key, float value);
void insert_double_hash_map(doubleHashTable* table, char* key, double value);
void insert_ldouble_hash_map(ldoubleHashTable* table, char* key, long double value);
void insert_bool_hash_map(boolHashTable* table, char* key, bool value);
void insert_string_hash_map(stringHashTable* table, char* key, char* value);
// --------------------------------------------------------------------------------

#define insert_hash_map(table, key, value) _Generic((table), \
    charHashTable*: insert_char_hash_map, \
    ucharHashTable*: insert_uchar_hash_map, \
    shortHashTable*: insert_short_hash_map, \
    ushortHashTable*: insert_ushort_hash_map, \
    intHashTable*: insert_int_hash_map, \
    uintHashTable*: insert_uint_hash_map, \
    longHashTable*: insert_long_hash_map, \
    ulongHashTable*: insert_ulong_hash_map, \
    llongHashTable*: insert_llong_hash_map, \
    ullongHashTable*: insert_ullong_hash_map, \
    floatHashTable*: insert_float_hash_map, \
    doubleHashTable*: insert_double_hash_map, \
    ldoubleHashTable*: insert_ldouble_hash_map, \
    boolHashTable*: insert_bool_hash_map, \
    stringHashTable*: insert_string_hash_map)(table, key, value)
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* hash_H */
// ================================================================================
// ================================================================================
// eof
