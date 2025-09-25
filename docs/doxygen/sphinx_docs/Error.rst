.. _error_code_file:

*******************
Error Code Overview
*******************

Purpose
=======

The ``error_codes.h`` interface defines a common error model for the **csalt** library.
Every public data structure in csalt exposes a field of type ``ErrorCode`` and every
public function that can fail sets this error field on the output object it modifies.
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

