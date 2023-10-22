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


str* init_string_nol(const char *strlit) {
    // Allocate memory for the struct
    str *s = (str *)malloc(sizeof(str));
    if (!s) {
        fprintf(stderr, "Failed to allocate memory for the string struct\n");
        return NULL;
    }
    // Find the length of the string
    s->len = strlen(strlit);
    // Allocate memory for the data variable
    s->data = (char *)malloc(s->len + 1); // +1 for the null-terminator
    if (!s->data) {
        free(s); // Make sure to free previously allocated memory to avoid leaks
        fprintf(stderr, "Failed to allocate memory for string data\n");
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
        fprintf(stderr, "Struct Memory Allocation failed\n");
        return NULL; 
    }

    // Allocate memory for string, accounting for the desired length
    char* new_data = malloc(sizeof(char) * alloc_len);
    if (!new_data) {
        free(new_struct);
        fprintf(stderr, "Memory Allocation Failed\n");
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

char* get_string(str* string) {
    return string->data;
}
// --------------------------------------------------------------------------------

size_t string_length(str* string) {
    return string->len;
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
// ================================================================================
// ================================================================================
// eof
