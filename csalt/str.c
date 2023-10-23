// ================================================================================
// ================================================================================
// - File:    str.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 20, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/str.h"
#include <string.h>


str* init_string_nol(const char *strlit) {
    // Allocate memory for the struct
    str *s = (str *)malloc(sizeof(str));
    if (!s) {
        int errnum = errno;
        fprintf(stderr, "Struct allocation error: %s\n", strerror(errnum));
        return NULL;
    }
    // Find the length of the string
    s->len = strlen(strlit);
    // Allocate memory for the data variable
    s->data = (char *)malloc(s->len + 1); // +1 for the null-terminator
    if (!s->data) {
        int errnum = errno;
        free(s); // Make sure to free previously allocated memory to avoid leaks
        fprintf(stderr, "String allocation error: %s\n", strerror(errnum));
        return NULL;
    }
    // Copy the string to the data variable
    strcpy(s->data, strlit);
    // Populate the alloc variable
    s->alloc = s->len + 1;
    return s;
}
// --------------------------------------------------------------------------------

str* init_string_len(char *strlit, size_t num) {
    size_t actual_len = strlen(strlit);
    size_t str_len = actual_len; // By default, set it to actual_len
    size_t alloc_len = actual_len + 1;

    if (num > actual_len) {
        alloc_len = num; // If num is greater, allocate that much
    }

    // Allocate memory for struct
    str* new_struct = (str*)malloc(sizeof(str));
    if (!new_struct) {
        int errnum = errno;
        fprintf(stderr, "Struct allocation error: %s\n", strerror(errnum));
        return NULL; 
    }

    // Allocate memory for string, accounting for the desired length
    char* new_data = malloc(sizeof(char) * alloc_len);
    if (!new_data) {
        int errnum = errno;
        free(new_struct);
        fprintf(stderr, "String allocation error: %s\n", strerror(errnum));
        return NULL;
    }

    // Copy the input string and null terminate it
    strncpy(new_data, strlit, str_len);
    new_data[str_len] = '\0';

    // Assign variables to struct
    new_struct->data = new_data;
    new_struct->len = str_len;
    new_struct->alloc = alloc_len;

    return new_struct;
}
// ================================================================================
// ================================================================================

char* get_string(str* str_struct) {
    if (str_struct == NULL) {
        fprintf(stderr, "get_string failed, empty struct\n");
        return NULL;
    }
    if (str_struct->data == NULL) {
        fprintf(stderr, "get_string failed, empty string\n");
        return NULL;
    }
    return str_struct->data;
}
// --------------------------------------------------------------------------------

size_t string_length(str* str_struct) {
    if (str_struct == NULL) {
        fprintf(stderr, "string_length failed, empty struct\n");
        return -1;
    }
    if (str_struct->data == NULL) {
        fprintf(stderr, "string_length failed, empty string\n");
        return -1;
    }
    return str_struct->len;
}
// ================================================================================
// ================================================================================

void free_string(str *str_struct) {
    if (str_struct->data != NULL)
        free(str_struct->data);
    if (str_struct != NULL)
        free(str_struct);
}
// --------------------------------------------------------------------------------

void cleanup_string(str **s) {
    if (s && *s) {
        free_string(*s);
    }
}
// --------------------------------------------------------------------------------

bool insert_string_lit(str *str_struct, char *string, size_t index) {
    if (!str_struct || !string || !str_struct->data) {
        fprintf(stderr, "Null pointer provided to insert_string_lit.\n");
        return false;
    }
    if (index > str_struct->len) {  // Allow insertion at the end by checking for > instead of >=
        fprintf(stderr, "String insert location out of bounds\n");
        return false;
    }
    size_t insert_len = strlen(string);
    size_t new_len = str_struct->len + insert_len;  // Length after insertion
    if (str_struct->alloc <= new_len) {  // Check if <= to ensure space for null terminator
        size_t new_alloc = new_len + 1;
        char *ptr = (char*)realloc(str_struct->data, new_alloc);
        if (ptr == NULL) {
            int errnum = errno;
            fprintf(stderr, "String Realloc Failed: %s\n", strerror(errnum));
            return false;
        }
        str_struct->data = ptr;
        str_struct->alloc = new_alloc;
    }
    memmove(str_struct->data + index + insert_len, 
            str_struct->data + index, 
            str_struct->len - index + 1);  // +1 to include null terminator
    memcpy(str_struct->data + index, string, insert_len);
    str_struct->len = new_len;  // Update the length
    return true;
}
// --------------------------------------------------------------------------------

bool insert_string_str(str *str_struct_one, str *str_struct_two, size_t index) {
    if (!str_struct_one || !str_struct_two || !str_struct_one->data || !str_struct_two->data) {
        fprintf(stderr, "Null pointer provided to insert_string_lit.\n");
        return false;
    }
    if (index > str_struct_one->len) {  // Allow insertion at the end by checking for > instead of >=
        fprintf(stderr, "String insert location out of bounds\n");
        return false;
    }
    size_t insert_len = str_struct_two->len;
    size_t new_len = str_struct_one->len + insert_len;  // Length after insertion
    if (str_struct_one->alloc <= new_len) {  // Check if <= to ensure space for null terminator
        size_t new_alloc = new_len + 1;
        char *ptr = (char*)realloc(str_struct_one->data, new_alloc);
        if (ptr == NULL) {
            int errnum = errno;
            fprintf(stderr, "String Realloc Failed: %s\n", strerror(errnum));
            return false;
        }
        str_struct_one->data = ptr;
        str_struct_one->alloc = new_alloc;
    }
    memmove(str_struct_one->data + index + insert_len, 
            str_struct_one->data + index, 
            str_struct_one->len - index + 1);  // +1 to include null terminator
    memcpy(str_struct_one->data + index, str_struct_two->data, insert_len);
    str_struct_one->len = new_len;  // Update the length
    return true;
}
// --------------------------------------------------------------------------------

bool trim_string(str *str_struct) {
    if (!str_struct || !str_struct->data) {
        fprintf(stderr, "Null pointer provided to trim_string\n");
        return false;
    }
    if (str_struct->len + 1 == str_struct->alloc)
        return true;
    if (str_struct->len + 1 > str_struct->alloc) {
        fprintf(stderr, "Missized string passed to trim_string\n");
        return false;
    }
    char *ptr = realloc(str_struct->data, str_struct->len + 1);
    if (ptr == NULL) {
        int errnum = errno;
        fprintf(stderr, "String allocation error: %s\n", strerror(errnum));
        return false;
    }
    str_struct->data = ptr;
    str_struct->alloc = str_struct->len + 1;
    return true;
}
// ================================================================================
// ================================================================================
// eof
