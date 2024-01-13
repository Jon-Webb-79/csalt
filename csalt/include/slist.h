// ================================================================================
// ================================================================================
// - File:    slist.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 17, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef slist_H
#define slist_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>

#include "str.h"
#include "admin.h"

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================

/**
 * @brief A macro to create node structs for a singly linked list.
 *
 * @param type The data type to be used in the node 
 * @param dat_type The typedef name for the node 
 *
 * @attribute data The data to be stored in a node 
 * @attribute next A pointer to the next node 
 */
#define SLIST_NODE(type, dat_type) \
    typedef struct dat_type {type data; struct dat_type *next;} dat_type
// --------------------------------------------------------------------------------

SLIST_NODE(char, char_slnode);
SLIST_NODE(unsigned char, uchar_slnode);
SLIST_NODE(short int, short_slnode);
SLIST_NODE(unsigned short int, ushort_slnode);
SLIST_NODE(int, int_slnode);
SLIST_NODE(unsigned int, uint_slnode);
SLIST_NODE(long int, long_slnode);
SLIST_NODE(unsigned long int, ulong_slnode);
SLIST_NODE(long long int, llong_slnode);
SLIST_NODE(unsigned long long int, ullong_slnode);
SLIST_NODE(float, float_slnode);
SLIST_NODE(double, double_slnode);
SLIST_NODE(long double, ldouble_slnode);
SLIST_NODE(bool, bool_slnode);
SLIST_NODE(str*, string_slnode);
// ================================================================================
// ================================================================================
// Vector data type structs 

/**
 * @brief A macro to create a struct containing information to track the attributes
 *        of a singly linked list
 *
 * @param type The data type that will be assigned to the data pointer.
 * @param dat_type The name given to the typedef for the struct.
 *
 * @attribute len The length of the singly linked list 
 * @attribute head A pointer to the head of the singly linked list
 */
#define SLIST_STRUCT(type, dat_type) \
    typedef struct { size_t len; struct type* head;} dat_type
// --------------------------------------------------------------------------------

SLIST_STRUCT(char_slnode, char_sl);
SLIST_STRUCT(uchar_slnode, uchar_sl);
SLIST_STRUCT(short_slnode, short_sl);
SLIST_STRUCT(ushort_slnode, ushort_sl);
SLIST_STRUCT(int_slnode, int_sl);
SLIST_STRUCT(uint_slnode, uint_sl);
SLIST_STRUCT(long_slnode, long_sl);
SLIST_STRUCT(ulong_slnode, ulong_sl);
SLIST_STRUCT(llong_slnode, llong_sl);
SLIST_STRUCT(ullong_slnode, ullong_sl);
SLIST_STRUCT(float_slnode, float_sl);
SLIST_STRUCT(double_slnode, double_sl);
SLIST_STRUCT(ldouble_slnode, ldouble_sl);
SLIST_STRUCT(bool_slnode, bool_sl);
SLIST_STRUCT(string_slnode, string_sl);
// ================================================================================
// ================================================================================

/**
 * @brief typedef function pointer to support init_sllist functions
 */
typedef void* (*InitSListFunc)();

/**
 * @brief Function that selects the correct singly linked list initialization function 
 *        based on type 
 *
 * @return A singly linked list initialization function for a specific data type.
 */
InitSListFunc init_sllist(dtype dat_type);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a singly linked list
 *
 * @return A char_sl struct 
 */
char_sl* init_char_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A uchar_sl struct 
 */
uchar_sl* init_uchar_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A short_sl struct 
 */
short_sl* init_short_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A ushort_sl struct 
 */
ushort_sl* init_ushort_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A int_sl struct 
 */
int_sl* init_int_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A uint_sl struct 
 */
uint_sl* init_uint_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A long_sl struct 
 */
long_sl* init_long_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A ulong_sl struct 
 */
ulong_sl* init_ulong_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A llong_sl struct 
 */
llong_sl* init_llong_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A llong_sl struct 
 */
ullong_sl* init_ullong_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A float_sl struct 
 */
float_sl* init_float_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A double_sl struct 
 */
double_sl* init_double_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A ldouble_sl struct 
 */
ldouble_sl* init_ldouble_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A bool_sl struct 
 */
bool_sl* init_bool_sllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A string_sl struct 
 */
string_sl* init_string_sllist();
// ================================================================================
// ================================================================================
// PUSH LINKED LIST DATA 

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true of function executes succesfully, false otherwise
 */
bool push_char_sllist(char_sl* sllist, char data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true of function executes succesfully, false otherwise
 */
bool push_uchar_sllist(uchar_sl* sllist, unsigned char data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_short_sllist(short_sl* sllist, short int data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_ushort_sllist(ushort_sl* sllist, unsigned short int data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_int_sllist(int_sl* sllist, int data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_uint_sllist(uint_sl* sllist, unsigned int data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_long_sllist(long_sl* sllist, long int data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_ulong_sllist(ulong_sl* sllist, unsigned long int data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_llong_sllist(llong_sl* sllist, long long int data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_ullong_sllist(ullong_sl* sllist, unsigned long long int data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_float_sllist(float_sl* sllist, float data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_double_sllist(double_sl* sllist, double data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_ldouble_sllist(ldouble_sl* sllist, long double data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_bool_sllist(bool_sl* sllist, bool data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_string_sllist(string_sl* sllist, char* data, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Adds a node to the singly linked list data with populated data 
 *
 * @param sllist A singly linked list container struct 
 * @param data The data to be added to the linked list 
 * @param index The index that data will be added to.
 * @return true if function executs succesfully, false otherwise.
 */
bool push_str_sllist(string_sl* sllist, str* data, size_t index);
// --------------------------------------------------------------------------------

#define push_sllist(sllist, data, index) _Generic((sllist), \
    char_sl*: push_char_sllist, \
    uchar_sl*: push_uchar_sllist, \
    short_sl*: push_short_sllist, \
    ushort_sl*: push_ushort_sllist, \
    int_sl*: push_int_sllist, \
    uint_sl*: push_uint_sllist, \
    long_sl*: push_long_sllist, \
    ulong_sl*: push_ulong_sllist, \
    llong_sl*: push_llong_sllist, \
    ullong_sl*: push_ullong_sllist, \
    float_sl*: push_float_sllist, \
    double_sl*: push_double_sllist, \
    ldouble_sl*: push_ldouble_sllist, \
    bool_sl*: push_bool_sllist, \
    string_sl*: push_string_sllist) (sllist, data,index)
// ================================================================================
// ================================================================================
// FREE 

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_char_sllist(char_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_uchar_sllist(uchar_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_short_sllist(short_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_ushort_sllist(ushort_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_int_sllist(int_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_uint_sllist(uint_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_long_sllist(long_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_ulong_sllist(ulong_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_llong_sllist(llong_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_ullong_sllist(ullong_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_float_sllist(float_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_double_sllist(double_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_ldouble_sllist(ldouble_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_bool_sllist(bool_sl* sllist);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory from a singly linked list
 *
 * @param sllist A singly linked list data structure
 */
void free_string_sllist(string_sl* sllist);
// --------------------------------------------------------------------------------

#define free_sllist(sllist) _Generic((sllist), \
    char_sl*: free_char_sllist, \
    uchar_sl*: free_uchar_sllist, \
    short_sl*: free_short_sllist, \
    ushort_sl*: free_ushort_sllist, \
    int_sl*: free_int_sllist, \
    uint_sl*: free_uint_sllist, \
    long_sl*: free_long_sllist, \
    ulong_sl*: free_ulong_sllist, \
    llong_sl*: free_llong_sllist, \
    ullong_sl*: free_ullong_sllist, \
    float_sl*: free_float_sllist, \
    double_sl*: free_double_sllist, \
    ldouble_sl*: free_ldouble_sllist, \
    bool_sl*: free_bool_sllist, \
    string_sl*: free_string_sllist) (sllist)
// ================================================================================
// ================================================================================
// Cleanup functions for garbage collection
// - while being public functions, theese are not mean to be accessed directly
//   by developers, and instead are prototypes for the garbage collection macros.

void _free_char_sllist(char_sl** vec);
void _free_uchar_sllist(uchar_sl** vec);
void _free_short_sllist(short_sl** vec);
void _free_ushort_sllist(ushort_sl** vec);
void _free_int_sllist(int_sl** vec);
void _free_uint_sllist(uint_sl** vec);
void _free_long_sllist(long_sl** vec);
void _free_ulong_sllist(ulong_sl** vec);
void _free_llong_sllist(llong_sl** vec);
void _free_ullong_sllist(ullong_sl** vec);
void _free_float_sllist(float_sl** vec);
void _free_double_sllist(double_sl** vec);
void _free_ldouble_sllist(ldouble_sl** vec);
void _free_bool_sllist(bool_sl** vec);
void _free_sstring_sllist(string_sl** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
#define gbc_char_sl __attribute__((cleanup(_free_char_sllist)))
#define gbc_uchar_sl __attribute__((cleanup(_free_uchar_sllist)))
#define gbc_short_sl __attribute__((cleanup(_free_short_sllist)))
#define gbc_ushort_sl __attribute__((cleanup(_free_ushort_sllist)))
#define gbc_int_sl __attribute__((cleanup(_free_int_sllist)))
#define gbc_uint_sl __attribute__((cleanup(_free_uint_sllist)))
#define gbc_long_sl __attribute__((cleanup(_free_long_sllist)))
#define gbc_ulong_sl __attribute__((cleanup(_free_ulong_sllist)))
#define gbc_llong_sl __attribute__((cleanup(_free_llong_sllist)))
#define gbc_ullong_sl __attribute__((cleanup(_free_ullong_sllist)))
#define gbc_float_sl __attribute__((cleanup(_free_float_sllist)))
#define gbc_double_sl __attribute__((cleanup(_free_double_sllist)))
#define gbc_ldouble_sl __attribute__((cleanup(_free_ldouble_sllist)))
#define gbc_bool_sl __attribute__((cleanup(_free_bool_sllist)))
#define gbc_string_sl __attribute__((cleanup(_free_sstring_sllist)))
#endif
// ================================================================================
// ================================================================================
// SINGLY LINKED LIST LENGTH 

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t char_sllist_length(char_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t uchar_sllist_length(uchar_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t short_sllist_length(short_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t ushort_sllist_length(ushort_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t int_sllist_length(int_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t uint_sllist_length(uint_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t long_sllist_length(long_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t ulong_sllist_length(ulong_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t llong_sllist_length(llong_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t ullong_sllist_length(ullong_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t float_sllist_length(float_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t double_sllist_length(double_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t ldouble_sllist_length(ldouble_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t bool_sllist_length(bool_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Extract the length of a statically linked list 
 *
 * @param list A singly linked list data structure 
 * @return the length of a singly linked list.
 */
size_t string_sllist_length(string_sl* list);
// --------------------------------------------------------------------------------

#define sllist_length(sllist) _Generic((sllist), \
    char_sl*: char_sllist_length, \
    uchar_sl*: uchar_sllist_length, \
    short_sl*: short_sllist_length, \
    ushort_sl*: ushort_sllist_length, \
    int_sl*: int_sllist_length, \
    uint_sl*: uint_sllist_length, \
    long_sl*: long_sllist_length, \
    ulong_sl*: ulong_sllist_length, \
    llong_sl*: llong_sllist_length, \
    ullong_sl*: ullong_sllist_length, \
    float_sl*: float_sllist_length, \
    double_sl*: double_sllist_length, \
    ldouble_sl*: ldouble_sllist_length, \
    bool_sl*: bool_sllist_length, \
    string_sl*: string_sllist_length) (sllist)
// ================================================================================
// ================================================================================
// GET SINGLY LINKED LIST VALUE AT INDEX 

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
char get_char_sllist(char_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
unsigned char get_uchar_sllist(uchar_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
short int get_short_sllist(short_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
unsigned short int get_ushort_sllist(ushort_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
int get_int_sllist(int_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
unsigned int get_uint_sllist(uint_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
long int get_long_sllist(long_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
unsigned long int get_ulong_sllist(ulong_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
long long int get_llong_sllist(llong_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
unsigned long long int get_ullong_sllist(ullong_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
float get_float_sllist(float_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
double get_double_sllist(double_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
long double get_ldouble_sllist(ldouble_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
bool get_bool_sllist(bool_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the value at a pseudo index 
 *
 * @param list A singly linked list data structure 
 * @param index The pseudo index where data is extracted from 
 * @return the value at a pseudo index
 */
str* get_string_sllist(string_sl* list, size_t index);
// --------------------------------------------------------------------------------

#define get_sllist(sllist, index) _Generic((sllist), \
    char_sl*: get_char_sllist, \
    uchar_sl*: get_uchar_sllist, \
    short_sl*: get_short_sllist, \
    ushort_sl*: get_ushort_sllist, \
    int_sl*: get_int_sllist, \
    uint_sl*: get_uint_sllist, \
    long_sl*: get_long_sllist, \
    ulong_sl*: get_ulong_sllist, \
    llong_sl*: get_llong_sllist, \
    ullong_sl*: get_ullong_sllist, \
    float_sl*: get_float_sllist, \
    double_sl*: get_double_sllist, \
    ldouble_sl*: get_ldouble_sllist, \
    bool_sl*: get_bool_sllist, \
    string_sl*: get_string_sllist) (sllist, index)
// ================================================================================
// ================================================================================
// POP SINGELY LINKED LIST DATA 

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return a char variable
 */
char pop_char_sllist(char_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return an unsigned char variable
 */
unsigned char pop_uchar_sllist(uchar_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return a short integer variable
 */
short int pop_short_sllist(short_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return an unsigned short int variable
 */
unsigned short int pop_ushort_sllist(ushort_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return an integer variable
 */
int pop_int_sllist(int_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return an unsigned integer variable
 */
unsigned int pop_uint_sllist(uint_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return a long int variable
 */
long int pop_long_sllist(long_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return an unsigned long integer variable
 */
unsigned long int pop_ulong_sllist(ulong_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return a long long integer variable
 */
long long int pop_llong_sllist(llong_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return an unsigned long long integer variable
 */
unsigned long long int pop_ullong_sllist(ullong_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return a float variable
 */
float pop_float_sllist(float_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return a double variable
 */
double pop_double_sllist(double_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return a long double variable
 */
long double pop_ldouble_sllist(ldouble_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return a bool variable
 */
bool pop_bool_sllist(bool_sl* list, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a node from a singly linked list and returns the data to a user 
 *
 * @param list A linked list data structure 
 * @param index The index to be removed
 * @return a str variable
 */
str* pop_string_sllist(string_sl* list, size_t index);
// --------------------------------------------------------------------------------

#define pop_sllist(list, index) _Generic((list), \
    char_sl*: pop_char_sllist, \
    uchar_sl*: pop_uchar_sllist, \
    short_sl*: pop_short_sllist, \
    ushort_sl*: pop_ushort_sllist, \
    int_sl*: pop_int_sllist, \
    uint_sl*: pop_uint_sllist, \
    long_sl*: pop_long_sllist, \
    ulong_sl*: pop_ulong_sllist, \
    llong_sl*: pop_llong_sllist, \
    ullong_sl*: pop_ullong_sllist, \
    float_sl*: pop_float_sllist, \
    double_sl*: pop_double_sllist, \
    ldouble_sl*: pop_ldouble_sllist, \
    bool_sl*: pop_bool_sllist, \
    string_sl*: pop_string_sllist) (list, index)
// ================================================================================
// ================================================================================
// REVERSE SINGLY LINKED LIST

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_char_sllist(char_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_uchar_sllist(uchar_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_short_sllist(short_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_ushort_sllist(ushort_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_int_sllist(int_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_uint_sllist(uint_sl* list);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_long_sllist(long_sl* list);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_ulong_sllist(ulong_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_llong_sllist(llong_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_ullong_sllist(ullong_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_float_sllist(float_sl* list);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_double_sllist(double_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_ldouble_sllist(ldouble_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_bool_sllist(bool_sl* list);
// --------------------------------------------------------------------------------

/**
 * @brief Reverses the order of values in a singly linked list.
 *
 * @param list a singly linked list data structure 
 */
void reverse_string_sllist(string_sl* list);
// --------------------------------------------------------------------------------

#define reverse_sllist(list) _Generic((list), \
    char_sl*: reverse_char_sllist, \
    uchar_sl*: reverse_uchar_sllist, \
    short_sl*: reverse_short_sllist, \
    ushort_sl*: reverse_ushort_sllist, \
    int_sl*: reverse_int_sllist, \
    uint_sl*: reverse_uint_sllist, \
    long_sl*: reverse_long_sllist, \
    ulong_sl*: reverse_ulong_sllist, \
    llong_sl*: reverse_llong_sllist, \
    ullong_sl*: reverse_ullong_sllist, \
    float_sl*: reverse_float_sllist, \
    double_sl*: reverse_double_sllist, \
    ldouble_sl*: reverse_ldouble_sllist, \
    bool_sl*: reverse_bool_sllist, \
    string_sl*: reverse_string_sllist) (list)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* slist_H */
// ================================================================================
// ================================================================================
// eof
