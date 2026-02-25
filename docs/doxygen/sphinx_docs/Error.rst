.. _error_code_file:

*******************
Error Code Overview
*******************

Purpose
=======

The ``c_error.h`` interface defines a common error model for the **csalt** library.
This error handling model provides a protable alternative to ``errno`` which is 
not portable, nor defined in any standard.

Approach
========

The ``c_error.h`` module provides a flexible error-handling framework that can
be used independently by consumers of the ``csalt`` library. Within ``csalt``,
however, a *hierarchical* error-handling strategy is applied. This strategy
recognizes that not all API surfaces require the same level of diagnostic detail,
and that error reporting must be balanced against performance and usability.

Error reporting in ``csalt`` functions typically falls into one of the
following four categories:

1. **No Error Reporting**  
   The function provides no indication of success or failure.  
   Such functions are typically internal utilities or performance-critical
   primitives where input validity is assumed and misuse constitutes a coding
   error rather than a runtime condition.

2. **Potential Error (Ambiguous Return Value)**  
   The function returns a value that *may* indicate an error, but the return
   value itself could also represent a valid condition.  
   Examples include:
   - returning ``NULL`` from an allocation function,
   - returning ``0`` when ``0`` may be a valid size or count.

   In these cases, callers must interpret results based on context.

3. **Error Indication (Unambiguous Failure)**
   The function returns a value that *always* indicates failure (e.g., ``NULL``),
   but provides **no information** about the cause.  
   Users must consult the documentation to understand possible failure modes.

4. **Error Description (Preferred Maximum Detail)**  
   The function returns an ``expected``-style struct such as ``arena_expect_t``
   or ``void_ptr_expect_t`` that:
   - explicitly indicates success or failure, and  
   - provides a specific ``error_code_t`` describing the failure cause.

   This form provides the most complete diagnostic information.

Design Rationale
----------------

Although type-4 error handling is the most expressive, it also introduces
overhead: callers must unpack a result structure and branch on success or
failure, which may be undesirable in extremely hot code paths. Therefore,
``csalt`` applies the following guiding principles:

- **Hot-path functions** (e.g., ``alloc_arena``) typically use type 2 or 3 error
  handling to avoid overhead in tight loops.
- **Initialization functions**, resource acquisition, and configuration APIs
  typically use type 4 error handling because failures are rare but important to
  diagnose accurately.
- **Coding errors** (e.g., passing ``NULL`` where a valid pointer is required)
  are generally *not* the target of the error-handling system. These are best
  identified by:
  - debug builds,
  - assertions,
  - static analysis, or
  - MISRA compliance tools.

  If type-4 handling is in use for a given function, coding errors may still
  manifest as ``error_code_t`` results, but this is considered a *byproduct* rather
  than an intentional feature.

Summary
-------

The error-handling model in ``csalt`` is intentionally flexible. It allows users
to select simple or detailed error reporting based on their needs while the
library maintains clear conventions that balance safety, diagnosability, and
performance.

Design Notes
============

- **Status location:** Each csalt object carries its own ``error_code_t`` so status is
  scoped to the object that was acted upon (no global state).
- **Return values:** Functions may return useful values directly (e.g., sizes, counts)
  while reporting failure through the object’s error field.
- **String conversion:** Use :c:func:`error_to_string` to produce a human-readable
  description suitable for any logging framework (the csalt core remains logger-agnostic).

API Reference
=============

ErrorCode enumeration (overview)
--------------------------------

The table below summarizes each :c:enum:`error_code_t` value, its category, and intended use.
Authoritative comments live in ``c_error.h``; this overview is for quick reference.

.. list-table::
   :header-rows: 1
   :widths: 22 10 16 52
   :align: left

   * - Name
     - Value
     - Category
     - Description
   * - ``INVALID_ERROR``
     - ``1``
     - meta
     - Sentinel indicating an *invalid error argument* was provided to an API expecting a valid ``ErrorCode``. Not produced by normal csalt operations.
   * - ``NO_ERROR``
     - ``0``
     - none
     - Operation completed successfully.

   * - ``INVALID_ARG``
     - ``-101``
     - argument
     - Invalid function argument or combination.
   * - ``NULL_POINTER``
     - ``-102``
     - argument
     - A required pointer argument was ``NULL``.
   * - ``OUT_OF_BOUNDS``
     - ``-103``
     - argument
     - Index was outside valid range for the target container/object.
   * - ``SIZE_MISMATCH``
     - ``-104``
     - argument
     - Dimensions/shapes of inputs are incompatible for the requested operation.
   * - ``UNINITIALIZED``
     - ``-105``
     - argument
     - Attempt to access or operate on an uninitialized element/field.
   * - ``ITERATOR_INVALID``
     - ``-106``
     - argument
     - Dangling/invalid iterator or cursor.
   * - ``PRECONDITION_FAIL``
     - ``-107``
     - argument
     - Required precondition not satisfied.
   * - ``POSTCONDITION_FAIL``
     - ``-108``
     - argument
     - Postcondition or invariant failed.
   * - ``ILLEGAL_STATE``
     - ``-109``
     - argument
     - API call not valid for the current state.

   * - ``BAD_ALLOC``
     - ``-201``
     - memory
     - Memory allocation failed (e.g., ``malloc``/``calloc`` returned ``NULL``).
   * - ``REALLOC_FAIL``
     - ``-202``
     - memory
     - Reallocation failed (``realloc`` returned ``NULL``; original buffer retained).
   * - ``OUT_OF_MEMORY``
     - ``-203``
     - memory
     - System out of memory or allocator could not satisfy the request.
   * - ``LENGTH_OVERFLOW``
     - ``-204``
     - memory
     - Size/length arithmetic overflow (e.g., ``n*sizeof(T)``).
   * - ``CAPACITY_OVERFLOW``
     - ``-205``
     - memory
     - Capacity limit exceeded (policy or representable limit).
   * - ``ALIGNMENT_ERROR``
     - ``-206``
     - memory
     - Required alignment not satisfied.

   * - ``STATE_CORRUPT``
     - ``-301``
     - state
     - Internal invariant violated / corruption detected.
   * - ``ALREADY_INITIALIZED``
     - ``-302``
     - state
     - Double-initialization or conflicting initialization.
   * - ``NOT_FOUND``
     - ``-303``
     - state
     - Requested item/key not found.
   * - ``EMPTY``
     - ``-304``
     - state
     - Container empty when an element was required.
   * - ``CONCURRENT_MODIFICATION``
     - ``-305``
     - state
     - Container modified during iteration (fail-fast).

   * - ``DIV_BY_ZERO``
     - ``-401``
     - math
     - Division by zero detected during computation.
   * - ``SINGULAR_MATRIX``
     - ``-402``
     - math
     - Matrix is singular / non-invertible for the requested operation.
   * - ``NUMERIC_OVERFLOW``
     - ``-403``
     - math
     - Numeric overflow/underflow (range error).
   * - ``DOMAIN_ERROR``
     - ``-404``
     - math
     - Math domain error (input outside valid domain).
   * - ``LOSS_OF_PRECISION``
     - ``-405``
     - math
     - Excessive loss of numeric precision / instability.

   * - ``FILE_OPEN``
     - ``-501``
     - I/O
     - File/handle could not be opened (missing, permissions, etc.).
   * - ``FILE_READ``
     - ``-502``
     - I/O
     - Error while reading from a file/stream.
   * - ``FILE_WRITE``
     - ``-503``
     - I/O
     - Error while writing to a file/stream.
   * - ``PERMISSION_DENIED``
     - ``-504``
     - I/O
     - Permission/ACL denied.
   * - ``IO_INTERRUPTED``
     - ``-505``
     - I/O
     - Operation interrupted (e.g., ``EINTR``).
   * - ``IO_TIMEOUT``
     - ``-506``
     - I/O
     - I/O timed out.
   * - ``IO_CLOSED``
     - ``-507``
     - I/O
     - Operation on a closed stream/descriptor.
   * - ``IO_WOULD_BLOCK``
     - ``-508``
     - I/O
     - Non-blocking operation would block.

   * - ``TYPE_MISMATCH``
     - ``-601``
     - type/format
     - Incompatible or unexpected type encountered.
   * - ``FORMAT_INVALID``
     - ``-602``
     - type/format
     - Invalid/unsupported data format.
   * - ``ENCODING_INVALID``
     - ``-603``
     - type/format
     - Invalid text encoding (e.g., UTF-8).
   * - ``PARSING_FAILED``
     - ``-604``
     - type/format
     - Parsing error.
   * - ``VALIDATION_FAILED``
     - ``-605``
     - type/format
     - Data failed validation rules.

   * - ``LOCK_FAILED``
     - ``-701``
     - concurrency
     - Failed to acquire/initialize lock.
   * - ``DEADLOCK_DETECTED``
     - ``-702``
     - concurrency
     - Potential or actual deadlock detected.
   * - ``THREAD_FAIL``
     - ``-703``
     - concurrency
     - Thread create/join failure.
   * - ``CANCELLED``
     - ``-704``
     - concurrency
     - Operation cancelled.
   * - ``RACE_DETECTED``
     - ``-705``
     - concurrency
     - Data race / unsynchronized access.

   * - ``CONFIG_INVALID``
     - ``-801``
     - config
     - Invalid configuration value/state.
   * - ``UNSUPPORTED``
     - ``-802``
     - config
     - Unsupported feature or platform.
   * - ``FEATURE_DISABLED``
     - ``-803``
     - config
     - Feature disabled by policy/build flags.
   * - ``VERSION_MISMATCH``
     - ``-804``
     - config
     - ABI/format/library version mismatch.
   * - ``RESOURCE_EXHAUSTED``
     - ``-805``
     - config
     - Non-memory resource exhausted (e.g., file descriptors).

   * - ``NOT_IMPLEMENTED``
     - ``-981``
     - generic
     - Functionality not yet implemented.
   * - ``OPERATION_UNAVAILABLE``
     - ``-982``
     - generic
     - Operation unavailable in current build/runtime.
   * - ``UNKNOWN``
     - ``-999``
     - generic
     - Fallback for unspecified/unknown failures.

Legend
~~~~~~
- **argument (-1xx)**: invalid inputs, NULLs, bounds, uninitialized access, pre/postcondition failures, illegal state for the call
- **memory (-2xx)**: allocation/reallocation failures, out-of-memory, size/capacity arithmetic overflow, alignment issues
- **state (-3xx)**: container/engine state problems (corruption, already initialized, not found, empty, concurrent modification)
- **math (-4xx)**: arithmetic/domain issues (divide by zero, singular matrices, range/overflow, excessive precision loss)
- **I/O (-5xx)**: filesystem/stream/socket operations (open/read/write, permissions, interrupted, timeout, closed, would-block)
- **type/format (-6xx)**: type mismatches, invalid format/encoding, parsing and validation failures
- **concurrency (-7xx)**: locking/threading issues (lock failure, deadlock, cancellation, data race)
- **config (-8xx)**: configuration/policy/environment (unsupported features, disabled by policy, version mismatch, non-memory resource exhaustion)
- **generic (-9xx)**: not implemented / unavailable / unknown
- **meta**: sentinel values not produced by normal operations (e.g., ``INVALID_ERROR = 1``); **none**: success (``NO_ERROR = 0``)

See also
~~~~~~~~
- :c:func:`error_to_string` — convert any :c:enum:`error_code_t` to a short human-readable message (strings defined in code comments).


Logging Helper Functions 
------------------------

The following function converts error codes into textual descriptions:

error_to_string 
~~~~~~~~~~~~~~~
.. doxygenfunction:: error_to_string
   :project: csalt

error_cat_to_string 
~~~~~~~~~~~~~~~~~~~
.. doxygenfunction:: error_cat_to_string
   :project: csalt

Error Category Predicates 
-------------------------
These helpers classify an :c:enum:`error_code_t` into the range-based taxonomy
(-1xx argument, -2xx memory, …). Each function returns ``true`` iff the code
belongs to that category.

.. code-block:: c

   #include "c_error.h"

   void log_by_category(error_code_t ec) {
       /* Fast boolean checks (header-defined static inline) */
       if (ec_is_mem(ec)) {
           fprintf(stderr, "[MEM/-2xx] %s\n", error_to_string(ec));
       } else if (ec_is_io(ec)) {
           fprintf(stderr, "[IO/-5xx] %s\n", error_to_string(ec));
       } else if (ec_is_arg(ec)) {
           fprintf(stderr, "[ARG/-1xx] %s\n", error_to_string(ec));
       } else if (ec_is_state(ec)) {
           fprintf(stderr, "[STATE/-3xx] %s\n", error_to_string(ec));
       } else if (ec_is_math(ec)) {
           fprintf(stderr, "[MATH/-4xx] %s\n", error_to_string(ec));
       } else if (ec_is_fmt(ec)) {
           fprintf(stderr, "[FMT/-6xx] %s\n", error_to_string(ec));
       } else if (ec_is_conc(ec)) {
           fprintf(stderr, "[CONC/-7xx] %s\n", error_to_string(ec));
       } else if (ec_is_cfg(ec)) {
           fprintf(stderr, "[CFG/-8xx] %s\n", error_to_string(ec));
       } else if (ec_is_gen(ec)) {
           fprintf(stderr, "[GEN/-9xx] %s\n", error_to_string(ec));
       } else {
           /* NO_ERROR / INVALID_ERROR (meta) or out-of-range */
           fprintf(stderr, "[NONE] %s\n", error_to_string(ec));
       }
   }

.. doxygenfunction:: ec_is_arg
   :project: csalt

.. doxygenfunction:: ec_is_mem
   :project: csalt

.. doxygenfunction:: ec_is_state
   :project: csalt

.. doxygenfunction:: ec_is_math
   :project: csalt

.. doxygenfunction:: ec_is_io
   :project: csalt

.. doxygenfunction:: ec_is_fmt
   :project: csalt

.. doxygenfunction:: ec_is_conc
   :project: csalt

.. doxygenfunction:: ec_is_cfg
   :project: csalt

.. doxygenfunction:: ec_is_gen
   :project: csalt

Error Handling Marco 
====================
The library provides a small set of convenience macros to model C++23-style
``std::expected<T, error_code_t>`` in C. These macros are documented in detail
in the C headers; this section simply exposes those docstrings into the
Sphinx documentation.  If the user wishes to ommit macros for explicit type 
performance, especially for scenarios where the code must meet MISRA standards 
these macros can be omitted by compiling with the ``STATIC_ONLY`` flag.

Each macro below is pulled directly from its Doxygen doc comment, so the
authoritative description always lives alongside the implementation.
Provide breif intro here.  

Data Structure 
--------------
The error handling struct used in this library implements the following 
syntax.  In the example it is implemented as a function like macro, but a 
user can implement their own explicit struct so as long as it adheres 
to this construct.

.. code-block:: c

   #define DEFINE_EXPECTED_TYPE(name, T)            \
       typedef struct name {                        \
           bool has_value;                          \
           union {                                  \
               T value;                             \
               ErrorCode error;                     \
           } u;                                     \
       } name

Defied Expected Structures 
--------------------------
The following versions of the ``DEFINE_EXPECTED_TUPE`` macros are pre defined 
in the ``c_error.h`` file. 

.. code-block:: text 

   uint8_t       uint8_expect_t 
   int8_t        int8_expect_t 
   uint32_t      uint32_expect_t 
   int32_t       int32_expect_t 
   uint64_t      uint64_expect_t 
   int64_t       int64_expect_t 
   float         float_expect_t 
   double        double_expect_t 
   long double   ldouble_expect_t
   void*         void_ptr_expect_t 
   uint8_t*      uint8_ptr_expect_t 
   int8_t*       int8_ptr_expect_t 
   uint16_t*     uint16_ptr_expect_t 
   int16_t*      int16_ptr_expect_t 
   uint32_t*     uint32_ptr_expect_t 
   int32_t*      int32_ptr_expect_t 
   uint64_t*     uint64_ptr_expect_t 
   int64_t*      int64_ptr_expect_t 
   float*        float_ptr_expect_t 
   double*       double_ptr_expect_t 
   long double*  ldouble_ptr_expect_t

DEFINE_EXPECTED_TYPE 
--------------------

.. doxygendefine:: DEFINE_EXPECTED_TYPE
   :project: csalt

EXPECTED_OK
-----------

.. doxygendefine:: EXPECTED_OK
   :project: csalt

EXPECTED_ERR 
------------

.. doxygendefine:: EXPECTED_ERR
   :project: csalt

EXPECTED_HAS_VALUE 
------------------

.. doxygendefine:: EXPECTED_HAS_VALUE
   :project: csalt

EXPECTED_VALUE 
--------------

.. doxygendefine:: EXPECTED_VALUE
   :project: csalt

EXPECTED_ERROR 
--------------

.. doxygendefine:: EXPECTED_ERROR
   :project: csalt
