
.. _sl_list:

*******************
Singly Linked List
*******************

The ``slist.h`` header file provides an implementation of the Singly Linked List
data structure in C. A Singly Linked List is a sequential data structure
comprising nodes, where each node contains data and a pointer to the next node
in the sequence. This structure allows efficient insertion and deletion of
elements at any position in the list.

Singly Linked Lists are commonly used for:

- **Dynamic Data Storage**: Unlike arrays, Singly Linked Lists do not have a
  fixed size and can dynamically grow or shrink, making them ideal for
  applications where the number of data elements is unknown in advance.

- **Implementing Stacks and Queues**: Due to their dynamic nature and efficient
  insertion/deletion operations, Singly Linked Lists can be used to implement
  other data structures like stacks and queues.

- **Lightweight Data Management**: In environments with limited resources,
  Singly Linked Lists provide a way to manage data without the overhead of
  additional data structure complexities.

The ``slist.h`` library provides functions to create, manage, and interact with
Singly Linked Lists, enabling developers to handle data in a flexible and
efficient manner. Within this library, a Singly Linked List has the following
characteristics:

- Each node in a Singly Linked List contains a data field and a pointer to the
  next node, forming a unidirectional chain of nodes.

- The first node of the list is referred to as the "head," and the last node
  (which points to `NULL`) is known as the "tail."

- Operations such as insertion, deletion, and traversal can be performed more
  efficiently than in other linear data structures like arrays, especially
  when dealing with large datasets.

- Singly Linked Lists allow for insertion and deletion of nodes at any position
  in the list with a time complexity of O(1) for head insertions and O(n) for
  insertions at other positions, where `n` is the number of elements in the
  list.

This library offers a practical approach to managing linear collections of
data, giving programmers a versatile tool for a wide array of applications.


Structures
==========
The ``slist.h`` header file utilizes the following macro to create a struct that 
will contain the data inserted into a singly linked list.

.. role:: c(code)
   :language: c 

.. code-block:: c 

   #define SLIST_NODE(type, dat_type) \
       typedef struct dat_type { \
           type data; \
           struct dat_type *next; \
       } dat_type

Parameters 
----------

- :c:`type`: The data type that the struct will contain. 
- :c:`dat_type`: THe typdef name given to the struct.

Attributes 
----------

- :c:`data`: The data being contained in the struct 
- :c:`next`: A pointer to the next struct in the linked list.

In addition, the ``slist.h`` header file contains a macro that is used to create 
a struct that manages the attributes of a singly linked list.  

.. code-block:: c 

   #define SLIST_STRUCT(type, dat_type) \
       typedef struct { \
           size_t len; \
           struct type* head; \
       } dat_type

Parameters 
----------

- :c:`type`: The typedef name assigned to a node struct.
- :c:`dat_type`: The typedef name assigned to this struct.

Attributes 
----------

- :c:`len`: The length of the singly linked list (i.e. the number of nodes assigned to the list)
- :c:`head`: A pointer to the first node in the linked list.

.. _sllist_dat_type:

Derived Data Types 
==================
The ``SLIST_NODE`` macro is pre-instantiated within the ``slist.h`` header file 
for these data types.  The ``str`` data type is further described in the 
:ref:`String <string_struct>` section of this document.

.. code-block:: c

   SLIST_NODE(char, char_slnode);
   SLIST_NODE(unsigned char, uchar_slnode);
   SLIST_NODE(short int, short_slnode);
   SLIST_NODE(unsigned short int, ushort_slnode);
   SLIST_NODE(int, int_slnode);
   SLIST_NODE(unsigned int, uint_slnode);
   SLIST_NODE(long int, long_slnode);
   SLIST_NODE(unsigned long int, ulong_slnode);
   SLIST_NODE(long long int, llong_slnode);
   SLIST_NODE(unsigned long long int, ullong_slnode);
   SLIST_NODE(float, float_slnode);
   SLIST_NODE(double, double_slnode);
   SLIST_NODE(long double, ldouble_slnode);
   SLIST_NODE(bool, bool_slnode);
   SLIST_NODE(str*, string_slnode);

The ``SLIST_STRUCT`` macro is pre-instantiated within the ``slist.h`` header file 
for the following typedef names.  Notice the linkage between the ``SLIST_NODE`` 
data types that are contained in their respective ``SLIST_STRUCT`` instance.

.. code-block:: c

   SLIST_STRUCT(char_slnode, char_sl);
   SLIST_STRUCT(uchar_slnode, uchar_sl);
   SLIST_STRUCT(short_slnode, short_sl);
   SLIST_STRUCT(ushort_slnode, ushort_sl);
   SLIST_STRUCT(int_slnode, int_sl);
   SLIST_STRUCT(uint_slnode, uint_sl);
   SLIST_STRUCT(long_slnode, long_sl);
   SLIST_STRUCT(ulong_slnode, ulong_sl);
   SLIST_STRUCT(llong_slnode, llong_sl);
   SLIST_STRUCT(ullong_slnode, ullong_sl);
   SLIST_STRUCT(float_slnode, float_sl);
   SLIST_STRUCT(double_slnode, double_sl);
   SLIST_STRUCT(ldouble_slnode, ldouble_sl);
   SLIST_STRUCT(bool_slnode, bool_sl);
   SLIST_STRUCT(string_slnode, string_sl);

.. _sllist_type_enum:

Enum Data Types 
===============
This ``slist.h`` header file relies on the ``dtype`` enum stored in ``admin.h``
to identify the data type of an instantiated listtor.  The possible enum values
that can be used are shown below and are further described in the upcoming sections 
of this document.

.. code-block:: c

   typedef enum {
       dChar,  // Keyword for a char data type as an integer data type 
       dUChar,  // keyword for unsigned char data type as an integer data type 
       dShort,  // Keyword for short int data type 
       dUShort,  // Keyword for unsigned short int data type 
       dInt,  // Keyword for int data type 
       dUInt,  // Keyword for unsigned int data type 
       dLong,  // Keyword for long int data type 
       dULong,  // Keyword for unsigned long int data type 
       dLongLong,  // Keyword for long long int data type 
       dULongLong,  // Keyword for unsigned long long int data type 
       dFloat,  // Keyword for float data type 
       dDouble,  // Keyword for double data type 
       dLDouble,  // Keyword for long double data type 
       dBool,  // Keyword for bool data type 
       dString  // Keyword for str data type
   } dtype;

Initialize Singly Linked List 
=============================
The ``slist.h`` header file provides the ``init_llist`` function for initializing 
a singly linked list.  This function is essentially a function pointer, intelligently 
selecting the appropriate initialization function based on the specified data 
type.

.. code-block:: c

   type init_llist(dtype dat_type)()

Parameters 
----------

- :c:`dat_type`: The data type for the singly linked list, represented by a ``dtype`` as described in :ref:`Enum Data Types <sllist_type_enum>`.

Returns 
-------

- :c:`type`: A struct that manages the singly linked list of the specified type in the :ref:`Singly Linked List Type <sllist_dat_type>`

Error Handling 
--------------
The ``init_sllist`` macro has one primary mechanism, which is a failure to allocate
memory.  If this occurs, the underlying functions will return a NULL pointer in
place of the data struct, and will also set the value of ``errno`` to ``ENOMEM``.
The developer can check for any of these two conditions if an error occurred.

Possible error codes:

- ``ENOMEM``: Indicates a filure to allocate memory

Example
-------
Below is an example using the ``init_sllist`` function to create a singly linked 
list data structure of type ``float_sl``.

.. code-block:: c 

   #include "slist.h"

   int main() {
       float_sl* list = init_sllist(dFloat)();
       if (list == NULL) {
           fprintf(stderr, "Error: Memory allocation failure\n");
           return EXIT_FAILURE;
       }
       // Operations on the listtor...
       // Remember to free dynamically allocated memory
       free_sllist(list);
       return EXIT_SUCCESS;
   }

Underlying Functions 
--------------------
The ``init_sllist`` function selects from one of the functions below to Initialize 
a dynamically allocated array.  If the user desires, they can directly select 
one of these functions instead of using the ``init_sllist`` function.

.. code-block:: c

   char_sl* init_char_sllist();
   uchar_sl* init_uchar_sllist();
   short_sl* init_short_sllist();
   ushort_sl* init_ushort_sllist();
   int_sl* init_int_sllist();
   uint_sl* init_uint_sllist();
   long_sl* init_long_sllist();
   ulong_sl* init_ulong_sllist();
   llong_sl* init_llong_sllist();
   ullong_sl* init_ullong_sllist();
   float_sl* init_float_sllist();
   double_sl* init_double_sllist();
   ldouble_sl* init_ldouble_sllist();
   bool_sl* init_bool_sllist();
   string_sl* init_string_sllist();

Free Singly Linked List 
=======================
A singly linked list is dynamically allocated and therefore must be manually
freed from memory.  The ``free_sllist`` macro has been provided in the ``slist.h``
header file to free a singly linked list of any data type shown in the 
:ref:`Derived Data Types <sllist_dat_type>`.

.. code-block:: c 

   #define free_sllist(list) 

Parameters 
----------

- :c:`list`: A singly linked list data structure of type defined in :ref:`Derived Data Types <sllist_dat_type>`.

.. note:: A singly linked list should not be freed from memory more than once; however, the underyling functions have logic to prevent harm if a user does doubly free a singly linked list data structre.

Error Handling 
--------------
When implementing the ``free_sllist`` macro, it is possible that the user may pass an
unintended data type, or an uninitialized struct.  If this is the case, the 
functions that underly the ``free_sllist`` macro, will set the value of ``errno``
to ``EINVAL`` and return to the calling program.

Possible Error Codes 

- ``EINVAL``: Indicates that an incorrect, or poorly populated value is passed to the function in place of ``list``.

Example 
-------
The following is an example for the use of the ``free_sllist`` macro.

.. code-block:: c 

   #include "slist.h"
   #include "print.h"

   int main() {

       int_sl* list = init_sllist();
       push_sllist(list, 1, 0);
       push_sllist(list, 2, 0);
       push_sllist(list, 3, 0);
       push_sllist(list, 4, 0);
       print(list);
       // Failure to use this macro can lead to memory leaks
       free_sllist(list);
       return 0;
   } 

.. code-block:: bash 

   >> { 4, 3, 2, 1 }

Underlying Functions 
--------------------
The ``free_sllist`` macro is the preferred mechanism to free a singly linked 
list due to its generic nature and type safety; however, the user can also 
implement one of the type specific functions in its place, which are listed 
below.

.. code-block:: c

   void free_char_sllist(char_sl* sllist);
   void free_uchar_sllist(uchar_sl* sllist);
   void free_short_sllist(short_sl* sllist);
   void free_ushort_sllist(ushort_sl* sllist);
   void free_int_sllist(int_sl* sllist);
   void free_uint_sllist(uint_sl* sllist);
   void free_long_sllist(long_sl* sllist);
   void free_ulong_sllist(ulong_sl* sllist);
   void free_llong_sllist(llong_sl* sllist);
   void free_ullong_sllist(ullong_sl* sllist);
   void free_float_sllist(float_sl* sllist);
   void free_double_sllist(double_sl* sllist);
   void free_ldouble_sllist(ldouble_sl* sllist);
   void free_bool_sllist(bool_sl* sllist);
   void free_string_sllist(string_sl* sllist);

Garbage Collection with Singly Linked Lists 
===========================================
While a user of this library can implement the ``free_sllist`` macro to 
manually free a singly linked list, they can also implement automated Garbage 
Collection if they use a `GCC` or `CLANG` compiler.  This feautre leverages the 
`cleanup` attribute available in these compilers and is not part of the standard 
C language.

The macro follows the naming convention ``gbc_<type>``, where ``<type>``
corresponds to the derived data types mentioned in 
:ref:`Derived Data Types <sllist_dat_type>`.

.. note:: A user should not implement garbage collection and then manually free data with the ``free_sllist`` macro.  However, the ``free_sllist`` macro has logic built into it to return control to the calling program without harm if such an operation is to occur.

Example 
-------
Below is an example demonstrating the use of garbage collection with a 
``int_ls`` linked list.  Notice the absence of a manual ``free_sllist``
call; the ``gbc_float_ls`` macro ensures automatic deallocation when 
variable goes out of scope.

.. code-block:: c 

   #include "slist.h"
   #include "print.h"

   int main() {

       int_sl* list = init_sllist();
       push_sllist(list, 1, 0);
       push_sllist(list, 2, 0);
       push_sllist(list, 3, 0);
       push_sllist(list, 4, 0);
       print(list);
       return 0;
   } 
  
Pushing Data to a Singly Linked List 
====================================
The ``push_sllist`` macro in the ``slist.h`` header file provides a versatile way
to insert data into a singly linked list.  Traditionally a linked list follows 
the LIFO approach, this macro allows a user to insert data at any specified psuedo-index,
offering greater flexibility in managing listtor elements.  However, the user should 
be aware that inserting data to pseudo-index 0 will lead to a time complexity 
of :math:`O(1)`; however, inserting to any other pseudo-index will have a time 
complexity of :math:`O(n)` where :math:`n` is proporitional to the value of the 
pseudo-index.

.. code-block:: c 

   #define push_sllist(list, data, index);

Parameters 
----------

- :c:`list`: A singly linked list data structure consistent with :ref:`Derived Data Types <sllist_dat_type>`.
- :c:`data`: The data being inserted into the linked list, of a data type appropriate for the ``list``.
- :c:`index`: The pseudo index where data is being inserted of type ``size_t``.

Returns 
-------

- :c:`status`: Returns ``true`` if function executes succesfully, ``false`` otherwise.

Error Handling 
--------------
The implementation of the ``push_sllist`` macro can lead to three possible error 
codes.  If the user passes a faulty pointer for the value of ``list``, the function 
will set the value of ``errno`` to ``EINVAL``.  If the function is unable to allocate 
memory for the next node, the function will set the value of ``errno`` to ``ENOMEM``,
and if the index is out of range, the function will set the value of ``errnor`` to 
``ERANGE``.

Possible Error Codes 

- ``EINVAL``: Indicates an inproperly formatted struct in place of ``list``.
- ``ENOMEM``: Indicates a failure to allocate new memory for new node or any relevant strings.
- ``ERANGE``: Indicates an index that is out of range.

Example 
-------
The following is an example for the proper use of the ``push_sllist`` macro.

.. code-block:: c 

   int main() {
        string_sl* list = init_sllist(dString)();
        push_sllist(list, "One", 0);
        push_sllist(list, "Two", 1);
        push_sllist(list, "Three", 2);
        push_sllist(list, "Four", 1);
        push_sllist(list, "Five", 0);
        print(list)
        free_sllist(list);
        return 0;
    }

.. code-block:: bash 

   >> { Five, One, Four, Two, Three }

Underlying Functions 
--------------------
The ``push_sllist`` macro in the ``sllist.h`` header file provides a versatile 
way to add data type a singly linked list with type safety; however, a developer 
can also use the underling functions shown below for specific data types. 

.. code-block:: c

   bool push_char_sllist(char_sl* list, char var, size_t index);
   bool push_uchar_sllist(uchar_sl* list, unsigned char var, size_t index);
   bool push_short_sllist(short_sl* list, short int var, size_t index);
   bool push_ushort_sllist(ushort_sl* list, unsigned short int var, size_t index);
   bool push_int_sllist(int_sl* list, int var, size_t index);
   bool push_uint_sllist(uint_sl* list, unsigned int var, size_t index);
   bool push_long_sllist(long_sl* list, long int var, size_t index);
   bool push_ulong_sllist(ulong_sl* list, unsigned long int var, size_t index);
   bool push_llong_sllist(llong_sl* list, long long int var, size_t index);
   bool push_ullong_sllist(ullong_sl* list, unsigned long long int var, size_t index);
   bool push_float_sllist(float_sl* list, float var, size_t index);
   bool push_double_sllist(double_sl* list, double var, size_t index);
   bool push_ldouble_sllist(ldouble_sl* list, long double var, size_t index);
   bool push_bool_sllist(bool_sl* list, bool var, size_t index);
   bool push_string_sllist(string_sl* list, const char* var, size_t index);
   bool push_str_sllist(string_sl* list, str* var, size_t index);

Pop Singly Linked List 
======================
The ``pop_sllist`` macro included in the ``slist.h`` header file provides a 
type safe method to pop data from a singly linked list, which removes the user 
defined node from the list and returns the data encoded within the deleted node 
to the calling program.  The form of the ``pop_sllist`` macro along with the 
macro parameters and returned variables are shown below.

.. note:: The data returned from ``pop_sllist`` when popping data from a ``string_sl`` data type is in the form of a dynamically allocated ``str`` struct and must be manually freed with the ``free_string`` macro.

.. code-block:: c 

   #define pop_sllist(list, index)

Parameters 
----------

- :c:`list`: A singly linked list data structure.
- :c:`index`: The pseudo-index from where the user wishes to remove data.

Returns 
-------

- The data contained in the deleted index.  The data type must be consistent with the type of the node.

Error Handling 
--------------
The implementation of the ``pop_sllist`` macro can lead to two possible error 
codes.  If the user passes a faulty pointer for the value of ``list``, the function 
will set the value of ``errno`` to ``EINVAL``, and if the index is out of range, 
the function will set the value of ``errnor`` to ``ERANGE``.

Possible Error Codes 

- ``EINVAL``: Indicates an inproperly formatted struct in place of ``list``.
- ``ERANGE``: Indicates an index that is out of range.

Example 
-------
The following is an example for the proper use of the ``pop_sllist`` macro.

.. code-block:: c 

   int main() {
       string_sl* list = init_sllist(dString)();
       push_sllist(list, "Zero", list->len);
       push_sllist(list, "One", list->len);
       push_sllist(list, "Two", list->len);
       push_sllist(list, "Three", list->len);
       push_sllist(list, "Four", list->len);
       push_sllist(list, "Five", list->len);
       str* one = pop_sllist(list, 0);
       str* two = pop_sllist(list, 2);
       print("First Popped Value: " one->data);
       print("Secon Popped Value: ", two->data)
       print("Remaining List: ", list);
       free_string(one);
       free_string(two);
       free_sllist(list);
       return 0;
   }

.. code-block:: bash 

   >> First Popped Value: Zero 
   >> Second Popped Valie: Three 
   >> Remaining List: { One, Two, Four, Five }

Underlying Functions 
--------------------
The ``pop_sllist`` macro provides a robust, but generic method for a developer 
to removed data from a singly linked list.  However, a user can also interact 
directly with type specific functions that the ``pop_sllist`` macro wraps around.
The type specific functions are provided below.

.. code-block:: c

   bool pop_char_sllist(char_sl* list, size_t index);
   bool pop_uchar_sllist(uchar_sl* list, size_t index);
   bool pop_short_sllist(short_sl* list, size_t index);
   bool pop_ushort_sllist(ushort_sl* list, size_t index);
   bool pop_int_sllist(int_sl* list, size_t index);
   bool pop_uint_sllist(uint_sl* list, size_t index);
   bool pop_long_sllist(long_sl* list, size_t index);
   bool pop_ulong_sllist(ulong_sl* list, size_t index);
   bool pop_llong_sllist(llong_sl* list, size_t index);
   bool pop_ullong_sllist(ullong_sl* list, size_t index);
   bool pop_float_sllist(float_sl* list, size_t index);
   bool pop_double_sllist(double_sl* list, size_t index);
   bool pop_ldouble_sllist(ldouble_sl* list, size_t index);
   bool pop_bool_sllist(bool_sl* list, size_t index);
   bool pop_string_sllist(string_sl* list, size_t index);

Reverse Singly List 
===================
The ``reverse_sllist`` macro leverages the ``_Generic`` keyword to select from several
functions in order to reverse the order of data in a singly linked list.  The form 
and parameters of the ``reverse_sllist`` macro are shown below.

.. code-block:: c

   #define reverse_sllist(list) ( /* Expression to reverse list */)
     
Parameters 
----------

- :c:`list`: A singly linked list data structure 

Error Handling 
--------------
The implementation of the ``reverse_sllist`` macro can lead to one possible error 
code.  If the user passes a faulty pointer for the value of ``list``, the function 
will set the value of ``errno`` to ``EINVAL``.

Possible Error Codes 

- ``EINVAL``: Indicates an inproperly formatted struct in place of ``list``.

Example 
-------
The following is an example for the proper use of the ``reverse_sllist`` macro.

.. code-block:: c 

   int main() {
       string_sl* list = init_sllist(dString)();
       push_sllist(list, "Zero", list->len);
       push_sllist(list, "One", list->len);
       push_sllist(list, "Two", list->len);
       push_sllist(list, "Three", list->len);
       push_sllist(list, "Four", list->len);
       push_sllist(list, "Five", list->len);
       print("Initial Linked List ", list)
       reverse_sllist(list);
       print("Reversed Linked List: ", list);
       free_sllist(list);
       return 0;
   }

.. code-block:: bash 

   >> Initial Linked List: [ Zero, One, Two, Three, Four, Five ]
   >> Reversed Linked List: [ Five, Four, Three, Two, One, Zero ]

Underlying Functions 
--------------------
The ``reverse_sllist`` macro provides a robust, but generic method for a developer 
to removed data from a singly linked list.  However, a user can also interact 
directly with type specific functions that the ``reverse_sllist`` macro wraps around.
The type specific functions are provided below.

.. code-block:: c

   bool reverse_char_sllist(char_sl* list);
   bool reverse_uchar_sllist(uchar_sl* list);
   bool reverse_short_sllist(short_sl* list);
   bool reverse_ushort_sllist(ushort_sl* list);
   bool reverse_int_sllist(int_sl* list);
   bool reverse_uint_sllist(uint_sl* list);
   bool reverse_long_sllist(long_sl* list);
   bool reverse_ulong_sllist(ulong_sl* list);
   bool reverse_llong_sllist(llong_sl* list);
   bool reverse_ullong_sllist(ullong_sl* list);
   bool reverse_float_sllist(float_sl* list);
   bool reverse_double_sllist(double_sl* list);
   bool reverse_ldouble_sllist(ldouble_sl* list);
   bool reverse_bool_sllist(bool_sl* list);
   bool reverse_string_sllist(string_sl* list);

Sort Singly Linked List 
=======================
The ``sort_sllist`` macro is a versatile tool for sorting vector data structures 
in either ``FORWARD`` or ``REVERSE`` direction. It intelligently selects the 
appropriate sorting function based on the vector type and the specified sorting 
algorithm, ensuring type safety through the use of the ``_Generic`` keyword.

.. code-block:: c 

   #define sort_sllist(vec, stype, direction) ( /* Expressions to sort vector */)   

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.
- :c:`stype`: An ``enum`` of type ``sort_type`` including ``BUBBLE``, ``SELECTION``, ``INSERT``, and ``MERGE``, representing various sorting algorithms.
- :c:`direction`: An ``enum`` of type ``iter_dir`` with possible values ``FORWARD`` or ``REVERSE``, representing the sorting direction.

The following table describes the parameters of the various sorting algorithms.

.. list-table:: Sorting Algorithms Complexity and Stability
   :widths: 25 25 25 25
   :header-rows: 1

   * - Algorithm
     - Time Complexity (Average/Worst)
     - Space Complexity
     - Stability
   * - Bubble Sort
     - O(n^2) / O(n^2)
     - O(1)
     - Stable
   * - Insertion Sort
     - O(n^2) / O(n^2)
     - O(1)
     - Stable
   * - Merge Sort
     - O(n log n) / O(n log n)
     - O(n)
     - Stable

Error Handling
--------------
The macro sets the ``errno`` global variable to indicate errors, such as:

- ``EINVAL``: Passed if ``vec`` is NULL or if an unsupported ``stype`` is provided.

Example 
-------
An example for sorting a vector using the quick sort algorithm in the ``FORWARD`` direction.

.. code-block:: c

   #include "vector.h"
   #include "print.h"

   int main() {
       int_sl* list = init_sllist(dInt)();
       push_sllist(list, 5, list->len);
       push_sllist(list, 1, list->len);
       push_sllist(list, 4, list->len);
       push_sllist(list, 2, list->len);
       push_sllist(list, 3, list->len);
       push_sllist(list, 6, list->len);
       print("Initial Linked List ", list)
       sort_sllist(list, MERGE, FORWARD)
       reverse_sllist(list);
       print("Reversed Linked List: ", list);
       free_sllist(list);
       return 0;
   }

.. code-block:: bash 

   >> Before sorting operation: [ 5, 1, 4, 2, 3, 6 ]
   >> After sorting operation: [ 1, 2, 3, 4, 5, 6 ]

Underlying Functions 
--------------------
While the ``sort_sllist`` macro is recommended for its ease of use and type safety, 
developers may also directly call the specific sorting functions:

.. code-block:: c 

   void sort_char_sllist(char_sl* vec, sort_type stype, iter_dir direction);
   void sort_uchar_sllist(uchar_sl* vec, sort_type stype, iter_dir direction);
   void sort_short_sllist(short_sl* vec, sort_type stype, iter_dir direction);
   void sort_ushort_sllist(ushort_sl* vec, sort_type stype, iter_dir direction);
   void sort_int_sllist(int_sl* vec, sort_type stype, iter_dir direction);
   void sort_uint_sllist(uint_sl* vec, sort_type stype, iter_dir direction);
   void sort_long_sllist(long_sl* vec, sort_type stype, iter_dir direction);
   void sort_ulong_sllist(ulong_sl* vec, sort_type stype, iter_dir direction);
   void sort_llong_sllist(llong_sl* vec, sort_type stype, iter_dir direction);
   void sort_ullong_sllist(ullong_sl* vec, sort_type stype, iter_dir direction);
   void sort_float_sllist(float_sl* vec, sort_type stype, iter_dir direction);
   void sort_double_sllist(double_sl* vec, sort_type stype, iter_dir direction);
   void sort_ldouble_sllist(ldouble_sl* vec, sort_type stype, iter_dir direction);
   void sort_bool_sllist(bool_sl* vec, sort_type stype, iter_dir direction);
   void sort_string_sllist(string_sl* vec, sort_type stype, iter_dir direction);
