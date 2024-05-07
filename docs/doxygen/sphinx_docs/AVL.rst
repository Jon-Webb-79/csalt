.. _avl_tree:

********************
AVL Tree
********************

The ``avl.h`` header file provides an implementation of the AVL Tree data structure 
in C. An AVL Tree is a type of self-balancing binary search tree where the height 
of the left and right subtrees of any node differ by no more than one. This 
balance is maintained through rotations during insertions and deletions, 
ensuring that the tree remains optimally balanced for efficient search operations.

AVL Trees are particularly useful for:

- **Efficient Search Operations**: Thanks to their self-balancing nature, AVL 
  Trees maintain an O(log n) search time complexity, making them ideal for 
  applications that require fast search, insert, and delete operations.

- **Maintaining Ordered Data**: AVL Trees store data in a sorted manner, allowing 
  for efficient in-order traversal of elements, which is crucial for range 
  queries and sorted data retrieval.

- **Dynamic Data Set Management**: They dynamically adjust their structure via 
  rotations to maintain balance, which is critical for handling intermediately 
  changing data sets where insertions and deletions happen frequently.

- **Implementation of Priority Queues**: Due to their ordered nature and efficient 
  insertion and deletion capabilities, AVL Trees are well-suited for implementing 
  priority queues where elements need to be accessed and removed in a sorted order.

- **Enhanced Performance for Read-Intensive Applications**: AVL Trees are particularly 
  advantageous in scenarios where the data structure undergoes frequent read 
  operations, as their balanced nature ensures consistently quick access times.

The ``avl.h`` library offers functions to create, manage, and interact with AVL 
Trees, providing developers with enhanced capabilities for handling data efficiently. 
Key characteristics of an AVL Tree include:

- **Self-Balancing Capability**: Each node in the AVL Tree automatically maintains 
  a balance factor (the height difference between its left and right subtree) 
  that is used to perform rotations and ensure the tree remains balanced after 
  insertions and deletions.

- **Rotations for Balancing**: Simple rotations (left, right) and double 
  rotations (left-right, right-left) are utilized to maintain tree balance, 
  enhancing the overall performance of dynamic data operations.

- **Logarithmic Height**: The maximum height of an AVL Tree is bound by 1.44 log₂(n+2), 
  ensuring that operations like search, insert, and delete can be performed in 
  logarithmic time complexity.

- **Node Structure**: Each node contains a data field, pointers to the left and 
  right children, and often a balance factor or height information to aid in 
  maintaining the tree's balance.

This library extends the capabilities of traditional binary search trees by 
ensuring that operations are carried out with maximum efficiency, regardless of 
the sequence of insertions and deletions, thus providing a robust tool for 
developers to manage ordered data structures effectively.

Structures 
==========
The ``avl.h`` header file implements several Structs to organize information that 
enables the AVL Binary Tree.  The first of these data structus if the ``AVLNode``
struct which has the following format.

.. role:: c(code)
   :language: c

.. code-block:: c

   typedef struct AVLNode {
       struct AVLNode* left;
       struct AVLNode* right;
       int height;
    } AVLNode;

The ``AVLNode`` struct is the base struct that an AVL Node is based on, and is 
developed to decouple generic operations from type specific operations.  This 
struct is used to facilitate several generic functions, such that a version of 
those functions does not need to be developed for each applicable data type,
while maintaining the type dependency of public facing functions.  This struct is 
not a public facing struct.

This library creates a type specific AVL Node struct that inherits from ``AVLNode``
with the following macro.

.. code-block:: c

   #define AVL_NODE(type, dat_type) \
   typedef struct { \
       AVLNode base; \
       type data; \
    } dat_type;

Parameters 
----------

- :c:`type`: The data type that the struct will contain. 
- :c:`dat_type`: The typdef name given to the struct.

Attributes 
----------

- :c:`base`: An AVLNode struct
- :c:`data`: The data contained in the node of data type ``type``

Finally, this header file implements another macro to create an overarching
struct that maintains specific binary tree metadata.  The macro and struct is 
shown below.

.. code-block:: c

   #define AVLTREE_STRUCT(dat_type) \
       struct { \
           size_t len; \
           AVLNode* root; \
           bool allow_duplicates; \
        } dat_type;

Parameters 
----------

- :c:`dat_type`: The typdef name given to the struct.

Attributes 
----------

- :c:`len`: The number of nodes in an AVL tree 
- :c:`root`: The root node in the AVL tree 
- :c:`allow_duplicates`: ``true`` if duplicate values are allowed in tree, ``false`` otherwise

.. _avl_dat_type:

Derived Data Types 
==================
The ``AVL_NODE`` macro is pre-instantiated within the ``avl.h`` header file for these 
data types.  The ``str`` data type is further described in the
:ref:`String <string_struct>` section of this document. 

.. code-block:: c

   AVL_NODE(char, charAVLNode)
   AVL_NODE(unsigned char, ucharAVLNode)
   AVL_NODE(short int, shortAVLNode)
   AVL_NODE(unsigned short int, ushortAVLNode)
   AVL_NODE(int, intAVLNode)
   AVL_NODE(unsigned int, uintAVLNode)
   AVL_NODE(long int, longAVLNode)
   AVL_NODE(unsigned long int, ulongAVLNode)
   AVL_NODE(long long int, llongAVLNode)
   AVL_NODE(unsigned long long int, ullongAVLNode)
   AVL_NODE(float, floatAVLNode)
   AVL_NODE(double, doubleAVLNode)
   AVL_NODE(long double, ldoubleAVLNode)
   AVL_NODE(bool, boolAVLNode)
   AVL_NODE(str*, stringAVLNode)

In addition, the ``AVLTREE_STRUCT`` macro is pre-instantiated with the following 
``typedef`` names.

.. code-block:: c

   AVLTREE_STRUCT(charAVLTree)
   AVLTREE_STRUCT(ucharAVLTree)
   AVLTREE_STRUCT(shortAVLTree)
   AVLTREE_STRUCT(ushortAVLTree)
   AVLTREE_STRUCT(intAVLTree)
   AVLTREE_STRUCT(uintAVLTree)
   AVLTREE_STRUCT(longAVLTree)
   AVLTREE_STRUCT(ulongAVLTree)
   AVLTREE_STRUCT(llongAVLTree)
   AVLTREE_STRUCT(ullongAVLTree)
   AVLTREE_STRUCT(floatAVLTree)
   AVLTREE_STRUCT(doubleAVLTree)
   AVLTREE_STRUCT(ldoubleAVLTree)
   AVLTREE_STRUCT(boolAVLTree)
   AVLTREE_STRUCT(stringAVLTree)

.. _avl_type_enum:

Enum Data Types 
===============
This ``avl.h`` header file relies on the ``dtype`` enum stored in ``admin.h``
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

init_avltree
============
The ``avl.h`` header file provides the ``init_avltree`` function for initializing 
an AVL Binary Tree linked list.  This function is essentially a function pointer, 
intelligently selecting the appropriate initialization function based on the 
specified data type.

.. code-block:: c

   type init_avltree(dtype dat_type)(bool duplicates)

Parameters 
----------

- :c:`dat_type`: The data type for the singly linked list, represented by a ``dtype`` as described in :ref:`Enum Data Types <dllist_type_enum>`.
- :c:`duplicates`: ``true`` if duplicate values are allowed, ``false`` otherwise.

Returns 
-------

- :c:`type`: A struct that manages the singly linked list of the specified type in the :ref:`Singly Linked List Type <dllist_dat_type>`

Error Handling 
--------------
The ``init_avltree`` function has one primary error mechanism, which is a failure 
to allocate memory.  If this occurs, the underlying functions will return a 
NULL pointer in place of the data struct, and will also set the value of 
``errno`` to ``ENOMEM``. The developer can check for any of these two conditions 
if an error occurred.

Possible error codes:

- ``ENOMEM``: Indicates a filure to allocate memory

Example
-------
Below is an example using the ``init_avltree`` function to create an AVL Binary 
Tree of type ``floatAVLTree``.

.. code-block:: c 

   #include "avl.h"
   #include "stdbool.h"

   int main() {
       floatAVLTree* list = init_avltree(dFloat)(true);
       if (list == NULL) {
           fprintf(stderr, "Error: Memory allocation failure\n");
           return EXIT_FAILURE;
       }
       // Operations on the listtor...
       // Remember to free dynamically allocated memory
       free_avltree(list);
       return EXIT_SUCCESS;
   }

Underlying Functions 
--------------------
The ``init_avltree`` function selects from one of the functions below to Initialize 
a binary tree.  If the user desires, they can directly select 
one of these functions instead of using the ``init_avltree`` function.

.. code-block:: c

   char_dl* init_char_avltree(bool duplicates);
   uchar_dl* init_uchar_avltree(bool duplicates);
   short_dl* init_short_avltree(bool duplicates);
   ushort_dl* init_ushort_avltree(bool duplicates);
   int_dl* init_int_avltree(bool duplicates);
   uint_dl* init_uint_avltree(bool duplicates);
   long_dl* init_long_avltree(bool duplicates);
   ulong_dl* init_ulong_avltree(bool duplicates);
   llong_dl* init_llong_avltree(bool duplicates);
   ullong_dl* init_ullong_avltree(bool duplicates);
   float_dl* init_float_avltree(bool duplicates);
   double_dl* init_double_avltree(bool duplicates);
   ldouble_dl* init_ldouble_avltree(bool duplicates);
   bool_dl* init_bool_avltree(bool duplicates);
   string_dl* init_string_avltree(bool duplicates);

free_avltree
============
The ``free_avltree`` macro provides a generic interface for freeing memory 
associated with AVL trees of various data types. This macro utilizes the C11 
``_Generic`` keyword, which allows for type-based dispatch, enabling it to 
call the appropriate type-specific free function based on the type of the AVL 
tree passed to it.

.. code-block:: c

    #define free_avltree(tree) _Generic((tree), \
        charAVLTree*: free_char_avltree, \
        ucharAVLTree*: free_uchar_avltree, \
        shortAVLTree*: free_short_avltree, \
        ushortAVLTree*: free_ushort_avltree, \
        intAVLTree*: free_int_avltree, \
        uintAVLTree*: free_uint_avltree, \
        longAVLTree*: free_long_avltree, \
        ulongAVLTree*: free_ulong_avltree, \
        llongAVLTree*: free_llong_avltree, \
        ullongAVLTree*: free_ullong_avltree, \
        floatAVLTree*: free_float_avltree, \
        doubleAVLTree*: free_double_avltree, \
        ldoubleAVLTree*: free_ldouble_avltree, \
        boolAVLTree*: free_bool_avltree, \
        stringAVLTree*: free_string_avltree)(tree)

The above macro highlights the underlying functions which can be called in 
place of the macro if the user desires.

Parameters 
----------
- :c:`tree`: An avl tree data structure of the type defined in :ref:`Derived Data types <avl_dat_type>`

.. note:: An AVL Tree should not be freed from memory more than once; however, the underyling functions have logic to prevent harm if a user does doubly free a tree data structure.

Error Handling
--------------
Each free function checks if the tree pointer is ``NULL`` before proceeding. 
If ``NULL``, the macro sets ``errno`` to ``EINVAL`` to indicate an invalid 
argument error. This ensures that attempting to free a ``NULL`` tree pointer 
does not result in undefined behavior.

Example 
-------

.. code-block:: c

   #include "avl.h"
   #include "print.h"

   int main() {
       stringAVLTree* tree = init_avltree(dString)(true);
       insert_avltree(tree, "One");
       insert_avltree(tree, "Two);
       print(tree);
       free_avltree(tree);
       return 0;
   }

.. code-block:: bash 

   >> [ One, Two ]

Garbage Collection with AVL Binary Trees 
========================================
While a user of this library can implement the ``free_avltree`` macro to 
manually free an AVL Tree, they can also implement automated Garbage 
Collection if they use a `GCC` or `CLANG` compiler.  This feautre leverages the 
`cleanup` attribute available in these compilers and is not part of the standard 
C language.

The macro follows the naming convention ``gbc_<type>_avltree``, where ``<type>``
corresponds to the derived data types mentioned in 
:ref:`Derived Data Types <avl_dat_type>`.

.. note:: A user should not implement garbage collection and then manually free data with the ``free_avltree`` macro.  However, the ``free_avltree`` macro has logic built into it to return control to the calling program without harm if such an operation is to occur.

Example 
-------
Below is an example demonstrating the use of garbage collection with a 
``intAVLTree`` tree.  Notice the absence of a manual ``free_avltree``
call; the ``gbc_int_dl`` macro ensures automatic deallocation when 
variable goes out of scope.

.. code-block:: c 

   #include "avl.h"
   #include "print.h"

   int main() {

       intAVLTree* tree gbc_int_avltree = init_avltree(dInt)(false);
       insert_avltree(tree, 1);
       insert_avltree(tree, 2);
       insert_avltree(tree, 3);
       insert_avltree(tree, 4);
       print(tree);
       return 0;
   }

.. code-block:: c

   >> { 1, 2, 3, 4 }

insert_avltree
==============
The ``insert_avltree`` macro simplifies the process of inserting elements into 
AVL trees by utilizing the C11 ``_Generic`` keyword for type-based dispatch. 
This macro selects the appropriate type-specific insertion function based on 
the type of the AVL tree provided, streamlining code usage across various data types.

.. code-block:: c

    #define insert_avltree(tree, value) _Generic((tree), \
        charAVLTree*: insert_char_avltree, \
        ucharAVLTree*: insert_uchar_avltree, \
        shortAVLTree*: insert_short_avltree, \
        ushortAVLTree*: insert_ushort_avltree, \
        intAVLTree*: insert_int_avltree, \
        uintAVLTree*: insert_uint_avltree, \
        longAVLTree*: insert_long_avltree, \
        ulongAVLTree*: insert_ulong_avltree, \
        llongAVLTree*: insert_llong_avltree, \
        ullongAVLTree*: insert_ullong_avltree, \
        floatAVLTree*: insert_float_avltree, \
        doubleAVLTree*: insert_double_avltree, \
        ldoubleAVLTree*: insert_ldouble_avltree, \
        boolAVLTree*: insert_bool_avltree, \
        stringAVLTree*: insert_string_avltree)(tree, value)

Parameters
----------
- :c:`tree`: A pointer to an AVL tree data structure, dynamically typed based on the type of tree.
- :c:`value`: The data to be inserted into the AVL tree, type-compatible with the tree's data type.

Error Handling
--------------
Each insertion function first checks if the tree pointer or value is ``NULL``. 
If any argument is ``NULL``, ``errno`` is set to ``EINVAL`` to indicate an 
invalid argument error. This check is critical for ensuring that null pointers 
do not lead to undefined behavior during memory operations.

.. note:: The function ``insert_string_avltree`` additionally checks the validity of the string pointer to ensure it is not ``NULL`` before attempting to insert, as string operations are particularly susceptible to errors from invalid memory references.

Example Usage with No Duplicates
--------------------------------

.. code-block:: c

    #include "avl.h"
    #include "print.h"

    int main() {
        intAVLTree* tree = init_avltree(dInt)(false);
        insert_avltree(tree, 42);
        insert_avltree(tree, 21);
        insert_avltree(tree, 21);
        print(tree);  
        free_avltree(tree);
        return 0;
    }

.. code-block:: bash

    >> [ 21, 42 ]

Example Usage with Duplicates 
-----------------------------

.. code-block:: c

    #include "avl.h"
    #include "print.h"

    int main() {
        intAVLTree* tree = init_avltree(dInt)(true);
        insert_avltree(tree, 42);
        insert_avltree(tree, 21);
        insert_avltree(tree, 21);
        print(tree);  
        free_avltree(tree);
        return 0;
    }

.. code-block:: bash

    >> [ 21, 21, 42 ]

This example initializes an integer AVL tree and inserts two integers using the 
``insert_avltree`` macro. The macro correctly selects ``insert_int_avltree`` 
based on the type of the tree pointer passed.

Remove from AVL Tree
====================
The ``remove_avltree`` macro provides a generic interface for removing elements 
from AVL trees of various data types. Utilizing the C11 ``_Generic`` keyword, 
this macro enables type-based dispatch to select the appropriate type-specific 
removal function according to the type of the AVL tree provided.

.. code-block:: c

    #define remove_avltree(tree, value) _Generic((tree), \
        charAVLTree*: remove_char_avltree, \
        ucharAVLTree*: remove_uchar_avltree, \
        shortAVLTree*: remove_short_avltree, \
        ushortAVLTree*: remove_ushort_avltree, \
        intAVLTree*: remove_int_avltree, \
        uintAVLTree*: remove_uint_avltree, \
        longAVLTree*: remove_long_avltree, \
        ulongAVLTree*: remove_ulong_avltree, \
        llongAVLTree*: remove_llong_avltree, \
        ullongAVLTree*: remove_ullong_avltree, \
        floatAVLTree*: remove_float_avltree, \
        doubleAVLTree*: remove_double_avltree, \
        ldoubleAVLTree*: remove_ldouble_avltree, \
        boolAVLTree*: remove_bool_avltree, \
        stringAVLTree*: remove_string_avltree)(tree, value)

The above macro also highlights the underlying functions that the user chan 
directly invoke if they so choose.

Parameters
----------
- :c:`tree`: A pointer to an AVL tree data structure, dynamically typed based on the type of tree.
- :c:`value`: The data to be removed from the AVL tree, type-compatible with the tree's data type.

.. note:: If the user passes a value that is not contained in the tree, the function will return control to the calling program with no effect to the structure. 

Error Handling
--------------
Each removal function first checks if the tree pointer or value is ``NULL``. 
If any argument is ``NULL``, ``errno`` is set to ``EINVAL`` to indicate an 
invalid argument error. This ensures that attempting to remove from a ``NULL`` 
tree pointer or using a ``NULL`` value does not lead to undefined behavior.

.. note:: The function ``remove_string_avltree`` also checks the validity of the string pointer to ensure it is not ``NULL`` before attempting removal, preventing potential crashes from dereferencing null pointers during string comparisons.

Example Usage
-------------

.. code-block:: c

    #include "avl.h"
    #include "print.h"

    int main() {
        intAVLTree* tree = init_avltree(dInt)(false);
        insert_avltree(tree, 42);
        insert_avltree(tree, 21);
        insert_avltree(tree, 20);
        insert_avltree(tree, 8);
        remove_avltree(tree, 42);
        print(tree);  
        free_avltree(tree);
        return 0;
    }

.. code-block:: bash

    >> [ 8, 20, 21 ]

This example demonstrates initializing an integer AVL tree, inserting four 
integers, and then removing one using the ``remove_avltree`` macro. 
The macro correctly selects ``remove_int_avltree`` based on the type of the 
tree pointer passed, effectively removing the specified value.

avltree_size
============
The ``avltree_size`` macro offers a uniform interface to retrieve the size of 
AVL trees across various data types. Utilizing the C11 ``_Generic`` keyword, 
it directs the call to the appropriate type-specific function that returns the 
size of the tree based on its type.

.. code-block:: c

    #define avltree_size(tree) _Generic((tree), \
        charAVLTree*: char_avltree_size, \
        ucharAVLTree*: uchar_avltree_size, \
        shortAVLTree*: short_avltree_size, \
        ushortAVLTree*: ushort_avltree_size, \
        intAVLTree*: int_avltree_size, \
        uintAVLTree*: uint_avltree_size, \
        longAVLTree*: long_avltree_size, \
        ulongAVLTree*: ulong_avltree_size, \
        llongAVLTree*: llong_avltree_size, \
        ullongAVLTree*: ullong_avltree_size, \
        floatAVLTree*: float_avltree_size, \
        doubleAVLTree*: double_avltree_size, \
        ldoubleAVLTree*: ldouble_avltree_size, \
        boolAVLTree*: bool_avltree_size, \
        stringAVLTree*: string_avltree_size)(tree)

The user can also interact directly with the macros underlying functions.

Parameters
----------
- :c:`tree`: A pointer to an AVL tree of any supported data type.

.. note:: Each function called by the macro returns the number of nodes (elements) in the AVL tree, allowing easy measurement of the tree's population.

Error Handling
--------------
Each function linked through this macro verifies if the provided tree pointer is 
``NULL`` before attempting to access its properties. If ``NULL``, ``errno`` is 
set to ``EINVAL`` (Invalid Argument), ensuring the program signals an error 
condition properly.

.. warning:: This function does not modify the tree but will report an error via ``errno`` if a ``NULL`` tree is mistakenly provided.

Example Usage
-------------

.. code-block:: c

    #include "avl.h"
    #include "print.h"

    int main() {
        intAVLTree* tree = init_avltree(dInt)(false);
        insert_avltree(tree, 10);
        insert_avltree(tree, 20);
        size_t size = avltree_size(tree);
        print("Size of AVL Tree: ", size);
        free_avltree(tree);
        return 0;
    }

.. code-block:: bash

    >> Size of AVL Tree: 2

This example initializes an integer AVL tree, inserts two integers into it, 
and then uses the ``avltree_size`` macro to determine how many elements are in 
the tree before it is freed.

min_avltree 
===========
The ``min_avltree`` macro provides a generic interface for obtaining the 
minimum value from AVL trees of various data types. Leveraging the C11 
``_Generic`` keyword, it dispatches the call to the appropriate type-specific 
function that retrieves the smallest value from the tree based on its type.

.. code-block:: c

    #define min_avltree(tree) _Generic((tree), \
        charAVLTree*: min_char_avltree, \
        ucharAVLTree*: min_uchar_avltree, \
        shortAVLTree*: min_short_avltree, \
        ushortAVLTree*: min_ushort_avltree, \
        intAVLTree*: min_int_avltree, \
        uintAVLTree*: min_uint_avltree, \
        longAVLTree*: min_long_avltree, \
        ulongAVLTree*: min_ulong_avltree, \
        llongAVLTree*: min_llong_avltree, \
        ullongAVLTree*: min_ullong_avltree, \
        floatAVLTree*: min_float_avltree, \
        doubleAVLTree*: min_double_avltree, \
        ldoubleAVLTree*: min_ldouble_avltree, \
        boolAVLTree*: min_bool_avltree, \
        stringAVLTree*: min_string_avltree)(tree)

Parameters
----------
- :c:`tree`: A pointer to an AVL tree of any supported data type.

.. note:: Each function called by this macro walks the tree to the leftmost node, which represents the minimum value in the AVL tree.

Error Handling
--------------
Each function verifies if the provided tree pointer is ``NULL`` before attempting 
to access its properties. If ``NULL``, ``errno`` is set to ``EINVAL`` 
(Invalid Argument), which helps prevent undefined behavior and signals improper usage.

.. warning:: Care must be taken with the returned value from trees containing dynamic data types like strings. Do not free the returned ``str*`` data directly, as it remains managed within the tree structure. Improper handling could lead to memory leaks or double free errors.

Example Usage
-------------

.. code-block:: c

    #include "avl.h"
    #include "print.h"

    int main() {
        stringAVLTree* tree = init_avltree(dString)(false);
        insert_avltree(tree, "Orange");
        insert_avltree(tree, "Apple");
        insert_avltree(tree, "Banana");
        str* minStr = min_avltree(tree);
        print("Minimum value: ", minStr->data);
        free_avltree(tree);
        return 0;
    }

.. code-block:: bash

    >> Minimum value: Apple

This example initializes a string AVL tree, inserts several strings, retrieves 
the smallest string without needing to free the returned value explicitly, and 
then frees the entire tree safely.

max_avltree 
===========
The ``max_avltree`` macro provides a generic interface for obtaining the 
maximum value from AVL trees of various data types. Leveraging the C11 
``_Generic`` keyword, it dispatches the call to the appropriate type-specific 
function that retrieves the smallest value from the tree based on its type.

.. code-block:: c

    #define max_avltree(tree) _Generic((tree), \
        charAVLTree*: max_char_avltree, \
        ucharAVLTree*: max_uchar_avltree, \
        shortAVLTree*: max_short_avltree, \
        ushortAVLTree*: max_ushort_avltree, \
        intAVLTree*: max_int_avltree, \
        uintAVLTree*: max_uint_avltree, \
        longAVLTree*: max_long_avltree, \
        ulongAVLTree*: max_ulong_avltree, \
        llongAVLTree*: max_llong_avltree, \
        ullongAVLTree*: max_ullong_avltree, \
        floatAVLTree*: max_float_avltree, \
        doubleAVLTree*: max_double_avltree, \
        ldoubleAVLTree*: max_ldouble_avltree, \
        boolAVLTree*: max_bool_avltree, \
        stringAVLTree*: max_string_avltree)(tree)

Parameters
----------
- :c:`tree`: A pointer to an AVL tree of any supported data type.

.. note:: Each function called by this macro walks the tree to the rightmost node, which represents the maximum value in the AVL tree.

Error Handling
--------------
Each function verifies if the provided tree pointer is ``NULL`` before attempting 
to access its properties. If ``NULL``, ``errno`` is set to ``EINVAL`` 
(Invalid Argument), which helps prevent undefined behavior and signals improper usage.

.. warning:: Care must be taken with the returned value from trees containing dynamic data types like strings. Do not free the returned ``str*`` data directly, as it remains managed within the tree structure. Improper handling could lead to memory leaks or double free errors.

Example Usage
-------------

.. code-block:: c

    #include "avl.h"
    #include "print.h"

    int main() {
        stringAVLTree* tree = init_avltree(dString)(false);
        insert_avltree(tree, "Orange");
        insert_avltree(tree, "Apple");
        insert_avltree(tree, "Banana");
        str* maxStr = max_avltree(tree);
        print("Maximum value: ", minStr->data);
        free_avltree(tree);
        return 0;
    }

.. code-block:: bash

    >> Maximum value: Orange

This example initializes a string AVL tree, inserts several strings, retrieves 
the greatest string without needing to free the returned value explicitly, and 
then frees the entire tree safely.
