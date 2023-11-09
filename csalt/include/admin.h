// ================================================================================
// ================================================================================
// - File:    admin.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    November 06, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef admin_H
#define admin_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================

/**
 * @brief An enum containing keywords for an iterator 
 *
 * This enum contains keywords that are used to describe the order of iteration
 * within an iterator 
 *
 * @attribute FORWARD Keyword to command a forward iteration 
 * @attribute REVERSE Keyword to command a reverese iteration
 */
typedef enum {
    FORWARD,
    REVERSE
} iter_dir;
// ================================================================================
// ================================================================================

/**
 * @brief An enum containing keywords for function selections 
 *
 * This enum contains keywords that represent the data types of possible functions 
 * that can be used to initialize data structures.
 *
 * @attribute dChar An alias for a char data type used to represent an integer 
 * @attribute dUChar An alias for an unsigned char data type used to represent an integer 
 * @attribute dShort An alias for a short int data type 
 * @attribute dUShort An alias for an unsigned short data type 
 * @attribute dInt An alias for an int data type 
 * @attribute dUInt An alias for an unsigned int data type 
 * @attribute dLong An alias for a long int data type 
 * @attribute dULong An alias for an unsigned long data type 
 * @attribute dLongLong An alias for a long long int data type 
 * @attribute dULongLong An alias for an unsigned long long int data type 
 * @attribute dFloat An alias for a float data type 
 * @attribute dDouble An alias for a double data type 
 * @attribute dLDouble An alias for a long double data type 
 * @attribute dBool An alias for a bool data type 
 * @attribute dString An alias for a str data type
 */
typedef enum {
    dChar,  // Keyword for a char data type as an integer data type 
    dUChar,  // keyword for unsigned char data type as an integer data type 
    dShort,  // Keyword for short int data type 
    dUShort,  // Keyword for unsigned short int data type 
    dInt,  // Keyword for int data type 
    dUInt,  // Keyword for unsigned int data type 
    dLong,  // Keyword for long int data type 
    dULong,  // Keyword for unsigned long int data type 
    dLongLong,  // Keyword for long long int data type 
    dULongLong,  // Keyword for unsigned long long int data type 
    dFloat,  // Keyword for float data type 
    dDouble,  // Keyword for double data type 
    dLDouble,  // Keyword for long double data type 
    dBool,  // Keyword for bool data type 
    dString  // Keyword for str data type
} dtype;
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* admin_H */
// ================================================================================
// ================================================================================
// eof
