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
and ``c_allocator.c`` files to support the ``arena_t`` data type.

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
related to a bump allocator.  ``The restore_arena`` function can extract the data from 
this structure to reconstitute a bump allocator.  This struct is defined as 
``ArenaCheckPoint`` in the .c file and ``ArenaCheckPointRep`` in the .h file.

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
The following data types are used in the implementation of the ``pool_t``
data type.

pool_t 
~~~~~~~
``pool_t`` is an opaque data structure that can not be directly accessed by a user.
This structure contains all of the metadata associated with a pool allocator in 
additon, this structure also contains a pointer to the ``arena_t`` structures 
which contain all memory allocations.  Metadata within this struct can be accessed 
through getter functions.

.. code-block:: c

   struct pool_t {
       arena_t* arena;          // Backing arena supplying memory (owned or borrowed)
       bool     owns_arena;     // true if this pool allocated and must destroy the arena
       size_t   block_size;     // User-requested block payload size (bytes)
       size_t   stride;         // Actual block size = block_size rounded up to required alignment
       size_t   blocks_per_chunk; // Number of blocks to allocate in each arena slice (growth granularity)
       uint8_t* cur;            // Pointer to next free byte in current arena slice (bump pointer)
       uint8_t* end;            // End of current arena slice (cur == end means next grow needed)
       void*    free_list;      // Head of intrusive free list (pointer stored in first word of freed blocks)
       size_t   total_blocks;   // Total number of blocks ever made available to this pool (including freed)
       size_t   free_blocks;    // Number of blocks currently in free_list (available to reuse)
       bool     grow_enabled;   // If false, pool cannot request new slices from arena (fixed-size mode)
   #ifdef DEBUG
       pool_slice_t* slices;    // Linked list of all memory slices obtained from arena (for debug verification)
   #endif
   };

PoolCheckPoint 
~~~~~~~~~~~~~~
``PoolCheckPoint`` is an opaque data structure that is used to store data 
related to a bump allocator.  The ``restore_pool`` function can extract the data from 
this structure to reconstitute a bump allocator.  This struct is defined as 
``PoolCheckPoint`` in the .c file and ``PoolCheckPointRep`` in the .h file.

.. code-block:: c 

   typedef struct {
       void*    free_list;      // Head of free list at checkpoint time
       size_t   free_blocks;    // Number of blocks in free list
       uint8_t* cur;            // Bump pointer position in current slice
       uint8_t* end;            // End of current slice at checkpoint time
       size_t   total_blocks;   // Total blocks available at checkpoint time
   #ifdef DEBUG
       pool_slice_t* slices;    // First slice at checkpoint time (for validation)
   #endif
   } PoolCheckpointRep;

Initialization and Memory Management 
------------------------------------
The following functions can be used to initialize a pool allocator 

init_pool_with_arena 
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_pool_with_arena
   :project: csalt

init_dynamic_pool
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_dynamic_pool
   :project: csalt

init_static_pool
~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_static_pool
   :project: csalt

alloc_pool 
~~~~~~~~~~

.. doxygenfunction:: alloc_pool
   :project: csalt

return_pool_element 
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: return_pool_element
   :project: csalt

reset_pool 
~~~~~~~~~~

.. doxygenfunction:: reset_pool
   :project: csalt

free_pool 
~~~~~~~~~

.. doxygenfunction:: free_pool
   :project: csalt

save_pool 
~~~~~~~~~

.. doxygenfunction:: save_pool
   :project: csalt

restore_pool 
~~~~~~~~~~~~

.. doxygenfunction:: restore_pool
   :project: csalt

pool_stats 
~~~~~~~~~~

.. doxygenfunction:: pool_stats
   :project: csalt

Gett and Setter Functions 
-------------------------
The following functions can be used to access attributes of the 
``pool_t`` data structure.

pool_block_size 
~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_block_size
   :project: csalt

pool_stride
~~~~~~~~~~~

.. doxygenfunction:: pool_stride
   :project: csalt

pool_total_blocks 
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_total_blocks
   :project: csalt

pool_free_blocks 
~~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_free_blocks
   :project: csalt

pool_alignment
~~~~~~~~~~~~~~

.. doxygenfunction:: pool_alignment
   :project: csalt

pool_bump_remaining_blocks
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_bump_remaining_blocks
   :project: csalt

pool_in_use_blocks 
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_in_use_blocks
   :project: csalt

pool_owns_arena 
~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_owns_arena 
   :project: csalt

pool_grow_enabled 
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_grow_enabled
   :project: csalt

pool_mtype 
~~~~~~~~~~

.. doxygenfunction:: pool_mtype
   :project: csalt

pool_size 
~~~~~~~~~

.. doxygenfunction:: pool_size
   :project: csalt

pool_alloc
~~~~~~~~~~

.. doxygenfunction:: pool_alloc
   :project: csalt

pool_footprint
~~~~~~~~~~~~~~

.. doxygenfunction:: pool_footprint
   :project: csalt

is_pool_ptr 
~~~~~~~~~~~

.. doxygenfunction:: is_pool_ptr
   :project: csalt

Function Like Macros 
--------------------

alloc_pool_type
~~~~~~~~~~~~~~~

.. doxygendefine:: alloc_pool_type
   :project: csalt
