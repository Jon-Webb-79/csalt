.. _int_vector_file:

******************
C Integer Overview
******************
The ``c_int.h`` header file contains implementations for statically 
and dynamically allocated arrays of `int` data types.  In addition, the 
``c_int.h`` header file contains an implementation for a dictionary of 
float values with a string keyword as well as various matrices to include 
``Dense``, ``CSC``, ``CSR``, and ``COO`` matrix types.


Float Vector Overview
=====================

An int vector provides both dynamic and static array containers for int values, offering
efficient management of numerical data with automatic memory handling for dynamic arrays
and safe access for static arrays. This implementation combines the flexibility of dynamic
arrays with the safety of bounds checking and proper memory management.  All of the functions 
described in this section can be found in the ``c_int.h`` header file.

The vector implementation within this ``c_int.h`` header file contains these 
key features for the implementation of ``int_v`` data types.

* Dynamic and static array support: Choose between automatic resizing or fixed-size arrays
* Memory safety: Proper encapsulation and memory management
* Bounds checking: Safe access to array elements
* Efficient access: O(1) access to any value in the vector
* Automatic cleanup: Optional garbage collection support with FLTVEC_GBC

Float vectors are particularly useful when:

* Managing collections of int values that may grow or shrink
* Working with an unknown number of int values at compile time
* Requiring sequential access to multiple int values
* Needing efficient random access to int values by index
* Managing related numerical int as a single unit
* Implementing numerical algorithms or data processing pipelines

the functions for the ``int_v`` data type have the following performance 
characteristics.

* Access time: O(1) for index-based access
* Insertion time: O(1) amortized for push_back operations
* Memory efficiency: Dynamic vectors grow geometrically to minimize reallocations
* Memory overhead: Minimal per-element overhead

Data Types
----------
The follwing enums and structs can be found in the ``c_int.h`` file to produce 
derived data types.

.. code:: c

   typedef enum {
       FORWARD = 0, // visit elements in forward order 
       REVERSE = 1  // visit elements in reverse order 
   } iter_dir;

   typedef enum {
       STATIC = 0  // Indicates static memory allocation on the stack 
       DYNAMIC = 1 // Indicates dynamic memory allocation on the heap 
   } alloc_t;

   typedef struct {
       int* data;         // Pointer to c-style array of int values
       size_t len;        // The populated length of the array 
       size_t alloc;      // The allocated length of the array 
       ErrorCode error;   // ErrorCode enum describing relevant errors 
       alloc_t alloc_type // DYNAMIC or STATIC memory allocation
   } int_v;

.. note:: ``ErrorCode`` enum can be found in the :ref:`c_error.h <error_code_file>` file.

Initialization and Memory Management
------------------------------------

Utility Functions 
-----------------

Vector Manipulation 
-------------------

Search Vector 
-------------

Vector Iterator 
---------------

Float Matrix Overview 
=====================

Data Type 
---------

Initialization and Memory Management 
------------------------------------

Utility Functions 
-----------------

Matrix Manipulation 
-------------------

Matrix Operations 
-----------------

Float Dictionary Overview
==========================

An int dictionary provides a hash table implementation for mapping string keys to int values, 
offering efficient key-value storage and retrieval. This implementation uses chained hashing for 
collision resolution and features automatic table resizing for optimal performance.

The key features of an int dictionary are the following;

* Dynamic resizing: Automatic growth when load factor threshold is reached
* Efficient lookup: O(1) average case access time
* Memory safety: Proper encapsulation and memory management
* String key support: Automatic key duplication and management
* Collision handling: Chained hashing for robust collision resolution
* Automatic cleanup: Optional garbage collection support with FDICT_GBC

An int dicitonary should be used for the following reasons.

Integer dictionaries are particularly useful when:

* Mapping strings to numerical values
* Requiring fast key-value lookups
* Managing relationships between text identifiers and measurements
* Implementing caches or lookup tables with string keys
* Building numerical mapping tables
* Creating frequency counters for string data

An  int matrix has the following performance characteristics.

* Access time: O(1) average case for lookups and insertions
* Space efficiency: Adaptive growth strategy for memory efficiency
* Collision handling: Chained hashing for reliable performance under high load
* Memory overhead: Small per-entry overhead for key storage and chain pointers

Data Types
----------

The following are derived data types used in the implementation of int dictionaries.

General Float Functions 
=======================

Generic Macro Overview
======================
