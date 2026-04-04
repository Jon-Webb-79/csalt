**************
Matrix Module
**************

``c_matrix.h`` provides a generic, type-erased matrix container built on top
of the csalt allocator and dtype systems.  It is designed to support multiple
storage formats, flexible memory models, and efficient structural operations,
while remaining fully agnostic to the stored data type.

Unlike numerical libraries, this module **does not implement mathematical
algorithms** (e.g., multiplication, inversion, decomposition).  It provides
only the container and structural operations required to build such systems.

Numerical functionality is intended to be implemented in **type-specific
modules** layered on top of this container.

Overview
--------

The matrix stores values as raw byte buffers using a runtime dtype system
(:c:type:`dtype_id_t`).  The container supports multiple internal
representations:

- Dense (row-major contiguous storage)
- COO (coordinate list sparse format)
- CSR (compressed sparse row)
- CSC (compressed sparse column)

Each representation is optimized for different access and modification
patterns.

.. code-block:: c

   #include "c_matrix.h"

   allocator_vtable_t alloc = heap_allocator();

   matrix_expect_t r = init_dense_matrix(3, 3, INT32_TYPE, alloc);
   if (!r.has_value) { /* handle error */ }

   matrix_t* mat = r.u.value;

   int32_t v = 5;
   set_matrix(mat, 1, 1, &v);

   int32_t out = 0;
   get_matrix(mat, 1, 1, &out);  /* out == 5 */

   return_matrix(mat);

.. note::

   The generic API performs no implicit type conversions.  The caller is
   responsible for ensuring that all inserted values match the declared
   ``dtype`` and ``data_size``.

Container vs Numerical Layer
----------------------------

This module is strictly a **container abstraction**.

It provides:

- memory management
- format conversions
- structural operations (transpose, reshape-like behavior)
- compatibility checks
- basic element access

It does *not* provide:

- matrix multiplication
- inversion or solving systems
- decompositions (LU, QR, etc.)
- numerical stability guarantees

These operations should be implemented in **type-specific modules**, such as:

- ``c_matrix_float.c``
- ``c_matrix_double.c``
- ``c_matrix_int32.c``

This separation ensures:

- clean abstraction boundaries
- better SIMD specialization
- reduced complexity in the generic layer

Memory Model
------------

The matrix uses different layouts depending on format.

Dense Format
~~~~~~~~~~~~

- Contiguous row-major storage
- Element offset computed as:

.. code-block:: c

   offset = (row * cols + col) * data_size

Sparse Formats
~~~~~~~~~~~~~~

COO:
- Stores triplets (row, col, value)
- Flexible insertion
- Requires sorting for optimal traversal

CSR:
- Row-compressed format
- Efficient row access and iteration

CSC:
- Column-compressed format
- Efficient column access and iteration

All memory is managed through :c:type:`allocator_vtable_t`.

Ownership Rules
---------------

The matrix owns:

- its header
- all internal buffers for the active format

Memory is released via :c:func:`return_matrix`.

Callers must never free internal buffers directly.

Allocator Integration
---------------------

All allocations are performed through :c:type:`allocator_vtable_t`.

This allows matrices to be backed by:

- heap allocators
- arena allocators
- pool allocators
- custom user-defined allocators

The matrix container makes no assumptions about allocator behavior beyond
the provided interface.

Recommended Allocators
----------------------

* **HeapAllocator**
  
  Best for:
  
  * general-purpose use
  * unit testing
  * debugging container logic

* **ArenaAllocator**
  
  Best for:
  
  * short-lived matrices
  * batch computations
  * build-then-discard workflows

* **BuddyAllocator**
  
  Best for:
  
  * large dynamic matrices
  * systems requiring memory coalescing

* **PoolAllocator**
  
  Best for:
  
  * fixed-size dense matrices
  * repeated allocations of similar shapes

Error Handling
--------------

The matrix follows the csalt *expected value* pattern:

- Constructors return :c:type:`matrix_expect_t`
- Mutating operations return :c:type:`error_code_t`

.. code-block:: c

   matrix_expect_t r = init_dense_matrix(...);
   if (!r.has_value) {
       error_code_t err = r.u.error;
       /* handle error */
   }

Type System and User-Defined Types
----------------------------------

The matrix relies on :c:type:`dtype_id_t` for runtime type information.

Built-in types include:

- ``INT32_TYPE``
- ``FLOAT_TYPE``
- ``DOUBLE_TYPE``
- etc.

User-defined types may be registered as follows:

.. code-block:: c

   typedef struct {
       float x, y, z;
   } vec3_t;

   #define VEC3_TYPE (USER_BASE_TYPE + 1u)

   static const dtype_t vec3_desc = {
       VEC3_TYPE,
       sizeof(vec3_t),
       "vec3"
   };

   ensure_dtype_registered(&vec3_desc);

   matrix_expect_t r =
       init_dense_matrix(4, 4, VEC3_TYPE, alloc);

.. note::

   User-defined dtype IDs must be >= ``USER_BASE_TYPE`` and must be unique.

Typed Wrapper Pattern (Recommended)
-----------------------------------

The generic API operates on ``void*`` buffers.  For usability and type safety,
it is recommended to define thin wrappers.

Example:

.. code-block:: c

   static inline error_code_t set_mat_i32(matrix_t* m,
                                          size_t r,
                                          size_t c,
                                          int32_t v) {
       return set_matrix(m, r, c, &v);
   }

   static inline int32_t get_mat_i32(const matrix_t* m,
                                     size_t r,
                                     size_t c) {
       int32_t out = 0;
       get_matrix(m, r, c, &out);
       return out;
   }

This provides:

- cleaner call sites
- compile-time type safety
- zero runtime overhead

Vector Helpers
--------------

The matrix API provides helpers for vector-shaped matrices:

- Row vector: ``1 x N``
- Column vector: ``N x 1``

.. code-block:: c

   matrix_expect_t v = init_row_vector(10, FLOAT_TYPE, alloc);

   if (matrix_is_vector(v.u.value)) {
       size_t len = matrix_vector_length(v.u.value);
   }

Core API
--------

Structs
~~~~~~~

.. doxygenstruct:: matrix_t
   :members:

.. doxygenstruct:: matrix_expect_t
   :members:

Initialisation and Teardown
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_dense_matrix

.. doxygenfunction:: init_coo_matrix

.. doxygenfunction:: return_matrix

Access Operations
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: get_matrix

.. doxygenfunction:: set_matrix

COO Operations
~~~~~~~~~~~~~~

.. doxygenfunction:: push_back_coo_matrix

.. doxygenfunction:: reserve_coo_matrix

.. doxygenfunction:: sort_coo_matrix

Conversion and Transformation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: convert_matrix

.. doxygenfunction:: transpose_matrix

Structural Operations
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: clear_matrix


.. doxygenfunction:: fill_matrix

.. doxygenfunction:: swap_matrix_rows

.. doxygenfunction:: swap_matrix_cols

Diagnostics
~~~~~~~~~~~

.. doxygenfunction:: matrix_equal

.. doxygenfunction:: is_zero_matrix

.. doxygenfunction:: matrix_storage_bytes

Compatibility Checks
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: matrix_has_same_shape

.. doxygenfunction:: matrix_has_same_dtype

.. doxygenfunction:: matrix_is_add_compatible

.. doxygenfunction:: matrix_is_multiply_compatible

Shape Helpers
~~~~~~~~~~~~~

.. doxygenfunction:: matrix_is_square

.. doxygenfunction:: matrix_is_vector

.. doxygenfunction:: matrix_is_row_vector

.. doxygenfunction:: matrix_is_col_vector

.. doxygenfunction:: matrix_vector_length
