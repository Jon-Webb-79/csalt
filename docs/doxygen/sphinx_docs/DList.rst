.. _dl_list:

********************
Doubly Linked List
********************

The ``dlist.h`` header file provides an implementation of the Doubly Linked List
data structure in C. A Doubly Linked List is an enhanced version of the singly
linked list, where each node contains data, a pointer to the next node, and a
pointer to the previous node in the sequence. This bi-directional linkage
facilitates more versatile traversals and operations, allowing efficient
insertion, deletion, and access from both ends of the list.

Doubly Linked Lists are particularly useful for:

- **Bidirectional Traversal**: Unlike Singly Linked Lists, Doubly Linked Lists
  allow traversal in both forward and backward directions, making them suitable
  for applications that require reverse iteration or navigation through the data.

- **Efficient Element Insertion and Deletion**: With pointers to both the next
  and previous nodes, elements can be added or removed from both ends of the
  list and at any position within the list with relative ease and efficiency.

- **Implementation of Complex Data Structures**: The versatility of Doubly
  Linked Lists makes them ideal for creating sophisticated data structures like
  doubly-ended queues, linked lists with both LIFO (Last In, First Out) and
  FIFO (First In, First Out) characteristics, and for facilitating easier
  cursor movement in text editors.

- **Enhanced Flexibility for Dynamic Data Management**: They provide a dynamic
  data storage solution capable of expanding and contracting as needed, without
  the fixed size limitations of arrays. This makes Doubly Linked Lists suitable
  for managing collections of data where the volume may change over time.

The ``dlist.h`` library provides functions to create, manage, and interact with
Doubly Linked Lists, offering developers enhanced control and flexibility over
data management processes. Key characteristics of a Doubly Linked List include:

- Each node contains a data field, a pointer to the next node (forward link),
  and a pointer to the previous node (backward link), creating a network of
  nodes that can be navigated in both directions.

- The first node of the list, known as the "head," has its previous pointer set
  to `NULL`, and the last node, or "tail," has its next pointer set to `NULL`.

- Operations such as insertion, deletion, and traversal are more efficient
  in certain scenarios compared to singly linked lists, especially when
  removing nodes from the end of the list or when traversing the list in reverse.

- Doubly Linked Lists allow for O(1) time complexity for insertions and deletions
  at both ends of the list and have an O(n/2) time complexity for operations at
  arbitrary positions, where `n` is the number of elements in the list.

This library extends the capabilities of traditional linked list structures,
providing a powerful tool for developers to handle complex data management
tasks with greater ease and efficiency.

Structures
==========
The ``dlist.h`` header file utilizes the following macro to create a struct that 
will contain the data inserted into a doubly linked list.

.. role:: c(code)
   :language: c 

.. code-block:: c 

   #define DLIST_NODE(type, dat_type) \
       typedef struct dat_type { \
           type data; \
           struct dat_type *next; \
           struct dat_type *prev; \
       } dat_type

Parameters 
----------

- :c:`type`: The data type that the struct will contain. 
- :c:`dat_type`: THe typdef name given to the struct.

Attributes 
----------

- :c:`data`: The data being contained in the struct 
- :c:`next`: A pointer to the next struct in the linked list.
- :c:`prev`: A pointer to the previous struct in the linked list.

In addition, the ``dlist.h`` header file contains a macro that is used to create 
a struct that manages the attributes of a singly linked list.  

.. code-block:: c 

   #define DLIST_STRUCT(type, dat_type) \
       typedef struct { \
           size_t len; \
           struct type* head; \
           struct type* tail; \
       } dat_type

Parameters 
----------

- :c:`type`: The typedef name assigned to a node struct.
- :c:`dat_type`: The typedef name assigned to this struct.

Attributes 
----------

- :c:`len`: The length of the singly linked list (i.e. the number of nodes assigned to the list)
- :c:`head`: A pointer to the first node in the linked list.
- :c:`tail`: A pointer to the last node in the linked list.

.. _dllist_dat_type:

Derived Data Types 
==================
The ``DLIST_NODE`` macro is pre-instantiated within the ``dlist.h`` header file 
for these data types.  The ``str`` data type is further described in the 
:ref:`String <string_struct>` section of this document.

.. code-block:: c

   DLIST_NODE(char, char_dlnode);
   DLIST_NODE(unsigned char, uchar_dlnode);
   DLIST_NODE(short int, short_dlnode);
   DLIST_NODE(unsigned short int, ushort_dlnode);
   DLIST_NODE(int, int_dlnode);
   DLIST_NODE(unsigned int, uint_dlnode);
   DLIST_NODE(long int, long_dlnode);
   DLIST_NODE(unsigned long int, ulong_dlnode);
   DLIST_NODE(long long int, llong_dlnode);
   DLIST_NODE(unsigned long long int, ullong_dlnode);
   DLIST_NODE(float, float_dlnode);
   DLIST_NODE(double, double_dlnode);
   DLIST_NODE(long double, ldouble_dlnode);
   DLIST_NODE(bool, bool_dlnode);
   DLIST_NODE(str*, string_dlnode);

The ``DLIST_STRUCT`` macro is pre-instantiated within the ``Dlist.h`` header file 
for the following typedef names.  Notice the linkage between the ``DLIST_NODE`` 
data types that are contained in their respective ``DLIST_STRUCT`` instance.

.. code-block:: c

   DLIST_STRUCT(char_dlnode, char_dl);
   DLIST_STRUCT(uchar_dlnode, uchar_dl);
   DLIST_STRUCT(short_dlnode, short_dl);
   DLIST_STRUCT(ushort_dlnode, ushort_dl);
   DLIST_STRUCT(int_dlnode, int_dl);
   DLIST_STRUCT(uint_dlnode, uint_dl);
   DLIST_STRUCT(long_dlnode, long_dl);
   DLIST_STRUCT(ulong_dlnode, ulong_dl);
   DLIST_STRUCT(llong_dlnode, llong_dl);
   DLIST_STRUCT(ullong_dlnode, ullong_dl);
   DLIST_STRUCT(float_dlnode, float_dl);
   DLIST_STRUCT(double_dlnode, double_dl);
   DLIST_STRUCT(ldouble_dlnode, ldouble_dl);
   DLIST_STRUCT(bool_dlnode, bool_dl);
   DLIST_STRUCT(string_dlnode, string_dl);

.. _dllist_type_enum:

Enum Data Types 
===============
This ``dlist.h`` header file relies on the ``dtype`` enum stored in ``admin.h``
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

Initialize Doubly Linked List 
=============================
The ``dlist.h`` header file provides the ``init_dlist`` function for initializing 
a singly linked list.  This function is essentially a function pointer, intelligently 
selecting the appropriate initialization function based on the specified data 
type.

.. code-block:: c

   type init_dlist(dtype dat_type)()

Parameters 
----------

- :c:`dat_type`: The data type for the singly linked list, represented by a ``dtype`` as described in :ref:`Enum Data Types <dllist_type_enum>`.

Returns 
-------

- :c:`type`: A struct that manages the singly linked list of the specified type in the :ref:`Singly Linked List Type <dllist_dat_type>`

Error Handling 
--------------
The ``init_dllist`` macro has one primary mechanism, which is a failure to allocate
memory.  If this occurs, the underlying functions will return a NULL pointer in
place of the data struct, and will also set the value of ``errno`` to ``ENOMEM``.
The developer can check for any of these two conditions if an error occurred.

Possible error codes:

- ``ENOMEM``: Indicates a filure to allocate memory

Example
-------
Below is an example using the ``init_dllist`` function to create a singly linked 
list data structure of type ``float_dl``.

.. code-block:: c 

   #include "dlist.h"

   int main() {
       float_dl* list = init_dllist(dFloat)();
       if (list == NULL) {
           fprintf(stderr, "Error: Memory allocation failure\n");
           return EXIT_FAILURE;
       }
       // Operations on the listtor...
       // Remember to free dynamically allocated memory
       free_dllist(list);
       return EXIT_SUCCESS;
   }

Underlying Functions 
--------------------
The ``init_dllist`` function selects from one of the functions below to Initialize 
a doubly linked list.  If the user desires, they can directly select 
one of these functions instead of using the ``init_dllist`` function.

.. code-block:: c

   char_dl* init_char_dllist();
   uchar_dl* init_uchar_dllist();
   short_dl* init_short_dllist();
   ushort_dl* init_ushort_dllist();
   int_dl* init_int_dllist();
   uint_dl* init_uint_dllist();
   long_dl* init_long_dllist();
   ulong_dl* init_ulong_dllist();
   llong_dl* init_llong_dllist();
   ullong_dl* init_ullong_dllist();
   float_dl* init_float_dllist();
   double_dl* init_double_dllist();
   ldouble_dl* init_ldouble_dllist();
   bool_dl* init_bool_dllist();
   string_dl* init_string_dllist();

Free Doubly Linked List 
=======================
A doubly linked list is dynamically allocated and therefore must be manually
freed from memory.  The ``free_dllist`` macro has been provided in the ``dlist.h``
header file to free a singly linked list of any data type shown in the 
:ref:`Derived Data Types <dllist_dat_type>`.

.. code-block:: c 

   #define free_dllist(list) 

Parameters 
----------

- :c:`list`: A singly linked list data structure of type defined in :ref:`Derived Data Types <dllist_dat_type>`.

.. note:: A doubly linked list should not be freed from memory more than once; however, the underyling functions have logic to prevent harm if a user does doubly free a singly linked list data structre.

Error Handling 
--------------
When implementing the ``free_dllist`` macro, it is possible that the user may pass an
unintended data type, or an uninitialized struct.  If this is the case, the 
functions that underly the ``free_dllist`` macro, will set the value of ``errno``
to ``EINVAL`` and return to the calling program.

Possible Error Codes 

- ``EINVAL``: Indicates that an incorrect, or poorly populated value is passed to the function in place of ``list``.

Example 
-------
The following is an example for the use of the ``free_dllist`` macro.

.. code-block:: c 

   #include "dlist.h"
   #include "print.h"

   int main() {

       int_dl* list = init_dllist();
       push_front_dllist(list, 1);
       push_front_dllist(list, 2);
       push_front_dllist(list, 3);
       push_front_dllist(list, 4);
       print(list);
       // Failure to use this macro can lead to memory leaks
       free_dllist(list);
       return 0;
   } 

.. code-block:: bash 

   >> { 4, 3, 2, 1 }

Underlying Functions 
--------------------
The ``free_dllist`` macro is the preferred mechanism to free a singly linked 
list due to its generic nature and type safety; however, the user can also 
implement one of the type specific functions in its place, which are listed 
below.

.. code-block:: c

   void free_char_dllist(char_dl* dllist);
   void free_uchar_dllist(uchar_dl* dllist);
   void free_short_dllist(short_dl* dllist);
   void free_ushort_dllist(ushort_dl* dllist);
   void free_int_dllist(int_dl* dllist);
   void free_uint_dllist(uint_dl* dllist);
   void free_long_dllist(long_dl* dllist);
   void free_ulong_dllist(ulong_dl* dllist);
   void free_llong_dllist(llong_dl* dllist);
   void free_ullong_dllist(ullong_dl* dllist);
   void free_float_dllist(float_dl* dllist);
   void free_double_dllist(double_dl* dllist);
   void free_ldouble_dllist(ldouble_dl* dllist);
   void free_bool_dllist(bool_dl* dllist);
   void free_string_dllist(string_dl* dllist);

Garbage Collection with Doubly Linked Lists 
===========================================
While a user of this library can implement the ``free_dllist`` macro to 
manually free a singly linked list, they can also implement automated Garbage 
Collection if they use a `GCC` or `CLANG` compiler.  This feautre leverages the 
`cleanup` attribute available in these compilers and is not part of the standard 
C language.

The macro follows the naming convention ``gbc_<type>_dl``, where ``<type>``
corresponds to the derived data types mentioned in 
:ref:`Derived Data Types <dllist_dat_type>`.

.. note:: A user should not implement garbage collection and then manually free data with the ``free_dllist`` macro.  However, the ``free_dllist`` macro has logic built into it to return control to the calling program without harm if such an operation is to occur.

Example 
-------
Below is an example demonstrating the use of garbage collection with a 
``int_dl`` linked list.  Notice the absence of a manual ``free_dllist``
call; the ``gbc_int_dl`` macro ensures automatic deallocation when 
variable goes out of scope.

.. code-block:: c 

   #include "dlist.h"
   #include "print.h"

   int main() {

       int_dl* list gbc_int_dl = init_sllist(dInt);
       push_front_dllist(list, 1);
       push_front_dllist(list, 2);
       push_front_dllist(list, 3);
       push_front_dllist(list, 4);
       print(list);
       return 0;
   }

.. code-block:: c

   >> { 4, 3, 2, 1 }

dlist_size
==========
In C, struct members are publicly accessible, which may lead to unintentional 
modifications of critical attributes, such as a doubly linked list's length. 
To mitigate this risk, the ``dlist_size()`` function (or macro, if designed as such) 
allows users to safely obtain the list's length without direct access to the 
``len`` attribute. Below is the definition and usage:

.. code-block:: c

   #define dlist_size(list)

Parameters
----------

- ``list``: The linked list data structure.

Returns
-------

- The length of the linked list as a ``size_t``.

Usage Example
-------------

.. code-block:: c

   #include "dlist.h"
   #include "print.h"

   int main() {
       int_dl* list = init_dllist(dInt)();
       push_front_dllist(list, 1);
       push_front_dllist(list, 2);
       push_front_dllist(list, 3);
       push_front_dllist(list, 4);
       print(dlist_size(list));
       free_dllist(list);
       return 0;
   }

.. code-block:: bash 

   >> 4

Underlying Functions
--------------------
For direct access or in cases where the macro cannot be used, the following 
type-specific functions are available:

.. code-block:: c

   size_t size_char_dlist(char_dl* list);
   size_t size_uchar_dllist(uchar_dl* dllist);
   size_t size_short_dllist(short_dl* dllist);
   size_t size_ushort_dllist(ushort_dl* dllist);
   size_t size_int_dllist(int_dl* dllist);
   size_t size_uint_dllist(uint_dl* dllist);
   size_t size_long_dllist(long_dl* dllist);
   size_t size_ulong_dllist(ulong_dl* dllist);
   size_t size_llong_dllist(llong_dl* dllist);
   size_t size_ullong_dllist(ullong_dl* dllist);
   size_t size_float_dllist(float_dl* dllist);
   size_t size_double_dllist(double_dl* dllist);
   size_t size_ldouble_dllist(ldouble_dl* dllist);
   size_t size_bool_dllist(bool_dl* dllist);
   size_t size_string_dllist(string_dl* dllist);

push_front_dlist
================

The ``push_front_dlist`` macro provides a generic interface for pushing data 
of various types to the front of a doubly linked list. This abstraction allows 
for type-safe operations across different kinds of data structures while 
minimizing the risk of misuse by encapsulating the implementation details.

Macro Definition
----------------

.. code-block:: c

    #define push_front_dllist(list, dat) _Generic((list), \
        char_dl*: push_char_front_dllist, \
        uchar_dl*: push_uchar_front_dllist, \
        short_dl*: push_short_front_dllist, \
        ushort_dl*: push_ushort_front_dllist, \
        int_dl*: push_int_front_dllist, \
        uint_dl*: push_uint_front_dllist, \
        long_dl*: push_long_front_dllist, \
        ulong_dl*: push_ulong_front_dllist, \
        llong_dl*: push_llong_front_dllist, \
        ullong_dl*: push_ullong_front_dllist, \
        float_dl*: push_float_front_dllist, \
        double_dl*: push_double_front_dllist, \
        ldouble_dl*: push_ldouble_front_dllist, \
        bool_dl*: push_bool_front_dllist, \
        string_dl*: push_string_front_dllist) (list, dat);

The macro utilizes the C11 `_Generic` keyword to dispatch the correct function 
based on the type of the list provided. This approach ensures type safety and 
reduces the potential for runtime errors.  A developer may also use the type 
specific functions that are wrapped by the ``push_front_dlist`` macro 
shown above.

.. note:: The ``push_string_front_dllist`` function takes a string literal as an input.  If a developer wishes to insert a ``str`` data type they must manually use the ``push_str_front_dllist`` function, which retains the same format for input variables as the macro.

Parameters
----------

- ``list``: A pointer to the doubly linked list data structure.
- ``dat``: The data to be pushed to the head of the list. The type of this parameter depends on the specific list type.

Returns
-------

The functions return a boolean value:

- ``true`` if the operation completes successfully.
- ``false`` otherwise, setting ``errno`` to the appropriate error code.

Example Usage
-------------

The following example demonstrates pushing an integer to the front of a doubly linked list:

.. code-block:: c

   #include "dlist.h"
   #include "print.h"

   int main() {
       int_dl* list = init_dllist(dInt)();
       push_front_dllist(list, 1);
       push_front_dllist(list, 2);
       push_front_dllist(list, 3);
       push_front_dllist(list, 4);
       print(dlist_size(list));
       print(list);
       free_dllist(list);
       return 0;
   }

.. code-block:: bash 

   >> 4
   >> { 4, 3, 2, 1 }

push_back_dllist()
==================

The ``push_back_dllist`` macro provides a generic interface to append data to 
the back of various types of doubly linked lists. This abstraction allows 
users to add elements to the end of a list without needing to directly call 
type-specific functions, enhancing code readability and maintainability.

Parameters
----------

- ``list``: A pointer to the doubly linked list data structure. The type of the list determines which specific push function is invoked.
- ``dat``: The data to be appended to the list. The type of this parameter must match the list's data type.

Returns
-------

- This macro returns a boolean value:
  
  - ``true`` if the data is successfully appended to the list.
  - ``false`` otherwise. On failure, ``errno`` is set to indicate the error.

Error Handling
--------------

The macro and its underlying functions use ``errno`` for error reporting:

- If ``list`` is NULL, ``errno`` is set to ``EINVAL`` (Invalid argument).
- If memory allocation fails during the operation, ``errno`` is set to ``ENOMEM`` (Not enough memory).

Use Cases
---------

The ``push_back_dllist`` macro simplifies appending data to lists of different 
types. For instance, adding a string to a doubly linked list of strings:

.. code-block:: c

    string_dl* myList = init_dllist(dString)();
    push_back_dllist(myList, "Example String");

Direct Use of Underlying Functions
----------------------------------

While the macro provides a convenient and generic way to append data, direct 
calls to the underlying type-specific functions are possible and might be 
preferred in scenarios where type safety is critical, or generic programming is not suitable:

.. code-block:: c

    bool push_string_back_dllist(string_dl* list, char* dat);

This approach bypasses the generic interface, requiring the developer to 
explicitly select the appropriate function for the data and list type, 
ensuring type compatibility.

Example
-------

The following example demonstrates initializing a list of strings and 
appending multiple strings using both the macro and the direct function call:

.. code-block:: c

   #include "dlist.h"
   #include "print.h"

   int main() {
       string_dl* list = init_dllist(dString)();
       push_back_dllist(list, "Hello");
       push_back_dllist(list, "World"); 
       // Direct function call
       char* newString = "Direct Call";
       push_string_back_dllist(list, newString);
       print(list);

       // Cleanup
       free_dllist(list);
       return 0;
   }

.. code-block:: bash

   >> { Hello, World, Direct Call }

This section provides comprehensive documentation on using the ``push_back_dllist`` macro and its associated functions to append data to doubly linked lists in a generic, type-safe manner.

