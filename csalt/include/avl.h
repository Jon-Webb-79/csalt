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

#include "str.h"

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
struct { \
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


#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* avl_H */
// ================================================================================
// ================================================================================
// eof
