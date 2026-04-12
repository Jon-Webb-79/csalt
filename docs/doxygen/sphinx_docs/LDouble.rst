.. _ldouble_type:

********************
long double Overview
********************
The ``long double`` family of data structures in csalt provides type-safe,
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

long double Array
=================
A :c:type:`ldouble_array_t` is an ordered sequence of ``ldouble`` values
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

   #include "c_ldouble.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   ldouble_array_expect_t r = init_ldouble_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   ldouble_array_t* arr = r.u.value;

   push_back_ldouble_array(arr, 30);
   push_back_ldouble_array(arr, 10);
   push_back_ldouble_array(arr, 20);

   sort_ldouble_array(arr, FORWARD);   /* arr is now [10, 20, 30] */

   return_ldouble_array(arr);

Structs
-------

.. doxygenstruct:: ldouble_array_t
   :members:

.. doxygenstruct:: ldouble_array_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_ldouble_array

.. doxygenfunction:: return_ldouble_array

.. doxygenfunction:: cumulative_ldouble_array

Push Operations
---------------

.. doxygenfunction:: push_back_ldouble_array

.. doxygenfunction:: push_front_ldouble_array

.. doxygenfunction:: push_at_ldouble_array

Get and Set
-----------

.. doxygenfunction:: get_ldouble_array_index

.. doxygenfunction:: set_ldouble_array_index

Pop Operations
--------------

.. doxygenfunction:: pop_back_ldouble_array

.. doxygenfunction:: pop_front_ldouble_array

.. doxygenfunction:: pop_any_ldouble_array

Utility Operations
------------------

.. doxygenfunction:: clear_ldouble_array

.. doxygenfunction:: copy_ldouble_array

.. doxygenfunction:: concat_ldouble_array

.. doxygenfunction:: slice_ldouble_array

.. doxygenfunction:: print_ldouble_array

Algorithms
----------

.. doxygenfunction:: reverse_ldouble_array

.. doxygenfunction:: sort_ldouble_array

Search
------

.. doxygenfunction:: ldouble_array_contains

.. doxygenfunction:: ldouble_array_binary_search 

.. doxygenfunction:: ldouble_array_binary_bracket

Introspection
-------------

.. doxygenfunction:: ldouble_array_size

.. doxygenfunction:: ldouble_array_alloc

.. doxygenfunction:: ldouble_array_data_size

.. doxygenfunction:: is_ldouble_array_empty

.. doxygenfunction:: is_ldouble_array_full

.. doxygenfunction:: is_ldouble_array_ptr

.. doxygenfunction:: ldouble_array_max

.. doxygenfunction:: ldouble_array_min

long double Dictionary 
======================
An ``ldouble_dict_t`` is a hash dictionary that maps C-string keys to
``long double`` values.  It is a type-safe wrapper around the generic dict
engine described in :ref:`c_dict`, with the value type fixed to
``sizeof(long double)`` and ``dtype`` fixed to ``LDOUBLE_TYPE`` at
initialisation.
 
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
:c:type:`allocator_vtable_t` must be supplied to :c:func:`init_ldouble_dict`
and to every :c:func:`insert_ldouble_dict` call.  All other operations use
the allocator that was stored at initialisation time.  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "ldouble_dict.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   ldouble_dict_expect_t r = init_ldouble_dict(16, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   ldouble_dict_t* d = r.u.value;
 
   insert_ldouble_dict(d, "pi",      3.14159265358979323846L, a);
   insert_ldouble_dict(d, "gravity", 9.80665L,                a);
   insert_ldouble_dict(d, "zero",    0.0L,                    a);
 
   long double v;
   get_ldouble_dict_value(d, "pi", &v);   /* v == 3.14159265358979323846L */
 
   update_ldouble_dict(d, "zero", -1.0L);
 
   pop_ldouble_dict(d, "gravity", NULL);  /* removes "gravity", discards value */
 
   return_ldouble_dict(d);
 
Plain vs ``_n`` Variants
------------------------
 
Every function that takes a key is available in two forms.
 
.. list-table::
   :header-rows: 1
   :widths: 45 55
 
   * - Plain variant
     - ``_n`` variant
   * - ``insert_ldouble_dict(d, key, value, a)``
     - ``insert_ldouble_dict_n(d, key, key_len, value, a)``
   * - ``pop_ldouble_dict(d, key, out)``
     - ``pop_ldouble_dict_n(d, key, key_len, out)``
   * - ``update_ldouble_dict(d, key, value)``
     - ``update_ldouble_dict_n(d, key, key_len, value)``
   * - ``get_ldouble_dict_value(d, key, out)``
     - ``get_ldouble_dict_value_n(d, key, key_len, out)``
   * - ``get_ldouble_dict_ptr(d, key)``
     - ``get_ldouble_dict_ptr_n(d, key, key_len)``
   * - ``has_ldouble_dict_key(d, key)``
     - ``has_ldouble_dict_key_n(d, key, key_len)``
 
The ``_n`` variants are particularly useful for splitting on sub-strings
without constructing a null-terminated copy:
 
.. code-block:: c
 
   /* Buffer holds "scale_factor" but we only want "scale" (5 bytes). */
   const char* buf = "scale_factor";
   insert_ldouble_dict_n(d, buf, 5, 0.1234567890123456789L, a);
 
   long double v;
   get_ldouble_dict_value_n(d, buf, 5, &v);   /* v == 0.1234567890123456789L */
   get_ldouble_dict_value(d, "scale", &v);     /* same key — v == 0.1234567890123456789L */
 
Structs
-------
 
.. note::
 
   ``ldouble_dict_t`` is a ``typedef`` alias for :c:struct:`dict_t`.  All
   internal fields are documented under :c:struct:`dict_t` in
   :ref:`c_dict`.  The ``data_size`` field is always ``sizeof(long double)``,
   which is platform-dependent: 10 bytes on x87 hardware (80-bit extended
   precision), padded to 12 bytes on 32-bit x86, and padded to 16 bytes on
   most x86-64 ABIs.  The ``dtype`` field is always ``LDOUBLE_TYPE``.
 
.. doxygenstruct:: ldouble_dict_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_ldouble_dict
.. doxygenfunction:: return_ldouble_dict
 
Insert
------
 
.. doxygenfunction:: insert_ldouble_dict
.. doxygenfunction:: insert_ldouble_dict_n
 
Pop
---
 
.. doxygenfunction:: pop_ldouble_dict
.. doxygenfunction:: pop_ldouble_dict_n
 
Update
------
 
.. doxygenfunction:: update_ldouble_dict
.. doxygenfunction:: update_ldouble_dict_n
 
Lookup
------
 
.. doxygenfunction:: get_ldouble_dict_value
.. doxygenfunction:: get_ldouble_dict_value_n
.. doxygenfunction:: get_ldouble_dict_ptr
.. doxygenfunction:: get_ldouble_dict_ptr_n
.. doxygenfunction:: has_ldouble_dict_key
.. doxygenfunction:: has_ldouble_dict_key_n
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_ldouble_dict
.. doxygenfunction:: copy_ldouble_dict
.. doxygenfunction:: merge_ldouble_dict
.. doxygenfunction:: print_ldouble_dict
 
Iteration
---------
 
:c:func:`foreach_ldouble_dict` visits every entry in bucket order (which is
not guaranteed to match insertion order).  The callback receives the key as
a null-terminated ``const char*`` pointer into the dict's internal storage,
the key length, the ``long double`` value, and an optional caller-supplied
context pointer.  The callback must not insert or remove entries during
traversal.
 
.. note::
 
   Values are stored and retrieved as exact bit patterns via ``memcpy``,
   preserving the full extended-precision mantissa.  As with :ref:`float` and
   :ref:`double`, NaN values can be stored and retrieved bitwise but cannot be
   looked up reliably.
 
   Use the ``%Lg`` specifier (capital ``L``) when printing ``long double``
   values — using ``%g`` without the ``L`` modifier silently truncates to
   ``double`` precision on most platforms and is undefined behaviour in strict
   C.  For full precision, use ``%.20Lg``.
 
   Literals must carry the ``L`` suffix to avoid implicit narrowing to
   ``double``; for example, write ``3.14159265358979323846L`` rather than
   ``3.14159265358979323846``.
 
.. code-block:: c
 
   static void print_entry(const char* key, size_t key_len,
                            long double value, void* ud) {
       (void)key_len; (void)ud;
       printf("  %s = %.20Lg\n", key, value);
   }
 
   foreach_ldouble_dict(d, print_entry, NULL);
 
.. doxygenfunction:: foreach_ldouble_dict
 
Introspection
-------------
 
.. doxygenfunction:: ldouble_dict_size
.. doxygenfunction:: ldouble_dict_hash_size
.. doxygenfunction:: ldouble_dict_alloc
.. doxygenfunction:: is_ldouble_dict_empty

long double Matrix
==================
A ``ldouble_matrix_t`` is a type-safe wrapper around the generic
:c:type:`matrix_t` container with the runtime ``dtype`` fixed to
``LDOUBLE_TYPE``.  It provides double-specific construction, element access,
conversion, comparison, and helper functions while reusing the generic
matrix engine implemented in ``c_matrix.h`` and ``c_matrix.c``.

The wrapper exists for APIs that either:

* fix the ``dtype`` to ``LDOUBLE_TYPE``
* replace ``void*`` element access with ``long double`` / ``long double*``
* provide double-specialised helper behaviour such as dense SIMD fast paths

All memory is managed through a caller-supplied
:c:type:`allocator_vtable_t`.  The library does not assume a default
allocator.

Storage Formats
---------------

``ldouble_matrix_t`` supports the same storage formats as
:c:type:`matrix_t`:

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Format
     - Description
   * - ``DENSE_MATRIX``
     - Row-major contiguous double buffer.
   * - ``COO_MATRIX``
     - Coordinate list with parallel ``row_idx``, ``col_idx``, and
       ``values`` arrays.  Intended for sparse matrix assembly.
   * - ``CSR_MATRIX``
     - Compressed Sparse Row format for sparse row-oriented operations.
   * - ``CSC_MATRIX``
     - Compressed Sparse Column format for sparse column-oriented operations.

.. note::

   ``set_double_matrix`` supports dense and COO matrices.  CSR and CSC are
   treated as derived sparse formats and are not directly mutable through
   element-wise set operations.

double Comparison and Zero Semantics
-----------------------------------

The generic matrix layer supports semantic comparison and semantic zero
testing through callback-style predicates such as
:c:type:`matrix_equal_fn` and :c:type:`matrix_zero_fn`.  These are
documented in ``c_matrix.h`` and are the correct place to describe
custom comparison behaviour. :contentReference[oaicite:7]{index=7}

For the double wrapper specifically:

* ``ldouble_matrix_equal`` performs exact logical equality and uses a
  dense SIMD fast path when both inputs are dense double matrices.
* Sparse and mixed-format comparisons fall back to generic element-wise
  comparison through the generic matrix engine.
* ``ldouble_matrix_is_zero`` uses a dense SIMD fast path for dense double
  matrices and falls back to the generic zero test for sparse formats.

.. note::

   Exact double equality is appropriate for structural container checks,
   but applications that require tolerance-based numerical comparison
   should document and apply their own comparison policy at a higher
   level.

Example Usage
-------------

.. code-block:: c

   #include "c_ldouble.h"

   allocator_vtable_t a = heap_allocator();

   ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 4, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   ldouble_matrix_t* m = r.u.value;

   set_ldouble_matrix(m, 0, 0, 1.0L);
   set_ldouble_matrix(m, 1, 2, 5.5L);
   set_ldouble_matrix(m, 2, 3, -3.0L);

   long double out = 0.0L;
   get_ldouble_matrix(m, 1, 2, &out);

   ldouble_matrix_expect_t tr = transpose_double_matrix(m, a);
   ldouble_matrix_expect_t csr = convert_double_matrix(m, CSR_MATRIX, a);

   return_ldouble_matrix(csr.u.value);
   return_ldouble_matrix(tr.u.value);
   return_ldouble_matrix(m);

.. code-block:: c

   ldouble_matrix_expect_t r = init_ldouble_coo_matrix(100, 100, 16, true, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   ldouble_matrix_t* sp = r.u.value;

   push_back_ldouble_coo_matrix(sp, 0, 5, 3.14L);
   push_back_ldouble_coo_matrix(sp, 42, 99, -1.0L);
   sort_double_coo_matrix(sp);

   long double v = 0.0;
   get_ldouble_matrix(sp, 10, 10, &v);   /* implicit zero */

   return_double_matrix(sp);

Structs
-------

.. note::

   ``ldouble_matrix_t`` is a ``typedef`` alias for :c:struct:`matrix_t`.
   The underlying generic storage layout is documented in :ref:`matrix`.

.. doxygenstruct:: ldouble_matrix_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_ldouble_dense_matrix
.. doxygenfunction:: init_ldouble_coo_matrix
.. doxygenfunction:: return_ldouble_matrix

Element Access
--------------

.. doxygenfunction:: get_ldouble_matrix
.. doxygenfunction:: set_ldouble_matrix

COO Assembly
------------

These functions are only valid for COO-format matrices.

.. doxygenfunction:: reserve_ldouble_coo_matrix
.. doxygenfunction:: push_back_ldouble_coo_matrix
.. doxygenfunction:: sort_ldouble_coo_matrix

Lifecycle / Structural Operations
---------------------------------

.. doxygenfunction:: clear_ldouble_matrix
.. doxygenfunction:: copy_ldouble_matrix
.. doxygenfunction:: convert_ldouble_matrix
.. doxygenfunction:: convert_ldouble_matrix_zero
.. doxygenfunction:: transpose_ldouble_matrix

Fill
----

.. doxygenfunction:: fill_ldouble_matrix

Shape and Compatibility
-----------------------

.. doxygenfunction:: ldouble_matrix_has_same_shape
.. doxygenfunction:: ldouble_matrix_is_square
.. doxygenfunction:: ldouble_matrix_is_sparse
.. doxygenfunction:: ldouble_matrix_is_zero
.. doxygenfunction:: ldouble_matrix_equal
.. doxygenfunction:: ldouble_matrix_is_add_compatible
.. doxygenfunction:: ldouble_matrix_is_multiply_compatible

Row and Column Swaps
--------------------

.. doxygenfunction:: swap_ldouble_matrix_rows
.. doxygenfunction:: swap_ldouble_matrix_cols

Special Constructors
--------------------

.. doxygenfunction:: init_ldouble_identity_matrix
.. doxygenfunction:: init_ldouble_row_vector
.. doxygenfunction:: init_ldouble_col_vector

Vector Queries
--------------

.. doxygenfunction:: ldouble_matrix_is_row_vector
.. doxygenfunction:: ldouble_matrix_is_col_vector
.. doxygenfunction:: ldouble_matrix_is_vector
.. doxygenfunction:: ldouble_matrix_vector_length

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
   ldouble-specific APIs in the public documentation.
