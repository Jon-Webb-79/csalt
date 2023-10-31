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

const size_t STR_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
const size_t STR_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
                                                    
str* init_string_nol(const char* strlit) {
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
    s->data[s->len] = '\0';
    // Populate the alloc variable
    s->alloc = s->len + 1;

    return s;
}
// --------------------------------------------------------------------------------

str* init_string_len(char* strlit, size_t num) {
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
// --------------------------------------------------------------------------------

size_t string_memory(str* str_struct) {
    if (str_struct == NULL) {
        fprintf(stderr, "string_length failed, empty struct\n");
        return -1;
    }
    if (str_struct->data == NULL) {
        fprintf(stderr, "string_length failed, empty string\n");
        return -1;
    }
    return str_struct->alloc;
}
// ================================================================================
// ================================================================================

void _free_string(str** str_struct_ptr) {
    if (!str_struct_ptr || !*str_struct_ptr) return;

    if ((*str_struct_ptr)->data) {
        free((*str_struct_ptr)->data);
        (*str_struct_ptr)->data = NULL; 
    }
    
    free(*str_struct_ptr);
    *str_struct_ptr = NULL; 
}
// --------------------------------------------------------------------------------

bool insert_string_lit(str* str_struct, char* string, size_t index) {
    if (!str_struct || !string || !str_struct->data) {
        fprintf(stderr, "Null pointer provided to insert_string_lit.\n");
        return false;
    }
    if (index > str_struct->len) {  // Allow insertion at the end by checking for > instead of >=
        fprintf(stderr, "String insert location out of bounds\n");
        return false;
    }
    size_t insert_len = strlen(string);
    size_t new_len = str_struct->len + insert_len;  
    if (str_struct->alloc <= new_len) {  
        size_t new_alloc;
        if (str_struct->alloc < STR_THRESHOLD) {
            // Geometric Growth
            new_alloc = (new_len * 2) + 1;
        } else {
            // Linear Growth
            new_alloc = str_struct->alloc + STR_FIXED_AMOUNT;
            if (new_alloc < new_len + 1) {
                new_alloc = new_len + 1; // ensure it's big enough for current request
            }
        }

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

bool insert_string_str(str* str_struct_one, str* str_struct_two, size_t index) {
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

    if (str_struct_one->alloc <= new_len) {  
        size_t new_alloc;
        if (str_struct_one->alloc < STR_THRESHOLD) {
            // Geometric Growth
            new_alloc = (new_len * 2) + 1;
        } else {
            // Linear Growth
            new_alloc = str_struct_one->alloc + STR_FIXED_AMOUNT;
            if (new_alloc < new_len + 1) {
                new_alloc = new_len + 1; // ensure it's big enough for current request
            }
        }

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

bool trim_string(str* str_struct) {
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
// --------------------------------------------------------------------------------

str* copy_string(str* str_struct) {
    if (!str_struct || !str_struct->data) {
        // Handle null pointers appropriately
        fprintf(stderr, "Null pointer provided to copy_string_wo_gbc.\n");
        return 0; // Or another designated error value
    }
    char str_var[str_struct->len + 1];
    memcpy(str_var, str_struct->data, str_struct->len + 1);
    str *one = init_string_len(str_var, str_struct->alloc);
    return one;
}
// ================================================================================
// ================================================================================

int compare_strings_lit(str* str_struct, char* string) {
    if (!str_struct || !string || !str_struct->data) {
        // Handle null pointers appropriately
        fprintf(stderr, "Null pointer provided to compare_strings_lit.\n");
        return 0; // Or another designated error value
    }

    size_t string_len = strlen(string);
    size_t min_len = (str_struct->len < string_len) ? str_struct->len : string_len;

    for (size_t i = 0; i < min_len; i++) {
        if (str_struct->data[i] != string[i]) {
            return (unsigned char)str_struct->data[i] - (unsigned char)string[i];
        }
    }
    return str_struct->len - string_len;
}
// --------------------------------------------------------------------------------

int compare_strings_str(str* str_struct_one, str* str_struct_two) {
    if (!str_struct_one || !str_struct_two || !str_struct_one->data || !str_struct_two->data) {
        // Handle null pointers appropriately
        fprintf(stderr, "Null pointer provided to compare_strings_str.\n");
        return 0; // Or another designated error value
    } 

    size_t string_len = str_struct_two->len;
    size_t min_len = (str_struct_one->len < string_len) ? str_struct_one->len : string_len;

    for (size_t i = 0; i < min_len; i++) {
        if (str_struct_one->data[i] != str_struct_two->data[i]) {
            return (unsigned char)str_struct_one->data[i] - (unsigned char)str_struct_two->data[i];
        }
    }
    return str_struct_one->len - string_len;
}
// ================================================================================
// ================================================================================

char* find_first_char(str* str_struct, char c) {
    if ( !str_struct || !str_struct->data) {
        fprintf(stderr, "Null pointer provided to find_char\n");
        return NULL;
    }
    for (size_t i = 0; i <= str_struct->len; i++ ) {
        if (str_struct->data[i] == c)
            return str_struct->data + i;
    }
    return NULL;
}
// --------------------------------------------------------------------------------

char* first_char_btw_ptrs(char c, char* min_ptr, char* max_ptr) {
    if (min_ptr > max_ptr) {
        fprintf(stderr, "min_ptr is not smaller than max_ptr in first_char_btw_ptrs\n");
        return NULL;
    }
    for (char* it = min_ptr; it != max_ptr; it++) {
        if (*it == c )
            return it;
    }
    return NULL;
}
// --------------------------------------------------------------------------------

char* last_char_btw_ptrs(char c, char* min_ptr, char* max_ptr) {
    if (min_ptr > max_ptr) {
        fprintf(stderr, "min_ptr is not smaller than max_ptr in last_char_btw_ptrs\n");
        return NULL;
    }
    for (char* it = max_ptr; it != min_ptr; it--) {
        if (*it == c )
            return it;
    }
    return NULL;
}
// --------------------------------------------------------------------------------

char* find_last_char(str* str_struct, char c) {
    if (!str_struct || !str_struct->data) {
        fprintf(stderr, "Null pointer provided to find_char\n");
        return NULL;
    }
    // Note that we need to handle the case when str_struct->len is 0
    if (str_struct->len == 0) {
        return NULL;
    }
    for (int i = str_struct->len; i >= 0; i--) {
        if (str_struct->data[i] == c)
            return str_struct->data + i;
    }
    return NULL;
}
// ================================================================================
// ================================================================================

char* find_first_lit_strstr(str* str_struct, char* string) {
    if (!str_struct || !str_struct->data || !string) {
        fprintf(stderr, "Null pointer provided to find_first_lin_strstr\n");
        return NULL;
    }
    size_t str_len = strlen(string);

    if (str_len > str_struct->len)
        return NULL;

    size_t j;
    for (size_t i = 0; i <= str_struct->len - str_len; i++) {
        for (j = 0; j < str_len; j++) {
           if (string[j] != str_struct->data[i + j])
               break;
        }
        if (j == str_len) return str_struct->data + i;
    }
    return NULL;
}
// --------------------------------------------------------------------------------

char* find_first_str_strstr(str* str_struct_one, str* str_struct_two) {
    if (!str_struct_one || !str_struct_two || !str_struct_one->data || !str_struct_two->data) {
        // Handle null pointers appropriately
        fprintf(stderr, "Null pointer provided to find_first_str_strstr.\n");
        return 0; // Or another designated error value
    } 
    size_t str_len = str_struct_two->len;

    if (str_len > str_struct_one->len)
        return NULL;

    size_t j;
    for (size_t i = 0; i <= str_struct_one->len - str_len; i++) {
        for (j = 0; j < str_len; j++) {
           if (str_struct_two->data[j] != str_struct_one->data[i + j])
               break;
        }
        if (j == str_len) return str_struct_one->data + i;
    }
    return NULL; 
}
// --------------------------------------------------------------------------------

char* find_last_lit_strstr(str* str_struct, char* string) {
    if (!str_struct || !str_struct->data || !string) {
        fprintf(stderr, "Null pointer provided to find_first_lin_strstr\n");
        return NULL;
    }
    size_t str_len = strlen(string);

    if (str_len > str_struct->len)
        return NULL;

    size_t j;
    for (int i = str_struct->len - str_len; i >= 0; i--) {
        for (j = 0; j < str_len; j++) {
           if (string[j] != str_struct->data[i + j])
               break;
        }
        if (j == str_len) return str_struct->data + i;
    }
    return NULL;
}
// --------------------------------------------------------------------------------

char* find_last_str_strstr(str* str_struct_one, str* str_struct_two) {
    if (!str_struct_one || !str_struct_two || !str_struct_one->data || !str_struct_two->data) {
        // Handle null pointers appropriately
        fprintf(stderr, "Null pointer provided to find_first_str_strstr.\n");
        return 0; // Or another designated error value
    } 
    size_t str_len = str_struct_two->len;

    if (str_len > str_struct_one->len)
        return NULL;

    size_t j;
    for (int i = str_struct_one->len - str_len; i >= 0; i--) {
        for (j = 0; j < str_len; j++) {
           if (str_struct_two->data[j] != str_struct_one->data[i + j])
               break;
        }
        if (j == str_len) return str_struct_one->data + i;
    }
    return NULL; 
}
// ================================================================================
// ================================================================================

char pop_str_char(str* str_struct) {
    if ( !str_struct || !str_struct->data) {
        fprintf(stderr, "Null pointer provided to pop_str_char\n");
        return '\0';
    }
    if (str_struct->len == 0) return '\0';

    size_t index = str_struct->len - 1;
    char val = str_struct->data[index];
    // Move the null terminator 1 char to the right
    memmove(str_struct->data + index,
            str_struct->data + index + 1,
            1);
    str_struct->len = str_struct->len - 1;
    return val;
}
// --------------------------------------------------------------------------------

char pop_str_char_index(str* str_struct, size_t index) {
    if ( !str_struct || !str_struct->data) {
        fprintf(stderr, "Null pointer provided to pop_str_char_index\n");
        return '\0';
    }
    if (str_struct->len == 0) return '\0';

    if (index < 0 || index > str_struct->len) {
        fprintf(stderr, "Out of bounds index in pop_str_char_index\n");
        return '\0';
    }
    char val = str_struct->data[index];
    // Move the all data 1 char to the right
    memmove(str_struct->data + index,
            str_struct->data + index + 1,
            str_struct->len - index);
    str_struct->len = str_struct->len - 1;
    return val;
}
// ================================================================================
// ================================================================================

str* pop_string_token(str* str_struct, char token) {
    if (!str_struct || !str_struct->data) {
        fprintf(stderr, "Null pointer provided to pop_string_token\n");
        return NULL;
    }
    if (str_struct->len == 0) {
        return NULL;
    }
    for (int i = str_struct->len - 1; i >= 0; i--) {
        if (str_struct->data[i] == token) {
            str *one = init_string(str_struct->data + (i + 1));
            
            // Set the null terminator after the token
            str_struct->data[i] = '\0';
            
            // Update the length of str_struct
            str_struct->len = i;

            return one;
        }
    }
    return NULL;
}
// ================================================================================
// ================================================================================
// PRIVATE FUNCTIONS

static char* _str_begin(str* s) {
    if (!s || !s->data) {
        fprintf(stderr, "Null pointer provided to dec_str_iter\n");
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static char* _str_end(str* s) {
    if (!s || !s->data) {
        fprintf(stderr, "Null pointer provided to dec_str_iter\n");
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _str_next(char** current) {
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _str_prev(char** current) {
    (*current)--;
}
// --------------------------------------------------------------------------------

static char _str_get(char** current) {
    return **current;
}

// --------------------------------------------------------------------------------
// PUBLIC FUNCTIONS

bool ptr_in_str_container(str* str_struct, char* ptr) {
    if (ptr >= str_struct->data && ptr <= str_struct->data + str_struct->len) {
        return true;
    }
    return false;
}
// --------------------------------------------------------------------------------

bool ptr_in_literal(char* ptr, char* min_ptr, char* max_ptr) {
    if (ptr >= min_ptr && ptr <= max_ptr) {
        return true;
    }
    return false;
}
// --------------------------------------------------------------------------------

void dec_str_iter(char* begin, char* end,
                  str_iter_dir direction, str_decorator decorator) {
    // Check the direction of iteration and validate iterators
    if (direction == FORWARD && end < begin) {
        fprintf(stderr, "Error: 'end' iterator should be after 'begin' for FORWARD iteration.\n");
        return;
    }
    if (direction == REVERSE && end > begin) {
        fprintf(stderr, "Error: 'end' iterator should be before 'begin' for REVERSE iteration.\n");
        return;
    }
    char* it = begin;
    while ( it != end ) {
        decorator(it);
        if ( direction == FORWARD)
            _str_next(&it);
        else 
            _str_prev(&it);
    }
}
// --------------------------------------------------------------------------------

str_iterator init_str_iterator() {
    str_iterator iter;

    iter.begin = _str_begin;
    iter.end = _str_end;
    iter.next = _str_next;
    iter.prev = _str_prev;
    iter.get = _str_get;
    return iter;
}
// ================================================================================
// ================================================================================

void to_uppercase(str *s) {
    char* begin = s->data;
    char* end = s->data + s->len;
    for (char* i =  begin; i != end; i++) {
        if (*i >= 'a' && *i <= 'z') *i -= 32;
    }
}
// --------------------------------------------------------------------------------

void to_lowercase(str *s) {
    char* begin = s->data;
    char* end = s->data + s->len;
    for (char* i =  begin; i != end; i++) {
        if (*i >= 'A' && *i <= 'Z') *i += 32;
    }
}
// ================================================================================
// ================================================================================
// eof
