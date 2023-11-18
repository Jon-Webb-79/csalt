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
// Include statements here

#include "include/str.h"
#include <string.h>
// ================================================================================
// ================================================================================
// CONSTANTS FOR MEMORY MANAGEMENT

const size_t STR_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
const size_t STR_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
// ================================================================================
// ================================================================================
// DEFINTIONS FOR BASIC STRING OPERATIONS

str* init_string_nol(const char* strlit) {
    if (strlit == NULL) {
        fprintf(stderr, "strlit is a NULL pointer in init_string_nol\n");
        // Dynamically allocate memory for a str object.
        str* a = malloc(sizeof(str));
        // Check if the allocation was successful.
        if (a == NULL) {
            fprintf(stderr, "Memory allocation failed in init_string_nol\n");
            return NULL;
        }
        // Initialize the members of the struct.
        a->data = NULL;
        a->len = 0;
        a->alloc = 0;
        return a;
    }
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
    if (strlit == NULL) {
        fprintf(stderr, "strlit is a NULL pointer in init_string_nol\n");
        // Dynamically allocate memory for a str object.
        str* a = malloc(sizeof(str));
        // Check if the allocation was successful.
        if (a == NULL) {
            fprintf(stderr, "Memory allocation failed in init_string_nol\n");
            return NULL;
        }
        // Initialize the members of the struct.
        a->data = NULL;
        a->len = 0;
        a->alloc = 0;
        return a;
    }
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
// --------------------------------------------------------------------------------

char* get_string(const str* str_struct) {
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

int string_length(str* str_struct) {
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
// --------------------------------------------------------------------------------

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

bool insert_string_lit(str* str_struct, const char* string, size_t index) {
    if ( !str_struct || !string ) {
        fprintf(stderr, "Null pointer provided to insert_string_lit.\n");
        return false;
    }
    if ( index > str_struct->len ) {  // Allow insertion at the end by checking for > instead of >=
        fprintf(stderr, "String insert location out of bounds\n");
        return false;
    }
    size_t insert_len = strlen(string);

    if ( str_struct->data == NULL ) {
        char* ptr = malloc(insert_len + 1);
        if ( ptr == NULL ) {
            fprintf(stderr, "Error: Malloc failed in insert_string_lit\n");
            return false;
        }
        str_struct->data = ptr;
        memcpy(str_struct->data, string, insert_len);
        str_struct->data[insert_len] = '\0';
        str_struct->len = insert_len;
        str_struct->alloc = insert_len + 1;
    }

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
    if (!str_struct_one || !str_struct_two || !str_struct_two->data) {
        fprintf(stderr, "Null pointer provided to insert_string_lit.\n");
        return false;
    }
    if (index > str_struct_one->len) {  // Allow insertion at the end by checking for > instead of >=
        fprintf(stderr, "String insert location out of bounds\n");
        return false;
    }
    size_t insert_len = str_struct_two->len;

    if ( str_struct_one->data == NULL ) {
        char* ptr = malloc(insert_len + 1);
        if ( ptr == NULL ) {
            fprintf(stderr, "Error: Malloc failed in insert_string_lit\n");
            return false;
        }
        str_struct_one->data = ptr;
        memcpy(str_struct_one->data, str_struct_two->data, insert_len);
        str_struct_one->data[insert_len] = '\0';
        str_struct_one->len = insert_len;
        str_struct_one->alloc = insert_len + 1;
    }

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
// ================================================================================
// ================================================================================

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
    // Allocate memory for the copy
    char *str_var = (char*)malloc(str_struct->len + 1);
    if (!str_var) {
        fprintf(stderr, "Memory allocation failed in copy_string.\n");
        return NULL;
    }
    //char str_var[str_struct->len + 1];
    memcpy(str_var, str_struct->data, str_struct->len + 1);
    str *one = init_string_len(str_var, str_struct->alloc);
    free(str_var);
    return one;
}
// ================================================================================
// ================================================================================

int compare_strings_lit(str* str_struct, char* string) {
    if (!str_struct || !string || !str_struct->data) {
        // Handle null pointers appropriately
        fprintf(stderr, "Null pointer provided to compare_strings_lit.\n");
        return INT_MIN; // Or another designated error value
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
        return INT_MIN; // Or another designated error value
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

char* first_char(char c, char* min_ptr, char* max_ptr) {
    if (min_ptr > max_ptr) {
        fprintf(stderr, "min_ptr is not smaller than max_ptr in first_char\n");
        return NULL;
    }
    for (char* it = min_ptr; it != max_ptr; it++) {
        if (*it == c )
            return it;
    }
    return NULL;
}
// --------------------------------------------------------------------------------

char* last_char(char c, char* min_ptr, char* max_ptr) {
    if (min_ptr > max_ptr) {
        fprintf(stderr, "min_ptr is not smaller than max_ptr in last_char\n");
        return NULL;
    }
    for (char* it = max_ptr; it != min_ptr; it--) {
        if (*it == c )
            return it;
    }
    return NULL;
}
// ================================================================================
// ================================================================================

char* first_literal_between_ptrs(char* string, char* min_ptr, char* max_ptr) {
    if (!string) {
        fprintf(stderr, "Null string provided to first_literal_between_ptrs\n");
        return NULL;
    }
    if (min_ptr >= max_ptr) {
        fprintf(stderr, "Min pointer larger than max pointer in first_literal_between_ptrs\n");
        return NULL;
    }
    size_t str_len = strlen(string);
    size_t j;

    for (char* it = min_ptr; it < max_ptr - str_len + 1; it++) {
        for (j = 0; j < str_len; j++) {
            if (string[j] != *(it + j)) {
                break;
            }
        }
        if (j == str_len) return it;
    }
    return NULL; 
}
// --------------------------------------------------------------------------------

char* first_str_between_ptrs(str* string, char* min_ptr, char* max_ptr) {
    if (!string || !string->data) {
        fprintf(stderr, "Null struct information provided for first_str_between_ptrs\n");
        return NULL;
    }
    if (min_ptr >= max_ptr) {
        fprintf(stderr, "Min pointer larger than max pointer in first_str_between_ptrs\n");
        return NULL;
    }
    size_t str_len = string->len;
    size_t j;

    for (char* it = min_ptr; it < max_ptr - str_len + 1; it++) { // note the change in the condition
        for (j = 0; j < str_len; j++) {
            if (string->data[j] != *(it + j)) {
                break;
            }
        }
        if (j == str_len) return it;
    }
    return NULL; 
}
// --------------------------------------------------------------------------------

char* last_literal_between_ptrs(char* string, char* min_ptr, char* max_ptr) {
    if (!string) {
        fprintf(stderr, "Null string provided for first_str_between_ptrs\n");
        return NULL;
    }
    if (min_ptr >= max_ptr) {
        fprintf(stderr, "Min pointer larger than max pointer in first_str_between_ptrs\n");
        return NULL;
    }
    if (min_ptr >= max_ptr) return NULL;
    size_t str_len = strlen(string);
    size_t j;

    for (char* it = max_ptr - str_len; it >= min_ptr; it--) { 
        for (j = 0; j < str_len; j++) {
            if (string[j] != *(it + j)) {
                break;
            }
        }
        if (j == str_len) return it;
    }
    return NULL;
}
// --------------------------------------------------------------------------------

char* last_str_between_ptrs(str* string, char* min_ptr, char* max_ptr) {
    if (!string || !string->data) {
        fprintf(stderr, "Null struct information provided for first_str_between_ptrs\n");
        return NULL;
    }
    if (min_ptr >= max_ptr) {
        fprintf(stderr, "Min pointer larger than max pointer in first_str_between_ptrs\n");
        return NULL;
    }
    size_t str_len = string->len;
    size_t j;

    for (char* it = max_ptr - str_len; it >= min_ptr; it--) { 
        for (j = 0; j < str_len; j++) {
            if (string->data[j] != *(it + j)) {
                break;
            }
        }
        if (j == str_len) return it;
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
    if (!str_struct || !str_struct->data || !ptr) {
        fprintf(stderr, "Invalid NULL parameter(s) provided to ptr_in_str_container\n");
        return false; // Return false or handle the error as appropriate.
    }

    if (ptr >= str_struct->data && ptr < str_struct->data + str_struct->len) {
        return true;
    }
    return false;
}
// --------------------------------------------------------------------------------

bool ptr_in_literal(char* string, char* ptr) {
    if (!string || !ptr) {
        fprintf(stderr, "Invalid NULL parameter(s) provided to ptr_in_literal\n");
        return false; // Return false or handle the error as appropriate.
    }

    // Calculate the end of the string correctly.
    char* max_ptr = string + strlen(string);
    if (ptr >= string && ptr < max_ptr) {
        return true;
    }
    return false;
}
// --------------------------------------------------------------------------------

void dec_str_iter(char* begin, char* end,
                  iter_dir direction, str_decorator decorator) {
    if ( decorator == NULL) {
        fprintf(stderr, "Function pointer in dec_str_iter points to NULL\n");
    }
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
    if(!s || !s->data) {
        fprintf(stderr, "Null pointer provided to to_uppercase\n");
        return;
    }
    char* begin = s->data;
    char* end = s->data + s->len;
    for (char* i =  begin; i != end; i++) {
        if (*i >= 'a' && *i <= 'z') *i -= 32;
    }
}
// --------------------------------------------------------------------------------

void to_lowercase(str *s) {
    if(!s || !s->data) {
        fprintf(stderr, "Null pointer provided to to_lowercase\n");
        return;
    }
    char* begin = s->data;
    char* end = s->data + s->len;
    for (char* i =  begin; i != end; i++) {
        if (*i >= 'A' && *i <= 'Z') *i += 32;
    }
}
// ================================================================================
// ================================================================================

bool drop_literal_substring(str* string, char* substring, char* min_ptr, char* max_ptr) {
    if (!string || !substring || !string->data) {
        fprintf(stderr, "Null data provided to drop_str_substring\n");
        return false;
    }

    // Check if min_ptr and max_ptr are within the string's data bounds.
    if (min_ptr < string->data || max_ptr > string->data + string->len) {
        fprintf(stderr, "Pointers out of bounds in drop_literaL_substring\n");
        return false;
    }

    size_t substr_len = strlen(substring);
    if (string->len < substr_len) return true;

    char* begin = min_ptr;
    char* end = max_ptr;
    char* ptr = last_literal_between_ptrs(substring, begin, end);
    size_t move_length;

    while ((ptr = last_literal_between_ptrs(substring, begin, end))) {
        move_length = end - ptr - (substr_len - 1);

        // Check if the character after the substring is a space
        if (*(ptr + substr_len) == ' ') {
            move_length--;  // Move one less character
            substr_len++;  // Include the space in the substring to be dropped
        }

        memmove(ptr, ptr + substr_len, move_length);
        end -= substr_len;
        string->len -= substr_len;

        // Null-terminate the string at the new end
        *(string->data + string->len) = '\0';

        // Reset the substring length for the next iteration 
        substr_len = strlen(substring);
    }
    return true;
}
// --------------------------------------------------------------------------------

bool drop_str_substring(str* string, str* substring, char* min_ptr, char* max_ptr) {
    if (!string || !substring || !string->data || !substring->data) {
        fprintf(stderr, "Null data provided to drop_str_substring\n");
        return false;
    }

    // Check if min_ptr and max_ptr are within the string's data bounds.
    if (min_ptr < string->data || max_ptr > string->data + string->len) {
        fprintf(stderr, "Pointers out of bounds in DROP_str_substring\n");
        return false;
    }

    size_t substr_len = substring->len;
    if (string->len < substr_len) return true;

    char* begin = min_ptr;
    char* end = max_ptr;
    char* ptr = last_str_between_ptrs(substring, begin, end);
    size_t move_length;

    while ((ptr = last_str_between_ptrs(substring, begin, end))) {
        move_length = end - ptr - (substr_len - 1);

        // Check if the character after the substring is a space
        if (*(ptr + substr_len) == ' ') {
            move_length--;  // Move one less character
            substr_len++;  // Include the space in the substring to be dropped
        }

        memmove(ptr, ptr + substr_len, move_length);
        end -= substr_len;
        string->len -= substr_len;

        // Null-terminate the string at the new end
        *(string->data + string->len) = '\0';

        // Reset the substring length for the next iteration 
        substr_len = string_length(substring);
    } 
    return true;
}
// ================================================================================
// ================================================================================

bool replace_str_substring(str* string, str* pattern, str* replace_string,
                           char* min_ptr, char* max_ptr) {
    if (!string || !string->data || !pattern || !pattern->data || 
        !replace_string || !replace_string->data || !min_ptr || !max_ptr) {
        fprintf(stderr, "Null data provided to replace_str_substring\n");
        return false;
    }
    
    // Check if min_ptr and max_ptr are within the string's data bounds.
    if (min_ptr < string->data || max_ptr > string->data + string->len) {
        fprintf(stderr, "Pointers out of bounds in replace_str_substring\n");
        return false;
    }
    
    // Calculate the delta between the pattern and replacement lengths.
    int delta = replace_string->len - pattern->len;

    char* ptr = last_str_between_ptrs(pattern, min_ptr, max_ptr);

    while (ptr) {
        // If the replacement string is the same length, copy it over.
        if (delta == 0) {
            memcpy(ptr, replace_string->data, replace_string->len);
            max_ptr = ptr;
        }

        // - If replacement string is smaller, copy string, move memory to the 
        //   left, and update length
        else if (delta < 0) {
            memcpy(ptr, replace_string->data, replace_string->len);

            size_t tail_length = string->data + string->len - (ptr + pattern->len);
            memmove(ptr + replace_string->len, ptr + pattern->len, tail_length);

            string->len += delta; // delta is negative, so it reduces string->len

            string->data[string->len] = '\0';

            max_ptr -= pattern->len - replace_string->len;
             string->data[string->len] = '\0';
        }
        else {
            size_t min_pos = min_ptr - string->data;
            size_t max_pos = max_ptr - string->data;
            size_t ptr_pos = ptr - string->data;

            // Calculate the new total memory required including the null terminator.
            size_t new_memory_required = string->len + delta + 1; 
            
            if (new_memory_required > string->alloc) {
                char* new_data = realloc(string->data, new_memory_required);
                if (new_data == NULL) {
                    fprintf(stderr, "Realloc failed in replace_str_substring\n");
                    return false;
                }

                // After realloc, the memory may have been moved. Update string->data and ptr.
                string->data = new_data;
                string->alloc = new_memory_required;
                min_ptr = string->data + min_pos;
                max_ptr = string->data + max_pos;
                ptr = string->data + ptr_pos;
            }
            memmove(ptr + replace_string->len,
                    ptr + pattern->len,
                    string->len - (ptr - string->data) - pattern->len);
            memcpy(ptr, replace_string->data, replace_string->len);

            string->len += delta;
            string->data[string->len] = '\0';
            max_ptr = ptr;
        }        
         // Find the next pattern instance within the updated pointer bounds.
         if (min_ptr < max_ptr)
            ptr = last_str_between_ptrs(pattern, min_ptr, max_ptr);
         else
            ptr = NULL;
    }

    return true;
}
// --------------------------------------------------------------------------------

bool replace_literal_substring(str* string, char* pattern, char* replace_string,
                               char* min_ptr, char* max_ptr) {
    if (!string || !string->data || !pattern ||  !replace_string || 
        !min_ptr || !max_ptr) {
        fprintf(stderr, "Null data provided to replace_str_substring\n");
        return false;
    }
    
    // Check if min_ptr and max_ptr are within the string's data bounds.
    if (min_ptr < string->data || max_ptr > string->data + string->len) {
        fprintf(stderr, "Pointers out of bounds in replace_str_substring\n");
        return false;
    }
    
    // Calculate the delta between the pattern and replacement lengths.
    int delta = strlen(replace_string) - strlen(pattern);

    char* ptr = last_literal_between_ptrs(pattern, min_ptr, max_ptr);
    size_t pattern_len = strlen(pattern);
    size_t replace_len = strlen(replace_string);

    while (ptr) {
        // If the replacement string is the same length, copy it over.
        if (delta == 0) {
            memcpy(ptr, replace_string, replace_len);
            max_ptr = ptr;
        }

        // - If replacement string is smaller, copy string, move memory to the 
        //   left, and update length
        else if (delta < 0) {
            memcpy(ptr, replace_string, replace_len);

            size_t tail_length = string->data + string->len - (ptr + pattern_len);
            memmove(ptr + replace_len, ptr + pattern_len, tail_length);

            string->len += delta; // delta is negative, so it reduces string->len

            string->data[string->len] = '\0';

            max_ptr -= pattern_len - replace_len;
            string->data[string->len] = '\0';
        }
        else {
            size_t min_pos = min_ptr - string->data;
            size_t max_pos = max_ptr - string->data;
            size_t ptr_pos = ptr - string->data;

            // Calculate the new total memory required including the null terminator.
            size_t new_memory_required = string->len + delta + 1; 
            
            if (new_memory_required > string->alloc) {
                char* new_data = realloc(string->data, new_memory_required);
                if (new_data == NULL) {
                    fprintf(stderr, "Realloc failed in replace_str_substring\n");
                    return false;
                }

                // After realloc, the memory may have been moved. Update string->data and ptr.
                string->data = new_data;
                string->alloc = new_memory_required;
                min_ptr = string->data + min_pos;
                max_ptr = string->data + max_pos;
                ptr = string->data + ptr_pos;
            }
            memmove(ptr + replace_len,
                    ptr + pattern_len,
                    string->len - (ptr - string->data) - pattern_len);
            memcpy(ptr, replace_string, replace_len);

            string->len += delta;
            string->data[string->len] = '\0';
            max_ptr = ptr;
        }        
         // Find the next pattern instance within the updated pointer bounds.
         if (min_ptr < max_ptr)
            ptr = last_literal_between_ptrs(pattern, min_ptr, max_ptr);
         else
            ptr = NULL;
    }

    return true;
}
// ================================================================================
// ================================================================================
// eof
