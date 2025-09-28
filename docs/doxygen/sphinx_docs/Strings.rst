*****************
C String Overview
*****************

The c_string library provides a dynamic string type and associated functions 
for safe string manipulation in C.  All of the functions described in this 
section can be found in the ``c_string.h`` header file.  This file implements 
the ``error_codes.h`` file to provide error code functionality.

Data Types
==========

string_t
--------
A dynamic string type that manages memory allocation automatically and provides safe string operations.
The internal structure is opaque to users, preventing direct manipulation of the memory.

While the data structure is opaque to the user, the struct has this 
format

.. code-block:: c

   typedef struct {
       char* str;           // pointer to string literal
       size_t len;          // The length of the populated literal 
       size_t alloc;        // The allocated memory in indices 
       ErrorCode error;     // The error code
   } string_t;

string_v
--------
The dynamic string vector type that manages memory allocation automatically and 
provides a safe encapsulation for ``string_t`` data types in an array format.
This struct is provided as an opaque data structure.

.. code-block:: c

   typedef struct {
       string_t* data;      // Pointer to array of string literals 
       size_t len;          // The populated length of the string array 
       size_t alloc;        // The allocated memory in indices 
       ErrorCode error;     // The error code
   } string_v;

String Overview 
===============
This section describes the attributes and functions associated with the 
`string_t` data type.

Initialization and Memory Management
------------------------------------
The functions and Macros in this section are used to control the creation,
memory allocation, and specific destruction of ``string_t`` data types.

init_string
~~~~~~~~~~~

.. doxygenfunction:: init_string
   :project: csalt

free_string
~~~~~~~~~~~

.. doxygenfunction:: free_string
   :project: csalt

STRING_GBC 
~~~~~~~~~~

.. doxygendefine:: STRING_GBC 
   :project: csalt 

trim_string
~~~~~~~~~~~

.. doxygenfunction:: trim_string
   :project: csalt

reserve-string
~~~~~~~~~~~~~~

.. doxygenfunction:: reserve_string
   :project: csalt

Utility Funcitons 
-----------------
The functions in this section are used to retrieve data from the `string_t` 
data structure.

get_string 
~~~~~~~~~~

.. doxygenfunction:: get_string
   :project: csalt

string_size
~~~~~~~~~~~

.. doxygenfunction:: string_size
   :project: csalt

string_alloc
~~~~~~~~~~~~

.. doxygenfunction:: string_alloc
   :project: csalt

get_string_error
~~~~~~~~~~~~~~~~

.. doxygenfunction:: get_string_error
   :project: csalt

compare_strings_lit
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: compare_strings_lit
   :project: csalt

compare_strings_string
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: compare_strings_string
   :project: csalt

compare_strings
~~~~~~~~~~~~~~~

.. doxygendefine:: compare_strings
   :project: csalt

copy_string 
~~~~~~~~~~~

.. doxygenfunction:: copy_string
   :project: csalt

pop_string_token 
~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_string_token
   :project: csalt

String Manipulation 
-------------------
The functions in this section are used to manipulate data in a `string_t` object.

string_string_concat 
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: string_string_concat
   :project: csalt

string_lit_concat 
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: string_lit_concat
   :project: csalt

string_concat 
~~~~~~~~~~~~~

.. doxygendefine:: string_concat
   :project: csalt

drop_lit_substr
~~~~~~~~~~~~~~~

.. doxygenfunction:: drop_lit_substr
   :project: csalt

drop_string_substr
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: drop_string_substr
   :project: csalt

drop_substr
~~~~~~~~~~~

.. doxygendefine:: drop_substr
   :project: csalt

replace_lit_substr
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: replace_lit_substr
   :project: csalt

replace_string_substr
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: replace_string_substr
   :project: csalt

replace_substr
~~~~~~~~~~~~~~

.. doxygendefine:: replace_substr
   :project: csalt

to_upper_char
~~~~~~~~~~~~~

.. doxygenfunction:: to_upper_char
   :project: csalt

to_lower_char
~~~~~~~~~~~~~

.. doxygenfunction:: to_lower_char
   :project: csalt

to_uppercase
~~~~~~~~~~~~

.. doxygenfunction:: to_uppercase
   :project: csalt

to_lowercase
~~~~~~~~~~~~

.. doxygenfunction:: to_lowercase
   :project: csalt

Search String 
-------------
The functions in this section can be used to search a string for spechic 
`char` values or sub strings.

first_char_occurance
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: first_char_occurrance
   :project: csalt

last_char_occurance
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: last_char_occurrance
   :project: csalt

first_lit_substr_occurrance 
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: first_lit_substr_occurrence 
   :project: csalt

first_string_substr_occurrance 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: first_string_substr_occurrence 
   :project: csalt

first_substr_occurrance 
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygendefine:: first_substr_occurrence 
   :project: csalt

last_lit_substr_occurrance 
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: last_lit_substr_occurrence 
   :project: csalt

last_string_substr_occurrance 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: last_string_substr_occurrence 
   :project: csalt

last_substr_occurrance 
~~~~~~~~~~~~~~~~~~~~~~

.. doxygendefine:: last_substr_occurrence 
   :project: csalt

is_string_ptr 
~~~~~~~~~~~~~

.. doxygenfunction:: is_string_ptr
   :project: csalt

String Iterator 
---------------
The following functions can be used to iterator over a string .

first_char
~~~~~~~~~~

.. doxygenfunction:: first_char
   :project: csalt

last_char
~~~~~~~~~

.. doxygenfunction:: last_char
   :project: csalt

String Vector Overview 
======================
This section describes that attributes and functions associated with the 
`string_v` data type.
