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

str_iter 
--------
The `str_iter` data type is used to manage a string iterator for 
quick user access to the internal string. 

.. code-block:: c

   // mutable interface
   typedef struct {
       string_t *owner;
       char     *begin;
       char     *end;    /* one past last */
       char     *cur;    /* current; valid iff cur < end */
   } str_iter;

   // immutable interface
   typedef struct {
       const string_t *owner;
       const char     *begin;
       const char     *end;    /* one past last */
       const char     *cur;    /* current; valid iff cur < end */
   } cstr_iter;

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

reserve_string
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

.. note:: Error codes can be found in :ref:`Error Code Overview <error_code_file>`. 

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

token_count 
~~~~~~~~~~~

.. doxygenfunction:: token_count
   :project: csalt

get_char
~~~~~~~~

.. doxygenfunction:: get_char
   :project: csalt

first_char
~~~~~~~~~~

.. doxygenfunction:: first_char
   :project: csalt

last_char
~~~~~~~~~

.. doxygenfunction:: last_char
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

replace_char
~~~~~~~~~~~~

.. doxygenfunction:: replace_char
   :project: csalt

trim_leading_whitespace
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: trim_leading_whitespace
   :project: csalt

trim_trailing_whitespace
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: trim_trailing_whitespace
   :project: csalt

trim_all_whitespace
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: trim_all_whitespace
   :project: csalt

swap_string
~~~~~~~~~~~

.. doxygenfunction:: swap_string
   :project: csalt

tokenize_string 
~~~~~~~~~~~~~~~

.. doxygenfunction:: tokenize_string
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
The iterator lets you traverse the payload of a ``string_t`` without exposing
its internals. Iterators are invalidated by operations that can reallocate,
such as concatenation, reserve, replace, or drop.

Mutable API Reference
~~~~~~~~~~~~~~~~~~~~~
.. doxygenstruct:: str_iter
   :project: csalt
.. doxygenfunction:: str_iter_make
   :project: csalt
.. doxygenfunction:: str_iter_valid
   :project: csalt
.. doxygenfunction:: str_iter_get
   :project: csalt
.. doxygenfunction:: str_iter_next
   :project: csalt
.. doxygenfunction:: str_iter_prev
   :project: csalt
.. doxygenfunction:: str_iter_advance
   :project: csalt
.. doxygenfunction:: str_iter_seek_begin
   :project: csalt
.. doxygenfunction:: str_iter_seek_end
   :project: csalt

Immutable API Reference 
~~~~~~~~~~~~~~~~~~~~~~~
.. doxygenstruct:: cstr_iter
   :project: csalt
.. doxygenfunction:: cstr_iter_make
   :project: csalt
.. doxygenfunction:: cstr_iter_valid
   :project: csalt
.. doxygenfunction:: cstr_iter_get
   :project: csalt
.. doxygenfunction:: cstr_iter_next
   :project: csalt
.. doxygenfunction:: cstr_iter_prev
   :project: csalt
.. doxygenfunction:: cstr_iter_advance
   :project: csalt
.. doxygenfunction:: cstr_iter_seek_begin
   :project: csalt
.. doxygenfunction:: cstr_iter_seek_end
   :project: csalt

Basic forward iteration
~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c

   #include "c_string.h"
   #include "csalt_string_iter.h"
   #include <stdio.h>

   void print_forward(string_t* s) {
       cstr_iter it = cstr_iter_make(s);
       for (; cstr_iter_valid(&it); cstr_iter_next(&it)) {
           putchar(cstr_iter_get(&it));
       }
       putchar('\n');
   }

   int main(void) {
       string_t* s = init_string("Hello, world!");
       if (!s) { perror("init_string"); return 1; }
       print_forward(s);
       free_string(s);
       return 0;
   }

Output::
  
  Hello, world!

Reverse iteration
~~~~~~~~~~~~~~~~~
.. code-block:: c

   #include "c_string.h"
   #include "csalt_string_iter.h"
   #include <stdio.h>

   void print_reverse(string_t* s) {
       str_iter it = str_iter_make(s);

       /* Go to end (one-past-last), then step back to last if any */
       str_iter_seek_end(&it);
       while (str_iter_prev(&it)) {
           putchar(str_iter_get(&it));
       }
       putchar('\n');
   }

   int main(void) {
       string_t* s = init_string("abcde");
       if (!s) { perror("init_string"); return 1; }
       print_reverse(s);
       free_string(s);
       return 0;
   }

Output::
  
  edcba

Find the first digit (returns pointer and index)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c

   #include "c_string.h"
   #include "csalt_string_iter.h"
   #include <stdio.h>

   const char* find_first_digit_ptr(const string_t* s) {
       cstr_iter it = cstr_iter_make(s);
       for (; cstr_iter_valid(&it); cstr_iter_next(&it)) {
           char ch = cstr_iter_get(&it);
           if (ch >= '0' && ch <= '9') {
               return cstr_iter_ptr(&it);  /* pointer into s payload */
           }
       }
       return NULL;
   }

   int main(void) {
       string_t* s = init_string("id=42; name=alice");
       if (!s) { perror("init_string"); return 1; }

       const char* p = find_first_digit_ptr(s);
       if (p) {
           /* position via iterator rebuilt at p */
           cstr_iter it = cstr_iter_make(s);
           while (cstr_iter_valid(&it) && cstr_iter_ptr(&it) != p) {
               cstr_iter_next(&it);
           }
           printf("first digit '%c' at index %zu\\n", *p, cstr_iter_pos(&it));
       } else {
           puts("no digit");
       }
       free_string(s);
       return 0;
   }

Possible output::
  
  first digit '4' at index 3

Windowed replacement using iterator-friendly bounds
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c

   #include "c_string.h"
   #include "csalt_string_iter.h"
   #include <stdio.h>

   int main(void) {
       string_t* s = init_string("foo X foo Y foo Z");
       if (!s) { perror("init_string"); return 1; }

       /* Choose a window: first 9 characters only ("foo X foo") */
       char* lo = first_char(s);   /* inclusive lower bound */
       char* hi = lo + 8;          /* inclusive upper bound */

       if (!replace_substr(s, "foo", "BAR", lo, hi)) {
           perror("replace_substr");
           fprintf(stderr, "%s\\n", error_to_string(get_string_error(s)));
           free_string(s);
           return 1;
       }

       printf("%s\\n", get_string(s));
       free_string(s);
       return 0;
   }

Output::
  
  BAR X BAR Y foo Z

Uppercase in place with the mutable iterator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c

   #include "c_string.h"
   #include "csalt_string_iter.h"
   #include <ctype.h>

   void to_uppercase_iter(string_t* s) {
       str_iter it = str_iter_make(s);
       for (; str_iter_valid(&it); str_iter_next(&it)) {
           char* p = str_iter_ptr(&it);
           unsigned char c = (unsigned char)*p;
           if (c >= 'a' && c <= 'z') *p = (char)(c - ('a' - 'A'));
       }
   }

   int main(void) {
       string_t* s = init_string("Hello, World! 123");
       to_uppercase_iter(s);
       puts(get_string(s));  /* HELLO, WORLD! 123 */
       free_string(s);
       return 0;
   }

Checksum over bytes with a const iterator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c

   #include "c_string.h"
   #include "csalt_string_iter.h"
   #include <stdint.h>
   #include <stdio.h>

   uint32_t checksum32(const string_t* s) {
       uint32_t h = 2166136261u;          /* FNV-1a base */
       cstr_iter it = cstr_iter_make(s);
       for (; cstr_iter_valid(&it); cstr_iter_next(&it)) {
           h ^= (uint8_t)cstr_iter_get(&it);
           h *= 16777619u;
       }
       return h;
   }

   int main(void) {
       string_t* s = init_string("abc");
       printf("%u\\n", (unsigned)checksum32(s));
       free_string(s);
       return 0;
   }

.. note::

   - Iterators are **invalidated** by any operation that may reallocate or resize
     the string (e.g., concat, reserve, replace, drop). If you need to mutate and
     then continue iterating, store **offsets** (indices) rather than raw pointers,
     perform the mutation, then **rebuild** the iterator from the new buffer.
   - ``end`` is one-past-last; ``*_get`` returns ``0`` when the iterator is not valid.

String Vector Overview 
======================
This section describes that attributes and functions associated with the 
`string_v` data type.

Initialization and Memory Management
------------------------------------

init_str_vector
~~~~~~~~~~~~~~~

.. doxygenfunction:: init_str_vector
   :project: csalt

free_str_vector
~~~~~~~~~~~~~~~

.. doxygenfunction:: free_str_vector
   :project: csalt

STRVEC_GBC 
~~~~~~~~~~

.. doxygendefine:: STRVEC_GBC 
   :project: csalt

Utility Funcitons 
-----------------

get_str_vector_error
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: get_str_vector_error
   :project: csalt

.. note:: Error codes can be found in :ref:`Error Code Overview <error_code_file>`.

str_vector_size
~~~~~~~~~~~~~~~

.. doxygenfunction:: str_vector_size
   :project: csalt

str_vector_alloc
~~~~~~~~~~~~~~~~

.. doxygenfunction:: str_vector_alloc
   :project: csalt

str_vector_index
~~~~~~~~~~~~~~~~

.. doxygenfunction:: str_vector_index
   :project: csalt

Vector Manipulation 
-------------------
The following functions can be used to insert data into a string vector.

push_back_str_vector
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: push_back_str_vector
   :project: csalt

push_front_str_vector
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: push_front_str_vector
   :project: csalt

pop_back_str_vector
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_back_str_vector
   :project: csalt

pop_front_str_vector
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_front_str_vector
   :project: csalt

pop_any_str_vector
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_any_str_vector
   :project: csalt

insert_str_vector
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: insert_str_vector
   :project: csalt

delete_front_str_vector
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: delete_front_str_vector
   :project: csalt

delete_back_str_vector
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: delete_back_str_vector
   :project: csalt

delete_any_str_vector
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: delete_any_str_vector
   :project: csalt

reverse_str_vector
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: reverse_str_vector
   :project: csalt

sort_str_vector
~~~~~~~~~~~~~~~

.. doxygenfunction:: sort_str_vector
   :project: csalt

Search Vector
-------------

binary_search_str_vector
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: binary_search_str_vector
   :project: csalt

String Vector Iterator 
----------------------
