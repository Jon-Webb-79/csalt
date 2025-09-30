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

#include <errno.h>
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

int set_errno_from_error(ErrorCode code)
{
    int e;

    switch (code) {
        /* success/sentinel */
        case NO_ERROR:                e = 0; break;
        case INVALID_ERROR:           e = EINVAL; break;

        /* -1xx: argument/input */
        case INVALID_ARG:
        case NULL_POINTER:
        case OUT_OF_BOUNDS:
        case SIZE_MISMATCH:
        case ITERATOR_INVALID:
        case PRECONDITION_FAIL:
        case POSTCONDITION_FAIL:
        case ILLEGAL_STATE:           e = EINVAL; break;
        case UNINITIALIZED:
        #ifdef ENODATA
                                      e = ENODATA; break;
        #else
                                      e = EINVAL;  break;
        #endif

        /* -2xx: memory/allocation */
        case BAD_ALLOC:
        case REALLOC_FAIL:
        case OUT_OF_MEMORY:           e = ENOMEM; break;
        case LENGTH_OVERFLOW:
        case CAPACITY_OVERFLOW:       e = EOVERFLOW; break;
        case ALIGNMENT_ERROR:         e = EINVAL; break;

        /* -3xx: state/container */
        case STATE_CORRUPT:           e = EFAULT; break;
        case ALREADY_INITIALIZED:
        #ifdef EALREADY
                                      e = EALREADY; break;
        #else
                                      e = EINVAL;   break;
        #endif
        case NOT_FOUND:               e = ENOENT; break;
        case EMPTY:
        #ifdef ENODATA
                                      e = ENODATA; break;
        #else
                                      e = EINVAL;  break;
        #endif
        case CONCURRENT_MODIFICATION: e = EBUSY; break;

        /* -4xx: math/domain */
        case DIV_BY_ZERO:
        case SINGULAR_MATRIX:
        case DOMAIN_ERROR:            e = EDOM; break;
        case NUMERIC_OVERFLOW:
        case LOSS_OF_PRECISION:       e = ERANGE; break;

        /* -5xx: file / I/O */
        case FILE_OPEN:
        case FILE_READ:
        case FILE_WRITE:              e = EIO; break;
        case PERMISSION_DENIED:       e = EACCES; break;
        case IO_INTERRUPTED:          e = EINTR; break;
        case IO_TIMEOUT:              e = ETIMEDOUT; break;
        case IO_CLOSED:               e = EBADF; break;
        case IO_WOULD_BLOCK:
        #ifdef EWOULDBLOCK
                                      e = EWOULDBLOCK; break;
        #else
                                      e = EAGAIN;      break;
        #endif

        /* -6xx: type/format/encoding */
        case TYPE_MISMATCH:
        case FORMAT_INVALID:
        case PARSING_FAILED:
        case VALIDATION_FAILED:       e = EINVAL; break;
        case ENCODING_INVALID:
        #ifdef EILSEQ
                                      e = EILSEQ; break;
        #else
                                      e = EINVAL; break;
        #endif

        /* -7xx: concurrency/synchronization */
        case LOCK_FAILED:
        case THREAD_FAIL:             e = EAGAIN; break;     /* resource/temp fail */
        case DEADLOCK_DETECTED:       e = EDEADLK; break;
        case CANCELLED:
        #ifdef ECANCELED
                                      e = ECANCELED; break;
        #else
                                      e = EINVAL;    break;
        #endif
        case RACE_DETECTED:           e = EFAULT; break;

        /* -8xx: configuration/policy/environment */
        case CONFIG_INVALID:
        case VERSION_MISMATCH:        e = EINVAL; break;
        case UNSUPPORTED:
        #ifdef ENOTSUP
                                      e = ENOTSUP; break;
        #else
                                      e = EOPNOTSUPP; break;
        #endif
        case FEATURE_DISABLED:        e = EPERM; break;
        case RESOURCE_EXHAUSTED:      e = ENOSPC; break;

        /* -9xx: generic/fallback */
        case NOT_IMPLEMENTED:         e = ENOSYS; break;
        case OPERATION_UNAVAILABLE:   e = EAGAIN; break;
        case UNKNOWN:
        default:                      e = EINVAL; break;
    }

    errno = e;
    return e;
}
// -------------------------------------------------------------------------------- 

ErrorCode error_from_errno(int e) {
    switch (e) {
        case 0:              return NO_ERROR;

        case ENOMEM:         return OUT_OF_MEMORY;
        case EOVERFLOW:      return LENGTH_OVERFLOW;
        case ERANGE:         return NUMERIC_OVERFLOW;
        case EDOM:           return DOMAIN_ERROR;

        case ENOENT:         return NOT_FOUND;
        case EACCES:         return PERMISSION_DENIED;
        case EINTR:          return IO_INTERRUPTED;
        case ETIMEDOUT:      return IO_TIMEOUT;
        case EBADF:          return IO_CLOSED;

        /* --- would-block vs again (may be the same value) --- */
        #if defined(EWOULDBLOCK) && defined(EAGAIN)
          #if EWOULDBLOCK != EAGAIN
            case EWOULDBLOCK:  return IO_WOULD_BLOCK;
            case EAGAIN:       return LOCK_FAILED;      /* resource/temp fail */
          #else
            /* Alias: choose a canonical meaning (IO would-block is most common) */
            case EAGAIN:       return IO_WOULD_BLOCK;
          #endif
        #elif defined(EWOULDBLOCK)
            case EWOULDBLOCK:  return IO_WOULD_BLOCK;
        #elif defined(EAGAIN)
            case EAGAIN:       return LOCK_FAILED;
        #endif

        #ifdef ENODATA
        case ENODATA:        return UNINITIALIZED;
        #endif

        #ifdef EILSEQ
        case EILSEQ:         return ENCODING_INVALID;
        #endif

        case EIO:            return FILE_READ;              /* generic I/O error */
        case EFAULT:         return STATE_CORRUPT;

        #ifdef EALREADY
        case EALREADY:       return ALREADY_INITIALIZED;
        #endif

        case ENOSYS:         return NOT_IMPLEMENTED;

        /* --- deadlock aliases --- */
        #if defined(EDEADLK) && defined(EDEADLOCK)
          #if EDEADLK != EDEADLOCK
            case EDEADLK:     return DEADLOCK_DETECTED;
            case EDEADLOCK:   return DEADLOCK_DETECTED;
          #else
            case EDEADLK:     return DEADLOCK_DETECTED;
          #endif
        #elif defined(EDEADLK)
            case EDEADLK:     return DEADLOCK_DETECTED;
        #elif defined(EDEADLOCK)
            case EDEADLOCK:   return DEADLOCK_DETECTED;
        #endif

        /* --- not supported aliases --- */
        #if defined(ENOTSUP) && defined(EOPNOTSUPP)
          #if ENOTSUP != EOPNOTSUPP
            case ENOTSUP:     return UNSUPPORTED;
            case EOPNOTSUPP:  return UNSUPPORTED;
          #else
            case ENOTSUP:     return UNSUPPORTED;
          #endif
        #elif defined(ENOTSUP)
            case ENOTSUP:     return UNSUPPORTED;
        #elif defined(EOPNOTSUPP)
            case EOPNOTSUPP:  return UNSUPPORTED;
        #endif

        #ifdef ECANCELED
        case ECANCELED:      return CANCELLED;
        #endif

        case EPERM:          return FEATURE_DISABLED;
        case ENOSPC:         return RESOURCE_EXHAUSTED;
        case EBUSY:          return CONCURRENT_MODIFICATION;

        case EINVAL:         return INVALID_ARG;

        default:             return UNKNOWN;
    }
}
// ================================================================================
// ================================================================================
// eof
