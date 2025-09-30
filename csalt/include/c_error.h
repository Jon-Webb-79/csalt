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
#include <stdbool.h>
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

/**
 * @file error_code.h
 * @brief Range-based error taxonomy for the csalt library.
 *
 * All “real” errors are negative and grouped into decimal ranges by category:
 *
 *   -1xx  Argument / Input errors (API misuse, bounds, preconditions)
 *   -2xx  Memory / Allocation errors (malloc/realloc/oom, size overflows)
 *   -3xx  State / Container errors (corruption, not found, empty, init state)
 *   -4xx  Math / Domain errors (division by zero, singular, numeric range)
 *   -5xx  I/O errors (open/read/write, permissions, timeouts)
 *   -6xx  Type / Format / Encoding errors (parse/validate/type mismatch)
 *   -7xx  Concurrency / Synchronization errors (locks, deadlock, cancellation)
 *   -8xx  Configuration / Policy / Environment (unsupported, versioning)
 *   -9xx  Generic / Fallback (not implemented, unknown)
 *
 * Success is `0` (::NO_ERROR). A single positive sentinel (::INVALID_ERROR = 1)
 * is reserved for getter-style APIs that cannot update an internal ::ErrorCode
 * field (e.g., NULL handle passed to a const getter). Do not store or propagate
 * ::INVALID_ERROR as a normal error; prefer category codes instead.
 *
 * Example use:
 * @code
 * if (vec == NULL) return NULL_POINTER;          // -102  (ARG)
 * if (idx >= vec->len) return OUT_OF_BOUNDS;     // -103  (ARG)
 * p = realloc(...); if (!p) return OUT_OF_MEMORY;// -203  (MEM)
 * @endcode
 *
 * Category helpers:
 * @code
 * if (EC_IS_MEM(err))   log_warn("memory issue: %d", err);
 * if (EC_CATEGORY(err) == 5) retry_io();  // any I/O (-5xx)
 * @endcode
 */
typedef enum ErrorCode {
    /* Success / sentinel */
    NO_ERROR            = 0,    /**< Success / no error */
    INVALID_ERROR       = 1,    /**< Non-taxonomy sentinel for invalid getter input */

    /* -1xx: Argument / Input */
    INVALID_ARG         = -101, /**< Invalid function argument */
    NULL_POINTER        = -102, /**< NULL pointer passed */
    OUT_OF_BOUNDS       = -103, /**< Index out of range */
    SIZE_MISMATCH       = -104, /**< Dimension/shape mismatch between objects */
    UNINITIALIZED       = -105, /**< Attempt to access uninitialized element */
    ITERATOR_INVALID    = -106, /**< Dangling/invalid iterator or cursor */
    PRECONDITION_FAIL   = -107, /**< Required precondition not satisfied */
    POSTCONDITION_FAIL  = -108, /**< Postcondition/invariant failed */
    ILLEGAL_STATE       = -109, /**< API call not valid for current state */

    /* -2xx: Memory / Allocation */
    BAD_ALLOC           = -201, /**< malloc/calloc failed */
    REALLOC_FAIL        = -202, /**< realloc failed (buffer unchanged) */
    OUT_OF_MEMORY       = -203, /**< System out of memory / allocator limit */
    LENGTH_OVERFLOW     = -204, /**< size/length arithmetic overflow */
    CAPACITY_OVERFLOW   = -205, /**< capacity policy or representable limit exceeded */
    ALIGNMENT_ERROR     = -206, /**< required alignment not satisfied */

    /* -3xx: State / Container */
    STATE_CORRUPT       = -301, /**< Internal invariant violated / corruption detected */
    ALREADY_INITIALIZED = -302, /**< Double-init or conflicting initialization */
    NOT_FOUND           = -303, /**< Requested item/key not found */
    EMPTY               = -304, /**< Container empty when element required */
    CONCURRENT_MODIFICATION = -305, /**< Modified during iteration (fail-fast) */

    /* -4xx: Math / Domain */
    DIV_BY_ZERO         = -401, /**< Division by zero */
    SINGULAR_MATRIX     = -402, /**< Non-invertible matrix / singular system */
    NUMERIC_OVERFLOW    = -403, /**< Numeric overflow/underflow (range error) */
    DOMAIN_ERROR        = -404, /**< Math domain error (invalid input domain) */
    LOSS_OF_PRECISION   = -405, /**< Numerically unstable / excessive loss */

    /* -5xx: File / I/O */
    FILE_OPEN           = -501, /**< Could not open file/handle */
    FILE_READ           = -502, /**< Error reading from file/handle */
    FILE_WRITE          = -503, /**< Error writing to file/handle */
    PERMISSION_DENIED   = -504, /**< Permission/ACL denied */
    IO_INTERRUPTED      = -505, /**< Interrupted (e.g., EINTR) */
    IO_TIMEOUT          = -506, /**< I/O timed out */
    IO_CLOSED           = -507, /**< Operation on closed descriptor/stream */
    IO_WOULD_BLOCK      = -508, /**< Non-blocking op would block (EWOULDBLOCK) */

    /* -6xx: Type / Format / Encoding */
    TYPE_MISMATCH       = -601, /**< Incompatible type */
    FORMAT_INVALID      = -602, /**< Invalid data format */
    ENCODING_INVALID    = -603, /**< Invalid text encoding (e.g., UTF-8) */
    PARSING_FAILED      = -604, /**< Parsing error */
    VALIDATION_FAILED   = -605, /**< Data failed validation rules */

    /* -7xx: Concurrency / Synchronization */
    LOCK_FAILED         = -701, /**< Failed to acquire/initialize lock */
    DEADLOCK_DETECTED   = -702, /**< Potential or actual deadlock */
    THREAD_FAIL         = -703, /**< Thread/create/join failure */
    CANCELLED           = -704, /**< Operation cancelled */
    RACE_DETECTED       = -705, /**< Data race / unsynchronized access */

    /* -8xx: Configuration / Policy / Environment */
    CONFIG_INVALID      = -801, /**< Invalid configuration value/state */
    UNSUPPORTED         = -802, /**< Unsupported feature or platform */
    FEATURE_DISABLED    = -803, /**< Feature disabled by policy/build flags */
    VERSION_MISMATCH    = -804, /**< ABI/format/library version mismatch */
    RESOURCE_EXHAUSTED  = -805, /**< Non-memory resource exhausted (fds, etc.) */

    /* -9xx: Generic / Fallback */
    NOT_IMPLEMENTED     = -981, /**< Functionality not yet implemented */
    OPERATION_UNAVAILABLE = -982,/**< Operation unavailable in current build/runtime */
    UNKNOWN             = -999  /**< Unknown/unspecified error */
} ErrorCode;
// -------------------------------------------------------------------------------- 

typedef enum {
    ECAT_NONE  = 0,  /* success/sentinel */
    ECAT_ARG   = 1,
    ECAT_MEM   = 2,
    ECAT_STATE = 3,
    ECAT_MATH  = 4,
    ECAT_IO    = 5,
    ECAT_FMT   = 6,
    ECAT_CONC  = 7,
    ECAT_CFG   = 8,
    ECAT_GEN   = 9
} ErrorCategory;

/**
 * @brief Return the category (decimal range) for an ErrorCode.
 *
 * 0 = success/sentinel (>=0), 1 = ARG(-1xx), 2 = MEM(-2xx), ..., 9 = GEN(-9xx).
 */
static inline ErrorCategory ec_category(ErrorCode ec) {
    return (ec >= 0) ? ECAT_NONE : (ErrorCategory)((-(int)ec) / 100);
}

/** @brief True if @p ec is an argument/input error (-1xx). */
static inline bool ec_is_arg(ErrorCode ec)   { return ec_category(ec) == ECAT_ARG; }
/** @brief True if @p ec is a memory/allocation error (-2xx). */
static inline bool ec_is_mem(ErrorCode ec)   { return ec_category(ec) == ECAT_MEM; }
/** @brief True if @p ec is a state/container error (-3xx). */
static inline bool ec_is_state(ErrorCode ec) { return ec_category(ec) == ECAT_STATE; }
/** @brief True if @p ec is a math/domain error (-4xx). */
static inline bool ec_is_math(ErrorCode ec)  { return ec_category(ec) == ECAT_MATH; }
/** @brief True if @p ec is an I/O error (-5xx). */
static inline bool ec_is_io(ErrorCode ec)    { return ec_category(ec) == ECAT_IO; }
/** @brief True if @p ec is a type/format error (-6xx). */
static inline bool ec_is_fmt(ErrorCode ec)   { return ec_category(ec) == ECAT_FMT; }
/** @brief True if @p ec is a concurrency error (-7xx). */
static inline bool ec_is_conc(ErrorCode ec)  { return ec_category(ec) == ECAT_CONC; }
/** @brief True if @p ec is a config/policy error (-8xx). */
static inline bool ec_is_cfg(ErrorCode ec)   { return ec_category(ec) == ECAT_CFG; }
/** @brief True if @p ec is a generic/fallback error (-9xx). */
static inline bool ec_is_gen(ErrorCode ec)   { return ec_category(ec) == ECAT_GEN; }
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert an ::ErrorCode to a short human-readable message.
 *
 * Returns a stable, static string literal describing @p code. Messages are
 * aligned with the range-based taxonomy (−1xx argument, −2xx memory, …) and
 * are suitable for logs, error prompts, or test assertions.
 *
 * Error reporting:
 * - Pure function: does not modify global state (e.g., `errno`).
 *
 * @param code  The ::ErrorCode to describe.
 *
 * @return Pointer to a constant, NUL-terminated string. The pointer refers to
 *         a static string literal that must not be freed or modified.
 *
 * @retval "Unrecognized error code"
 *   Returned when @p code is outside the known enumerators (fallback path).
 *
 * @note The mapping is intentionally concise and stable for logging. For a
 *       broader label (category), see ::error_cat_to_string or
 *       ::error_category_str. To translate between ::ErrorCode and `errno`,
 *       use ::set_errno_from_error and ::error_from_errno.
 *
 * @par Example
 * @code{.c}
 * ErrorCode ec = BAD_ALLOC;
 * fprintf(stderr, "[%s/%d] %s\n",
 *         error_category_tag(ec), (int)ec, error_to_string(ec));
 * // -> "[MEM/-201] Memory allocation failed"
 * @endcode
 */
const char* error_to_string(ErrorCode code);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert an ::ErrorCategory to an uppercase label for logging.
 *
 * Maps the range-based category (e.g., ::ECAT_MEM) to a stable, uppercase
 * description such as "MEMORY ERROR". Useful for log prefixes and summaries.
 * Returns a pointer to a static string literal; do not free or modify it.
 *
 * @param cat  The ::ErrorCategory value (e.g., ::ECAT_ARG, ::ECAT_IO).
 *
 * @return A constant NUL-terminated string: one of
 *         "NO ERROR", "ARGUMENT ERROR", "MEMORY ERROR", "STATE ERROR",
 *         "MATH ERROR", "I/O ERROR", "TYPE/FORMAT ERROR",
 *         "CONCURRENCY ERROR", "CONFIG ERROR", "GENERIC ERROR",
 *         or "UNKNOWN ERROR" for out-of-range values.
 *
 * @note Pure function: does not modify global state (e.g., `errno`) and is
 *       thread-safe. If you prefer shorter tags (e.g., "MEM", "IO"), provide a
 *       companion function such as `error_category_tag()`.
 *
 * @par Example
 * @code{.c}
 * ErrorCategory cat = ECAT_MEM;
 * fprintf(stderr, "[%s] allocation failed\n", error_cat_to_string(cat));
 * // -> "[MEMORY ERROR] allocation failed"
 * @endcode
 */
const char* error_cat_to_string(ErrorCategory cat);
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Set POSIX `errno` from a range-based ::ErrorCode and return the value set.
 *
 * Translates the library’s ::ErrorCode taxonomy (−1xx argument, −2xx memory, …)
 * into a best-fit POSIX `errno`, assigns it to the calling thread’s `errno`, and
 * returns the same integer. This centralizes error→`errno` policy so call sites
 * behave consistently.
 *
 * Mapping summary (high level):
 * - Success/sentinel: ::NO_ERROR → 0; ::INVALID_ERROR → `EINVAL`.
 * - −1xx (argument/input): `EINVAL` (except ::UNINITIALIZED → `ENODATA` if available).
 * - −2xx (memory): allocation failures → `ENOMEM`; size/capacity overflow → `EOVERFLOW`;
 *   ::ALIGNMENT_ERROR → `EINVAL`.
 * - −3xx (state/container): ::NOT_FOUND → `ENOENT`; ::EMPTY → `ENODATA` if available;
 *   ::ALREADY_INITIALIZED → `EALREADY` if available; ::STATE_CORRUPT → `EFAULT`;
 *   ::CONCURRENT_MODIFICATION → `EBUSY`.
 * - −4xx (math/domain): domain errors (divide by zero, singular) → `EDOM`;
 *   range/precision issues → `ERANGE`.
 * - −5xx (I/O): ::FILE_* → `EIO`; ::PERMISSION_DENIED → `EACCES`; ::IO_INTERRUPTED → `EINTR`;
 *   ::IO_TIMEOUT → `ETIMEDOUT`; ::IO_CLOSED → `EBADF`; ::IO_WOULD_BLOCK → `EWOULDBLOCK`
 *   if defined, otherwise `EAGAIN`.
 * - −6xx (type/format): general → `EINVAL`; ::ENCODING_INVALID → `EILSEQ` if available.
 * - −7xx (concurrency): ::LOCK_FAILED / ::THREAD_FAIL → `EAGAIN`;
 *   ::DEADLOCK_DETECTED → `EDEADLK` if available; ::CANCELLED → `ECANCELED` if available;
 *   ::RACE_DETECTED → `EFAULT`.
 * - −8xx (config/policy/env): ::UNSUPPORTED → `ENOTSUP` (or `EOPNOTSUPP`);
 *   ::FEATURE_DISABLED → `EPERM`; ::RESOURCE_EXHAUSTED → `ENOSPC`;
 *   ::CONFIG_INVALID / ::VERSION_MISMATCH → `EINVAL`.
 * - −9xx (generic): ::NOT_IMPLEMENTED → `ENOSYS`;
 *   ::OPERATION_UNAVAILABLE → `EAGAIN`; ::UNKNOWN (and any unmapped) → `EINVAL`.
 *
 * Platform conditionals:
 * - If `ENODATA`, `EILSEQ`, `EALREADY`, or `ECANCELED` are absent, the mapping
 *   falls back to reasonable defaults (typically `EINVAL`).
 * - If `EWOULDBLOCK` is unavailable, ::IO_WOULD_BLOCK maps to `EAGAIN`.
 *
 * Error reporting:
 * - Always sets and returns the chosen `errno` value (including 0 for success).
 * - This function itself does not fail.
 *
 * @param code  The ::ErrorCode to translate.
 *
 * @return The POSIX `errno` value assigned to `errno` for @p code.
 *
 * @retval 0            If @p code is ::NO_ERROR.
 * @retval EINVAL       For invalid arguments/misuse and most unmapped codes.
 * @retval ENOMEM       For allocation failures (::BAD_ALLOC, ::REALLOC_FAIL, ::OUT_OF_MEMORY).
 * @retval EOVERFLOW    For size/capacity arithmetic overflow (::LENGTH_OVERFLOW, ::CAPACITY_OVERFLOW).
 * @retval ENOENT       For ::NOT_FOUND.
 * @retval ENODATA      For ::UNINITIALIZED and ::EMPTY (if available).
 * @retval EDOM         For ::DIV_BY_ZERO, ::SINGULAR_MATRIX, ::DOMAIN_ERROR.
 * @retval ERANGE       For ::NUMERIC_OVERFLOW and ::LOSS_OF_PRECISION.
 * @retval EIO          For ::FILE_OPEN / ::FILE_READ / ::FILE_WRITE.
 * @retval EACCES       For ::PERMISSION_DENIED.
 * @retval EINTR        For ::IO_INTERRUPTED.
 * @retval ETIMEDOUT    For ::IO_TIMEOUT.
 * @retval EBADF        For ::IO_CLOSED.
 * @retval EWOULDBLOCK  For ::IO_WOULD_BLOCK (else `EAGAIN`).
 * @retval EILSEQ       For ::ENCODING_INVALID (if available).
 * @retval EFAULT       For ::STATE_CORRUPT and ::RACE_DETECTED.
 * @retval EALREADY     For ::ALREADY_INITIALIZED (if available).
 * @retval EDEADLK      For ::DEADLOCK_DETECTED (if available).
 * @retval ECANCELED    For ::CANCELLED (if available).
 * @retval ENOTSUP      For ::UNSUPPORTED (or `EOPNOTSUPP`).
 * @retval EPERM        For ::FEATURE_DISABLED.
 * @retval ENOSPC       For ::RESOURCE_EXHAUSTED.
 * @retval ENOSYS       For ::NOT_IMPLEMENTED.
 * @retval EAGAIN       For ::LOCK_FAILED, ::THREAD_FAIL, ::OPERATION_UNAVAILABLE (and
 *                      as a fallback for would-block on platforms without `EWOULDBLOCK`).
 *
 * @thread_safety This function writes the thread-local `errno`; it is safe to call
 *                concurrently from multiple threads.
 *
 * @warning Call this **before** any operation that might overwrite `errno`
 *          (e.g., immediately after detecting/setting an ::ErrorCode).
 *
 * @note The mapping is not one-to-one; the inverse (::error_from_errno) is lossy.
 *       Keep both helpers consistent if you change policy.
 *
 * @see error_from_errno, error_to_string, error_cat_to_string, ec_category
 *
 * @par Example
 * @code{.c}
 * if (!push_back_str_vector(vec, s)) {
 *     // vec->error set by callee; set errno consistently for perror()/logs
 *     set_errno_from_error(vec->error);
 *     perror("push_back_str_vector");
 *     return false;
 * }
 * @endcode
 */

int set_errno_from_error(ErrorCode code);
// -------------------------------------------------------------------------------- 

/**
 * @brief Translate a POSIX `errno` value to a canonical ::ErrorCode.
 *
 * Provides a best-effort **reverse mapping** from @p e to the library’s
 * range-based ::ErrorCode taxonomy (−1xx argument, −2xx memory, …).
 * Because multiple ::ErrorCode values often share the same `errno`, this
 * function returns a single **canonical** code for each `errno`
 * (e.g., `ENOMEM` → ::OUT_OF_MEMORY).
 *
 * Semantics:
 * - **Pure function**: does **not** modify `errno` or any global state.
 * - **Lossy mapping**: several distinct ::ErrorCode values can map to the
 *   same `errno`; the reverse cannot always recover the original value.
 *
 * Mapping summary (high level):
 * - `0` → ::NO_ERROR.
 * - Memory: `ENOMEM` → ::OUT_OF_MEMORY; `EOVERFLOW` → ::LENGTH_OVERFLOW;
 *   `ERANGE` → ::NUMERIC_OVERFLOW.
 * - Math/domain: `EDOM` → ::DOMAIN_ERROR.
 * - State/container: `EFAULT` → ::STATE_CORRUPT; `EALREADY` → ::ALREADY_INITIALIZED;
 *   `ENOENT` → ::NOT_FOUND; `EBUSY` → ::CONCURRENT_MODIFICATION.
 * - I/O: `EIO` → ::FILE_READ; `EACCES` → ::PERMISSION_DENIED; `EINTR` → ::IO_INTERRUPTED;
 *   `ETIMEDOUT` → ::IO_TIMEOUT; `EBADF` → ::IO_CLOSED;
 *   `EWOULDBLOCK`/`EAGAIN` → ::IO_WOULD_BLOCK (or ::LOCK_FAILED when distinct—see below).
 * - Type/format: `EILSEQ` → ::ENCODING_INVALID.
 * - Argument/usage: `EINVAL` → ::INVALID_ARG.
 * - Concurrency: `EDEADLK`/`EDEADLOCK` → ::DEADLOCK_DETECTED; `ECANCELED` → ::CANCELLED.
 * - Config/env: `ENOTSUP`/`EOPNOTSUPP` → ::UNSUPPORTED; `EPERM` → ::FEATURE_DISABLED;
 *   `ENOSPC` → ::RESOURCE_EXHAUSTED.
 * - Generic: `ENOSYS` → ::NOT_IMPLEMENTED; any other/unknown value → ::UNKNOWN.
 *
 * Platform/alias handling:
 * - `EWOULDBLOCK` may equal `EAGAIN`. If **equal**, both map canonically to ::IO_WOULD_BLOCK.
 *   If **distinct**, `EWOULDBLOCK` → ::IO_WOULD_BLOCK and `EAGAIN` → ::LOCK_FAILED.
 * - `ENOTSUP` and `EOPNOTSUPP` may alias; both map to ::UNSUPPORTED.
 * - `EDEADLK` and `EDEADLOCK` may alias; both map to ::DEADLOCK_DETECTED.
 * - Optional errno values (`ENODATA`, `EILSEQ`, `ECANCELED`, `EALREADY`) are used
 *   when available; code is guarded with `#ifdef` to stay portable.
 *
 * @param e  A POSIX `errno` value (e.g., the current `errno`).
 *
 * @return A canonical ::ErrorCode for @p e. For `e == 0`, returns ::NO_ERROR.
 *
 * @retval NO_ERROR            For `0`.
 * @retval OUT_OF_MEMORY       For `ENOMEM`.
 * @retval LENGTH_OVERFLOW     For `EOVERFLOW`.
 * @retval NUMERIC_OVERFLOW    For `ERANGE`.
 * @retval DOMAIN_ERROR        For `EDOM`.
 * @retval NOT_FOUND           For `ENOENT`.
 * @retval PERMISSION_DENIED   For `EACCES`.
 * @retval IO_INTERRUPTED      For `EINTR`.
 * @retval IO_TIMEOUT          For `ETIMEDOUT`.
 * @retval IO_CLOSED           For `EBADF`.
 * @retval IO_WOULD_BLOCK      For `EWOULDBLOCK` (or for `EAGAIN` when it aliases).
 * @retval LOCK_FAILED         For `EAGAIN` (when distinct from `EWOULDBLOCK`).
 * @retval UNINITIALIZED       For `ENODATA` (if defined).
 * @retval ENCODING_INVALID    For `EILSEQ` (if defined).
 * @retval FILE_READ           For `EIO`.
 * @retval STATE_CORRUPT       For `EFAULT`.
 * @retval ALREADY_INITIALIZED For `EALREADY` (if defined).
 * @retval NOT_IMPLEMENTED     For `ENOSYS`.
 * @retval DEADLOCK_DETECTED   For `EDEADLK`/`EDEADLOCK` (if defined).
 * @retval UNSUPPORTED         For `ENOTSUP`/`EOPNOTSUPP`.
 * @retval CANCELLED           For `ECANCELED` (if defined).
 * @retval FEATURE_DISABLED    For `EPERM`.
 * @retval RESOURCE_EXHAUSTED  For `ENOSPC`.
 * @retval INVALID_ARG         For `EINVAL`.
 * @retval UNKNOWN             For any other value.
 *
 * @note Keep this policy consistent with ::set_errno_from_error. If you change
 *       either mapping, update the other and adjust tests accordingly.
 *
 * @par Example
 * @code{.c}
 * if (read(fd, buf, n) < 0) {
 *     ErrorCode ec = error_from_errno(errno);
 *     log_error("[%s/%d] %s (errno=%d:%s)",
 *               error_category_tag(ec), (int)ec, error_to_string(ec),
 *               errno, strerror(errno));
 * }
 * @endcode
 */
ErrorCode error_from_errno(int e);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* error_codes_H */
// ================================================================================
// ================================================================================
// eof
