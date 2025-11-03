.. _allocator_file:

********************
C Allocator Overview
********************

Memory allocation in C typically involves direct use of ``malloc()``,
``calloc()``, and ``free()``. While flexible, these can incur performance
overhead, lead to fragmentation, and increase the risk of memory leaks or
dangling pointers in large applications.

The *C Allocator module* in this library provides lightweight and efficient
allocation utilities implemented in pure C and declared in
``c_allocator.h``. The module has no external dependencies within the CSalt
library, making it suitable for independent use as well as integration into
larger systems.

Build-time configuration flags modify behavior depending on needs:

* ``STATIC_ONLY`` — Disables all heap allocation. Only stack or static
  memory supplied by the application is permitted.
* ``NO_FUNCTION_MACROS`` — Removes convenience macros that wrap allocation
  behavior. This improves transparency and supports MISRA C compliance.

These features make the allocator suitable for environments requiring strict
determinism and safety analysis, such as embedded and real-time systems or projects 
that require compliance to MISRA C standards.

Arena Overview 
==============
The ``arena_t`` allocator implements a *bump-pointer* memory allocation model.
Memory is acquired in large blocks, and each allocation advances an internal
write cursor. All allocated memory may be reclaimed simultaneously, rather
than individually.

This model is ideal when:

* a group of allocations share the same lifetime
* memory may be cleared in a single operation
* peak memory needs are known in advance or bounded

Examples include:

* parsing and temporary string generation
* construction of intermediate data structures
* frame-based game engine or graphical workloads
* scratch space for iterative scientific computations

Benefits
--------

* **Fast allocation performance** (constant time)
* **No fragmentation**
* **High data locality**
* **Complete reset in constant time** using ``reset_arena()``

Limitations
-----------

* Individual objects cannot be freed
* Memory usage may exceed minimal requirements (i.e. dynamic allocations are in powers of 2)
* Best used in controlled phases of execution

Static vs. Dynamic Arenas
-------------------------

There are two Arena types provided:

* *Static arena_t* — fixed-size buffer supplied by the caller; no heap usage.
* *Dynamic arena_t* — optionally grows through additional memory chunks.

When the build is configured with ``STATIC_ONLY``, dynamic features are
excluded entirely at compile time, ensuring the resulting codebase contains
no hidden heap allocation paths.

This allows the same API to serve both safety-critical systems and
high-performance workloads, simply by selecting different build flags.

Data Types 
----------
The following are data structures and derived data types used in the ``c_allocator.h``
and ``c_allocator.c`` files.

Chunk 
~~~~~
Chunk is an opaque data structure that is not visibile to the user.  This struct 
contains metadata on a chunk of allocated memory.  If the allocator is initialized 
for dynamic memory allocation and it allows resizing, it will also point to the  
next ``Chunk`` data structure.

.. code-block:: c

   struct Chunk{
       uint8_t *chunk;     // Pointer to beginning of memory
       size_t len;         // Populated length of memory within struct in bytes
       size_t alloc;       // Allocated length of memory within struct in bytes
       struct Chunk* next; // Pointer to next memory chunk
   };

arena_t 
~~~~~~~
``arena_t`` is an opaque data structure that can not be directly accessed by a user.
This structure contains all of the metadata associated with a bump allocator in 
additon, this structure also contains pointers to the head and tail ``Chunk`` structures 
which contain all memory allocations.  Metadata within this struct can be accessed 
through getter functions.

.. code-block:: c

   typedef struct {
       uint8_t* cur;      // A pointer to the next available memory slot
       Chunk* head;       // Pointer to head of memory chunk linked list
       Chunk* tail;       // Pointer to the tail of memory chunks for the linked list

       size_t alignment;  // alignment in bytes 
       size_t len;        // Total memory used in bytes between all memory chunks
       size_t alloc;      // Total memory allocated in bytes between all memory chunks
       size_t tot_alloc;  // Total memory allocated to include containers
       size_t min_chunk;  // The minimum chunk size in bytes

       uint8_t mem_type;  // type of memory used
       uint8_t resize;    // allows resizing if true with mem_type == DYNAMIC
       uint8_t _pad[6];   // keep 8 byte passing
   } arena_t;

ArenaCheckPoint 
~~~~~~~~~~~~~~~
``ArenaCheckPoint`` is an opaque data structure that is used to store data 
related to a bump allocator.  The XX function can extract the data from 
this structure to reconstitute a bump allocator.

.. code-block:: c 

   typedef struct {
       Chunk* chunk;     // Pointer to saved memory chunk
       uint8_t* cur;     // Current pointer position in saved chunk
       size_t len;       // Length of saved chunk
   } ArenaCheckPointRep;

Initialization and Memory Management
------------------------------------
The functions in this section can be used to initialize memory for a bump allocator,
parse that memory to variables and to deallocate the memory.

init_dynamic_arena 
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_dynamic_arena
   :project: csalt

init_static_arena
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_static_arena
   :project: csalt

init_darena 
~~~~~~~~~~~

.. doxygenfunction:: init_darena
   :project: csalt

init_sarena 
~~~~~~~~~~~

.. doxygenfunction:: init_sarena
   :project: csalt

free_arena
~~~~~~~~~~

.. doxygenfunction:: free_arena
   :project: csalt

alloc_arena 
~~~~~~~~~~~

.. doxygenfunction:: alloc_arena
   :project: csalt

alloc_arena_aligned
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: alloc_arena_aligned
   :project: csalt

Utility Funcitons 
-----------------

is_arena_ptr
~~~~~~~~~~~~

.. doxygenfunction:: is_arena_ptr
   :project: csalt

is_arena_ptr_sized
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: is_arena_ptr_sized
   :project: csalt

reset_arena
~~~~~~~~~~~

.. doxygenfunction:: reset_arena
   :project: csalt

save_arena
~~~~~~~~~~

.. doxygenfunction:: save_arena
   :project: csalt

restore_arena
~~~~~~~~~~~~~

.. doxygenfunction:: restore_arena
   :project: csalt

arena_stats 
~~~~~~~~~~~

.. doxygenfunction:: arena_stats
   :project: csalt

Getter and Setter Functions 
---------------------------

arena_remaining
~~~~~~~~~~~~~~~

.. doxygenfunction:: arena_remaining
   :project: csalt


arena_chunk_count
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: arena_chunk_count
   :project: csalt


arena_mtype
~~~~~~~~~~~

.. doxygenfunction:: arena_mtype
   :project: csalt


arena_size
~~~~~~~~~~

.. doxygenfunction:: arena_size
   :project: csalt

arena_alloc
~~~~~~~~~~~

.. doxygenfunction:: arena_alloc
   :project: csalt

total_arena_alloc
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: total_arena_alloc
   :project: csalt

arena_alignment
~~~~~~~~~~~~~~~

.. doxygenfunction:: arena_alignment
   :project: csalt

arena_min_chunk_size
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: arena_min_chunk_size
   :project: csalt

toggle_arena_resize 
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: toggle_arena_resize
   :project: csalt

Function Like Macros 
--------------------

arena_alloc_type
~~~~~~~~~~~~~~~~

.. doxygendefine:: arena_alloc_type
   :project: csalt

arena_alloc_array
~~~~~~~~~~~~~~~~~

.. doxygendefine:: arena_alloc_array
   :project: csalt

arena_alloc_type_zeroed
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygendefine:: arena_alloc_type_zeroed
   :project: csalt

arena_alloc_array_zeroed
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygendefine:: arena_alloc_array_zeroed
   :project: csalt

Pool Allocator 
==============

Data Types 
----------

Initialization and Memory Management 
------------------------------------

Gett and Setter Functions 
-------------------------

Function Like Macros 
--------------------
