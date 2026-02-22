.. _types:

Data Types (dtypes)
===================

Overview
--------

The ``dtypes.h`` header is the foundation of the library's type system. It provides
a runtime registry of data type descriptors that is shared across every data structure
in the library — arrays, matrices, and any future structures you add. Rather than
hardcoding type information into each data structure, the library treats type identity
as a first-class, centrally managed concern.

This design serves two goals. First, it allows the library to perform type-safe
operations on type-erased byte buffers at runtime, catching mismatches between a data
structure and the type of data being written into or read from it. Second, it allows
users to define and register their own custom types — structs, composite types, or
any fixed-size data — and use them with any data structure in the library without
modifying library source code.

Any project that uses this library should include ``dtypes.h`` directly when working
with the type registry. All other library headers (``array.h``, ``matrix.h``, etc.)
include ``dtypes.h`` transitively, so explicit inclusion is only necessary when
interacting with the registry outside the context of a specific data structure.

.. code-block:: c

   #include "dtypes.h"


How It Works
------------

Every data type in the library is identified by a :c:type:`dtype_id_t`, which is a
32-bit unsigned integer. Each ID maps to a :c:type:`dtype_t` descriptor stored in an
internal registry. The descriptor records the ID, the size in bytes of one element of
that type, and a human-readable name used for debugging and logging.

When a data structure such as an array is initialized for a given type, it stores that
type's ID. Every subsequent operation — push, get, insert — validates the ID against
the type the structure was initialized with. This ensures that data written as floats
cannot be silently read back as integers or any other type.

The registry is populated at startup with a set of built-in C primitive types. Users
extend the registry by registering their own descriptors before using them with any
data structure.


Initialization
--------------

The registry must be initialized before use. All data structure init functions in the
library call :c:func:`init_dtype_registry` internally, so in most cases no explicit
call is required. However, if the registry is needed before any data structure is
initialized — for example, to register custom types at program startup — it should be
called explicitly.

.. code-block:: c

   if (init_dtype_registry() == false) {
       // handle error
   }

It is safe to call ``init_dtype_registry()`` multiple times. After the first successful
call, subsequent calls return ``true`` immediately without re-registering built-in types.


Built-in Types
--------------

The following type IDs are registered automatically by :c:func:`init_dtype_registry`.
They cover all common C primitive types and should not be reused for user-defined types.

.. list-table::
   :header-rows: 1
   :widths: 30 20 50

   * - Constant
     - ID
     - C Type
   * - ``UNKNOWN_TYPE``
     - 0
     - Uninitialized or invalid
   * - ``FLOAT_TYPE``
     - 1
     - ``float``
   * - ``DOUBLE_TYPE``
     - 2
     - ``double``
   * - ``LDOUBLE_TYPE``
     - 3
     - ``long double``
   * - ``CHAR_TYPE``
     - 4
     - ``char``
   * - ``UCHAR_TYPE``
     - 5
     - ``unsigned char``
   * - ``INT8_TYPE``
     - 6
     - ``int8_t``
   * - ``UINT8_TYPE``
     - 7
     - ``uint8_t``
   * - ``INT16_TYPE``
     - 8
     - ``int16_t``
   * - ``UINT16_TYPE``
     - 9
     - ``uint16_t``
   * - ``INT32_TYPE``
     - 10
     - ``int32_t``
   * - ``UINT32_TYPE``
     - 11
     - ``uint32_t``
   * - ``INT64_TYPE``
     - 12
     - ``int64_t``
   * - ``UINT64_TYPE``
     - 13
     - ``uint64_t``
   * - ``BOOL_TYPE``
     - 14
     - ``bool``
   * - ``SIZE_T_TYPE``
     - 15
     - ``size_t``

IDs 16 through 999 are reserved for future built-in type expansion. Do not assign
user-defined types to values in this range.


Registering User-Defined Types
-------------------------------

User-defined type IDs must be greater than or equal to ``USER_BASE_TYPE`` (1000).
Each ID must be unique within the registry. The recommended convention is to define
named constants relative to ``USER_BASE_TYPE`` so that any future change to its value
does not require updates throughout user code.

.. code-block:: c

   #define MY_VEC3_TYPE    (USER_BASE_TYPE + 1u)
   #define MY_COMPLEX_TYPE (USER_BASE_TYPE + 2u)

To register a type, define a :c:type:`dtype_t` descriptor and pass it to
:c:func:`ensure_dtype_registered`. This function is idempotent — calling it multiple
times with the same descriptor is safe. It is the preferred registration function for
typed wrapper init functions because it combines library initialization, duplicate
detection, and registration into a single call.

.. code-block:: c

   #include "dtypes.h"

   typedef struct { float x; float y; float z; } vec3_t;

   static const dtype_t vec3_desc = {
       MY_VEC3_TYPE, sizeof(vec3_t), "vec3"
   };

   vec3_array_t* init_vec3_array(size_t indices, bool growth) {
       if (ensure_dtype_registered(&vec3_desc) == false) return NULL;
       return init_array(indices, MY_VEC3_TYPE, growth);
   }

If you need finer control, :c:func:`register_dtype` can be called directly. Unlike
``ensure_dtype_registered``, it returns ``false`` if the type is already registered,
so it should only be used when duplicate registration is an error condition rather
than a no-op.


Checking Registry Capacity
---------------------------

The registry holds a maximum of ``MAX_DTYPES`` (128) entries in total, including
built-in types. The 16 built-in types registered at startup consume the first 16 slots,
leaving 112 slots for user-defined types under default configuration. Use
:c:func:`available_dtype_slots` to check remaining capacity before registering types
in applications that define many custom types.

.. code-block:: c

   size_t slots = available_dtype_slots();
   if (slots == 0u) {
       // registry is full
   }


Looking Up a Type
-----------------

:c:func:`lookup_dtype` searches the registry by ID and returns a pointer to the
internal descriptor if found. This is useful when you need to retrieve the size or
name of a type from its ID alone, for example in generic data structure operations or
for diagnostic output.

.. code-block:: c

   const dtype_t* desc = lookup_dtype(FLOAT_TYPE);
   if (desc != NULL) {
       printf("type: %s, size: %zu bytes\n", desc->name, desc->data_size);
   }

The returned pointer is owned by the registry and remains valid for the lifetime of
the program. It must not be modified or freed by the caller.


API Reference
-------------

.. doxygenfile:: c_dtypes.h
   :project: csalt


Summary of ID Ranges
--------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Range
     - Purpose
   * - ``0``
     - Reserved — ``UNKNOWN_TYPE``, indicates uninitialized state
   * - ``1`` – ``15``
     - Built-in C primitive types, registered by ``init_dtype_registry()``
   * - ``16`` – ``999``
     - Reserved for future built-in type expansion
   * - ``1000`` and above
     - User-defined types; must be ``>= USER_BASE_TYPE``
