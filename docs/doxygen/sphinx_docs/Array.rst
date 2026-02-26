.. _array:

*************
Generic Array
*************

``c_array.h`` provides the generic, type-erased dynamic array that underpins
every typed array wrapper in csalt.  It stores elements as raw bytes in a
contiguous heap-allocated buffer and resolves element size, type identity, and
allocator at initialisation time, caching all three on the struct so that no
registry lookups or virtual dispatch overhead occurs during normal operations.

The generic API requires a :c:type:`dtype_id_t` argument on every call that
reads or writes element data.  This type tag is checked against the tag
fixed at initialisation and any mismatch is returned as ``TYPE_MISMATCH``
before any data is touched.  If you do not need this runtime type check, use
one of the typed wrappers (e.g. :ref:`uint8`) which hard-code the dtype and
hide it from the call site entirely.

All memory is managed through an :c:type:`allocator_vtable_t` supplied by the
caller.  csalt does not assume a default allocator — the caller must
explicitly provide one at every call site that allocates memory (initialisation,
copy, and slice).  See :ref:`allocator_file` for the full allocator API, the
list of available implementations, and guidance on writing your own.

Error handling follows the *expected value* pattern: operations that produce a
new :c:type:`array_t` return an :c:type:`array_expect_t` whose ``has_value``
field distinguishes success from failure; in-place mutations return an
:c:type:`error_code_t` directly.  Callers must always check the result before
using the value.

.. code-block:: c

   #include "c_array.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   array_expect_t r = init_array(8, UINT8_TYPE, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   array_t* arr = r.u.value;

   uint8_t val = 42;
   push_back_array(arr, &val, UINT8_TYPE);

   val = 0;
   get_array_index(arr, 0, &val, UINT8_TYPE);  /* val == 42 */

   return_array(arr);

.. note::

   For day-to-day use with a single element type, prefer a typed wrapper such
   as :ref:`uint8`.  The generic API is intended for code that must operate
   on arrays of arbitrary or runtime-determined dtype, such as container
   libraries, serialisers, or generic algorithms.

Structs
-------

.. doxygenstruct:: array_t
   :members:

.. doxygenstruct:: array_expect_t
   :members:

.. doxygenstruct:: size_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_array

.. doxygenfunction:: return_array

Push Operations
---------------

All three push variants accept a ``dtype`` argument that must match the
``dtype`` fixed at initialisation.  A mismatch returns ``TYPE_MISMATCH``
without modifying the array.  The ``data`` pointer must point to at least
``array->data_size`` bytes.

.. doxygenfunction:: push_back_array

.. doxygenfunction:: push_front_array

.. doxygenfunction:: push_at_array

Get and Set
-----------

.. doxygenfunction:: get_array_index

.. doxygenfunction:: set_array_index

Pop Operations
--------------

All three pop variants accept ``NULL`` for *out* when the removed value is
not needed.  Popping from an empty array returns ``EMPTY``.  The *out*
buffer, when non-``NULL``, must be at least ``array->data_size`` bytes.

.. doxygenfunction:: pop_back_array

.. doxygenfunction:: pop_front_array

.. doxygenfunction:: pop_any_array

Utility Operations
------------------

.. doxygenfunction:: clear_array

.. doxygenfunction:: copy_array

.. doxygenfunction:: concat_array

.. doxygenfunction:: slice_array

Type Query
----------

.. doxygenfunction:: array_type

Algorithms
----------

.. doxygenfunction:: reverse_array

.. doxygenfunction:: sort_array

.. doxygenfunction:: binary_search_array 

.. doxygenfunction:: binary_bracket_array

.. note::

   ``sort_array`` requires a caller-supplied comparator following the same
   convention as ``qsort(3)``.  Typed wrappers such as
   :c:func:`sort_uint8_array` provide a built-in comparator so the caller
   does not need to write one for primitive types.

Search
------

.. doxygenfunction:: array_contains

Introspection
-------------

All introspection functions are O(1) read-only operations.  Every function
returns ``0`` or ``false`` when passed a ``NULL`` pointer rather than
dereferencing it.

.. doxygenfunction:: array_size

.. doxygenfunction:: array_alloc

.. doxygenfunction:: array_data_size

.. doxygenfunction:: is_array_empty

.. doxygenfunction:: is_array_full

.. doxygenfunction:: is_array_ptr
