String Functions
================

The c_string library provides a dynamic string type and associated functions 
for safe string manipulation in C.  All of the functions described in this 
section can be found in the ``c_string.h`` header file.

Data Types
----------

string_t
~~~~~~~~
A dynamic string type that manages memory allocation automatically and provides safe string operations.
The internal structure is opaque to users, preventing direct manipulation of the memory.

While the data structure is opaque to the user, the struct has this 
format

.. code-block:: c

   typedef struct {
       char* str;
       size_t len;
       size_t alloc;
   } string_t;

string_v
--------
The dynamic string vector type that manages memory allocation automatically and 
provides a safe encapsulation for ``string_t`` data types in an array format.
This struct is provided as an opaque data structure.

.. code-block:: c

   typedef struct {
       string_t* data;
       size_t len;
       size_t alloc;
   } string_v;

Core Functions
--------------
The functions and Macros in this section are core to the creation and 
desctruction of ``size_t`` data types.

Initialization and Memory Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The functions and Macros in this section are used to control the creation,
memory allocation, and specific destruction of ``string_t`` data types.

init_string
^^^^^^^^^^^
.. c:function:: string_t* init_string(const char* str)

   Creates and initializes a new ``string_t`` object from a C-style string. If
   compiled with ``gcc`` of ``clang`` compilers, consider using :ref:`GBC_STRING
   macro <string_cleanup_macro>`.  If the developer wishes to Pre-allocate 
   a larger amount of memory than would be normally allocated with this function, 
   they can also call the :ref:`reserve_string <reserve-string-func>` function 
   after initialization.

   :param str: A null-terminated C string
   :returns: Pointer to new ``string_t`` object, or NULL on failure
   :raises: Sets errno to ENOMEM if memory allocation fails or EINVAL if str is NULL

   Example:

   .. code-block:: c

      string_t* str = init_string("Hello, World!");
      /*
       * if compiling with gcc or clang consider using 
       * string_t* str STRING_GBC init_string("Hello, World!");
       */
      if (str == NULL) {
          fprintf(stderr, "Failed to initialize string\n");
          return 1;
      }
      printf("String content: %s\n", get_string(str));
      // If STRING_GBC macro is used, their is no need to call free_string()
      free_string(str);

   Output::

      String content: Hello, World!

.. _reserve-string-func:

reserve_string
^^^^^^^^^^^^^^
.. c:function:: bool reserve_string(string_t* str, size_t len)

  Pre-allocates memory for a ``string_t`` object to optimize for future concatenations.
  Will not reduce allocation size below current size.

  :param str: ``string_t`` object to reserve memory for
  :param len: Desired buffer length in bytes
  :returns: true if successful, false if len is less than current allocation or on error
  :raises: Sets errno to EINVAL if str is NULL or len is too small, ENOMEM if allocation fails

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Hello");
     if (str) {
         printf("Initial allocation: %zu\n", string_alloc(str));
         
         // Reserve more space
         if (reserve_string(str, 20)) {
             printf("After reserve(20): %zu\n", string_alloc(str));
             
             // Try to reserve less space (should fail)
             if (!reserve_string(str, 10)) {
                 printf("Failed to reduce allocation as expected\n");
             }
             
             // Content remains unchanged
             printf("String content: %s\n", get_string(str));
         }
     }

  Output::

     Initial allocation: 6
     After reserve(20): 20
     Failed to reduce allocation as expected
     String content: Hello

trim_string
^^^^^^^^^^^
.. c:function:: bool trim_string(string_t* str)

  Reduces the allocated memory of a ``string_t`` object to the minimum required size
  (string length plus null terminator). This is useful for optimizing memory usage
  after string operations that might have left excess allocated space.

  :param str: ``string_t`` object to trim
  :returns: true if successful or already at minimum size, false on error
  :raises: Sets errno to EINVAL if str is NULL or corrupted, ENOMEM if reallocation fails

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Hello");
     if (str) {
         // First reserve extra space
         reserve_string(str, 20);
         printf("Before trim - Content: %s, Size: %zu, Allocated: %zu\n",
                get_string(str), string_size(str), string_alloc(str));
         
         // Now trim the excess space
         if (trim_string(str)) {
             printf("After trim  - Content: %s, Size: %zu, Allocated: %zu\n",
                    get_string(str), string_size(str), string_alloc(str));
         }
     }

  Output::

     Before trim - Content: Hello, Size: 5, Allocated: 20
     After trim  - Content: Hello, Size: 5, Allocated: 6

.. _free-string-func:

free_string
^^^^^^^^^^^
.. c:function:: void free_string(string_t* str)

   Deallocates all memory associated with a string_t object.

   :param str: Pointer to string_t object to free
   :raises: Sets errno to EINVAL if str is NULL

   Example:

   .. code-block:: c

      string_t* str = init_string("Hello");
      // Use the string...
      free_string(str);
      str = NULL;  // Good practice to avoid dangling pointers

Automatic Cleanup
~~~~~~~~~~~~~~~~~
In general the C language does not allow automated garbage collection of 
memory that is out of scope.  This section describes a poor mans 
garbage collection within the C language, for the ``string_t`` data type,
that can only be enabled if compiled with ``gcc`` or ``clang``.

.. _string_cleanup_macro:

STRING_GBC
^^^^^^^^^^
.. c:macro:: STRING_GBC

   Enables automatic cleanup of ``string_t`` objects when they go out of scope.
   Available only with ``GCC`` and ``Clang`` compilers.  If this option is available 
   for your compiler, this is the preferred method for memory management.

   Example:

   .. code-block:: c

      void example_function(void) {
          STRING_GBC string_t* str = init_string("Hello");
          if (!str) {
              return;
          }
          printf("String: %s\n", get_string(str));
          // No need to call free_string - cleanup happens automatically
      }

   Output::

      String: Hello

String Access Functions
-----------------------
The functions discussed in this section are used to access the string within 
the ``string_t`` data type as well as the allocated memory and size of a string.

get_string
~~~~~~~~~~
.. c:function:: const char* get_string(const string_t* str)

  Retrieves the C string stored in a ``string_t`` object.

  :param str: Pointer to the string_t object
  :returns: Pointer to the null-terminated string, or NULL on failure
  :raises: Sets errno to EINVAL if str is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Hello, World!");
     // If not compiled with gcc or clang, string_t* str = init_string("Hello, World!");
     if (str) {
         const char* content = get_string(str);
         if (content) {
             printf("String content: %s\n", content);
         }
         // If not compiled with gcc or clang, free_string(str);
     }

  Output::

     String content: Hello, World!

.. _string-size-func:

string_size
~~~~~~~~~~~
.. c:function:: const size_t string_size(const string_t* str)

  Returns the length of the string (number of characters excluding null terminator).
  This is the equivalant of the ``strlen`` function from the ``string.h`` header 
  file; however, this function is safely bounded by the length of the string 
  and is not prone to buffer overflow attacks.

  :param str: Pointer to the ``string_t`` object
  :returns: Length of string, or LONG_MAX on failure
  :raises: Sets errno to EINVAL if str is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Hello");
     // If not compiled with gcc or clang, string_t* str = init_string("Hello");
     if (str) {
         size_t len = string_size(str);
         if (len != LONG_MAX) {
             printf("String: %s\nLength: %zu\n", get_string(str), len);
         }
         // If not compiled with gcc or clang, free_string(str);
     }

  Output::

     String: Hello
     Length: 5

The developer may consider the use of the :ref:`s-size-macro` Macro as a generic 
helper function to determine the populated size of a string data type.

.. _string-alloc-func:

string_alloc
~~~~~~~~~~~~
.. c:function:: const size_t string_alloc(const string_t* str)

  Returns the total allocated capacity of the string buffer. 

  :param str: Pointer to the ``string_t`` object
  :returns: Allocated capacity in bytes, or LONG_MAX on failure
  :raises: Sets errno to EINVAL if str is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Test");
     // If not compiled with gcc or clang, string_t* str = init_string("Test");
     if (str) {
         printf("String: %s\n", get_string(str));
         printf("Length: %zu\n", string_size(str));
         printf("Allocated: %zu\n", string_alloc(str));
         // If not compiled with gcc or clang, free_string(str);
     }

  Output::

     String: Test
     Length: 4
     Allocated: 5

The developer may consider the use of the :ref:`s-alloc-macro` Macro as a generic 
helper function to determine the allocated size of a string data type.

String Manipulation Functions
-----------------------------
The functions and Macros within this section allow a user to manipulate the 
string data within the ``string_t`` data type.

Concatenation Functions and Macros 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
These functions and macros allow a user to concatenate data to a ``string_t``
data type with a string literal or another ``string_t`` data type.  These 
functions are essentially the equivalent of a ``push_back`` function for the 
``string_t`` data type.

.. _string-string-concat-func:

string_string_concat
^^^^^^^^^^^^^^^^^^^^
.. c:function:: bool string_string_concat(string_t* str1, const string_t* str2)

  Concatenates two ``string_t`` objects, appending the second string to the first.
  Developers should consider using the :ref:`string_concat macro <string-concat-macro>`
  in place of the ``string_string_concat`` function. This function is bounded by
  the length of the second string and is not prone to buffer overflow attacks.

  :param str1: Destination string_t object
  :param str2: Source string_t object to append
  :returns: true if successful, false on failure
  :raises: Sets errno to ENOMEM on allocation failure or EINVAL if either input is NULL

  Example:

  .. code-block:: c

     string_t* str1 STRING_GBC = init_string("Hello ");
     string_t* str2 STRING_GBC = init_string("World!");
     if (str1 && str2) {
         printf("Before: %s\n", get_string(str1));
         if (string_string_concat(str1, str2)) {
             printf("After:  %s\n", get_string(str1));
         }
     }

  Output::

     Before: Hello 
     After:  Hello World!

.. _string-lit-concat-func:

string_lit_concat
^^^^^^^^^^^^^^^^^
.. c:function:: bool string_lit_concat(string_t* str1, const char* literal)

  Concatenates a C string literal to a ``string_t`` object.Developers should consider 
  using the :ref:`string_concat macro <string-concat-macro>` in place of the 
  ``string_lit_concat`` function. 

  :param str1: Destination string_t object
  :param literal: C string to append
  :returns: true if successful, false on failure
  :raises: Sets errno to ENOMEM on allocation failure or EINVAL if either input is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Hello ");
     if (str) {
         printf("Before: %s\n", get_string(str));
         if (string_lit_concat(str, "World!")) {
             printf("After:  %s\n", get_string(str));
         }
     }

  Output::

     Before: Hello 
     After:  Hello World!

Drop Substring Functions and Macros 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The functions and Macros in this section are used to search the char data 
within a ``string_t`` data type for a sub-string.  If the sub-string is found 
it is removed from the string and the data is shifted an appropriate amount 
to form a contiguous char array within the ``string_t`` data type.

.. _drop-lit-substr-func:

drop_lit_substr
^^^^^^^^^^^^^^^
.. c:function:: bool drop_lit_substr(string_t* string, const char* substring, char* min_ptr, char* max_ptr)

  Removes all occurrences of a C string literal substring between two pointers in a ``string_t`` object.
  Searches from end to beginning and preserves existing spaces between words. 
  The :ref:`drop_substr <drop-substr-macro>` generic Macro can also be used in place of this function. 

  :param string: ``string_t`` object to modify
  :param substring: C string literal to remove
  :param min_ptr: Pointer to start of search range
  :param max_ptr: Pointer to end of search range
  :returns: true if successful (including no matches found), false on error
  :raises: Sets errno to EINVAL if inputs are NULL or range invalid, ERANGE if pointers out of bounds

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("hello world hello there hello");
     printf("Initial length: %ld\n", string_size(str));
     char* start = first_char(str);
     char* end = last_char(str);
     
     if (drop_lit_substr(str, "hello", start, end)) {
         printf("Result: '%s'\n", get_string(str));
         printf("Final length: %ld\n", string_size(str));
     }

  Output::

     Initial Length: 29
     Result: 'world there '
     Final length: 11

.. _drop-string-substr-func:

drop_string_substr
^^^^^^^^^^^^^^^^^^
.. c:function:: bool drop_string_substr(string_t* string, const string_t* substring, char* min_ptr, char* max_ptr)

  Removes all occurrences of a ``string_t`` substring between two pointers in another string_t object.
  Searches from end to beginning and preserves existing spaces between words.
  The :ref:`drop_substr <drop-substr-macro>` generic Macro can also be used in 
  place of this function.

  :param string: string_t object to modify
  :param substring: string_t object containing substring to remove
  :param min_ptr: Pointer to start of search range
  :param max_ptr: Pointer to end of search range
  :returns: true if successful (including no matches found), false on error
  :raises: Sets errno to EINVAL if inputs are NULL or range invalid, ERANGE if pointers out of bounds

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("hello world hello there hello");
     string_t* sub STRING_GBC = init_string("hello");
     printf("Initial length: %ld\n", string_size(str));
     char* start = first_char(str);
     char* end = last_char(str);
     
     if (drop_string_substr(str, sub, start, end)) {
         printf("Result: '%s'\n", get_string(str));
         printf("Final length: %ld\n", string_size(str));
     }

  Output::

     Initial Length: 29
     Result: 'world there '
     Final length: 11

.. _drop-substr-macro:

drop_substr
^^^^^^^^^^^
.. c:macro:: drop_substr(string, substr, min_ptr, max_ptr)

  A generic macro that selects the appropriate substring removal function based on
  the type of the second argument. Provides a unified interface for removing substrings
  regardless of the substring type.  This macro is a wrapper around the 
  :ref:`drop_lit_substr() <drop-lit-substr-func>` and 
  :ref:`drop_string_substr() <drop-string-substr-func>` functions.

  Example:

  .. code-block:: c

     string_t* str = init_string("test hello test hello test");
     string_t* sub STRING_GBC = init_string("hello");
     char* start = first_char(str);
     char* end = last_char(str);
     
     // Using with literal
     printf("Using literal - Before: '%s'\n", get_string(str));
     drop_substr(str, "hello", start, end);
     printf("After: '%s'\n", get_string(str));
     
     // Reset string
     free_string(str);
     str STRING_GBC = init_string("test hello test hello test");
     
     // Using with string_t
     printf("\nUsing string_t - Before: '%s'\n", get_string(str));
     drop_substr(str, sub, start, end);
     printf("After: '%s'\n", get_string(str));

  Output::

     Using literal - Before: 'test hello test hello test'
     After: 'test test test'
     
     Using string_t - Before: 'test hello test hello test'
     After: 'test test test'

String Replacement Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The functions described in this section can be used to replace sub string 
ocurrences over a user defined range with a new sub-string.

.. _replace-lit-substring-func:

replace_lit_substr
^^^^^^^^^^^^^^^^^^
.. c:function:: bool replace_lit_substr(string_t* string, const char* pattern, const char* replace_string, char* min_ptr, char* max_ptr)

  Replaces all occurrences of a C string literal pattern with a replacement string between
  two specified pointers in a ``string_t`` object. Searches from right to left.
  Developers may also choose to implement the :ref:`replace_substring <replace-substring-macro>`
  Macro in place of this function.

  :param string: ``string_t`` object to modify
  :param pattern: C string literal to search for and replace
  :param replace_string: C string literal to replace pattern with
  :param min_ptr: Pointer to start of search range
  :param max_ptr: Pointer to end of search range
  :returns: true if successful (including no matches found), false on error
  :raises: Sets errno to EINVAL if inputs are NULL, ERANGE if pointers out of bounds

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("hello world hello there hello");
     char* start = first_char(str);
     char* end = last_char(str);
     
     // Replace all "hello" with "hi" in the entire string
     if (replace_lit_substr(str, "hello", "hi", start, end)) {
         printf("After replacement: '%s'\n", get_string(str));
     }
     
  Output::

     After replacement: 'hi world hi there hi'

.. _replace-string-substring-func:

replace_string_substr
^^^^^^^^^^^^^^^^^^^^^
.. c:function:: bool replace_string_substr(string_t* string, const string_t* pattern, const string_t* replace_string, char* min_ptr, char* max_ptr)

  Replaces all occurrences of a ``string_t`` pattern with another ``string_t`` between
  two specified pointers in a ``string_t object``. Searches from right to left.
  Developers may also choose to implement the :ref:`replace_substring <replace-substring-macro>`
  Macro in place of this function.

  :param string: ``string_t`` object to modify
  :param pattern: ``string_t`` object containing pattern to search for
  :param replace_string: ``string_t`` object containing replacement string
  :param min_ptr: Pointer to start of search range
  :param max_ptr: Pointer to end of search range
  :returns: true if successful (including no matches found), false on error
  :raises: Sets errno to EINVAL if inputs are NULL, ERANGE if pointers out of bounds

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("hello world hello there");
     string_t* pattern STRING_GBC = init_string("hello");
     string_t* replacement STRING_GBC = init_string("hi");
     
     char* start = first_char(str) + 6;  // Start after first "hello"
     char* end = last_char(str);
     
     // Replace "hello" with "hi" only in specified range
     if (replace_string_substr(str, pattern, replacement, start, end)) {
         printf("After partial replacement: '%s'\n", get_string(str));
     }
     
  Output::

     After partial replacement: 'hello world hi there'

.. _replace-substring-macro:

replace_substr
^^^^^^^^^^^^^^
.. c:macro:: replace_substr(string, pattern, replace_string, min_ptr, max_ptr)

  A generic macro that selects the appropriate replacement function based on the type
  of the pattern argument. Provides a unified interface for string replacement operations.
  This macro wraps the :ref:`replace_string_substr() <replace-string-substring-func>`
  and :ref:`replace_lit_substr() <replace-lit-substring-func>` functions

  Example:

  .. code-block:: c

     string_t* str = init_string("hello world hello there");
     string_t* pat = init_string("hello");
     string_t* rep = init_string("hi");
     char* start = first_char(str);
     char* end = last_char(str);
     
     // Using with literal strings
     replace_substr(str, "hello", "hi", start, end);
     printf("Using literals: '%s'\n", get_string(str));
     
     // Reset string
     free_string(str);
     str = init_string("hello world hello there");
     
     // Using with string_t objects
     replace_substr(str, pat, rep, start, end);
     printf("Using string_t: '%s'\n", get_string(str));
     
     free_string(str);
     free_string(pat);
     free_string(rep);

  Output::

     Using literals: 'hi world hi there'
     Using string_t: 'hi world hi there'

String Case Conversion
~~~~~~~~~~~~~~~~~~~~~~

to_uppercase
^^^^^^^^^^^^
.. c:function:: void to_uppercase(string_t* s)

  Converts all lowercase letters in a ``string_t`` object to uppercase.
  Non-alphabetic characters are left unchanged.

  :param s: ``string_t`` object to convert
  :raises: Sets errno to EINVAL if s is NULL or contains invalid data

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Hello, World! 123");
     printf("Before: %s\n", get_string(str));
     
     to_uppercase(str);
     printf("After: %s\n", get_string(str));

  Output::

     Before: Hello, World! 123
     After: HELLO, WORLD! 123

to_lowercase
^^^^^^^^^^^^
.. c:function:: void to_lowercase(string_t* s)

  Converts all uppercase letters in a ``string_t`` object to lowercase.
  Non-alphabetic characters are left unchanged.

  :param s: ``string_t`` object to convert
  :raises: Sets errno to EINVAL if s is NULL or contains invalid data

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Hello, WORLD! 123");
     printf("Before: %s\n", get_string(str));
     
     to_lowercase(str);
     printf("After: %s\n", get_string(str));

  Output::

     Before: Hello, WORLD! 123
     After: hello, world! 123

  Note:
     These functions work only with ASCII characters. For Unicode characters,
     a more comprehensive character handling library should be used.

replace_char
~~~~~~~~~~~~
.. c:function:: void replace_char(string_t* str, size_t index, char value)

  Replaces the character at a specified index in a ``string_t`` object.

  :param str: ``string_t`` object to modify
  :param index: Zero-based index of the character to replace
  :param value: New character value
  :raises: Sets errno to EINVAL if str is NULL, ERANGE if index is out of bounds

  Example:

  .. code-block:: c

     string_t* str = init_string("hello");
     
     replace_char(str, 0, 'H');
     printf("After capitalizing first letter: %s\n", get_string(str));
     
     replace_char(str, 4, '!');
     printf("After replacing last letter: %s\n", get_string(str));
     
     free_string(str);

  Output::

     After capitalizing first letter: Hello
     After replacing last letter: Hell!

String Whitespace Trimming
~~~~~~~~~~~~~~~~~~~~~~~~~~

trim_leading_whitespace
^^^^^^^^^^^^^^^^^^^^^^^
.. c:function:: void trim_leading_whitespace(string_t* str)

  Removes all whitespace characters (spaces, tabs, newlines) from the beginning of a string.
  The rest of the string, including any internal whitespace, remains unchanged.

  :param str: ``string_t`` object to modify
  :raises: Sets errno to EINVAL if str is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("   hello world  ");
     printf("Before: '%s'\n", get_string(str));
     
     trim_leading_whitespace(str);
     printf("After:  '%s'\n", get_string(str));
     
  Output::

     Before: '   hello world  '
     After:  'hello world  '

trim_trailing_whitespace
^^^^^^^^^^^^^^^^^^^^^^^^
.. c:function:: void trim_trailing_whitespace(string_t* str)

  Removes all whitespace characters (spaces, tabs, newlines) from the end of a string.
  The rest of the string, including any internal whitespace, remains unchanged.

  :param str: ``string_t`` object to modify
  :raises: Sets errno to EINVAL if str is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("hello   world   ");
     printf("Before: '%s'\n", get_string(str));
     
     trim_trailing_whitespace(str);
     printf("After:  '%s'\n", get_string(str));

  Output::

     Before: 'hello   world   '
     After:  'hello   world'

trim_all_whitespace
^^^^^^^^^^^^^^^^^^^
.. c:function:: void trim_all_whitespace(string_t* str)

  Removes all whitespace characters (spaces, tabs, newlines) from throughout the string,
  including leading, trailing, and between words.

  :param str: ``string_t`` object to modify
  :raises: Sets errno to EINVAL if str is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("  hello   world  there  ");
     printf("Before: '%s'\n", get_string(str));
     
     trim_all_whitespace(str);
     printf("After:  '%s'\n", get_string(str));

  Output::

     Before: '  hello   world  there  '
     After:  'helloworldthere'

String Comparison Functions
---------------------------
The functions and Macros in this section are used to compare to strings 
for equivalance or for their differences.

.. _string-lit-compare-func:

compare_strings_lit
~~~~~~~~~~~~~~~~~~~
.. c:function:: int compare_strings_lit(const string_t* str_struct, const char* string)

  Compares a ``string_t`` object with a C string literal lexicographically.
  Developers should consider using the :ref:`compare_strings macro <compare-strings-macro>`
  in place of the ``compare_strings_lit`` function. This is the equivalent of the 
  ``strcmp`` function from the ``string.h`` header file.  However, this function 
  is bounded by the size of the first string and is not subject to buffer overflow 
  attacks.

  :param str_struct: ``string_t`` object to compare
  :param string: C string literal to compare against
  :returns: < 0 if str_struct is less than string, 
           0 if equal, 
           > 0 if str_struct is greater than string,
           INT_MIN on error
  :raises: Sets errno to EINVAL if either input is NULL

  Example:

  .. code-block:: c

     string_t* str = init_string("hello");
     if (str) {
         printf("Comparing '%s' with 'hello': %d\n", 
                get_string(str), compare_strings_lit(str, "hello"));
         printf("Comparing '%s' with 'world': %d\n", 
                get_string(str), compare_strings_lit(str, "world"));
         printf("Comparing '%s' with 'apple': %d\n", 
                get_string(str), compare_strings_lit(str, "apple"));
         free_string(str);
     }

  Output::

     Comparing 'hello' with 'hello': 0
     Comparing 'hello' with 'world': -15
     Comparing 'hello' with 'apple': 7

.. _string-string-compare-func:

compare_strings_string
~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: int compare_strings_string(const string_t* str_struct_one, string_t* str_struct_two)

  Compares two ``string_t`` objects lexicographically. Developers should consider 
  using the :ref:`compare-strings macro <compare-strings-macro>` in place of the 
  ``compare_strings_string`` function. This is hte equivalent of the ``strcmp``
  function from the ``string.h`` header file; however, it is bounded by the length 
  of the strings and is not subject to buffer overflow attacks.

  :param str_struct_one: First ``string_t`` object to compare
  :param str_struct_two: Second ``string_t`` object to compare against
  :returns: < 0 if str_struct_one is less than str_struct_two, 
           0 if equal, 
           > 0 if str_struct_one is greater than str_struct_two,
           INT_MIN on error
  :raises: Sets errno to EINVAL if either input is NULL

  Example:

  .. code-block:: c

     string_t* str1 = init_string("hello");
     string_t* str2 = init_string("world");
     string_t* str3 = init_string("hello");
     
     if (str1 && str2 && str3) {
         printf("Comparing '%s' with '%s': %d\n", 
                get_string(str1), get_string(str2), 
                compare_strings_string(str1, str2));
         printf("Comparing '%s' with '%s': %d\n", 
                get_string(str1), get_string(str3), 
                compare_strings_string(str1, str3));
         
         free_string(str1);
         free_string(str2);
         free_string(str3);
     }

  Output::

     Comparing 'hello' with 'world': -15
     Comparing 'hello' with 'hello': 0

String Utility Functions
------------------------
The functions and Macros in this section offer general utility functions 
for characterizing data within the ``size_t`` data type.

get_char
~~~~~~~~
.. c:function:: char get_char(string_t* str, size_t index)

  Retrieves the character at a specified index in a ``string_t`` object.

  :param str: ``string_t`` object to access
  :param index: Zero-based index of the character to retrieve
  :returns: Character at specified index, or 0 on error
  :raises: Sets errno to EINVAL if str is NULL, ERANGE if index is out of bounds

  Example:

  .. code-block:: c

     string_t* str = init_string("Hello");
     
     char first = get_char(str, 0);
     printf("First character: %c\n", first);
     
     char last = get_char(str, 4);
     printf("Last character: %c\n", last);
     
     free_string(str);

  Output::

     First character: H
     Last character: o

copy_string
~~~~~~~~~~~
.. c:function:: string_t* copy_string(const string_t* str)

  Creates a deep copy of a ``string_t`` object, preserving both content and allocation size.

  :param str: ``string_t`` object to copy
  :returns: New string_t object with identical content, or NULL on failure
  :raises: Sets errno to EINVAL if str is NULL or ENOMEM if allocation fails

  Example:

  .. code-block:: c

     string_t* original = init_string("Hello World");
     if (original) {
         printf("Original: %s\n", get_string(original));
         
         string_t* copy = copy_string(original);
         if (copy) {
             printf("Copy: %s\n", get_string(copy));
             printf("Original size: %zu\n", string_size(original));
             printf("Copy size: %zu\n", string_size(copy));
             printf("Original allocation: %zu\n", string_alloc(original));
             printf("Copy allocation: %zu\n", string_alloc(copy));
             
             free_string(copy);
         }
         free_string(original);
     }

  Output::

     Original: Hello World
     Copy: Hello World
     Original size: 11
     Copy size: 11
     Original allocation: 12
     Copy allocation: 12

first_char_occurance
~~~~~~~~~~~~~~~~~~~~
.. c:function:: inline char* first_char_occurance(string_t* str, char value)

  Finds the first occurrence of a character in a ``string_t`` object.
  Similar to ``strchr()`` from the C standard library.

  :param str: string_t object to search in
  :param value: Character to search for
  :returns: Pointer to the first occurrence of the character, or NULL if not found
  :raises: Sets errno to EINVAL if str is NULL

  Example:

  .. code-block:: c

     string_t* str = init_string("Hello World");
     if (str) {
         char* result = first_char_occurance(str, 'l');
         if (result) {
             printf("First 'l' found at position: %ld\n", result - get_string(str));
             printf("Remaining string from 'l': %s\n", result);
         }
         
         // Try finding a character that doesn't exist
         if (!first_char_occurance(str, 'z')) {
             printf("Character 'z' not found\n");
         }
         
         free_string(str);
     }

  Output::

     First 'l' found at position: 2
     Remaining string from 'l': llo World
     Character 'z' not found

last_char_occurance
~~~~~~~~~~~~~~~~~~~
.. c:function:: inline char* last_char_occurance(string_t* str, char value)

  Finds the last occurrence of a character in a ``string_t`` object.
  Similar to ``strrchr()`` from the C standard library.

  :param str: string_t object to search in
  :param value: Character to search for
  :returns: Pointer to the last occurrence of the character, or NULL if not found
  :raises: Sets errno to EINVAL if str is NULL

Example:

.. code-block:: c

   string_t* str = init_string("Hello World");
   if (str) {
       char* first = first_char_occurance(str, 'l');
       char* last = last_char_occurance(str, 'l');

       // ptrdiff_t can be accessed from the stddef.h header file
       if (first && last) {
           ptrdiff_t first_pos = first - get_string(str);
           ptrdiff_t last_pos = last - get_string(str);
           printf("First 'l' at position: %td\n", first_pos);
           printf("Last 'l' at position: %td\n", last_pos);
           printf("Number of characters between: %td\n", last - first);
       }
       
       free_string(str);
   }

Output::

   First 'l' at position: 2
   Last 'l' at position: 9
   Number of characters between: 7

first_char
~~~~~~~~~~
.. c:function:: char* first_char(string_t* str)

  Returns a pointer to the beginning of the string stored in a ``string_t`` object.
  Similar to begin() in C++ string class.

  :param str: ``string_t`` object to get start pointer from
  :returns: Pointer to the first character of the string, or NULL on error
  :raises: Sets errno to EINVAL if str is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Hello World");
     if (str) {
         char* start = first_char(str);
         if (start) {
             printf("First character: %c\n", *start);
             printf("First two chars: %.2s\n", start);
         }
     }

  Output::

     First character: H
     First two chars: He

last_char
~~~~~~~~~
.. c:function:: char* last_char(string_t* str)

  Returns a pointer to the last character of the string stored in a ``string_t`` object.
  Note: This points to the last actual character, not the null terminator.

  :param str: ``string_t`` object to get end pointer from
  :returns: Pointer to the last character of the string, or NULL on error
  :raises: Sets errno to EINVAL if str is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("Hello World");
     if (str) {
         char* start = first_char(str);
         char* end = last_char(str);
         
         if (start && end) {
             printf("First character: %c\n", *start);
             printf("Last character: %c\n", *end);
             printf("String length: %td\n", end - start + 1);
         }
     }

  Output::

     First character: H
     Last character: d
     String length: 11

.. _first-lit-substr-func:

First Occurance of a Substring 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The functions and Macros in this section offer a utility to search for the 
first occurrence of a sub-string within a ``string_t`` data type.  These functions 
and Macros return the location of the sub-string start point as a ``char`` pointer.

first_lit_substr_occurance
^^^^^^^^^^^^^^^^^^^^^^^^^^
.. c:function:: char* first_lit_substr_occurance(string_t* str, char* sub_str)

  Finds the first occurrence of a C string literal substring within a ``string_t`` object.
  Similar to strstr() from the C standard library.  The user can also implement 
  :ref:`first_substr_occurance <first-substr-macro>` macro in place of this function.

  :param str: ``string_t`` object to search within
  :param sub_str: C string literal to search for
  :returns: Pointer to first character of substring if found, NULL otherwise
  :raises: Sets errno to EINVAL if either input is NULL

  Example:

  .. code-block:: c

     string_t* str = init_string("hello world hello there");
     char* result = first_lit_substr_occurance(str, "world");
     
     if (result) {
         printf("Found 'world' at position: %td\n", result - get_string(str));
         printf("Remaining string: %s\n", result);
     }
     
     free_string(str);

  Output::

     Found 'world' at position: 6
     Remaining string: world hello there

.. _first-str-substr-func:

first_string_substr_occurrence
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. c:function:: char* first_string_substr_occurrence(string_t* str, string_t* sub_str)

  Finds the first occurrence of a ``string_t`` substring within another ``string_t`` object.
  The developer can also implement the :ref:`first_substr_occurance() <first-substr-macro>`
  Macro in place of this function.

  :param str: ``string_t`` object to search within
  :param sub_str: ``string_t`` object containing the substring to search for
  :returns: Pointer to first character of substring if found, NULL otherwise
  :raises: Sets errno to EINVAL if either input is NULL

  Example:

  .. code-block:: c

     string_t* str = init_string("hello world hello there");
     string_t* sub = init_string("world");
     
     char* result = first_string_substr_occurrence(str, sub);
     if (result) {
         printf("Found substring at position: %td\n", result - get_string(str));
     }
     
     free_string(str);
     free_string(sub);

  Output::

     Found substring at position: 6

.. _first-substr-macro:

first_substr_occurrence
^^^^^^^^^^^^^^^^^^^^^^^
.. c:macro:: first_substr_occurance(str1, str2)

  A generic macro that selects the appropriate substring search function based on
  the type of the second argument. Provides a unified interface for substring
  searching regardless of the substring type.  This macro wraps the 
  :ref:`first_lit_substr_occurrence() <first-lit-substr-func>` and 
  :ref:`first_string_substr_occurrence() <first-str-substr-func>` functions

  Example:

  .. code-block:: c

     string_t* str = init_string("hello world hello there");
     string_t* sub = init_string("world");
     
     // Using with string literal
     char* result1 = first_substr_occurrence(str, "hello");
     if (result1) {
         printf("Found 'hello' at position: %td\n", result1 - get_string(str));
     }
     
     // Using with string_t
     char* result2 = first_substr_occurance(str, sub);
     if (result2) {
         printf("Found 'world' at position: %td\n", result2 - get_string(str));
     }
     
     free_string(str);
     free_string(sub);

  Output::

     Found 'hello' at position: 0
     Found 'world' at position: 6

Last Occurance of a Substring 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The functions and Macros in this section offer a utility to search for the 
last occurrence of a sub-string within a ``string_t`` data type.  These functions 
and Macros return the location of the sub-string start point as a ``char`` pointer.

.. _last-lit-substr-func:

last_lit_substr_occurrence
^^^^^^^^^^^^^^^^^^^^^^^^^^
.. c:function:: char* last_lit_substr_occurrence(string_t* str, char* sub_str)

  Finds the last occurrence of a C string literal substring within a ``string_t`` object.
  Similar to strstr() but searches from end to beginning.  Developer can use the 
  generic :ref:`last_substr_occurrence() <last-substr-macro>` macro in place of this function.

  :param str: ``string_t`` object to search within
  :param sub_str: C string literal to search for
  :returns: Pointer to first character of last substring match if found, NULL otherwise
  :raises: Sets errno to EINVAL if either input is NULL

  Example:

  .. code-block:: c

     string_t* str = init_string("hello world hello there");
     char* result = last_lit_substr_occurrence(str, "hello");
     
     if (result) {
         printf("Last 'hello' found at position: %td\n", result - get_string(str));
         printf("Remaining string: %s\n", result);
     }
     
     free_string(str);

  Output::

     Last 'hello' found at position: 12
     Remaining string: hello there

.. _last-string-substr-func:

last_string_substr_occurrence
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. c:function:: char* last_string_substr_occurrence(string_t* str, string_t* sub_str)

  Finds the last occurrence of a ``string_t`` substring within another ``string_t`` object.
  Developer can use the generic :ref:`last_substr_occurrence() <last-substr-macro>` 
  macro in place of this function.

  :param str: ``string_t`` object to search within
  :param sub_str: ``string_t`` object containing the substring to search for
  :returns: Pointer to first character of last substring match if found, NULL otherwise
  :raises: Sets errno to EINVAL if either input is NULL

  Example:

  .. code-block:: c

     string_t* str = init_string("hello world hello there");
     string_t* sub = init_string("hello");
     
     char* result = last_string_substr_occurrence(str, sub);
     if (result) {
         printf("Last occurrence found at position: %td\n", result - get_string(str));
         printf("Text from match onwards: %s\n", result);
     }
     
     free_string(str);
     free_string(sub);

  Output::

     Last occurrence found at position: 12
     Text from match onwards: hello there

.. _last-substr-macro:

last_substr_occurrence
^^^^^^^^^^^^^^^^^^^^^^
.. c:macro:: last_substr_occurrence(str1, str2)

  A generic macro that selects the appropriate substring search function based on
  the type of the second argument. Provides a unified interface for finding the
  last occurrence of a substring regardless of the substring type.  This macro 
  wraps the :ref:`last_lit_substr_occurrence() <last-lit-substr-func>` 
  and :ref:`last_string_substr_occurrence() <last-string-substr-func>` functions.

  Example:

  .. code-block:: c

     string_t* str = init_string("hello world hello there hello");
     string_t* sub = init_string("hello");
     
     // Using with string literal
     char* result1 = last_substr_occurrence(str, "hello");
     if (result1) {
         printf("Last 'hello' using literal at: %td\n", result1 - get_string(str));
     }
     
     // Using with string_t
     char* result2 = last_substr_occurrence(str, sub);
     if (result2) {
         printf("Last 'hello' using string_t at: %td\n", result2 - get_string(str));
     }
     
     free_string(str);
     free_string(sub);

  Output::

     Last 'hello' using literal at: 18
     Last 'hello' using string_t at: 18

is_string_ptr
~~~~~~~~~~~~~
.. c:function:: bool is_string_ptr(string_t* str, char* ptr)

  Determines if a pointer falls within the valid bounds of a ``string_t`` object's string data.
  This is useful for validating pointers returned by string search functions or pointer
  arithmetic operations.

  :param str: ``string_t`` object containing the string data
  :param ptr: Pointer to check
  :returns: true if pointer is within string bounds, false otherwise
  :raises: Sets errno to EINVAL if either input is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("hello world");
     // Get a pointer to the middle of the string
     char* middle = first_char(str) + 5;  // Points to space
     
     if (is_string_ptr(str, middle)) {
         printf("Pointer is within string bounds\n");
         printf("Character at pointer: '%c'\n", *middle);
     }
     
     // Check a pointer outside bounds
     char* beyond = first_char(str) + string_size(str);  // Points to null terminator
     if (!is_string_ptr(str, beyond)) {
         printf("Pointer is outside string bounds\n");
     }
     

  Output::

     Pointer is within string bounds
     Character at pointer: ' '
     Pointer is outside string bounds

.. note:: The valid range includes all characters from the first character up to, but not including, the null terminator. A pointer equal to str->str + str->len (pointing to the null terminator) is considered out of bounds.

Character Case Conversion
~~~~~~~~~~~~~~~~~~~~~~~~~
The following two functions can be used to change the case of a ``char`` value.
These functions can be useful when iterating through a string in cases 
where selective portions of a string must be transformed to a different 
case.

to_upper_char
^^^^^^^^^^^^^
.. c:function:: void to_upper_char(char* val)

  Converts a single character to uppercase if it is a lowercase letter.
  Non-alphabetic characters are left unchanged.

  :param val: Pointer to character to convert
  :raises: Sets errno to EINVAL if val is NULL

  Example:

  .. code-block:: c

     char c = 'a';
     printf("Before: %c\n", c);
     to_upper_char(&c);
     printf("After: %c\n", c);

     char d = '5';  // Non-alphabetic character
     to_upper_char(&d);
     printf("Number remains: %c\n", d);

  Output::

     Before: a
     After: A
     Number remains: 5

to_lower_char
^^^^^^^^^^^^^
.. c:function:: void to_lower_char(char* val)

  Converts a single character to lowercase if it is an uppercase letter.
  Non-alphabetic characters are left unchanged.

  :param val: Pointer to character to convert
  :raises: Sets errno to EINVAL if val is NULL

  Example:

  .. code-block:: c

     char c = 'A';
     printf("Before: %c\n", c);
     to_lower_char(&c);
     printf("After: %c\n", c);

     char d = '!';  // Non-alphabetic character
     to_lower_char(&d);
     printf("Symbol remains: %c\n", d);

  Output::

     Before: A
     After: a
     Symbol remains: !

.. note:: These functions work only with ASCII characters. For Unicode characters, a more comprehensive character handling library should be used.

String Token Operations
-----------------------

pop_string_token
~~~~~~~~~~~~~~~~
.. c:function:: string_t* pop_string_token(string_t* str_struct, char token)

  Splits a string at the rightmost occurrence of a specified token character.
  Returns the portion of the string after the token as a new ``string_t`` object,
  and modifies the original string to contain only the portion before the token.

  :param str_struct: ``string_t`` object to split
  :param token: Character to use as the splitting token
  :returns: New string_t object containing the portion after the token,
           or NULL if token not found or on error
  :raises: Sets errno to EINVAL if str_struct is NULL

  Example:

  .. code-block:: c

     string_t* str = init_string("hello,world,there");
     printf("Original: %s\n", get_string(str));
     
     string_t* right = pop_string_token(str, ',');
     if (right) {
         printf("After first pop:\n");
         printf("Left part: %s\n", get_string(str));
         printf("Popped part: %s\n", get_string(right));
         
         // Pop another token
         string_t* right2 = pop_string_token(str, ',');
         if (right2) {
             printf("\nAfter second pop:\n");
             printf("Left part: %s\n", get_string(str));
             printf("Popped part: %s\n", get_string(right2));
             free_string(right2);
         }
         free_string(right);
     }
     free_string(str);

  Output::

     Original: hello,world,there
     After first pop:
     Left part: hello,world
     Popped part: there

     After second pop:
     Left part: hello
     Popped part: world

.. note:: The original string is modified by this function. If the token is not found, the original string remains unchanged and NULL is returned.

token_count
~~~~~~~~~~~
.. c:function:: size_t token_count(const string_t* str, const char* delim)

  Counts the number of tokens in a ``string_t`` object based on specified delimiter characters.
  Consecutive delimiters are treated as a single delimiter, and leading/trailing
  delimiters are ignored. A common use for this function would be to use white 
  spaces as a method to count the number of words in a text string.

  :param str: ``string_t`` object to analyze
  :param delim: String containing one or more delimiter characters
  :returns: Number of tokens found, or 0 if string is empty or on error
  :raises: Sets errno to EINVAL if str or delim is NULL

  Example:

  .. code-block:: c

     string_t* str STRING_GBC = init_string("hello   world  there");
     size_t count = token_count(str, " ");
     printf("Token count: %zu\n", count);

     string_t* str2 STRING_GBC = init_string("one,two;three,four");
     size_t count2 = token_count(str2, ",;");  // Multiple delimiters
     printf("Token count: %zu\n", count2);
     
     string_t* str3 STRING_GBC = init_string("   extra spaces   ");
     size_t count3 = token_count(str3, " ");
     printf("Token count: %zu\n", count3);

  Output::

     Token count: 3
     Token count: 4
     Token count: 1

.. note:: The function treats any character in the delim string as a delimiter. For example, if delim is ".,", both period and comma will be treated as delimiters. 

tokenize_string
~~~~~~~~~~~~~~~
.. c:function:: string_v* tokenize_string(const string_t* str, const char* delim)

  Splits a string into tokens based on specified delimiter characters.
  Creates a vector of strings containing each token while preserving
  the original string.

  :param str: string_t object to tokenize
  :param delim: String containing delimiter characters (e.g., " ,;")
  :returns: New string vector containing tokens, or NULL on error
  :raises: Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure

  Example:

  .. code-block:: c

     string_t* str = init_string("hello,world;how are you");
     string_v* tokens = tokenize_string(str, " ,;");
     
     if (tokens) {
         printf("Found %zu tokens:\n", str_vector_size(tokens));
         for (size_t i = 0; i < str_vector_size(tokens); i++) {
             printf("%zu: %s\n", i + 1, 
                    get_string(str_vector_index(tokens, i)));
         }
         free_str_vector(tokens);
     }
     
     free_string(str);

  Output::

     Found 5 tokens:
     1: hello
     2: world
     3: how
     4: are
     5: you

  Features:
     * Handles multiple delimiter characters
     * Skips consecutive delimiters
     * Preserves original string
     * Memory efficient - no string duplication
     * Proper error handling

  Example with multiple delimiters:

  .. code-block:: c

     string_t* str = init_string("name:John,age:30;city:New York");
     string_v* tokens = tokenize_string(str, ":,;");
     
     if (tokens) {
         for (size_t i = 0; i < str_vector_size(tokens); i += 2) {
             printf("%s = %s\n", 
                    get_string(str_vector_index(tokens, i)),
                    get_string(str_vector_index(tokens, i + 1)));
         }
         free_str_vector(tokens);
     }
     
     free_string(str);

  Output::

     name = John
     age = 30
     city = New York

  Note:
     Empty strings and strings containing only delimiters will result
     in an empty vector (size 0).

count_words
~~~~~~~~~~~
.. c:function:: dict_t* count_words(const string_t* str, const char* delim)

  Creates a dictionary containing word frequency counts from a string. Each word 
  becomes a key in the dictionary with its frequency as the value.

  :param str: string_t object to analyze
  :param delim: String containing delimiter characters (e.g., " ,.;")
  :returns: New dictionary containing word counts, or NULL on error
  :raises: Sets errno to EINVAL for NULL inputs or empty string, ENOMEM for allocation failure

  Example:

  .. code-block:: c

     string_t* text = init_string("hello world hello test world");
     dict_t* counts = count_words(text, " ");
     
     if (counts) {
         // Print word frequencies
         string_v* words = get_dict_keys(counts);
         for (size_t i = 0; i < str_vector_size(words); i++) {
             const char* word = get_string(str_vector_index(words, i));
             size_t frequency = get_dict_value(counts, word);
             printf("%s: %zu\n", word, frequency);
         }
         
         free_str_vector(words);
         free_dict(counts);
     }
     
     free_string(text);

  Output::

     hello: 2
     world: 2
     test: 1

  Example with multiple delimiters:

  .. code-block:: c

     string_t* text = init_string("hello,world;hello.test");
     dict_t* counts = count_words(text, ",.;");
     
     // Get total word count
     size_t total = 0;
     string_v* words = get_dict_keys(counts);
     for (size_t i = 0; i < str_vector_size(words); i++) {
         total += get_dict_value(counts, get_string(str_vector_index(words, i)));
     }
     
     printf("Total words: %zu\n", total);
     
     free_str_vector(words);
     free_dict(counts);
     free_string(text);

  Output::

     Total words: 4

.. note::

     - Word matching is case-sensitive ("Hello" and "hello" are counted separately)
     - Empty strings or strings containing only delimiters return empty dictionaries
     - Multiple consecutive delimiters are treated as a single delimiter
     - The returned dictionary must be freed using free_dict()

String Swapping
---------------

swap_string
~~~~~~~~~~~
.. c:function:: void swap_string(string_t* a, string_t* b)

   Swaps the contents of two string_t objects in place. The operation exchanges
   all internal data including string content, length, and allocation information.

   :param a: Pointer to first string_t object
   :param b: Pointer to second string_t object
   :raises: Sets errno to EINVAL if either input is NULL

   Example:

   .. code-block:: c

      string_t* str1 STRING_GBC = init_string("hello");
      string_t* str2 STRING_GBC = init_string("world");
      
      printf("Before swap:\n");
      printf("str1: %s\n", get_string(str1));
      printf("str2: %s\n", get_string(str2));
      
      swap_string(str1, str2);
      
      printf("\nAfter swap:\n");
      printf("str1: %s\n", get_string(str1));
      printf("str2: %s\n", get_string(str2));

   Output::

      Before swap:
      str1: hello
      str2: world

      After swap:
      str1: world
      str2: hello

   Note:
      The swap operation is efficient as it only exchanges pointers and metadata
      without copying the actual string contents. Memory ownership is preserved,
      allowing the strings to be properly freed after swapping.

String Iterator
---------------

The string iterator provides direct character-level access to a string_t object. While the c_string
library generally encourages using its provided functions for string manipulation, the iterator
offers more granular control when needed.

.. warning::
  Direct string manipulation through the iterator should be used with caution. Modifying string
  contents directly (especially inserting null terminators or changing string length) without
  updating the string_t metadata can lead to undefined behavior.

str_iter
~~~~~~~~
.. c:type:: str_iter

  Structure containing function pointers for string iteration.

  .. c:member:: char* (*begin)(string_t* s)
  
     Returns pointer to the first character of the string

  .. c:member:: char* (*end)(string_t* s)
  
     Returns pointer to position after the last character

  .. c:member:: void (*next)(char** current)
  
     Advances iterator to next character

  .. c:member:: void (*prev)(char** current)
  
     Moves iterator to previous character

  .. c:member:: char (*get)(char** current)
  
     Returns character at current iterator position

init_str_iter
~~~~~~~~~~~~~
.. c:function:: str_iter init_str_iter(void)

  Initializes and returns a string iterator object.

Example:

.. code-block:: c

  string_t* str = init_string("Hello");
  str_iter iter = init_str_iter();
  
  // Forward iteration
  char* current = iter.begin(str);
  char* end = iter.end(str);
  
  while (current != end) {
      printf("%c", iter.get(&current));
      iter.next(&current);
  }
  printf("\n");
  
  // Reverse iteration
  current = iter.end(str) - 1;  // Start from last character
  char* begin = iter.begin(str);
  
  while (current >= begin) {
      printf("%c", iter.get(&current));
      iter.prev(&current);
  }
  printf("\n");
  
  free_string(str);

Output::

  Hello
  olleH

Safe Usage Guidelines
~~~~~~~~~~~~~~~~~~~~~
* Do not insert null terminators within the string
* Do not modify string length without updating the string_t length field
* Stay within the string bounds (begin to end)
* Check for NULL return values from begin() and end()
* Use the provided string manipulation functions when possible

String Vector Overview
=======================

A string vector provides a dynamic array of string_t objects, offering efficient management of
multiple strings with automatic memory handling and resizing capabilities. This implementation
combines the flexibility of dynamic arrays with the safety and convenience of the string_t type.

Key Features
------------

* Dynamic resizing: Automatically grows to accommodate new strings
* Memory safety: Proper encapsulation and memory management
* String type integration: Direct compatibility with string_t objects
* Efficient access: O(1) access to any string in the vector
* Automatic cleanup: Optional garbage collection support with STRVEC_GBC

When to Use String Vectors
--------------------------

String vectors are particularly useful when:

* Managing collections of strings that may grow or shrink
* Working with an unknown number of strings at compile time
* Needing sequential access to multiple strings
* Requiring efficient random access to strings by index
* Managing related strings as a single unit
* Implementing string-based data structures like command histories or word lists

Performance Characteristics
---------------------------

* Access time: O(1) for index-based access
* Insertion time: O(1) amortized for push_back operations
* Memory efficiency: Grows geometrically to minimize reallocations
* Memory overhead: Minimal per-string overhead beyond string content

Data Types 
==========
The following are derived data types used in the implementation for a string 
vector.

string_v
--------
The dynamic string vector type that manages memory allocation automatically and 
provides a safe encapsulation for ``string_t`` data types in an array format.
This struct is provided as an opaque data structure.

.. code-block:: c

   typedef struct {
       string_t* data;
       size_t len;
       size_t alloc;
   } string_v;

string_t
--------
A dynamic string type that manages memory allocation automatically and provides safe string operations.
The internal structure is opaque to users, preventing direct manipulation of the memory.

While the data structure is opaque to the user, the struct has this 
format

.. code-block:: c

   typedef struct {
       char* str;
       size_t len;
       size_t alloc;
    } string_t;

Core Functions
==============
The c_string library provides a dynamically allocated string vector of ``string_t``
data types.  All of the functions described in this section can be found in the 
``c_string.h`` header file.

Initialization and Memory Management
------------------------------------
The functions and Macros in this section are used to control the creation,
memory allocation, and specific destruction of ``string_v`` data types.

The functions and Macros in this section are core to the creation and destruction of
``string_v`` data types.

.. warning::

   The ``string_v`` methods encapsulates ``string_t`` data types into a dynamically 
   allocated vector.  The user should take care not to free data within the 
   vector with the :ref:`free_string() <free-string-func>` function or the 
   :ref:`STRING_GBC <string_cleanup_macro>` macro to avoid dangling pointers

init_str_vector
~~~~~~~~~~~~~~~
.. c:function:: string_v* init_str_vector(size_t buffer)

  Initializes a new string vector with specified initial capacity.
  The vector will automatically grow if needed when adding elements.

  :param buffer: Initial capacity (number of strings) to allocate
  :returns: Pointer to new ``string_v`` object, or NULL on allocation failure
  :raises: Sets errno to ENOMEM if memory allocation fails

  Example:

  .. code-block:: c

     // Create vector with initial capacity for 10 strings
     string_v* vec = init_str_vector(10);
     if (!vec) {
         fprintf(stderr, "Failed to initialize vector\n");
         return 1;
     }

     // Use the vector
     push_back_str_vector(vec, "hello");
     
     // Free when done
     free_str_vector(vec);

free_str_vector
~~~~~~~~~~~~~~~
.. c:function:: void free_str_vector(string_v* vec)

  Frees all memory associated with a string vector, including all contained
  strings. After calling, the vector pointer should not be used.  If using a
  ``gcc`` or ``clang`` compiler, the developer should consider using the 
  :ref:`STRVEC_GBC <stringv_cleanup_macro>` Macro in place of the ``free_str_vector`` 
  function.

  :param vec: String vector to free
  :raises: Sets errno to EINVAL if vec is NULL

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(5);
     push_back_str_vector(vec, "hello");
     push_back_str_vector(vec, "world");
     
     // Free the vector and all its strings
     free_str_vector(vec);
     vec = NULL;  // Good practice to avoid dangling pointer

String Data Addition
--------------------
The following functions provide methods for adding string data to a ``string_v``
data type.

push_back_str_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool push_back_str_vector(string_v* vec, const char* str)

  Adds a string to the end of the vector. If needed, the vector automatically
  resizes to accommodate the new string. For vectors smaller than VEC_THRESHOLD,
  capacity doubles when full. For larger vectors, a fixed amount is added.
  This is the most efficient method for adding data to a string vector with 
  a time efficiency of :math:`O(1)`.

  :param vec: Target string vector
  :param str: String to add to vector
  :returns: true if successful, false on error
  :raises: Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(2);
     
     // Add some strings
     push_back_str_vector(vec, "first");
     push_back_str_vector(vec, "second");
     
     // Vector will automatically resize
     push_back_str_vector(vec, "third");
     
     printf("Vector size: %zu\n", str_vector_size(vec));
     
     free_str_vector(vec);

  Output::

     Vector size: 3

  This method can also be used indirectly with existing ``string_t`` data types as shown 
  by the example below.

  .. code-block:: c

     string_v* vec = init_str_vector(2);
     
     // Create Strings 
     string_t* str1 STRING_GBC = init_string("first");
     string_t* str2 STRING_GBC = init_string("second");
     // Add some strings
     push_back_str_vector(vec, get_string(str1));
     push_back_str_vector(vec, get_string(str2));
     
     // Vector will automatically resize
     push_back_str_vector(vec, "third");
     
     printf("Vector size: %zu\n", str_vector_size(vec));
     
     free_str_vector(vec);

    Output::

     Vector size: 3

  The folllowing should be considered when using this function

  * If reallocation fails, the original vector remains unchanged
  * If string allocation fails after moving elements, the vector is restored to its original state
  * The operation requires enough contiguous memory for the entire resized array

push_front_str_vector
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool push_front_str_vector(string_v* vec, const char* value)

  Adds a string to the beginning of the vector, shifting all existing elements
  to the right. Automatically resizes the vector if needed.
  This is the least efficient method for adding data to a string vector with 
  a time efficiency of :math:`O(n)`. 

  :param vec: Target string vector
  :param value: String to add at front
  :returns: true if successful, false on error
  :raises: Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure

  Example:

  .. code-block:: c

     string_v* vec STRVEC_GBC = init_str_vector(2);
     
     // Add "world" at the back
     push_back_str_vector(vec, "world");
     
     // Add "hello" at the front
     if (push_front_str_vector(vec, "hello")) {
         // Print all strings
         for (size_t i = 0; i < str_vector_size(vec); i++) {
             printf("%s ", get_string(str_vector_index(vec, i)));
         }
         printf("\n");
     }
     
  Output::

     hello world

  This method can also work indirectly with existing ``string_t`` data types 
  as shown below.

  .. code-block:: c

     string_v* vec STRVEC_GBC = init_str_vector(2);
     
     // Add "world" at the back
     push_back_str_vector(vec, "world");

     string_t* str STRING_GBC = init_string("hello");

     // Add "hello" at the front
     if (push_front_str_vector(vec, get_string(str))) {
         // Print all strings
         for (size_t i = 0; i < str_vector_size(vec); i++) {
             printf("%s ", get_string(str_vector_index(vec, i)));
         }
         printf("\n");
     }

  Output::

     hello world

  The folllowing should be considered when using this function

  * If reallocation fails, the original vector remains unchanged
  * If string allocation fails after moving elements, the vector is restored to its original state
  * The operation requires enough contiguous memory for the entire resized array

  .. note::

     When resizing is needed, the vector grows either by doubling (when size < VEC_THRESHOLD)
     or by adding a fixed amount (when size >= VEC_THRESHOLD). This provides efficient
     amortized performance for both small and large vectors.

insert_str_vector
~~~~~~~~~~~~~~~~~
.. c:function:: bool insert_str_vector(string_v* vec, const char* str, size_t index)

  Inserts a string at any valid position in the vector, shifting subsequent
  elements to the right. Automatically resizes the vector if needed.
  The time complexity of this function can range from :math:`O(1)` to
  :math:`O(n)` depending on where the data is inserted.

  :param vec: Target string vector
  :param str: String to insert
  :param index: Position at which to insert (0 to vec->len)
  :returns: true if successful, false on error
  :raises: Sets errno to EINVAL for NULL inputs, ERANGE for invalid index,
          or ENOMEM on allocation failure

  Example:

  .. code-block:: c

     string_v* vec STRVEC_GBC = init_str_vector(3);
     
     // Create initial vector
     push_back_str_vector(vec, "first");
     push_back_str_vector(vec, "third");
     
     // Insert "second" between them
     if (insert_str_vector(vec, "second", 1)) {
         // Print all strings
         for (size_t i = 0; i < str_vector_size(vec); i++) {
             printf("%s ", get_string(str_vector_index(vec, i)));
         }
         printf("\n");
     }

  Output::

     first second third

  The folllowing should be considered when using this function

  * If reallocation fails, the original vector remains unchanged
  * If string allocation fails after moving elements, the vector is restored to its original state
  * The operation requires enough contiguous memory for the entire resized array

  .. note::

     When resizing is needed, the vector grows either by doubling (when size < VEC_THRESHOLD)
     or by adding a fixed amount (when size >= VEC_THRESHOLD). This provides efficient
     amortized performance for both small and large vectors.

Drop String Functions
---------------------
The string vector provides three different methods for removing elements, each with
different performance characteristics.

pop_back_str_vector
~~~~~~~~~~~~~~~~~~~
.. c:function:: string_t* pop_back_str_vector(string_v* vec)

  Removes and returns the last element from the vector. This is the most efficient
  removal operation as it requires no element shifting.

  :param vec: Target string vector
  :returns: Pointer to removed string_t object, or NULL on error
  :raises: Sets errno to EINVAL for NULL input or empty vector
  :time complexity: O(1) - Constant time operation

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(2);
     push_back_str_vector(vec, "first");
     push_back_str_vector(vec, "second");
     
     string_t* popped = pop_back_str_vector(vec);
     if (popped) {
         printf("Popped string: %s\n", get_string(popped));
         printf("Remaining size: %zu\n", str_vector_size(vec));
         free_string(popped);
     }
     
     free_str_vector(vec);

  Output::

     Popped string: second
     Remaining size: 1

pop_front_str_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: string_t* pop_front_str_vector(string_v* vec)

  Removes and returns the first element from the vector. Requires shifting all
  remaining elements left by one position.

  :param vec: Target string vector
  :returns: Pointer to removed string_t object, or NULL on error
  :raises: Sets errno to EINVAL for NULL input or empty vector
  :time complexity: O(n) - Linear time based on vector size

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(2);
     push_back_str_vector(vec, "first");
     push_back_str_vector(vec, "second");
     
     string_t* popped = pop_front_str_vector(vec);
     if (popped) {
         printf("Popped string: %s\n", get_string(popped));
         printf("New first element: %s\n", 
                get_string(str_vector_index(vec, 0)));
         free_string(popped);
     }
     
     free_str_vector(vec);

  Output::

     Popped string: first
     New first element: second

pop_any_str_vector
~~~~~~~~~~~~~~~~~~
.. c:function:: string_t* pop_any_str_vector(string_v* vec, size_t index)

  Removes and returns the element at the specified index. Performance varies based
  on the removal position - removing from the end is fast, while removing from
  the start or middle requires shifting elements.

  :param vec: Target string vector
  :param index: Position of element to remove
  :returns: Pointer to removed string_t object, or NULL on error
  :raises: Sets errno to EINVAL for NULL input or empty vector, ERANGE for invalid index
  :time complexity: O(1) to O(n) depending on index position

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(3);
     push_back_str_vector(vec, "first");
     push_back_str_vector(vec, "second");
     push_back_str_vector(vec, "third");
     
     // Remove middle element
     string_t* popped = pop_any_str_vector(vec, 1);
     if (popped) {
         printf("Popped string: %s\n", get_string(popped));
         printf("Remaining strings: %s, %s\n",
                get_string(str_vector_index(vec, 0)),
                get_string(str_vector_index(vec, 1)));
         free_string(popped);
     }
     
     free_str_vector(vec);

  Output::

     Popped string: second
     Remaining strings: first, third

.. note::

  All pop operations maintain vector consistency and properly manage memory of
  removed elements. The caller is responsible for freeing the returned string_t
  object using free_string().

delete_back_str_vector
~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool delete_back_str_vector(string_v* vec)

  Removes and frees the last element in a string vector.

  :param vec: Target string vector
  :returns: true if successful, false if vector empty or invalid
  :raises: Sets errno to EINVAL if vec is NULL or empty

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(2);
     push_back_str_vector(vec, "first");
     push_back_str_vector(vec, "second");
     
     if (delete_back_str_vector(vec)) {
         printf("Size after delete: %zu\n", str_vector_size(vec));
     }
     
     free_str_vector(vec);

  Output::

     Size after delete: 1

delete_front_str_vector
~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool delete_front_str_vector(string_v* vec)

  Removes and frees the first element in a string vector, shifting remaining
  elements left.

  :param vec: Target string vector
  :returns: true if successful, false if vector empty or invalid
  :raises: Sets errno to EINVAL if vec is NULL or empty

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(2);
     push_back_str_vector(vec, "first");
     push_back_str_vector(vec, "second");
     
     if (delete_front_str_vector(vec)) {
         printf("Remaining element: %s\n", 
                get_string(str_vector_index(vec, 0)));
     }
     
     free_str_vector(vec);

  Output::

     Remaining element: second

delete_any_str_vector
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool delete_any_str_vector(string_v* vec, size_t index)

  Removes and frees the element at the specified index, shifting subsequent
  elements left.

  :param vec: Target string vector
  :param index: Position of element to delete
  :returns: true if successful, false if index invalid or vector empty
  :raises: Sets errno to EINVAL if vec is NULL or empty, ERANGE if index out of bounds

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(3);
     push_back_str_vector(vec, "one");
     push_back_str_vector(vec, "two");
     push_back_str_vector(vec, "three");
     
     if (delete_any_str_vector(vec, 1)) {  // Delete "two"
         printf("Remaining elements: ");
         for (size_t i = 0; i < str_vector_size(vec); i++) {
             printf("%s ", get_string(str_vector_index(vec, i)));
         }
         printf("\n");
     }
     
     free_str_vector(vec);

  Output::

     Remaining elements: one three

Note:
  These functions free the memory associated with the deleted elements,
  making them simpler to use than their "pop" counterparts when you don't
  need the removed values. For cases where you need the removed value,
  use the corresponding pop functions instead.

Automatic Cleanup 
-----------------
In general the C language does not allow automated garbage collection of 
memory that is out of scope.  This section describes a poor mans 
garbage collection within the C language, for the ``string_v`` data type,
that can only be enabled if compiled with ``gcc`` or ``clang``.

.. _stringv_cleanup_macro:

STRVEC_GBC
~~~~~~~~~~

.. c:macro:: STRVEC_GBC

  Macro that enables automatic cleanup of string vectors when they go out of
  scope. Only available when using GCC or Clang compilers. Uses the cleanup
  attribute to automatically call _free_str_vector.

  Example:

  .. code-block:: c

     void process_strings(void) {
         // Vector will be automatically freed when function returns
         STRVEC_GBC string_v* vec = init_str_vector(10);
         
         push_back_str_vector(vec, "hello");
         push_back_str_vector(vec, "world");
         
         // No need to call free_str_vector
     }  // vec is 

Utility Functions
=================
The following functions can be used to retrieve basic information from 
a ``string_v`` data type.

Vector Size and Access Functions 
--------------------------------
The following functions are used to determine the allocated size and populated 
length of a ``string_v`` data type/

.. _str-vector-index-func:

str_vector_index
~~~~~~~~~~~~~~~~
.. c:function:: const string_t* str_vector_index(const string_v* vec, size_t index)

  Retrieves a pointer to the string_t object at the specified index in the vector.
  Provides safe, bounds-checked access to vector elements.

  :param vec: Source string vector
  :param index: Zero-based index of desired element
  :returns: Pointer to string_t at specified index, or NULL on error
  :raises: Sets errno to EINVAL for NULL input, ERANGE for out-of-bounds index

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(2);
     push_back_str_vector(vec, "hello");
     push_back_str_vector(vec, "world");
     
     const string_t* str = str_vector_index(vec, 1);
     if (str) {
         printf("Second string: %s\n", get_string(str));
     }
     
     free_str_vector(vec);

  Output::

     Second string: world

.. _str-vector-size-func:

str_vector_size
~~~~~~~~~~~~~~~
.. c:function:: const size_t str_vector_size(const string_v* vec)

  Returns the current number of elements in the vector. This represents the
  actual number of strings stored, not the allocated capacity.

  :param vec: String vector to query
  :returns: Number of elements in vector, or LONG_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(5);  // Capacity of 5
     push_back_str_vector(vec, "first");
     push_back_str_vector(vec, "second");
     
     printf("Vector size: %zu\n", str_vector_size(vec));
     
     free_str_vector(vec);

  Output::

     Vector size: 2

The developer may also consider the safe use of the :ref:`s_alloc <s-size-macro>`
Macro in place of the ``str_vector_size`` function.

.. _str-vector-alloc-func:

str_vector_alloc
~~~~~~~~~~~~~~~~
.. c:function:: const size_t str_vector_alloc(const string_v* vec)

  Returns the current allocation size (capacity) of the vector. This represents
  the number of elements that can be stored without requiring reallocation.

  :param vec: String vector to query
  :returns: Current allocation size, or LONG_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(5);
     
     printf("Initial capacity: %zu\n", str_vector_alloc(vec));
     
     // Add strings until reallocation occurs
     for(int i = 0; i < 6; i++) {
         push_back_str_vector(vec, "test");
         printf("After push %d - Size: %zu, Capacity: %zu\n",
                i + 1, str_vector_size(vec), str_vector_alloc(vec));
     }
     
     free_str_vector(vec);

  Output::

     Initial capacity: 5
     After push 1 - Size: 1, Capacity: 5
     After push 2 - Size: 2, Capacity: 5
     After push 3 - Size: 3, Capacity: 5
     After push 4 - Size: 4, Capacity: 5
     After push 5 - Size: 5, Capacity: 5
     After push 6 - Size: 6, Capacity: 10

The developer may also consider the safe use of the :ref:`s_alloc <s-alloc-macro>`
Macro in place of the ``str_vector_alloc`` function.

.. note::

  These functions provide the basic mechanisms for inspecting a vector's state
  and accessing its contents. They are fundamental to safe vector manipulation
  and are used extensively by other vector operations.

Vector Sorting Algorithm 
------------------------

sort_str_vector
~~~~~~~~~~~~~~~
.. c:function:: void sort_str_vector(string_v* vec, iter_dir direction)

  Sorts a string vector in either ascending (FORWARD) or descending (REVERSE) order
  using an optimized QuickSort algorithm with median-of-three pivot selection and
  insertion sort for small subarrays.

  :param vec: String vector to sort
  :param direction: FORWARD for ascending, REVERSE for descending order
  :raises: Sets errno to EINVAL if vec is NULL

  Example:

  .. code-block:: c

     string_v* vec STRVEC_GBC = init_str_vector(3);
     push_back_str_vector(vec, "banana");
     push_back_str_vector(vec, "apple");
     push_back_str_vector(vec, "cherry");
     
     // Sort in ascending order
     sort_str_vector(vec, FORWARD);
     for (size_t i = 0; i < str_vector_size(vec); i++) {
         printf("%s\n", get_string(str_vector_index(vec, i)));
     }
     
     // Sort in descending order
     sort_str_vector(vec, REVERSE);
     for (size_t i = 0; i < str_vector_size(vec); i++) {
         printf("%s\n", get_string(str_vector_index(vec, i)));
     }
     

  Output::

     apple
     banana
     cherry

     cherry
     banana
     apple

  Performance Characteristics
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^
  * Time Complexity: :math:`O(n log (n))` average case
  * Space Complexity: :math:`O(log (n))` for recursion stack
  * Optimizations:
     - Median-of-three pivot selection for better performance on sorted/nearly sorted data
     - Insertion sort for small subarrays (less than 10 elements)
     - Tail recursion elimination to reduce stack usage

  .. note::

     The sort is stable for equal elements, and special handling is provided for
     empty strings and vectors with duplicate elements.

Vector Reversal
---------------

reverse_str_vector
~~~~~~~~~~~~~~~~~~
.. c:function:: void reverse_str_vector(string_v* vec)

   Reverses the order of elements in a string vector. The operation is performed
   in place without allocating additional memory.

   :param vec: string vector to reverse
   :raises: Sets errno to EINVAL if vec is NULL or invalid

   Example:

   .. code-block:: c

      string_v* vec STRVEC_GBC = init_str_vector(3);
      push_back_str_vector(vec, "first");
      push_back_str_vector(vec, "second");
      push_back_str_vector(vec, "third");
      
      printf("Before: ");
      for (size_t i = 0; i < str_vector_size(vec); i++) {
          printf("%s ", get_string(str_vector_index(vec, i)));
      }
      printf("\n");
      
      reverse_str_vector(vec);
      
      printf("After: ");
      for (size_t i = 0; i < str_vector_size(vec); i++) {
          printf("%s ", get_string(str_vector_index(vec, i)));
      }
      printf("\n");
      
   Output::

      Before: first second third
      After: third second first

.. note::

      The reversal is performed in place, making it memory efficient for
      large vectors. Empty vectors and single-element vectors remain unchanged.

Binary Search Operations
------------------------

binary_search_str_vector
^^^^^^^^^^^^^^^^^^^^^^^^
.. c:function:: size_t binary_search_str_vector(string_v* vec, char* value, bool sort_first)

  Performs a binary search on a string vector to find a specific value.
  Can optionally sort the vector before searching.

  :param vec: String vector to search
  :param value: String value to find
  :param sort_first: true to sort vector before searching, false if already sorted
  :returns: Index where value was found, or LONG_MAX if not found
  :raises: Sets errno to EINVAL if inputs invalid, ENODATA if vector empty

  Example:

  .. code-block:: c

     string_v* vec = init_str_vector(3);
     push_back_str_vector(vec, "apple");
     push_back_str_vector(vec, "banana");
     push_back_str_vector(vec, "cherry");
     
     // Search in pre-sorted vector
     size_t index = binary_search_str_vector(vec, "banana", false);
     if (index != LONG_MAX) {
         printf("Found 'banana' at index: %zu\n", index);
     }
     
     // Search with automatic sorting
     push_back_str_vector(vec, "date");
     push_back_str_vector(vec, "apple");  // Second "apple"
     
     index = binary_search_str_vector(vec, "date", true);
     if (index != LONG_MAX) {
         printf("Found 'date' at index: %zu\n", index);
     }
     
     free_str_vector(vec);

  Output::

     Found 'banana' at index: 1
     Found 'date' at index: 3

  Note:
     - Vector must be sorted in ascending order for binary search to work correctly
     - If sort_first is false, the function assumes the vector is already sorted
     - For unsorted vectors, always set sort_first to true
     - Returns first occurrence if value appears multiple times

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

String Operation Macros
-----------------------
The following macros can be used to manipulate string data.

.. _string-concat-macro:

string_concat
~~~~~~~~~~~~~
A type-safe macro for string concatenation that automatically selects the appropriate
function based on the type of the second argument:

- For char* arguments: Calls string_lit_concat
- For string_t* arguments: Calls string_string_concat

This eliminates the need for developers to manually choose between different concatenation
functions while ensuring type safety. This Macro may be safely used in place 
of the :ref:`string_string_concat() <string-string-concat-func>`
and :ref:`string_list_concat() <string-lit-concat-func>`

Example:

.. code-block:: c

  string_t* str = init_string("Hello ");
  
  // Using with string literal
  string_concat(str, "world");  // Uses string_lit_concat
  
  // Using with string_t
  string_t* other = init_string(" again");
  string_concat(str, other);    // Uses string_string_concat

.. _compare-strings-macro:

string_compare
~~~~~~~~~~~~~~
A type-safe comparison macro that selects the appropriate comparison function based on
the second argument's type:

- For char* arguments: Calls compare_strings_lit
- For string_t* arguments: Calls compare_strings_string

Provides a uniform interface for string comparison while maintaining type safety and
optimal performance. This Macro may be safely used in place of the
:ref:`compare_strings_string() <string-string-compare-func>` and
:ref:`compare_strings_lit() <string-lit-compare-func>` functions.

Example:

.. code-block:: c

  string_t* str = init_string("hello");
  
  // Compare with literal
  if (string_compare(str, "hello") == 0) {
      printf("Strings are equal\n");
  }
  
  // Compare with string_t
  string_t* other = init_string("world");
  if (string_compare(str, other) < 0) {
      printf("'hello' comes before 'world'\n");
  }

Size Inspection Macros
----------------------
These macros provide several advantages:

- **Consistency**: Same syntax for both string types
- **Type Safety**: Compiler ensures correct function selection
- **Code Clarity**: Eliminates need to remember type-specific function names
- **Maintainability**: Changes to underlying functions only need updates in one place

.. _s-size-macro:

s_size
~~~~~~
A generic macro that provides a uniform interface for checking the current populated
size of both string_t and string_v objects. It automatically selects:

- For string_t*: Uses string_size to get string length
- For string_v*: Uses str_vector_size to get number of strings

This macro simplifies size queries by providing a consistent interface regardless of
the underlying type. This Macro may be safely used in place of the 
:ref:`string_size() <string-size-func>`, and :ref:`str_vector_size() <str-vector-size-func>`

Example:

.. code-block:: c

  string_t* str = init_string("hello world");
  size_t str_len = s_size(str);       // Returns 11
  
  string_v* vec = init_str_vector(2);
  push_back_str_vector(vec, "first");
  push_back_str_vector(vec, "second");
  size_t vec_len = s_size(vec);       // Returns 2

.. _s-alloc-macro:

s_alloc
~~~~~~~
A generic macro that provides a uniform interface for checking the current allocation
size of both string_t and string_v objects. It selects:

- For string_t*: Uses string_alloc to get allocated buffer size
- For string_v*: Uses str_vector_alloc to get allocated vector capacity

This macro is particularly useful for capacity planning and debugging memory usage.
This Macro may be safely used in place of the :ref:`string_alloc <string-alloc-func>`,
and :ref:`str-vector-alloc() <str-vector-alloc-func>`

Example:

.. code-block:: c

  string_t* str = init_string("test");
  size_t str_capacity = s_alloc(str);    // Returns 5 (includes null terminator)
  
  string_v* vec = init_str_vector(10);
  size_t vec_capacity = s_alloc(vec);    // Returns 10

Example combining both macros:

.. code-block:: c

  void print_metrics(void* data, const char* type) {
      if (strcmp(type, "string") == 0) {
          string_t* str = (string_t*)data;
          printf("String length: %zu, allocated: %zu\n",
                 s_size(str), s_alloc(str));
      } else if (strcmp(type, "vector") == 0) {
          string_v* vec = (string_v*)data;
          printf("Vector size: %zu, capacity: %zu\n",
                 s_size(vec), s_alloc(vec));
      }
  }
