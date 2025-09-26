.. _error_code_file:

*******************
Error Code Overview
*******************

Purpose
=======

The ``error_codes.h`` interface defines a common error model for the **csalt** library.
Every public data structure in csalt exposes a field of type ``ErrorCode`` and every
public function that can fail sets this error field on the output object it modifies.
However, any function that allocates a data structure or destroys a data structure will 
rely on `errno` for error handling.  In addition, data structures passed to a function 
with a `const` reference will not have error fields set on the output object.  In addition 
at the beginning of each function where an error code can be set on an object it 
is reset to `NO_ERROR` to ensure that any attached error codes are local to the  
function.
This allows callers to:

1. Invoke an operation on a csalt object.
2. Retrieve the most recent error condition from that object.
3. Convert the error to a human-readable message for logs or diagnostics.

By convention, ``NO_ERROR`` is ``0`` and all error conditions are **negative** values.
This avoids collisions with valid sizes/indices and allows simple checks like
``if (err < 0)`` to detect failures.


Typical Usage
=============

.. code-block:: c

   #include "error_codes.h"
   #include "c_float.h"

   float_v* vec = init_float_vector(10);
   bool val = push_back_float_vector(vec, 10.0);
   if (!val) printf("%s: %s, "Push Vector", error_to_string(float_vector_error(vec)));
   free_float_vector(vec);

Design Notes
============

- **Status location:** Each csalt object carries its own ``ErrorCode`` so status is
  scoped to the object that was acted upon (no global state).
- **Return values:** Functions may return useful values directly (e.g., sizes, counts)
  while reporting failure through the object’s error field.
- **String conversion:** Use :c:func:`error_to_string` to produce a human-readable
  description suitable for any logging framework (the csalt core remains logger-agnostic).

API Reference
=============

ErrorCode enumeration (overview)
--------------------------------

The table below summarizes each :c:enum:`ErrorCode` value, its category, and intended use.
Authoritative comments live in ``error_codes.h``; this overview is for quick reference.

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
   * - ``BAD_ALLOC``
     - ``-2``
     - memory
     - Memory allocation failed (e.g., ``malloc``/allocator returned ``NULL``).
   * - ``REALLOC_FAIL``
     - ``-3``
     - memory
     - Reallocation failed (e.g., ``realloc`` returned ``NULL`` and original buffer retained).
   * - ``OUT_OF_MEMORY``
     - ``-4``
     - memory
     - System out of memory or allocator could not satisfy the request.
   * - ``NULL_POINTER``
     - ``-10``
     - argument
     - A required pointer argument was ``NULL``.
   * - ``OUT_OF_BOUNDS``
     - ``-11``
     - argument
     - Index was outside valid range for the target container/object.
   * - ``SIZE_MISMATCH``
     - ``-12``
     - argument
     - Dimensions/shapes of inputs are incompatible for the requested operation.
   * - ``INVALID_ARG``
     - ``-13``
     - argument
     - A function argument had an invalid value or combination.
   * - ``UNINITIALIZED``
     - ``-14``
     - argument
     - Attempt to access or operate on an uninitialized element/field.
   * - ``DIV_BY_ZERO``
     - ``-20``
     - numeric
     - Division by zero detected during computation.
   * - ``SINGULAR_MATRIX``
     - ``-21``
     - numeric
     - Matrix is singular/non-invertible for the requested operation.
   * - ``NUMERIC_OVERFLOW``
     - ``-22``
     - numeric
     - Numeric overflow/underflow or domain/range error occurred.
   * - ``FILE_OPEN``
     - ``-30``
     - I/O
     - File could not be opened (missing, permissions, etc.).
   * - ``FILE_READ``
     - ``-31``
     - I/O
     - Error while reading from a file/stream.
   * - ``FILE_WRITE``
     - ``-32``
     - I/O
     - Error while writing to a file/stream.
   * - ``TYPE_MISMATCH``
     - ``-40``
     - type/format
     - Incompatible or unexpected type encountered.
   * - ``FORMAT_INVALID``
     - ``-41``
     - type/format
     - Invalid/unsupported data format or encoding.
   * - ``UNKNOWN``
     - ``-99``
     - generic
     - Fallback for unspecified/unknown failures.

Legend
~~~~~~
- **memory**: allocation/reallocation/resource exhaustion  
- **argument**: invalid inputs, nulls, bounds, or uninitialized access  
- **numeric**: arithmetic/domain/singularity issues  
- **I/O**: filesystem/stream operations  
- **type/format**: schema, encoding, or type incompatibilities  
- **meta**: sentinel values not produced by normal operations

See also
~~~~~~~~
- :c:func:`error_to_string` — convert any :c:enum:`ErrorCode` to a short human-readable message (strings defined in code comments).


Enum Types
----------

.. doxygenfile:: error_codes.h
   :project: csalt
   :sections: enums

Functions
---------

The following function converts error codes into textual descriptions:

.. doxygenfunction:: error_to_string
   :project: csalt

