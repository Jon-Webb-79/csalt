// ================================================================================
// ================================================================================
// - File:    test_error.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_error.h"
#include "test_suite.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>


#define ASSERT_TRUE_NAMED(cond, name)  \
    do { if (!(cond)) fail_msg("predicate %s expected TRUE, got FALSE", (name)); } while (0)
#define ASSERT_FALSE_NAMED(cond, name) \
    do { if (cond)   fail_msg("predicate %s expected FALSE, got TRUE", (name)); } while (0)
// ================================================================================ 
// ================================================================================ 
// TEST ERROR_TO_STRING 

/* Small helper to keep assertions consistent. */
static void check(ErrorCode code, const char *expected)
{
    const char *s = error_to_string(code);
    assert_non_null(s);
    assert_string_equal(s, expected);
}

/* --- Individual test cases --- */

static void test_error_to_string_no_error(void **state)
{
    (void)state;
    check(NO_ERROR, "No error");
}

static void test_error_to_string_invalid_error(void **state)
{
    (void)state;
    check(INVALID_ERROR, "Invalid error entry");
}

static void test_error_to_string_argument_cases(void **state)
{
    (void)state;
    check(INVALID_ARG,       "Invalid function argument");
    check(NULL_POINTER,      "Null pointer passed");
    check(OUT_OF_BOUNDS,     "Index out of range");
    check(SIZE_MISMATCH,     "Dimension/size mismatch");
    check(UNINITIALIZED,     "Uninitialized element access");
    check(ITERATOR_INVALID,  "Invalid iterator/cursor");
    check(PRECONDITION_FAIL, "Precondition failed");
    check(POSTCONDITION_FAIL,"Postcondition failed");
    check(ILLEGAL_STATE,     "Illegal state for operation");
}

static void test_error_to_string_memory_cases(void **state)
{
    (void)state;
    check(BAD_ALLOC,         "Memory allocation failed");
    check(REALLOC_FAIL,      "Memory reallocation failed");
    check(OUT_OF_MEMORY,     "Out of memory");
    check(LENGTH_OVERFLOW,   "Length/size arithmetic overflow");
    check(CAPACITY_OVERFLOW, "Capacity limit exceeded");
    check(ALIGNMENT_ERROR,   "Required alignment not satisfied");
}

static void test_error_to_string_state_cases(void **state)
{
    (void)state;
    check(STATE_CORRUPT,           "Internal state corrupt");
    check(ALREADY_INITIALIZED,     "Already initialized");
    check(NOT_FOUND,               "Item not found");
    check(EMPTY,                   "Container is empty");
    check(CONCURRENT_MODIFICATION, "Concurrent modification detected");
}

static void test_error_to_string_math_cases(void **state)
{
    (void)state;
    check(DIV_BY_ZERO,       "Division by zero");
    check(SINGULAR_MATRIX,   "Singular/non-invertible matrix");
    check(NUMERIC_OVERFLOW,  "Numeric overflow/underflow");
    check(DOMAIN_ERROR,      "Math domain error");
    check(LOSS_OF_PRECISION, "Loss of numeric precision");
}

static void test_error_to_string_io_cases(void **state)
{
    (void)state;
    check(FILE_OPEN,       "Failed to open file/handle");
    check(FILE_READ,       "Error reading from file/handle");
    check(FILE_WRITE,      "Error writing to file/handle");
    check(PERMISSION_DENIED,"Permission denied");
    check(IO_INTERRUPTED,  "I/O interrupted");
    check(IO_TIMEOUT,      "I/O timed out");
    check(IO_CLOSED,       "Operation on closed stream/descriptor");
    check(IO_WOULD_BLOCK,  "Operation would block");
}

static void test_error_to_string_format_cases(void **state)
{
    (void)state;
    check(TYPE_MISMATCH,     "Type mismatch");
    check(FORMAT_INVALID,    "Invalid data format");
    check(ENCODING_INVALID,  "Invalid text encoding");
    check(PARSING_FAILED,    "Parsing failed");
    check(VALIDATION_FAILED, "Validation failed");
}

static void test_error_to_string_concurrency_cases(void **state)
{
    (void)state;
    check(LOCK_FAILED,        "Lock operation failed");
    check(DEADLOCK_DETECTED,  "Deadlock detected");
    check(THREAD_FAIL,        "Thread operation failed");
    check(CANCELLED,          "Operation cancelled");
    check(RACE_DETECTED,      "Data race detected");
}

static void test_error_to_string_config_cases(void **state)
{
    (void)state;
    check(CONFIG_INVALID,     "Invalid configuration");
    check(UNSUPPORTED,        "Unsupported feature/platform");
    check(FEATURE_DISABLED,   "Feature disabled by policy/build");
    check(VERSION_MISMATCH,   "Version/ABI mismatch");
    check(RESOURCE_EXHAUSTED, "Resource exhausted");
}

static void test_error_to_string_generic_cases(void **state)
{
    (void)state;
    check(NOT_IMPLEMENTED,       "Not implemented");
    check(OPERATION_UNAVAILABLE, "Operation unavailable");
    check(UNKNOWN,               "Unknown error");
}

/* Ensure the default fallback literal is returned for unmapped values. */
static void test_error_to_string_fallback_unrecognized(void **state)
{
    (void)state;
    /* Pick a value outside your enum set: negative and not in any case label. */
    const char *s = error_to_string((ErrorCode)(-7777));
    assert_non_null(s);
    assert_string_equal(s, "Unrecognized error code");
}

/* Same input twice returns the *same pointer* (string literal stability). */
static void test_error_to_string_pointer_stability(void **state)
{
    (void)state;
    const char *a = error_to_string(BAD_ALLOC);
    const char *b = error_to_string(BAD_ALLOC);
    assert_ptr_equal(a, b);
}
// ================================================================================ 
// ================================================================================ 
// TEST ERROR_CAT_TO_STRING 

static void check_cat(ErrorCategory c, const char *expected)
{
    const char *s = error_cat_to_string(c);
    assert_non_null(s);
    assert_string_equal(s, expected);
}

static void test_error_cat_to_string_all(void **state)
{
    (void)state;

    struct {
        ErrorCategory c;
        const char   *s;
    } cases[] = {
        { ECAT_NONE,  "NO ERROR" },
        { ECAT_ARG,   "ARGUMENT ERROR" },
        { ECAT_MEM,   "MEMORY ERROR" },
        { ECAT_STATE, "STATE ERROR" },
        { ECAT_MATH,  "MATH ERROR" },
        { ECAT_IO,    "I/O ERROR" },
        { ECAT_FMT,   "TYPE/FORMAT ERROR" },
        { ECAT_CONC,  "CONCURRENCY ERROR" },
        { ECAT_CFG,   "CONFIG ERROR" },
        { ECAT_GEN,   "GENERIC ERROR" },
    };

    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        check_cat(cases[i].c, cases[i].s);
    }
}

static void test_error_cat_to_string_unknown(void **state)
{
    (void)state;
    /* Value outside defined categories should hit the default. */
    const char *s = error_cat_to_string((ErrorCategory)9999);
    assert_non_null(s);
    assert_string_equal(s, "UNKNOWN ERROR");
}

static void test_error_cat_to_string_pointer_stability(void **state)
{
    (void)state;
    /* Same input should return the same literal pointer each time. */
    const char *a = error_cat_to_string(ECAT_MEM);
    const char *b = error_cat_to_string(ECAT_MEM);
    assert_ptr_equal(a, b);
}
// ================================================================================ 
// ================================================================================ 
// TEST ERRNO_FROM_ERROR 

/* Helper: ensure both return value and global errno match expectation. */
static void expect_errno_for(ErrorCode code, int expected_errno)
{
    errno = INT_MIN;  /* poison */
    int ret = set_errno_from_error(code);
    assert_int_equal(ret, expected_errno);
    assert_int_equal(errno, expected_errno);
}

/* --- Core cases --- */

static void test_set_errno_no_error(void **state)
{
    (void)state;
    expect_errno_for(NO_ERROR, 0);
}

static void test_set_errno_invalid_error(void **state)
{
    (void)state;
    expect_errno_for(INVALID_ERROR, EINVAL);
}

/* -1xx Argument/Input */
static void test_set_errno_argument_group(void **state)
{
    (void)state;
    expect_errno_for(INVALID_ARG,       EINVAL);
    expect_errno_for(NULL_POINTER,      EINVAL);
    expect_errno_for(OUT_OF_BOUNDS,     ERANGE);
    expect_errno_for(SIZE_MISMATCH,     EINVAL);
    expect_errno_for(ITERATOR_INVALID,  EINVAL);
    expect_errno_for(PRECONDITION_FAIL, EINVAL);
    expect_errno_for(POSTCONDITION_FAIL,EINVAL);
    expect_errno_for(ILLEGAL_STATE,     EINVAL);
}

/* Special-case mapping: ENODATA when available, else EINVAL */
static void test_set_errno_uninitialized_special(void **state)
{
    (void)state;
#ifdef ENODATA
    expect_errno_for(UNINITIALIZED, ENODATA);
    expect_errno_for(EMPTY,         ENODATA);
#else
    expect_errno_for(UNINITIALIZED, EINVAL);
    expect_errno_for(EMPTY,         EINVAL);
#endif
}

/* -2xx Memory/Allocation */
static void test_set_errno_memory_group(void **state)
{
    (void)state;
    expect_errno_for(BAD_ALLOC,    ENOMEM);
    expect_errno_for(REALLOC_FAIL, ENOMEM);
    expect_errno_for(OUT_OF_MEMORY,ENOMEM);
    expect_errno_for(ALIGNMENT_ERROR, EINVAL);
}

static void test_set_errno_overflow_group(void **state) {
    (void)state;
    expect_errno_for(LENGTH_OVERFLOW,   EOVERFLOW);
    expect_errno_for(CAPACITY_OVERFLOW, EOVERFLOW);
}

/* -3xx State/Container */
static void test_set_errno_state_group(void **state) {
    (void)state;
    expect_errno_for(STATE_CORRUPT, EFAULT);
#ifdef EALREADY
    expect_errno_for(ALREADY_INITIALIZED, EALREADY);
#else
    expect_errno_for(ALREADY_INITIALIZED, EINVAL);
#endif
    expect_errno_for(NOT_FOUND, ENOENT);
    expect_errno_for(CONCURRENT_MODIFICATION, EBUSY);
}

/* -4xx Math/Domain */
static void test_set_errno_math_group(void **state) {
    (void)state;
    expect_errno_for(DIV_BY_ZERO,      EDOM);
    expect_errno_for(SINGULAR_MATRIX,  EDOM);
    expect_errno_for(DOMAIN_ERROR,     EDOM);
    expect_errno_for(NUMERIC_OVERFLOW, ERANGE);
    expect_errno_for(LOSS_OF_PRECISION,ERANGE);
}

/* -5xx I/O */
static void test_set_errno_io_group(void **state) {
    (void)state;
    expect_errno_for(FILE_OPEN,  EIO);
    expect_errno_for(FILE_READ,  EIO);
    expect_errno_for(FILE_WRITE, EIO);
    expect_errno_for(PERMISSION_DENIED, EACCES);
    expect_errno_for(IO_INTERRUPTED,    EINTR);
    expect_errno_for(IO_TIMEOUT,        ETIMEDOUT);
    expect_errno_for(IO_CLOSED,         EBADF);
#ifdef EWOULDBLOCK
    expect_errno_for(IO_WOULD_BLOCK, EWOULDBLOCK);
#else
    expect_errno_for(IO_WOULD_BLOCK, EAGAIN);
#endif
}

/* -6xx Type/Format/Encoding */
static void test_set_errno_format_group(void **state) {
    (void)state;
    expect_errno_for(TYPE_MISMATCH,    EINVAL);
    expect_errno_for(FORMAT_INVALID,   EINVAL);
    expect_errno_for(PARSING_FAILED,   EINVAL);
    expect_errno_for(VALIDATION_FAILED,EINVAL);
#ifdef EILSEQ
    expect_errno_for(ENCODING_INVALID, EILSEQ);
#else
    expect_errno_for(ENCODING_INVALID, EINVAL);
#endif
}

/* -7xx Concurrency/Synchronization */
static void test_set_errno_concurrency_group(void **state) {
    (void)state;
    expect_errno_for(LOCK_FAILED,  EAGAIN);
    expect_errno_for(THREAD_FAIL,  EAGAIN);
#ifdef EDEADLK
    expect_errno_for(DEADLOCK_DETECTED, EDEADLK);
#endif
#ifdef ECANCELED
    expect_errno_for(CANCELLED, ECANCELED);
#else
    expect_errno_for(CANCELLED, EINVAL);
#endif
    expect_errno_for(RACE_DETECTED, EFAULT);
}

/* -8xx Config/Env */
static void test_set_errno_config_group(void **state) {
    (void)state;
    expect_errno_for(CONFIG_INVALID,     EINVAL);
    expect_errno_for(VERSION_MISMATCH,   EINVAL);
#ifdef ENOTSUP
    expect_errno_for(UNSUPPORTED, ENOTSUP);
#elif defined(EOPNOTSUPP)
    expect_errno_for(UNSUPPORTED, EOPNOTSUPP);
#endif
    expect_errno_for(FEATURE_DISABLED,   EPERM);
    expect_errno_for(RESOURCE_EXHAUSTED, ENOSPC);
}

/* -9xx Generic/Fallback */
static void test_set_errno_generic_group(void **state) {
    (void)state;
    expect_errno_for(NOT_IMPLEMENTED,       ENOSYS);
    expect_errno_for(OPERATION_UNAVAILABLE, EAGAIN);
    expect_errno_for(UNKNOWN,               EINVAL);

    /* Also verify the default path for a value outside the enum. */
    expect_errno_for((ErrorCode)(-7777), EINVAL);
}
// ================================================================================ 
// ================================================================================ 
// TEST ERROR_FROM_ERRNO 

static void expect_error_for_errno(int e, ErrorCode expected)
{
    int old = 12345;          /* sentinel */
    errno = old;
    ErrorCode ec = error_from_errno(e);
    assert_int_equal(ec, expected);
    assert_int_equal(errno, old);  /* must not change errno */
}

/* --- Tests --- */

static void test_error_from_errno_zero_success(void **state) {
    (void)state;
    expect_error_for_errno(0, NO_ERROR);
}

static void test_error_from_errno_core_mappings(void **state) {
    (void)state;
    expect_error_for_errno(ENOMEM,      OUT_OF_MEMORY);
    expect_error_for_errno(EOVERFLOW,   LENGTH_OVERFLOW);
    expect_error_for_errno(ERANGE,      NUMERIC_OVERFLOW);
    expect_error_for_errno(EDOM,        DOMAIN_ERROR);

    expect_error_for_errno(ENOENT,      NOT_FOUND);
    expect_error_for_errno(EACCES,      PERMISSION_DENIED);
    expect_error_for_errno(EINTR,       IO_INTERRUPTED);
    expect_error_for_errno(ETIMEDOUT,   IO_TIMEOUT);
    expect_error_for_errno(EBADF,       IO_CLOSED);

    expect_error_for_errno(EIO,         FILE_READ);
    expect_error_for_errno(EFAULT,      STATE_CORRUPT);

    expect_error_for_errno(ENOSYS,      NOT_IMPLEMENTED);
    expect_error_for_errno(EPERM,       FEATURE_DISABLED);
    expect_error_for_errno(ENOSPC,      RESOURCE_EXHAUSTED);
    expect_error_for_errno(EBUSY,       CONCURRENT_MODIFICATION);

    expect_error_for_errno(EINVAL,      INVALID_ARG);
}

static void test_error_from_errno_wouldblock_again(void **state) {
    (void)state;
    /* Mirrors the alias logic in the implementation. */
#if defined(EWOULDBLOCK) && defined(EAGAIN)
  #if EWOULDBLOCK != EAGAIN
    expect_error_for_errno(EWOULDBLOCK, IO_WOULD_BLOCK);
    expect_error_for_errno(EAGAIN,      LOCK_FAILED);
  #else
    expect_error_for_errno(EAGAIN,      IO_WOULD_BLOCK);
  #endif
#elif defined(EWOULDBLOCK)
    expect_error_for_errno(EWOULDBLOCK, IO_WOULD_BLOCK);
#elif defined(EAGAIN)
    expect_error_for_errno(EAGAIN,      LOCK_FAILED);
#endif
}

static void test_error_from_errno_optional_codes(void **state) {
    (void)state;
#ifdef ENODATA
    expect_error_for_errno(ENODATA, UNINITIALIZED);
#endif
#ifdef EILSEQ
    expect_error_for_errno(EILSEQ,  ENCODING_INVALID);
#endif
#ifdef EALREADY
    expect_error_for_errno(EALREADY, ALREADY_INITIALIZED);
#endif
#ifdef ECANCELED
    expect_error_for_errno(ECANCELED, CANCELLED);
#endif
}

static void test_error_from_errno_deadlock_aliases(void **state) {
    (void)state;
#if defined(EDEADLK) && defined(EDEADLOCK)
  #if EDEADLK != EDEADLOCK
    expect_error_for_errno(EDEADLK,   DEADLOCK_DETECTED);
    expect_error_for_errno(EDEADLOCK, DEADLOCK_DETECTED);
  #else
    expect_error_for_errno(EDEADLK,   DEADLOCK_DETECTED);
  #endif
#elif defined(EDEADLK)
    expect_error_for_errno(EDEADLK,   DEADLOCK_DETECTED);
#elif defined(EDEADLOCK)
    expect_error_for_errno(EDEADLOCK, DEADLOCK_DETECTED);
#endif
}

static void test_error_from_errno_notsup_aliases(void **state) {
    (void)state;
#if defined(ENOTSUP) && defined(EOPNOTSUPP)
  #if ENOTSUP != EOPNOTSUPP
    expect_error_for_errno(ENOTSUP,    UNSUPPORTED);
    expect_error_for_errno(EOPNOTSUPP, UNSUPPORTED);
  #else
    expect_error_for_errno(ENOTSUP,    UNSUPPORTED);
  #endif
#elif defined(ENOTSUP)
    expect_error_for_errno(ENOTSUP,    UNSUPPORTED);
#elif defined(EOPNOTSUPP)
    expect_error_for_errno(EOPNOTSUPP, UNSUPPORTED);
#endif
}

static void test_error_from_errno_does_not_touch_errno(void **state) {
    (void)state;
    /* Try a few values and ensure errno is never changed. */
    int keep = 7777;
    errno = keep;
    (void)error_from_errno(ENOMEM);
    assert_int_equal(errno, keep);
    (void)error_from_errno(0);
    assert_int_equal(errno, keep);
    (void)error_from_errno(123456); /* unknown */
    assert_int_equal(errno, keep);
}

static void test_error_from_errno_unknown_fallback(void **state) {
    (void)state;
    /* Negative values are safe “not an errno” sentinels on POSIX. */
    expect_error_for_errno(-1, UNKNOWN);
}
// ================================================================================ 
// ================================================================================ 
// TEST BOOL FUNCTIONS 

typedef bool (*pred_fn)(ErrorCode);

/* Order matters: index 0..8 must match categories ARG..GEN below */
static pred_fn PRED[] = {
    ec_is_arg,  ec_is_mem,  ec_is_state, ec_is_math, ec_is_io,
    ec_is_fmt,  ec_is_conc, ec_is_cfg,   ec_is_gen
};
static const char *PRED_NAME[] = {
    "arg","mem","state","math","io","fmt","conc","cfg","gen"
};

/* Helper: check that exactly one predicate (at idx_expected) is true */
static void expect_only(size_t idx_expected, ErrorCode ec) {
    for (size_t i = 0; i < sizeof PRED / sizeof PRED[0]; ++i) {
        bool got  = PRED[i](ec);
        bool want = (i == idx_expected);
        if (want)  { ASSERT_TRUE_NAMED(got,  PRED_NAME[i]); }
        else       { ASSERT_FALSE_NAMED(got, PRED_NAME[i]); }
    }
}

static void expect_none(ErrorCode ec) {
    for (size_t i = 0; i < sizeof PRED / sizeof PRED[0]; ++i) {
        ASSERT_FALSE_NAMED(PRED[i](ec), PRED_NAME[i]);
    }
}

static void test_ec_predicates_each_category(void **state) {
    (void)state;

    /* -1xx argument */
    ErrorCode arg_codes[] = {
        INVALID_ARG, NULL_POINTER, OUT_OF_BOUNDS, SIZE_MISMATCH, UNINITIALIZED,
        ITERATOR_INVALID, PRECONDITION_FAIL, POSTCONDITION_FAIL, ILLEGAL_STATE
    };
    for (size_t i = 0; i < sizeof arg_codes/sizeof *arg_codes; ++i)
        expect_only(0, arg_codes[i]); /* idx 0 = ARG */

    /* -2xx memory */
    ErrorCode mem_codes[] = {
        BAD_ALLOC, REALLOC_FAIL, OUT_OF_MEMORY, LENGTH_OVERFLOW,
        CAPACITY_OVERFLOW, ALIGNMENT_ERROR
    };
    for (size_t i = 0; i < sizeof mem_codes/sizeof *mem_codes; ++i)
        expect_only(1, mem_codes[i]); /* idx 1 = MEM */

    /* -3xx state */
    ErrorCode state_codes[] = {
        STATE_CORRUPT, ALREADY_INITIALIZED, NOT_FOUND, EMPTY, CONCURRENT_MODIFICATION
    };
    for (size_t i = 0; i < sizeof state_codes/sizeof *state_codes; ++i)
        expect_only(2, state_codes[i]); /* idx 2 = STATE */

    /* -4xx math */
    ErrorCode math_codes[] = {
        DIV_BY_ZERO, SINGULAR_MATRIX, NUMERIC_OVERFLOW, DOMAIN_ERROR, LOSS_OF_PRECISION
    };
    for (size_t i = 0; i < sizeof math_codes/sizeof *math_codes; ++i)
        expect_only(3, math_codes[i]); /* idx 3 = MATH */

    /* -5xx io */
    ErrorCode io_codes[] = {
        FILE_OPEN, FILE_READ, FILE_WRITE, PERMISSION_DENIED, IO_INTERRUPTED,
        IO_TIMEOUT, IO_CLOSED, IO_WOULD_BLOCK
    };
    for (size_t i = 0; i < sizeof io_codes/sizeof *io_codes; ++i)
        expect_only(4, io_codes[i]); /* idx 4 = IO */

    /* -6xx format */
    ErrorCode fmt_codes[] = {
        TYPE_MISMATCH, FORMAT_INVALID, ENCODING_INVALID, PARSING_FAILED, VALIDATION_FAILED
    };
    for (size_t i = 0; i < sizeof fmt_codes/sizeof *fmt_codes; ++i)
        expect_only(5, fmt_codes[i]); /* idx 5 = FMT */

    /* -7xx concurrency */
    ErrorCode conc_codes[] = {
        LOCK_FAILED, DEADLOCK_DETECTED, THREAD_FAIL, CANCELLED, RACE_DETECTED
    };
    for (size_t i = 0; i < sizeof conc_codes/sizeof *conc_codes; ++i)
        expect_only(6, conc_codes[i]); /* idx 6 = CONC */

    /* -8xx config */
    ErrorCode cfg_codes[] = {
        CONFIG_INVALID, UNSUPPORTED, FEATURE_DISABLED, VERSION_MISMATCH, RESOURCE_EXHAUSTED
    };
    for (size_t i = 0; i < sizeof cfg_codes/sizeof *cfg_codes; ++i)
        expect_only(7, cfg_codes[i]); /* idx 7 = CFG */

    /* -9xx generic */
    ErrorCode gen_codes[] = {
        NOT_IMPLEMENTED, OPERATION_UNAVAILABLE, UNKNOWN
    };
    for (size_t i = 0; i < sizeof gen_codes/sizeof *gen_codes; ++i)
        expect_only(8, gen_codes[i]); /* idx 8 = GEN */
}

static void test_ec_predicates_nonerror_values(void **state) {
    (void)state;
    expect_none(NO_ERROR);
    expect_none(INVALID_ERROR); /* meta/sentinel, not a category */
}

/* Optional: for an out-of-range negative value, no predicate should match. */
static void test_ec_predicates_unknown_negative(void **state) {
    (void)state;
    ErrorCode weird = (ErrorCode)(-7777); /* not in -1xx..-9xx enum set */
    expect_none(weird);
}
// ================================================================================ 
// ================================================================================ 

const struct CMUnitTest test_error[] = {
    cmocka_unit_test(test_error_to_string_no_error),
    cmocka_unit_test(test_error_to_string_invalid_error),
    cmocka_unit_test(test_error_to_string_argument_cases),
    cmocka_unit_test(test_error_to_string_memory_cases),
    cmocka_unit_test(test_error_to_string_state_cases),
    cmocka_unit_test(test_error_to_string_math_cases),
    cmocka_unit_test(test_error_to_string_io_cases),
    cmocka_unit_test(test_error_to_string_format_cases),
    cmocka_unit_test(test_error_to_string_concurrency_cases),
    cmocka_unit_test(test_error_to_string_config_cases),
    cmocka_unit_test(test_error_to_string_generic_cases),
    cmocka_unit_test(test_error_to_string_fallback_unrecognized),
    cmocka_unit_test(test_error_to_string_pointer_stability),
    cmocka_unit_test(test_error_cat_to_string_all),
    cmocka_unit_test(test_error_cat_to_string_unknown),
    cmocka_unit_test(test_error_cat_to_string_pointer_stability),
    cmocka_unit_test(test_set_errno_no_error),
    cmocka_unit_test(test_set_errno_invalid_error),
    cmocka_unit_test(test_set_errno_argument_group),
    cmocka_unit_test(test_set_errno_uninitialized_special),
    cmocka_unit_test(test_set_errno_memory_group),
    cmocka_unit_test(test_set_errno_overflow_group),
    cmocka_unit_test(test_set_errno_state_group),
    cmocka_unit_test(test_set_errno_math_group),
    cmocka_unit_test(test_set_errno_io_group),
    cmocka_unit_test(test_set_errno_format_group),
    cmocka_unit_test(test_set_errno_concurrency_group),
    cmocka_unit_test(test_set_errno_config_group),
    cmocka_unit_test(test_set_errno_generic_group),
    cmocka_unit_test(test_error_from_errno_zero_success),
    cmocka_unit_test(test_error_from_errno_core_mappings),
    cmocka_unit_test(test_error_from_errno_wouldblock_again),
    cmocka_unit_test(test_error_from_errno_optional_codes),
    cmocka_unit_test(test_error_from_errno_deadlock_aliases),
    cmocka_unit_test(test_error_from_errno_notsup_aliases),
    cmocka_unit_test(test_error_from_errno_does_not_touch_errno),
    cmocka_unit_test(test_error_from_errno_unknown_fallback),
};

const size_t test_error_count = sizeof(test_error) / sizeof(test_error[0]);
// ================================================================================
// ================================================================================
// eof
