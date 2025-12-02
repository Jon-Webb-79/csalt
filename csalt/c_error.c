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

#include "c_error.h"
// ================================================================================ 
// ================================================================================ 
// DEFIINE CONST CHAR RETURNS FOR TYPES 

const char* error_to_string(ErrorCode code)
{
    switch (code) {
        case NO_ERROR:               return "No error";
        case INVALID_ERROR:          return "Invalid error entry";

        /* -1xx: Argument / Input */
        case INVALID_ARG:            return "Invalid function argument";
        case NULL_POINTER:           return "Null pointer passed";
        case OUT_OF_BOUNDS:          return "Index out of range";
        case SIZE_MISMATCH:          return "Dimension/size mismatch";
        case UNINITIALIZED:          return "Uninitialized element access";
        case ITERATOR_INVALID:       return "Invalid iterator/cursor";
        case PRECONDITION_FAIL:      return "Precondition failed";
        case POSTCONDITION_FAIL:     return "Postcondition failed";
        case ILLEGAL_STATE:          return "Illegal state for operation";

        /* -2xx: Memory / Allocation */
        case BAD_ALLOC:              return "Memory allocation failed";
        case REALLOC_FAIL:           return "Memory reallocation failed";
        case OUT_OF_MEMORY:          return "Out of memory";
        case LENGTH_OVERFLOW:        return "Length/size arithmetic overflow";
        case CAPACITY_OVERFLOW:      return "Capacity limit exceeded";
        case ALIGNMENT_ERROR:        return "Required alignment not satisfied";

        /* -3xx: State / Container */
        case STATE_CORRUPT:          return "Internal state corrupt";
        case ALREADY_INITIALIZED:    return "Already initialized";
        case NOT_FOUND:              return "Item not found";
        case EMPTY:                  return "Container is empty";
        case CONCURRENT_MODIFICATION:return "Concurrent modification detected";

        /* -4xx: Math / Domain */
        case DIV_BY_ZERO:            return "Division by zero";
        case SINGULAR_MATRIX:        return "Singular/non-invertible matrix";
        case NUMERIC_OVERFLOW:       return "Numeric overflow/underflow";
        case DOMAIN_ERROR:           return "Math domain error";
        case LOSS_OF_PRECISION:      return "Loss of numeric precision";

        /* -5xx: File / I/O */
        case FILE_OPEN:              return "Failed to open file/handle";
        case FILE_READ:              return "Error reading from file/handle";
        case FILE_WRITE:             return "Error writing to file/handle";
        case PERMISSION_DENIED:      return "Permission denied";
        case IO_INTERRUPTED:         return "I/O interrupted";
        case IO_TIMEOUT:             return "I/O timed out";
        case IO_CLOSED:              return "Operation on closed stream/descriptor";
        case IO_WOULD_BLOCK:         return "Operation would block";

        /* -6xx: Type / Format / Encoding */
        case TYPE_MISMATCH:          return "Type mismatch";
        case FORMAT_INVALID:         return "Invalid data format";
        case ENCODING_INVALID:       return "Invalid text encoding";
        case PARSING_FAILED:         return "Parsing failed";
        case VALIDATION_FAILED:      return "Validation failed";

        /* -7xx: Concurrency / Synchronization */
        case LOCK_FAILED:            return "Lock operation failed";
        case DEADLOCK_DETECTED:      return "Deadlock detected";
        case THREAD_FAIL:            return "Thread operation failed";
        case CANCELLED:              return "Operation cancelled";
        case RACE_DETECTED:          return "Data race detected";

        /* -8xx: Configuration / Policy / Environment */
        case CONFIG_INVALID:         return "Invalid configuration";
        case UNSUPPORTED:            return "Unsupported feature/platform";
        case FEATURE_DISABLED:       return "Feature disabled by policy/build";
        case VERSION_MISMATCH:       return "Version/ABI mismatch";
        case RESOURCE_EXHAUSTED:     return "Resource exhausted";

        /* -9xx: Generic / Fallback */
        case NOT_IMPLEMENTED:        return "Not implemented";
        case OPERATION_UNAVAILABLE:  return "Operation unavailable";
        case UNKNOWN:                return "Unknown error";
    }
    return "Unrecognized error code"; /* Fallback for unmapped values */
}
// ================================================================================ 
// ================================================================================ 

const char* error_cat_to_string(ErrorCategory cat) {
    switch(cat) {
        case ECAT_NONE:     return "NO ERROR";
        case ECAT_ARG:      return "ARGUMENT ERROR";
        case ECAT_MEM:      return "MEMORY ERROR";
        case ECAT_STATE:    return "STATE ERROR";
        case ECAT_MATH:     return "MATH ERROR";
        case ECAT_IO:       return "I/O ERROR";
        case ECAT_FMT:      return "TYPE/FORMAT ERROR";
        case ECAT_CONC:     return "CONCURRENCY ERROR";
        case ECAT_CFG:      return "CONFIG ERROR";
        case ECAT_GEN:      return "GENERIC ERROR";
    }
    return "UNKNOWN ERROR";
}
// ================================================================================ 
// ================================================================================ 
// eof
