.. _vector_file:

****************
C Float Overview 
****************

c_float
=======

A float vector provides both dynamic and static array containers for float values, offering
efficient management of numerical data with automatic memory handling for dynamic arrays
and safe access for static arrays. This implementation combines the flexibility of dynamic
arrays with the safety of bounds checking and proper memory management.  All of the functions 
described in this section can be found in the ``c_float.h`` header file.

Data Types
==========

The following are derived data types used in the implementation of float vectors.

float_v
-------
The float vector type that can manage either dynamically or statically allocated float arrays
with automatic bounds checking and memory management.

.. code-block:: c

   typedef struct {
       float* data;
       size_t len;
       size_t alloc;
       alloc_t alloc_type;
   } float_v;

Core Functions
==============

The c_float library provides both dynamically and statically allocated float arrays through
the float_v type. All of the functions described in this section can be found in the
``c_float.h`` header file.

Initialization and Memory Management
------------------------------------

The functions and Macros in this section are used to control the creation,
memory allocation, and specific destruction of ``float_v`` data types.

init_float_vector
~~~~~~~~~~~~~~~~~
.. c:function:: float_v* init_float_vector(size_t buffer)

   Initializes a new dynamically allocated float vector with specified initial capacity.
   The vector will automatically grow if needed when adding elements.

   :param buffer: Initial capacity (number of floats) to allocate
   :returns: Pointer to new float_v object, or NULL on allocation failure
   :raises: Sets errno to EINVAL if buffer is 0, ENOMEM if memory allocation fails

   Example:

   .. code-block:: c

      // Create vector with initial capacity for 10 floats
      float_v* vec = init_float_vector(10);
      if (!vec) {
          fprintf(stderr, "Failed to initialize vector\\n");
          return 1;
      }
      
      // Free when done
      free_float_vector(vec);

init_float_array
~~~~~~~~~~~~~~~~
.. c:macro:: init_float_array(size)

   Creates a statically allocated float array wrapped in a float_v structure.
   Must be used at variable declaration.

   :param size: Size of the static array to create
   :returns: float_v structure containing the static array

   Example:

   .. code-block:: c

      // Create static array of 10 floats
      float_v arr = init_float_array(10);
      
      // No need to free - automatically cleaned up when out of scope

free_float_vector
~~~~~~~~~~~~~~~~~
.. c:function:: void free_float_vector(float_v* vec)

   Frees all memory associated with a dynamically allocated float vector.
   This function should only be used with vectors created by init_float_vector().
   For vectors created with init_float_array(), this function will return an error.
   If using a ``gcc`` or ``clang`` compiler, consider using the FLTVEC_GBC macro
   instead.

   :param vec: Float vector to free
   :raises: Sets errno to EINVAL if vec is NULL or if attempting to free a static array

   Example:

   .. code-block:: c

      float_v* vec = init_float_vector(5);
      
      // Use the vector...
      
      // Free the vector when done
      free_float_vector(vec);
      vec = NULL;  // Good practice to avoid dangling pointer

.. warning::

   The free_float_vector() function should only be used with dynamically allocated
   vectors created by init_float_vector(). Attempting to free a static

trim_float_vector
~~~~~~~~~~~~~~~~~
.. c:function:: void trim_float_vector(float_v* vec)

   Reduces the allocated memory of a float vector to match its current size,
   eliminating any unused capacity. This operation has no effect on static arrays
   or vectors that are already at optimal capacity.

   :param vec: Target float vector
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty,
           ERANGE for size_t overflow, ENOMEM if reallocation fails

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(10);  // Allocate space for 10 floats
      
      // Add 3 values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      printf("Before trim: size = %zu, capacity = %zu\n", 
             f_size(vec), f_alloc(vec));
      
      trim_float_vector(vec);
      
      printf("After trim:  size = %zu, capacity = %zu\n", 
             f_size(vec), f_alloc(vec));
      
   Output::

      Before trim: size = 3, capacity = 10
      After trim:  size = 3, capacity = 3

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(5);
      
      // Add some values
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      
      printf("Before trim: size = %zu, capacity = %zu\n", 
             f_size(&arr), f_alloc(&arr));
      
      trim_float_vector(&arr);  // No effect on static arrays
      
      printf("After trim:  size = %zu, capacity = %zu\n", 
             f_size(&arr), f_alloc(&arr));

   Output::

      Before trim: size = 2, capacity = 5
      After trim:  size = 2, capacity = 5

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Sets errno to EINVAL
     - Returns without modification
   
   * If vector is empty:
     - Sets errno to ENODATA
     - Returns without modification
   
   * If memory reallocation fails:
     - Sets errno to ENOMEM
     - Returns without modification
     - Original vector remains unchanged

   The following conditions result in no modification and no error:

   * Static arrays (alloc_type == STATIC)
   * Vectors where capacity equals size
   
   .. note::

      This function is useful for reclaiming unused memory in vectors that have
      shrunk significantly from their peak size. However, frequent trimming
      can be counterproductive if the vector size fluctuates often, as it
      may lead to repeated allocations when the vector grows again.

Automatic Cleanup
-----------------

The C language does not generally allow automated garbage collection of memory that
is out of scope. This section describes a poor man's garbage collection within the
C language, for the ``float_v`` data type, that can only be enabled if compiled
with ``gcc`` or ``clang``.

FLTVEC_GBC
~~~~~~~~~~
.. c:macro:: FLTVEC_GBC

   Macro that enables automatic cleanup of dynamically allocated float vectors when they
   go out of scope. Only available when using GCC or Clang compilers. Uses the cleanup
   attribute to automatically call _free_float_vector.

   Example:

   .. code-block:: c

      void process_floats(void) {
          // Vector will be automatically freed when function returns
          FLTVEC_GBC float_v* vec = init_float_vector(10);
          
          // Use the vector...
          
          // No need to call free_float_vector
      }  // vec is automatically freed here

   .. note::

      This macro should only be used with dynamically allocated vectors created by
      init_float_vector(). It is not needed for static arrays created with
      init_float_array() as they are automatically cleaned up when going out of scope.

Data Addition 
-------------
The functions below can be used to add data to a dynamically allocated array 
or a statically allocated vector.

push_back_float_vector
~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool push_back_float_vector(float_v* vec, const float value)

   Adds a float value to the end of the vector. If needed, the vector automatically
   resizes to accommodate the new value. For vectors smaller than VEC_THRESHOLD,
   capacity doubles when full. For larger vectors, a fixed amount is added.
   This is the most efficient method for adding data to a float vector with
   a time efficiency of :math:`O(1)`. If the structure passed is for a statically allocated 
   array, the function will return ``false``, if the user tries to enter data to 
   an out of bounds index and will set ``errno`` to ``EINVAL``

   :param vec: Target float vector
   :param value: Float value to add to vector
   :returns: true if successful, false on error
   :raises: Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure

   Vector Example:

   .. code-block:: c

      float_v* vec = init_float_vector(2);
      
      // Add some values
      push_back_float_vector(vec, 3.14f);
      push_back_float_vector(vec, 2.718f);
      
      // Vector will automatically resize
      push_back_float_vector(vec, 1.414f);
      
      printf("Vector size: %zu\n", f_size(vec));
      printf("[ ");
      for (size_t i = 0; i < f_size(vec) - 1; i++) 
          printf("%f, ", float_vector_index(vec, 1));
      printf("%f ]\n", float_vector_index(vec, f_size(vec) - 1))
      
      free_float_vector(vec);

   Output::

      Vector size: 3
      [ 3.140000, 2.718000, 1.414000 ]

   Array Example:

   .. code-block:: c

      float_v* vec = init_float_array(2);
      
      // Add some values
      push_back_float_vector(vec, 3.14f);
      push_back_float_vector(vec, 2.718f);
      
      // Array will refuse third element because it is out of bounds 
      
      if (!push_back_float_vector(vec, 1.414f))
          printf("push back failed\n");
      
      printf("Array size: %zu\n", f_size(vec));
      printf("[ ");
      for (size_t i = 0; i < f_size(vec) - 1; i++) 
          printf("%f, ", float_vector_index(vec, 1));
      printf("%f ]\n", float_vector_index(vec, f_size(vec) - 1))
      
      free_float_vector(vec);

   Output::

      push back failed
      Array size: 2
      [ 3.140000, 2.718000 ]

   The following should be considered when using this function:

   * The vector must be properly initialized using init_float_vector() or init_float_array()
   * For static arrays (created with init_float_array), attempts to exceed capacity will fail
   * If reallocation fails for dynamic vectors, the original vector remains unchanged
   * Any float value can be stored, including zero, infinities, and NaN
   * The operation requires enough contiguous memory for the entire resized array in dynamic case

   .. note::

      When resizing is needed, the vector grows either by doubling (when size < VEC_THRESHOLD)
      or by adding a fixed amount (when size >= VEC_THRESHOLD). This provides efficient
      amortized performance for both small and large vectors.

push_front_float_vector
~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool push_front_float_vector(float_v* vec, const float value)

   Adds a float value to the beginning of the vector, shifting all existing elements
   to the right. Automatically resizes the vector if needed when using dynamic allocation.
   This is the least efficient method for adding data to a float vector with
   a time efficiency of :math:`O(n)`.

   :param vec: Target float vector
   :param value: Float value to add at front
   :returns: true if successful, false on error
   :raises: Sets errno to EINVAL for NULL inputs or if static array is full,
           ENOMEM on allocation failure, ERANGE on size_t overflow

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(3);
      
      // Add some values from the back
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      // Add value at the front
      push_front_float_vector(vec, 1.0f);
      
      // Print all values
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Vector will grow automatically if needed
      push_front_float_vector(vec, 0.0f);
      
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
   Output::

      1.0 2.0 3.0
      0.0 1.0 2.0 3.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Add values to static array
      push_front_float_vector(&arr, 3.0f);
      printf("After first:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      push_front_float_vector(&arr, 2.0f);
      printf("After second: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      push_front_float_vector(&arr, 1.0f);
      printf("After third:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Array is now full - this will fail
      if (!push_front_float_vector(&arr, 0.0f)) {
          printf("Cannot add to full static array\n");
      }

   Output::

      After first:  3.0
      After second: 2.0 3.0
      After third:  1.0 2.0 3.0
      Cannot add to full static array

   The following should be considered when using this function:

   * For static arrays (created with init_float_array):
     - Attempts to exceed capacity will fail with errno set to EINVAL
     - No automatic resizing occurs
   
   * For dynamic vectors (created with init_float_vector):
     - Vector will automatically resize when full
     - Growth follows the doubling strategy for small vectors
     - Growth adds fixed amount for vectors larger than VEC_THRESHOLD
   
   * Performance considerations:
     - All existing elements must be moved right by one position
     - More expensive than push_back_float_vector for large vectors
     - Memory reallocation may occur for dynamic vectors

   .. note::

      When resizing is needed in dynamic vectors, the vector grows either by
      doubling (when size < VEC_THRESHOLD) or by adding a fixed amount
      (when size >= VEC_THRESHOLD). This provides efficient amortized
      performance while preventing excessive memory usage in large vectors.

insert_float_vector
~~~~~~~~~~~~~~~~~~~
.. c:function:: bool insert_float_vector(float_v* vec, float value, size_t index)

   Inserts a float value at any valid position in the vector, shifting subsequent
   elements to the right. Automatically resizes the vector if needed when using dynamic
   allocation. The time complexity of this function varies from :math:`O(1)` 
   to :math:`O(n)` depending on the insertion position.

   :param vec: Target float vector
   :param value: Float value to insert
   :param index: Position at which to insert (0 to vec->len)
   :returns: true if successful, false on error
   :raises: Sets errno to EINVAL for NULL inputs or if static array is full,
           ERANGE for invalid index or on size_t overflow,
           ENOMEM on allocation failure

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(4);
      
      // Create initial vector
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 3.0f);
      
      // Initial state
      printf("Initial:     ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Insert 2.0 between them
      insert_float_vector(vec, 2.0f, 1);
      
      printf("After insert: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
   Output::

      Initial:     1.0 3.0
      After insert: 1.0 2.0 3.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Insert values at different positions
      insert_float_vector(&arr, 3.0f, 0);  // First insertion
      printf("First insert:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      insert_float_vector(&arr, 1.0f, 0);  // At beginning
      printf("Second insert: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      insert_float_vector(&arr, 2.0f, 1);  // In middle
      printf("Third insert:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Array is now full - this will fail
      if (!insert_float_vector(&arr, 4.0f, 1)) {
          printf("Cannot insert into full static array\n");
      }

   Output::

      First insert:  3.0
      Second insert: 1.0 3.0
      Third insert:  1.0 2.0 3.0
      Cannot insert into full static array

   The following should be considered when using this function:

   * For static arrays (created with init_float_array):
     - Attempts to exceed capacity will fail with errno set to EINVAL
     - No automatic resizing occurs
     - Must be careful not to exceed fixed size
   
   * For dynamic vectors (created with init_float_vector):
     - Vector will automatically resize when full
     - Growth follows the doubling strategy for small vectors
     - Growth adds fixed amount for vectors larger than VEC_THRESHOLD
   
   * Performance considerations:
     - Inserting at the beginning requires moving all elements (most expensive)
     - Inserting at the end is equivalent to push_back (least expensive)
     - Cost increases with number of elements that must be shifted
     - Memory reallocation may occur for dynamic vectors

   .. note::

      The valid range for index is [0, length]. An index equal to the length
      performs an append operation. Any index greater than the length will
      result in ERANGE error.

update_float_vector
~~~~~~~~~~~~~~~~~~~
.. c:function:: void update_float_vector(float_v* vec, size_t index, float replacement_value)

   Updates a single element in a float vector at the specified index with a new value.
   Works with both dynamic vectors and static arrays.

   :param vec: Target float vector
   :param index: Position of element to update (0 to len-1)
   :param replacement_value: New value to store at the specified index
   :raises: Sets errno to EINVAL for NULL input or empty vector,
           ERANGE for index out of bounds

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(3);
      
      // Add initial values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      printf("Before update: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Update middle value
      update_float_vector(vec, 1, 5.0f);
      
      printf("After update:  ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");

   Output::

      Before update: 1.0 2.0 3.0
      After update:  1.0 5.0 3.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Add values
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      push_back_float_vector(&arr, 3.0f);
      
      printf("Before update: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Update first and last values
      update_float_vector(&arr, 0, 10.0f);
      update_float_vector(&arr, 2, 30.0f);
      
      printf("After update:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Before update: 1.0 2.0 3.0
      After update:  10.0 2.0 30.0

   Error Handling:

   * If vec is NULL, has invalid data pointer, or is empty:
     - Sets errno to EINVAL
     - Returns without modification
   
   * If index is out of bounds:
     - Sets errno to ERANGE
     - Returns without modification

   .. note::

      This function provides direct element access for updating values. Unlike
      some other operations, it works identically for both dynamic vectors
      and static arrays since it doesn't modify the container's size or
      capacity.

Data Removal
------------
The following functions can be used to remove ``float`` data from a dynamically 
allocated vector or statically allocated array.

pop_back_float_vector
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: float pop_back_float_vector(float_v* vec)

   Removes and returns the last element from the vector or array. This is the most
   efficient removal operation as it requires no element shifting. The time 
   complexity of this function is :math:`O(1)`.

   :param vec: Target float vector
   :returns: The removed float value, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(3);
      
      // Add some values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      float popped = pop_back_float_vector(vec);
      if (errno == 0) {
          printf("Popped value: %.1f\n", popped);
          printf("Remaining size: %zu\n", f_size(vec));
      }

   Output::

      Initial values: 1.0 2.0 3.0
      Popped value: 3.0
      Remaining size: 2

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(2);
      
      // Add values to static array
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop values
      float first_pop = pop_back_float_vector(&arr);
      printf("First pop: %.1f\n", first_pop);
      
      float second_pop = pop_back_float_vector(&arr);
      printf("Second pop: %.1f\n", second_pop);
      
      // Try to pop from empty array
      float result = pop_back_float_vector(&arr);
      if (errno == ENODATA) {
          printf("Cannot pop from empty array\n");
      }

   Output::

      Initial values: 1.0 2.0
      First pop: 2.0
      Second pop: 1.0
      Cannot pop from empty array

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If vector or array is empty:
     - Returns FLT_MAX
     - Sets errno to ENODATA

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between an error
      condition and a valid FLT_MAX value that was stored in the vector. If errno
      is 0, the returned FLT_MAX was a valid stored value.

   Example with error checking:

   .. code-block:: c

      float_v* vec = init_float_vector(2);
      push_back_float_vector(vec, FLT_MAX);  // Store actual FLT_MAX
      
      errno = 0;
      float value = pop_back_float_vector(vec);
      if (errno == 0) {
          // This was a valid FLT_MAX stored in the vector
          printf("Valid FLT_MAX popped\n");
      } else if (errno == ENODATA) {
          printf("Vector is empty\n");
      } else if (errno == EINVAL) {
          printf("Invalid vector\n");
      }
      
      free_float_vector(vec);

   Output::

      Valid FLT_MAX popped

pop_front_float_vector
~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: float pop_front_float_vector(float_v* vec)

   Removes and returns the first element from the vector or array, shifting all
   remaining elements left by one position. This operation requires moving all
   remaining elements and is therefore less efficient than pop_back_float_vector.
   This function has a time complexity of :math:`O(n)`.

   :param vec: Target float vector
   :returns: The removed float value, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(3);
      
      // Add some values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      float popped = pop_front_float_vector(vec);
      if (errno == 0) {
          printf("Popped value: %.1f\n", popped);
          printf("Remaining values: ");
          for (size_t i = 0; i < f_size(vec); i++) {
              printf("%.1f ", float_vector_index(vec, i));
          }
          printf("\n");
      }
      
   Output::

      Initial values: 1.0 2.0 3.0
      Popped value: 1.0
      Remaining values: 2.0 3.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(2);
      
      // Add values to static array
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop first value
      float first_pop = pop_front_float_vector(&arr);
      printf("First pop: %.1f\n", first_pop);
      printf("After first pop: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop remaining value
      float second_pop = pop_front_float_vector(&arr);
      printf("Second pop: %.1f\n", second_pop);
      
      // Try to pop from empty array
      float result = pop_front_float_vector(&arr);
      if (errno == ENODATA) {
          printf("Cannot pop from empty array\n");
      }

   Output::

      Initial values: 1.0 2.0
      First pop: 1.0
      After first pop: 2.0
      Second pop: 2.0
      Cannot pop from empty array

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If vector or array is empty:
     - Returns FLT_MAX
     - Sets errno to ENODATA

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between an error
      condition and a valid FLT_MAX value that was stored in the vector. If errno
      is 0, the returned FLT_MAX was a valid stored value.

pop_any_float_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: float pop_any_float_vector(float_v* vec, size_t index)

   Removes and returns the element at the specified index in the vector or array,
   shifting any subsequent elements to the left. Performance varies based on the
   removal position - removing from the end is fast, while removing from the start
   or middle requires shifting elements.  This function has a time complexity that 
   range from :math:`O(n)` to :math:`O(1)` depending on the index from which 
   data is popped.

   :param vec: Target float vector
   :param index: Position of element to remove (0 to len-1)
   :returns: The removed float value, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty,
           ERANGE for invalid index or on size_t overflow

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(4);
      
      // Add values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 4.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Pop middle value (index 1)
      float popped = pop_any_float_vector(vec, 1);
      if (errno == 0) {
          printf("Popped value: %.1f\n", popped);
          printf("Remaining values: ");
          for (size_t i = 0; i < f_size(vec); i++) {
              printf("%.1f ", float_vector_index(vec, i));
          }
          printf("\n");
      }

   Output::

      Initial values: 1.0 2.0 3.0 4.0
      Popped value: 2.0
      Remaining values: 1.0 3.0 4.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Add values
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      push_back_float_vector(&arr, 3.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop first value (index 0)
      float first = pop_any_float_vector(&arr, 0);
      printf("After pop first: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop last value (index 1)
      float last = pop_any_float_vector(&arr, 1);
      printf("After pop last: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Initial values: 1.0 2.0 3.0
      After pop first: 2.0 3.0
      After pop last: 2.0

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If vector or array is empty:
     - Returns FLT_MAX
     - Sets errno to ENODATA
   
   * If index is out of bounds:
     - Returns FLT_MAX
     - Sets errno to ERANGE

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between an error
      condition and a valid FLT_MAX value that was stored in the vector. If errno
      is 0, the returned FLT_MAX was a valid stored value.

   Performance Considerations:

   * Removing from the last position (index == len-1) is O(1)
   * Removing from the beginning requires shifting all elements left: O(n)
   * Removing from position i requires shifting n-i elements: O(n-i)
   * For frequent removals from the front, consider using pop_front_float_vector()
   * For frequent removals from the back, consider using pop_back_float_vector()

Utility Functions
=================
The following functions and macros can be used to retrieve basic information from
float containers.

Vector Size and Access Functions
--------------------------------
The following functions and macros are used to determine the allocated size and populated
length of float containers.

.. _float-size-func:

c_float_ptr 
~~~~~~~~~~~~
.. c:function:: float* c_float_ptr(float_v* vec)

   Returns a pointer to the beginning of the float array.  Generally 
   speaking, it is frowned upon to directly access data within the vector; 
   however, this supports integration with many legacy C libraries where the 
   array is passed as a pointer and the bounds is controlled by a size variable.

   :param vec: A pointer to a double_v data type 
   :returns: A pointer to a s style raw array 
   :raises: EINVAL for NULL input pointer or NULL data pointer.

   Example:

   .. code-block:: c 

      void print_data(float* array, size_t len) {
          printf("[ ");
          for (size_t i = 0; i < len - 1; i++) {
              printf("%f, ", array[i]);
          }
          printf("%f ]");
      }

      int main() {
          float_v* vec = init_float_vector(3);
          push_back_float_vector(vec, 1.0);
          push_back_float_vector(vec, 2.0);
          push_back_float_vector(vec, 3.0);
          print_data(vec, float_vector_size(vec));
          free_float_vector(vec);
          return 0;
      }

   .. code-block:: bash 

      [ 1.0000, 2.0000, 3.0000 ]

float_vector_size
~~~~~~~~~~~~~~~~~
.. c:function:: const size_t float_vector_size(const float_v* vec)

   Returns the current number of elements in the vector. This represents the
   actual number of floats stored, not the allocated capacity. 
   The :ref:`f_alloc <f-alloc-macro>` generic macro can be used in place of this 
   function. 

   :param vec: Float vector to query
   :returns: Number of elements in vector, or LONG_MAX on error
   :raises: Sets errno to EINVAL for NULL input or invalid data pointer

   Example:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(5);  // Capacity of 5
      push_back_float_vector(vec, 1);
      push_back_float_vector(vec, 2);
      push_back_float_vector(vec, 3);
      printf("Vector size: %zu\n", float_vector_size(vec));

   Result 

   .. code-block:: bash 

      Vector size: 3

.. _float-alloc-func:

float_vector_alloc
~~~~~~~~~~~~~~~~~~
.. c:function:: const size_t float_vector_alloc(const float_v* vec)

   Returns the current allocation size (capacity) of the vector. This represents
   the number of elements that can be stored without requiring reallocation.
   The :ref:`f_alloc <f-alloc-macro>` generic macro can be used in place of this 
   function.

   :param vec: Float vector to query
   :returns: Current allocation size, or LONG_MAX on error
   :raises: Sets errno to EINVAL for NULL input or invalid data pointer

   Example:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(5);
      printf("Allocation size: %zu\n", float_vector_alloc(vec));

   Result 

   .. code-block:: bash 

      Allocation size: 5

float_vector_index
~~~~~~~~~~~~~~~~~~
.. c:function:: const float float_vector_index(const float_v* vec, size_t index)

   Safely retrieves the value at the specified index in a float vector. Works with
   both dynamic vectors and static arrays.

   :param vec: Target float vector
   :param index: Position of element to retrieve (0 to len-1)
   :returns: Value at specified index, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ERANGE for index out of bounds

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec = init_float_vector(3);
      
      // Add values
      push_back_float_vector(vec, 1.5f);
      push_back_float_vector(vec, 2.5f);
      push_back_float_vector(vec, 3.5f);
      
      // Access values
      printf("First value:  %.1f\n", float_vector_index(vec, 0));
      printf("Second value: %.1f\n", float_vector_index(vec, 1));
      printf("Third value:  %.1f\n", float_vector_index(vec, 2));
      
      // Demonstrate error handling
      float result = float_vector_index(vec, 3);  // Invalid index
      if (result == FLT_MAX && errno == ERANGE) {
          printf("Error: Index out of bounds\n");
      }
      
      free_float_vector(vec);

   Output::

      First value:  1.5
      Second value: 2.5
      Third value:  3.5
      Error: Index out of bounds

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(2);
      
      // Add values
      push_back_float_vector(&arr, 10.0f);
      push_back_float_vector(&arr, 20.0f);
      
      // Safe access
      errno = 0;
      float first = float_vector_index(&arr, 0);
      if (errno == 0) {
          printf("First element: %.1f\n", first);
      }
      
      // Boundary check
      errno = 0;
      float invalid = float_vector_index(&arr, 5);
      if (errno == ERANGE) {
          printf("Attempted access beyond array bounds\n");
      }

   Output::

      First element: 10.0
      Attempted access beyond array bounds

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If index is out of bounds:
     - Returns FLT_MAX
     - Sets errno to ERANGE

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between
      an error condition and a valid FLT_MAX value that was stored in the
      vector. This function provides bounds-checked access to prevent
      buffer overflows and undefined behavior.

Re-Order and Sort Data 
----------------------
These functions can be used to change the order of data in a dynamically allocated 
or statically allocated ``float`` array.

reverse_float_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: void reverse_float_vector(float_v* vec)

   Reverses the order of elements in a float vector or array. The operation is performed
   in place without allocating additional memory.

   :param vec: Target float vector
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec = init_float_vector(4);
      
      // Add some values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      printf("Before reverse: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      reverse_float_vector(vec);
      
      printf("After reverse:  ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      free_float_vector(vec);

   Output::

      Before reverse: 1.0 2.0 3.0
      After reverse:  3.0 2.0 1.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Add values
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      push_back_float_vector(&arr, 3.0f);
      
      printf("Before reverse: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      reverse_float_vector(&arr);
      
      printf("After reverse:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Before reverse: 1.0 2.0 3.0
      After reverse:  3.0 2.0 1.0

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Sets errno to EINVAL
     - Returns without modifying data
   
   * If vector or array is empty:
     - Sets errno to ENODATA
     - Returns without modifying data

   Performance Characteristics:

   * Time complexity: O(n) where n is the number of elements
   * Space complexity: O(1) as reversal is performed in place
   * Uses constant extra space regardless of vector size
   
   .. note::

      The function performs the reversal in place by swapping pairs of elements
      from the ends toward the middle. This approach minimizes memory usage and
      maintains efficiency for both small and large vectors.

sort_float_vector
~~~~~~~~~~~~~~~~~
.. c:function:: void sort_float_vector(float_v* vec, iter_dir direction)

   Sorts a float vector or array in either ascending (FORWARD) or descending (REVERSE) order
   using an optimized QuickSort algorithm with median-of-three pivot selection and
   insertion sort for small subarrays.

   :param vec: Target float vector
   :param direction: FORWARD for ascending, REVERSE for descending order
   :raises: Sets errno to EINVAL if vec is NULL

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec = init_float_vector(5);
      
      // Add some unsorted values
      push_back_float_vector(vec, 5.0f);
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 4.0f);
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      
      printf("Before sort: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Sort in ascending order
      sort_float_vector(vec, FORWARD);
      
      printf("Ascending:   ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Sort in descending order
      sort_float_vector(vec, REVERSE);
      
      printf("Descending:  ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      free_float_vector(vec);

   Output::

      Before sort: 5.0 3.0 4.0 1.0 2.0
      Ascending:   1.0 2.0 3.0 4.0 5.0
      Descending:  5.0 4.0 3.0 2.0 1.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(4);
      
      // Add unsorted values
      push_back_float_vector(&arr, 4.0f);
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 3.0f);
      push_back_float_vector(&arr, 2.0f);
      
      printf("Before sort: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      sort_float_vector(&arr, FORWARD);
      
      printf("After sort:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Before sort: 4.0 1.0 3.0 2.0
      After sort:  1.0 2.0 3.0 4.0

   Implementation Details:

   The sorting algorithm uses a hybrid approach combining QuickSort with
   Insertion Sort for optimal performance:

   * QuickSort with median-of-three pivot selection for large partitions
   * Insertion Sort for small partitions (less than 10 elements)
   * Tail-call optimization to reduce stack usage
   * Special handling for duplicate elements and special values (NaN, infinities)

   Performance Characteristics:

   * Average time complexity: O(n log n)
   * Worst case time complexity: O(n²) (rare due to median-of-three)
   * Space complexity: O(log n) for recursion stack
   * In-place sorting: No additional memory allocation
   * Stable: No, equal elements may be reordered
   * Adaptive: Yes, performs better on partially sorted arrays

   Special Value Handling:

   * NaN values are moved to the end of the array
   * Infinities are properly ordered (-∞ < finite numbers < +∞)
   * Zero values (both -0.0 and +0.0) are treated as equal

   .. note::

      For very small arrays (n < 10), the function automatically uses Insertion Sort
      instead of QuickSort, as this is more efficient for small datasets.

Search Vector 
-------------

binary_search_float_vector
~~~~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: size_t binary_search_float_vector(float_v* vec, float value, float tolerance, bool sort_first)

   Performs a binary search on a float vector to find a value within the specified
   tolerance. Can optionally sort the vector before searching. Returns the index of
   the first matching value found, or LONG_MAX if not found.

   :param vec: Target float vector
   :param value: Float value to search for
   :param tolerance: Maximum allowed difference between values to consider a match
   :param sort_first: If true, sorts the vector before searching
   :returns: Index of found value, or LONG_MAX if not found
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with ordered vector:

   .. code-block:: c

      float_v* vec = init_float_vector(5);
      
      // Add sorted values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 4.0f);
      push_back_float_vector(vec, 5.0f);
      
      // Search for exact value
      size_t index = binary_search_float_vector(vec, 3.0f, 0.0001f, false);
      if (index != LONG_MAX) {
          printf("Found 3.0 at index %zu\n", index);
      }
      
      // Search with tolerance
      index = binary_search_float_vector(vec, 2.95f, 0.1f, false);
      if (index != LONG_MAX) {
          printf("Found value near 2.95 at index %zu\n", index);
      }
      
      free_float_vector(vec);

   Output::

      Found 3.0 at index 2
      Found value near 2.95 at index 2

   Example with unordered vector:

   .. code-block:: c

      float_v* vec = init_float_vector(5);
      
      // Add unsorted values
      push_back_float_vector(vec, 5.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 4.0f);
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 3.0f);
      
      // Search with auto-sort
      size_t index = binary_search_float_vector(vec, 4.0f, 0.0001f, true);
      if (index != LONG_MAX) {
          printf("Found 4.0 at index %zu\n", index);
      }
      
      // Vector is now sorted for subsequent searches
      index = binary_search_float_vector(vec, 2.0f, 0.0001f, false);
      if (index != LONG_MAX) {
          printf("Found 2.0 at index %zu\n", index);
      }
      
      free_float_vector(vec);

   Output::

      Found 4.0 at index 3
      Found 2.0 at index 1

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns LONG_MAX
     - Sets errno to EINVAL
   
   * If vector is empty:
     - Returns LONG_MAX
     - Sets errno to ENODATA
   
   * If value is not found within tolerance:
     - Returns LONG_MAX
     - Does not set errno

   Performance Characteristics:

   * Time Complexity:
     - O(log n) if vector is sorted and sort_first is false
     - O(n log n) if sort_first is true due to sorting overhead
   * Space Complexity: O(1)

   .. note::

      The tolerance parameter allows for approximate matches, which is useful when
      working with floating-point values that may have small representation
      errors. Setting tolerance to 0.0f requires an exact match.

Min and Max Values 
------------------
The following functions can be used to find the maximum and minimum values 
in a dynamically allocated vector or a statically allocated array.

Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
such as AVX or SSE where supported, enabling high-performance parallel summation
of float values. Falls back to scalar implementation on platforms where SIMD is 
not available.

.. note:: 

   If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of float arrays.

min_float_vector
~~~~~~~~~~~~~~~~
.. c:function:: float min_float_vector(float_v* vec)

   Returns the minimum value in a float vector. Works with both dynamic vectors
   and static arrays.

   :param vec: Target float vector
   :returns: Minimum value in vector, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, empty vector, or invalid data pointer

   Example:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(5);
      
      // Add values
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 4.0f);
      push_back_float_vector(vec, -2.0f);
      push_back_float_vector(vec, 5.0f);
      
      float min_val = min_float_vector(vec);
      if (errno == 0) {
          printf("Minimum value: %.1f\n", min_val);
      }
      
   Output::

      Minimum value: -2.0

max_float_vector
~~~~~~~~~~~~~~~~
.. c:function:: float max_float_vector(float_v* vec)

   Returns the maximum value in a float vector. Works with both dynamic vectors
   and static arrays.

   :param vec: Target float vector
   :returns: Maximum value in vector, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, empty vector, or invalid data pointer

   Example:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(5);
      
      // Add values
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 4.0f);
      push_back_float_vector(vec, -2.0f);
      push_back_float_vector(vec, 5.0f);
      
      float max_val = max_float_vector(vec);
      if (errno == 0) {
          printf("Maximum value: %.1f\n", max_val);
      }

   Output::

      Maximum value: 5.0

Example using both functions with static array:

   .. code-block:: c

      float_v arr = init_float_array(4);
      
      // Add values
      push_back_float_vector(&arr, 3.14f);
      push_back_float_vector(&arr, -1.5f);
      push_back_float_vector(&arr, 2.718f);
      push_back_float_vector(&arr, 0.0f);
      
      printf("Values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.3f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      printf("Min: %.3f\n", min_float_vector(&arr));
      printf("Max: %.3f\n", max_float_vector(&arr));

   Output::

      Values: 3.140 -1.500 2.718 0.000
      Min: -1.500
      Max: 3.140

Error Handling for Both Functions:

* If vec is NULL, has invalid data pointer, or is empty:
  - Returns FLT_MAX
  - Sets errno to EINVAL

Special Value Handling:

* NaN values are ignored
* Infinities are properly compared
* Both positive and negative zeros are treated as equal

.. note::

   When FLT_MAX is returned, always check errno to distinguish between
   an error condition and a valid FLT_MAX value that was actually present
   in the vector.

Vector Statistics
------------------
These functions can be used to determine basic statistical parameters of a 
vector or array.

sum_float_vector
~~~~~~~~~~~~~~~~
.. c:function:: float sum_float_vector(float_v* vec)

   Calculates the sum of all elements in a float vector. Works with both dynamic
   vectors and static arrays.

   Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
   such as AVX or SSE where supported, enabling high-performance parallel summation
   of float values. Falls back to scalar implementation on platforms where SIMD is 
   not available.

   :param vec: Target float vector
   :returns: Sum of all elements, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, empty vector, or NaN values

   .. note:: 

      If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of float arrays. 

   Example:

   .. code-block:: c

      float_v* vec = init_float_vector(4);
      
      // Add values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 4.0f);
      
      float sum = sum_float_vector(vec);
      if (errno == 0) {
          printf("Sum: %.1f\n", sum);
      }
      
      free_float_vector(vec);

   Output::

      Sum: 10.0

average_float_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: float average_float_vector(float_v* vec)

   Calculates the arithmetic mean (average) of all elements in a float vector.
   Works with both dynamic vectors and static arrays.

   Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
   such as AVX or SSE where supported, enabling high-performance parallel summation
   of float values. Falls back to scalar implementation on platforms where SIMD is 
   not available.

   :param vec: Target float vector
   :returns: Average of all elements, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, empty vector, or NaN values

   .. note:: 

      If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of float arrays.

   Example:

   .. code-block:: c

      float_v* vec = init_float_vector(4);
      
      // Add values
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 4.0f);
      push_back_float_vector(vec, 6.0f);
      push_back_float_vector(vec, 8.0f);
      
      float avg = average_float_vector(vec);
      if (errno == 0) {
          printf("Average: %.1f\n", avg);
      }
      
      free_float_vector(vec);

   Output::

      Average: 5.0

Example using both functions with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Add values
      push_back_float_vector(&arr, 1.5f);
      push_back_float_vector(&arr, 2.5f);
      push_back_float_vector(&arr, 3.5f);
      
      printf("Values:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      printf("Sum:     %.1f\n", sum_float_vector(&arr));
      printf("Average: %.1f\n", average_float_vector(&arr));

   Output::

      Values:  1.5 2.5 3.5
      Sum:     7.5
      Average: 2.5

Special Value Handling:

* Infinity values are allowed and propagate through calculations
* NaN values will cause the functions to return FLT_MAX and set errno to EINVAL
* Both positive and negative zeros are handled correctly

Error Handling for Both Functions:

* If vec is NULL, has invalid data pointer, or is empty:
  - Returns FLT_MAX
  - Sets errno to EINVAL
* If any value in the vector is NaN:
  - Returns FLT_MAX
  - Sets errno to EINVAL

.. note::

   When FLT_MAX is returned, always check errno to distinguish between
   an error condition and a valid calculation that resulted in FLT_MAX.

stdev_float_vector
~~~~~~~~~~~~~~~~~~
.. c:function:: float stdev_float_vector(float_v* vec)

   Calculates the population standard deviation of elements in a float vector.
   Works with both dynamic vectors and static arrays.

   Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
   such as AVX or SSE where supported, enabling high-performance parallel summation
   of float values. Falls back to scalar implementation on platforms where SIMD is 
   not available.

   :param vec: Target float vector
   :returns: Standard deviation of elements, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input or empty vector

   .. note:: 

      If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of float arrays.

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(4);
      
      // Add values
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 4.0f);
      push_back_float_vector(vec, 4.0f);
      push_back_float_vector(vec, 6.0f);
      
      printf("Values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      float stdev = stdev_float_vector(vec);
      if (errno == 0) {
          printf("Standard Deviation: %.3f\n", stdev);
      }

   Output::

      Values: 2.0 4.0 4.0 6.0
      Standard Deviation: 1.414

Cummulative Distribution Function (CDF)
---------------------------------------

cum_sum_float_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: float_v* cum_sum_float_vector(float_v* vec)

   Creates a new vector containing the cumulative sum of elements from the input vector.
   Each element in the output vector is the sum of all elements up to and including
   that position in the input vector. Works with both dynamic vectors and static arrays.

   Internally optimized using SIMD (Single Instruction, Multiple Data) instructions 
   such as AVX or SSE where supported, enabling high-performance parallel summation
   of float values. Falls back to scalar implementation on platforms where SIMD is 
   not available.

   :param vec: Target float vector
   :returns: New vector containing cumulative sums, or NULL on error
   :raises: Sets errno to EINVAL for NULL input or empty vector, ENODATA for failed push operations

   .. note:: 

      If compiled with `-march=native`, `-mavx`, or `-msse`, this function will use hardware-accelerated AVX or SSE instructions for fast processing of float arrays.

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(4);
      
      // Add values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 4.0f);
      
      printf("Original values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      float_v* cum_sum = cum_sum_float_vector(vec);
      if (cum_sum != NULL) {
          printf("Cumulative sums: ");
          for (size_t i = 0; i < f_size(cum_sum); i++) {
              printf("%.1f ", float_vector_index(cum_sum, i));
          }
          printf("\n");
          
          free_float_vector(cum_sum);
      }

   Output::

      Original values: 1.0 2.0 3.0 4.0
      Cumulative sums: 1.0 3.0 6.0 10.0

Example using both functions with negative values:

   .. code-block:: c

      float_v* vec = init_float_vector(4);
      
      // Add values including negatives
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, -2.0f);
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, -4.0f);
      
      printf("Values:          ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      float stdev = stdev_float_vector(vec);
      if (errno == 0) {
          printf("Std Deviation:   %.3f\n", stdev);
      }
      
      float_v* cum_sum = cum_sum_float_vector(vec);
      if (cum_sum != NULL) {
          printf("Running totals:  ");
          for (size_t i = 0; i < f_size(cum_sum); i++) {
              printf("%.1f ", float_vector_index(cum_sum, i));
          }
          printf("\n");
          
          free_float_vector(cum_sum);
      }
      
      free_float_vector(vec);

   Output::

      Values:          1.0 -2.0 3.0 -4.0
      Std Deviation:   2.944
      Running totals:  1.0 -1.0 2.0 -2.0

Error Handling:

* If vec is NULL, has invalid data pointer, or is empty:
  - stdev_float_vector returns FLT_MAX and sets errno to EINVAL
  - cum_sum_float_vector returns NULL and sets errno to EINVAL

* If memory allocation fails in cum_sum_float_vector:
  - Returns NULL
  - Sets errno to ENODATA

Special Value Handling:

* Infinity values propagate through calculations
* Result will be infinite if any calculations overflow
* Both functions handle negative values correctly

.. note::

   The standard deviation calculation uses a population standard deviation
   formula (dividing by n), not a sample standard deviation formula
   (dividing by n-1).

Copy Vector 
~~~~~~~~~~~
.. c:function:: float_v* copy_float_vector(float_v* vec)

   Creates a deep copy of a vector or array.

   :param vec: The vector or array to be copied
   :returns: New vector containing a copy of the input vector
   :raises: Sets errno to EINVAL for NULL input or empty vector, ENOMEM for failure to create new vector

   .. note:: 

      This method will always output a dynamically allocated array even if the input array is statically allocated.

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(4);
      
      // Add values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 4.0f);
      
      printf("Original values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");

      float_v* FLTVEC_GBC new_vec = copy_float_vector(vec);
      
      if (new_vec != NULL) {
          printf("New values: ");
          for (size_t i = 0; i < f_size(new_vec); i++) {
              printf("%.1f ", float_vector_index(new_vec, i));
          }
          printf("\n");
      }

   Output::

      Original values: 1.0 2.0 3.0 4.0
      New values: 1.0 2.0 3.0 4.0

Float Matrix Overview 
=====================
The ``matrix_f`` type provides a flexible and extensible representation of 2D matrices
containing ``float`` values. It supports automatic format selection and dynamic 
conversion between internal formats including:

* Dense (row-major)
* COO (coordinate list)
* CSR (compressed sparse row)

All format-specific behavior is encapsulated. Users should access data through
the generic API functions documented here.

Matrix objects must be created and managed using the generic functions declared in 
the ``c_float.h`` header file.

.. code-block:: c

   typedef struct matrix_f matrix_f;

Matrix Initialization and Cleanup
---------------------------------

create_float_matrix
~~~~~~~~~~~~~~~~~~~
.. c:function:: matrix_f* create_float_matrix(size_t rows, size_t cols, size_t estimated_zeros)

   Initializes a new matrix object with the specified number of rows and columns.
   Internally selects an appropriate format based on dimensions.

   :param rows: Number of matrix rows
   :param cols: Number of matrix columns
   :param estimated_zeros: The estimated number of empty elements in the matrix. Used to determine the optimum matrix format.
   :returns: Pointer to new ``matrix_f`` object, or ``NULL`` on failure
   :raises: Sets ``errno`` to ``EINVAL`` for zero dimensions, ``ENOMEM`` on allocation failure

   Example:

   .. code-block:: c

      matrix_f* mat FLTMAT_GBC = create_float_matrix(5, 5, 0);
      if (!mat) {
          perror("Failed to create matrix");
      }

free_float_matrix
~~~~~~~~~~~~~~~~~
.. c:function:: void free_float_matrix(matrix_f* mat)

   Frees all memory associated with a matrix object. Only required if not using
   the :ref:`FLTMAT_GBC <matrix_auto_gc>` macro.

   :param mat: Matrix to free
   :raises: Sets ``errno`` to ``EINVAL`` if input is NULL

   Example:

   .. code-block:: c

      matrix_f* mat = create_float_matrix(10, 10, 0);
      // Use matrix...
      free_float_matrix(mat);

.. _matrix_auto_gc:

FLTMAT_GBC
~~~~~~~~~~
.. c:macro:: FLTMAT_GBC

   Enables automatic cleanup of ``matrix_f`` objects at end of scope.
   Only available with GCC or Clang compilers that support the ``cleanup`` attribute.

   Example:

   .. code-block:: c

      void compute(void) {
          matrix_f* FLTMAT_GBC mat = create_float_matrix(4, 4, 0);
          // Matrix is freed automatically when function returns
      }

Matrix Element Access
---------------------

insert_float_matrix
~~~~~~~~~~~~~~~~~~~
.. c:function:: bool insert_float_matrix(matrix_f** mat, size_t row, size_t col, float value, bool convert_to_csr)

   Inserts or updates a float value at the specified (row, col) position. Typically a user 
   would not want to trigger a conversion to a CSR matrix until the matrix is fully populated to 
   its maximum extent.  However, sometimes for storage reasons an insert operation may trigger this 
   transformation, which is why the ``convert_to_csr`` flag exists in this function.

   :param mat: Target matrix
   :param row: Row index
   :param col: Column index
   :param value: Float value to insert
   :param convert_to_csr: true if an insert should be allowed to trigger a csr matrix conversion, false otherwise
   :returns: ``true`` if successful, ``false`` on error
   :raises: Sets ``errno`` to ``EINVAL`` for NULL input or unsupported format,
            ``ERANGE`` for out-of-bounds access,
            ``ENOMEM`` if internal resize fails (COO only)

   Example:

   .. code-block:: c

      matrix_f* mat FLTMAT_GBC = create_float_matrix(10, 15, 5);
      insert_float_matrix(&mat, 2, 3, 5.5f, false);

pop_float_matrix
~~~~~~~~~~~~~~~~
.. c:function:: float pop_float_matrix(matrix_f** mat, size_t row, size_t col)

   Removes and returns the value at the specified (row, col) position.
   Returns ``FLT_MAX`` if the entry is not present.

   :param mat: Target matrix
   :param row: Row index
   :param col: Column index
   :returns: Value at the specified position, or ``FLT_MAX`` if not found
   :raises: Sets ``errno`` to ``EINVAL`` for NULL input or unsupported format,
            ``ERANGE`` for out-of-bounds indices,
            ``ENODATA`` if the position is unoccupied

   Example:

   .. code-block:: c

      float value = pop_float_matrix(&mat, 2, 3);
      if (errno == 0) {
          printf("Removed value: %.2f\n", value);
      }

get_float_matrix
~~~~~~~~~~~~~~~~
.. c:function:: float get_float_matrix(matrix_f* mat, size_t row, size_t col)

   Returns the value at a specific matrix position, or ``FLT_MAX`` on error or
   if no value exists (in sparse formats).

   :param mat: Target matrix
   :param row: Row index
   :param col: Column index
   :returns: Value at position, or ``FLT_MAX`` on error
   :raises: Sets ``errno`` to ``EINVAL`` for NULL input, ``ERANGE`` for out-of-bounds

   Example:

   .. code-block:: c

      float value = get_float_matrix(mat, 1, 1);
      if (errno == 0) {
          printf("Value at (1,1): %.2f\n", value);
      }

Matrix Utility Functions
------------------------

float_matrix_rows
~~~~~~~~~~~~~~~~~
.. c:function:: size_t float_matrix_rows(const matrix_f* mat)

   Returns the number of rows in the matrix.

   :param mat: Matrix to query
   :returns: Number of rows, or ``SIZE_MAX`` on error
   :raises: Sets ``errno`` to ``EINVAL`` for NULL input

float_matrix_cols
~~~~~~~~~~~~~~~~~
.. c:function:: size_t float_matrix_cols(const matrix_f* mat)

   Returns the number of columns in the matrix.

   :param mat: Matrix to query
   :returns: Number of columns, or ``SIZE_MAX`` on error
   :raises: Sets ``errno`` to ``EINVAL`` for NULL input

float_matrix_type
~~~~~~~~~~~~~~~~~
.. c:function:: matrix_type float_matrix_type(const matrix_f* mat)

   Returns the internal storage format of the matrix (e.g., DENSE_MATRIX, SPARSE_COO_MATRIX, etc.).

   :param mat: Matrix to query
   :returns: Enum representing the matrix format
   :raises: Sets ``errno`` to ``EINVAL`` for NULL input

.. note::

   Use the ``float_matrix_type()`` function for logging or debugging purposes. 
   Most operations should rely on the generic interface regardless of internal format.

Matrix Format Conversion and Optimization
-----------------------------------------

finalize_float_matrix
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool finalize_float_matrix(matrix_f** mat_ptr)

   Finalizes a matrix by converting it to a more efficient internal representation.
   If the matrix is large and sparse, this function may convert it to COO or CSR.

   :param mat_ptr: Pointer to a ``matrix_f*`` to finalize
   :returns: ``true`` on success, ``false`` on error
   :raises: Sets ``errno`` to ``EINVAL`` if input is NULL

   .. note::
      This function may replace the pointer value if a new matrix structure is allocated.

   Example:

   .. code-block:: c

      matrix_f* mat FLTMAT_GBC = create_float_matrix(1000, 1000);
      // Populate matrix...
      finalize_float_matrix(&mat);  // May convert to CSR

maybe_convert_float_matrix
~~~~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: void maybe_convert_float_matrix(matrix_f** mat_ptr, bool convert_to_csr)

   Heuristically evaluates whether a matrix should be converted to a more efficient
   internal format. Dense matrices may be converted to COO, and COO matrices to CSR.

   :param mat_ptr: Pointer to a ``matrix_f*`` to evaluate
   :param convert_to_csr: If true, allows conversion from COO to CSR format
   :returns: None
   :raises: No errno set; only converts on success

   .. warning::
      The matrix pointer may be replaced if a new structure is allocated.

compact_float_csr_matrix
~~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: void compact_float_csr_matrix(matrix_f* mat)

   Compacts a CSR matrix by removing logically deleted entries (tombstones)
   and reallocating storage to fit only the active elements.

   :param mat: CSR matrix to compact
   :returns: None
   :raises: Sets ``errno`` to ``ENOMEM`` if reallocation fails,
            or ``EINVAL`` if the matrix is not in CSR format

   Example:

   .. code-block:: c

      matrix_f* mat FLTMAT_GBC = create_float_matrix(100, 100);
      // Populate, delete entries...
      compact_float_csr_matrix(mat);  // Shrinks internal arrays

convert_float_coo_to_csr
~~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: matrix_f* convert_float_coo_to_csr(const matrix_f* coo_mat)

   Converts a matrix from COO (coordinate list) format to CSR (compressed sparse row) format.
   The original matrix remains unchanged. Returns a newly allocated matrix in CSR format.

   :param coo_mat: COO matrix to convert
   :returns: New ``matrix_f*`` in CSR format, or ``NULL`` on failure
   :raises: Sets ``errno`` to ``EINVAL`` if input is invalid,
            or ``ENOMEM`` on allocation failure

   Example:

   .. code-block:: c

      matrix_f* coo = create_float_matrix(200, 200);
      // Populate with insert_float_matrix...
      matrix_f* csr = convert_float_coo_to_csr(coo);
      if (!csr) perror("Conversion failed");



Float Dictionary Overview
==========================

A float dictionary provides a hash table implementation for mapping string keys to float values, 
offering efficient key-value storage and retrieval. This implementation uses chained hashing for 
collision resolution and features automatic table resizing for optimal performance.

Key Features
------------

* Dynamic resizing: Automatic growth when load factor threshold is reached
* Efficient lookup: O(1) average case access time
* Memory safety: Proper encapsulation and memory management
* String key support: Automatic key duplication and management
* Collision handling: Chained hashing for robust collision resolution
* Automatic cleanup: Optional garbage collection support with FDICT_GBC

When to Use Float Dictionaries
------------------------------

Float dictionaries are particularly useful when:

* Mapping strings to numerical values
* Requiring fast key-value lookups
* Managing relationships between text identifiers and measurements
* Implementing caches or lookup tables with string keys
* Building numerical mapping tables
* Creating frequency counters for string data

Performance Characteristics
---------------------------

* Access time: O(1) average case for lookups and insertions
* Space efficiency: Adaptive growth strategy for memory efficiency
* Collision handling: Chained hashing for reliable performance under high load
* Memory overhead: Small per-entry overhead for key storage and chain pointers

Data Types
==========

The following are derived data types used in the implementation of float dictionaries.

dict_f
------
Opaque type representing a float dictionary that maps string keys to float values.
Implementation details are hidden from the user for encapsulation.

.. code-block:: c

   typedef struct dict_f dict_f;

Core Functions
==============

The float dictionary implementation provides a complete set of functions for dictionary 
manipulation. All functions are declared in the ``c_float.h`` header file.

Initialization and Memory Management
------------------------------------

The functions and Macros in this section control the creation, memory allocation,
and destruction of ``dict_f`` data types.

init_float_dict
~~~~~~~~~~~~~~~~~
.. c:function:: dict_f* init_float_dict(void)

   Initializes a new empty dictionary with default initial capacity.

   :returns: Pointer to new dict_f object, or NULL on allocation failure
   :raises: Sets errno to ENOMEM if memory allocation fails

   Example:

   .. code-block:: c

      dict_f* dict = init_float_dict();
      if (!dict) {
          fprintf(stderr, "Failed to initialize dictionary\\n");
          return 1;
      }
      
      // Use dictionary...
      
      free_float_dict(dict);

free_float_dict
~~~~~~~~~~~~~~~
.. c:function:: void free_float_dict(dict_f* dict)

   Frees all memory associated with a dictionary, including all stored keys and nodes.
   
   :param dict: Dictionary to free
   
   Example:

   .. code-block:: c

      dict_f* dict = init_float_dict();
      // Use dictionary...
      free_float_dict(dict);
      dict = NULL;  // Good practice to avoid dangling pointer

FDICT_GBC
~~~~~~~~~
.. c:macro:: FDICT_GBC

   Macro that enables automatic cleanup of dictionaries when they go out of scope.
   Only available when using GCC or Clang compilers.

   Example:

   .. code-block:: c

      void process_data(void) {
          FDICT_GBC dict_f* dict = init_float_dict();
          // Use dictionary...
      }  // dict is automatically freed here

Data Insertion and Update
-------------------------

insert_float_dict
~~~~~~~~~~~~~~~~~
.. c:function:: bool insert_float_dict(dict_f* dict, const char* key, float value)

   Inserts a new key-value pair into the dictionary. If the key already exists,
   the function returns false. The dictionary automatically resizes if needed.

   :param dict: Target dictionary
   :param key: String key to insert
   :param value: Float value to associate with key
   :returns: true if insertion successful, false if key exists or error occurs
   :raises: Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure,
           EEXIST if key already exists

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      
      if (insert_float_dict(dict, "temperature", 23.5f)) {
          printf("Value inserted successfully\n");
      }
      
      // Trying to insert same key again fails
      if (!insert_float_dict(dict, "temperature", 24.0f)) {
          printf("Key already exists\n");
      }

      printf("Key: 'temperature', Value: %f\n", get_float_dict_value(dict, "temperature"));

   .. code-block:: bash

      Value inserted succesfully 
      Key already exists
      Key: 'temperature', Value: 23.50000

update_float_dict
~~~~~~~~~~~~~~~~~
.. c:function:: bool update_float_dict(dict_f* dict, const char* key, float value)

   Updates the value associated with an existing key. If the key doesn't exist,
   the function returns false.

   :param dict: Target dictionary
   :param key: String key to update
   :param value: New float value to associate with key
   :returns: true if update successful, false if key not found or error occurs
   :raises: Sets errno to EINVAL for NULL inputs, ENOENT if key not found

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      insert_float_dict(dict, "temperature", 31.7);
      insert_float_dict(dict, "pressure", 101.127);
      if (update_float_dict(dict, "temperature", 24.0f)) {
          printf("Value updated successfully\n");
      } else {
          printf("Key not found\n");
      }
      printf("Key: 'temperature', Value: %f\n", get_float_dict_value(dict, "temperature"));

   .. code-block:: bash 

      Value updated succesfully 
      Key: 'temperature', Value: 24.0000

Data Retrieval
--------------

get_float_dict_value
~~~~~~~~~~~~~~~~~~~~
.. c:function:: float get_float_dict_value(const dict_f* dict, const char* key)

   Retrieves the value associated with a key. Returns FLT_MAX if the key
   is not found.

   :param dict: Target dictionary
   :param key: String key to look up
   :returns: Associated float value, or FLT_MAX if not found
   :raises: Sets errno to EINVAL for NULL inputs, ENOENT if key not found

   Example:

   .. code-block:: c
      
      dict_f* dict FDICT_GBC = init_float_dict();
      insert_float_dict(dict, "temperature", 31.7);
      insert_float_dict(dict, "pressure", 101.127);
      if (update_float_dict(dict, "temperature", 24.0f)) {
          printf("Value updated successfully\n");
      } else {
          printf("Key not found\n");
      }
      printf("Key: 'temperature', Value: %f\n", get_float_dict_value(dict, "temperature"));

   .. code-block:: bash 

      Value updated succesfully 
      Key: 'temperature', Value: 24.0000
     
Data Removal
------------

pop_float_dict
~~~~~~~~~~~~~~
.. c:function:: float pop_float_dict(dict_f* dict, const char* key)

   Removes and returns the value associated with a key. Returns FLT_MAX if
   the key is not found.

   :param dict: Target dictionary
   :param key: String key to remove
   :returns: Value associated with key, or FLT_MAX if not found
   :raises: Sets errno to EINVAL for NULL inputs, ENOENT if key not found

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      insert_float_dict(dict, "temperature", 31.7);
      insert_float_dict(dict, "temperature", 101.7);
      float value = pop_float_dict(dict, "temperature");
      float value = get_float_dict_value(dict, "temperature);
      if (value == FLT_MAX && errno = ENOENT) {
          printf("Removed value associated with: 'temperature'");
      }

   .. code-block::

      Removed value associated with 'temperature'

Utility Functions
-----------------

.. _float-dict-size-func:

float_dict_size
~~~~~~~~~~~~~~~
.. c:function:: size_t float_dict_size(const dict_f* dict)

  Returns the number of non-empty buckets in the dictionary.  The user 
  can also use the :ref:`f_size <f-size-macro>` Generic Macro in place 
  of this function.

  :param dict: Target dictionary
  :returns: Number of non-empty buckets, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example with single-item buckets:

  .. code-block:: c

     dict_f* dict = init_float_dict();
     
     // Add values that will hash to different buckets
     insert_float_dict(dict, "temperature", 23.5f);
     insert_float_dict(dict, "humidity", 45.0f);
     insert_float_dict(dict, "pressure", 1013.2f);
     
     printf("Number of buckets used: %zu\n", float_dict_size(dict));
     // printf("Number of buckets used: %zu\n", f_size(dict) // Optional macro use
     printf("Total key-value pairs: %zu\n", float_dict_hash_size(dict));
     
     free_float_dict(dict);

  Output::

     Number of buckets used: 3
     Total key-value pairs: 3

  Example with collision:

  .. code-block:: c

     dict_f* dict = init_float_dict();
     
     // Add values that might hash to same bucket
     insert_float_dict(dict, "value1", 1.0f);
     insert_float_dict(dict, "value2", 2.0f);
     insert_float_dict(dict, "value3", 3.0f);
     
     printf("Number of buckets used: %zu\n", float_dict_size(dict));
     printf("Total key-value pairs: %zu\n", float_dict_hash_size(dict));
     
     free_float_dict(dict);

  Output::

     Number of buckets used: 1
     Total key-value pairs: 3

.. _float-dict-alloc-func:

float_dict_alloc
~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dict_alloc(const dict_f* dict)

  Returns the total number of buckets allocated in the dictionary. The user 
  can also use the :ref:`f_alloc <f-alloc-macro>` Generic Macro in place 
  of this function. 

  :param dict: Target dictionary
  :returns: Total number of buckets, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example showing growth:

  .. code-block:: c

     dict_f* dict = init_float_dict();
     
     printf("Initial allocation: %zu buckets\n", float_dict_alloc(dict));
     // printf("Initial allocations: %zy buckets\n", f_alloc(dict)) \\ Optional Macro use
     
     // Add many values to trigger resize
     char key[20];
     for(int i = 0; i < 20; i++) {
         sprintf(key, "key%d", i);
         insert_float_dict(dict, key, (float)i);
         
         if (i % 10 == 0) {
             printf("After %d insertions: %zu buckets\n", 
                    i+1, float_dict_alloc(dict));
         }
     }
     
     free_float_dict(dict);

  Output::

     Initial allocation: 16 buckets
     After 1 insertions: 16 buckets
     After 11 insertions: 32 buckets
     After 21 insertions: 32 buckets

float_dict_hash_size
~~~~~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dict_hash_size(const dict_f* dict)

  Returns the total number of key-value pairs in the dictionary.

  :param dict: Target dictionary
  :returns: Number of key-value pairs, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example showing relationship between metrics:

  .. code-block:: c

     dict_f* dict = init_float_dict();
     
     insert_float_dict(dict, "a", 1.0f);
     insert_float_dict(dict, "b", 2.0f);
     insert_float_dict(dict, "c", 3.0f);
     
     printf("Dictionary metrics:\n");
     printf("  Total buckets allocated: %zu\n", float_dict_alloc(dict));
     printf("  Buckets containing items: %zu\n", float_dict_size(dict));
     printf("  Total key-value pairs: %zu\n", float_dict_hash_size(dict));
     
     // Remove one item
     pop_float_dict(dict, "b");
     
     printf("\nAfter removing one item:\n");
     printf("  Total buckets allocated: %zu\n", float_dict_alloc(dict));
     printf("  Buckets containing items: %zu\n", float_dict_size(dict));
     printf("  Total key-value pairs: %zu\n", float_dict_hash_size(dict));
     
     free_float_dict(dict);

  Output::

     Dictionary metrics:
       Total buckets allocated: 16
       Buckets containing items: 3
       Total key-value pairs: 3

     After removing one item:
       Total buckets allocated: 16
       Buckets containing items: 2
       Total key-value pairs: 2

merge_float_dict
~~~~~~~~~~~~~~~~
.. c:function:: dict_f* merge_float_dict(const dict_f* dict1, const dict_f* dict2, bool overwrite)

   Merges two dictionaries into a new dictionary. The resulting dictionary contains all 
   entries from both input dictionaries. If a key exists in both dictionaries:

   - If ``overwrite`` is ``true``, the value from ``dict2`` replaces the value from ``dict1``.
   - If ``overwrite`` is ``false``, the original value from ``dict1`` is preserved.

   Neither ``dict1`` nor ``dict2`` is modified by this operation.

   :param dict1: First input dictionary
   :param dict2: Second input dictionary
   :param overwrite: If true, dict2 values overwrite dict1 values on key conflicts
   :returns: New dictionary containing merged entries, or NULL on failure
   :raises: Sets errno to EINVAL for NULL inputs, or propagates errors from underlying operations

   Example:

   .. code-block:: c

      dict_f* dict1 FDICT_GBC = init_float_dict();
      dict_f* dict2 FDICT_GBC = init_float_dict();
      
      insert_float_dict(dict1, "temperature", 25.0f);
      insert_float_dict(dict1, "humidity", 40.0f);

      insert_float_dict(dict2, "humidity", 45.0f);  // Key conflict
      insert_float_dict(dict2, "pressure", 1012.5f);

      // Merge with overwrite
      dict_f* merged FDICT_GBC = merge_float_dict(dict1, dict2, true);

      printf("Merged Dictionary:\n");
      foreach_float_dict(merged, print_entry, NULL);

   Example output::

      Merged Dictionary:
      temperature: 25.00
      humidity: 45.00
      pressure: 1012.50

   Example without overwrite:

   .. code-block:: c

      dict_f* merged_no_overwrite FDICT_GBC = merge_float_dict(dict1, dict2, false);

      printf("Merged Dictionary (no overwrite):\n");
      foreach_float_dict(merged_no_overwrite, print_entry, NULL);

   Example output::

      Merged Dictionary (no overwrite):
      temperature: 25.00
      humidity: 40.00
      pressure: 1012.50

   Notes:

   - The caller is responsible for freeing the returned merged dictionary.
   - If memory allocation fails at any point, NULL is returned and errno is set appropriately.

clear_float_dict
~~~~~~~~~~~~~~~~
.. c:function:: bool clear_float_dict(dict_f* dict)

   Removes all key-value pairs from the dictionary without freeing the dictionary itself.
   After calling this function, the dictionary remains allocated and can be reused
   without reinitialization.

   :param dict: Target dictionary to clear
   :returns: true if all entries were cleared successfully, false otherwise
   :raises: Sets errno to EINVAL for NULL input

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      
      insert_float_dict(dict, "temperature", 23.5f);
      insert_float_dict(dict, "pressure", 1013.2f);

      printf("Before clearing:\n");
      printf("  Total key-value pairs: %zu\n", float_dict_hash_size(dict));
      
      clear_float_dict(dict);

      printf("After clearing:\n");
      printf("  Total key-value pairs: %zu\n", float_dict_hash_size(dict));

   Output::

      Before clearing:
        Total key-value pairs: 2
      After clearing:
        Total key-value pairs: 0

   Notes:

   - The dictionary structure and its internal hash table remain allocated after clearing.
   - This function is useful when reusing an existing dictionary without reallocating it.

copy_float_dict
~~~~~~~~~~~~~~~
.. c:function:: dict_f* copy_float_dict(const dict_f* dict)

   Creates a deep copy of a dictionary, duplicating all key-value pairs into a new dictionary.
   Changes made to the copied dictionary do not affect the original.

   :param dict: Target dictionary to copy
   :returns: Pointer to new dictionary containing copies of all entries, or NULL on error
   :raises: Sets errno to EINVAL for NULL input, or ENOMEM for allocation failure

   Example:

   .. code-block:: c

      dict_f* original FDICT_GBC = init_float_dict();
      insert_float_dict(original, "sensor1", 10.5f);
      insert_float_dict(original, "sensor2", 12.3f);

      dict_f* duplicate FDICT_GBC = copy_float_dict(original);

      printf("Original Dictionary:\n");
      foreach_float_dict(original, print_entry, NULL);

      printf("\nCopied Dictionary:\n");
      foreach_float_dict(duplicate, print_entry, NULL);

      // Modify original
      update_float_dict(original, "sensor1", 99.9f);

      printf("\nAfter modifying original:\n");
      printf("Original Dictionary:\n");
      foreach_float_dict(original, print_entry, NULL);
      printf("Copied Dictionary (unchanged):\n");
      foreach_float_dict(duplicate, print_entry, NULL);

   Output::

      Original Dictionary:
      sensor1: 10.50
      sensor2: 12.30

      Copied Dictionary:
      sensor1: 10.50
      sensor2: 12.30

      After modifying original:
      Original Dictionary:
      sensor1: 99.90
      sensor2: 12.30
      Copied Dictionary (unchanged):
      sensor1: 10.50
      sensor2: 12.30

   Notes:

   - The caller is responsible for freeing the copied dictionary using `free_float_dict`.
   - Copying a NULL dictionary returns NULL and sets errno to EINVAL.

has_key_float_dict
~~~~~~~~~~~~~~~~~~
.. c:function:: bool has_key_float_dict(const dict_f* dict, const char* key)

   Checks if a specified key exists in the dictionary without retrieving its value.

   :param dict: Target dictionary to search
   :param key: String key to look for
   :returns: true if key exists, false otherwise
   :raises: Sets errno to EINVAL for NULL input

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      insert_float_dict(dict, "temperature", 23.5f);
      insert_float_dict(dict, "pressure", 1012.8f);

      if (has_key_float_dict(dict, "temperature")) {
          printf("'temperature' exists in the dictionary\n");
      } else {
          printf("'temperature' not found\n");
      }

      if (!has_key_float_dict(dict, "humidity")) {
          printf("'humidity' not found in the dictionary\n");
      }

   Output::

      'temperature' exists in the dictionary
      'humidity' not found in the dictionary

   Notes:

   - This function does not modify the dictionary.
   - Useful for checking the presence of a key before inserting or updating.


Iterator Support
----------------

dict_iterator
~~~~~~~~~~~~~
.. c:type:: void (*dict_iterator)(const char* key, float value, void* user_data)

   Function type for dictionary iteration callbacks.

   :param key: Current key being visited
   :param value: Value associated with current key
   :param user_data: User-provided context data

foreach_float_dict
~~~~~~~~~~~~~~~~~~
.. c:function:: bool foreach_float_dict(const dict_f* dict, dict_iterator iter, void* user_data)

   Iterates over all key-value pairs in the dictionary, calling the provided
   callback function for each pair.

   :param dict: Target dictionary
   :param iter: Iterator callback function
   :param user_data: Optional user data passed to callback
   :returns: true if iteration completed, false on error
   :raises: Sets errno to EINVAL for NULL dict or iter

   Example:

  .. code-block:: c

     // Basic print callback
     void print_entry(const char* key, float value, void* user_data) {
         printf("%s: %.2f\n", key, value);
     }

     // Create and populate dictionary
     dict_f* dict = init_float_dict();
     insert_float_dict(dict, "temperature", 23.5f);
     insert_float_dict(dict, "humidity", 45.0f);
     insert_float_dict(dict, "pressure", 1013.2f);

     printf("Dictionary contents:\n");
     foreach_float_dict(dict, print_entry, NULL);

     free_float_dict(dict);

  Output::

     Dictionary contents:
     temperature: 23.50
     humidity: 45.00
     pressure: 1013.20

  Example with data accumulation:

  .. code-block:: c

     // Structure for accumulating statistics
     typedef struct {
         float sum;
         size_t count;
         float min;
         float max;
     } stats_data;

     // Callback to gather statistics
     void gather_stats(const char* key, float value, void* user_data) {
         stats_data* stats = (stats_data*)user_data;
         stats->sum += value;
         stats->count++;
         if (value < stats->min) stats->min = value;
         if (value > stats->max) stats->max = value;
     }

     // Print callback with formatted output
     void print_with_prefix(const char* key, float value, void* user_data) {
         const char* prefix = (const char*)user_data;
         printf("%s%s: %.2f\n", prefix, key, value);
     }

     int main() {
         dict_f* dict = init_float_dict();
         
         // Add some sensor readings
         insert_float_dict(dict, "sensor1", 10.5f);
         insert_float_dict(dict, "sensor2", 15.7f);
         insert_float_dict(dict, "sensor3", 12.3f);
         insert_float_dict(dict, "sensor4", 8.9f);
         
         // Print with custom prefix
         printf("Raw Readings:\n");
         const char* prefix = "  Reading ";
         foreach_float_dict(dict, print_with_prefix, (void*)prefix);
         
         // Calculate statistics
         stats_data stats = {0.0f, 0, FLT_MAX, -FLT_MAX};
         foreach_float_dict(dict, gather_stats, &stats);
         
         printf("\nStatistics:\n");
         printf("  Number of readings: %zu\n", stats.count);
         printf("  Average reading: %.2f\n", stats.sum / stats.count);
         printf("  Minimum reading: %.2f\n", stats.min);
         printf("  Maximum reading: %.2f\n", stats.max);
         
         free_float_dict(dict);
         return 0;
     }

  Output::

     Raw Readings:
       Reading sensor1: 10.50
       Reading sensor2: 15.70
       Reading sensor3: 12.30
       Reading sensor4: 8.90

     Statistics:
       Number of readings: 4
       Average reading: 11.85
       Minimum reading: 8.90
       Maximum reading: 15.70

  Example with error handling:

  .. code-block:: c

     bool iterate_dict(dict_f* dict, dict_iterator iter, void* user_data) {
         if (!foreach_float_dict(dict, iter, user_data)) {
             if (errno == EINVAL) {
                 printf("Error: Invalid dictionary or iterator\n");
             } else {
                 printf("Unknown error during iteration\n");
             }
             return false;
         }
         return true;
     }

     // Test error cases
     dict_f* dict = init_float_dict();
     insert_float_dict(dict, "test", 1.0f);

     printf("Testing NULL dictionary:\n");
     iterate_dict(NULL, print_entry, NULL);

     printf("\nTesting NULL iterator:\n");
     iterate_dict(dict, NULL, NULL);

     free_float_dict(dict);

  Output::

     Testing NULL dictionary:
     Error: Invalid dictionary or iterator

     Testing NULL iterator:
     Error: Invalid dictionary or iterator

Key/Value Extraction
--------------------

get_keys_float_dict
~~~~~~~~~~~~~~~~~~~
.. c:function:: string_v* get_keys_float_dict(const dict_f* dict)

   Returns a ``string_v`` object containing all keys in the dictionary.  The developer 
   must use the ``c_string.h`` header file with this function.  A full description 
   of the ``string_v`` object can be found at `C String Library <https://c-string.readthedocs.io/en/latest/>`_ 

   :param dict: Target dictionary
   :returns: Vector containing all keys, or NULL on error
   :raises: Sets errno to EINVAL for NULL input, ENOMEM for allocation failure

   .. code-block:: c
    
      #include "c_float.h"
      #include "c_string.h"


      dict_f* dict = init_float_dict();
      insert_float_dict("One", 1.1f);
      insert_float_dict("Two", 2.2f);
      insert_float_dict("Three", 3.3f);
      insert_float_dict("Four", 4.4f);
      string_v* values = get_keys_float_dict(dict);

      printf("Vector has %zu indices", s_size(values));
      printf("[ ");
      for (size_t i = 1; i < s_size(values) - 1; i++) {
          printf("%f, ", str_vector_index(vec, i));
      }
      printf("%f ]", str_vector_index(vec, s_size(values)));

      free_float_dict(dict);
      free_str_vector(dict);

   .. code-block::

      Vector has 4 indices
      [ One, Two, Three, Four ]

get_values_float_dict
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: float_v* get_values_float_dict(const dict_f* dict)

   Returns a ``float_v`` object containing all values in the dictionary as a 
   dynamically allocated vector.
   The user should consult with the :ref:`Float Vector <vector_file>` documentation
   to understand how to utilizie the ``float_v`` object and how to properly 
   free all vector memory.  The ``float_v`` object is contained within the 
   context of the ``c_float.h`` header file

   :param dict: Target dictionary
   :returns: Vector containing all values, or NULL on error
   :raises: Sets errno to EINVAL for NULL input, ENOMEM for allocation failure

   Example:

   .. code-block:: c

      dict_f* dict = init_float_dict();
      insert_float_dict("One", 1.1f);
      insert_float_dict("Two", 2.2f);
      insert_float_dict("Three", 3.3f);
      insert_float_dict("Four", 4.4f);
      float_v* values = get_values_float_dict(dict);

      printf("Vector has %zu indices", f_size(values));
      printf("[ ");
      for (size_t i = 1; i < f_size(values) - 1; i++) {
          printf("%f, ", float_vector_index(vec, i));
      }
      printf("%f ]", float_vector_index(vec, f_size(values)));

      free_float_dict(dict);
      free_float_vector(dict);

   .. code-block::

      Vector has 4 indices
      [ 1.10000, 2.20000, 3.30000, 4.40000 ]

Float Vector Dictionary Overview
================================

A float vector dictionary (`dict_fv`) maps string keys to dynamically allocated float vector (`float_v`) values. 
It supports efficient retrieval and manipulation of float arrays via their associated keys, 
with support for automatic memory management, key-based lookup, and vector pointer access.

Key Features
------------

* Maps strings to dynamically allocated float vectors (`float_v`)
* Pointer access to vectors for compatibility with all `float_v` functions
* Automatic resizing based on load factor
* Clean separation of memory ownership and safety
* Optional automatic cleanup using `FDICTV_GBC`

When to Use Float Vector Dictionaries
-------------------------------------

* Managing named arrays of numerical data (e.g., sensor histories, grouped data)
* Passing vector references into numerical and statistical functions
* Implementing high-performance lookup tables for float vectors
* Maintaining structured mappings of string labels to float arrays

Performance Characteristics
---------------------------

* Lookup and insert: O(1) average time using chained hashing
* Optimized for dynamic arrays only — `STATIC` arrays are not allowed
* Supports full dictionary and vector lifecycle management

Data Types
==========

The following are derived data types used in the implementation of float dictionaries.

dict_fv
-------
Opaque type representing a float vector dictionary that maps string keys to 
`float_v*` values.

.. code-block:: c

   typedef struct dict_fv dict_fv;

   // Do not access internal structure directly — use provided API

Core Functions
==============

The float vector dictionary implementation provides a complete set of functions 
for dictionary manipulation. All functions are declared in the ``c_float.h`` 
header file.

Initialization and Memory Management
------------------------------------

The functions and Macros in this section control the creation, memory allocation,
and destruction of ``dict_fv`` data types.

init_floatv_dict
~~~~~~~~~~~~~~~~
.. c:function:: dict_f* init_floatv_dict(void)

   Initializes a new empty dictionary with default initial capacity.

   :returns: Pointer to new dict_fv object, or NULL on allocation failure
   :raises: Sets errno to ENOMEM if memory allocation fails

   Example:

   .. code-block:: c

      dict_fv* dict = init_floatv_dict();
      if (!dict) {
          fprintf(stderr, "Failed to initialize dictionary\\n");
          return 1;
      }
      
      // Use dictionary...
      
      free_floatv_dict(dict);

free_floatv_dict
~~~~~~~~~~~~~~~~
.. c:function:: void free_floatv_dict(dict_f* dict)

   Frees all memory associated with a dictionary, including all stored keys and nodes.
   
   :param dict: Dictionary to free
   
   Example:

   .. code-block:: c

      dict_fv* dict = init_floatv_dict();
      // Use dictionary...
      free_floatv_dict(dict);
      dict = NULL;  // Good practice to avoid dangling pointer

FDICTV_GBC
~~~~~~~~~~
.. c:macro:: FDICTV_GBC

   Macro that enables automatic cleanup of dictionaries when they go out of scope.
   Only available when using GCC or Clang compilers.

   Example:

   .. code-block:: c

      void process_data(void) {
          FDICTV_GBC dict_f* dict = init_floatv_dict();
          // Use dictionary...
      }  // dict is automatically freed here

Data Insertion and Update
-------------------------
The following functions can be used to insert vectors to the ``dict_fv`` data 
type.

create_floatv_dict 
~~~~~~~~~~~~~~~~~~
.. c:function:: bool create_floatv_dict(dict_fv* dict, char* key, size_t size)

   Creates a new dynamically allocated vector within the dictionary and assigns 
   it to a string literal key.

   :param dict: Target dictionary
   :param key: String key to insert
   :param size: An estimate for the size of the dynamically allocated array
   :returns: true if insertion successful, false if key exists or error occurs
   :raises: Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure,
            EEXIST if key already exists

   Example:

   .. code-block:: c

      dict_f* dict FDICTV_GBC = init_floatv_dict();
      
      if (create_floatv_dict(dict, "temperature", 20)) {
          printf("Array of size %d assigned to the key '%s'\n", 20, "temperature");
      }
     
   .. code-block:: bash

      Array of size 20 assigned to the key 'temperature'

insert_floatv_dict
~~~~~~~~~~~~~~~~~~
.. c:function:: bool insert_floatv_dict(dict_fv* dict, const char* key, float_v* vec)

   Inserts an existing dynamically allocated float vector into the dictionary under the specified key.
   This function allows advanced users to create and manipulate `float_v` objects independently before 
   assigning them to a key-value pair in the dictionary.

   Unlike :c:func:`create_floatv_dict`, which allocates a new vector internally, this function
   accepts ownership of an already allocated dynamic vector and associates it with the provided key.
   Once this dictionary assumes ownership of the vector, you should only handle data 
   within the vector through this dictionary to avoid memory issues.

   :param dict: Target dictionary
   :param key: String key to associate with the float vector
   :param vec: A dynamically allocated float vector (`float_v*`) to insert
   :returns: true on successful insertion, false on failure
   :raises: 
     - `EINVAL` if any argument is NULL  
     - `EEXIST` if the key already exists in the dictionary  
     - `EPERM` if the float vector was not dynamically allocated  
     - `ENOMEM` if internal memory allocation fails

   .. important::

      This function only accepts dynamically allocated float vectors. 
      Attempting to insert a statically allocated vector will fail with `errno` set to `EPERM`.

   Example:

   .. code-block:: c

      dict_fv* dict FDICTV_GBC = init_floatv_dict();
      
      float_v* vec = init_float_vector(10);
      for (int i = 0; i < 10; i++) {
          push_back_float_vector(vec, (float)i);
      }

      if (!insert_floatv_dict(dict, "acceleration", vec)) {
          perror("Insert failed");
          free_float_vector(vec);  // Must be freed manually if insert fails
      }

      // Use dictionary...

   Example Output:

   .. code-block:: bash

      (no output if success; if failure due to static vector:)
      Insert failed: Operation not permitted

   Notes:

   - This function does not duplicate the vector — ownership of `vec` is transferred to the dictionary.
   - The dictionary will automatically free the vector when it is removed or when the dictionary is destroyed.
   - Use this function when the vector has been created and populated prior to insertion.

Data Retrieval
--------------
These functions allow a user to access data from ``dict_fv`` data types.

return_floatv_pointer
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: float_v* return_floatv_pointer(dict_fv* dict, const char* key)

   Retrieves a pointer to the float vector associated with a given key.

   This function enables direct access to the underlying `float_v` object for use
   in numerical operations or vector manipulation routines. It is particularly
   useful for passing the vector to other library functions that operate on
   `float_v*` types.

   :param dict: Target dictionary
   :param key: Key string whose associated vector should be returned
   :returns: Pointer to the associated `float_v` object, or NULL on error
   :raises: 
     - `EINVAL` if either `dict` or `key` is NULL  
     - `ENOENT` if the key is not found in the dictionary

   Example:

   .. code-block:: c

      dict_fv* dict FDICTV_GBC = init_floatv_dict();
      create_floatv_dict(dict, "velocity", 10);
      float_v* v = return_floatv_pointer(dict, "velocity");

      if (!v) {
          perror("Failed to retrieve vector");
          return 1;
      }

      push_back_float_vector(v, 12.34f);
      printf("velocity[0] = %f\n", float_vector_index(v, 0));

   Output:

   .. code-block:: bash

      velocity[0] = 12.340000

   Notes:

   - The returned pointer is owned by the dictionary — do not free it manually.
   - Always check the return value for NULL before using the pointer.
   - If the key does not exist, `errno` is set to `ENOENT` and NULL is returned.

Data Removal
------------

pop_floatv_dict
~~~~~~~~~~~~~~~
.. c:function:: bool pop_floatv_dict(dict_fv* dict, const char* key)

   Removes a key-vector pair from the dictionary and deallocates the associated vector.

   This function deletes the `float_v` vector associated with the given key and removes
   the key from the hash table. It is a safe way to free memory associated with specific
   entries without affecting the rest of the dictionary.

   :param dict: Target dictionary
   :param key: Key string to remove
   :returns: true if the entry was found and removed, false otherwise
   :raises: 
     - `EINVAL` if `dict` or `key` is NULL  
     - `ENOENT` if the key does not exist in the dictionary

   Example:

   .. code-block:: c

      dict_fv* dict FDICTV_GBC = init_floatv_dict();
      create_floatv_dict(dict, "data", 5);

      // Add some values
      float_v* v = return_floatv_pointer(dict, "data");
      push_back_float_vector(v, 3.14f);
      push_back_float_vector(v, 2.71f);

      // Remove the entry
      if (pop_floatv_dict(dict, "data")) {
          printf("Key 'data' and associated vector removed successfully\n");
      }

   Output:

   .. code-block:: bash

      Key 'data' and associated vector removed successfully

   Notes:

   - If the key is not found, the function returns false and sets `errno` to `ENOENT`.
   - The memory for both the vector and key string is freed automatically.
   - After removal, attempting to access the key again will result in an error.

Utility Functions
-----------------

.. _floatv-dict-size-func:

float_dictv_size
~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dictv_size(const dict_fv* dict)

  Returns the number of non-empty buckets in the float vector dictionary.  
  The user can also use the :ref:`f_size <f-size-macro>` Generic Macro 
  in place of this function.

  :param dict: Target float vector dictionary
  :returns: Number of non-empty buckets, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example with distinct keys:

  .. code-block:: c

     dict_fv* dict = init_floatv_dict();

     create_floatv_dict(dict, "temperature", 10);
     create_floatv_dict(dict, "humidity", 5);
     create_floatv_dict(dict, "pressure", 3);

     printf("Number of buckets used: %zu\n", float_dictv_size(dict));
     printf("Total key-value pairs: %zu\n", float_dictv_hash_size(dict));

     free_floatv_dict(dict);

  Output::

     Number of buckets used: 3
     Total key-value pairs: 3

  Example with hash collisions:

  .. code-block:: c

     dict_fv* dict = init_floatv_dict();

     create_floatv_dict(dict, "aaa", 1);
     create_floatv_dict(dict, "bbb", 1);
     create_floatv_dict(dict, "ccc", 1); // These might hash to the same bucket

     printf("Number of buckets used: %zu\n", float_dictv_size(dict));
     printf("Total key-value pairs: %zu\n", float_dictv_hash_size(dict));

     free_floatv_dict(dict);

  Output::

     Number of buckets used: 1
     Total key-value pairs: 3

.. _floatv-dict-alloc-func:

float_dictv_alloc 
~~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dictv_alloc(const dict_fv* dict)

Returns the total number of hash buckets allocated in the float vector dictionary.
The user can also use the :ref:f_alloc <f-alloc-macro> Generic Macro
in place of this function.

:param dict: Target float vector dictionary
:returns: Total number of buckets, or SIZE_MAX on error
:raises: Sets errno to EINVAL for NULL input

Example showing automatic resizing:

.. code-block:: c 

    dict_fv* dict = init_floatv_dict();

    printf("Initial allocation: %zu buckets\n", float_dictv_alloc(dict));
    // printf("Initial allocation: %zu buckets\n", f_alloc(dict)); // Optional macro use

    char key[20];
    for (int i = 0; i < 30; ++i) {
        sprintf(key, "key%d", i);
        create_floatv_dict(dict, key, 4);

        if (i % 10 == 0) {
            printf("After %d insertions: %zu buckets\n", 
                   i + 1, float_dictv_alloc(dict));
        }
    }

    free_floatv_dict(dict);

Output:: 

   Initial allocation: 16 buckets
   After 1 insertions: 16 buckets
   After 11 insertions: 32 buckets
   After 21 insertions: 48 buckets
  
float_dictv_hash_size 
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dictv_hash_size(const dict_fv* dict)

  Returns the total number of key-vector pairs in the dictionary.

  :param dict: Target float vector dictionary
  :returns: Number of key-value pairs, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example with additions and deletions:

  .. code-block:: c

     dict_fv* dict = init_floatv_dict();

     create_floatv_dict(dict, "sensor1", 5);
     create_floatv_dict(dict, "sensor2", 5);
     create_floatv_dict(dict, "sensor3", 5);

     printf("Initial metrics:\n");
     printf("  Total allocated: %zu\n", float_dictv_alloc(dict));
     printf("  Buckets used:    %zu\n", float_dictv_size(dict));
     printf("  Total entries:   %zu\n", float_dictv_hash_size(dict));

     pop_floatv_dict(dict, "sensor2");

     printf("\nAfter removal:\n");
     printf("  Total allocated: %zu\n", float_dictv_alloc(dict));
     printf("  Buckets used:    %zu\n", float_dictv_size(dict));
     printf("  Total entries:   %zu\n", float_dictv_hash_size(dict));

     free_floatv_dict(dict);

  Output::

     Initial metrics:
       Total allocated: 16
       Buckets used:    3
       Total entries:   3

     After removal:
       Total allocated: 16
       Buckets used:    2
       Total entries:   2

merge_floatv_dict
~~~~~~~~~~~~~~~~~
.. c:function:: dict_fv* merge_floatv_dict(const dict_fv* dict1, const dict_fv* dict2, bool overwrite)

   Merges two float vector dictionaries into a new dictionary. The resulting dictionary 
   contains all key-vector pairs from both input dictionaries. If a key exists in both:

   - If ``overwrite`` is ``true``, the vector from ``dict2`` replaces the vector from ``dict1``.
   - If ``overwrite`` is ``false``, the vector from ``dict1`` is preserved.

   All vectors are deep copied. Neither input dictionary is modified.

   :param dict1: First dictionary to merge
   :param dict2: Second dictionary to merge
   :param overwrite: Determines whether values from ``dict2`` replace existing keys in ``dict1``
   :returns: Newly allocated merged dictionary, or NULL on error
   :raises: Sets errno to EINVAL if inputs are NULL, EPERM if non-dynamic vectors are found, or ENOMEM

   Example:

   .. code-block:: c

      dict_fv* dict1 = init_floatv_dict();
      dict_fv* dict2 = init_floatv_dict();

      float_v* vec1 = init_float_vector(1);
      float_v* vec2 = init_float_vector(1);
      push_back_float_vector(vec1, 3.14f);
      push_back_float_vector(vec2, 6.28f);

      insert_floatv_dict(dict1, "value", vec1);
      insert_floatv_dict(dict2, "value", vec2);

      dict_fv* merged = merge_floatv_dict(dict1, dict2, true);

      float_v* result = return_floatv_pointer(merged, "value");
      printf("Merged value: %.2f\n", float_vector_index(result, 0));

      free_floatv_dict(dict1);
      free_floatv_dict(dict2);
      free_floatv_dict(merged);

   Output::

      Merged value: 6.28

   Notes:

   - This function guarantees deep copies of all vectors.
   - Caller is responsible for freeing the merged dictionary.
   - All vectors must be dynamically allocated.

clear_floatv_dict
~~~~~~~~~~~~~~~~~
.. c:function:: void clear_floatv_dict(dict_fv* dict)

   Removes all key-vector pairs from the dictionary without deallocating the dictionary structure itself.
   This operation is useful when reusing the same dictionary after clearing its contents.

   :param dict: Dictionary to clear
   :raises: Sets errno to EINVAL if the input is NULL

   Example:

   .. code-block:: c

      dict_fv* dict = init_floatv_dict();
      create_floatv_dict(dict, "temperature", 3);
      create_floatv_dict(dict, "pressure", 2);

      printf("Before clear: %zu keys\n", float_dictv_hash_size(dict));

      clear_floatv_dict(dict);

      printf("After clear: %zu keys\n", float_dictv_hash_size(dict));

      free_floatv_dict(dict);

   Output::

      Before clear: 2 keys
      After clear: 0 keys

   Notes:

   - All vectors and keys are freed.
   - The dictionary is reusable after this operation.

copy_floatv_dict
~~~~~~~~~~~~~~~~
.. c:function:: dict_fv* copy_floatv_dict(const dict_fv* original)

   Creates a deep copy of a float vector dictionary. Each vector in the copy is a
   newly allocated clone of the original vector.

   :param original: Dictionary to copy
   :returns: A newly allocated dictionary containing deep copies of all entries
   :raises: Sets errno to EINVAL if input is NULL, or ENOMEM if memory allocation fails

   Example:

   .. code-block:: c

      dict_fv* original = init_floatv_dict();
      float_v* vec = init_float_vector(1);
      push_back_float_vector(vec, 9.81f);
      insert_floatv_dict(original, "gravity", vec);

      dict_fv* copy = copy_floatv_dict(original);

      float_v* copied_vec = return_floatv_pointer(copy, "gravity");
      printf("Copied value: %.2f\n", float_vector_index(copied_vec, 0));

      free_floatv_dict(original);
      free_floatv_dict(copy);

   Output::

      Copied value: 9.81

   Notes:

   - The copied dictionary is independent of the original.
   - Useful for isolating dictionary state across function calls or processing steps.

has_key_floatv_dict
~~~~~~~~~~~~~~~~~~~
.. c:function:: bool has_key_floatv_dict(const dict_fv* dict, const char* key)

   Checks if a given key exists in the float vector dictionary.

   :param dict: Dictionary to query
   :param key: Key string to look for
   :returns: true if key exists, false otherwise
   :raises: Sets errno to EINVAL for NULL inputs

   Example:

   .. code-block:: c

      dict_fv* dict = init_floatv_dict();
      create_floatv_dict(dict, "velocity", 10);

      if (has_key_floatv_dict(dict, "velocity")) {
          printf("Found 'velocity'\n");
      }

      if (!has_key_floatv_dict(dict, "altitude")) {
          printf("'altitude' not found\n");
      }

      free_floatv_dict(dict);

   Output::

      Found 'velocity'
      'altitude' not found

   Notes:

   - This function does not retrieve or modify the vector.
   - Can be used to check for key presence before insertion or access.

Iterator Support
----------------

dict_fv_iterator
~~~~~~~~~~~~~~~~
.. c:type:: void (*dict_fv_iterator)(const char* key, const float_v* value, void* user_data)

   Function type for float vector dictionary iteration callbacks.

   :param key: Current key being visited
   :param value: Pointer to the associated float_v vector
   :param user_data: Optional user-provided context data

foreach_floatv_dict
~~~~~~~~~~~~~~~~~~~
.. c:function:: bool foreach_floatv_dict(const dict_fv* dict, dict_fv_iterator iter, void* user_data)

   Iterates over all key-vector pairs in the dictionary, calling the provided
   callback function for each entry.

   :param dict: Target float vector dictionary
   :param iter: Iterator callback function
   :param user_data: Optional user context data passed to the callback
   :returns: true if iteration completes successfully, false on error
   :raises: Sets errno to EINVAL if dict or iter is NULL

   Example:

   .. code-block:: c

      void print_vector_entry(const char* key, const float_v* vec, void* user_data) {
          printf("%s: [", key);
          for (size_t i = 0; i < f_size(vec); ++i) {
              printf("%.2f%s", float_vector_index(vec, i), (i < f_size(vec) - 1) ? ", " : "");
          }
          printf("]\n");
      }

      dict_fv* dict = init_floatv_dict();
      float_v* vec1 = init_float_vector(3);
      push_back_float_vector(vec1, 1.1f);
      push_back_float_vector(vec1, 2.2f);
      push_back_float_vector(vec1, 3.3f);
      insert_floatv_dict(dict, "set1", vec1);

      float_v* vec2 = init_float_vector(2);
      push_back_float_vector(vec2, 4.4f);
      push_back_float_vector(vec2, 5.5f);
      insert_floatv_dict(dict, "set2", vec2);

      printf("Dictionary contents:\n");
      foreach_floatv_dict(dict, print_vector_entry, NULL);
      free_floatv_dict(dict);

   Output::

      Dictionary contents:
      set1: [1.10, 2.20, 3.30]
      set2: [4.40, 5.50]

   Example with data aggregation:

   .. code-block:: c

      typedef struct {
          float total_sum;
          size_t total_count;
      } agg_data;

      void sum_elements(const char* key, const float_v* vec, void* user_data) {
          agg_data* stats = (agg_data*)user_data;
          for (size_t i = 0; i < f_size(vec); ++i) {
              stats->total_sum += float_vector_index(vec, i);
              stats->total_count++;
          }
      }

      dict_fv* dict = init_floatv_dict();
      float_v* vec = init_float_vector(4);
      for (float i = 0; i < 4.0f; i++)
          push_back_float_vector(vec, i);
      insert_floatv_dict(dict, "numbers", vec);

      agg_data stats = {0};
      foreach_floatv_dict(dict, sum_elements, &stats);
      printf("Sum: %.2f, Count: %zu\n", stats.total_sum, stats.total_count);
      free_floatv_dict(dict);

   Output::

      Sum: 6.00, Count: 4

   Example with error handling:

   .. code-block:: c

      bool safe_iteration(dict_fv* dict, dict_fv_iterator iter, void* user_data) {
          if (!foreach_floatv_dict(dict, iter, user_data)) {
              if (errno == EINVAL) {
                  fprintf(stderr, "Invalid dictionary or iterator\n");
              }
              return false;
          }
          return true;
      }

      void dummy_iter(const char* key, const float_v* vec, void* user_data) {}

      int main(void) {
          dict_fv* dict = init_floatv_dict();
          insert_floatv_dict(dict, "test", init_float_vector(1));

          safe_iteration(NULL, dummy_iter, NULL);  // NULL dictionary
          safe_iteration(dict, NULL, NULL);        // NULL iterator

          free_floatv_dict(dict);
          return 0;
      }

   Output::

      Invalid dictionary or iterator
      Invalid dictionary or iterator

   Notes:

   - The callback function is invoked once for each key-vector pair.
   - The `user_data` parameter can be used to accumulate statistics or context.

get_keys_floatv_dict
~~~~~~~~~~~~~~~~~~~~
.. c:function:: string_v* get_keys_floatv_dict(const dict_fv* dict)

   Returns a ``string_v`` object containing all keys from a float vector dictionary.
   Each key maps to a dynamically allocated ``float_v`` array in the original dictionary.

   The developer must include the ``c_string.h`` header file to use the ``string_v`` type.
   A full description of the ``string_v`` object can be found at 
   `C String Library <https://c-string.readthedocs.io/en/latest/>`_.

   :param dict: Target float vector dictionary
   :returns: Vector containing all keys, or NULL on error
   :raises: Sets errno to EINVAL for NULL input, or ENOMEM for memory allocation failure

   Example:

   .. code-block:: c

      #include "c_float.h"
      #include "c_string.h"

      dict_fv* dict = init_floatv_dict();
      create_floatv_dict(dict, "One", 3);
      create_floatv_dict(dict, "Two", 3);
      create_floatv_dict(dict, "Three", 3);
      create_floatv_dict(dict, "Four", 3);

      string_v* keys = get_keys_floatv_dict(dict);

      printf("Vector has %zu indices\n", s_size(keys));
      printf("[ ");
      for (size_t i = 0; i < s_size(keys) - 1; i++) {
          printf("%s, ", str_vector_index(keys, i));
      }
      printf("%s ]\n", str_vector_index(keys, s_size(keys) - 1));

      free_floatv_dict(dict);
      free_str_vector(keys);

   Example output::

      Vector has 4 indices
      [ One, Two, Three, Four ]

Generic Macro Overview
======================

The C language's ``_Generic`` keyword, introduced in C11, enables type-safe generic programming
through compile-time type selection. This feature allows the creation of macros that can
adapt their behavior based on argument types while maintaining type safety and avoiding the
pitfalls of traditional C macros.

The macros in this library leverage ``_Generic`` to provide intuitive interfaces that work
seamlessly with both string literals (char*) and string_t objects. This type-safe approach
ensures proper function selection at compile time, reducing runtime overhead and preventing
type-related errors.

Benefits of Generic Macros
--------------------------
- Type safety: Compiler catches type mismatches at compile time
- Code clarity: Single interface for multiple types
- Performance: No runtime overhead for type checking
- Maintainability: Centralized type handling logic

Size Inspection Macros
----------------------
These macros provide several advantages:

- **Consistency**: Same syntax for both string types
- **Type Safety**: Compiler ensures correct function selection
- **Code Clarity**: Eliminates need to remember type-specific function names
- **Maintainability**: Changes to underlying functions only need updates in one place

.. _f-size-macro:

f_size
~~~~~~
A generic macro that provides a uniform interface for checking the current populated
size of both float_v and xxxx objects.  This macro works with dynamically allocated 
vectors and statically allocated arrays.  It automatically selects:

- For float_v*: Uses float_vector_size to get string length

This macro simplifies size queries by providing a consistent interface regardless of
the underlying type. This Macro may be safely used in place of the 
:ref:`float_vector_size() <float-size-func>`, :ref:`float_dict_size() <float-dict-size-func>` 
and :ref:`float_dictv_size() <floatv-dict-size-func>` functions.

Example:

.. code-block:: c

   float_v* vec FLTVEC_GBC = init_float_vector(5);  // Capacity of 5
   push_back_float_vector(vec, 1);
   push_back_float_vector(vec, 2);
   push_back_float_vector(vec, 3);
   printf("Vector size: %zu\n", f_size(vec));

Result

.. code-block:: bash 

   Vector size: 5

.. _f-alloc-macro:

f_alloc
~~~~~~~
A generic macro that provides a uniform interface for checking the current allocation
size of both float_v xxxx objects. This macro works with dynamically allocated vectors 
and statically allocated arrays. It selects:

- For float_v*: Uses float_vector_alloc to get allocated vector capacity

This macro is particularly useful for capacity planning and debugging memory usage.
This Macro may be safely used in place of the :ref:`float_vector_alloc() <float-alloc-func>`
:ref:`float_dict_alloc() <float-dict-alloc-func>` and 
:ref:`float_dictv_alloc() <floatv-dict-alloc-func>` functions.

Example:

.. code-block:: c

    float_v* vec = init_float_array(5);  // Capacity of 5
    push_back_float_vector(vec, 1);
    push_back_float_vector(vec, 2);
    push_back_float_vector(vec, 3);
    printf("Array Length: %zu\n", f_size(vec));
    printf("Array Allocated Size: %zu\n", f_alloc(vec));

Result 

.. code-block:: bash 

   Array Length: 3
   Array Allocated Size: 5

Utility Functions 
=================
The functions listed below can be used to augment ``float`` object handling.

swap_float
----------
.. c:function:: void swap_float(float* a, float* b)

   Swaps the values of two float variables. Uses a temporary variable to ensure
   safe exchange of values.

   :param a: Pointer to first float value
   :param b: Pointer to second float value
   :raises: Sets errno to EINVAL for NULL input pointers

   Example:

   .. code-block:: c

      float x = 1.0f;
      float y = 2.0f;
      
      printf("Before swap: x = %.1f, y = %.1f\n", x, y);
      swap_float(&x, &y);
      printf("After swap:  x = %.1f, y = %.1f\n", x, y);

   Output::

      Before swap: x = 1.0, y = 2.0
      After swap:  x = 2.0, y = 1.0

   .. note::

      This utility function serves as a helper for other float manipulation functions
      but can also be used independently for general float value swapping operations.
