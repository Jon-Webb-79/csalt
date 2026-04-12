.. _uint16:

*****************
uint16_t Overview
*****************
The ``uint16_t`` family of data structures in csalt provides type-safe,
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

uint16_t Array
==============
A :c:type:`uint16_array_t` is an ordered sequence of ``uint16_t`` values
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

   #include "c_uint16.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   uint16_array_expect_t r = init_uint16_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   uint16_array_t* arr = r.u.value;

   push_back_uint16_array(arr, 30);
   push_back_uint16_array(arr, 10);
   push_back_uint16_array(arr, 20);

   sort_uint16_array(arr, FORWARD);   /* arr is now [10, 20, 30] */

   return_uint16_array(arr);

Structs
-------

.. doxygenstruct:: uint16_array_t
   :members:

.. doxygenstruct:: uint16_array_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_uint16_array

.. doxygenfunction:: return_uint16_array

Push Operations
---------------

.. doxygenfunction:: push_back_uint16_array

.. doxygenfunction:: push_front_uint16_array

.. doxygenfunction:: push_at_uint16_array

Get and Set
-----------

.. doxygenfunction:: get_uint16_array_index

.. doxygenfunction:: set_uint16_array_index

Pop Operations
--------------

.. doxygenfunction:: pop_back_uint16_array

.. doxygenfunction:: pop_front_uint16_array

.. doxygenfunction:: pop_any_uint16_array

Utility Operations
------------------

.. doxygenfunction:: clear_uint16_array

.. doxygenfunction:: copy_uint16_array

.. doxygenfunction:: concat_uint16_array

.. doxygenfunction:: slice_uint16_array

.. doxygenfunction:: print_uint16_array

Algorithms
----------

.. doxygenfunction:: reverse_uint16_array

.. doxygenfunction:: sort_uint16_array

Search
------

.. doxygenfunction:: uint16_array_contains

.. doxygenfunction:: uint16_array_binary_search 

.. doxygenfunction:: uint16_array_binary_bracket

Introspection
-------------

.. doxygenfunction:: uint16_array_size

.. doxygenfunction:: uint16_array_alloc

.. doxygenfunction:: uint16_array_data_size

.. doxygenfunction:: is_uint16_array_empty

.. doxygenfunction:: is_uint16_array_full

.. doxygenfunction:: is_uint16_array_ptr

.. doxygenfunction:: uint16_array_max

.. doxygenfunction:: uint16_array_min

uint16_t Dictionary 
===================
A ``uint16_dict_t`` is a hash dictionary that maps C-string keys to
``uint16_t`` values.  It is a type-safe wrapper around the generic
dict engine described in :ref:`c_dict`, with the value type fixed to
``sizeof(uint16_t)`` and ``dtype`` fixed to ``UINT16_TYPE`` at initialisation.
 
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
:c:type:`allocator_vtable_t` must be supplied to :c:func:`init_uint16_dict`
and to every :c:func:`insert_uint16_dict` call.  All other operations use
the allocator that was stored at initialisation time.  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "uint16_dict.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   uint16_dict_expect_t r = init_uint16_dict(16, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   uint16_dict_t* d = r.u.value;
 
   insert_uint16_dict(d, "width",  1920u, a);
   insert_uint16_dict(d, "height", 1080u, a);
   insert_uint16_dict(d, "depth",    32u, a);
 
   uint16_t v;
   get_uint16_dict_value(d, "width", &v);   /* v == 1920 */
 
   update_uint16_dict(d, "width", 2560u);
 
   pop_uint16_dict(d, "depth", NULL);       /* removes "depth", discards value */
 
   return_uint16_dict(d);
 
Plain vs ``_n`` Variants
------------------------
 
Every function that takes a key is available in two forms.
 
.. list-table::
   :header-rows: 1
   :widths: 45 55
 
   * - Plain variant
     - ``_n`` variant
   * - ``insert_uint16_dict(d, key, value, a)``
     - ``insert_uint16_dict_n(d, key, key_len, value, a)``
   * - ``pop_uint16_dict(d, key, out)``
     - ``pop_uint16_dict_n(d, key, key_len, out)``
   * - ``update_uint16_dict(d, key, value)``
     - ``update_uint16_dict_n(d, key, key_len, value)``
   * - ``get_uint16_dict_value(d, key, out)``
     - ``get_uint16_dict_value_n(d, key, key_len, out)``
   * - ``get_uint16_dict_ptr(d, key)``
     - ``get_uint16_dict_ptr_n(d, key, key_len)``
   * - ``has_uint16_dict_key(d, key)``
     - ``has_uint16_dict_key_n(d, key, key_len)``
 
The ``_n`` variants are particularly useful for splitting on sub-strings
without constructing a null-terminated copy:
 
.. code-block:: c
 
   /* Buffer holds "width_max" but we only want "width" (5 bytes). */
   const char* buf = "width_max";
   insert_uint16_dict_n(d, buf, 5, 1920u, a);
 
   uint16_t v;
   get_uint16_dict_value_n(d, buf, 5, &v);   /* v == 1920 */
   get_uint16_dict_value(d, "width", &v);     /* same key — also v == 1920 */
 
Structs
-------
 
.. note::
 
   ``uint16_dict_t`` is a ``typedef`` alias for :c:struct:`dict_t`.  All
   internal fields are documented under :c:struct:`dict_t` in
   :ref:`c_dict`.  The ``data_size`` field is always ``sizeof(uint16_t)``
   and the ``dtype`` field is always ``UINT16_TYPE``.
 
.. doxygenstruct:: uint16_dict_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_uint16_dict
.. doxygenfunction:: return_uint16_dict
 
Insert
------
 
.. doxygenfunction:: insert_uint16_dict
.. doxygenfunction:: insert_uint16_dict_n
 
Pop
---
 
.. doxygenfunction:: pop_uint16_dict
.. doxygenfunction:: pop_uint16_dict_n
 
Update
------
 
.. doxygenfunction:: update_uint16_dict
.. doxygenfunction:: update_uint16_dict_n
 
Lookup
------
 
.. doxygenfunction:: get_uint16_dict_value
.. doxygenfunction:: get_uint16_dict_value_n
.. doxygenfunction:: get_uint16_dict_ptr
.. doxygenfunction:: get_uint16_dict_ptr_n
.. doxygenfunction:: has_uint16_dict_key
.. doxygenfunction:: has_uint16_dict_key_n
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_uint16_dict
.. doxygenfunction:: copy_uint16_dict
.. doxygenfunction:: merge_uint16_dict
.. doxygenfunction:: print_uint16_dict
 
Iteration
---------
 
:c:func:`foreach_uint16_dict` visits every entry in bucket order (which is
not guaranteed to match insertion order).  The callback receives the key as
a null-terminated ``const char*`` pointer into the dict's internal storage,
the key length, the ``uint16_t`` value, and an optional caller-supplied
context pointer.  The callback must not insert or remove entries during
traversal.
 
.. code-block:: c
 
   static void print_entry(const char* key, size_t key_len,
                            uint16_t value, void* ud) {
       (void)key_len; (void)ud;
       printf("  %s = %u\n", key, (unsigned)value);
   }
 
   foreach_uint16_dict(d, print_entry, NULL);
 
.. doxygenfunction:: foreach_uint16_dict
 
Introspection
-------------
 
.. doxygenfunction:: uint16_dict_size
.. doxygenfunction:: uint16_dict_hash_size
.. doxygenfunction:: uint16_dict_alloc
.. doxygenfunction:: is_uint16_dict_empty

uint16_t Matrix 
===============
A ``uint16_matrix_t`` is a type-safe wrapper around the generic
:c:type:`matrix_t` container with the runtime ``dtype`` fixed to
``UINT16_TYPE``.  It provides uint16-specific construction, element access,
conversion, comparison, and helper functions while reusing the generic
matrix engine implemented in ``c_matrix.h`` and ``c_matrix.c``.

The wrapper exists for APIs that either:

* fix the ``dtype`` to ``UINT16_TYPE``
* replace ``void*`` element access with ``uint16_t`` / ``uint16_t*``
* provide uint16-specialised helper behaviour such as dense SIMD fast paths

All memory is managed through a caller-supplied
:c:type:`allocator_vtable_t`.  The library does not assume a default
allocator.

Storage Formats
---------------

``uint16_matrix_t`` supports the same storage formats as
:c:type:`matrix_t`:

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Format
     - Description
   * - ``DENSE_MATRIX``
     - Row-major contiguous uint16 buffer.
   * - ``COO_MATRIX``
     - Coordinate list with parallel ``row_idx``, ``col_idx``, and
       ``values`` arrays.  Intended for sparse matrix assembly.
   * - ``CSR_MATRIX``
     - Compressed Sparse Row format for sparse row-oriented operations.
   * - ``CSC_MATRIX``
     - Compressed Sparse Column format for sparse column-oriented operations.

.. note::

   ``set_uint16_matrix`` supports dense and COO matrices.  CSR and CSC are
   treated as derived sparse formats and are not directly mutable through
   element-wise set operations.

uint16 Comparison and Zero Semantics
-----------------------------------

The generic matrix layer supports semantic comparison and semantic zero
testing through callback-style predicates such as
:c:type:`matrix_equal_fn` and :c:type:`matrix_zero_fn`.  These are
documented in ``c_matrix.h`` and are the correct place to describe
custom comparison behaviour. :contentReference[oaicite:7]{index=7}

For the uint16 wrapper specifically:

* ``uint16_matrix_equal`` performs exact logical equality and uses a
  dense SIMD fast path when both inputs are dense uint16 matrices.
* Sparse and mixed-format comparisons fall back to generic element-wise
  comparison through the generic matrix engine.
* ``uint16_matrix_is_zero`` uses a dense SIMD fast path for dense uint16
  matrices and falls back to the generic zero test for sparse formats.

SIMD Fast Paths
---------------

Several dense uint16 matrix operations use SIMD-specialised implementations
when supported by the target platform.  Mixed-format and sparse cases
fall back to the generic matrix implementation.

Current dense-uint16 SIMD fast paths include:

* ``convert_uint16_matrix`` for dense-to-CSR conversion
* ``transpose_uint16_matrix`` for dense transpose
* ``fill_uint16_matrix`` for dense fill
* ``uint16_matrix_is_zero`` for dense zero detection
* ``uint16_matrix_equal`` for dense equality checks 

Example Usage
-------------

.. code-block:: c

   #include "c_uint16.h"

   allocator_vtable_t a = heap_allocator();

   uint16_matrix_expect_t r = init_uint16_dense_matrix(3, 4, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   uint16_matrix_t* m = r.u.value;

   set_uint16_matrix(m, 0, 0, 1u);
   set_uint16_matrix(m, 1, 2, 5u);
   set_uint16_matrix(m, 2, 3, 3u);

   uint16_t out = 0u;
   get_uint16_matrix(m, 1, 2, &out);

   uint16_matrix_expect_t tr = transpose_uint16_matrix(m, a);
   uint16_matrix_expect_t csr = convert_uint16_matrix(m, CSR_MATRIX, a);

   return_uint16_matrix(csr.u.value);
   return_uint16_matrix(tr.u.value);
   return_uint16_matrix(m);

.. code-block:: c

   uint16_matrix_expect_t r = init_uint16_coo_matrix(100, 100, 16, true, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   uint16_matrix_t* sp = r.u.value;

   push_back_uint16_coo_matrix(sp, 0, 5, 3u);
   push_back_uint16_coo_matrix(sp, 42, 99, 1u);
   sort_uint16_coo_matrix(sp);

   uint16_t v = 0u;
   get_uint16_matrix(sp, 10, 10, &v);   /* implicit zero */

   return_uint16_matrix(sp);

Structs
-------

.. note::

   ``uint16_matrix_t`` is a ``typedef`` alias for :c:struct:`matrix_t`.
   The underlying generic storage layout is documented in :ref:`matrix`.

.. doxygenstruct:: uint16_matrix_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_uint16_dense_matrix
.. doxygenfunction:: init_uint16_coo_matrix
.. doxygenfunction:: return_uint16_matrix

Element Access
--------------

.. doxygenfunction:: get_uint16_matrix
.. doxygenfunction:: set_uint16_matrix

COO Assembly
------------

These functions are only valid for COO-format matrices.

.. doxygenfunction:: reserve_uint16_coo_matrix
.. doxygenfunction:: push_back_uint16_coo_matrix
.. doxygenfunction:: sort_uint16_coo_matrix

Lifecycle / Structural Operations
---------------------------------

.. doxygenfunction:: clear_uint16_matrix
.. doxygenfunction:: copy_uint16_matrix
.. doxygenfunction:: convert_uint16_matrix
.. doxygenfunction:: convert_uint16_matrix_zero
.. doxygenfunction:: transpose_uint16_matrix

Fill
----

.. doxygenfunction:: fill_uint16_matrix

Shape and Compatibility
-----------------------

.. doxygenfunction:: uint16_matrix_has_same_shape
.. doxygenfunction:: uint16_matrix_is_square
.. doxygenfunction:: uint16_matrix_is_sparse
.. doxygenfunction:: uint16_matrix_is_zero
.. doxygenfunction:: uint16_matrix_equal
.. doxygenfunction:: uint16_matrix_is_add_compatible
.. doxygenfunction:: uint16_matrix_is_multiply_compatible

Row and Column Swaps
--------------------

.. doxygenfunction:: swap_uint16_matrix_rows
.. doxygenfunction:: swap_uint16_matrix_cols

Special Constructors
--------------------

.. doxygenfunction:: init_uint16_identity_matrix
.. doxygenfunction:: init_uint16_row_vector
.. doxygenfunction:: init_uint16_col_vector

Vector Queries
--------------

.. doxygenfunction:: uint16_matrix_is_row_vector
.. doxygenfunction:: uint16_matrix_is_col_vector
.. doxygenfunction:: uint16_matrix_is_vector
.. doxygenfunction:: uint16_matrix_vector_length

Print 
-----

.. doxygenfunction:: print_uint16_matrix

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
   uint16-specific APIs in the public documentation.
