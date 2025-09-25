// ================================================================================
// ================================================================================
// - File:    error_codes.c
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

#include "error_codes.h"
// ================================================================================ 
// ================================================================================ 

const char* error_to_string(ErrorCode code) {
    switch (code) {
        case NO_ERROR:           return "No error";
        case INVALID_ERROR:      return "Invalid error entry";

        /* Memory / allocation errors */
        case BAD_ALLOC:          return "Memory allocation failed";
        case REALLOC_FAIL:       return "memory reallocation failed";
        case OUT_OF_MEMORY:      return "System out of memory";

        /* Argument / input errors */
        case NULL_POINTER:       return "Null pointer passed";
        case OUT_OF_BOUNDS:      return "Index out of range";
        case SIZE_MISMATCH:      return "Dimension mismatch between objects";
        case INVALID_ARG:        return "Invalid function argument";
        case UNINITIALIZED:      return "Attempt to access uninitialized element";

        /* Math / computation errors */
        case DIV_BY_ZERO:        return "Division by zero";
        case SINGULAR_MATRIX:    return "Matrix is singular / non-invertible";
        case NUMERIC_OVERFLOW:   return "Numeric overflow or underflow occurred";

        /* File / I/O errors */
        case FILE_OPEN:          return "Could not open file";
        case FILE_READ:          return "Error reading file";
        case FILE_WRITE:         return "Error writing file";

        /* Type / format errors */
        case TYPE_MISMATCH:      return "Incompatible type";
        case FORMAT_INVALID:     return "Invalid format or encoding";

        /* Generic / fallback */
        case UNKNOWN:            return "Unknown error";
    }
    return "Unrecognized error code";  /* Fallback for unmapped values */
}
// ================================================================================
// ================================================================================
// eof
