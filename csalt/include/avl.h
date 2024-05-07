// ================================================================================
// ================================================================================
// - File:    avl.h
// - Purpose: This file contains prorotypes for the AVL Binary Tree 
//            implementation
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 27, 2024
// - Version: 1.0
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef avl_H
#define avl_H

#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <float.h>

#include "str.h"
#include "admin.h"
#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================  
// Define structs for an AVL Binary Tree 

/** 
 * @brief Generic struct ti help implement generic helper functions
 * 
 * @attribute left A pointer to the left node
 * @attribute right A pointer to the right node 
 * @attribute height The height of a node
 */
typedef struct AVLNode {
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;
// --------------------------------------------------------------------------------

/**
 * @brief A macro to help define the node structs of different types
 *
 * @param type The data type of the node 
 * @param dat_type The name given to the struct typedef 
 * @attribute base A pointer to an AVLNode struct 
 * @attribute data The data contained by the tree node
 */
#define AVL_NODE(type, dat_type) \
typedef struct { \
    AVLNode base; \
    type data; \
} dat_type;

// --------------------------------------------------------------------------------
// AVL_NODE implementations 

AVL_NODE(char, charAVLNode)
AVL_NODE(unsigned char, ucharAVLNode)
AVL_NODE(short int, shortAVLNode)
AVL_NODE(unsigned short int, ushortAVLNode)
AVL_NODE(int, intAVLNode)
AVL_NODE(unsigned int, uintAVLNode)
AVL_NODE(long int, longAVLNode)
AVL_NODE(unsigned long int, ulongAVLNode)
AVL_NODE(long long int, llongAVLNode)
AVL_NODE(unsigned long long int, ullongAVLNode)
AVL_NODE(float, floatAVLNode)
AVL_NODE(double, doubleAVLNode)
AVL_NODE(long double, ldoubleAVLNode)
AVL_NODE(bool, boolAVLNode)
AVL_NODE(str*, stringAVLNode)
// --------------------------------------------------------------------------------

/**
 * @brief A macro to define the struct which encompasses an avl tree 
 *
 * @param dat_type The name to be given to the struct typedef
 * @attribute len The length of the binary tree 
 * @attribute root A pointer to an AVLNode struct 
 * @attribute allow_duplicates true if duplicate values are allowed, otherwise false
 */
#define AVLTREE_STRUCT(dat_type) \
typedef struct { \
    size_t len; \
    AVLNode* root; \
    bool allow_duplicates; \
} dat_type;
// --------------------------------------------------------------------------------
// AVLTREE_STRUCT IMPLEMENTATION 

AVLTREE_STRUCT(charAVLTree)
AVLTREE_STRUCT(ucharAVLTree)
AVLTREE_STRUCT(shortAVLTree)
AVLTREE_STRUCT(ushortAVLTree)
AVLTREE_STRUCT(intAVLTree)
AVLTREE_STRUCT(uintAVLTree)
AVLTREE_STRUCT(longAVLTree)
AVLTREE_STRUCT(ulongAVLTree)
AVLTREE_STRUCT(llongAVLTree)
AVLTREE_STRUCT(ullongAVLTree)
AVLTREE_STRUCT(floatAVLTree)
AVLTREE_STRUCT(doubleAVLTree)
AVLTREE_STRUCT(ldoubleAVLTree)
AVLTREE_STRUCT(boolAVLTree)
AVLTREE_STRUCT(stringAVLTree)
// ================================================================================
// ================================================================================ 
// DEFINE FUNCTIONS TO INSTANTIATE AN AVL BINARY TREE

/**
 * @brief typedef function pointer to support init_dllist functions
 */
typedef void* (*InitAVLFunc)(bool);

/**
 * @brief Function that selects the correct doubly linked list initialization function 
 *        based on type 
 *
 * @return A doubly linked list initialization function for a specific data type.
 */
InitAVLFunc init_avltree(dtype dat_type);

/**
 * @brief instantiates a char binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
charAVLTree* init_char_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates an unsigned char binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
ucharAVLTree* init_uchar_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates a short int binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
shortAVLTree* init_short_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates an unsigned short int binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
ushortAVLTree* init_ushort_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates an int binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
intAVLTree* init_int_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates an unsigned int binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
uintAVLTree* init_uint_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates a long int binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
longAVLTree* init_long_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates an unsigned long int binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
ulongAVLTree* init_ulong_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates a long long int binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
llongAVLTree* init_llong_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates an unsigned long long int binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
ullongAVLTree* init_ullong_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates a float binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
floatAVLTree* init_float_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates a double binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
doubleAVLTree* init_double_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates a long double binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
ldoubleAVLTree* init_ldouble_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates a bool binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
boolAVLTree* init_bool_avltree(bool duplicates);
// --------------------------------------------------------------------------------

/**
 * @brief instantiates a string binary tree 
 *
 * @param duplicates true if duplicate values are allowed, false otherwise
 */
stringAVLTree* init_string_avltree(bool duplicates);
// ================================================================================
// ================================================================================
// INSERT FUNCTIONS 

/**
 * @brief This function inserts char data into a binary tree 
 *
 * @param tree an charAVLTree data structure 
 * @param value An char value
 */
void insert_char_avltree(charAVLTree* tree, char value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts unsigned char data into a binary tree 
 *
 * @param tree an ucharAVLTree data structure 
 * @param value An unsigned char value
 */
void insert_uchar_avltree(ucharAVLTree* tree, unsigned char value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts short int data into a binary tree 
 *
 * @param tree an shortAVLTree data structure 
 * @param value An short int value
 */
void insert_short_avltree(shortAVLTree* tree, short int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts unsigned short int data into a binary tree 
 *
 * @param tree an ushortAVLTree data structure 
 * @param value An unsigned short int value
 */
void insert_ushort_avltree(ushortAVLTree* tree, unsigned short int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts int data into a binary tree 
 *
 * @param tree an intAVLTree data structure 
 * @param value An int value
 */
void insert_int_avltree(intAVLTree* tree, int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts unsigned int data into a binary tree 
 *
 * @param tree an uintAVLTree data structure 
 * @param value An unsigned int value
 */
void insert_uint_avltree(uintAVLTree* tree, unsigned int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts long int data into a binary tree 
 *
 * @param tree an longAVLTree data structure 
 * @param value A long int value
 */
void insert_long_avltree(longAVLTree* tree, long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts a long int data into a binary tree 
 *
 * @param tree an ulongAVLTree data structure 
 * @param value A unsigned long int value
 */
void insert_ulong_avltree(ulongAVLTree* tree, unsigned long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts long long int data into a binary tree 
 *
 * @param tree an llongAVLTree data structure 
 * @param value A long long int value
 */
void insert_llong_avltree(llongAVLTree* tree, long long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts unsigned long long int data into a binary tree 
 *
 * @param tree an ullongAVLTree data structure 
 * @param value An unsigned long long int value
 */
void insert_ullong_avltree(ullongAVLTree* tree, unsigned long long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts float data into a binary tree 
 *
 * @param tree an floatAVLTree data structure 
 * @param value An float value
 */
void insert_float_avltree(floatAVLTree* tree, float value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts double data into a binary tree 
 *
 * @param tree and doubleAVLTree data structure 
 * @param value A double value
 */
void insert_double_avltree(doubleAVLTree* tree, double value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts long double data into a binary tree 
 *
 * @param tree an ldoubleAVLTree data structure 
 * @param value A long double value
 */
void insert_ldouble_avltree(ldoubleAVLTree* tree, long double value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts bool data into a binary tree 
 *
 * @param tree an boolAVLTree data structure 
 * @param value A bool value
 */
void insert_bool_avltree(boolAVLTree* tree, bool value);
// --------------------------------------------------------------------------------

/**
 * @brief This function inserts str data into a binary tree 
 *
 * @param tree an stringAVLTree data structure 
 * @param value A str value
 */
void insert_string_avltree(stringAVLTree* tree, char* value);
// --------------------------------------------------------------------------------

#define insert_avltree(tree, value) _Generic((tree), \
    charAVLTree*: insert_char_avltree, \
    ucharAVLTree*: insert_uchar_avltree, \
    shortAVLTree*: insert_short_avltree, \
    ushortAVLTree*: insert_ushort_avltree, \
    intAVLTree*: insert_int_avltree, \
    uintAVLTree*: insert_uint_avltree, \
    longAVLTree*: insert_long_avltree, \
    ulongAVLTree*: insert_ulong_avltree, \
    llongAVLTree*: insert_llong_avltree, \
    ullongAVLTree*: insert_ullong_avltree, \
    floatAVLTree*: insert_float_avltree, \
    doubleAVLTree*: insert_double_avltree, \
    ldoubleAVLTree*: insert_ldouble_avltree, \
    boolAVLTree*: insert_bool_avltree, \
    stringAVLTree*: insert_string_avltree)(tree,value)
// ================================================================================
// ================================================================================

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A charAVLTree data structure 
 * @param value a char value
 */
void remove_char_avltree(charAVLTree* tree, char value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A ucharAVLTree data structure 
 * @param value an unsigned char value
 */
void remove_uchar_avltree(ucharAVLTree* tree, unsigned char value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A shortAVLTree data structure 
 * @param value a short int value
 */
void remove_short_avltree(shortAVLTree* tree, short int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A ushortAVLTree data structure 
 * @param value a short int value
 */
void remove_ushort_avltree(ushortAVLTree* tree, unsigned short value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A intAVLTree data structure 
 * @param value an int value
 */
void remove_int_avltree(intAVLTree* tree, int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A uintAVLTree data structure 
 * @param value an unsigned int value
 */
void remove_uint_avltree(uintAVLTree* tree, unsigned int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A longAVLTree data structure 
 * @param value an long int value
 */
void remove_long_avltree(longAVLTree* tree, long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A ulongAVLTree data structure 
 * @param value an unsigned long int value
 */
void remove_ulong_avltree(ulongAVLTree* tree, unsigned long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A llongAVLTree data structure 
 * @param value an long long int value
 */
void remove_llong_avltree(llongAVLTree* tree, long long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A ullongAVLTree data structure 
 * @param value an unsigned long long int value
 */
void remove_ullong_avltree(ullongAVLTree* tree, unsigned long long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A floatAVLTree data structure 
 * @param value a float int value
 */
void remove_float_avltree(floatAVLTree* tree, float value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A doubleAVLTree data structure 
 * @param value a double int value
 */
void remove_double_avltree(doubleAVLTree* tree, double value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A ldoubleAVLTree data structure 
 * @param value a long double value
 */
void remove_ldouble_avltree(ldoubleAVLTree* tree, long double value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A boolAVLTree data structure 
 * @param value a bool value
 */
void remove_bool_avltree(boolAVLTree* tree, bool value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will remove a node from a tree if it exists.
 *
 * @param tree A stringAVLTree data structure 
 * @param value a char value
 */
void remove_string_avltree(stringAVLTree* tree, char* value);
// --------------------------------------------------------------------------------

#define remove_avltree(tree, value) _Generic((tree), \
    charAVLTree*: remove_char_avltree, \
    ucharAVLTree*: remove_uchar_avltree, \
    shortAVLTree*: remove_short_avltree, \
    ushortAVLTree*: remove_ushort_avltree, \
    intAVLTree*: remove_int_avltree, \
    uintAVLTree*: remove_uint_avltree, \
    longAVLTree*: remove_long_avltree, \
    ulongAVLTree*: remove_ulong_avltree, \
    llongAVLTree*: remove_llong_avltree, \
    ullongAVLTree*: remove_ullong_avltree, \
    floatAVLTree*: remove_float_avltree, \
    doubleAVLTree*: remove_double_avltree, \
    ldoubleAVLTree*: remove_ldouble_avltree, \
    boolAVLTree*: remove_bool_avltree, \
    stringAVLTree*: remove_string_avltree)(tree, value)
// ================================================================================
// ================================================================================

/**
 * @brief This function will free all memory from a charAVLTree 
 *
 * @param tree A charAVLTree
 */
void free_char_avltree(charAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a ucharAVLTree 
 *
 * @param tree A ucharAVLTree
 */
void free_uchar_avltree(ucharAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a shortAVLTree 
 *
 * @param tree A shortAVLTree
 */
void free_short_avltree(shortAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a ushortAVLTree 
 *
 * @param tree A ushortAVLTree
 */
void free_ushort_avltree(ushortAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a intAVLTree 
 *
 * @param tree A intAVLTree
 */
void free_int_avltree(intAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a uintAVLTree 
 *
 * @param tree A uintAVLTree
 */
void free_uint_avltree(uintAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a longAVLTree 
 *
 * @param tree A longAVLTree
 */
void free_long_avltree(longAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a ulongAVLTree 
 *
 * @param tree A ulongAVLTree
 */
void free_ulong_avltree(ulongAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a llongAVLTree 
 *
 * @param tree A llongAVLTree
 */
void free_llong_avltree(llongAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a ullongAVLTree 
 *
 * @param tree A ullongAVLTree
 */
void free_ullong_avltree(ullongAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a floatAVLTree 
 *
 * @param tree A floatAVLTree
 */
void free_float_avltree(floatAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a doubleAVLTree 
 *
 * @param tree A doubleAVLTree
 */
void free_double_avltree(doubleAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a ldoubleAVLTree 
 *
 * @param tree A ldoubleAVLTree
 */
void free_ldouble_avltree(ldoubleAVLTree* tree);
// --------------------------------------------------------------------------------

/**
 * @brief This function will free all memory from a boolAVLTree 
 *
 * @param tree A boolAVLTree
 */
void free_bool_avltree(boolAVLTree* tree);
// -------------------------------------------------------------------------------- 

/**
 * @brief This function will free all memory from a stringAVLTree 
 *
 * @param tree A stringAVLTree
 */
void free_string_avltree(stringAVLTree* tree);
// --------------------------------------------------------------------------------

#define free_avltree(tree) _Generic((tree), \
    charAVLTree*: free_char_avltree, \
    ucharAVLTree*: free_uchar_avltree, \
    shortAVLTree*: free_short_avltree, \
    ushortAVLTree*: free_ushort_avltree, \
    intAVLTree*: free_int_avltree, \
    uintAVLTree*: free_uint_avltree, \
    longAVLTree*: free_long_avltree, \
    ulongAVLTree*: free_ulong_avltree, \
    llongAVLTree*: free_llong_avltree, \
    ullongAVLTree*: free_ullong_avltree, \
    floatAVLTree*: free_float_avltree, \
    doubleAVLTree*: free_double_avltree, \
    ldoubleAVLTree*: free_ldouble_avltree, \
    boolAVLTree*: free_bool_avltree, \
    stringAVLTree*: free_string_avltree)(tree)
// ================================================================================
// ================================================================================

void _freeCharAvltree(charAVLTree** root);
void _freeUCharAvltree(ucharAVLTree** root);
void _freeShortAvltree(shortAVLTree** root);
void _freeUShortAvltree(ushortAVLTree** root);
void _freeIntAvltree(intAVLTree** root);
void _freeUIntAvltree(uintAVLTree** root);
void _freeLongAvltree(longAVLTree** root);
void _freeULongAvltree(ulongAVLTree** root);
void _freeLLongAvltree(llongAVLTree** root);
void _freeULLongAvltree(ullongAVLTree** root);
void _freeFloatAvltree(floatAVLTree** root);
void _freeDoubleAvltree(doubleAVLTree** root);
void _freeLDoubleAvltree(ldoubleAVLTree** root);
void _freeBoolAvltree(boolAVLTree** root);
void _freeStringAvltree(stringAVLTree** root);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
#define gbc_char_avltree __attribute__((cleanup(_freeCharAvltree)))
#define gbc_uchar_avltree __attribute__((cleanup(_freeUCharAvltree)))
#define gbc_short_avltree __attribute__((cleanup(_freeShortAvltree)))
#define gbc_ushort_avltree __attribute__((cleanup(_freeUShortAvltree)))
#define gbc_int_avltree __attribute__((cleanup(_freeIntAvltree)))
#define gbc_uint_avltree __attribute__((cleanup(_freeUIntAvltree)))
#define gbc_long_avltree __attribute__((cleanup(_freeLongAvltree)))
#define gbc_ulong_avltree __attribute__((cleanup(_freeULongAvltree)))
#define gbc_llong_avltree __attribute__((cleanup(_freeLLongAvltree)))
#define gbc_ullong_avltree __attribute__((cleanup(_freeULLongAvltree)))
#define gbc_float_avltree __attribute__((cleanup(_freeFloatAvltree)))
#define gbc_double_avltree __attribute__((cleanup(_freeDoubleAvltree)))
#define gbc_ldouble_avltree __attribute__((cleanup(_freeLDoubleAvltree)))
#define gbc_bool_avltree __attribute__((cleanup(_freeBoolAvltree)))
#define gbc_string_avltree __attribute__((cleanup(_freeStringAvltree)))
#endif
// ================================================================================
// ================================================================================

size_t char_avltree_size(charAVLTree* tree);
size_t uchar_avltree_size(ucharAVLTree* tree);
size_t short_avltree_size(shortAVLTree* tree);
size_t ushort_avltree_size(ushortAVLTree* tree);
size_t int_avltree_size(intAVLTree* tree);
size_t uint_avltree_size(uintAVLTree* tree);
size_t long_avltree_size(longAVLTree* tree);
size_t ulong_avltree_size(ulongAVLTree* tree);
size_t llong_avltree_size(llongAVLTree* tree);
size_t ullong_avltree_size(ullongAVLTree* tree);
size_t float_avltree_size(floatAVLTree* tree);
size_t double_avltree_size(doubleAVLTree* tree);
size_t ldouble_avltree_size(ldoubleAVLTree* tree);
size_t bool_avltree_size(boolAVLTree* tree);
size_t string_avltree_size(stringAVLTree* tree);
// --------------------------------------------------------------------------------

#define avltree_size(tree) _Generic((tree), \
    charAVLTree*: char_avltree_size, \
    ucharAVLTree*: uchar_avltree_size, \
    shortAVLTree*: short_avltree_size, \
    ushortAVLTree*: ushort_avltree_size, \
    intAVLTree*: int_avltree_size, \
    uintAVLTree*: uint_avltree_size, \
    longAVLTree*: long_avltree_size, \
    ulongAVLTree*: ulong_avltree_size, \
    llongAVLTree*: llong_avltree_size, \
    ullongAVLTree*: ullong_avltree_size, \
    floatAVLTree*: float_avltree_size, \
    doubleAVLTree*: double_avltree_size, \
    ldoubleAVLTree*: ldouble_avltree_size, \
    boolAVLTree*: bool_avltree_size, \
    stringAVLTree*: string_avltree_size)(tree)
// ================================================================================
// ================================================================================

char min_char_avltree(charAVLTree* tree);
unsigned char min_uchar_avltree(ucharAVLTree* tree);
short int min_short_avltree(shortAVLTree* tree);
unsigned short int min_ushort_avltree(ushortAVLTree* tree);
int min_int_avltree(intAVLTree* tree);
unsigned int min_uint_avltree(uintAVLTree* tree);
long int min_long_avltree(longAVLTree* tree);
unsigned long int min_ulong_avltree(ulongAVLTree* tree);
long long int min_llong_avltree(llongAVLTree* tree);
unsigned long long int min_ullong_avltree(ullongAVLTree* tree);
float min_float_avltree(floatAVLTree* tree);
double min_double_avltree(doubleAVLTree* tree);
long double min_ldouble_avltree(ldoubleAVLTree* tree);
bool min_bool_avltree(boolAVLTree* tree);
str* min_string_avltree(stringAVLTree* tree);
// --------------------------------------------------------------------------------

#define min_avltree(tree) _Generic((tree), \
    charAVLTree*: min_char_avltree, \
    ucharAVLTree*: min_uchar_avltree, \
    shortAVLTree*: min_short_avltree, \
    ushortAVLTree*: min_ushort_avltree, \
    intAVLTree*: min_int_avltree, \
    uintAVLTree*: min_uint_avltree, \
    longAVLTree*: min_long_avltree, \
    ulongAVLTree*: min_ulong_avltree, \
    llongAVLTree*: min_llong_avltree, \
    ullongAVLTree*: min_ullong_avltree, \
    floatAVLTree*: min_float_avltree, \
    doubleAVLTree*: min_double_avltree, \
    ldoubleAVLTree*: min_ldouble_avltree, \
    boolAVLTree*: min_bool_avltree, \
    stringAVLTree*: min_string_avltree)(tree)
// ================================================================================
// ================================================================================

char max_char_avltree(charAVLTree* tree);
unsigned char max_uchar_avltree(ucharAVLTree* tree);
short int max_short_avltree(shortAVLTree* tree);
unsigned short int max_ushort_avltree(ushortAVLTree* tree);
int max_int_avltree(intAVLTree* tree);
unsigned int max_uint_avltree(uintAVLTree* tree);
long int max_long_avltree(longAVLTree* tree);
unsigned long int max_ulong_avltree(ulongAVLTree* tree);
long long int max_llong_avltree(llongAVLTree* tree);
unsigned long long int max_ullong_avltree(ullongAVLTree* tree);
float max_float_avltree(floatAVLTree* tree);
double max_double_avltree(doubleAVLTree* tree);
long double max_ldouble_avltree(ldoubleAVLTree* tree);
bool max_bool_avltree(boolAVLTree* tree);
str* max_string_avltree(stringAVLTree* tree);
// --------------------------------------------------------------------------------

#define max_avltree(tree) _Generic((tree), \
    charAVLTree*: max_char_avltree, \
    ucharAVLTree*: max_uchar_avltree, \
    shortAVLTree*: max_short_avltree, \
    ushortAVLTree*: max_ushort_avltree, \
    intAVLTree*: max_int_avltree, \
    uintAVLTree*: max_uint_avltree, \
    longAVLTree*: max_long_avltree, \
    ulongAVLTree*: max_ulong_avltree, \
    llongAVLTree*: max_llong_avltree, \
    ullongAVLTree*: max_ullong_avltree, \
    floatAVLTree*: max_float_avltree, \
    doubleAVLTree*: max_double_avltree, \
    ldoubleAVLTree*: max_ldouble_avltree, \
    boolAVLTree*: max_bool_avltree, \
    stringAVLTree*: max_string_avltree)(tree)
// ================================================================================
// ================================================================================

char_v* avltree_char_values(charAVLTree* tree, char value, Boolean comp);
uchar_v* avltree_uchar_values(ucharAVLTree* tree, unsigned char value, Boolean comp);
short_v* avltree_short_values(shortAVLTree* tree, short int value, Boolean comp);
ushort_v* avltree_ushort_values(ushortAVLTree* tree, unsigned short int value, Boolean comp);
int_v* avltree_int_values(intAVLTree* tree, int value, Boolean comp);
uint_v* avltree_uint_values(uintAVLTree* tree, unsigned int value, Boolean comp);
long_v* avltree_long_values(longAVLTree* tree, long int value, Boolean comp);
ulong_v* avltree_ulong_values(ulongAVLTree* tree, unsigned long int value, Boolean comp);
llong_v* avltree_llong_values(llongAVLTree* tree, long long int value, Boolean comp);
ullong_v* avltree_ullong_values(ullongAVLTree* tree, unsigned long long int value, Boolean comp);
float_v* avltree_float_values(floatAVLTree* tree, float value, Boolean comp);
double_v* avltree_double_values(doubleAVLTree* tree, double value, Boolean comp);
ldouble_v* avltree_ldouble_values(ldoubleAVLTree* tree, long double value, Boolean comp);
bool_v* avltree_bool_values(boolAVLTree* tree, bool value, Boolean comp);
string_v* avltree_string_values(stringAVLTree* treee, char* value, Boolean comp);
// ================================================================================

#define avltree_values(tree, value, comp) _Generic((tree), \
    charAVLTree*: avltree_char_values, \
    ucharAVLTree*: avltree_uchar_values, \
    shortAVLTree*: avltree_short_values, \
    ushortAVLTree*: avltree_ushort_values, \
    intAVLTree*: avltree_int_values, \
    uintAVLTree*: avltree_uint_values, \
    longAVLTree*: avltree_long_values, \
    ulongAVLTree*: avltree_ulong_values, \
    llongAVLTree*: avltree_llong_values, \
    ullongAVLTree*: avltree_ullong_values, \
    floatAVLTree*: avltree_float_values, \
    doubleAVLTree*: avltree_double_values, \
    ldoubleAVLTree*: avltree_ldouble_values, \
    boolAVLTree*: avltree_bool_values, \
    stringAVLTree*: avltree_string_values)(tree, value, comp)
// ================================================================================
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* avl_H */
// ================================================================================
// ================================================================================
// eof
