.. _string_file:

*****************
C String Overview
*****************

String handling in C is traditionally performed using raw character arrays
and functions from ``string.h``. While efficient, this approach requires
manual management of memory allocation, buffer sizing, and null termination,
which can introduce risks such as buffer overflows, memory leaks, and
undefined behavior in large or safety-critical systems.

The *C String module* in this library provides a lightweight, allocator-aware
string container implemented in pure C and declared in ``c_string.h``.
Unlike conventional C strings, this container explicitly tracks:

* The current string length (excluding the null terminator)
* The total allocated buffer size (including space for the terminator)
* The allocator used to create and release the string memory

By integrating directly with the allocator abstraction defined in
``c_allocator.h``, the string container supports heap, arena, pool, slab,
or custom allocation strategies without changing the public API.  
This enables deterministic memory behavior suitable for embedded,
real-time, or safety-regulated environments.

String construction follows a **capacity-driven model**:

* A requested capacity of ``0`` allocates exactly enough memory to store the
  full input string plus the null terminator.
* A non-zero capacity allocates space for the requested number of characters
  **plus one byte for the null terminator**.
* If the requested capacity is smaller than the input string length, the
  stored string is **safely truncated** and always null-terminated.
* If the requested capacity is larger, unused buffer space remains available
  for future operations.

All functions return explicit success/error state using the
``*_expect_t`` pattern defined in ``c_error.h``, avoiding implicit failure
modes common in traditional C string handling.

These characteristics make the C String module appropriate for:

* Deterministic and allocator-controlled memory management
* Embedded or real-time software requiring bounded behavior
* Safety-critical systems emphasizing explicit error handling
* Large applications seeking consistent container abstractions in C

Data Types
==========
The following data structures and derived data types are defined in
``c_string.h`` and implemented in ``c_string.c`` to support the allocator-aware
string container.

string_t
--------
``string_t`` is a **public, non-opaque** data structure that represents a
dynamically managed C-style string.  
Unlike opaque container types used elsewhere in the library, this structure is
intentionally visible so that users may:

* Inspect internal metadata directly when appropriate
* Integrate the container with custom utilities or serialization logic
* Extend behavior through user-defined helper functions or wrappers

The structure stores both the string data and the metadata required for safe
memory management through the allocator abstraction defined in
``c_allocator.h``.

Key properties maintained by ``string_t`` include:

* A pointer to a null-terminated character buffer
* The current logical length of the string (excluding the terminator)
* The total allocated buffer size in bytes (including space for the terminator)
* The allocator instance responsible for allocation and release of memory

These fields allow deterministic control over memory usage while preserving
compatibility with standard C string operations.

.. code-block:: c

   typedef struct {
       char* str;                 // Pointer to null-terminated character buffer
       size_t len;                // Logical string length (excludes '\0')
       size_t alloc;              // Total allocated bytes (includes space for '\0')
       allocator_vtable_t allocator; // Allocator used for memory management
   } string_t;

The following invariants are guaranteed for any valid ``string_t`` instance:

* ``str`` always points to a **null-terminated** character sequence.
* ``alloc >= len + 1`` to ensure space for the terminator.
* Memory ownership and release are handled exclusively through the stored
  allocator.

Because the structure is public, users must preserve these invariants when
manipulating fields directly.  
Violating them may result in undefined behavior or allocator misuse.

string_expect_t
---------------
``string_expect_t`` is a lightweight result container used for explicit error
handling during string construction and operations.  
This follows the ``*_expect_t`` convention defined in ``c_error.h`` and avoids
implicit failure modes such as returning ``NULL`` without context.

.. code-block:: c

   typedef struct {
       bool has_value;
       union {
           string_t* value;
           error_code_t error;
       } u;
   } string_expect_t;

When ``has_value`` is ``true``, the ``value`` field contains a valid
``string_t`` pointer that must eventually be released using
``return_string()``.  
When ``has_value`` is ``false``, the ``error`` field contains the associated
error code describing the failure condition.

String Functions 
================

Creation and Teardown
---------------------

init_string 
~~~~~~~~~~~

.. doxygenfunction:: init_string
   :project: csalt

return_string 
~~~~~~~~~~~~~

.. doxygenfunction:: return_string
   :project: csalt

Utility Functions 
-----------------

get_string_index
~~~~~~~~~~~~~~~~

.. doxygenfunction:: get_string_index
   :project: csalt

const_string 
~~~~~~~~~~~~

.. doxygenfunction:: const_string
   :project: csalt

string_size 
~~~~~~~~~~~

.. doxygenfunction:: string_size
   :project: csalt

string_alloc 
~~~~~~~~~~~~

.. doxygenfunction:: string_alloc
   :project: csalt

str_compare
~~~~~~~~~~~

.. doxygenfunction:: str_compare
   :project: csalt

string_compare
~~~~~~~~~~~~~~

.. doxygenfunction:: string_compare
   :project: csalt

is_string_ptr
~~~~~~~~~~~~~

.. doxygenfunction:: is_string_ptr
   :project: csalt

is_string_ptr_sized
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: is_string_ptr_sized
   :project: csalt

find_substr
~~~~~~~~~~~

.. doxygenfunction:: find_substr
   :project: csalt

find_substr_lit
~~~~~~~~~~~~~~~

.. doxygenfunction:: find_substr_lit
   :project: csalt

String Manipulation
-------------------

str_concat
~~~~~~~~~~

.. doxygenfunction:: str_concat
   :project: csalt

string_concat
~~~~~~~~~~~~~

.. doxygenfunction:: string_concat
   :project: csalt

reset_string
~~~~~~~~~~~~

.. doxygenfunction:: reset_string
   :project: csalt

copy_string
~~~~~~~~~~~

.. doxygenfunction:: copy_string
   :project: csalt

word_count
~~~~~~~~~~

.. doxygenfunction:: word_count 
   :project: csalt

word_count_lit
~~~~~~~~~~~~~~

.. doxygenfunction:: word_count_lit
   :project: csalt

token_count
~~~~~~~~~~~

.. doxygenfunction:: token_count
   :project: csalt

token_count_lit
~~~~~~~~~~~~~~~

.. doxygenfunction:: token_count_lit
   :project: csalt

to_uppercase
~~~~~~~~~~~~

.. doxygenfunction:: to_uppercase
   :project: csalt

to_lowercase
~~~~~~~~~~~~

.. doxygenfunction:: to_lowercase
   :project: csalt

drop_substr
~~~~~~~~~~~

.. doxygenfunction:: drop_substr
   :project: csalt

drop_substr_lit
~~~~~~~~~~~~~~~

.. doxygenfunction:: drop_substr_lit
   :project: csalt

replace_substr
~~~~~~~~~~~~~~

.. doxygenfunction:: replace_substr
   :project: csalt

replace_substr_lit
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: replace_substr_lit
   :project: csalt

pop_str_token_lit
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_str_token_lit
   :project: csalt

pop_str_token
~~~~~~~~~~~~~

.. doxygenfunction:: pop_str_token
   :project: csalt

Generic Macros
--------------

The generic macros described in this section are only available when
``ARENA_USE_CONVENIENCE_MACROS`` is enabled and the code is **not**
compiled with ``NO_FUNCTION_MACROS``.

concat_string
~~~~~~~~~~~~~

.. doxygendefine:: concat_string
   :project: csalt

compare_string
~~~~~~~~~~~~~~

.. doxygendefine:: compare_string
   :project: csalt

count_words
~~~~~~~~~~~

.. doxygendefine:: count_words
   :project: csalt

find_substring
~~~~~~~~~~~~~~

.. doxygendefine:: find_substring
   :project: csalt

count_tokens
~~~~~~~~~~~~

.. doxygendefine:: count_tokens
   :project: csalt

drop_substring
~~~~~~~~~~~~~~

.. doxygendefine:: drop_substring
   :project: csalt

replace_substring
~~~~~~~~~~~~~~~~~

.. doxygendefine:: replace_substring
   :project: csalt

pop_string_token
~~~~~~~~~~~~~~~~

.. doxygendefine:: pop_string_token
   :project: csalt
