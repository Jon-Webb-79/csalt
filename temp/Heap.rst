.. _heap:

*********************
Binary Heap
*********************

``c_array.h`` provides a generic, type-erased binary heap built on top of the
csalt allocator and dtype systems.  It is designed for priority-queue workloads
where the caller controls memory, element type, and heap ordering through a
comparator function.

Unlike :ref:`array`, this module does not provide primitive typed wrappers.
Instead, it is intended to serve as a foundation for user-defined priority
structures, where the caller declares the stored type via
:c:type:`dtype_id_t`, supplies a comparator, and builds thin wrappers around
the generic API.

Overview
--------

The heap stores values as a contiguous, type-erased byte buffer managed by
an embedded :c:type:`array_t`.  The heap property — every parent compares
at least as favourably as either of its children under the caller-supplied
comparator — is maintained after every push and pop.

Heap ordering is determined entirely by the comparator passed at
initialisation:

- A comparator returning ``> 0`` when ``*a > *b`` produces a **max-heap**
  (largest element at the root).
- A comparator returning ``> 0`` when ``*a < *b`` produces a **min-heap**
  (smallest element at the root).
- Any total ordering consistent with the ``qsort(3)`` convention is accepted,
  making custom priority schemes straightforward to express.

.. code-block:: c

   #include "c_array.h"

   static int cmp_int32_max(const void* a, const void* b) {
       int32_t va = *(const int32_t*)a;
       int32_t vb = *(const int32_t*)b;
       return (va > vb) - (va < vb);   /* max-heap */
   }

   allocator_vtable_t alloc = heap_allocator();

   heap_expect_t r = init_heap(16, INT32_TYPE, true, alloc, cmp_int32_max);
   if (!r.has_value) { /* handle r.u.error */ }

   heap_t* h = r.u.value;

   int32_t v = 42;
   heap_push(h, &v);

   int32_t top;
   heap_pop(h, &top);   /* top == 42 */

   return_heap(h);

.. note::

   The generic API performs no implicit type conversions.  The caller is
   responsible for ensuring that all inserted values match the declared
   ``dtype`` and that the comparator interprets element pointers consistently
   with the stored type.

Memory Model
------------

The heap wraps an :c:type:`array_t` internally:

.. code-block:: c

   typedef struct {
       array_t*   data;
       dtype_id_t dtype;
       int      (*cmp)(const void*, const void*);
   } heap_t;

The ``array_t`` owns a flat ``uint8_t*`` buffer that holds all elements
packed contiguously.  Element ``i`` is located at
``data->data + i * data->data_size``.  The heap_t struct itself is also
allocated through the caller-supplied allocator vtable, matching the same
pattern used by ``array_t``.

This layout provides:

- A single contiguous allocation for all elements
- No per-node pointer overhead
- Efficient cache access during sift operations, which traverse at most
  ``O(log n)`` elements along a root-to-leaf path

Growth Strategy
---------------

When ``growth`` is ``true`` and the backing buffer is full, it is
reallocated using the same tiered strategy as :c:type:`array_t`:

=========================================  =========================
Current capacity                           New capacity
=========================================  =========================
0                                          1
< 1 024                                    2×
1 024 – 8 191                              1.5× (current + current/2)
8 192 – 65 535                             1.25× (current + current/4)
≥ 65 536                                   current + 256
=========================================  =========================

When ``growth`` is ``false``, :c:func:`heap_push` returns
``CAPACITY_OVERFLOW`` once the initial capacity is exhausted and the heap is
left unmodified.

Ownership Rules
---------------

- The heap owns:

  - its ``heap_t`` struct
  - its embedded ``array_t`` struct
  - the element data buffer inside ``array_t``

- All memory is released via :c:func:`return_heap`
- The comparator function pointer is not owned; the caller must ensure it
  remains valid for the lifetime of the heap
- After :c:func:`return_heap` returns, the pointer must not be used

Allocator Integration
---------------------

All allocations are performed through :c:type:`allocator_vtable_t`.

This allows the heap to be backed by:

- heap allocators
- arena allocators
- pool allocators
- custom user-defined allocators

The heap does not assume ownership semantics beyond what is defined by the
allocator implementation.

See :ref:`allocator_file` for details.

Recommended Allocators
----------------------

Different allocators are appropriate depending on how the heap is used:

* **HeapAllocator**

  Best for:

  * general-purpose use
  * unit testing
  * host-side applications
  * debugging priority logic independent of allocator complexity

  This is the right allocator to start with because its failure modes are
  simple and deterministic.

* **ArenaAllocator**

  Best for:

  * build-then-discard workflows, such as a priority queue used during a
    single planning or scheduling pass
  * situations where the full heap lifetime matches a larger arena lifetime
  * cases where growth is disabled and capacity is known in advance

  Because all heap memory is contiguous in the backing array, arena
  allocation pairs especially well with fixed-capacity heaps.

* **BuddyAllocator**

  Best for:

  * deterministic dynamic allocation with growth enabled
  * environments where the heap may grow and later release memory through
    normal destruction
  * systems requiring more structure than a heap allocator

* **PoolAllocator / SlabAllocator**

  Less commonly the right choice for ``heap_t`` directly, because the
  backing buffer is a single variable-length allocation rather than a
  collection of uniform-size objects.  These allocators are better suited
  for typed wrapper structs that embed or point to a ``heap_t``.

Error Handling
--------------

The heap follows the csalt *expected value* pattern:

- Constructors (:c:func:`init_heap`, :c:func:`copy_heap`) return
  :c:type:`heap_expect_t`
- Mutating and iterating operations return :c:type:`error_code_t`
- Inspection operations (:c:func:`heap_peek`, :c:func:`heap_size`,
  :c:func:`heap_alloc`) return safe default values (``NULL`` or ``0``) when
  passed a ``NULL`` pointer

Callers must always check return values before use.

.. code-block:: c

   heap_expect_t r = init_heap(...);
   if (!r.has_value) {
       error_code_t err = r.u.error;
       /* handle error */
   }

   int32_t v = 10;
   error_code_t err = heap_push(h, &v);
   if (err != NO_ERROR) { /* handle error */ }

Type System and User-Defined Types
-----------------------------------

The heap relies on :c:type:`dtype_id_t` to identify the stored data type.
Built-in types are defined in ``c_dtypes.h``:

- ``INT32_TYPE``
- ``FLOAT_TYPE``
- ``DOUBLE_TYPE``
- ``STRING_TYPE``
- etc.

For user-defined types, you must define a unique type ID, describe the type
with a :c:type:`dtype_t` descriptor, register it with the dtype registry,
and then pass the ID to :c:func:`init_heap`.

Step 1 — Define the type and its ID
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Choose an ID at or above ``USER_BASE_TYPE`` that does not conflict with any
other registered ID in the application.  Define a :c:type:`dtype_t`
descriptor with the ID, element size, and a human-readable name used for
debugging.

.. code-block:: c

   #include "c_dtypes.h"

   /* A simple task entry for a priority queue */
   typedef struct {
       int      priority;
       uint64_t id;
       char     label[32];
   } task_t;

   #define TASK_TYPE (USER_BASE_TYPE + 1u)

   static const dtype_t task_desc = {
       TASK_TYPE,
       sizeof(task_t),
       "task_t"
   };

Step 2 — Register the type
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Call :c:func:`ensure_dtype_registered` before the first call to
:c:func:`init_heap`.  This function is idempotent — calling it multiple times
with the same descriptor is safe.

.. code-block:: c

   if (!ensure_dtype_registered(&task_desc)) {
       /* registration failed — registry may be full */
   }

.. note::

   User-defined dtype IDs must be ``>= USER_BASE_TYPE`` and unique within
   the application.  Define them as named macros rather than raw numeric
   values to make collisions easier to detect.  The registry supports up to
   ``MAX_DTYPES`` total entries (built-in and user-defined combined); call
   :c:func:`available_dtype_slots` to check capacity before registering if
   needed.

Step 3 — Write a comparator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The comparator follows the ``qsort(3)`` convention.  It receives two
``const void*`` pointers into the heap's element buffer and must return a
negative, zero, or positive integer.

.. code-block:: c

   /* Max-heap by priority field: highest priority at root */
   static int cmp_task_priority(const void* a, const void* b) {
       const task_t* ta = (const task_t*)a;
       const task_t* tb = (const task_t*)b;
       return (ta->priority > tb->priority) - (ta->priority < tb->priority);
   }

Step 4 — Initialise the heap
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pass the registered ``TASK_TYPE`` and the comparator to :c:func:`init_heap`.

.. code-block:: c

   allocator_vtable_t alloc = heap_allocator();

   heap_expect_t r = init_heap(64, TASK_TYPE, true, alloc, cmp_task_priority);
   if (!r.has_value) { /* handle r.u.error */ }

   heap_t* h = r.u.value;

Typed Wrapper Pattern (Recommended)
-------------------------------------

Because the generic API operates on ``void*`` buffers, it is strongly
recommended to define thin typed wrappers for your specific data types.
This eliminates accidental type errors at call sites with no runtime
overhead.

The wrapper pattern has three parts: an init wrapper that handles
registration, push/pop wrappers that cast to the concrete type, and a
foreach wrapper that accepts a typed callback.

Init wrapper
~~~~~~~~~~~~~

.. code-block:: c

   /* task_heap.h */

   typedef struct {
       int      priority;
       uint64_t id;
       char     label[32];
   } task_t;

   #define TASK_TYPE (USER_BASE_TYPE + 1u)

   /**
    * Initialise a max-heap of task_t ordered by priority.
    *
    * @param capacity  Initial element capacity.  Must be > 0.
    * @param growth    If true, the backing buffer grows automatically.
    * @param alloc_v   Allocator vtable for all memory operations.
    *
    * @return heap_expect_t with has_value true on success.
    *         On failure, has_value is false and u.error contains the code.
    */
   static inline heap_expect_t init_task_heap(size_t             capacity,
                                              bool               growth,
                                              allocator_vtable_t alloc_v) {
       static const dtype_t task_desc = { TASK_TYPE, sizeof(task_t), "task_t" };
       if (!ensure_dtype_registered(&task_desc))
           return (heap_expect_t){ .has_value = false,
                                   .u.error   = ILLEGAL_STATE };
       return init_heap(capacity, TASK_TYPE, growth, alloc_v,
                        cmp_task_priority);
   }

Push and pop wrappers
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   /**
    * Push a task_t onto the heap.
    *
    * @param h  Pointer to the heap.  Must not be NULL.
    * @param t  Pointer to the task to copy in.  Must not be NULL.
    * @return   NO_ERROR on success, or the error code from heap_push.
    */
   static inline error_code_t push_task(heap_t* h, const task_t* t) {
       return heap_push(h, t);
   }

   /**
    * Remove and retrieve the highest-priority task from the heap.
    *
    * @param h    Pointer to the heap.  Must not be NULL.
    * @param out  Buffer to receive the removed task.  Must not be NULL.
    *             Must be at least sizeof(task_t) bytes.
    * @return     NO_ERROR on success, EMPTY if the heap is empty,
    *             or NULL_POINTER if h or out is NULL.
    */
   static inline error_code_t pop_task(heap_t* h, task_t* out) {
       return heap_pop(h, out);
   }

   /**
    * Return a read-only pointer to the highest-priority task without
    * removing it.  The pointer is invalidated by the next push or pop.
    *
    * @param h  Pointer to the heap.  Must not be NULL.
    * @return   Pointer to the root task, or NULL if the heap is NULL or empty.
    */
   static inline const task_t* peek_task(const heap_t* h) {
       return (const task_t*)heap_peek(h);
   }

Foreach wrapper
~~~~~~~~~~~~~~~~

The ``ctx`` parameter is the key to making the generic
:c:func:`heap_foreach` useful from a typed wrapper.  It lets you pass any
state into the callback without globals or static variables — an
accumulator, a filter predicate, a file handle, or any other context your
application needs.

.. code-block:: c

   /**
    * Callback type for task_foreach.
    *
    * @param task  Pointer to the current task element.  Valid only for the
    *              duration of the call.  Do not store this pointer.
    * @param ctx   Opaque pointer forwarded from the task_foreach call.
    */
   typedef void (*task_visitor_fn)(const task_t* task, void* ctx);

   /* Adapter that casts the void* element to task_t* before invoking the
      typed visitor.  This is the only function that touches raw void*. */
   typedef struct {
       task_visitor_fn fn;
       void*           ctx;
   } _task_foreach_ctx_t;

   static void _task_foreach_adapter(const void* elem, void* ctx) {
       _task_foreach_ctx_t* c = (_task_foreach_ctx_t*)ctx;
       c->fn((const task_t*)elem, c->ctx);
   }

   /**
    * Visit every task in the heap in unspecified (heap-internal) order.
    *
    * The heap must not be mutated during iteration.
    *
    * @param h    Pointer to the heap.  Must not be NULL.
    * @param fn   Typed callback invoked once per element.  Must not be NULL.
    * @param ctx  Opaque pointer forwarded to every fn call.  May be NULL.
    * @return     NO_ERROR, NULL_POINTER, or EMPTY.
    */
   static inline error_code_t task_foreach(const heap_t*   h,
                                           task_visitor_fn fn,
                                           void*           ctx) {
       _task_foreach_ctx_t adapter_ctx = { fn, ctx };
       return heap_foreach(h, _task_foreach_adapter, &adapter_ctx);
   }

Complete usage example
~~~~~~~~~~~~~~~~~~~~~~~

The following example initialises a task priority queue, pushes several
tasks, inspects the root, iterates over all tasks, and then drains them in
priority order by popping from a copy.

.. code-block:: c

   #include "c_array.h"
   #include "c_allocator.h"
   #include <stdio.h>

   /* --- Type definition and registration (as above) --- */

   /* Callback: print each task during foreach */
   static void print_task(const task_t* t, void* ctx) {
       (void)ctx;
       printf("  id=%" PRIu64 "  priority=%d  label=%s\n",
              t->id, t->priority, t->label);
   }

   int main(void) {
       allocator_vtable_t alloc = heap_allocator();

       /* 1. Initialise */
       heap_expect_t r = init_task_heap(8, true, alloc);
       if (!r.has_value) return 1;
       heap_t* h = r.u.value;

       /* 2. Push tasks */
       task_t tasks[] = {
           { .priority = 3, .id = 1, .label = "low"    },
           { .priority = 9, .id = 2, .label = "urgent" },
           { .priority = 6, .id = 3, .label = "normal" },
           { .priority = 1, .id = 4, .label = "idle"   },
       };
       for (int i = 0; i < 4; i++)
           push_task(h, &tasks[i]);

       /* 3. Peek at the highest-priority task without removing it */
       const task_t* top = peek_task(h);
       printf("Root: %s (priority %d)\n", top->label, top->priority);

       /* 4. Iterate over all tasks in unspecified order */
       printf("All tasks (heap order):\n");
       task_foreach(h, print_task, NULL);

       /* 5. Drain in priority order by copying then popping */
       heap_expect_t cr = copy_heap(h, alloc);
       if (!cr.has_value) { return_heap(h); return 1; }
       heap_t* tmp = cr.u.value;

       printf("Tasks in priority order:\n");
       task_t out;
       while (heap_size(tmp) > 0u) {
           pop_task(tmp, &out);
           printf("  %s (priority %d)\n", out.label, out.priority);
       }

       /* 6. Clean up */
       return_heap(tmp);
       return_heap(h);
       return 0;
   }

This produces output similar to::

   Root: urgent (priority 9)
   All tasks (heap order):
     id=2  priority=9  label=urgent
     id=1  priority=3  label=low
     id=3  priority=6  label=normal
     id=4  priority=1  label=idle
   Tasks in priority order:
     urgent (priority 9)
     normal (priority 6)
     low (priority 3)
     idle (priority 1)

.. note::

   The heap-order traversal from ``task_foreach`` visits all elements but
   does not guarantee sorted output.  When ordered traversal is required,
   copy the heap with :c:func:`copy_heap` and drain the copy via
   :c:func:`heap_pop` as shown above.  This preserves the original heap
   intact while allowing a one-shot ordered pass at ``O(n log n)`` cost.

Core API
--------

Structs
~~~~~~~

.. doxygenstruct:: heap_t
   :members:

.. doxygenstruct:: heap_expect_t
   :members:

Initialisation and Teardown
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_heap

.. doxygenfunction:: return_heap

Mutation Operations
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: heap_push

.. doxygenfunction:: heap_pop

Inspection Operations
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: heap_peek

.. doxygenfunction:: heap_size

.. doxygenfunction:: heap_alloc

Iteration
~~~~~~~~~

.. doxygenfunction:: heap_foreach

Copy
~~~~

.. doxygenfunction:: copy_heap
