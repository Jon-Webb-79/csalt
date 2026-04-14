.. _uint8:

****************
uint8_t Overview
****************
The ``uint8_t`` family of data structures in csalt provides type-safe,
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

uint8_t Array
=============
A :c:type:`uint8_array_t` is an ordered sequence of ``uint8_t`` values
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

   #include "c_uint8.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   uint8_array_expect_t r = init_uint8_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   uint8_array_t* arr = r.u.value;

   push_back_uint8_array(arr, 30);
   push_back_uint8_array(arr, 10);
   push_back_uint8_array(arr, 20);

   sort_uint8_array(arr, FORWARD);   /* arr is now [10, 20, 30] */

   return_uint8_array(arr);

Structs
-------

.. doxygenstruct:: uint8_array_t
   :members:

.. doxygenstruct:: uint8_array_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_uint8_array

.. doxygenfunction:: return_uint8_array

Push Operations
---------------

.. doxygenfunction:: push_back_uint8_array

.. doxygenfunction:: push_front_uint8_array

.. doxygenfunction:: push_at_uint8_array

Get and Set
-----------

.. doxygenfunction:: get_uint8_array_index

.. doxygenfunction:: set_uint8_array_index

Pop Operations
--------------

.. doxygenfunction:: pop_back_uint8_array

.. doxygenfunction:: pop_front_uint8_array

.. doxygenfunction:: pop_any_uint8_array

Utility Operations
------------------

.. doxygenfunction:: clear_uint8_array

.. doxygenfunction:: copy_uint8_array

.. doxygenfunction:: concat_uint8_array

.. doxygenfunction:: slice_uint8_array

.. doxygenfunction:: print_uint8_array

.. doxygenfunction:: uint8_array_equal

Algorithms
----------

.. doxygenfunction:: reverse_uint8_array

.. doxygenfunction:: sort_uint8_array

Search
------

.. doxygenfunction:: uint8_array_contains

.. doxygenfunction:: uint8_array_binary_search 

.. doxygenfunction:: uint8_array_binary_bracket

Introspection
-------------

.. doxygenfunction:: uint8_array_size

.. doxygenfunction:: uint8_array_alloc

.. doxygenfunction:: uint8_array_data_size

.. doxygenfunction:: is_uint8_array_empty

.. doxygenfunction:: is_uint8_array_full

.. doxygenfunction:: is_uint8_array_ptr

.. doxygenfunction:: uint8_array_max 

.. doxygenfunction:: uint8_array_min

uint8_t Dictionary 
==================

A ``uint8_dict_t`` is a hash dictionary that maps C-string keys to
``uint8_t`` values.  It is a type-safe wrapper around the generic
dict engine described in :ref:`c_dict`, with the value type fixed to
``sizeof(uint8_t)`` and ``dtype`` fixed to ``UINT8_TYPE`` at initialisation.
 
Keys are null-terminated C-strings.  Every function is available in two
forms: a plain variant that measures the key length with ``strlen``, and an
``_n`` variant that accepts an explicit ``size_t key_len`` argument.  The
``_n`` variants are useful when the key is a sub-string of a larger buffer,
when its length is already known and the ``strlen`` scan is unnecessary, or
when the key contains embedded null bytes.
 
The dict does not store a pointer to the caller's key — it copies the key
bytes into its own allocator-managed storage on every insert.  The caller
may free or reuse the key memory immediately after any dict call returns.
 
The dict does not have a default allocator.  An
:c:type:`allocator_vtable_t` must be supplied to :c:func:`init_uint8_dict`
and to every :c:func:`insert_uint8_dict` call.  All other operations use
the allocator that was stored at initialisation time.  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "uint8_dict.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   uint8_dict_expect_t r = init_uint8_dict(16, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   uint8_dict_t* d = r.u.value;
 
   insert_uint8_dict(d, "red",   255u, a);
   insert_uint8_dict(d, "green", 128u, a);
   insert_uint8_dict(d, "blue",    0u, a);
 
   uint8_t v;
   get_uint8_dict_value(d, "green", &v);   /* v == 128 */
 
   update_uint8_dict(d, "red", 200u);
 
   pop_uint8_dict(d, "blue", NULL);        /* removes "blue", discards value */
 
   return_uint8_dict(d);
 
Plain vs ``_n`` Variants
------------------------
 
Every function that takes a key is available in two forms.
 
.. list-table::
   :header-rows: 1
   :widths: 45 55
 
   * - Plain variant
     - ``_n`` variant
   * - ``insert_uint8_dict(d, key, value, a)``
     - ``insert_uint8_dict_n(d, key, key_len, value, a)``
   * - ``pop_uint8_dict(d, key, out)``
     - ``pop_uint8_dict_n(d, key, key_len, out)``
   * - ``update_uint8_dict(d, key, value)``
     - ``update_uint8_dict_n(d, key, key_len, value)``
   * - ``get_uint8_dict_value(d, key, out)``
     - ``get_uint8_dict_value_n(d, key, key_len, out)``
   * - ``get_uint8_dict_ptr(d, key)``
     - ``get_uint8_dict_ptr_n(d, key, key_len)``
   * - ``has_uint8_dict_key(d, key)``
     - ``has_uint8_dict_key_n(d, key, key_len)``
 
The ``_n`` variants are particularly useful for splitting on sub-strings
without constructing a null-terminated copy:
 
.. code-block:: c
 
   /* Buffer holds "brightness_max" but we only want "brightness" (10 bytes). */
   const char* buf = "brightness_max";
   insert_uint8_dict_n(d, buf, 10, 200u, a);
 
   uint8_t v;
   get_uint8_dict_value_n(d, buf, 10, &v);   /* v == 200 */
   get_uint8_dict_value(d, "brightness", &v); /* same key — also v == 200 */
 
Structs
-------
 
.. note::
 
   ``uint8_dict_t`` is a ``typedef`` alias for :c:struct:`dict_t`.  All
   internal fields are documented under :c:struct:`dict_t` in
   :ref:`c_dict`.  The ``data_size`` field is always ``sizeof(uint8_t)``
   and the ``dtype`` field is always ``UINT8_TYPE``.
 
.. doxygenstruct:: uint8_dict_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_uint8_dict
.. doxygenfunction:: return_uint8_dict
 
Insert
------
 
.. doxygenfunction:: insert_uint8_dict
.. doxygenfunction:: insert_uint8_dict_n
 
Pop
---
 
.. doxygenfunction:: pop_uint8_dict
.. doxygenfunction:: pop_uint8_dict_n
 
Update
------
 
.. doxygenfunction:: update_uint8_dict
.. doxygenfunction:: update_uint8_dict_n
 
Lookup
------
 
.. doxygenfunction:: get_uint8_dict_value
.. doxygenfunction:: get_uint8_dict_value_n
.. doxygenfunction:: get_uint8_dict_ptr
.. doxygenfunction:: get_uint8_dict_ptr_n
.. doxygenfunction:: has_uint8_dict_key
.. doxygenfunction:: has_uint8_dict_key_n
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_uint8_dict
.. doxygenfunction:: copy_uint8_dict
.. doxygenfunction:: merge_uint8_dict
.. doxygenfunction:: print_uint8_dict
 
Iteration
---------
 
:c:func:`foreach_uint8_dict` visits every entry in bucket order (which is
not guaranteed to match insertion order).  The callback receives the key as
a null-terminated ``const char*`` pointer into the dict's internal storage,
the key length, the ``uint8_t`` value, and an optional caller-supplied
context pointer.  The callback must not insert or remove entries during
traversal.
 
.. code-block:: c
 
   static void print_entry(const char* key, size_t key_len,
                            uint8_t value, void* ud) {
       (void)key_len; (void)ud;
       printf("  %s = %u\n", key, (unsigned)value);
   }
 
   foreach_uint8_dict(d, print_entry, NULL);
 
.. doxygenfunction:: foreach_uint8_dict
 
Introspection
-------------
 
.. doxygenfunction:: uint8_dict_size
.. doxygenfunction:: uint8_dict_hash_size
.. doxygenfunction:: uint8_dict_alloc
.. doxygenfunction:: is_uint8_dict_empty

uint8_t Matrix 
==============
A ``uint8_matrix_t`` is a type-safe wrapper around the generic
:c:type:`matrix_t` container with the runtime ``dtype`` fixed to
``UINT8_TYPE``.  It provides uint8-specific construction, element access,
conversion, comparison, and helper functions while reusing the generic
matrix engine implemented in ``c_matrix.h`` and ``c_matrix.c``.

The wrapper exists for APIs that either:

* fix the ``dtype`` to ``UINT8_TYPE``
* replace ``void*`` element access with ``uint8_t`` / ``uint8_t*``
* provide uint8-specialised helper behaviour such as dense SIMD fast paths

All memory is managed through a caller-supplied
:c:type:`allocator_vtable_t`.  The library does not assume a default
allocator.

Storage Formats
---------------

``uint8_matrix_t`` supports the same storage formats as
:c:type:`matrix_t`:

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Format
     - Description
   * - ``DENSE_MATRIX``
     - Row-major contiguous uint8 buffer.
   * - ``COO_MATRIX``
     - Coordinate list with parallel ``row_idx``, ``col_idx``, and
       ``values`` arrays.  Intended for sparse matrix assembly.
   * - ``CSR_MATRIX``
     - Compressed Sparse Row format for sparse row-oriented operations.
   * - ``CSC_MATRIX``
     - Compressed Sparse Column format for sparse column-oriented operations.

.. note::

   ``set_uint8_matrix`` supports dense and COO matrices.  CSR and CSC are
   treated as derived sparse formats and are not directly mutable through
   element-wise set operations.

uint8 Comparison and Zero Semantics
-----------------------------------

The generic matrix layer supports semantic comparison and semantic zero
testing through callback-style predicates such as
:c:type:`matrix_equal_fn` and :c:type:`matrix_zero_fn`.  These are
documented in ``c_matrix.h`` and are the correct place to describe
custom comparison behaviour. :contentReference[oaicite:7]{index=7}

For the uint8 wrapper specifically:

* ``uint8_matrix_equal`` performs exact logical equality and uses a
  dense SIMD fast path when both inputs are dense uint8 matrices.
* Sparse and mixed-format comparisons fall back to generic element-wise
  comparison through the generic matrix engine.
* ``uint8_matrix_is_zero`` uses a dense SIMD fast path for dense uint8
  matrices and falls back to the generic zero test for sparse formats.

SIMD Fast Paths
---------------

Several dense uint8 matrix operations use SIMD-specialised implementations
when supported by the target platform.  Mixed-format and sparse cases
fall back to the generic matrix implementation.

Current dense-uint8 SIMD fast paths include:

* ``convert_uint8_matrix`` for dense-to-CSR conversion
* ``transpose_uint8_matrix`` for dense transpose
* ``fill_uint8_matrix`` for dense fill
* ``uint8_matrix_is_zero`` for dense zero detection
* ``uint8_matrix_equal`` for dense equality checks 

Example Usage
-------------

.. code-block:: c

   #include "c_uint8.h"

   allocator_vtable_t a = heap_allocator();

   uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 4, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   uint8_matrix_t* m = r.u.value;

   set_uint8_matrix(m, 0, 0, 1u);
   set_uint8_matrix(m, 1, 2, 5u);
   set_uint8_matrix(m, 2, 3, 3u);

   uint8_t out = 0u;
   get_uint8_matrix(m, 1, 2, &out);

   uint8_matrix_expect_t tr = transpose_uint8_matrix(m, a);
   uint8_matrix_expect_t csr = convert_uint8_matrix(m, CSR_MATRIX, a);

   return_uint8_matrix(csr.u.value);
   return_uint8_matrix(tr.u.value);
   return_uint8_matrix(m);

.. code-block:: c

   uint8_matrix_expect_t r = init_uint8_coo_matrix(100, 100, 16, true, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   uint8_matrix_t* sp = r.u.value;

   push_back_uint8_coo_matrix(sp, 0, 5, 3u);
   push_back_uint8_coo_matrix(sp, 42, 99, 1u);
   sort_uint8_coo_matrix(sp);

   uint8_t v = 0u;
   get_uint8_matrix(sp, 10, 10, &v);   /* implicit zero */

   return_uint8_matrix(sp);

Structs
-------

.. note::

   ``uint8_matrix_t`` is a ``typedef`` alias for :c:struct:`matrix_t`.
   The underlying generic storage layout is documented in :ref:`matrix`.

.. doxygenstruct:: uint8_matrix_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_uint8_dense_matrix
.. doxygenfunction:: init_uint8_coo_matrix
.. doxygenfunction:: return_uint8_matrix

Element Access
--------------

.. doxygenfunction:: get_uint8_matrix
.. doxygenfunction:: set_uint8_matrix

COO Assembly
------------

These functions are only valid for COO-format matrices.

.. doxygenfunction:: reserve_uint8_coo_matrix
.. doxygenfunction:: push_back_uint8_coo_matrix
.. doxygenfunction:: sort_uint8_coo_matrix

Lifecycle / Structural Operations
---------------------------------

.. doxygenfunction:: clear_uint8_matrix
.. doxygenfunction:: copy_uint8_matrix
.. doxygenfunction:: convert_uint8_matrix
.. doxygenfunction:: convert_uint8_matrix_zero
.. doxygenfunction:: transpose_uint8_matrix

Fill
----

.. doxygenfunction:: fill_uint8_matrix

Shape and Compatibility
-----------------------

.. doxygenfunction:: uint8_matrix_has_same_shape
.. doxygenfunction:: uint8_matrix_is_square
.. doxygenfunction:: uint8_matrix_is_sparse
.. doxygenfunction:: uint8_matrix_is_zero
.. doxygenfunction:: uint8_matrix_equal
.. doxygenfunction:: uint8_matrix_is_add_compatible
.. doxygenfunction:: uint8_matrix_is_multiply_compatible

Row and Column Swaps
--------------------

.. doxygenfunction:: swap_uint8_matrix_rows
.. doxygenfunction:: swap_uint8_matrix_cols

Special Constructors
--------------------

.. doxygenfunction:: init_uint8_identity_matrix
.. doxygenfunction:: init_uint8_row_vector
.. doxygenfunction:: init_uint8_col_vector

Vector Queries
--------------

.. doxygenfunction:: uint8_matrix_is_row_vector
.. doxygenfunction:: uint8_matrix_is_col_vector
.. doxygenfunction:: uint8_matrix_is_vector
.. doxygenfunction:: uint8_matrix_vector_length

Print 
-----

.. doxygenfunction:: print_uint8_matrix


Generic Introspection
---------------------

Matrix introspection is documented in ``c_matrix.h`` rather than repeated
for each typed wrapper.  Users should call the generic matrix functions
directly:

.. doxygenfunction:: matrix_rows
.. doxygenfunction:: matrix_cols
.. doxygenfunction:: matrix_data_size
.. doxygenfunction:: matrix_dtype
.. doxygenfunction:: matrix_format
.. doxygenfunction:: matrix_nnz

.. note::

   These functions are type-agnostic and therefore are not duplicated as
   uint8-specific APIs in the public documentation.
