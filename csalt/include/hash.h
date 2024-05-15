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
#include <float.h>

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

char pop_char_hash_map(charHashTable* table, char* key);
unsigned char pop_uchar_hash_map(ucharHashTable* table, char* key);
short int pop_short_hash_map(shortHashTable* table, char* key);
unsigned short int pop_ushort_hash_map(ushortHashTable* table, char* key);
int pop_int_hash_map(intHashTable* table, char* key);
unsigned int pop_uint_hash_map(uintHashTable* table, char* key);
long int pop_long_hash_map(longHashTable* table, char* key);
unsigned long int pop_ulong_hash_map(ulongHashTable* table, char* key);
long long int pop_llong_hash_map(llongHashTable* table, char* key);
unsigned long long int pop_ullong_hash_map(ullongHashTable* table, char* key);
float pop_float_hash_map(floatHashTable* table, char* key);
double pop_double_hash_map(doubleHashTable* table, char* key);
long double pop_ldouble_hash_map(ldoubleHashTable* table, char* key);
bool pop_bool_hash_map(boolHashTable* table, char* key);
str* pop_string_hash_map(stringHashTable* table, char* key);
// --------------------------------------------------------------------------------

#define pop_hash_map(table, key) _Generic((table), \
    charHashTable*: pop_char_hash_map, \
    ucharHashTable*: pop_uchar_hash_map, \
    shortHashTable*: pop_short_hash_map, \
    ushortHashTable*: pop_ushort_hash_map, \
    intHashTable*: pop_int_hash_map, \
    uintHashTable*: pop_uint_hash_map, \
    longHashTable*: pop_long_hash_map, \
    ulongHashTable*: pop_ulong_hash_map, \
    llongHashTable*: pop_llong_hash_map, \
    ullongHashTable*: pop_ullong_hash_map, \
    floatHashTable*: pop_float_hash_map, \
    doubleHashTable*: pop_double_hash_map, \
    ldoubleHashTable*: pop_ldouble_hash_map, \
    boolHashTable*: pop_bool_hash_map, \
    stringHashTable*: pop_string_hash_map) (table, key)
// ================================================================================
// ================================================================================

char get_char_hash_value(charHashTable* table, char* key);
unsigned char get_uchar_hash_value(ucharHashTable* table, char* key);
short int get_short_hash_value(shortHashTable* table, char* key);
unsigned short int get_ushort_hash_value(ushortHashTable* table, char* key);
int get_int_hash_value(intHashTable* table, char* key);
unsigned int get_uint_hash_value(uintHashTable* table, char* key);
long get_long_hash_value(longHashTable* table, char* key);
unsigned long get_ulong_hash_value(ulongHashTable* table, char* key);
long long int get_llong_hash_value(llongHashTable* table, char* key);
unsigned long long int get_ullong_hash_value(ullongHashTable* table, char* key);
float get_float_hash_value(floatHashTable* table, char* key);
double get_double_hash_value(doubleHashTable* table, char* key);
long double get_ldouble_hash_value(ldoubleHashTable* table, char* key);
bool get_bool_hash_value(boolHashTable* table, char* key);
str* get_string_hash_value(stringHashTable* table, char* key);
// --------------------------------------------------------------------------------

#define get_hash_value(table, key) _Generic((table), \
    charHashTable*: get_char_hash_value, \
    ucharHashTable*: get_uchar_hash_value, \
    shortHashTable*: get_short_hash_value, \
    ushortHashTable*: get_ushort_hash_value, \
    intHashTable*: get_int_hash_value, \
    uintHashTable*: get_uint_hash_value, \
    longHashTable*: get_long_hash_value, \
    ulongHashTable*: get_ulong_hash_value, \
    llongHashTable*: get_llong_hash_value, \
    ullongHashTable*: get_ullong_hash_value, \
    floatHashTable*: get_float_hash_value, \
    doubleHashTable*: get_double_hash_value, \
    ldoubleHashTable*: get_ldouble_hash_value, \
    boolHashTable*: get_bool_hash_value, \
    stringHashTable*: get_string_hash_value) (table, key)
// ================================================================================
// ================================================================================

void free_char_hash_map(charHashTable* table);
void free_uchar_hash_map(ucharHashTable* table);
void free_short_hash_map(shortHashTable* table);
void free_ushort_hash_map(ushortHashTable* table);
void free_int_hash_map(intHashTable* table);
void free_uint_hash_map(uintHashTable* table);
void free_long_hash_map(longHashTable* table);
void free_ulong_hash_map(ulongHashTable* table);
void free_llong_hash_map(llongHashTable* table);
void free_ullong_hash_map(ullongHashTable* table);
void free_float_hash_map(floatHashTable* table);
void free_double_hash_map(doubleHashTable* table);
void free_ldouble_hash_map(ldoubleHashTable* table);
void free_bool_hash_map(boolHashTable* table);
void free_string_hash_map(stringHashTable* table);
// --------------------------------------------------------------------------------

#define get_hash_map(table, key) _Generic((table), \
    charHashTable*: free_char_hash_map, \
    ucharHashTable*: free_uchar_hash_map, \
    shortHashTable*: free_short_hash_map, \
    ushortHashTable*: free_ushort_hash_map, \
    intHashTable*: free_int_hash_map, \
    uintHashTable*: free_uint_hash_map, \
    longHashTable*: free_long_hash_map, \
    ulongHashTable*: free_ulong_hash_map, \
    llongHashTable*: free_llong_hash_map, \
    ullongHashTable*: free_ullong_hash_map, \
    floatHashTable*: free_float_hash_map, \
    doubleHashTable*: free_double_hash_map, \
    ldoubleHashTable*: free_ldouble_hash_map, \
    boolHashTable*: free_bool_hash_map, \
    stringHashTable*: free_string_hash_map) (table, key)
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* hash_H */
// ================================================================================
// ================================================================================
// eof
