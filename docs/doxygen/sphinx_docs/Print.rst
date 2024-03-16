*****
Print
*****
The C language natively includes the ``printf`` function for formatted output 
to ``stdout``. However, for simpler use cases where formatting isn't needed, 
the ``print`` macro in the ``print.h`` header file offers an easy alternative. 
It allows printing up to 8 variables of any standard data type, including those 
defined in this library, without explicit format specifiers.

Standard Data Types
===================
The ``print`` macro can handle any standard C data type, including but not 
limited to ``char``, ``unsigned char``, ``int``, ``unsigned int``, ``long int``, 
``unsigned long int``, ``long long int``, ``unsigned long long int``, 
``float``, ``double``, ``long double``, ``bool``, and strings (``char*``). 
Below is an example demonstrating its usage.

Eaxmple
-------
A basic example for the use of the ``print`` macro.

.. code-block:: c 

   #include "print.h"

   int main() {
       int a = 1;
       long int b = 2;
       double c = 3.4;
       print("An int: ", a, ", A long int: ", b, ", A double: ", c);
       return 0;
   }

.. code-block:: bash 

   >> An int: 1, A long int: 2, A double 3.40000

String Container
================
The library introduces the ``str`` data type, a dynamically allocated structure 
that encapsulates a string literal along with its metadata. This struct offers 
a more robust and flexible way of handling strings in C. The ``str`` struct is 
elaborated in the :ref:`String <string_struct>` section of this document. 
The ``print`` macro is capable of printing the ``str`` data type, treating it 
as one of the up to eight variables it can handle in a single call.

Example 1
---------
This example demonstrates using the ``print`` macro to print a literal string 
alongside a ``str`` data type.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       char* literal = "This is a string literal.\n";
       str* container = init_string("This is a string container!");
       print(literal, container);
       // Remember to free dynamically allocated memory
       free_string(container);
       return 0;
   }

.. code-block:: bash 

   >> This is a string literal.
   >> This is a string container!

Example 2
---------
The ``str`` data type is dynamically allocated and may be freed during program 
execution. It's crucial to ensure that a `str` object is not printed after it 
has been freed. The ``print`` macro includes logic to handle such scenarios 
gracefully by checking for NULL pointers and providing informative messages.

.. note:: While most compilers detect and prevent operations on freed memory during compilation, accessing freed memory is undefined behavior in C. The `print` macro's checks serve as an additional safeguard.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str* container = init_string("This is a string container!");
       free_string(container);
       // Attempt to print a null struct 
       print(container);
        
       // Attempt to print a struct with a null string
       str empty = { .data = NULL, .len = 0, .alloc = 0 }; 
       print(&empty);

       return 0;
   }

.. code-block:: bash 

   >> Error: The struct is a NULL pointer.
   >> Error: The string is a NULL pointer.

Vectors and Arrays
==================
The library enhances its functionality with derived data types for dynamically 
allocated vectors, as detailed in the :ref:`Vector <vector_lib>` section. 
The ``print`` macro is specially designed to handle these vector data types. 
It prints the contents of a vector, encapsulated within square brackets 
(i.e., ``[ ]``), either on a single line or across multiple lines for longer 
vectors, providing a clear and concise representation of the vector's contents.

The ``print`` macro is also derived to allow for the implementation of the 
statically allocated array data structure, as detailed in the :ref:`Array <array_lib>`
section.

Example 1
---------
This example demonstrates printing a vector data type alongside a string. The 
macro automatically formats the vector output for readability.

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       int_v* vec = init_vector(dInt)(5);
       push_vector(vec, 1, vec->len);
       push_vector(vec, 2, vec->len);
       push_vector(vec, 3, vec->len);
       push_vector(vec, 4, 0);
       print("This is a vector: ", vec);
       free_vector(vec); // Always remember to free dynamically allocated memory
       return 0;
   }

.. code-block:: bash 

   >> This is a vector: [ 4, 1, 2, 3 ]

.. code-block:: c

   #include "print.h"
   #include "array.h"

   int main() {
       int val[5];
       int_arr arr = init_array(val, 5);
       push_vector(arr, 1, vec.len);
       push_vector(arr, 2, vec.len);
       push_vector(arr, 3, vec.len);
       push_vector(arr, 4, 0);
       print("This is an array: ", arr);
       return 0;
   }

.. code-block:: bash 

   >> This is a vector: [ 4, 1, 2, 3 ]

.. note:: The `print` macro adapts its output format based on the vector's length, ensuring optimal readability. For longer vectors, the output may span multiple lines to avoid overly long single-line outputs.

Min and Max Heap 
================
The print command can also be used with the :ref:`Min Heap <min_heap>` and 
:ref:`Max Heap <max_heap>` data structures.  When printing Min and Max heap data 
structures, the data will be printed as if it were an array, with square brackets 
(i.e. ``[ ]``) encapsulating the data.

Example 
-------
This example demonstrates the use of the ``print`` macro to display the contents 
of a min heap to the screen.  The syntax is identical for a max heap data 
structure.

.. code-block:: c 

   #include "heap.h"
   #include "print.h"

   int main() {
       char_min_hp* heap = init_min_heap(dChar)(7);
       enqueue_min_heap(heap, 5);
       enqueue_min_heap(heap, 1);
       enqueue_min_heap(heap, 2);
       enqueue_min_heap(heap, 3);
       enqueue_min_heap(heap, 6);
       enqueue_min_heap(heap, 4);
       enqueue_min_heap(heap, 7);
       print(heap);
       free_min_heap(heap);
       return 0;
   }

.. code-block:: bash 

   >> [ 1, 3, 2, 5, 6, 4, 7 ]

Singly and Doubly Linked Lists
==============================
The print command can also be used with the :ref:`Singly Linked Lists <sl_list>` and 
:ref:`Doubly Linked Lists <dl_list>` data structures.  When printing Singly and 
Doubly Linked List data structures, the data will be encapsulated by curly brackets 
(i.e. ``{ }``) encapsulating the data.

Example 
-------
This example demonstrates the use of the ``print`` macro to display the contents 
of a doubly linked list.  The syntax is identical for a singly linked list.

.. code-block:: c

   #include "dlist.h"
   #include "print.h"

   int main() {
       int_dl* list = init_dllist(dInt)();
       push_back_dllist(list, 1);
       push_back_dllist(list, 2);
       push_back_dllist(list, 3);
       push_back_dllist(list, 4);
       print(list);
       free_dllist(list);
       return 0;
   }

.. code-block:: bash 

   >> { 1, 2, 3, 4 }
