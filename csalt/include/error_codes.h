// ================================================================================
// ================================================================================
// - File:    error_codes.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    September 24, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef error_codes_H
#define error_codes_H
// ================================================================================ 
// ================================================================================ 

#include <stdio.h>

// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

/**
 * @enum ErrorCode
 * @brief Standardized error codes used throughout the csalt library.
 *
 * Error codes are represented as signed integers. By convention:
 * - `0` indicates success (`NO_ERROR`).
 * - Negative values represent error conditions.
 *
 * Codes are grouped into categories by number range:
 * - Memory / allocation errors (`-2` to `-4`)
 * - Argument / input errors (`-10` to `-14`)
 * - Math / computation errors (`-20` to `-22`)
 * - File / I/O errors (`-30` to `-32`)
 * - Type / format errors (`-40` to `-41`)
 * - Generic / fallback (`-99`)
 *
 * This convention allows quick checks like:
 * @code
 * ErrorCode err = some_function();
 * if (err < 0) {
 *     // handle error
 * }
 * @endcode
 */
typedef enum {
    INVALID_ERROR       = 1,   /* Invalid error argument */
    NO_ERROR            = 0,   /* No error */

    /* Memory / allocation errors */
    BAD_ALLOC           = -2,  /* Memory allocation failed */
    REALLOC_FAIL        = -3,  /* Reallocation failed */
    OUT_OF_MEMORY       = -4,  /* System out of memory */

    /* Argument / input errors */
    NULL_POINTER        = -10, /* Null pointer passed */
    OUT_OF_BOUNDS       = -11, /* Index out of range */
    SIZE_MISMATCH       = -12, /* Dimension mismatch between objects */
    INVALID_ARG         = -13, /* Invalid function argument */
    UNINITIALIZED       = -14, /* Attempt to access uninitialized element */

    /* Math / computation errors */
    DIV_BY_ZERO         = -20, /* Division by zero */
    SINGULAR_MATRIX     = -21, /* Matrix is singular / non-invertible */
    NUMERIC_OVERFLOW    = -22, /* Overflow/underflow occurred */

    /* File / I/O errors */
    FILE_OPEN           = -30, /* Could not open file */
    FILE_READ           = -31, /* Error reading file */
    FILE_WRITE          = -32, /* Error writing file */

    /* Type / format errors */
    TYPE_MISMATCH       = -40, /* Incompatible type */
    FORMAT_INVALID      = -41, /* Invalid format or encoding */

    /* Generic / fallback */
    UNKNOWN             = -99  /* Unknown error */
} ErrorCode;
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert an ErrorCode into a human-readable string.
 *
 * This function provides a textual description of an error code
 * defined in the ErrorCode enum. It is primarily intended for
 * diagnostic output and logging.
 *
 * @param code The ErrorCode to be described.
 * @return A constant string describing the error.
 *         If the code is not recognized, returns
 *         "Unrecognized error code".
 *
 * Example:
 * @code
 * ErrorCode err = BAD_ALLOC;
 * printf("Operation failed: %s\n", error_to_string(err));
 * @endcode
 */
const char* error_to_string(ErrorCode code);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* error_codes_H */
// ================================================================================
// ================================================================================
// eof
