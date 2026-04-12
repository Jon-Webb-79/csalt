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

.. doxygenfunction:: print_int8_array

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

.. doxygenfunction:: int8_array_max

.. doxygenfunction:: int8_array_min

int8_t Dictionary 
=================
An ``int8_dict_t`` is a hash dictionary that maps C-string keys to
``int8_t`` values.  It is a type-safe wrapper around the generic
dict engine described in :ref:`c_dict`, with the value type fixed to
``sizeof(int8_t)`` and ``dtype`` fixed to ``INT8_TYPE`` at initialisation.
 
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
:c:type:`allocator_vtable_t` must be supplied to :c:func:`init_int8_dict`
and to every :c:func:`insert_int8_dict` call.  All other operations use
the allocator that was stored at initialisation time.  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "int8_dict.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   int8_dict_expect_t r = init_int8_dict(16, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   int8_dict_t* d = r.u.value;
 
   insert_int8_dict(d, "offset", -10, a);
   insert_int8_dict(d, "bias",     5, a);
   insert_int8_dict(d, "zero",     0, a);
 
   int8_t v;
   get_int8_dict_value(d, "offset", &v);   /* v == -10 */
 
   update_int8_dict(d, "offset", 10);
 
   pop_int8_dict(d, "zero", NULL);         /* removes "zero", discards value */
 
   return_int8_dict(d);
 
Plain vs ``_n`` Variants
------------------------
 
Every function that takes a key is available in two forms.
 
.. list-table::
   :header-rows: 1
   :widths: 45 55
 
   * - Plain variant
     - ``_n`` variant
   * - ``insert_int8_dict(d, key, value, a)``
     - ``insert_int8_dict_n(d, key, key_len, value, a)``
   * - ``pop_int8_dict(d, key, out)``
     - ``pop_int8_dict_n(d, key, key_len, out)``
   * - ``update_int8_dict(d, key, value)``
     - ``update_int8_dict_n(d, key, key_len, value)``
   * - ``get_int8_dict_value(d, key, out)``
     - ``get_int8_dict_value_n(d, key, key_len, out)``
   * - ``get_int8_dict_ptr(d, key)``
     - ``get_int8_dict_ptr_n(d, key, key_len)``
   * - ``has_int8_dict_key(d, key)``
     - ``has_int8_dict_key_n(d, key, key_len)``
 
The ``_n`` variants are particularly useful for splitting on sub-strings
without constructing a null-terminated copy:
 
.. code-block:: c
 
   /* Buffer holds "delta_fine" but we only want "delta" (5 bytes). */
   const char* buf = "delta_fine";
   insert_int8_dict_n(d, buf, 5, -3, a);
 
   int8_t v;
   get_int8_dict_value_n(d, buf, 5, &v);   /* v == -3 */
   get_int8_dict_value(d, "delta", &v);     /* same key — also v == -3 */
 
Structs
-------
 
.. note::
 
   ``int8_dict_t`` is a ``typedef`` alias for :c:struct:`dict_t`.  All
   internal fields are documented under :c:struct:`dict_t` in
   :ref:`c_dict`.  The ``data_size`` field is always ``sizeof(int8_t)``
   and the ``dtype`` field is always ``INT8_TYPE``.
 
.. doxygenstruct:: int8_dict_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_int8_dict
.. doxygenfunction:: return_int8_dict
 
Insert
------
 
.. doxygenfunction:: insert_int8_dict
.. doxygenfunction:: insert_int8_dict_n
 
Pop
---
 
.. doxygenfunction:: pop_int8_dict
.. doxygenfunction:: pop_int8_dict_n
 
Update
------
 
.. doxygenfunction:: update_int8_dict
.. doxygenfunction:: update_int8_dict_n
 
Lookup
------
 
.. doxygenfunction:: get_int8_dict_value
.. doxygenfunction:: get_int8_dict_value_n
.. doxygenfunction:: get_int8_dict_ptr
.. doxygenfunction:: get_int8_dict_ptr_n
.. doxygenfunction:: has_int8_dict_key
.. doxygenfunction:: has_int8_dict_key_n
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_int8_dict
.. doxygenfunction:: copy_int8_dict
.. doxygenfunction:: merge_int8_dict
.. doxygenfunction:: print_int8_dict
 
Iteration
---------
 
:c:func:`foreach_int8_dict` visits every entry in bucket order (which is
not guaranteed to match insertion order).  The callback receives the key as
a null-terminated ``const char*`` pointer into the dict's internal storage,
the key length, the ``int8_t`` value, and an optional caller-supplied
context pointer.  The callback must not insert or remove entries during
traversal.
 
.. code-block:: c
 
   static void print_entry(const char* key, size_t key_len,
                            int8_t value, void* ud) {
       (void)key_len; (void)ud;
       printf("  %s = %d\n", key, (int)value);
   }
 
   foreach_int8_dict(d, print_entry, NULL);
 
.. doxygenfunction:: foreach_int8_dict
 
Introspection
-------------
 
.. doxygenfunction:: int8_dict_size
.. doxygenfunction:: int8_dict_hash_size
.. doxygenfunction:: int8_dict_alloc
.. doxygenfunction:: is_int8_dict_empty


int8_t Matrix 
=============
A ``int8_matrix_t`` is a type-safe wrapper around the generic
:c:type:`matrix_t` container with the runtime ``dtype`` fixed to
``INT_TYPE``.  It provides int8-specific construction, element access,
conversion, comparison, and helper functions while reusing the generic
matrix engine implemented in ``c_matrix.h`` and ``c_matrix.c``.

The wrapper exists for APIs that either:

* fix the ``dtype`` to ``INT8_TYPE``
* replace ``void*`` element access with ``int8_t`` / ``int8_t*``
* provide int8-specialised helper behaviour such as dense SIMD fast paths

All memory is managed through a caller-supplied
:c:type:`allocator_vtable_t`.  The library does not assume a default
allocator.

Storage Formats
---------------

``int8_matrix_t`` supports the same storage formats as
:c:type:`matrix_t`:

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Format
     - Description
   * - ``DENSE_MATRIX``
     - Row-major contiguous int8 buffer.
   * - ``COO_MATRIX``
     - Coordinate list with parallel ``row_idx``, ``col_idx``, and
       ``values`` arrays.  Intended for sparse matrix assembly.
   * - ``CSR_MATRIX``
     - Compressed Sparse Row format for sparse row-oriented operations.
   * - ``CSC_MATRIX``
     - Compressed Sparse Column format for sparse column-oriented operations.

.. note::

   ``set_int8_matrix`` supports dense and COO matrices.  CSR and CSC are
   treated as derived sparse formats and are not directly mutable through
   element-wise set operations.

int8 Comparison and Zero Semantics
-----------------------------------

The generic matrix layer supports semantic comparison and semantic zero
testing through callback-style predicates such as
:c:type:`matrix_equal_fn` and :c:type:`matrix_zero_fn`.  These are
documented in ``c_matrix.h`` and are the correct place to describe
custom comparison behaviour. :contentReference[oaicite:7]{index=7}

For the int8 wrapper specifically:

* ``int8_matrix_equal`` performs exact logical equality and uses a
  dense SIMD fast path when both inputs are dense int8 matrices.
* Sparse and mixed-format comparisons fall back to generic element-wise
  comparison through the generic matrix engine.
* ``int8_matrix_is_zero`` uses a dense SIMD fast path for dense int8
  matrices and falls back to the generic zero test for sparse formats.

SIMD Fast Paths
---------------

Several dense int8 matrix operations use SIMD-specialised implementations
when supported by the target platform.  Mixed-format and sparse cases
fall back to the generic matrix implementation.

Current dense-int8 SIMD fast paths include:

* ``convert_int8_matrix`` for dense-to-CSR conversion
* ``transpose_int8_matrix`` for dense transpose
* ``fill_int8_matrix`` for dense fill
* ``int8_matrix_is_zero`` for dense zero detection
* ``int8_matrix_equal`` for dense equality checks 

Example Usage
-------------

.. code-block:: c

   #include "c_int8.h"

   allocator_vtable_t a = heap_allocator();

   int8_matrix_expect_t r = init_int8_dense_matrix(3, 4, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   int8_matrix_t* m = r.u.value;

   set_int8_matrix(m, 0, 0, 1);
   set_int8_matrix(m, 1, 2, 5);
   set_int8_matrix(m, 2, 3, 3);

   int8_t out = 0u;
   get_int8_matrix(m, 1, 2, &out);

   int8_matrix_expect_t tr = transpose_int8_matrix(m, a);
   int8_matrix_expect_t csr = convert_int8_matrix(m, CSR_MATRIX, a);

   return_int8_matrix(csr.u.value);
   return_int8_matrix(tr.u.value);
   return_int8_matrix(m);

.. code-block:: c

   int8_matrix_expect_t r = init_int8_coo_matrix(100, 100, 16, true, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   int8_matrix_t* sp = r.u.value;

   push_back_int8_coo_matrix(sp, 0, 5, 3);
   push_back_int8_coo_matrix(sp, 42, 99, 1);
   sort_int8_coo_matrix(sp);

   int8_t v = 0u;
   get_int8_matrix(sp, 10, 10, &v);   /* implicit zero */

   return_int8_matrix(sp);

Structs
-------

.. note::

   ``int8_matrix_t`` is a ``typedef`` alias for :c:struct:`matrix_t`.
   The underlying generic storage layout is documented in :ref:`matrix`.

.. doxygenstruct:: int8_matrix_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_int8_dense_matrix
.. doxygenfunction:: init_int8_coo_matrix
.. doxygenfunction:: return_int8_matrix

Element Access
--------------

.. doxygenfunction:: get_int8_matrix
.. doxygenfunction:: set_int8_matrix

COO Assembly
------------

These functions are only valid for COO-format matrices.

.. doxygenfunction:: reserve_int8_coo_matrix
.. doxygenfunction:: push_back_int8_coo_matrix
.. doxygenfunction:: sort_int8_coo_matrix

Lifecycle / Structural Operations
---------------------------------

.. doxygenfunction:: clear_int8_matrix
.. doxygenfunction:: copy_int8_matrix
.. doxygenfunction:: convert_int8_matrix
.. doxygenfunction:: convert_int8_matrix_zero
.. doxygenfunction:: transpose_int8_matrix

Fill
----

.. doxygenfunction:: fill_int8_matrix

Shape and Compatibility
-----------------------

.. doxygenfunction:: int8_matrix_has_same_shape
.. doxygenfunction:: int8_matrix_is_square
.. doxygenfunction:: int8_matrix_is_sparse
.. doxygenfunction:: int8_matrix_is_zero
.. doxygenfunction:: int8_matrix_equal
.. doxygenfunction:: int8_matrix_is_add_compatible
.. doxygenfunction:: int8_matrix_is_multiply_compatible

Row and Column Swaps
--------------------

.. doxygenfunction:: swap_int8_matrix_rows
.. doxygenfunction:: swap_int8_matrix_cols

Special Constructors
--------------------

.. doxygenfunction:: init_int8_identity_matrix
.. doxygenfunction:: init_int8_row_vector
.. doxygenfunction:: init_int8_col_vector

Vector Queries
--------------

.. doxygenfunction:: int8_matrix_is_row_vector
.. doxygenfunction:: int8_matrix_is_col_vector
.. doxygenfunction:: int8_matrix_is_vector
.. doxygenfunction:: int8_matrix_vector_length

Print 
-----

.. doxygenfunction:: print_int8_matrix

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
   int8-specific APIs in the public documentation.
