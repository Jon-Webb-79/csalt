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

.. _arena_overview:

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

It is also appropriate to use a single arena for program lifecycle 
memory managent if sufficient memory exists for the application.

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
This structure contains all of the metadata associated with a bump allocator, in 
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
       uint8_t owns_memory; // 0 if structure does not own memory, 1 otherwise
       uint8_t _pad[5];   // keep 8 byte passing
   } arena_t;

ArenaCheckPoint 
~~~~~~~~~~~~~~~
``ArenaCheckPoint`` is an opaque data structure that is used to store data 
related to a bump allocator.  The ``restore_arena()`` function can extract the data from 
this structure to reconstitute a bump allocator.  This struct is defined as 
``ArenaCheckPoint`` in the .c file and ``ArenaCheckPointRep`` in the .h file.

.. code-block:: c 

   typedef struct {
       Chunk* chunk;     // Pointer to saved memory chunk
       uint8_t* cur;     // Current pointer position in saved chunk
       size_t len;       // Length of saved chunk
   } ArenaCheckPointRep;

arena_expect_t 
~~~~~~~~~~~~~~
``arena_expect_t`` is an error handling struct to be used in the creation 
of ``arena_t`` data types to catch and convey errors to a user.

.. code-block:: c

   typedef struct {
       bool has_value;
       union {
           arena_t* value;
           ErrorCode error;
       } u;
   } arena_expect_t;

Initialization and Memory Management
------------------------------------
The functions in this section can be used to initialize memory for a bump allocator,
parse that memory to variables and to deallocate the memory. The csalt library restricts 
the user to a maximum heap allocation of 16 MB per chunk, which is sufficient for most  
applications.  Larger memory allocations for engineering calculations (i.e. 1400 x 1400 matrices)
will require a different custom allocator.

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

init_arena_with_arena
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_arena_with_arena
   :project: csalt

init_arena_with_buddy 
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_arena_with_buddy
   :project: csalt

return_arena_with_buddy 
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: return_arena_with_buddy
   :project: csalt

free_arena
~~~~~~~~~~

.. doxygenfunction:: free_arena
   :project: csalt

alloc_arena 
~~~~~~~~~~~

.. doxygenfunction:: alloc_arena
   :project: csalt

realloc_arena
~~~~~~~~~~~~~

.. doxygenfunction:: realloc_arena
   :project: csalt

alloc_arena_aligned
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: alloc_arena_aligned
   :project: csalt

realloc_arena_aligned
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: realloc_arena_aligned
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

arena_owns_memory 
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: arena_owns_memory
   :project: csalt

toggle_arena_resize 
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: toggle_arena_resize
   :project: csalt

Arena Context Functions
-----------------------

The following functions provide the arena-backed implementation of the
allocator vtable interface. They adapt an ``arena_t`` instance to the generic
allocator API by exposing allocation, reallocation, and bulk deallocation
operations in a consistent form.

Each function corresponds to one of the prototypes defined in the
allocator context system (see :ref:`context`), and is used
internally by the ``arena_allocator()`` helper to construct a fully
configured ``allocator_vtable_t`` for arena-based allocation.

These functions are not intended to be called directly by user code;
instead, they are used through the generic allocator interface.

arena_v_alloc
~~~~~~~~~~~~~

.. doxygenfunction:: arena_v_alloc
   :project: csalt

arena_v_alloc_aligned
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: arena_v_alloc_aligned
   :project: csalt

arena_v_realloc
~~~~~~~~~~~~~~~

.. doxygenfunction:: arena_v_realloc
   :project: csalt

arena_v_realloc_aligned
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: arena_v_realloc_aligned
   :project: csalt

arena_v_return
~~~~~~~~~~~~~~

.. doxygenfunction:: arena_v_return
   :project: csalt

arena_v_free
~~~~~~~~~~~~

.. doxygenfunction:: arena_v_free
   :project: csalt

Pool Allocator Overview
=======================

The ``pool_t`` allocator implements a *fixed-size block* allocation model.
Each pool manages memory divided into uniformly sized blocks, enabling
extremely fast allocation and deallocation of objects of the same size.
Pools may be backed by either static or dynamic arenas, and may optionally
grow by acquiring additional slices of memory from a dynamic arena.

This model is ideal when:

* many objects of identical size are frequently allocated and freed
* allocation and deallocation must be deterministic and extremely fast
* fragmentation must be minimized or eliminated
* objects may be recycled using an intrusive free-list

Examples include:

* ECS (entity–component–system) component storage
* job-system nodes, task descriptors, or scheduler queues
* physics or graphics engine object caches
* network packet or frame metadata
* high-frequency scientific workloads with uniform object shapes

Benefits
--------

* **Constant-time allocation and free** via an intrusive LIFO free-list
* **No fragmentation**, since all blocks share the same size and alignment
* **High locality**, as blocks are carved contiguously from arena slices
* **Optional reuse of memory** via ``return_pool_element()``
* **Optional growth** when backed by a dynamic arena
* **No external malloc/free** — all pool metadata resides inside the arena

Limitations
-----------

* Block size is fixed at initialization; allocations must fit within ``block_size``
* Pools do not support resizing of individual allocations
* Dynamically growing pools require a dynamic arena with adequate space
* Static pools have fixed capacity and cannot grow
* Individual blocks cannot be freed to the system; reclamation occurs only when
  the entire pool is destroyed via ``free_pool()``

Static vs. Dynamic Pools
------------------------

Pools may be constructed in one of two modes:

* **Static pool** — backed by an ``arena_t`` created with ``init_static_arena()``.
  Capacity is determined entirely by the caller-provided buffer. Growth is
  permanently disabled, and the pool always prewarms a single contiguous slice
  during initialization.

* **Dynamic pool** — backed by an ``arena_t`` created with ``init_dynamic_arena()``.
  Additional slices may be allocated on demand as long as the arena’s growth
  policy allows it. Growth can be toggled at runtime using
  ``toggle_pool_growth()``.

Both pool types share the same opaque ``pool_t`` structure and the same API.
When the build is configured with ``STATIC_ONLY``, dynamic features are omitted
completely, ensuring compliance with safety-critical environments that forbid
hidden heap allocation paths.

``pool_t`` provides deterministic, high-performance allocation patterns well
suited to real-time and cache-sensitive applications, especially when paired
with ``arena_t`` for higher-level memory partitioning.

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

pool_expect_t 
~~~~~~~~~~~~~~
``pool_expect_t`` is an error handling struct to be used in the creation 
of ``pool_t`` data types to catch and convey errors to a user.

.. code-block:: c

   typedef struct {
       bool has_value;
       union {
           pool_t* value;
           ErrorCode error;
       } u;
   } pool_expect_t;

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

pool_owns_memory 
~~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_owns_memory
   :project: csalt

Function Like Macros 
--------------------

alloc_pool_type
~~~~~~~~~~~~~~~

.. doxygendefine:: alloc_pool_type
   :project: csalt

Pool Context Functions
----------------------

The following functions provide the pool-backed implementation of the
allocator vtable interface. They adapt a ``pool_t`` instance to the generic
allocator API by exposing allocation, deallocation, and element-return
operations in a consistent form.

Each function corresponds to one of the prototypes defined in the
allocator context system (see :ref:`context`), and is used
internally by the ``pool_allocator()`` helper to construct a fully
configured ``allocator_vtable_t`` for pool-based allocation.

These functions are not intended to be called directly by user code;
instead, they are used through the generic allocator interface.

pool_v_alloc
~~~~~~~~~~~~

.. doxygenfunction:: pool_v_alloc
   :project: csalt

pool_v_alloc_aligned
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_v_alloc_aligned
   :project: csalt

pool_v_realloc
~~~~~~~~~~~~~~

.. doxygenfunction:: pool_v_realloc
   :project: csalt

pool_v_realloc_aligned
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pool_v_realloc_aligned
   :project: csalt

pool_v_return
~~~~~~~~~~~~~

.. doxygenfunction:: pool_v_return
   :project: csalt

pool_v_free
~~~~~~~~~~~

.. doxygenfunction:: pool_v_free
   :project: csalt

Freelist Overview
=================

The ``freelist_t`` allocator implements a *returnable-block* allocation model.
Unlike the :ref:`arena_t <arena_overview>` bump allocator—where memory is
only reclaimed via a full reset—the freelist supports **individual frees**
while still maintaining high performance and predictable behavior.

Memory is managed as a contiguous region divided into variable-sized blocks.
Each block is either:

* **allocated**, containing a header that records its total size and internal
  alignment offset, or
* **free**, tracked in a singly linked free list ordered by address.

This layout allows the allocator to return memory to the free list and
efficiently coalesce adjacent free blocks, reducing external fragmentation.

When to Use a Freelist
----------------------

The freelist allocator is ideal when:

* allocations and deallocations are intermixed
* memory reuse is essential
* predictable fragmentation behavior is required
* you want deterministic, bounded allocation times without relying on ``malloc``

Common use cases:

* transient or reusable object pools  
* high-frequency allocation patterns in game or simulation engines  
* embedded systems needing predictable memory footprints  
* dynamic buffers inside larger memory arenas  

Benefits
--------

* **Individual deallocation** — Unlike an arena, objects can be returned and reused.
* **Predictable performance** — First-fit allocation runs in bounded linear time.
* **Coalescing** — Adjacent free blocks automatically merge, limiting external fragmentation.
* **Contiguous memory region** — No per-allocation heap calls, no hidden system overhead.
* **Compatible with arenas** — Freelist can be layered atop an ``arena_t`` for full stack control.

Limitations
-----------

* **Potential fragmentation**  
  The freelist updates internal block boundaries dynamically. Fragmentation may occur, though automatic coalescing keeps it bounded.

* **Internal fragmentation**  
  Every allocation includes a small header that stores:
  
  * total block size  
  * user pointer offset (for alignment)  

  This means each allocation consumes slightly more memory than requested.
  Alignments > struct alignment may increase padding.

* **External fragmentation**  
  When free blocks cannot be merged—because allocated blocks lie between them—the available space becomes split across multiple regions. This is inherent to any free-list allocator.

* **More overhead than an arena**  
  Allocation is slower than bump allocation, though significantly faster than ``malloc``.

Static vs. Dynamic Freelist
---------------------------

Two initialization paths exist:

* **Static freelist** — wraps a user-provided buffer  
* **Dynamic freelist** — allocates its own backing ``arena_t``

Dynamic freelists are available only when the build is compiled with:

.. code-block:: c

   ARENA_ENABLE_DYNAMIC = 1

This ensures MISRA-compliant builds (``STATIC_ONLY``) completely exclude heap usage.

Reset Behavior
--------------

Calling ``reset_freelist(fl)`` restores the allocator to its initial state:

* all blocks become free  
* the free list is replaced by one large free block  
* accounting returns to zero  

This mirrors the arena’s ``reset_arena()``, but the freelist preserves its
ability to perform individual allocations afterward.

Data Types 
----------
The following are data structures and derived data types used in the ``c_allocator.h``
and ``c_allocator.c`` files to support the ``freelist_t`` data type.

freelist_t 
~~~~~~~~~~
The ``freelist_t`` data type is the fundamental structure that enables the 
freelist allocator.

.. code-block:: c 

   struct freelist_t {
       free_block_t* head;     // Head of free list - accessed first in alloc
       uint8_t*      cur;      // High-water mark - updated on alloc
       size_t        len;      // Current usage - updated on alloc/free
       size_t        alignment;// Checked on every alloc
       void*         memory;   // Start of memory region (for reset/bounds checking)
       size_t        alloc;    // Total usable memory
       size_t        tot_alloc;// Total including overhead
       arena_t*      parent_arena;  // Parent arena reference
       bool          owns_memory;   // Ownership flag
       uint8_t       _pad[7];       // Explicit padding to 8-byte boundary
   };

freelist_header_t 
~~~~~~~~~~~~~~~~~
The ``freelist_header_t`` structure provides data at the beginning of an 
allocated memory block that enables variable alignment per allocation.

.. code-block:: c

   typedef struct freelist_header {
       size_t block_size;  // total size of the allocated block (from block_start)
       size_t offset;      // (uint8_t*)user_ptr - (uint8_t*)block_start
   } freelist_header_t;

free_block_t 
~~~~~~~~~~~~
The ``free_block_t`` data structure is the basis for a non-intrusive pointer 
that enables a link list to tracked returned memory.

.. code-block:: c 

   typedef struct free_block {
       size_t size;
       struct free_block* next;
   } free_block_t;

freelist_expect_t 
~~~~~~~~~~~~~~~~~
``freelist_expect_t`` is an error handling struct to be used in the creation 
of ``freelist_t`` data types to catch and convey errors to a user.

.. code-block:: c

   typedef struct {
       bool has_value;
       union {
           freelist_t* value;
           ErrorCode error;
       } u;
   } freelist_expect_t;

Initialization and Memory Management
------------------------------------
The functions in this section can be used to initialize memory for a free list allocator,
parse that memory to variables and to deallocate the memory.

init_freelist_with_arena 
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_freelist_with_arena
   :project: csalt

init_static_freelist
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_static_freelist
   :project: csalt

init_dynamic_freelist
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_dynamic_freelist
   :project: csalt

free_freelist
~~~~~~~~~~~~~

.. doxygenfunction:: free_freelist
   :project: csalt

alloc_freelist
~~~~~~~~~~~~~~

.. doxygenfunction:: alloc_freelist
   :project: csalt

realloc_freelist
~~~~~~~~~~~~~~~~

.. doxygenfunction:: realloc_freelist
   :project: csalt

alloc_freelist_aligned
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: alloc_freelist_aligned
   :project: csalt

realloc_freelist_aligned
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: realloc_freelist_aligned
   :project: csalt 

return_freelist
~~~~~~~~~~~~~~~

.. doxygenfunction:: reset_freelist
   :project: csalt

return_freelist_element
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: return_freelist_element
   :project: csalt

Utility Funcitons 
-----------------

is_freelist_ptr
~~~~~~~~~~~~~~~

.. doxygenfunction:: is_freelist_ptr
   :project: csalt

is_freelist_ptr_sized
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: is_freelist_ptr_sized
   :project: csalt

freelist_stats 
~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_stats
   :project: csalt

Getter and Setter Functions 
---------------------------

freelist_remaining
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_remaining
   :project: csalt

freelist_mtype
~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_mtype
   :project: csalt

freelist_size
~~~~~~~~~~~~~

.. doxygenfunction:: freelist_size
   :project: csalt

freelist_alloc
~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_alloc
   :project: csalt

total_freelist_alloc
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: total_freelist_alloc
   :project: csalt

freelist_alignment
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_alignment
   :project: csalt

min_freelist_alloc
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: min_freelist_alloc
   :project: csalt

.. _context:

Freelist Context Functions
--------------------------

The following functions provide the freelist-backed implementation of the
allocator vtable interface. They adapt an ``freelist_t`` instance to the generic
allocator API by exposing allocation, reallocation, and bulk deallocation
operations in a consistent form.

freelist_v_alloc
~~~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_v_alloc
   :project: csalt

freelist_v_alloc_aligned
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_v_alloc_aligned
   :project: csalt

freelist_v_realloc
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_v_realloc
   :project: csalt

freelist_v_realloc_aligned
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_v_realloc_aligned
   :project: csalt

freelist_v_return
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_v_return
   :project: csalt

freelist_v_free
~~~~~~~~~~~~~~~

.. doxygenfunction:: freelist_v_free
   :project: csalt

Buddy Allocator Overview
========================

The ``buddy_t`` allocator implements a classic *binary buddy allocation* scheme.
Memory is acquired as a single, power-of-two–sized pool, and all allocations are
satisfied by recursively subdividing blocks into halves ("buddies") until the
smallest usable block size is reached. Freed blocks are efficiently coalesced
with their buddies to restore larger contiguous blocks.

This model provides a balance between **fast allocation**, **bounded
fragmentation**, and the ability to **free individual blocks**, while retaining
predictable block sizes and layout.

The buddy system is ideal when:

* the allocator must handle **arbitrary allocation and free patterns**
* **fragmentation control** is important
* memory needs can vary dynamically within a bounded pool
* allocations benefit from **power-of-two block sizing** and **alignment guarantees**
* the application requires **deterministic performance** for alloc/free

Examples include:

* custom memory managers in real-time or embedded software
* scratch allocators for physics engines, audio engines, or network buffers
* fixed-size arenas that must support both allocation and deallocation
* game engines and simulation systems where fragmentation must remain bounded
* OS kernels (many use buddy systems internally for page-frame allocation)

Benefits
--------

* **Fast allocation and deallocation**  
  Typically O(log N), often near O(1) due to shallow free lists.

* **Automatic coalescing**  
  When a block is freed, its buddy is detected and merged, restoring larger
  blocks and reducing fragmentation.

* **Bounded fragmentation**  
  Fragmentation is limited to internal power-of-two rounding, rather than the
  arbitrary fragmentation of general-purpose heaps.

* **Predictable memory layout**  
  Block sizes and boundaries follow a binary tree structure, enabling more
  deterministic and cache-friendly memory usage.

* **Strong alignment guarantees**  
  Power-of-two block sizes combined with ``base_align`` ensure that returned
  pointers meet strict alignment requirements.

Limitations
-----------

* **Internal fragmentation**  
  Each allocation is rounded up to the next power of two.

* **Fixed pool size**  
  The buddy allocator does not grow once created; the pool size is fixed.

* **Overhead for many small allocations**  
  Very small objects may occupy larger blocks depending on ``min_block_size``.

* **More complex than an arena allocator**  
  Allocation and freeing may be logarithmic and require block splitting and
  coalescing.

* **Not ideal for large numbers of long-lived objects**  
  Internal fragmentation can accumulate if most allocations hover near
  power-of-two boundaries.

Data Types 
----------
The following are data structures and derived data types used in the ``c_allocator.h``
and ``c_allocator.c`` files to support the ``buddy_t`` data type.

buddy_t
~~~~~~~
``buddy_t`` is an opaque data structure that is not visibile to the user.  This struct 
contains metadata on a chunk of allocated memory.

.. code-block:: c

   struct buddy_t {
       void           *base;         /* Base address of the OS-allocated pool */
       buddy_block_t **free_lists;   /* Array of free-list heads (one per order level) */
       size_t          pool_size;    /* Total pool size in bytes (power of two) */
       size_t          len;          /* Total bytes currently consumed (block sizes) */
       size_t          alloc;        /* Total usable pool bytes (== pool_size) */
       size_t          total_alloc;  /* Total memory including pool + metadata overhead */
       size_t          base_align;   /* Required alignment for returned user pointers */
       size_t          user_offset;  /* Byte offset from block start to aligned user data */
       uint32_t        min_order;    /* log2(minimum block size) */
       uint32_t        max_order;    /* log2(pool size) */
       uint32_t        num_levels;   /* Number of free-list levels (block orders) */
       uint8_t         _pad[4];      /* Pad struct to maintain 8-byte alignment */
   };

The ``buddy_t`` data type uses the following structs.

.. code-block:: c 

   typedef struct buddy_block {
       struct buddy_block *next;
   } buddy_block_t;

   typedef struct buddy_header {
       uint32_t order;   /* log2(block_size) */
       size_t   block_offset;
   } buddy_header_t;

buddy_expect_t 
~~~~~~~~~~~~~~
``buddy_expect_t`` is an error handling struct to be used in the creation 
of ``buddy_t`` data types to catch and convey errors to a user.

.. code-block:: c

   typedef struct {
       bool has_value;
       union {
           buddy_t* value;
           ErrorCode error;
       } u;
   } buddy_expect_t;

Initialization and Memory Management
------------------------------------
The functions in this section can be used to initialize memory for a bump allocator,
parse that memory to variables and to deallocate the memory.

init_buddy_allocator
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_buddy_allocator
   :project: csalt

free_buddy
~~~~~~~~~~

.. doxygenfunction:: free_buddy
   :project: csalt

alloc_buddy
~~~~~~~~~~~

.. doxygenfunction:: alloc_buddy
   :project: csalt

realloc_buddy
~~~~~~~~~~~~~

.. doxygenfunction:: realloc_buddy
   :project: csalt

return_buddy_element 
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: return_buddy_element
   :project: csalt

alloc_buddy_aligned
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: alloc_buddy_aligned
   :project: csalt

reset_buddy
~~~~~~~~~~~

.. doxygenfunction:: reset_buddy
   :project: csalt

realloc_buddy_aligned
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: realloc_buddy_aligned
   :project: csalt

Utility Funcitons 
-----------------

is_buddy_ptr
~~~~~~~~~~~~

.. doxygenfunction:: is_buddy_ptr
   :project: csalt

is_buddy_ptr_sized
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: is_buddy_ptr_sized
   :project: csalt

buddy_stats
~~~~~~~~~~~

.. doxygenfunction:: buddy_stats
   :project: csalt

Getter and Setter Functions 
---------------------------

buddy_remaining
~~~~~~~~~~~~~~~

.. doxygenfunction:: buddy_remaining
   :project: csalt

buddy_size
~~~~~~~~~~

.. doxygenfunction:: buddy_size
   :project: csalt

buddy_alloc
~~~~~~~~~~~

.. doxygenfunction:: buddy_alloc
   :project: csalt

total_buddy_alloc
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: total_buddy_alloc
   :project: csalt

buddy_alignment
~~~~~~~~~~~~~~~

.. doxygenfunction:: buddy_alignment
   :project: csalt

buddy_largest_block
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: buddy_largest_block
   :project: csalt

Buddy Context Functions
-----------------------

The following functions provide the arena-backed implementation of the
allocator vtable interface. They adapt an ``buddy_t`` instance to the generic
allocator API by exposing allocation, reallocation, and bulk deallocation
operations in a consistent form.

buddy_v_alloc
~~~~~~~~~~~~~

.. doxygenfunction:: buddy_v_alloc
   :project: csalt

buddy_v_alloc_aligned
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: buddy_v_alloc_aligned
   :project: csalt

buddy_v_realloc
~~~~~~~~~~~~~~~

.. doxygenfunction:: buddy_v_realloc
   :project: csalt

buddy_v_realloc_aligned
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: buddy_v_realloc_aligned
   :project: csalt

buddy_v_return
~~~~~~~~~~~~~~

.. doxygenfunction:: buddy_v_return
   :project: csalt

buddy_v_free
~~~~~~~~~~~~

.. doxygenfunction:: buddy_v_free
   :project: csalt

Slab Allocator Overview
=======================
The ``slab_t`` allocator implements a *fixed-size object* memory allocation
model. Memory is acquired from an underlying allocator (typically a
``buddy_t`` allocator) in page-sized chunks called *slabs*. Each slab is
subdivided into uniformly-sized slots, and allocations simply return the next
available slot.

This design provides fast, predictable memory behavior for workloads where
objects have the same size and are frequently allocated and freed.

This model is ideal when:

* many objects share the **same fixed size**
* allocation and deallocation occur **frequently**
* high throughput and predictable memory reuse are required
* fragmentation must be minimized

Examples include:

* kernel-style data structure allocation (inspired by the Linux slab allocator)
* fixed-size request/response messaging buffers
* game engine entities or component storage pools
* allocator back-ends for frameworks with small, frequently-used structs
* real-time or embedded systems where ``malloc``/``free`` overhead is undesirable

Benefits
--------

* **Very fast allocation and free**  
  (constant time; usually a pointer pop/push from a free-list)
* **No external fragmentation**, since all slots are equal size
* **Cache-friendly layout**  
  (objects of the same type are tightly packed in memory)
* **Stable pointer addresses** while slabs remain allocated
* **Deterministic memory usage** for a class of fixed-size objects

Limitations
-----------

* Supports **fixed-size** allocations only  
  (all objects must fit within one slot)
* **Reallocation is not supported**  
  (slots cannot grow; callers must allocate/copy/free manually)
* May exhibit **internal fragmentation** if objects do not use the entire slot
* Memory consumption grows in units of full slabs (pages)

Data Types 
----------
The following are data structures and derived data types used in the ``c_allocator.h``
and ``c_allocator.c`` files to support the ``arena_t`` data type.

slab_t
~~~~~~
``slab_t`` is an opaque data structure that is not visibile to the user.  This struct 
contains metadata for the slab allocator type.

.. code-block:: c

   typedef struct slab_t {
       buddy_t *buddy;        /* backing buddy allocator */
       size_t   obj_size;     /* user-visible object size */
       size_t   slot_size;    /* internal slot size (>= obj_size and >= sizeof(slab_slot_t)) */
       size_t   align;        /* slot alignment (power of two) */
       size_t   slab_bytes;   /* total bytes in each slab page (buddy allocation size we carve up) */
       size_t   page_hdr_bytes;/* bytes reserved for slab_page header (aligned) */
       size_t   objs_per_slab;/* slots per page */
       size_t   len;          /* bytes currently in-use by this slab (obj_size * live objects) */
       slab_page_t *pages;    /* linked list of pages (headers sit at page base) */
       slab_slot_t *free_list;/* global free-list of free slots */
   } slab_t;

slab_expect_t 
~~~~~~~~~~~~~
``slab_expect_t`` is an error handling struct to be used in the creation 
of ``slab_t`` data types to catch and convey errors to a user.

.. code-block:: c

   typedef struct {
       bool has_value;
       union {
           slab_t* value;
           ErrorCode error;
       } u;
   } slab_expect_t;

Initialization and Memory Management
------------------------------------
The functions in this section can be used to initialize memory for a slab allocator,
parse that memory to variables.

init_slab_allocator
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_slab_allocator
   :project: csalt

alloc_slab
~~~~~~~~~~

.. doxygenfunction:: alloc_slab
   :project: csalt

return_slab
~~~~~~~~~~~

.. doxygenfunction:: return_slab
   :project: csalt

Utility Funcitons 
-----------------

is_slab_ptr
~~~~~~~~~~~

.. doxygenfunction:: is_slab_ptr
   :project: csalt

reset_slab
~~~~~~~~~~

.. doxygenfunction:: reset_slab
   :project: csalt

save_slab
~~~~~~~~~

.. doxygenfunction:: save_slab
   :project: csalt

restore_slab
~~~~~~~~~~~~~

.. doxygenfunction:: restore_slab
   :project: csalt

slab_stats 
~~~~~~~~~~~

.. doxygenfunction:: slab_stats
   :project: csalt

Getter and Setter Functions 
---------------------------

slab_size
~~~~~~~~~

.. doxygenfunction:: slab_size
   :project: csalt

slab_alloc
~~~~~~~~~~

.. doxygenfunction:: slab_alloc
   :project: csalt

total_slab_alloc
~~~~~~~~~~~~~~~~

.. doxygenfunction:: total_slab_alloc
   :project: csalt

slab_stride
~~~~~~~~~~~

.. doxygenfunction:: slab_stride
   :project: csalt

slab_total_blocks
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: slab_total_blocks
   :project: csalt

slab_free_blocks
~~~~~~~~~~~~~~~~

.. doxygenfunction:: slab_free_blocks
   :project: csalt

slab_in_use_blocks
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: slab_in_use_blocks
   :project: csalt

slab_alignment
~~~~~~~~~~~~~~~

.. doxygenfunction:: slab_alignment
   :project: csalt


Slab Context Functions
----------------------

The following functions provide the arena-backed implementation of the
allocator vtable interface. They adapt an ``slab_t`` instance to the generic
allocator API by exposing allocation, reallocation, and bulk deallocation
operations in a consistent form.

slab_v_alloc
~~~~~~~~~~~~

.. doxygenfunction:: slab_v_alloc
   :project: csalt

slab_v_alloc_aligned
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: slab_v_alloc_aligned
   :project: csalt

slab_v_realloc
~~~~~~~~~~~~~~

.. doxygenfunction:: slab_v_realloc
   :project: csalt

slab_v_realloc_aligned
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: slab_v_realloc_aligned
   :project: csalt

slab_v_return
~~~~~~~~~~~~~

.. doxygenfunction:: slab_v_return
   :project: csalt

slab_v_free
~~~~~~~~~~~

.. doxygenfunction:: slab_v_free
   :project: csalt

slab_allocator 
~~~~~~~~~~~~~~

.. doxygenfunction:: slab_allocator
   :project: csalt

Context Function Pointers
=========================

The CSALT allocator framework supports multiple allocation backends
(arena, pool, dynamic arena, system heap, etc.).  
To provide a uniform, "drop-in" interface for all allocator types,
the library defines a generic context-based dispatch table.  
This table contains function pointers corresponding to the fundamental
operations that any allocator must implement.

These function pointers are grouped into a single structure,
``allocator_vtable_t``, which acts as a virtual method table (vtable)
for memory management. Each allocator fills in the appropriate
function pointers and provides an opaque ``ctx`` pointer referencing
its internal state (arena instance, pool instance, system-heap adapter,
and so on).

.. code-block:: c

   typedef struct {
       alloc_prototype           allocate;
       alloc_aligned_prototype   allocate_aligned;
       realloc_prototype         reallocate;
       realloc_aligned_prototype reallocate_aligned;
       return_prototype          return_element;
       free_prototype            deallocate;
       void*                     ctx;      // backing arena, pool, system heap, etc.
   } allocator_vtable_t;

The following sections document each function-pointer type in detail.

alloc_prototype
---------------

.. doxygentypedef:: alloc_prototype
   :project: csalt

alloc_aligned_prototype
-----------------------

.. doxygentypedef:: alloc_aligned_prototype
   :project: csalt

realloc_prototype
-----------------

.. doxygentypedef:: realloc_prototype
   :project: csalt

realloc_aligned_prototype
-------------------------

.. doxygentypedef:: realloc_aligned_prototype
   :project: csalt

return_prototype
----------------

.. doxygentypedef:: return_prototype
   :project: csalt

free_prototype
--------------

.. doxygentypedef:: free_prototype
   :project: csalt

Standard Allocator Implementation 
---------------------------------
The ``allocator_vtable_t`` type is meant for robust custom allocators but it 
can also be used for standard ``malloc``, ``realloc`` and ``free`` functions that 
are part of the standard C library.

v_alloc 
~~~~~~~

.. doxygenfunction:: v_alloc
   :project: csalt

v_alloc_aligned 
~~~~~~~~~~~~~~~

.. doxygenfunction:: v_alloc_aligned
   :project: csalt

v_realloc 
~~~~~~~~~

.. doxygenfunction:: v_realloc
   :project: csalt

v_realloc_aligned 
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: v_realloc_aligned
   :project: csalt

v_return 
~~~~~~~~

.. doxygenfunction:: v_return
   :project: csalt

v_free 
~~~~~~

.. doxygenfunction:: v_free
   :project: csalt

malloc_allocator 
~~~~~~~~~~~~~~~~

.. doxygenfunction:: malloc_allocator
   :project: csalt

