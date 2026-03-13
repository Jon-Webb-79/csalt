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

str_array
=========
 
A :c:type:`str_array_t` is an ordered sequence of owned ``string_t*``
pointers backed by a contiguous heap-allocated buffer of pointer-sized
slots.  It supports O(1) amortised append, O(n) insert and remove at
arbitrary positions, in-place lexicographic sort, and both linear and binary
search.  It is implemented as a thin type-safe wrapper around the generic
:ref:`array` engine described in ``c_array.h``.
 
The array does not have a default allocator — an
:c:type:`allocator_vtable_t` must be supplied at every call site that
allocates memory (initialisation, copy, and tokenisation).  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "str_array.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   str_array_expect_t r = init_str_array(8, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   str_array_t* arr = r.u.value;
 
   push_back_lit(arr, "banana");
   push_back_lit(arr, "apple");
   push_back_lit(arr, "cherry");
 
   sort_str_array(arr, FORWARD);   /* arr is now ["apple", "banana", "cherry"] */
 
   string_expect_t e = get_str_array_index(arr, 0);
   if (e.has_value) {
       printf("%s\n", const_string(e.u.value));   /* "apple" */
       return_string(e.u.value);
   }
 
   return_str_array(arr);
 
Ownership Rules
---------------
 
+---------------------------------------+-----------------------------------------------+
| Operation                             | Ownership outcome                             |
+=======================================+===============================================+
| ``push_back_str`` /                   | Deep-copies *src* via ``copy_string``.        |
| ``push_front_str`` /                  | The caller retains ownership of the           |
| ``push_at_str``                       | original ``string_t*``.                       |
+---------------------------------------+-----------------------------------------------+
| ``push_back_lit`` /                   | Allocates a new ``string_t*`` from the        |
| ``push_front_lit`` /                  | C-string via ``init_string``.                 |
| ``push_at_lit``                       |                                               |
+---------------------------------------+-----------------------------------------------+
| ``get_str_array_index``               | Returns an **owned** deep copy.  Caller must  |
|                                       | call ``return_string`` on the result.         |
+---------------------------------------+-----------------------------------------------+
| ``get_str_array_ptr``                 | Returns a **borrowed** pointer into internal  |
|                                       | storage.  Do **not** call ``return_string``   |
|                                       | on the result.  Invalidated by any mutation.  |
+---------------------------------------+-----------------------------------------------+
| ``pop_back_str_array`` /              | Removes the element and calls                 |
| ``pop_front_str_array`` /             | ``return_string`` internally.  Call           |
| ``pop_any_str_array``                 | ``get_str_array_index`` first if the value    |
|                                       | is needed.                                    |
+---------------------------------------+-----------------------------------------------+
| ``set_str_array_index_str`` /         | Releases the displaced pointer via            |
| ``set_str_array_index_lit``           | ``return_string`` before writing the new one. |
+---------------------------------------+-----------------------------------------------+
| ``return_str_array``                  | Calls ``return_string`` on every live         |
|                                       | pointer, then frees the buffer and struct.    |
+---------------------------------------+-----------------------------------------------+
 
Structs
-------
 
.. doxygenstruct:: str_array_t
   :members:
 
.. doxygenstruct:: str_array_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_str_array
.. doxygenfunction:: return_str_array
 
Push Operations — string_t* source
------------------------------------
 
These functions accept an existing ``string_t*`` and store a deep copy of it.
The caller retains ownership of the original.
 
.. doxygenfunction:: push_back_str
.. doxygenfunction:: push_front_str
.. doxygenfunction:: push_at_str
 
Push Operations — C-string literal source
------------------------------------------
 
These functions accept a null-terminated ``const char*`` and allocate a new
``string_t*`` via ``init_string`` before storing it.
 
.. doxygenfunction:: push_back_lit
.. doxygenfunction:: push_front_lit
.. doxygenfunction:: push_at_lit
 
Push Convenience Macros
-----------------------
 
When ``ARENA_USE_CONVENIENCE_MACROS`` is defined and ``NO_FUNCTION_MACROS``
is **not** defined, three type-generic macros are available that dispatch to
the correct ``_str`` or ``_lit`` variant at compile time based on the type
of the source argument.  Any other source type produces a compile-time error.
 
.. list-table::
   :header-rows: 1
   :widths: 30 25 45
 
   * - Macro
     - ``const char*`` / ``char*`` dispatch
     - ``const string_t*`` / ``string_t*`` dispatch
   * - ``push_back_string(arr, src)``
     - ``push_back_lit(arr, src)``
     - ``push_back_str(arr, src)``
   * - ``push_front_string(arr, src)``
     - ``push_front_lit(arr, src)``
     - ``push_front_str(arr, src)``
   * - ``push_at_string(arr, index, src)``
     - ``push_at_lit(arr, index, src)``
     - ``push_at_str(arr, index, src)``
 
.. code-block:: c
 
   #define ARENA_USE_CONVENIENCE_MACROS
   #include "str_array.h"
 
   allocator_vtable_t a = heap_allocator();
   str_array_expect_t r = init_str_array(4, true, a);
   str_array_t* arr = r.u.value;
 
   push_back_string(arr, "hello");          /* dispatches to push_back_lit  */
 
   string_expect_t rs = init_string("world", 0u, a);
   push_back_string(arr, rs.u.value);       /* dispatches to push_back_str  */
   return_string(rs.u.value);
 
   return_str_array(arr);
 
Get Operations
--------------
 
.. doxygenfunction:: get_str_array_index
.. doxygenfunction:: get_str_array_ptr
 
.. note::
 
   :c:func:`get_str_array_index` returns an **owned** deep copy — the caller
   must call ``return_string`` on the result when finished.
 
   :c:func:`get_str_array_ptr` returns a **borrowed** pointer directly into
   the array's backing storage.  It is valid only until the next mutation of
   the array.  Do **not** call ``return_string`` on the result.
 
Pop Operations
--------------
 
All pop functions remove the target element and call ``return_string`` on it
internally.  The value is **not** returned to the caller.  If the value is
needed, call :c:func:`get_str_array_index` before popping.
 
.. doxygenfunction:: pop_back_str_array
.. doxygenfunction:: pop_front_str_array
.. doxygenfunction:: pop_any_str_array
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_str_array
.. doxygenfunction:: copy_str_array
.. doxygenfunction:: set_str_array_index_str
.. doxygenfunction:: set_str_array_index_lit
.. doxygenfunction:: reverse_str_array
 
Algorithms
----------
 
.. doxygenfunction:: sort_str_array
 
Search
------
 
.. doxygenfunction:: str_array_contains_str
.. doxygenfunction:: str_array_contains_lit
.. doxygenfunction:: str_array_binary_search_str
.. doxygenfunction:: str_array_binary_search_lit
 
Tokenisation
------------
 
The tokenisation functions split a :c:type:`string_t` on a **character set**:
every byte in the delimiter argument is treated as an independent
single-character separator.  Each segment between consecutive separator
occurrences — including empty segments produced by consecutive separators or
by a leading or trailing separator — is deep-copied into a new ``string_t``
and appended to the returned array.
 
An optional ``[begin, end)`` window restricts the scan to a sub-range of the
source string.  Pass ``NULL`` for either pointer to use the natural start or
end of the string.
 
.. code-block:: c
 
   /* Example — split on comma or semicolon */
   allocator_vtable_t a = heap_allocator();
   string_expect_t rs = init_string("one,two;three", 0u, a);
 
   str_array_expect_t r = string_delim_array_lit(rs.u.value, ",;", NULL, NULL, a);
   /* r.u.value contains ["one", "two", "three"] */
 
   /* Windowed example — inspect only bytes [4, 11): "two;thr" */
   const uint8_t* begin = (const uint8_t*)rs.u.value->str + 4u;
   const uint8_t* end   = (const uint8_t*)rs.u.value->str + 11u;
   str_array_expect_t r2 = string_delim_array_lit(rs.u.value, ";", begin, end, a);
   /* r2.u.value contains ["two", "thr"] */
 
   return_str_array(r2.u.value);
   return_str_array(r.u.value);
   return_string(rs.u.value);
 
.. doxygenfunction:: string_delim_array_lit
.. doxygenfunction:: string_delim_array_str
 
Tokenisation Convenience Macro
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
When ``ARENA_USE_CONVENIENCE_MACROS`` is defined and ``NO_FUNCTION_MACROS``
is **not** defined, a type-generic macro dispatches to the correct variant at
compile time based on the type of the ``delim_set`` argument.  Any other
delimiter type produces a compile-time error.
 
.. list-table::
   :header-rows: 1
   :widths: 35 30 35
 
   * - Macro
     - ``const char*`` / ``char*`` dispatch
     - ``const string_t*`` / ``string_t*`` dispatch
   * - ``string_delim_array(s, delim_set, begin, end, alloc_v)``
     - ``string_delim_array_lit(...)``
     - ``string_delim_array_str(...)``
 
.. code-block:: c
 
   #define ARENA_USE_CONVENIENCE_MACROS
   #include "str_array.h"
 
   allocator_vtable_t a = heap_allocator();
   string_expect_t rs = init_string("a:b-c", 0u, a);
 
   /* C-string delimiter — dispatches to string_delim_array_lit */
   str_array_expect_t r1 = string_delim_array(rs.u.value, ":-", NULL, NULL, a);
 
   /* string_t delimiter — dispatches to string_delim_array_str */
   string_expect_t rds = init_string(":-", 0u, a);
   str_array_expect_t r2 = string_delim_array(rs.u.value, rds.u.value, NULL, NULL, a);
 
   return_str_array(r2.u.value);
   return_string(rds.u.value);
   return_str_array(r1.u.value);
   return_string(rs.u.value);
 
Introspection
-------------
 
.. doxygenfunction:: str_array_size
.. doxygenfunction:: str_array_alloc
.. doxygenfunction:: str_array_data_size
.. doxygenfunction:: is_str_array_empty
.. doxygenfunction:: is_str_array_full
