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
    dStr  // Keyword for str data type
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
