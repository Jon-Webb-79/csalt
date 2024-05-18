.. _unordered_hash_map:

********************
Unordered Hash Map
********************

The ``hash.h`` header file provides an implementation of an unordered hash map 
data structure in C. An unordered hash map, also known as a hash table, is a 
data structure that maps keys to values for highly efficient lookup. The 
efficiency comes from the use of a hash function that computes an index into 
an array of buckets or slots, from which the desired value can be found.

Unordered hash maps are particularly useful for:

- **Efficient Lookup Operations**: Hash maps provide average O(1) time complexity for search, insert, and delete operations, making them ideal for applications that require fast access to data.

- **Dynamic Data Set Management**: They dynamically resize to maintain efficient operations as the number of key-value pairs grows, ensuring that the load factor (the ratio of the number of elements to the number of buckets) stays within a desired range.

- **Flexible Key-Value Storage**: Hash maps can store associations between various types of keys and values, providing flexibility in managing different data types.

- **Fast Membership Testing**: Hash maps allow for quick membership testing, which is particularly useful in scenarios where you need to check the existence of elements frequently.

The ``hash.h`` library offers functions to create, manage, and interact with hash 
maps, providing developers with a powerful tool for efficient data management. 
Key characteristics of an unordered hash map include:

- **Hash Function**: A well-designed hash function that distributes keys uniformly across the buckets to minimize collisions and ensure efficient operations.

- **Collision Resolution**: Techniques such as chaining (where each bucket points to a linked list of entries that map to the same bucket) to handle hash collisions and maintain efficient performance.

- **Dynamic Resizing**: The hash map dynamically resizes when the load factor exceeds a predefined threshold, reallocating and rehashing all entries to maintain efficiency.

- **Generic Data Types**: Through the use of macros, the library supports hash maps for various data types, allowing for flexible and type-safe operations.

Structures
==========
The ``hash.h`` header file implements several structs to organize information 
that enables the unordered hash map. The primary data structures are defined 
using macros to facilitate the creation of type-specific hash maps.

.. role:: c(code)
   :language: c

.. code-block:: c

   #define DEFINE_NODE_TYPE(TYPE, TYPENAME)        \
       typedef struct TYPENAME##Node {             \
           char* key;                              \
           TYPE value;                             \
           struct TYPENAME##Node* next;            \
       } TYPENAME##Node;

   #define DEFINE_HASH_TABLE_TYPE(TYPENAME)        \
       typedef struct {                            \
           TYPENAME##Node* keyValues;              \
           size_t hash_size;                       \
           size_t size;                            \
           size_t alloc;                           \
       } TYPENAME##HashTable;

Parameters
----------

- :c:`TYPE`: The data type that the struct will contain.
- :c:`TYPENAME`: The typedef name given to the struct.

Attributes
----------

- :c:`key`: A dynamically allocated string that serves as the key.
- :c:`value`: The value associated with the key of data type ``TYPE``.
- :c:`next`: A pointer to the next node in the chain for collision resolution.
- :c:`keyValues`: A pointer to the array of buckets.
- :c:`hash_size`: The number of key-value pairs in the hash table.
- :c:`size`: The number of populated buckets in the hash table.
- :c:`alloc`: The total number of allocated buckets in the hash table.

.. _hash_dat_type:

Derived Data Types 
==================
The ``DEFINE_NODE_TYPE`` and ``DEFINE_HASH_TABLE_TYPE`` macros are pre-instantiated 
within the ``hash.h`` header file for these data types, allowing for flexible and 
type-safe operations on various types of key-value pairs.

.. code-block:: c

   DEFINE_NODE_TYPE(char, char)
   DEFINE_NODE_TYPE(unsigned char, uchar)
   DEFINE_NODE_TYPE(short int, short)
   DEFINE_NODE_TYPE(unsigned short int, ushort)
   DEFINE_NODE_TYPE(int, int)
   DEFINE_NODE_TYPE(unsigned int, uint)
   DEFINE_NODE_TYPE(long int, long)
   DEFINE_NODE_TYPE(unsigned long int, ulong)
   DEFINE_NODE_TYPE(long long int, llong)
   DEFINE_NODE_TYPE(unsigned long long int, ullong)
   DEFINE_NODE_TYPE(float, float)
   DEFINE_NODE_TYPE(double, double)
   DEFINE_NODE_TYPE(long double, ldouble)
   DEFINE_NODE_TYPE(bool, bool)
   DEFINE_NODE_TYPE(char*, string)

Additionally, the ``DEFINE_HASH_TABLE_TYPE`` macro is pre-instantiated with 
the following ``typedef`` names:

.. code-block:: c

   DEFINE_HASH_TABLE_TYPE(char)
   DEFINE_HASH_TABLE_TYPE(uchar)
   DEFINE_HASH_TABLE_TYPE(short)
   DEFINE_HASH_TABLE_TYPE(ushort)
   DEFINE_HASH_TABLE_TYPE(int)
   DEFINE_HASH_TABLE_TYPE(uint)
   DEFINE_HASH_TABLE_TYPE(long)
   DEFINE_HASH_TABLE_TYPE(ulong)
   DEFINE_HASH_TABLE_TYPE(llong)
   DEFINE_HASH_TABLE_TYPE(ullong)
   DEFINE_HASH_TABLE_TYPE(float)
   DEFINE_HASH_TABLE_TYPE(double)
   DEFINE_HASH_TABLE_TYPE(ldouble)
   DEFINE_HASH_TABLE_TYPE(bool)
   DEFINE_HASH_TABLE_TYPE(string)

This library extends the capabilities of traditional unordered data structures 
by ensuring that operations are carried out with maximum efficiency, regardless 
of the sequence of insertions and deletions, thus providing a robust tool for 
developers to manage key-value pairs effectively.

init_hash_map Function
======================

The ``init_hash_map`` function provides a generic way to initialize hash maps 
for various data types where the key is a string literal. This function leverages 
function pointers to return the appropriate initialization function for 
the specified data type, simplifying the creation of type-specific hash maps.

Description
-----------

The ``init_hash_map`` function accepts an enumeration value representing the 
desired data type and returns a function pointer to the corresponding hash map 
initialization function. Each initialization function allocates memory for the 
hash map, initializes its fields, and returns a pointer to the newly created hash map.

Function Signature
------------------

.. code-block:: c

    typedef void* (*InitHashFunc)(bool);
    InitHashFunc init_hash_map(dtype dat_type);

Parameters
----------

- :c:`dat_type`: An enumeration value representing the data type of the hash map's values. The available data types are:

  - :c:`dChar`: char
  - :c:`dUChar`: unsigned char
  - :c:`dShort`: short int
  - :c:`dUShort`: unsigned short int
  - :c:`dInt`: int
  - :c:`dUInt`: unsigned int
  - :c:`dLong`: long int
  - :c:`dULong`: unsigned long int
  - :c:`dLongLong`: long long int
  - :c:`dULongLong`: unsigned long long int
  - :c:`dFloat`: float
  - :c:`dDouble`: double
  - :c:`dLDouble`: long double
  - :c:`dBool`: bool
  - :c:`dString`: char*

Return Value
------------

The function returns a pointer to the appropriate hash map initialization function 
based on the specified data type. If the data type is not recognized, the function 
returns :c:`NULL`.

Default Values for Initialization
---------------------------------

Each initialization function sets the following default values for the hash map:

- :c:`keyValues`: An array of nodes, each initialized with a :c:`NULL` key and next pointer.
- :c:`hash_size`: 0
- :c:`size`: 0
- :c:`alloc`: The size of the allocated array, defaulted to 3.

Code Example
------------

Here's an example of how to use the ``init_hash_map`` function to initialize a hash map for integers:

.. code-block:: c

    #include "hash.h"

    int main() {
        InitHashFunc init_func = init_hash_map(dInt);
        if (init_func) {
            intHashTable* table = (intHashTable*)init_func(false);
            if (table) {
                // Use the hash table
                free_hash(table);
            } else {
                // Handle memory allocation failure
                fprintf(stderr, "Failed to initialize hash table\n");
            }
        } else {
            // Handle invalid data type
            fprintf(stderr, "Invalid data type\n");
        }
        return 0;
    }

Error Handling
--------------

- If memory allocation fails during the initialization of a hash map, the function sets :c:`errno` to :c:`ENOMEM` and returns :c:`NULL`.
- If an invalid data type is specified, the function returns :c:`NULL`.

Underlying Functions
--------------------

The following functions are called by the :c:`init_hash_map` function based on the specified data type:

- :c:`init_char_hash_map`
- :c:`init_uchar_hash_map`
- :c:`init_short_hash_map`
- :c:`init_ushort_hash_map`
- :c:`init_int_hash_map`
- :c:`init_uint_hash_map`
- :c:`init_long_hash_map`
- :c:`init_ulong_hash_map`
- :c:`init_llong_hash_map`
- :c:`init_ullong_hash_map`
- :c:`init_float_hash_map`
- :c:`init_double_hash_map`
- :c:`init_ldouble_hash_map`
- :c:`init_bool_hash_map`
- :c:`init_string_hash_map`

Each of these functions follows the same pattern of allocating memory, initializing 
fields, and returning a pointer to the newly created hash map.

.. code-block:: c

    charHashTable* init_char_hash_map(dChar) {
        charHashTable* hashPtr = malloc(sizeof(*hashPtr));
        if (!hashPtr) {
            errno = ENOMEM;
            return NULL;
        }
        charNode* arrPtr = malloc(size * sizeof(charNode));
        if (!arrPtr) {
            errno = ENOMEM;
            free(hashPtr);
            return NULL;
        }

        // Initialize each index in the keyValues array with a designated head node
        for (size_t i = 0; i < size; i++) {
            arrPtr[i].key = NULL; // Set the head node's key pointer to NULL
            arrPtr[i].next = NULL; // Set the head node's next pointer to NULL
            arrPtr[i].value = 0; // Initialize value
        }

        hashPtr->keyValues = arrPtr;
        hashPtr->hash_size = 0;
        hashPtr->size = 0;
        hashPtr->alloc = size;
        return hashPtr;
    }

insert_hash_map Macro
=====================

The ``insert_hash_map`` macro provides a type-safe way to insert key-value 
pairs into hash maps of various data types. By leveraging the C11 `_Generic` 
keyword, this macro automatically selects the appropriate type-specific insertion 
function based on the type of the hash map provided.

Description
------------

The ``insert_hash_map`` macro simplifies the process of inserting key-value 
pairs into hash maps by automatically dispatching to the correct type-specific 
insertion function. This ensures type safety and reduces the need for manual 
function selection.

Function Signature
------------------

.. code-block:: c

    #define insert_hash_map(table, key, value) _Generic((table), \
        charHashTable*: insert_char_hash_map, \
        ucharHashTable*: insert_uchar_hash_map, \
        shortHashTable*: insert_short_hash_map, \
        ushortHashTable*: insert_ushort_hash_map, \
        intHashTable*: insert_int_hash_map, \
        uintHashTable*: insert_uint_hash_map, \
        longHashTable*: insert_long_hash_map, \
        ulongHashTable*: insert_ulong_hash_map, \
        llongHashTable*: insert_llong_hash_map, \
        ullongHashTable*: insert_ullong_hash_map, \
        floatHashTable*: insert_float_hash_map, \
        doubleHashTable*: insert_double_hash_map, \
        ldoubleHashTable*: insert_ldouble_hash_map, \
        boolHashTable*: insert_bool_hash_map, \
        stringHashTable*: insert_string_hash_map)(table, key, value)

Parameters
----------

- :c:`table`: A pointer to the hash table into which the key-value pair will be inserted. The type of the table determines which insertion function is called.
- :c:`key`: The key as a string literal to be inserted into the hash map.
- :c:`value`: The value associated with the key. The type of this parameter depends on the type of the hash map.

Error Handling
--------------

The following error handling mechanisms are implemented within the type-specific 
insertion functions:

- **Memory Allocation Failure**: If memory allocation for the new key or the new node fails, the function sets :c:`errno` to :c:`ENOMEM` and returns without modifying the hash table.
- **Duplicate Key**: If the key already exists in the hash table, the function sets :c:`errno` to :c:`EINVAL` and returns without modifying the hash table.

Code Example
------------

Here's an example of how to use the ``insert_hash_map`` macro to insert key-value 
pairs into an integer hash map. The example also demonstrates how to use the 
``free_hash_map`` macro to free all allocated memory.

.. code-block:: c

    #include "hash.h"
    #include <stdio.h>

    int main() {
        // Initialize the hash map for integers
        intHashTable* table = init_hash_map(dInt);
        if (!table) {
            fprintf(stderr, "Failed to initialize hash table\n");
            return 1;
        }

        // Insert key-value pairs into the hash map
        insert_hash_map(table, "Bob", 20);
        insert_hash_map(table, "Alice", 30);
        insert_hash_map(table, "Eve", 25);

        // Retrieve and print values
        int value = get_hash_value(table, "Bob");
        if (value != INT_MAX) {
            printf("Bob's value: %d\n", value);
        } else {
            printf("Bob not found in the hash map\n");
        }

        // Free the hash map
        free_hash_map(table);

        return 0;
    }

When the above code is run, it should produce the following output:

.. code-block:: console

    Bob's value: 20

The corresponding :c:`free_hash_map` macro is used to free all allocated memory 
for any type-specific hash map, ensuring that the resources are properly 
released when the hash map is no longer needed.

pop_hash_map Macro
==================

The ``pop_hash_map`` macro provides a type-safe way to remove key-value pairs 
from hash maps of various data types. By leveraging the C11 `_Generic` keyword, 
this macro automatically selects the appropriate type-specific pop function 
based on the type of the hash map provided.

Description
-----------

The ``pop_hash_map`` macro simplifies the process of removing key-value pairs 
from hash maps by automatically dispatching to the correct type-specific pop 
function. This ensures type safety and reduces the need for manual function 
selection.

Function Signature
------------------

.. code-block:: c

    #define pop_hash_map(table, key) _Generic((table), \
        charHashTable*: pop_char_hash_map, \
        ucharHashTable*: pop_uchar_hash_map, \
        shortHashTable*: pop_short_hash_map, \
        ushortHashTable*: pop_ushort_hash_map, \
        intHashTable*: pop_int_hash_map, \
        uintHashTable*: pop_uint_hash_map, \
        longHashTable*: pop_long_hash_map, \
        ulongHashTable*: pop_ulong_hash_map, \
        llongHashTable*: pop_llong_hash_map, \
        ullongHashTable*: pop_ullong_hash_map, \
        floatHashTable*: pop_float_hash_map, \
        doubleHashTable*: pop_double_hash_map, \
        ldoubleHashTable*: pop_ldouble_hash_map, \
        boolHashTable*: pop_bool_hash_map, \
        stringHashTable*: pop_string_hash_map) (table, key)

Parameters
----------

- :c:`table`: A pointer to the hash table from which the key-value pair will be removed. The type of the table determines which pop function is called.
- :c:`key`: The key as a string literal to be removed from the hash map.

Error Handling
--------------

The following error handling mechanisms are implemented within the type-specific pop functions:

- **Key Not Found**: If the key does not exist in the hash table, the function returns a type-specific default value. For numerical types, this is typically a sentinel value (e.g., :c:`INT_MAX` for integers).

Code Example
------------

Here's an example of how to use the ``pop_hash_map`` macro to remove key-value
pairs from an integer hash map. The example also demonstrates how to use the 
``free_hash_map`` macro to free all allocated memory.

.. code-block:: c

    #include "hash.h"
    #include <stdio.h>

    int main() {
        // Initialize the hash map for integers
        intHashTable* table = init_hash_map(dInt);
        if (!table) {
            fprintf(stderr, "Failed to initialize hash table\n");
            return 1;
        }

        // Insert key-value pairs into the hash map
        insert_hash_map(table, "Bob", 20);
        insert_hash_map(table, "Alice", 30);
        insert_hash_map(table, "Eve", 25);

        // Pop and print values
        int value = pop_hash_map(table, "Bob");
        if (value != INT_MAX) {
            printf("Popped Bob's value: %d\n", value);
        } else {
            printf("Bob not found in the hash map\n");
        }

        value = get_hash_value(table, "Bob");
        if (value != INT_MAX) {
            printf("Bob's value: %d\n", value);
        } else {
            printf("Bob not found in the hash map\n");
        }

        // Free the hash map
        free_hash_map(table);

        return 0;
    }

Expected Output
---------------

When the above code is run, it should produce the following output:

.. code-block:: console

    Popped Bob's value: 20
    Bob not found in the hash map

Special Considerations for Strings
----------------------------------

For hash maps where the values are strings (i.e., :c:`stringHashTable`), the 
returned string is dynamically allocated and must be manually freed by the caller 
to avoid memory leaks. Below is the type-specific pop function for strings:

When using :c:`pop_string_hash_map`, the returned :c:`str*` should be freed by 
the caller to prevent memory leaks:

.. code-block:: c

    str* value = pop_hash_map(table, "Bob");
    if (value) {
        printf("Popped Bob's value: %s\n", value->data);
        free(value->data);
        free(value);
    } else {
        printf("Bob not found in the hash map\n");
    }

The developer can also use the ``gbc_str`` macro to automate the freeing of 
memory 

.. code-block:: c

    str* value gbc_str = pop_hash_map(table, "Bob");
    // The user no longer needs to manually free value

get_hash_value Macro
====================

The ``get_hash_value`` macro provides a type-safe way to retrieve values from 
hash maps of various data types based on a given key. By leveraging the C11 
`_Generic` keyword, this macro automatically selects the appropriate type-specific 
retrieval function based on the type of the hash map provided.

Description
-----------

The ``get_hash_value`` macro simplifies the process of retrieving values from 
hash maps by automatically dispatching to the correct type-specific retrieval 
function. This ensures type safety and reduces the need for manual function selection.

Function Signature
------------------

.. code-block:: c

    #define get_hash_value(table, key) _Generic((table), \
        charHashTable*: get_char_hash_value, \
        ucharHashTable*: get_uchar_hash_value, \
        shortHashTable*: get_short_hash_value, \
        ushortHashTable*: get_ushort_hash_value, \
        intHashTable*: get_int_hash_value, \
        uintHashTable*: get_uint_hash_value, \
        longHashTable*: get_long_hash_value, \
        ulongHashTable*: get_ulong_hash_value, \
        llongHashTable*: get_llong_hash_value, \
        ullongHashTable*: get_ullong_hash_value, \
        floatHashTable*: get_float_hash_value, \
        doubleHashTable*: get_double_hash_value, \
        ldoubleHashTable*: get_ldouble_hash_value, \
        boolHashTable*: get_bool_hash_value, \
        stringHashTable*: get_string_hash_value) (table, key)

Parameters
----------

- :c:`table`: A pointer to the hash table from which the value will be retrieved. The type of the table determines which retrieval function is called.
- :c:`key`: The key as a string literal to retrieve the value from the hash map.

Error Handling
--------------

The following error handling mechanisms are implemented within the type-specific 
retrieval functions:

- **Key Not Found**: If the key does not exist in the hash table, the function returns a type-specific default value. For numerical types, this is typically a sentinel value (e.g., :c:`SHRT_MAX` for short integers).

Code Example
------------

Here's an example of how to use the ``get_hash_value`` macro to retrieve values 
from an integer hash map. The example also demonstrates how to use the 
``free_hash_map`` macro to free all allocated memory.

.. code-block:: c

    #include "hash.h"
    #include <stdio.h>

    int main() {
        // Initialize the hash map for integers
        intHashTable* table = init_hash_map(dInt);
        if (!table) {
            fprintf(stderr, "Failed to initialize hash table\n");
            return 1;
        }

        // Insert key-value pairs into the hash map
        insert_hash_map(table, "Bob", 20);
        insert_hash_map(table, "Alice", 30);
        insert_hash_map(table, "Eve", 25);

        // Retrieve and print values
        int value = get_hash_value(table, "Bob");
        if (value != INT_MAX) {
            printf("Bob's value: %d\n", value);
        } else {
            printf("Bob not found in the hash map\n");
        }

        // Free the hash map
        free_hash_map(table);

        return 0;
    }

Expected Output
---------------

When the above code is run, it should produce the following output:

.. code-block:: console

    Bob's value: 20

Special Considerations for Strings
----------------------------------

For hash maps where the values are strings (i.e., :c:`stringHashTable`), the 
returned string is a shallow copy. This means that the string pointer returned 
by the :c:`get_string_hash_value` function points to the original string stored 
in the hash map. The user should take care not to free this string, as it is 
managed by the hash map itself. However, other string operations defined in 
the :c:`str.h` file can be safely performed on it.


Here's an example of how to retrieve and use a string value from a string hash map:

.. code-block:: c

    #include "hash.h"
    #include <stdio.h>

    int main() {
        // Initialize the hash map for strings
        stringHashTable* table = init_hash_map(dString);
        if (!table) {
            fprintf(stderr, "Failed to initialize hash table\n");
            return 1;
        }

        // Insert key-value pairs into the hash map
        insert_hash_map(table, "Bob", "Hello");
        insert_hash_map(table, "Alice", "World");

        // Retrieve and print values
        str* value = get_hash_value(table, "Bob");
        if (value) {
            printf("Bob's value: %s\n", value->data);
        } else {
            printf("Bob not found in the hash map\n");
        }

        // Free the hash map
        free_hash_map(table);

        return 0;
    }

Expected Output for Strings
---------------------------

When the above code is run, it should produce the following output:

.. code-block:: console

    Bob's value: Hello

free_hash_map Macro
===================

The ``free_hash_map`` macro provides a type-safe way to free memory allocated 
for hash maps of various data types. By leveraging the C11 `_Generic` keyword, 
this macro automatically selects the appropriate type-specific free function 
based on the type of the hash map provided.

Description
-----------

The ``free_hash_map`` macro simplifies the process of freeing memory allocated 
for hash maps by automatically dispatching to the correct type-specific free 
function. This ensures type safety and reduces the need for manual function selection.

Function Signature
------------------

.. code-block:: c

    #define free_hash_map(table) _Generic((table), \
        charHashTable*: free_char_hash_map, \
        ucharHashTable*: free_uchar_hash_map, \
        shortHashTable*: free_short_hash_map, \
        ushortHashTable*: free_ushort_hash_map, \
        intHashTable*: free_int_hash_map, \
        uintHashTable*: free_uint_hash_map, \
        longHashTable*: free_long_hash_map, \
        ulongHashTable*: free_ulong_hash_map, \
        llongHashTable*: free_llong_hash_map, \
        ullongHashTable*: free_ullong_hash_map, \
        floatHashTable*: free_float_hash_map, \
        doubleHashTable*: free_double_hash_map, \
        ldoubleHashTable*: free_ldouble_hash_map, \
        boolHashTable*: free_bool_hash_map, \
        stringHashTable*: free_string_hash_map) (table)

Parameters
----------

- :c:`table`: A pointer to the hash table that will be freed. The type of the table determines which free function is called.

Error Handling
--------------

- **NULL Pointer Check**: Each type-specific free function should check if the 
  provided table pointer is :c:`NULL` before attempting to free its contents.

Code Example
------------

Here's an example of how to use the ``free_hash_map`` macro to free all 
allocated memory for an integer hash map.

.. code-block:: c

    #include "hash.h"
    #include <stdio.h>

    int main() {
        // Initialize the hash map for integers
        intHashTable* table = init_int_hash_map();
        if (!table) {
            fprintf(stderr, "Failed to initialize hash table\n");
            return 1;
        }

        // Insert key-value pairs into the hash map
        insert_hash_map(table, "Bob", 20);
        insert_hash_map(table, "Alice", 30);
        insert_hash_map(table, "Eve", 25);

        // Free the hash map
        free_hash_map(table);

        return 0;
    }

Automatic Garbage Collection
============================

The automatic garbage collection macros provide a convenient way to automatically 
free memory allocated for hash maps of various data types when they go out of scope. 
These macros leverage the `__attribute__((cleanup(...)))` feature available in 
GCC and Clang to ensure that the appropriate cleanup function is called when 
the variable goes out of scope.

Description
-----------

The automatic garbage collection macros simplify memory management by ensuring 
that hash maps are automatically freed when they go out of scope. This helps 
prevent memory leaks and reduces the need for manual memory management.

Function Signatures
-------------------

.. code-block:: c

    void _freeCharHashTable(charHashTable** table);
    void _freeUCharHashTable(ucharHashTable** table);
    void _freeShortHashTable(shortHashTable** table);
    void _freeUShortHashTable(ushortHashTable** table);
    void _freeIntHashTable(intHashTable** table);
    void _freeUIntHashTable(uintHashTable** table);
    void _freeLongHashTable(longHashTable** table);
    void _freeULongHashTable(ulongHashTable** table);
    void _freeLLongHashTable(llongHashTable** table);
    void _freeULLongHashTable(ullongHashTable** table);
    void _freeFloatHashTable(floatHashTable** table);
    void _freeDoubleHashTable(doubleHashTable** table);
    void _freeLDoubleHashTable(ldoubleHashTable** table);
    void _freeBoolHashTable(boolHashTable** table);
    void _freeStringHashTable(stringHashTable** table);

Each of these functions accepts a double pointer to a hash table and calls the 
corresponding `free_type_hash_map` function to free the memory allocated for the hash table.

Automatic Garbage Collection Macros
-----------------------------------

.. code-block:: c

    #if defined(__GNUC__) || defined(__clang__)
    #define gbc_char_map __attribute__((cleanup(_freeCharHashTable)))
    #define gbc_uchar_map __attribute__((cleanup(_freeUCharHashTable)))
    #define gbc_short_map __attribute__((cleanup(_freeShortHashTable)))
    #define gbc_ushort_map __attribute__((cleanup(_freeUShortHashTable)))
    #define gbc_int_map __attribute__((cleanup(_freeIntHashTable)))
    #define gbc_uint_map __attribute__((cleanup(_freeUIntHashTable)))
    #define gbc_long_map __attribute__((cleanup(_freeLongHashTable)))
    #define gbc_ulong_map __attribute__((cleanup(_freeULongHashTable)))
    #define gbc_llong_map __attribute__((cleanup(_freeLLongHashTable)))
    #define gbc_ullong_map __attribute__((cleanup(_freeULLongHashTable)))
    #define gbc_float_map __attribute__((cleanup(_freeFloatHashTable)))
    #define gbc_double_map __attribute__((cleanup(_freeDoubleHashTable)))
    #define gbc_ldouble_map __attribute__((cleanup(_freeLDoubleHashTable)))
    #define gbc_bool_map __attribute__((cleanup(_freeBoolHashTable)))
    #define gbc_string_map __attribute__((cleanup(_freeStringHashTable)))
    #endif

These macros apply the `__attribute__((cleanup(...)))` attribute to variables, 
ensuring that the specified cleanup function is called when the variable goes out of scope.

Usage
-----

To use the automatic garbage collection macros, simply declare your hash map 
variable using the appropriate macro. For example:

.. code-block:: c

    #include "hash.h"
    #include <stdio.h>

    int main() {
        // Declare hash maps with automatic garbage collection
        gbc_int_map intHashTable* table = init_int_hash_map();
        if (!table) {
            fprintf(stderr, "Failed to initialize hash table\n");
            return 1;
        }

        // Insert key-value pairs into the hash map
        insert_hash_map(table, "Bob", 20);
        insert_hash_map(table, "Alice", 30);
        insert_hash_map(table, "Eve", 25);

        // Retrieve and print values
        int value = get_hash_value(table, "Bob");
        if (value != INT_MAX) {
            printf("Bob's value: %d\n", value);
        } else {
            printf("Bob not found in the hash map\n");
        }

        // The hash map is automatically freed when it goes out of scope
        return 0;
    }

Expected Output
---------------

When the above code is run, it should produce the following output:

.. code-block:: console

    Bob's value: 20

update_hash_map Macro
=====================

The ``update_hash_map`` macro provides a type-safe way to update values in 
hash maps of various data types based on a given key. By leveraging the C11 
`_Generic` keyword, this macro automatically selects the appropriate 
type-specific update function based on the type of the hash map provided.

Description
-----------

The ``update_hash_map`` macro simplifies the process of updating values in 
hash maps by automatically dispatching to the correct type-specific update 
function. This ensures type safety and reduces the need for manual function selection.

Function Signature
------------------

.. code-block:: c

    #define update_hash_map(table, key, value) _Generic((table), \
        charHashTable*: update_char_hash_map, \
        ucharHashTable*: update_uchar_hash_map, \
        shortHashTable*: update_short_hash_map, \
        ushortHashTable*: update_ushort_hash_map, \
        intHashTable*: update_int_hash_map, \
        uintHashTable*: update_uint_hash_map, \
        longHashTable*: update_long_hash_map, \
        ulongHashTable*: update_ulong_hash_map, \
        llongHashTable*: update_llong_hash_map, \
        ullongHashTable*: update_ullong_hash_map, \
        floatHashTable*: update_float_hash_map, \
        doubleHashTable*: update_double_hash_map, \
        ldoubleHashTable*: update_ldouble_hash_map, \
        boolHashTable*: update_bool_hash_map, \
        stringHashTable*: update_string_hash_map) (table, key, value)

Parameters
----------

- :c:`table`: A pointer to the hash table in which the value will be updated. The type of the table determines which update function is called.
- :c:`key`: The key as a string literal to update the value in the hash map.
- :c:`value`: The new value to be associated with the key. The type of this parameter depends on the type of the hash map.

Error Handling
--------------

- **Key Not Found**: If the key does not exist in the hash map, the function 
  takes no action. The existing entries in the hash map remain unchanged.

Code Example
------------

Here's an example of how to use the ``update_hash_map`` macro to update values in an 
integer hash map. The example also demonstrates how to use the ``free_hash_map``
macro to free all allocated memory.

.. code-block:: c

    #include "hash.h"
    #include <stdio.h>

    int main() {
        // Initialize the hash map for integers
        intHashTable* table = init_hash_map(dInt);
        if (!table) {
            fprintf(stderr, "Failed to initialize hash table\n");
            return 1;
        }

        // Insert key-value pairs into the hash map
        insert_hash_map(table, "Bob", 20);
        insert_hash_map(table, "Alice", 30);
        insert_hash_map(table, "Eve", 25);

        // Update a value in the hash map
        update_hash_map(table, "Alice", 35);

        // Retrieve and print values
        int value = get_hash_value(table, "Alice");
        if (value != INT_MAX) {
            printf("Alice's updated value: %d\n", value);
        } else {
            printf("Alice not found in the hash map\n");
        }

        // Free the hash map
        free_hash_map(table);

        return 0;
    }

Expected Output
---------------

When the above code is run, it should produce the following output:

.. code-block:: console

    Alice's updated value: 35


Expected Output for Strings
---------------------------

When the following code is run, it should produce the expected output.

.. code-block:: c

    #include "hash.h"
    #include <stdio.h>

    int main() {
        // Initialize the hash map for strings
        stringHashTable* table = init_hash_map(dString);
        if (!table) {
            fprintf(stderr, "Failed to initialize hash table\n");
            return 1;
        }

        // Insert key-value pairs into the hash map
        insert_hash_map(table, "Bob", "Hello");
        insert_hash_map(table, "Alice", "World");

        // Update a value in the hash map
        update_hash_map(table, "Alice", "Everyone");

        // Retrieve and print values
        str* value = get_hash_value(table, "Alice");
        if (value) {
            printf("Alice's updated value: %s\n", value->data);
        } else {
            printf("Alice not found in the hash map\n");
        }

        // Free the hash map
        free_hash_map(table);

        return 0;
    }

Expected Output for Strings
---------------------------

When the above code is run, it should produce the following output:

.. code-block:: console

    Alice's updated value: Everyone

hash_map_alloc Macro
====================

The ``hash_map_alloc`` macro provides a type-safe way to retrieve the allocated 
size of hash maps of various data types. By leveraging the C11 `_Generic` keyword, 
this macro automatically selects the appropriate type-specific allocation retrieval 
function based on the type of the hash map provided.

Description
-----------

The ``hash_map_alloc`` macro simplifies the process of retrieving the allocated 
size of hash maps by automatically dispatching to the correct type-specific function. 
This ensures type safety and reduces the need for manual function selection.

Function Signature
------------------

.. code-block:: c

    #define hash_map_alloc(table) _Generic((table), \
        charHashTable*: char_hash_map_alloc, \
        ucharHashTable*: uchar_hash_map_alloc, \
        shortHashTable*: short_hash_map_alloc, \
        ushortHashTable*: ushort_hash_map_alloc, \
        intHashTable*: int_hash_map_alloc, \
        uintHashTable*: uint_hash_map_alloc, \
        longHashTable*: long_hash_map_alloc, \
        ulongHashTable*: ulong_hash_map_alloc, \
        llongHashTable*: llong_hash_map_alloc, \
        ullongHashTable*: ullong_hash_map_alloc, \
        floatHashTable*: float_hash_map_alloc, \
        doubleHashTable*: double_hash_map_alloc, \
        ldoubleHashTable*: ldouble_hash_map_alloc, \
        boolHashTable*: bool_hash_map_alloc, \
        stringHashTable*: string_hash_map_alloc) (table)

Parameters
----------

- :c:`table`: A pointer to the hash table for which the allocated size will be retrieved. The type of the table determines which allocation retrieval function is called.

Return Value
------------

The macro returns the allocated size of the hash map, which represents the number 
of buckets allocated for storing key-value pairs.

Code Example
------------

Here's an example of how to use the ``hash_map_alloc`` macro to retrieve the 
allocated size of an integer hash map. The example also demonstrates how to use 
the ``free_hash_map`` macro to free all allocated memory.

.. code-block:: c

    #include "hash.h"
    #include <stdio.h>

    int main() {
        // Initialize the hash map for integers
        intHashTable* table = init_hash_map(dInt);
        if (!table) {
            fprintf(stderr, "Failed to initialize hash table\n");
            return 1;
        }

        // Insert key-value pairs into the hash map
        insert_hash_map(table, "Bob", 20);
        insert_hash_map(table, "Alice", 30);
        insert_hash_map(table, "Eve", 25);

        // Retrieve and print the allocated size of the hash map
        size_t alloc_size = hash_map_alloc(table);
        printf("Allocated size of the hash map: %zu\n", alloc_size);

        // Free the hash map
        free_hash_map(table);

        return 0;
    }

Expected Output
---------------

When the above code is run, it should produce the following output:

.. code-block:: console

    Allocated size of the hash map: 3


