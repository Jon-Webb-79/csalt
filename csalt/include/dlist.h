// ================================================================================
// ================================================================================
// - File:    dlist.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 09, 2024
// - Version: 1.0
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef dlist_H
#define dlist_H

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <stdint.h>
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
 * @brief A macro to create node structs for a doubly linked list.
 *
 * @param type The data type to be used in the node 
 * @param dat_type The typedef name for the node 
 *
 * @attribute data The data to be stored in a node 
 * @attribute next A pointer to the next node 
 * @attribute prev A pointer to the previous node
 */
#define DLIST_NODE(type, dat_type) \
    typedef struct dat_type {type data; struct dat_type *next; struct dat_type *prev;} dat_type
// --------------------------------------------------------------------------------

DLIST_NODE(char, char_dlnode);
DLIST_NODE(unsigned char, uchar_dlnode);
DLIST_NODE(short int, short_dlnode);
DLIST_NODE(unsigned short int, ushort_dlnode);
DLIST_NODE(int, int_dlnode);
DLIST_NODE(unsigned int, uint_dlnode);
DLIST_NODE(long int, long_dlnode);
DLIST_NODE(unsigned long int, ulong_dlnode);
DLIST_NODE(long long int, llong_dlnode);
DLIST_NODE(unsigned long long int, ullong_dlnode);
DLIST_NODE(float, float_dlnode);
DLIST_NODE(double, double_dlnode);
DLIST_NODE(long double, ldouble_dlnode);
DLIST_NODE(bool, bool_dlnode);
DLIST_NODE(str*, string_dlnode);
// ================================================================================
// ================================================================================
// Doubly Linked List data type structs 

/**
 * @brief A macro to create a struct containing information to track the attributes
 *        of a doubly linked list
 *
 * @param type The data type that will be assigned to the data pointer.
 * @param dat_type The name given to the typedef for the struct.
 *
 * @attribute len The length of the singly linked list 
 * @attribute head A pointer to the head of the singly linked list
 */
#define DLIST_STRUCT(type, dat_type) \
    typedef struct { size_t len; struct type* head; struct type* tail;} dat_type
// --------------------------------------------------------------------------------

DLIST_STRUCT(char_dlnode, char_dl);
DLIST_STRUCT(uchar_dlnode, uchar_dl);
DLIST_STRUCT(short_dlnode, short_dl);
DLIST_STRUCT(ushort_dlnode, ushort_dl);
DLIST_STRUCT(int_dlnode, int_dl);
DLIST_STRUCT(uint_dlnode, uint_dl);
DLIST_STRUCT(long_dlnode, long_dl);
DLIST_STRUCT(ulong_dlnode, ulong_dl);
DLIST_STRUCT(llong_dlnode, llong_dl);
DLIST_STRUCT(ullong_dlnode, ullong_dl);
DLIST_STRUCT(float_dlnode, float_dl);
DLIST_STRUCT(double_dlnode, double_dl);
DLIST_STRUCT(ldouble_dlnode, ldouble_dl);
DLIST_STRUCT(bool_dlnode, bool_dl);
DLIST_STRUCT(string_dlnode, string_dl);
// ================================================================================
// ================================================================================

/**
 * @brief typedef function pointer to support init_dllist functions
 */
typedef void* (*InitDListFunc)();

/**
 * @brief Function that selects the correct doubly linked list initialization function 
 *        based on type 
 *
 * @return A doubly linked list initialization function for a specific data type.
 */
InitDListFunc init_dllist(dtype dat_type);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a doubly linked list
 *
 * @return A char_dl struct 
 */
char_dl* init_char_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a char struct for a dynamically allocated array
 *
 * @return A uchar_sl struct 
 */
uchar_dl* init_uchar_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A short_sl struct 
 */
short_dl* init_short_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A ushort_sl struct 
 */
ushort_dl* init_ushort_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A int_sl struct 
 */
int_dl* init_int_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A uint_sl struct 
 */
uint_dl* init_uint_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A long_sl struct 
 */
long_dl* init_long_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A ulong_sl struct 
 */
ulong_dl* init_ulong_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A llong_sl struct 
 */
llong_dl* init_llong_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A llong_sl struct 
 */
ullong_dl* init_ullong_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A float_sl struct 
 */
float_dl* init_float_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A double_sl struct 
 */
double_dl* init_double_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A ldouble_sl struct 
 */
ldouble_dl* init_ldouble_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A bool_sl struct 
 */
bool_dl* init_bool_dllist();
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @return A string_sl struct 
 */
string_dl* init_string_dllist();
// ================================================================================
// ================================================================================

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type char_dl 
 * @param dat Data to be pushed to the head of type char
 * @return True if the function executes properly, false otherwise.
 */
bool push_char_front_dllist(char_dl* list, char dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type unsigned uchar_dl 
 * @param dat Data to be pushed to the head of type unsigned char
 * @return True if the function executes properly, false otherwise.
 */
bool push_uchar_front_dllist(uchar_dl* list, unsigned char dat);
// -------------------------------------------------------------------------------- 

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type short_dl
 * @param dat Data to be pushed to the head of type short int
 * @return True if the function executes properly, false otherwise.
 */
bool push_short_front_dllist(short_dl* list, short int dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type ushort_dl 
 * @param dat Data to be pushed to the head of type unsigned short int
 * @return True if the function executes properly, false otherwise.
 */
bool push_ushort_front_dllist(ushort_dl* list, unsigned short int dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type int_dl
 * @param dat Data to be pushed to the head of type int
 * @return True if the function executes properly, false otherwise.
 */
bool push_int_front_dllist(int_dl* list, int dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type uint_dl
 * @param dat Data to be pushed to the head of type unsigned int
 * @return True if the function executes properly, false otherwise.
 */
bool push_uint_front_dllist(uint_dl* list, unsigned int dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type long_dl 
 * @param dat Data to be pushed to the head of type long int
 * @return True if the function executes properly, false otherwise.
 */
bool push_long_front_dllist(long_dl* list, long int dat);
// -------------------------------------------------------------------------------- 

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type ulong_dl 
 * @param dat Data to be pushed to the head of type unsigned long
 * @return True if the function executes properly, false otherwise.
 */
bool push_ulong_front_dllist(ulong_dl* list, unsigned long int dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type llong_dl 
 * @param dat Data to be pushed to the head of type long long int
 * @return True if the function executes properly, false otherwise.
 */
bool push_llong_front_dllist(llong_dl* list, long long int dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type ullong_dl 
 * @param dat Data to be pushed to the head of type unsigned long long int
 * @return True if the function executes properly, false otherwise.
 */
bool push_ullong_front_dllist(ullong_dl* list, unsigned long long int dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type float _dl
 * @param dat Data to be pushed to the head of type float
 * @return True if the function executes properly, false otherwise.
 */
bool push_float_front_dllist(float_dl* list, float dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type double_dl 
 * @param dat Data to be pushed to the head of type double
 * @return True if the function executes properly, false otherwise.
 */
bool push_double_front_dllist(double_dl* list, double dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type ldouble_dl 
 * @param dat Data to be pushed to the head of type long double
 * @return True if the function executes properly, false otherwise.
 */
bool push_ldouble_front_dllist(ldouble_dl* list, long double dat);
// -------------------------------------------------------------------------------- 

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type bool_dl
 * @param dat Data to be pushed to the head of type bool
 * @return True if the function executes properly, false otherwise.
 */
bool push_bool_front_dllist(bool_dl* list, bool dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type bool_dl 
 * @param dat Data to be pushed to the head of type char*
 * @return True if the function executes properly, false otherwise.
 */
bool push_string_front_dllist(string_dl* list, char *dat);
// --------------------------------------------------------------------------------

/**
 * @brief pushes data to the head of the doubly lined list 
 *
 * @param list A doubly linked list data structure of type string_dl 
 * @param dat Data to be pushed to the head of type str
 * @return True if the function executes properly, false otherwise.
 */
bool push_str_front_dllist(string_dl* list, str *dat);
// --------------------------------------------------------------------------------

#define push_front_dllist(list, dat) _Generic((list), \
    char_dl*: push_char_front_dllist, \
    uchar_dl*: push_uchar_front_dllist, \
    short_dl*: push_short_front_dllist, \
    ushort_dl*: push_ushort_front_dllist, \
    int_dl*: push_int_front_dllist, \
    uint_dl*: push_uint_front_dllist, \
    long_dl*: push_long_front_dllist, \
    ulong_dl*: push_ulong_front_dllist, \
    llong_dl*: push_llong_front_dllist, \
    ullong_dl*: push_ullong_front_dllist, \
    float_dl*: push_float_front_dllist, \
    double_dl*: push_double_front_dllist, \
    ldouble_dl*: push_ldouble_front_dllist, \
    bool_dl*: push_bool_front_dllist, \
    string_dl*: push_string_front_dllist) (list, dat);
// ================================================================================
// ================================================================================

void free_char_dllist(char_dl* list);
void free_uchar_dllist(uchar_dl* list);
void free_short_dllist(short_dl* list);
void free_ushort_dllist(ushort_dl* list);
void free_int_dllist(int_dl* list);
void free_uint_dllist(uint_dl* list);
void free_long_dllist(long_dl* list);
void free_ulong_dllist(ulong_dl* list);
void free_llong_dllist(llong_dl* list);
void free_ullong_dllist(ullong_dl* list);
void free_float_dllist(float_dl* list);
void free_double_dllist(double_dl* list);
void free_ldouble_dllist(ldouble_dl* list);
void free_bool_dllist(bool_dl* list);
void free_string_dllist(string_dl* list);
// --------------------------------------------------------------------------------

#define free_dllist(list) _Generic((list), \
    char_dl*: free_char_dllist, \
    uchar_dl*: free_uchar_dllist, \
    short_dl*: free_short_dllist, \
    ushort_dl*: free_ushort_dllist, \
    int_dl*: free_int_dllist, \
    uint_dl*: free_uint_dllist, \
    long_dl*: free_long_dllist, \
    ulong_dl*: free_ulong_dllist, \
    llong_dl*: free_llong_dllist, \
    ullong_dl*: free_ullong_dllist, \
    float_dl*: free_float_dllist, \
    double_dl*: free_double_dllist, \
    ldouble_dl*: free_ldouble_dllist, \
    bool_dl*: free_bool_dllist, \
    string_dl*: free_string_dllist)(list)
// ================================================================================
// ================================================================================

size_t size_char_dlist(char_dl* list);
size_t size_uchar_dlist(uchar_dl* list);
size_t size_short_dlist(short_dl* list);
size_t size_ushort_dlist(ushort_dl* list);
size_t size_int_dlist(int_dl* list);
size_t size_uint_dlist(uint_dl* list);
size_t size_long_dlist(long_dl* list);
size_t size_ulong_dlist(ulong_dl* list);
size_t size_llong_dlist(llong_dl* list);
size_t size_ullong_dlist(ullong_dl* list);
size_t size_float_dlist(float_dl* list);
size_t size_double_dlist(double_dl* list);
size_t size_ldouble_dlist(ldouble_dl* list);
size_t size_bool_dlist(bool_dl* list);
size_t size_string_dlist(string_dl* list);
// --------------------------------------------------------------------------------

#define dlist_size(list) _Generic((list), \
    char_dl*: size_char_dlist, \
    uchar_dl*: size_uchar_dlist, \
    short_dl*: size_short_dlist, \
    ushort_dl*: size_ushort_dlist, \
    int_dl*: size_int_dlist, \
    uint_dl*: size_uint_dlist, \
    long_dl*: size_long_dlist, \
    ulong_dl*: size_ulong_dlist, \
    llong_dl*: size_llong_dlist, \
    ullong_dl*: size_ullong_dlist, \
    float_dl*: size_float_dlist, \
    double_dl*: size_double_dlist, \
    ldouble_dl*: size_ldouble_dlist, \
    bool_dl*: size_bool_dlist, \
    string_dl*: size_string_dlist)(list)
// ================================================================================
// ================================================================================
// Cleanup functions for garbage collection
// - while being public functions, theese are not mean to be accessed directly
//   by developers, and instead are prototypes for the garbage collection macros.

void _free_char_dllist(char_dl** vec);
void _free_uchar_dllist(uchar_dl** vec);
void _free_short_dllist(short_dl** vec);
void _free_ushort_dllist(ushort_dl** vec);
void _free_int_dllist(int_dl** vec);
void _free_uint_dllist(uint_dl** vec);
void _free_long_dllist(long_dl** vec);
void _free_ulong_dllist(ulong_dl** vec);
void _free_llong_dllist(llong_dl** vec);
void _free_ullong_dllist(ullong_dl** vec);
void _free_float_dllist(float_dl** vec);
void _free_double_dllist(double_dl** vec);
void _free_ldouble_dllist(ldouble_dl** vec);
void _free_bool_dllist(bool_dl** vec);
void _free_sstring_dllist(string_dl** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
#define gbc_char_dl __attribute__((cleanup(_free_char_dllist)))
#define gbc_uchar_dl __attribute__((cleanup(_free_uchar_dllist)))
#define gbc_short_dl __attribute__((cleanup(_free_short_dllist)))
#define gbc_ushort_dl __attribute__((cleanup(_free_ushort_dllist)))
#define gbc_int_dl __attribute__((cleanup(_free_int_dllist)))
#define gbc_uint_dl __attribute__((cleanup(_free_uint_dllist)))
#define gbc_long_dl __attribute__((cleanup(_free_long_dllist)))
#define gbc_ulong_dl __attribute__((cleanup(_free_ulong_dllist)))
#define gbc_llong_dl __attribute__((cleanup(_free_llong_dllist)))
#define gbc_ullong_dl __attribute__((cleanup(_free_ullong_dllist)))
#define gbc_float_dl __attribute__((cleanup(_free_float_dllist)))
#define gbc_double_dl __attribute__((cleanup(_free_double_dllist)))
#define gbc_ldouble_dl __attribute__((cleanup(_free_ldouble_dllist)))
#define gbc_bool_dl __attribute__((cleanup(_free_bool_dllist)))
#define gbc_string_dl __attribute__((cleanup(_free_sstring_dllist)))
#endif
// ================================================================================
// ================================================================================

bool push_char_back_dllist(char_dl* list, char dat);
bool push_uchar_back_dllist(uchar_dl* list, unsigned char dat);
bool push_short_back_dllist(short_dl* list, short int dat);
bool push_ushort_back_dllist(ushort_dl* list, unsigned short int dat);
bool push_int_back_dllist(int_dl* list, int dat);
bool push_uint_back_dllist(uint_dl* list, unsigned int dat);
bool push_long_back_dllist(long_dl* list, long int dat);
bool push_ulong_back_dllist(ulong_dl* list, unsigned long int dat);
bool push_llong_back_dllist(llong_dl* list, long long int dat);
bool push_ullong_back_dllist(ullong_dl* list, unsigned long long int dat);
bool push_float_back_dllist(float_dl* list, float dat);
bool push_double_back_dllist(double_dl* list, double dat);
bool push_ldouble_back_dllist(ldouble_dl* list, long double dat);
bool push_bool_back_dllist(bool_dl* list, bool dat);
bool push_string_back_dllist(string_dl* list, char *dat);
bool push_str_back_dllist(string_dl* list, str *dat);
// --------------------------------------------------------------------------------

#define push_back_dllist(list, dat) _Generic((list), \
    char_dl*: push_char_back_dllist, \
    uchar_dl*: push_uchar_back_dllist, \
    short_dl*: push_short_back_dllist, \
    ushort_dl*: push_ushort_back_dllist, \
    int_dl*: push_int_back_dllist, \
    uint_dl*: push_uint_back_dllist, \
    long_dl*: push_long_back_dllist, \
    ulong_dl*: push_ulong_back_dllist, \
    llong_dl*: push_llong_back_dllist, \
    ullong_dl*: push_ullong_back_dllist, \
    float_dl*: push_float_back_dllist, \
    double_dl*: push_double_back_dllist, \
    ldouble_dl*: push_ldouble_back_dllist, \
    bool_dl*: push_bool_back_dllist, \
    string_dl*: push_string_back_dllist)(list, dat)
// ================================================================================
// ================================================================================

bool insert_char_dllist(char_dl* list, char dat, size_t index);
bool insert_uchar_dllist(uchar_dl* list, unsigned char dat, size_t index);
bool insert_short_dllist(short_dl* list, short int dat, size_t index);
bool insert_ushort_dllist(ushort_dl* list, unsigned short int dat, size_t index);
bool insert_int_dllist(int_dl* list, int dat, size_t index);
bool insert_uint_dllist(uint_dl* list, unsigned int dat, size_t index);
bool insert_long_dllist(long_dl* list, long int dat, size_t index);
bool insert_ulong_dllist(ulong_dl* list, unsigned long int dat, size_t index);
bool insert_llong_dllist(llong_dl* list, long long int dat, size_t index);
bool insert_ullong_dllist(ullong_dl* list, unsigned long long int dat, size_t index);
bool insert_float_dllist(float_dl* list, float dat, size_t index);
bool insert_double_dllist(double_dl* list, double dat, size_t index);
bool insert_ldouble_dllist(ldouble_dl* list, long double dat, size_t index);
bool insert_bool_dllist(bool_dl* list, bool dat, size_t index);
bool insert_string_dllist(string_dl* list, char *dat, size_t index);
bool insert_str_dllist(string_dl* list, str *dat, size_t index);
// --------------------------------------------------------------------------------

#define insert_dllist(list, dat, index) _Generic((list), \
    char_dl*: insert_char_dllist, \
    uchar_dl*: insert_uchar_dllist, \
    short_dl*: insert_short_dllist, \
    ushort_dl*: insert_ushort_dllist, \
    int_dl*: insert_int_dllist, \
    uint_dl*: insert_uint_dllist, \
    long_dl*: insert_long_dllist, \
    ulong_dl*: insert_ulong_dllist, \
    llong_dl*: insert_llong_dllist, \
    ullong_dl*: insert_ullong_dllist, \
    float_dl*: insert_float_dllist, \
    double_dl*: insert_double_dllist, \
    ldouble_dl*: insert_ldouble_dllist, \
    bool_dl*: insert_bool_dllist, \
    string_dl*: insert_string_dllist)(list, dat, index)
// ================================================================================
// ================================================================================

char pop_char_front_dllist(char_dl* list);
unsigned char pop_uchar_front_dllist(uchar_dl* list);
short int pop_short_front_dllist(short_dl* list);
unsigned short int pop_ushort_front_dllist(ushort_dl* list);
int pop_int_front_dllist(int_dl* list);
unsigned int pop_uint_front_dllist(uint_dl* list);
long int pop_long_front_dllist(long_dl* list);
unsigned long int pop_ulong_front_dllist(ulong_dl* list);
long long int pop_llong_front_dllist(llong_dl* list);
unsigned long long int pop_ullong_front_dllist(ullong_dl* list);
float pop_float_front_dllist(float_dl* list);
double pop_double_front_dllist(double_dl* list);
long double pop_ldouble_front_dllist(ldouble_dl* list);
bool pop_bool_front_dllist(bool_dl* list);
str* pop_string_front_dllist(string_dl* list);
// --------------------------------------------------------------------------------

#define pop_front_dllist(list) _Generic((list), \
    char_dl*: pop_char_front_dllist, \
    uchar_dl*: pop_uchar_front_dllist, \
    short_dl*: pop_short_front_dllist, \
    ushort_dl*: pop_ushort_front_dllist, \
    int_dl*: pop_int_front_dllist, \
    uint_dl*: pop_uint_front_dllist, \
    long_dl*: pop_long_front_dllist, \
    ulong_dl*: pop_ulong_front_dllist, \
    llong_dl*: pop_llong_front_dllist, \
    ullong_dl*: pop_ullong_front_dllist, \
    float_dl*: pop_float_front_dllist, \
    double_dl*: pop_double_front_dllist, \
    ldouble_dl*: pop_ldouble_front_dllist, \
    bool_dl*: pop_bool_front_dllist, \
    string_dl*: pop_string_front_dllist)(list)
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* dlist_H */
// ================================================================================
// ================================================================================
// eof
