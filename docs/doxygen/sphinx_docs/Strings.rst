.. _string_struct:

.. role:: c(code)
   :language: c

Strings
=======
The standard C library does not provide a dedicated string data type but 
instead allows users to create string literals or arrays of ``char`` characters. 
While the implementation of string literals in the C language is straightforward 
and efficient, it can lead to security issues, most notably buffer overflow attacks. 
This chapter introduces a ``str`` data type created for this library that helps 
minimize the security risks associated with C strings. It also aims to enhance 
the efficiency of processing strings. All functionalities described in this chapter 
are accessible through the ``str.h`` header file.

Structure
=========
The ``str`` library utilizes the following structure to manage strings:

.. code-block:: c

   typedef struct
   {
       char* data;   // Pointer to the string data
       size_t len;   // Length of the string not including the null terminator
       size_t alloc; // Total allocated space for the string in bytes
   } str;

Attributes
----------

- :c:`data`: A pointer to the string data.
- :c:`len`: The length of the string in bytes, excluding the null terminator ``\0``.
- :c:`alloc`: The total amount of allocated memory for the string data, in bytes.

.. _init_string:

Initialize String
=================
Strings in this library are encapsulated within a dynamically allocated 
``str`` structure. The memory required to store the string data is also 
dynamically allocated, necessitating proper memory management. Both the 
string structure and the data it contains must be freed after use.

The ``init_string`` macro is designed to instantiate and initialize a string 
structure. This macro simulates function overloading by wrapping two functions. 
In the following implementation, the ``buff`` parameter is optional. Omitting 
``buff`` prompts the function to allocate enough memory for the string plus the 
null terminator. If ``buff`` is provided and is less than the length of the 
string plus the null terminator, enough memory will be allocated to accommodate 
the string and the terminator. Optionally, ``buff`` can be set larger than the 
initial string length to avoid reallocations if the string is expected to grow, 
thereby optimizing performance. The space complexity of this operation is 
:math:`O(1)`, and the time complexity is :math:`O(n)`.

.. code-block:: c

   str* init_string(char* string, size_t buff);

Parameters
----------

- :c:`string`: A string literal. Passing a non-null-terminated character array 
  will yield a returen ``str`` data type with a NULL pointer to the ``data`` attribute.
- :c:`buff`: The buffer size for allocating storage for the ``string`` and 
  potential future growth. This value defaults to the length of ``string`` if not provided.

Returns
-------

- :c:`str*`: A pointer to the string structure of type ``str``. Returns NULL and 
  logs an error to stderr in the event of a memory allocation failure.

Example 1
---------
The following example demonstrates initializing a string with just enough memory 
for the string content. We must manually free both the structure and the string 
using the ``free_string`` function.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str* one = init_string("Hello World!");
       print(one);
       print(string_length(one));
       print(string_memory(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> Hello World!
   >> 11
   >> 12

Example 2
---------
This example illustrates initializing a string with a buffer larger than 
necessary, anticipating future growth and avoiding the need for reallocation.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str* one = init_string("Hello World!", 30);
       print(one);
       print(string_length(one));
       print(string_memory(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> Hello World!
   >> 11
   >> 30

Underlying Functions
--------------------
The ``init_string`` macro makes use of the C11 ``_Generic`` keyword to simulate 
overloading, wrapping the following two functions:

.. code-block:: c

   str init_string_nol(char *string);
   str init_string_len(char *string, size_t buff);

Below is an example using these functions directly:

.. code-block:: c 
   
   #include "print.h"
   #include "str.h"
  
   int main() {
       str* one = init_string_nol("Hello!");
       str* two = init_string_len("Hello!", 30);
       free_string(one);
       free_string(two);
       return 0;
   }

Initialize String with Garbage Collection
=========================================
Several functions in this library return a ``str*`` data type, including the 
``init_string`` macro. When a function returns a ``str*``, the developer is 
usually required to free the memory allocations at some point in the program. 
To bypass manual memory management, developers can employ the ``gbc_str`` 
macro that employs the ``__attribute__(cleanup)`` directive for automatic 
memory cleanup, thus facilitating garbage collection. 
**NOTE:** This macro is compatible only with the ``gcc`` and ``clang`` 
compilers.

Refer to the :ref:`Initialize String <init_string>` section for attribute 
details. The examples below illustrate how to use the ``gbc_str`` macro, 
thereby obviating the need to call the ``free_string`` function explicitly. 
Although this initialization automates memory management, the option to 
manually call ``free_string`` remains. This method has a space complexity of 
:math:`O(1)` and a time complexity of :math:`O(n)`. 
**NOTE:** After freeing memory, caution should be taken to avoid double-free 
errors, though the underlying function includes safeguards against this.

Example 1
---------
Here, a string is initialized with only the necessary memory allocation. 
Note that the manual call to ``free_string`` is omitted.

.. code-block:: c

   #include "str.h"
   #include "print.h"

   int main() {
       str* one gbc_str = init_string("Hello World!");
       print(one);
       print(string_length(one));
       print(string_memory(one));
       // No need to call free_string(one);
       return 0;
   }

.. code-block:: bash

   >> Hello World!
   >> 11
   >> 12


Example 2
---------

This example initializes a string with additional buffer space to minimize 
future allocations.

.. code-block:: c

   #include "str.h"
   #include "print.h"

   int main() {
       str* one gbc_str = init_string("Hello World!", 30);
       print(one);
       print(string_length(one));
       print(string_memory(one));
       // No need to call free_string(one);
       return 0;
   }

.. code-block:: bash

   >> Hello World!
   >> 11
   >> 30

Free String
===========
The ``free_string`` function is designed to free all memory associated with 
an ``str`` struct, including the struct itself and its dynamic memory 
allocations. This function has a space complexity of :math:`O(1)` and a time 
complexity of :math:`O(1)`.

.. code-block:: c

   void free_string(str* str_struct);

Parameters
----------

- :c:`str_struct`: A pointer to a string struct of type ``str``.

Example
-------

.. code-block:: c

   #include "str.h"

   int main() {
       str* one = init_string("Hello World!", 30);
       free_string(one);
       // After this point, 'one' should not be used without being reassigned.
       return 0;
   }

Get String
==========
Directly interfacing with the ``str`` struct is not recommended, as it poses 
a risk of unintentionally modifying an attribute, potentially leading to 
undefined behavior. The ``get_string`` function provides a safe way to access 
the string content within the ``str`` struct without exposing the underlying 
implementation. This function has a space complexity of 
:math:`O(1)` and a time complexity of :math:`O(1)`.

.. code-block:: c

   char* get_string(const str* str_struct);

Parameters
----------

- :c:`str_struct`: A pointer to a string container of type ``str``.

Returns
-------

- A pointer to a string (``char*``). If the input is a NULL pointer, it will 
  return `NULL` and print an error message to `stderr`.

Example
-------
This example demonstrates how to use the ``get_string`` function to retrieve 
a string from a ``str`` struct.

.. code-block:: c

   #include "str.h"
   #include "print.h"

   int main() {
       str* one = init_string("Hello World!");
       const char* string = get_string(one);
       if (string) {
           print(string);
       }
       free_string(one);
       return 0;
   }

.. code-block:: bash

   >> Hello World!

String Length
=============
Direct interaction with the ``str`` struct is discouraged as it could lead to 
unintentional modifications, causing undefined behavior. The ``string_length`` 
function is provided for safely obtaining the length of the string stored in 
a ``str`` struct. Unlike string literals in C, which use a null terminator to 
denote the end of the string, the length in this implementation is stored as 
an attribute of the ``str`` struct, providing constant-time access to the 
string's length. This function has a space complexity of 
:math:`O(1)` and a time complexity of :math:`O(1)`.

.. code-block:: c

   size_t string_length(const str* str_struct);

Parameters
----------

- :c:`str_struct`: A pointer to a string container of type ``str``. If the 
  input is NULL, it will return `0` and print an error message to `stderr`.

Returns
-------

- The length of the string in the ``str`` container, not including the null terminator.

Example
-------
This example demonstrates how to use the ``string_length`` function to obtain 
the length of a string within a ``str`` struct.

.. code-block:: c

   #include "str.h"
   #include "print.h"

   int main() {
       str* one = init_string("Hello World!");
       size_t length = string_length(one);
       if (one) {
           print(length);
       }
       free_string(one);
       return 0;
   }

.. code-block:: bash

   >> 11

String Memory
=============
The ``string_memory`` function informs the user of the memory allocation size 
for the string in units of ``chars``. While it is possible to access this 
information through the ``struct->alloc`` attribute, direct manipulation of 
struct attributes is risky and could result in undefined behavior. To prevent 
such issues, this function provides a safe means to retrieve the allocation 
size. This function has a space complexity of :math:`O(1)` and a time 
complexity of :math:`O(1)`. 

.. code-block:: c

   int string_memory(const str *str_struct);

Parameters
----------

- :c:`str_struct`: A pointer to a struct of type ``str``.

Returns
-------

- :c:`alloc`: The memory allocation size for the string in units of ``chars``. If the input is NULL, it will return a value of -1.

Example
-------

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str *one = init_string("Hello", 20);
       if (one) {
           print("The string size is: %zu\n", string_memory(one));
       }
       free_string(one);
       return 0;
   }

.. code-block:: bash

   >> The string size is: 20

Insert String
=============
The ``insert_string`` function allows a user to insert a string literal or 
another ``str`` container into a ``str`` container. The function can insert 
the string at any position within the first string. If the provided data 
contains NULL values, or if memory allocation for the string concatenation 
fails, the function will return ``false``. The operation has a time complexity 
of :math:`O(a+b)`, where :math:`a` and :math:`b` are the lengths of the 
original and inserted strings respectively. Inserting at the end of the 
original string has a time complexity of :math:`O(b)`. If additional memory 
allocation is required, it may be up to the combined length of the two strings.

.. code-block:: c

   bool insert_string(str *str_one, const char* || str* str_two, size_t index);

Parameters
----------

- :c:`str_one`: The ``str`` container into which the string will be inserted.
- :c:`str_two`: Either a string literal or another ``str`` container to insert.
- :c:`index`: The index at which ``str_two`` will be inserted into ``str_one``.

Returns
-------

- Returns ``true`` if the function executes successfully, otherwise returns ``false`` and prints an error message to ``stderr``.

Examples 1
----------
Example to show when a string literal is instered into a ``str`` container.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str *a = init_string("Hello");
       bool result = insert_string(a, " World!", string_length(a));
       print("Operation successful: ", result);
       print("Combined string: ", a);
       print("Length of combined string: ", string_length(a));
       free_string(a);
       return 0;
   }

.. code-block:: bash

   >> Operation successful: true
   >> Combined string: Hello World!
   >> Length of combined string: 12

Example 2
---------
This example shows a user where a ``str`` container is inserted into another 
``str`` container.

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *a = init_string("Hello");
       str *b = init_string(" World!");
       bool result = insert_string(a, b, 2);
       print("Operation succesful: ", result);
       print("Combined string: ", a);
       print("Length of combined string: ", string_length(one));
       free_string(one);
       free_string(two);
       return 0;
   }

.. code-block:: bash 

   >> Operation succesful: true
   >> Combined string: He World!llo
   >> Length of combined string: 11

Example 3
---------
This example shows how the function fails when an index out of bounds is
selected.  The function can also fail for a failure to reallocate memory 
if required, or if the user passes a NULL ``str`` container or string literal,
or if one of the ``str`` containers has a NULL pointer to its string.

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *a = init_string("Hello");
       str *b = init_string(" World!");
       bool result = insert_string(a, b, 50);
       print("Operation succesful: ", result);
       print("Combined string: ", a);
       print("Length of combined string: ", string_length(one));
       free_string(one);
       free_string(two);
       return 0;
   }

.. code-block:: bash 

   >> Operation succesfull: false
   >> Combined string: Hello
   >> Length of combined string: 5

Underlying Functions 
--------------------
The ``insert_string`` macro relies on a ``_Generic`` operator that connects
the following two functions which can be used in place of the ``insert_string``
macro.

.. code-block:: c

   bool insert_string_lit(str *str_struct, char *string, size_t index);
   bool insert_string_str(str *str_struct_one, str *str_struct_two, size_t index);

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *a = init_string("Hello");
       str *b = init_string(" World!");
       bool result = insert_string(a, b, 2);
       print(result);
       print(a);
       print(string_length(one));
       free_string(one);
       free_string(two);
       return 0;
   }

.. code-block:: bash 

   >> true
   >> He World!llo
   >> 11

Trim String
===========
The ``trim_string`` function adjusts the memory allocated for a ``str`` 
container so that it matches the length of the string plus one for the 
null-terminator. This is useful for reclaiming memory if the original 
allocation was larger than necessary. The function returns ``true`` if 
successful. It may return ``false`` if the string container's memory is 
already undersized relative to the string length, or if a NULL pointer is 
passed for the string container or its data.

The function has a time complexity of :math:`O(1)`. It performs a single check 
and a possible reallocation, but this does not depend on the length of the 
string itself. The space complexity is :math:`O(n)`, where :math:`n` is the 
length of the string. In the worst case, the function may reduce the 
allocation to match exactly the space needed for the string, which includes 
the string length plus one for the null terminator. 

.. code-block:: c

   bool trim_string(str *str_struct);

Parameters
----------

- :c:`str_struct`: A string container of type ``str`` to be trimmed.

Returns
-------

- Returns ``true`` if the function executes successfully, otherwise returns ``false`` with an error message printed to ``stderr``.

Example 1
---------
Example for an oversized string

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       // String with oversized memory allocation
       str *one = init_string("Hello", 30);
       print("Length: ", one);
       print("Allocated memory before trim: ", string_memory(one));
       bool val = trim_string(one);
       print("Trim successful: ", val);
       print("Length after trim: ", string_length(one));
       print("Allocated memory after trim: ", string_memory(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash

   >> Length: 5
   >> Allocated memory before trim: 30
   >> Trim successful: true
   >> Length after trim: 5
   >> Allocated memory after trim: 6

Example 2
---------
Example for a properly sized string

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       // String with proper memory allocation
       str *one = init_string("Hello");
       print("Length: ", string_length(one));
       print("Allocated memory before trim: ", string_memory(one));
       bool val = trim_string(one);
       print("Trim successful: ", val);
       print("Length after trim: ", string_length(one));
       print("Allocated memory after trim: ", string_memory(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash

   >> Length: 5
   >> Allocated memory before trim: 6
   >> Trim successful: true
   >> Length after trim: 5
   >> Allocated memory after trim: 6

Example 3
---------
Example where a NULL struct is passed to the function.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       // Passing a NULL struct to the function
       bool val = trim_string(NULL);
       print("Trim successful: ", val);
       return 0;
   }

.. code-block:: bash

   >> Null pointer provided to trim_string
   >> Trim successful: false

Copy String
===========
The ``copy_string`` function creates a deep copy of a provided ``str`` container, 
including a duplicate of the string data and its associated memory allocation. 
If the copy is successful, a pointer to the new string container is returned. 
If the function encounters an error, such as a NULL pointer as input or a 
failure in memory allocation, it returns NULL.  This function has a time 
complexity of :math:`O(n)` and a space complexity of :math:`O(max(n, num))` 
where :math:`n` and :math:`num` represent the string length and the allocation 
length respectively.

.. code-block:: c

   str* copy_string(str *str_struct);

Parameters
----------

- :c:`str_struct`: A string container of type ``str`` to be copied.

Returns
-------

- Returns a pointer to the newly created copy of the input ``str`` struct, or NULL if an error occurs.

Example
-------

.. code-block:: c

   #include "str.h"
   #include "print.h"

   int main() {
       str *one = init_string("Hello", 20);
       str *two = copy_string(one);
       if (two) {
           print(get_string(two));
           print(string_length(two));
           print(string_memory(two));
       }
       // Remember to free the memory for both string containers
       free_string(one);
       free_string(two);
       return 0;
   }

.. code-block:: bash

   >> Hello
   >> 5
   >> 20

Compare Strings
===============

The ``compare_strings`` macro is designed to compare a string container 
against another string container or a string literal. This macro uses the C11 
``_Generic`` keyword to choose the appropriate function for comparison based 
on the type of the second argument. The functions perform a 
character-by-character comparison and return an integer value that reflects 
the comparison result. This macro has the time complexity 
:math:`O(min(N,M))` where :math:`N` and :math:`M` represent then
lengths of the two strings.  This function also has a space complexity of
:math:`O(1)`.

.. code-block:: c

    int compare_strings(str *str_one, str* || char* str_two);

Parameters
----------

- ``str_one``: A pointer to a string container of type ``str``.
- ``str_two``: Either a second string container of type ``str`` or a string literal (``char*``).

Returns
-------

An ``int`` value:

- ``INT_MIN`` if a NULL pointer is provided for any of the strings, or if a string's data is NULL.
- ``0`` if both strings are equal.
- A negative value if ``str_one`` is lexicographically less than ``str_two``.
- A positive value if ``str_one`` is lexicographically greater than ``str_two``.

If a NULL pointer is provided for any of the strings, or if a string's data 
is NULL, the function returns ``INT_MIN`` as defined in the ``limits.h``
header file, and outputs an error message to ``stderr``.

Example Usage
-------------
Example 1: Comparing a string container with a string literal.

.. code-block:: c

    #include "print.h"
    #include "str.h"

    int main() {
        str *one = init_string("Hello");
        int result = compare_strings(one, "Helloo");
        print(result);
        free_string(one);
        return 0;
    }

.. code-block:: bash

    >> -1

Example 2: Comparing two string containers.

.. code-block:: c

    #include "print.h"
    #include "str.h"

    int main() {
        str *one = init_string("Hello");
        str *two = init_string("Hello");
        int result = compare_strings(one, two);
        print(result);
        free_string(one);
        free_string(two);
        return 0;
    }

.. code-block:: bash

    >> 0

Underlying Functions
--------------------

The ``compare_strings`` macro relies on the following functions, chosen by 
the ``_Generic`` keyword based on the type of the second argument:

``compare_strings_lit``
    Compares a string container with a string literal.

    .. code-block:: c

        int compare_strings_lit(str *str_struct, char *string);

``compare_strings_str``
    Compares two string containers.

    .. code-block:: c

        int compare_strings_str(str *str_struct_one, str *str_struct_two);

The comparison is done lexicographically and is safe as long as the string 
literals are null-terminated. When using string containers, both the length 
and the content are compared, which is inherently safe and does not depend 
on null-termination.

Find Char 
=========
The ``first_char`` and ``last_char`` functions search for the first or last 
occurrence of a character within the range specified by two pointers. They 
ensure that ``min_ptr`` is not greater than ``max_ptr`` and return a NULL 
pointer with a message to ``stderr`` if this condition is not met.
These functions have a time complexity of :math:`O(n)` and a space 
complexity of :math:`O(1)`.

.. code-block:: c

   char* first_char(char c, char* min_ptr, char* max_ptr);
   char* last_char(char c, char* min_ptr, char* max_ptr);

Parameters
----------

- :c:`c`: The character being searched for.
- :c:`min_ptr`: A pointer to the starting position of the search range.
- :c:`max_ptr`: A pointer to the ending position of the search range.

Returns
-------

- :c:`ptr`: A pointer to the found character `c`, or NULL if not found.

Example 1
---------
An example of using the functions to search a string literal from beginning 
to end for an instance of a character.

.. code-block:: c

   #include "print.h"

   int main() {
       char* str = "Hello this is a string";
       char* ptr1 = first_char('i', str, str + strlen(str));
       char* ptr2 = last_char('i', str, str + strlen(str));
       print(ptr1);
       print(ptr2);
       return 0;
   }

.. code-block:: bash

   >> is is a string
   >> ing

Example 2
---------
An example where ``min_ptr`` skips the first few characters of a string.

.. code-block:: c

   #include "print.h"

   int main() {
       str* str = init_string("ababcdefg");
       char* result = first_char('b', get_string(str), get_string(str) + string_length(str));
       print(result);
       return 0;
   }

.. code-block:: bash

   >> bcdefg

Example 3
---------
Example when the ``min_ptr`` is greater than ``max_ptr``.

.. code-block:: c

   #include "print.h"

   int main() {
       char* str = "Hello this is a string";
       char* result = first_char('i', str + strlen(str), str);
       print(result); // Expect NULL
       return 0;
   }

.. code-block:: bash

   >> Error: min_ptr is not smaller than max_ptr in first_char
   >> NULL

Find Strings
============
The ``first_substring`` and ``last_substring`` macros are designed to locate the 
first or last occurrence of a sub-string within a specific section of a string. 
These macros perform a search for the sub-string bounded by the given upper and 
lower pointer values. Passing a NULL value for the sub-string pattern or an invalid 
pointer range will result in an error message printed to ``stderr`` and a return 
value of NULL. This macro has a time complexity of :math:`O(n*m)` where 
:math:`n` and :math:`m` are the sizes of the string and sub-string 
respectively, and the space complexity is :math:`O(1)`.

.. code-block:: c

   char* first_substring(str* string || char* string, char* min_ptr, char* max_ptr);
   char* last_substring(str* string || char* string, char* min_ptr, char* max_ptr);

Parameters
----------

- :c:`string`: A string literal or a string structure containing the sub-string pattern. It must not be NULL.
- :c:`min_ptr`: A char pointer to the minimum location for a sub-string search.
- :c:`max_ptr`: A char pointer to the maximum location for a sub-string search. The search is conducted up to but not including `max_ptr`.

Return
------

- :c:`ptr`: A char pointer to the location of the sub-string within the specified range, or NULL if the sub-string is not found or in case of an error.

Example 1
---------
Here is an example where ``first_substring`` is used to search an entire string.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       char* one = "This is a string in a string";
       char* two = "string";
       char* three = first_substring(two, one, one + strlen(one));
       print(three); // Output should be a pointer to the first occurrence of "string" in "one"
       return 0;
   }

.. code-block:: bash

   >> string in a string

Example 2
---------
This example demonstrates ``first_substring`` being applied to a portion of a string.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str* one = init_string("This is a string in a string");
       char* two = "string";
       char* three = first_substring(two, get_string(one) + 18, get_string(one) + string_length(one));
       print(three); // Output should be a pointer to "string" found after the 18th character in "one"
       free_string(one);
       return 0;
   }

.. code-block:: bash

   >> string

Example 3
---------
This example shows the usage of ``last_substring`` when an error occurs.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str* one = init_string("This is a string in a string");
       str* two = {.data=NULL, .len=0, .alloc=0};
       char* three = last_substring(two, get_string(one) + 18, get_string(one) + string_length(one));
       print(three);
       free_string(one);
       return 0;
   }

.. code-block:: bash

   >> NUll struct information provided to first_str_between_ptrs
   >> NULL

Pop Char 
========
The ``pop_string_char`` macro provides a convenient way to remove a character 
from a string container. It selects the appropriate function to call based on 
the number of arguments provided: if an index is specified, it uses 
``pop_str_char_index``; otherwise, it defaults to using ``pop_str_char``, 
which removes the last character in the string. If errors such as null 
pointers or an out of bounds index occur, the macro returns a null terminator 
``\0`` and prints an error message to ``stderr``. This macro can have a 
time complexity that ranges from :math:`O(1)` to :math:`O(n)` depending on
where data is popped from in the string.  This function has a space complexity 
of :math:`O(1)`.

.. code-block:: c 

   char pop_string_char(str *str_struct[, size_t index]);

Parameters 
----------

- :c:`str_struct`: A string container of type ``str``.
- :c:`index`: [Optional] The index at which to pop the character. If not provided, the last character of the string is popped.

Returns 
-------

- :c:`char_val`: The ``char`` value popped from the string. If the string is empty or a null pointer is provided, ``'\0'`` is returned.

Example 1
---------
Pop the last character from the string using `pop_string_char` without providing an index.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str *one = init_string("Goodbye");
       char val = pop_string_char(one);
       print(val);  // Outputs: e
       print(one);  // Outputs: Goodby
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> e
   >> Goodby 

Example 2 
---------
Pop a character from a specific index in the string. This operation is more 
costly than popping from the end of the string.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str *one = init_string("Goodbye");
       char val = pop_string_char(one, 3);
       print(val);  // Outputs: b
       print(one);  // Outputs: Goode
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> b
   >> Goode

Underlying Functions 
--------------------
The ``pop_string_char`` macro selects from one of two functions to allow the 
appearance of overloading.  The functions can be used in place of the macro and 
they are shown below.

.. code-block:: c

   char pop_str_char(str *str_struct);
   char pop_str_index(str *str_struct, size_t index);

Pop String Token
================
The ``pop_string_token`` macro wraps two functions that allow a user to pop 
all data from a string to the right of the right most token.  If the function 
recieved NULL pointers for the ``str`` struct, or the struct data, it will 
return a NULL pointer, and write a message to ``stderr``.

.. code-block:: c 

   str* pop_string_token(str *str_struct, char token);

Parameters
----------

- :c:`str_struct`: A string container of type ``str``
- :c:`token`: A character that denotes the division point in the string.

Returns 
-------

- :c:`str_struct`: a new ``str`` object containing the substring after the last occurrence of `token`. If `token` is not found, returns NULL.

Example 1
---------
Return a string that must be manually freed 

.. code-block:: c

   #define "print.h"
   #define "str.h"

   int main() {
       str *one = init_string("2023/10/24");
       str *two = pop_string_token(one, '/');
       // The same as str *two = pop_string_token(one, '/', false);
       print(one);
       print(two);

       free_string(one);
       free_string(two);
       return 0;
   }

.. code-block:: bash 

   >> 2023/10 
   >> 24 

Example 2
---------
In this example we will create to strings that are garbage collected and 
do not need to be manually freed.

.. code-block:: c 


   #define "print.h"
   #define "str.h"

   int main() {
       str *one = init_string_gbc("2023/10/24");
       str *two gbc_str = pop_string_token(one, '/');
       print(one);
       print(two);
       return 0;
   }

.. code-block:: bash 

   >> 2023/10 
   >> 24

Underlying Functions 
--------------------
The ``pop_string_token`` macro wraps two functions that can be used in place of
the macro.  The functions are shown below.

.. code-block:: c 

   str* string_pop_token_wogbc(str *str_struct, char token);
   str* string_pop_token_wgbc(str *str_struct, char token, bool gdb);

Test Pointer
============
To support operations such as iteration, it is necessary to validate ``char`` 
pointers before their use, ensuring they point within the bounds of a string 
container or a string literal. The functions ``ptr_in_str_container`` 
and ``ptr_in_literal`` can be used for such validations.

.. code-block:: c

   bool ptr_in_str_container(str* string, char* ptr);
   bool ptr_in_literal(char* string, char* ptr);

Parameters
----------

- :c:`string`: A null-terminated string for `ptr_in_literal` or ``str`` data type for ``ptr_in_str_container`` function.
- :c:`ptr`: The `char` pointer to be validated.

Returns
-------

- :c:`status`: ``true`` if ``ptr`` is within the bounds of the string, ``false`` otherwise.

Example 1
---------
Check if a pointer is within the bounds of a literal before iterating over it. 
This example prints a string one character at a time.

.. code-block:: c

   #include <stdio.h>
   #include "str.h"

   int main() {
       const char* one = "Hello Again!";
       const char* begin = one;
       const char* end = one + strlen(one);

       for (const char* i = begin; i < end; i++) {
           if(ptr_in_literal(one, i)) {
               printf("%c", *i);
           }
       }
       printf("\n");
       return 0;
   }

.. code-block:: bash

   >> Hello Again!

Example 2
---------
Check if a pointer is within the bounds of a string container before iterating 
over it. This example prints the string one character at a time, starting from 
the second character.

.. code-block:: c

   #include <stdio.h>
   #include "str.h"

   int main() {
       str* my_str = init_string("Hello Again!");
       char* begin = my_str->data + 1;
       char* end = my_str->data + my_str->len;

       for (char* i = begin; i < end; i++) {
           if(ptr_in_str_container(my_str, i)) {
               printf("%c", *i);
           }
       }
       printf("\n");
       free_string(my_str);
       return 0;
   }

.. code-block:: bash

   >> ello Again!

Iterator
========
Iterators are powerful tools in C that enable traversing data structures, 
regardless of whether they have contiguous memory allocation or not, such as 
arrays, linked lists, or trees. An iterator abstracts the process of stepping 
through a collection, often using a pointer to reference current elements 
rather than array indices.

The ``str`` container in this library represents a dynamically allocated 
string, and an iterator for this type facilitates the manipulation and 
traversal of its characters. This is preferable to using raw loops and 
indices, providing a common interface across different data structures.

The ``str_iterator`` struct encapsulates function pointers for standard 
iteration operations:

.. code-block:: c 

   typedef struct {
       char* (*begin) (str* s);    // Returns pointer to first element
       char* (*end) (str* s);      // Returns pointer to one past the last element
       void (*next) (char** current);    // Advances the pointer to the next element
       void (*prev) (char** current);    // Moves the pointer to the previous element
       char (*get) (char** current);    // Returns the value at the current element
   } str_iterator;

Attributes 
----------

- :c:`begin`: This function pointer returns the address of the first character in a `str` container, or NULL if an error occurs (such as when a NULL pointer is provided).
- :c:`end`: This function pointer returns the address of the null terminator in a `str` container, signifying one past the last valid character.
- :c:`next`: This function pointer advances the current pointer to the next character.
- :c:`prev`: This function pointer moves the current pointer to the previous character.
- :c:`get`: This function pointer retrieves the character at the current position pointed to by the provided pointer address.

To use the iterator, initialize it with the ``init_str_iterator`` function, 
which sets up the function pointers to their corresponding internal functions.

Example 1
---------
This example shows how to use the iterator with a for loop to iterate forwards 
through data in a ``str`` container and Capitalize the values within the 
string.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       str *one = init_string("This is a Long String");
       str_iterator it = init_str_iterator();
       char* begin = it.begin(one);
       char* end = it.end(one);
       char a;
       for (char* i =  begin; i != end; it.next(&i)) {
           a = it.get(&i);
           if (a >= 'a' && a <= 'z') *i -= 32;
       }
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> THIS IS A LONG STRING

Example 2
---------
In this example we use an while loop to iterate through the desired
data.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main () {
       str *one = init_string("This is a Long String");
       str_iterator it = init_str_iterator();
       char* begin = it.begin(one);
       char* end = it.end(one);
       char a; 
       while(begin != end) {
           a = it.get(&begin);
           if (a >= 'a' && a <= 'z') *begin -= 32;
           it.next(&begin);
       }
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> THIS IS A LONG STRING

Decorate Iterator
=================
The `dec_str_iter` function is designed to apply a specified operation to each 
character within a range of a string, as defined by `begin` and `end` pointers. 
This operation is determined by a `decorator` function passed as an argument, 
allowing for flexible manipulation of string characters. This approach is 
based on the strategy pattern, effectively decoupling the iteration process 
from the actions performed during iteration.

.. code-block:: c 

   void dec_str_iter(char* begin, char* end,
                     str_iter_dir direction, str_decorator decorator);

Parameters
----------

- :c:`begin`: A pointer to the starting character within a `str` container for decoration.
- :c:`end`: A pointer to the character just past the last character to be decorated within a `str` container. The `end` character itself is not decorated.
- :c:`direction`: An enumeration value of type `str_iter_dir` that specifies the direction of iteration; it can be `FORWARD` or `REVERSE`.
- :c:`decorator`: A function pointer of type `str_decorator` which is an alias for `void func(char*)`. This function is applied to each character in the specified range.

Error Handling
--------------

If the iterators are incorrectly ordered (e.g., `end` is before `begin` for 
forward iteration), the function will report an error and not perform any 
decoration. If the `decorator` function pointer is `NULL`, the function 
should safely handle this case, print a message to ``stderr`` and return 
control to the calling program.

Example 1
---------
The following example demonstrates how to use `dec_str_iter` to convert 
lowercase characters to uppercase, starting from the fourth character of the 
string.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   void uppercase_char(char* a) {
       if (*a >= 'a' && *a <= 'z') {
           *a -= 32;
       }
   }

   int main() {
       str *one = init_string("This is a Long String");
       str_iterator it = init_str_iterator();
       char* begin = it.begin(one);
       char* end = it.end(one); 
       dec_str_iter(begin + 3, end, FORWARD, uppercase_char);
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> ThiS IS A LONG STRING

Uppercase
=========
The `to_uppercase` function converts all alphabetic characters in a string to 
their uppercase equivalents. Other characters in the string are not affected.

.. code-block:: c 

   void to_uppercase(str *s);

Parameters 
----------

- :c:`s`: A non-NULL string container of type `str` with `s->data` pointing to 
  a valid null-terminated string.

Error Handling
--------------

The function will output an error message to `stderr` if a null pointer is 
passed either as the ``s`` parameter or within the ``s->data``.

Example 
-------
Convert a string to uppercase in memory.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       str* one = init_string("this will be uppercase");
       to_uppercase(one);
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> THIS WILL BE UPPERCASE 

Lowercase
=========
The `to_lowercase` function converts all alphabetic characters in a string to 
their lowercase equivalents. Other characters in the string remain unchanged.

.. code-block:: c 

   void to_lowercase(str *s);

Parameters 
----------

- :c:`s`: A non-NULL string container of type `str` with `s->data` pointing to a valid null-terminated string.

Error Handling
--------------

The function will output an error message to `stderr` if a null pointer is 
passed either as the ``s`` parameter or within the ``s->data``.

Example 
-------
Convert a string to lowercase in memory.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       str* one = init_string("THIS WILL BE LOWERCASE");
       to_lowercase(one);
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> this will be lowercase

Delete Substring
================
The ``drop_substring`` macro provides a way to remove all occurrences of a 
specified substring within a given range of a string. It uses the C11 
``_Generic`` keyword to dispatch to the correct function based on the type 
of `substring` provided. The macro will handle string containers (``str*``) 
or string literals (``char*``). If null pointers are passed, or if the provided 
range is outside the bounds of the string, an error message is printed to 
``stderr``.

.. code-block:: c 

   bool drop_substring(str* string, str* || char* substring, char* min_ptr, char* max_ptr);

Parameters 
----------

- :c:`string`: A non-NULL string container of type `str` from which substrings will be deleted.
- :c:`substring`: The pattern to remove, which can be of type `char*` for string literals or `str*` for string containers.
- :c:`min_ptr`: A pointer to the minimum position in `string` from where the search begins.
- :c:`max_ptr`: A pointer to the maximum position in `string` to which the search is limited.

Returns 
-------

- :c:`status`: Returns ``true`` if the function completes its search, which does not necessarily mean a substring was removed; returns ``false`` if an error occurs, with a corresponding message printed to `stderr`.

Example 1
---------
A use case where the developer passes a string literal to the function to 
remove all occurances of a string between two pointers.

.. code-block:: c

   #include "str.h"
   #include "print.h"

   int main() {
       str* data = init_string("Remove all instances of Remove without this Remove");
       char* pattern = "Remove";
       drop_substring(data, pattern, get_string(one), get_string(one) + (string_length(one) - 7));
       print(data)
       free_string(data);
       return 0;
   }

.. code-block:: bash 

   >> all instances of without this Remove 

Example 2 
---------
A user case where all instances of a sub-string are removed from the entire string 
where the pattern is a string container.

.. code-block:: c

   #include "str.h"
   #include "print.h"

   int main() {
       str* data = init_string("Remove all instances of Remove without this Remove");
       str* pattern = init_string("Remove");
       drop_substring(data, pattern, get_string(one), get_string(one) + (string_length(one));
       print(data)
       free_string(data);
       free_string(pattern);
       return 0;
   }

.. code-block:: bash 

   >> all instances of without this 

Replace Substring
=================

The ``replace_substring`` macro selects between two functions that search for 
a substring pattern in a string. If the pattern is found, it replaces the 
pattern with a developer-supplied replacement substring. The operation returns 
``false`` and prints a message to ``stderr`` if any argument is ``NULL``.

.. note::
   The type of ``pattern_string`` and ``replacement_string`` must match. They can either be of type ``str*`` or ``char*``.

Usage:

.. code-block:: c

   bool replace_substring(str* string, char* || str* pattern_string,
                          char* || str* replacement_string,
                          char* min_ptr, char* max_ptr);

Parameters
----------

- ``string``: The string object containing substrings to be replaced.
- ``pattern_string``: The substring pattern to search for within ``string``.
- ``replacement_string``: The substring that will replace each instance of ``pattern_string``.
- ``min_ptr``: A pointer to the minimum position in ``string`` to start the search.
- ``max_ptr``: A pointer to the maximum position in ``string`` to end the search.

Returns:
--------

- ``bool``: ``true`` if the operation is successful, ``false`` otherwise.

Examples
--------

Example 1: Replacing a substring with a longer one.

.. code-block:: c

   #include "str.h"

   int main() {
       str* my_string gbc_str = init_string("Remove all 'Remove' with 'Replace'!");
       str* pattern gbc_str = init_string("Remove");
       str* replacement gbc_str = init_string("Replace");
       replace_substring(my_string, pattern, replacement, get_string(my_string), 
                         get_string(my_string) + string_length(my_string));
       print(my_string);
       return 0;
   }

.. code-block:: bash 

   >> Replace all 'Replace' with 'Replace'!

Example 2: Replacing a substring with a shorter one.

.. code-block:: c

   #include "str.h"
   #include "print.h"

   int main() {
       str* my_string = init_string("Remove all 'Remove' with 'Replace'!");
       char* pattern = "Remove";
       char* replacement = "Rep";
       replace_substring(my_string, pattern, replacement, get_string(my_string), 
                         get_string(my_string) + string_length(my_string));
       print(my_string);
       free_string(my_string);
       return 0;
   }

.. code-block:: bash 

   >> Rep all 'Rep' with 'Rep'!

Example 3: Replacing a substring within part of the string.

.. code-block:: c

   #include "str.h"
   #include "print.h"

   int main() {
       str* my_string = init_string("Remove all 'Remove' with 'Replace'!");
       char* pattern = "Remove";
       char* replacement = "Rep";
       char* max_ptr = get_string(my_string) + string_length(my_string) - strlen(" with 'Replace'!");
       replace_substring(my_string, pattern, replacement, get_string(my_string), max_ptr);
       print(my_string);
       free_string(my_string);
       return 0;
   } 

.. code-block:: bash 

   >> Rep all 'Rep' with 'Replace'!

Underlying Functions
--------------------

The ``replace_substring`` macro utilizes the ``_Generic`` keyword to choose 
between the following two functions. These functions can be used directly in 
lieu of the macro.

.. code-block:: c

   bool replace_str_substring(str* string, str* pattern_string,
                              str* replacement_string,
                              char* min_ptr, char* max_ptr);
   bool replace_literal_substring(str* string, char* pattern_string,
                                  char* replacement_string,
                                  char* min_ptr, char* max_ptr);

