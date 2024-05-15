// ================================================================================
// ================================================================================
// - File:    hash.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 13, 2024
// - Version: 1.0
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/hash.h"

// ================================================================================
// ================================================================================

const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t HASH_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t HASH_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
// ================================================================================
// ================================================================================
// Static functions

static size_t hash_function(const char* key) {
    size_t hash = 5381; // Initialize hash with a prime number
    int c;

    while ((c = *key++)) { // Iterate over each character in the key
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash; // Return the computed hash value
}
// ================================================================================

static void resize_char_hash(charHashTable* table, size_t new_size) {
    // Allocate a new larger array
    charNode* new_table = malloc(new_size * sizeof(charNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        charNode* current = table->keyValues[i].next;
        while (current) {
            charNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_uchar_hash(ucharHashTable* table, size_t new_size) {
    // Allocate a new larger array
    ucharNode* new_table = malloc(new_size * sizeof(ucharNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        ucharNode* current = table->keyValues[i].next;
        while (current) {
            ucharNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_short_hash(shortHashTable* table, size_t new_size) {
    // Allocate a new larger array
    shortNode* new_table = malloc(new_size * sizeof(shortNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        shortNode* current = table->keyValues[i].next;
        while (current) {
            shortNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_ushort_hash(ushortHashTable* table, size_t new_size) {
    // Allocate a new larger array
    ushortNode* new_table = malloc(new_size * sizeof(ushortNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        ushortNode* current = table->keyValues[i].next;
        while (current) {
            ushortNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_int_hash(intHashTable* table, size_t new_size) {
    // Allocate a new larger array
    intNode* new_table = malloc(new_size * sizeof(intNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        intNode* current = table->keyValues[i].next;
        while (current) {
            intNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_uint_hash(uintHashTable* table, size_t new_size) {
    // Allocate a new larger array
    uintNode* new_table = malloc(new_size * sizeof(uintNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        uintNode* current = table->keyValues[i].next;
        while (current) {
            uintNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_long_hash(longHashTable* table, size_t new_size) {
    // Allocate a new larger array
    longNode* new_table = malloc(new_size * sizeof(longNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        longNode* current = table->keyValues[i].next;
        while (current) {
            longNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// -------------------------------------------------------------------------------- 

static void resize_ulong_hash(ulongHashTable* table, size_t new_size) {
    // Allocate a new larger array
    ulongNode* new_table = malloc(new_size * sizeof(ulongNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        ulongNode* current = table->keyValues[i].next;
        while (current) {
            ulongNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_llong_hash(llongHashTable* table, size_t new_size) {
    // Allocate a new larger array
    llongNode* new_table = malloc(new_size * sizeof(llongNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        llongNode* current = table->keyValues[i].next;
        while (current) {
            llongNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_ullong_hash(ullongHashTable* table, size_t new_size) {
    // Allocate a new larger array
    ullongNode* new_table = malloc(new_size * sizeof(ullongNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        ullongNode* current = table->keyValues[i].next;
        while (current) {
            ullongNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_float_hash(floatHashTable* table, size_t new_size) {
    // Allocate a new larger array
    floatNode* new_table = malloc(new_size * sizeof(floatNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        floatNode* current = table->keyValues[i].next;
        while (current) {
            floatNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_double_hash(doubleHashTable* table, size_t new_size) {
    // Allocate a new larger array
    doubleNode* new_table = malloc(new_size * sizeof(doubleNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        doubleNode* current = table->keyValues[i].next;
        while (current) {
            doubleNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_ldouble_hash(ldoubleHashTable* table, size_t new_size) {
    // Allocate a new larger array
    ldoubleNode* new_table = malloc(new_size * sizeof(ldoubleNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        ldoubleNode* current = table->keyValues[i].next;
        while (current) {
            ldoubleNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_bool_hash(boolHashTable* table, size_t new_size) {
    // Allocate a new larger array
    boolNode* new_table = malloc(new_size * sizeof(boolNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        boolNode* current = table->keyValues[i].next;
        while (current) {
            boolNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// --------------------------------------------------------------------------------

static void resize_string_hash(stringHashTable* table, size_t new_size) {
    // Allocate a new larger array
    stringNode* new_table = malloc(new_size * sizeof(stringNode));
    if (!new_table) {
        errno = ENOMEM;
        return;
    }
    
    // Initialize the new array
    for (size_t i = 0; i < new_size; i++) {
        new_table[i].key = NULL;
        new_table[i].next = NULL;
        new_table[i].value = 0; // Initialize value
    }
    
    // Rehash existing key-value pairs and insert them into the new array
    for (size_t i = 0; i < table->alloc; i++) {
        stringNode* current = table->keyValues[i].next;
        while (current) {
            stringNode* next = current->next;
            size_t new_index = hash_function(current->key) % new_size;
            
            // Move the node to the new hash table
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            current = next;
        }
    }
    
    // Update the size to reflect the number of populated indices
    table->size = 0;
    for (size_t i = 0; i < new_size; i++) {
        if (new_table[i].next != NULL) {
            table->size++;
        }
    }
    
    // Free the old array, but do not free the keys or keyValues structs
    free(table->keyValues);
    
    // Update the hash table's fields with the new array and size
    table->keyValues = new_table;
    table->alloc = new_size;
}
// ================================================================================
// ================================================================================

charHashTable* init_char_hash_map(size_t size) {
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
// --------------------------------------------------------------------------------

ucharHashTable* init_uchar_hash_map(size_t size) {
    ucharHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    ucharNode* arrPtr = malloc(size * sizeof(ucharNode));
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
// --------------------------------------------------------------------------------

shortHashTable* init_short_hash_map(size_t size) {
    shortHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    shortNode* arrPtr = malloc(size * sizeof(shortNode));
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
// --------------------------------------------------------------------------------

ushortHashTable* init_ushort_hash_map(size_t size) {
    ushortHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    ushortNode* arrPtr = malloc(size * sizeof(ushortNode));
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
// --------------------------------------------------------------------------------

intHashTable* init_int_hash_map(size_t size) {
    intHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    intNode* arrPtr = malloc(size * sizeof(intNode));
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
// --------------------------------------------------------------------------------

uintHashTable* init_uint_hash_map(size_t size) {
    uintHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    uintNode* arrPtr = malloc(size * sizeof(uintNode));
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
// --------------------------------------------------------------------------------

longHashTable* init_long_hash_map(size_t size) {
    longHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    longNode* arrPtr = malloc(size * sizeof(longNode));
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
// --------------------------------------------------------------------------------

ulongHashTable* init_ulong_hash_map(size_t size) {
    ulongHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    ulongNode* arrPtr = malloc(size * sizeof(ulongNode));
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
// --------------------------------------------------------------------------------

llongHashTable* init_llong_hash_map(size_t size) {
    llongHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    llongNode* arrPtr = malloc(size * sizeof(llongNode));
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
// --------------------------------------------------------------------------------

ullongHashTable* init_ullong_hash_map(size_t size) {
    ullongHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    ullongNode* arrPtr = malloc(size * sizeof(ullongNode));
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
// --------------------------------------------------------------------------------

floatHashTable* init_float_hash_map(size_t size) {
    floatHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    floatNode* arrPtr = malloc(size * sizeof(floatNode));
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
// --------------------------------------------------------------------------------

doubleHashTable* init_double_hash_map(size_t size) {
    doubleHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    doubleNode* arrPtr = malloc(size * sizeof(doubleNode));
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
// --------------------------------------------------------------------------------

ldoubleHashTable* init_ldouble_hash_map(size_t size) {
    ldoubleHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    ldoubleNode* arrPtr = malloc(size * sizeof(ldoubleNode));
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
// --------------------------------------------------------------------------------

boolHashTable* init_bool_hash_map(size_t size) {
    boolHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    boolNode* arrPtr = malloc(size * sizeof(boolNode));
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
// --------------------------------------------------------------------------------

stringHashTable* init_string_hash_map(size_t size) {
    stringHashTable* hashPtr = malloc(sizeof(*hashPtr));
    if (!hashPtr) {
        errno = ENOMEM;
        return NULL;
    }
    stringNode* arrPtr = malloc(size * sizeof(stringNode));
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
// --------------------------------------------------------------------------------

InitHashFunc init_hash_map(dtype dat_type) {
    switch(dat_type) {
        case dChar: return (InitHashFunc)init_char_hash_map;
        case dUChar: return (InitHashFunc)init_uchar_hash_map;
        case dShort: return (InitHashFunc)init_short_hash_map;
        case dUShort: return (InitHashFunc)init_ushort_hash_map;
        case dInt: return (InitHashFunc)init_int_hash_map;
        case dUInt: return (InitHashFunc)init_uint_hash_map;
        case dLong: return (InitHashFunc)init_long_hash_map;
        case dULong: return (InitHashFunc)init_ulong_hash_map;
        case dLongLong: return (InitHashFunc)init_llong_hash_map;
        case dULongLong: return (InitHashFunc)init_ullong_hash_map;
        case dFloat: return (InitHashFunc)init_float_hash_map;
        case dDouble: return (InitHashFunc)init_double_hash_map;
        case dLDouble: return (InitHashFunc)init_ldouble_hash_map;
        case dBool: return (InitHashFunc)init_bool_hash_map;
        case dString: return (InitHashFunc)init_string_hash_map;
        default: return NULL;
    } 
}
// ================================================================================
// ================================================================================

void insert_char_hash_table(charHashTable* table, char* key, char value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_char_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    charNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    charNode* new_node = malloc(sizeof(charNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_uchar_hash_table(ucharHashTable* table, char* key, unsigned char value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_uchar_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    ucharNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    ucharNode* new_node = malloc(sizeof(ucharNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_short_hash_table(shortHashTable* table, char* key, short int value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_short_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    shortNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    shortNode* new_node = malloc(sizeof(shortNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_ushort_hash_table(ushortHashTable* table, char* key, unsigned short int value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_ushort_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    ushortNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    ushortNode* new_node = malloc(sizeof(ushortNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_int_hash_table(intHashTable* table, char* key, int value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_int_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    intNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    intNode* new_node = malloc(sizeof(intNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_uint_hash_table(uintHashTable* table, char* key, unsigned int value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_uint_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    uintNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    uintNode* new_node = malloc(sizeof(uintNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_long_hash_table(longHashTable* table, char* key, long int value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_long_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    longNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    longNode* new_node = malloc(sizeof(longNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_ulong_hash_table(ulongHashTable* table, char* key, unsigned long int value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_ulong_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    ulongNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    ulongNode* new_node = malloc(sizeof(ulongNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_llong_hash_table(llongHashTable* table, char* key, unsigned long int value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_llong_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    llongNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    llongNode* new_node = malloc(sizeof(llongNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_ullong_hash_table(ullongHashTable* table, char* key, unsigned long long int value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_ullong_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    ullongNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    ullongNode* new_node = malloc(sizeof(ullongNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_float_hash_table(floatHashTable* table, char* key, float value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_float_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    floatNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    floatNode* new_node = malloc(sizeof(floatNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_double_hash_table(doubleHashTable* table, char* key, double value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_double_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    doubleNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    doubleNode* new_node = malloc(sizeof(doubleNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_ldouble_hash_table(ldoubleHashTable* table, char* key, long double value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_ldouble_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    ldoubleNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    ldoubleNode* new_node = malloc(sizeof(ldoubleNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_bool_hash_table(boolHashTable* table, char* key, bool value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_bool_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    boolNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    boolNode* new_node = malloc(sizeof(boolNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = value;
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// --------------------------------------------------------------------------------

void insert_string_hash_table(stringHashTable* table, char* key, char* value) {
    // Check if resizing is needed
    if (table->hash_size >= table->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_alloc = table->alloc < HASH_THRESHOLD ? table->alloc * 2 : table->alloc + HASH_FIXED_AMOUNT;
        resize_string_hash(table, new_alloc);
    } 
    
    size_t index = hash_function(key) % table->alloc;

    // Check if the key already exists in the hash table
    stringNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key already exists, return control to the calling program
            errno = EINVAL;
            return;
        }
        current = current->next;
    }

    // Allocate memory for the key
    char* new_key = malloc((strlen(key) + 1) * sizeof(char));
    if (!new_key) {
        errno = ENOMEM;
        return;
    }
    strcpy(new_key, key); // Copy the key to the dynamically allocated memory
    
    // Insert the new key-value pair into the hash table
    stringNode* new_node = malloc(sizeof(stringNode));
    if (!new_node) {
        // Handle memory allocation failure
        free(new_key); // Free the allocated key memory
        return;
    }
    new_node->key = new_key;
    new_node->value = init_string_nol(value);
    new_node->next = table->keyValues[index].next;
    table->keyValues[index].next = new_node;
    
    table->hash_size++; // Increment the number of key-value pairs
    
    // Update the size if the index was previously empty
    if (table->keyValues[index].next == new_node) {
        table->size++;
    }
}
// ================================================================================
// ================================================================================

char pop_char_hash_map(charHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    charNode* prev = &table->keyValues[index];
    charNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            char value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return CHAR_MAX;
}
// --------------------------------------------------------------------------------

unsigned char pop_uchar_hash_map(ucharHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    ucharNode* prev = &table->keyValues[index];
    ucharNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            unsigned char value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return UCHAR_MAX;
}
// --------------------------------------------------------------------------------

short int pop_short_hash_map(shortHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    shortNode* prev = &table->keyValues[index];
    shortNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            short int value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return SHRT_MAX;
}
// --------------------------------------------------------------------------------

unsigned short int pop_ushort_hash_map(ushortHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    ushortNode* prev = &table->keyValues[index];
    ushortNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            unsigned short int value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return USHRT_MAX;
}
// --------------------------------------------------------------------------------

int pop_int_hash_map(intHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    intNode* prev = &table->keyValues[index];
    intNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            int value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return INT_MAX;
}
// --------------------------------------------------------------------------------

unsigned int pop_uint_hash_map(uintHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    uintNode* prev = &table->keyValues[index];
    uintNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            unsigned int value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return UINT_MAX;
}
// --------------------------------------------------------------------------------

long int pop_long_hash_map(longHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    longNode* prev = &table->keyValues[index];
    longNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            long int value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return LONG_MAX;
}
// --------------------------------------------------------------------------------

unsigned long int pop_ulong_hash_map(ulongHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    ulongNode* prev = &table->keyValues[index];
    ulongNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            unsigned long int value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return ULONG_MAX;
}
// --------------------------------------------------------------------------------

long long int pop_llong_hash_map(llongHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    llongNode* prev = &table->keyValues[index];
    llongNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            long long int value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return LLONG_MAX;
}
// --------------------------------------------------------------------------------

unsigned long long int pop_ullong_hash_map(ullongHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    ullongNode* prev = &table->keyValues[index];
    ullongNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            unsigned long long int value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return ULLONG_MAX;
}
// --------------------------------------------------------------------------------

float pop_float_hash_map(floatHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    floatNode* prev = &table->keyValues[index];
    floatNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            float value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return FLT_MAX;
}
// --------------------------------------------------------------------------------

double pop_double_hash_map(doubleHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    doubleNode* prev = &table->keyValues[index];
    doubleNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            double value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return DBL_MAX;
}
// --------------------------------------------------------------------------------

long double pop_ldouble_hash_map(ldoubleHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    ldoubleNode* prev = &table->keyValues[index];
    ldoubleNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            long double value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return LDBL_MAX;
}
// --------------------------------------------------------------------------------

bool pop_bool_hash_map(boolHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    boolNode* prev = &table->keyValues[index];
    boolNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            bool value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return false;
}
// --------------------------------------------------------------------------------

str* pop_string_hash_map(stringHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;

    // Traverse the linked list at the index
    stringNode* prev = &table->keyValues[index];
    stringNode* current = prev->next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, unlink the node from the linked list
            prev->next = current->next;
            
            // Retrieve the value associated with the key
            str* value = current->value;

            // Free the memory allocated for the key and the node
            free(current->key);
            free(current);

            // Decrement the number of key-value pairs in the hash table
            table->hash_size--;

            // Return the value associated with the key
            return value;
        }
        prev = current;
        current = current->next;
    }

    return NULL;
}
// ================================================================================
// ================================================================================

char get_char_hash_value(charHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    charNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return CHAR_MAX; 
}
// --------------------------------------------------------------------------------

unsigned char get_uchar_hash_value(ucharHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    ucharNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return UCHAR_MAX; 
}
// --------------------------------------------------------------------------------

short int get_short_hash_value(shortHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    shortNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return SHRT_MAX; 
}
// --------------------------------------------------------------------------------

unsigned short int get_ushort_hash_value(ushortHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    ushortNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return USHRT_MAX; 
}
// --------------------------------------------------------------------------------

int get_int_hash_value(intHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    intNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return INT_MAX; 
}
// --------------------------------------------------------------------------------

unsigned int get_uint_hash_value(uintHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    uintNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return UINT_MAX; 
}
// --------------------------------------------------------------------------------

long int get_long_hash_value(longHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    longNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return LONG_MAX; 
}
// --------------------------------------------------------------------------------

unsigned long int get_ulong_hash_value(ulongHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    ulongNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return ULONG_MAX; 
}
// --------------------------------------------------------------------------------

long long int get_llong_hash_value(llongHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    llongNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return LLONG_MAX; 
}
// --------------------------------------------------------------------------------

unsigned long long int get_ullong_hash_value(ullongHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    ullongNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return LLONG_MAX; 
}
// --------------------------------------------------------------------------------

float get_float_hash_value(floatHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    floatNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return FLT_MAX; 
}
// --------------------------------------------------------------------------------

double get_double_hash_value(doubleHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    doubleNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return DBL_MAX; 
}
// --------------------------------------------------------------------------------

long double get_ldouble_hash_value(ldoubleHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    ldoubleNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return LDBL_MAX; 
}
// --------------------------------------------------------------------------------

bool get_bool_hash_value(boolHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    boolNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return false; 
}
// --------------------------------------------------------------------------------

str* get_string_hash_value(stringHashTable* table, char* key) {
    size_t index = hash_function(key) % table->alloc;
    // Traverse the linked list at the index
    stringNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key found, return the corresponding value
            return current->value;
        }
        current = current->next;
    }

    return NULL; 
}
// ================================================================================
// ================================================================================

void free_char_hash_map(charHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        charNode* current = table->keyValues[i].next; // Start from the head of the list
        charNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_uchar_hash_map(ucharHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        ucharNode* current = table->keyValues[i].next; // Start from the head of the list
        ucharNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_short_hash_map(shortHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        shortNode* current = table->keyValues[i].next; // Start from the head of the list
        shortNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_ushort_hash_map(ushortHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        ushortNode* current = table->keyValues[i].next; // Start from the head of the list
        ushortNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_int_hash_map(intHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        intNode* current = table->keyValues[i].next; // Start from the head of the list
        intNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_uint_hash_map(uintHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        uintNode* current = table->keyValues[i].next; // Start from the head of the list
        uintNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_long_hash_map(longHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        longNode* current = table->keyValues[i].next; // Start from the head of the list
        longNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_ulong_hash_map(ulongHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        ulongNode* current = table->keyValues[i].next; // Start from the head of the list
        ulongNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_llong_hash_map(llongHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        llongNode* current = table->keyValues[i].next; // Start from the head of the list
        llongNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// -------------------------------------------------------------------------------- 

void free_ullong_hash_map(ullongHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        ullongNode* current = table->keyValues[i].next; // Start from the head of the list
        ullongNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_float_hash_map(floatHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        floatNode* current = table->keyValues[i].next; // Start from the head of the list
        floatNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_double_hash_map(doubleHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        doubleNode* current = table->keyValues[i].next; // Start from the head of the list
        doubleNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_ldouble_hash_map(ldoubleHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        ldoubleNode* current = table->keyValues[i].next; // Start from the head of the list
        ldoubleNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_bool_hash_map(boolHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        boolNode* current = table->keyValues[i].next; // Start from the head of the list
        boolNode* next = NULL;
        while (current) {      
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// --------------------------------------------------------------------------------

void free_string_hash_map(stringHashTable* table) {
    for (size_t i = 0; i < table->alloc; i++) {
        stringNode* current = table->keyValues[i].next; // Start from the head of the list
        stringNode* next = NULL;
        while (current) {      
            next = current->next;
            if (current->value)
                free_string(current->value);
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(table->keyValues); 
    free(table); 
}
// ================================================================================
// ================================================================================ 

void update_char_hash_map(charHashTable* table, char* key, char value) {
    size_t index = hash_function(key) % table->alloc;
    charNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_uchar_hash_map(ucharHashTable* table, char* key, unsigned char value) {
    size_t index = hash_function(key) % table->alloc;
    ucharNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_short_hash_map(shortHashTable* table, char* key, short int value) {
    size_t index = hash_function(key) % table->alloc;
    shortNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_ushort_hash_map(ushortHashTable* table, char* key, unsigned short int value) {
    size_t index = hash_function(key) % table->alloc;
    ushortNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_int_hash_map(intHashTable* table, char* key, int value) {
    size_t index = hash_function(key) % table->alloc;
    intNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_uint_hash_map(uintHashTable* table, char* key, unsigned int value) {
    size_t index = hash_function(key) % table->alloc;
    uintNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_long_hash_map(longHashTable* table, char* key, long int value) {
    size_t index = hash_function(key) % table->alloc;
    longNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_ulong_hash_map(ulongHashTable* table, char* key, unsigned long int value) {
    size_t index = hash_function(key) % table->alloc;
    ulongNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_llong_hash_map(llongHashTable* table, char* key, long long int value) {
    size_t index = hash_function(key) % table->alloc;
    llongNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_ullong_hash_map(ullongHashTable* table, char* key, unsigned long long int value) {
    size_t index = hash_function(key) % table->alloc;
    ullongNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_float_hash_map(floatHashTable* table, char* key, float value) {
    size_t index = hash_function(key) % table->alloc;
    floatNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_double_hash_map(doubleHashTable* table, char* key, double value) {
    size_t index = hash_function(key) % table->alloc;
    doubleNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_ldouble_hash_map(ldoubleHashTable* table, char* key, long double value) {
    size_t index = hash_function(key) % table->alloc;
    ldoubleNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_bool_hash_map(boolHashTable* table, char* key, bool value) {
    size_t index = hash_function(key) % table->alloc;
    boolNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// --------------------------------------------------------------------------------

void update_string_hash_map(stringHashTable* table, char* key, char* value) {
    size_t index = hash_function(key) % table->alloc;
    stringNode* current = table->keyValues[index].next;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            free_string(current->value);
            current->value = init_string_nol(value);
            return;
        }
        current = current->next;
    }
    // If key is not found, no action is taken
}
// ================================================================================
// ================================================================================

size_t char_hash_map_size(charHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t uchar_hash_map_size(ucharHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t short_hash_map_size(shortHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t ushort_hash_map_size(ushortHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t int_hash_map_size(intHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t uint_hash_map_size(uintHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t long_hash_map_size(longHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t ulong_hash_map_size(ulongHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t llong_hash_map_size(llongHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t ullong_hash_map_size(ullongHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t float_hash_map_size(floatHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t double_hash_map_size(doubleHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t ldouble_hash_map_size(ldoubleHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t bool_hash_map_size(boolHashTable* table) {
    return table->alloc;
}
// --------------------------------------------------------------------------------

size_t string_hash_map_size(stringHashTable* table) {
    return table->alloc;
}
// ================================================================================
// ================================================================================
// eof
