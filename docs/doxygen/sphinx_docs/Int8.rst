.. _int8:

***************
int8_t Overview
***************
The ``int8_t`` family of data structures in csalt provides type-safe,
allocator-agnostic containers for 8-bit unsigned integer data.  Every
structure wraps the generic ``array_t`` engine from ``c_array.h`` with the
element type fixed to ``UINT8_TYPE`` at initialisation, eliminating the need
for the caller to pass a ``dtype_id_t`` argument to any operation.

All memory is managed through an :c:type:`allocator_vtable_t` supplied by
the caller at initialisation time.  csalt does not assume a default
allocator — the caller must explicitly provide one.  The library ships
several ready-made implementations and the interface is open so that custom
allocators can be substituted without changing any call sites.  See
:ref:`allocator_file` for the full allocator API, the list of available
implementations, and guidance on writing your own.

Error handling follows the *expected value* pattern throughout: operations
that produce a new object return a ``*_expect_t`` struct whose ``has_value``
field distinguishes success from failure, while in-place mutations return an
:c:type:`error_code_t` directly.  Callers must always check the result
before using the value.

int8_t Array
=============
A :c:type:`int8_array_t` is an ordered sequence of ``int8_t`` values
backed by a contiguous heap-allocated buffer.  It supports O(1) amortised
append, O(n) insert and remove at arbitrary positions, SIMD-accelerated
reverse and search, and an in-place comparison sort.  It is implemented as
a thin type-safe wrapper around the generic :ref:`array` engine described
in ``c_array.h``.

The array does not have a default allocator — an :c:type:`allocator_vtable_t`
must be supplied at every call site that allocates memory (initialisation,
copy, and slice).  See :ref:`allocator_file` for available allocators and
the trade-offs between them.

.. code-block:: c

   #include "c_int8.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   int8_array_expect_t r = init_uint8_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   int8_array_t* arr = r.u.value;

   push_back_int8_array(arr, 30);
   push_back_int8_array(arr, 10);
   push_back_int8_array(arr, 20);

   sort_int8_array(arr, FORWARD);   /* arr is now [10, 20, 30] */

   return_int8_array(arr);

Structs
-------

.. doxygenstruct:: int8_array_t
   :members:

.. doxygenstruct:: int8_array_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_int8_array

.. doxygenfunction:: return_int8_array

Push Operations
---------------

.. doxygenfunction:: push_back_int8_array

.. doxygenfunction:: push_front_int8_array

.. doxygenfunction:: push_at_int8_array

Get and Set
-----------

.. doxygenfunction:: get_int8_array_index

.. doxygenfunction:: set_int8_array_index

Pop Operations
--------------

.. doxygenfunction:: pop_back_int8_array

.. doxygenfunction:: pop_front_int8_array

.. doxygenfunction:: pop_any_int8_array

Utility Operations
------------------

.. doxygenfunction:: clear_int8_array

.. doxygenfunction:: copy_int8_array

.. doxygenfunction:: concat_int8_array

.. doxygenfunction:: slice_int8_array

Algorithms
----------

.. doxygenfunction:: reverse_int8_array

.. doxygenfunction:: sort_int8_array

Search
------

.. doxygenfunction:: int8_array_contains

.. doxygenfunction:: int8_array_binary_search 

.. doxygenfunction:: int8_array_binary_bracket

Introspection
-------------

.. doxygenfunction:: int8_array_size

.. doxygenfunction:: int8_array_alloc

.. doxygenfunction:: int8_array_data_size

.. doxygenfunction:: is_int8_array_empty

.. doxygenfunction:: is_int8_array_full

.. doxygenfunction:: is_int8_array_ptr

int8_t Dictionary 
=================

int8_t Matrix 
=============
