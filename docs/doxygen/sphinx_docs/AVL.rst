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

Initialize an AVL Tree 
=======================
The ``avl.h`` header file provides the ``init_avltree`` function for initializing 
an AVL Binary Tree linked list.  This function is essentially a function pointer, 
intelligently selecting the appropriate initialization function based on the 
specified data type.

.. code-block:: c

   type init_avltree(dtype dat_type)()

Parameters 
----------

- :c:`dat_type`: The data type for the singly linked list, represented by a ``dtype`` as described in :ref:`Enum Data Types <dllist_type_enum>`.

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

   int main() {
       floatAVLTree* list = init_avltree(dFloat)();
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
