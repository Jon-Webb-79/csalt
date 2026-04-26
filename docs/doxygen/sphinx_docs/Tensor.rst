.. _tensor:

******
Tensor
******

``c_tensor.h`` provides the generic, type-erased N-dimensional array that
underpins every typed wrapper in csalt for one-dimensional arrays, two-dimensional
matrices, and higher-rank tensors.  It stores elements as raw bytes in a
contiguous allocator-managed buffer and resolves element size, type identity,
shape, strides, and allocator at initialisation time, caching all of them on
the struct so that no registry lookups or virtual dispatch overhead occurs
during normal operations.

Runtime behaviour is governed by a ``tensor_mode_t`` field set at initialisation:

* ``TENSOR_STRUCT`` — all slots are zero-initialised at construction and
  considered live for the lifetime of the tensor.  ``len`` equals ``alloc``
  equals the product of all shape dimensions.  Element access is bounded by
  ``alloc`` and remains valid after ``clear_tensor``.  This mode is used for
  matrices and higher-rank tensors.

* ``ARRAY_STRUCT`` — ``len`` tracks the populated element count;
  slots in ``[len, alloc)`` are inaccessible until exposed by a push operation.
  Element access is bounded by ``len``.  Automatic reallocation on push is
  optionally enabled via the ``growth`` flag.  This mode is used for
  one-dimensional dynamic arrays.

The generic API requires a :c:type:`dtype_id_t` argument on every call that
reads or writes element data.  This type tag is checked against the tag fixed
at initialisation and any mismatch is returned as ``TYPE_MISMATCH`` before any
data is touched.

All memory is managed through an :c:type:`allocator_vtable_t` supplied by the
caller.  csalt does not assume a default allocator — the caller must explicitly
provide one at every call site that allocates memory (initialisation and copy).
See :ref:`allocator_file` for the full allocator API, the list of available
implementations, and guidance on writing your own.

Error handling follows the *expected value* pattern: operations that produce a
new :c:type:`tensor_t` return a :c:type:`tensor_expect_t` whose ``has_value``
field distinguishes success from failure; in-place mutations return an
:c:type:`error_code_t` directly.  Callers must always check the result before
using the value.

.. code-block:: c

   #include "c_tensor.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   /* Create a 3x4 matrix of floats (TENSOR_STRUCT). */
   size_t shape[] = { 3, 4 };
   tensor_expect_t r = init_tensor(2, shape, FLOAT_TYPE, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   tensor_t* t = r.u.value;

   /* Write to element (1, 2) using an N-dimensional index. */
   float val = 1.5f;
   size_t idx[] = { 1, 2 };
   set_tensor_nd_index(t, idx, &val, FLOAT_TYPE);

   /* Read it back. */
   float out = 0.0f;
   get_tensor_nd_index(t, idx, &out, FLOAT_TYPE);  /* out == 1.5f */

   return_tensor(t);

.. note::

   For day-to-day use with a single element type, prefer a typed wrapper.
   The generic API is intended for code that must operate on tensors of 
   arbitrary or runtime-determined dtype, such as container libraries, 
   serialisers, or generic algorithms.

Recommended Allocators
----------------------

Different allocators are appropriate depending on how tensors are constructed,
resized, and accessed:

* **HeapAllocator**

  Best for:

  * general-purpose dynamic arrays and matrices
  * frequent resizing of ``ARRAY_STRUCT`` tensors
  * applications where simplicity and correctness are priorities

  The heap allocator provides flexible and predictable behaviour for both
  fixed-shape tensors and growing arrays.

* **ArenaAllocator**

  Best for:

  * fixed-shape tensors built once and discarded together
  * batch construction of many small tensors within a single lifetime
  * intermediate tensors in algorithms

  Arena allocation is efficient when tensors are not resized after
  construction.  ``ARRAY_STRUCT`` tensors that grow frequently may leave
  unused memory in the arena.

* **BuddyAllocator**

  Best for:

  * large fixed-shape tensors or matrices
  * systems where memory fragmentation must be controlled
  * long-lived tensors with periodic growth

  The buddy allocator handles large contiguous allocations well and reduces
  fragmentation compared to a general heap.

* **PoolAllocator / SlabAllocator**

  Best for:

  * fixed-capacity tensors of uniform shape
  * performance-critical paths with predictable memory use
  * many small tensors of identical rank and shape

  These allocators are not well suited for ``ARRAY_STRUCT`` tensors that
  grow, but can be very efficient when shape and capacity are known in advance.

Structs
-------

.. doxygenstruct:: tensor_t
   :members:

.. doxygenstruct:: tensor_expect_t
   :members:

.. doxygenenum:: tensor_mode_t

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_tensor

.. doxygenfunction:: init_tensor_array

.. doxygenfunction:: return_tensor

Array Operations
----------------

The following operations are only valid for ``ARRAY_STRUCT`` tensors.
Calling them on a ``TENSOR_STRUCT`` tensor returns ``ILLEGAL_STATE``.
All push and pop variants accept a ``dtype`` argument that must match the
``dtype`` fixed at initialisation; a mismatch returns ``TYPE_MISMATCH``
without modifying the tensor.

.. doxygenfunction:: push_back_tensor

.. doxygenfunction:: push_front_tensor

.. doxygenfunction:: push_at_tensor

.. doxygenfunction:: pop_back_tensor

.. doxygenfunction:: pop_front_tensor

.. doxygenfunction:: pop_at_tensor

Get and Set
-----------

Flat-index access is valid for both modes.  The effective bound is ``alloc``
for ``TENSOR_STRUCT`` and ``len`` for ``ARRAY_STRUCT``.
N-dimensional index access is only valid for ``TENSOR_STRUCT`` tensors;
calling either ``_nd`` variant on a ``ARRAY_STRUCT`` tensor returns
``ILLEGAL_STATE``.

.. doxygenfunction:: set_tensor_index

.. doxygenfunction:: set_tensor_nd_index

.. doxygenfunction:: get_tensor_index

.. doxygenfunction:: get_tensor_nd_index

Utility Operations
------------------

.. doxygenfunction:: clear_tensor

.. doxygenfunction:: copy_tensor

.. doxygenfunction:: concat_tensor_array

.. doxygenfunction:: slice_tensor_array

Type Query
----------

.. doxygenfunction:: tensor_dtype

Introspection
-------------

All introspection functions are O(1) read-only operations.  Every function
returns ``0``, ``false``, or ``NULL`` when passed a ``NULL`` pointer rather
than dereferencing it.

.. doxygenfunction:: tensor_size

.. doxygenfunction:: tensor_alloc

.. doxygenfunction:: tensor_data_size

.. doxygenfunction:: tensor_ndim

.. doxygenfunction:: tensor_shape_dim

.. doxygenfunction:: tensor_shape

.. doxygenfunction:: tensor_shape_ptr

.. doxygenfunction:: tensor_strides_ptr

.. doxygenfunction:: tensor_shape_str

.. doxygenfunction:: is_tensor_empty

.. doxygenfunction:: is_tensor_full

.. doxygenfunction:: is_tensor_ptr
