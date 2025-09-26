// ================================================================================
// ================================================================================
// - File:    c_string.c
// - purpose: this file contains the implementation for functions used in the 
//            c_string library
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 30, 2024
// - Version: 0.1
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_string.h"

#include <errno.h>  // For errno and strerror 
#include <stdlib.h> // For size_t, malloc, and realloc
#include <string.h> // For strerror
#include <limits.h> // For INT_MIN
#include <ctype.h>  // For isspace
#include <stdint.h> // For SIZE_MAX
// ================================================================================ 
// ================================================================================

static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
// ================================================================================ 
// ================================================================================ 
// STRING_T DATA TYPE 

struct string_t {
    char* str;
    size_t len;
    size_t alloc;
    ErrorCode error;
};
// ================================================================================ 
// ================================================================================ 
// PRIVATE FUNCTIONS

static char* _last_literal_between_ptrs(const char* string, char* min_ptr, char* max_ptr) {
    if (!string) {
        fprintf(stderr, "ERROR: Null string provided for last_literal_between_ptrs\n");
        return NULL;
    }
    if (min_ptr >= max_ptr) {
        fprintf(stderr, "ERROR: Min pointer larger than max pointer in last_literal_between_ptrs\n");
        return NULL;
    }
    
    size_t str_len = strlen(string);
    if (str_len == 0) return NULL;
    
    // Ensure we don't search past the end
    char* search_start = max_ptr - str_len + 1;
    if (search_start < min_ptr) return NULL;
    
    for (char* it = search_start; it >= min_ptr; it--) {
        size_t j;
        for (j = 0; j < str_len; j++) {
            if (string[j] != *(it + j)) {
                break;
            }
        }
        if (j == str_len) return it;
    }
    return NULL;
}
// ================================================================================ 
// ================================================================================ 
// --------------------------------------------------------------------------------

string_t* init_string(const char* str) {
    if (str == NULL) {
        errno = EINVAL;
        return NULL;
    }

    // Typically SIMD-optimized in libc
    size_t len = strlen(str);

    // Guard against pathological overflow on len+1
    if (len == SIZE_MAX) { errno = EOVERFLOW; return NULL; }

    string_t* ptr = malloc(sizeof(string_t));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }

    char* ptr2 = malloc(len + 1);
    if (!ptr2) {
        errno = ENOMEM;
        free(ptr);
        return NULL;
    }

    memcpy(ptr2, str, len);
    ptr2[len] = '\0';
    
    ptr->str = ptr2;
    ptr->len = len;
    ptr->alloc = len + 1;
    ptr->error = NO_ERROR;
    return ptr;
}
// -------------------------------------------------------------------------------- 

ErrorCode get_string_error(const string_t* str) {
    if (!str) {
        errno = EINVAL;
    }
    return str->error;
}
// --------------------------------------------------------------------------------

void free_string(string_t* str) {
    if (!str) {
        errno = EINVAL;
        return;
    }
    if (str->str) {
        free(str->str);
    }
    free(str);
}
// --------------------------------------------------------------------------------

void _free_string(string_t** str) {
    if (str && *str) {
        free_string(*str);
        *str = NULL;
    }
}
// --------------------------------------------------------------------------------

const char* get_string(const string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return NULL;
    }
    return str->str;
}
// --------------------------------------------------------------------------------

size_t string_size(const string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return str->len;
}
// --------------------------------------------------------------------------------

size_t string_alloc(const string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return str->alloc;
}
// --------------------------------------------------------------------------------

bool string_string_concat(string_t* str1, const string_t* str2) {
    if (!str1 || !str2) {
        errno = EINVAL;
        if (str1) str1->error = INVALID_ARG;
        return false;
    }
    // Reset error code 
    str1->error = NO_ERROR;
    if (!str1->str) {
        errno = EINVAL;
        str1->error = NULL_POINTER;
        return false;
    }
    if (!str2->str) {
        errno = EINVAL;
        str1->error = NULL_POINTER;
        /* str2 is const; don't mutate it */
        return false;
    }

    const size_t len1 = str1->len;
    const size_t len2 = str2->len;

    /* size_t overflow guard for len1 + len2 + 1 */
    if (len2 > SIZE_MAX - 1 - len1) {
        errno = EOVERFLOW;
        str1->error = NUMERIC_OVERFLOW;
        return false;
    }
    const size_t needed = len1 + len2 + 1;

    /* Detect if src lies inside str1->str (overlap before a potential realloc) */
    const char* src_initial = str2->str;
    const char* s1_begin    = str1->str;
    const char* s1_end_used = str1->str + len1 + 1; /* include NUL */

    const bool src_overlaps_dest = (src_initial >= s1_begin && src_initial < s1_end_used);
    char* temp = NULL;

    /* If we must grow AND the source overlaps str1's buffer, stash a temp copy first */
    if (needed > str1->alloc && src_overlaps_dest) {
        temp = (char*)malloc(len2);
        if (!temp) {
            errno = ENOMEM;
            str1->error = BAD_ALLOC;
            return false;
        }
        memcpy(temp, src_initial, len2);
        src_initial = temp; /* use temp as the copy source after realloc */
    }

    /* Ensure capacity */
    if (needed > str1->alloc) {
        char* newbuf = (char*)realloc(str1->str, needed);
        if (!newbuf) {
            free(temp);
            errno = ENOMEM;
            str1->error = REALLOC_FAIL;
            return false;
        }
        str1->str   = newbuf;
        str1->alloc = needed;
    }

    /* If str1 == str2, the source is the start of the (possibly moved) buffer */
    if (str1 == str2) {
        src_initial = str1->str;
    }

    /* Append safely; memmove handles any overlap */
    memmove(str1->str + len1, src_initial, len2);
    str1->str[len1 + len2] = '\0';

    str1->len   = len1 + len2;
    str1->error = NO_ERROR;

    free(temp);
    return true;
}
// --------------------------------------------------------------------------------

bool string_lit_concat(string_t* str1, const char* literal) {
    if (!str1 || !literal) {
        if (str1) str1->error = INVALID_ARG;
        errno = EINVAL;
        return false;
    }
    // Reset error code 
    str1->error = NO_ERROR;
    if (!str1->str) {
        str1->error = NULL_POINTER;
        errno = EINVAL;
        return false;
    }

    const size_t len1 = str1->len;
    const size_t len2 = strlen(literal);

    // Fast path: nothing to append
    if (len2 == 0) {
        str1->error = NO_ERROR;
        return true;
    }

    // Guard against size_t overflow in len1 + len2 + 1
    if (len2 > SIZE_MAX - 1 - len1) {
        str1->error = NUMERIC_OVERFLOW;
        errno = EOVERFLOW;
        return false;
    }
    const size_t needed = len1 + len2 + 1;

    // Does the source pointer lie inside str1->str's *current used* region?
    const char* s1_begin    = str1->str;
    const char* s1_end_used = str1->str + len1 + 1;  // include current NUL
    const bool   overlaps   = (literal >= s1_begin && literal < s1_end_used);

    // If we must grow and the source aliases str1->str, stash a temp copy first
    const char* src = literal;
    char* temp = NULL;
    if (needed > str1->alloc && overlaps) {
        temp = (char*)malloc(len2);
        if (!temp) {
            str1->error = BAD_ALLOC;
            errno = ENOMEM;
            return false;
        }
        memcpy(temp, literal, len2);
        src = temp;  // safe to use after realloc
    }

    // Ensure capacity
    if (needed > str1->alloc) {
        char* newbuf = (char*)realloc(str1->str, needed);
        if (!newbuf) {
            free(temp);
            str1->error = REALLOC_FAIL;
            errno = ENOMEM;
            return false;
        }
        str1->str   = newbuf;
        str1->alloc = needed;
    }

    // Append safely; memmove handles any overlap
    memmove(str1->str + len1, src, len2);
    str1->str[len1 + len2] = '\0';

    str1->len   = len1 + len2;
    str1->error = NO_ERROR;

    free(temp);
    return true;
}
// --------------------------------------------------------------------------------

int compare_strings_lit(const string_t* str_struct, const char* string) {
    if (!str_struct || !string || !str_struct->str) {
        errno = EINVAL;
        return INT_MIN; // Or another designated error value
    }

    size_t string_len = strlen(string);
    size_t min_len = (str_struct->len < string_len) ? str_struct->len : string_len;

    for (size_t i = 0; i < min_len; i++) {
        if (str_struct->str[i] != string[i]) {
            return (unsigned char)str_struct->str[i] - (unsigned char)string[i];
        }
    }
    return str_struct->len - string_len;
}
// --------------------------------------------------------------------------------

int compare_strings_string(const string_t* str_struct_one, string_t* str_struct_two) {
    if (!str_struct_one || !str_struct_two || !str_struct_one->str || !str_struct_two->str) {
        errno = EINVAL;
        return INT_MIN; // Or another designated error value
    } 

    size_t string_len = str_struct_two->len;
    size_t min_len = (str_struct_one->len < string_len) ? str_struct_one->len : string_len;

    for (size_t i = 0; i < min_len; i++) {
        if (str_struct_one->str[i] != str_struct_two->str[i]) {
            return (unsigned char)str_struct_one->str[i] - (unsigned char)str_struct_two->str[i];
        }
    }
    return str_struct_one->len - string_len;
}
// --------------------------------------------------------------------------------

string_t* copy_string(const string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return NULL;
    }
    string_t* new_str = init_string(get_string(str));
    if (new_str->alloc < str->alloc) 
        reserve_string(new_str, str->alloc);
    return new_str; 
}
// --------------------------------------------------------------------------------

bool reserve_string(string_t* str, size_t len) {
    if (!str || !str->str) {
        errno = EINVAL;
        return false;
    }

    // Ensure the requested length is greater than the current allocation
    if (len <= str->alloc) {
        errno = EINVAL;
        return false;
    }

    // Attempt to reallocate memory
    char* ptr = realloc(str->str, sizeof(char) * len);
    if (!ptr) {
        errno = ENOMEM;
        fprintf(stderr,"ERROR: Failed to reallocate memory for char* in reserver_string()\n");
        return false;
    }

    // Update the string_t structure with the new allocation
    str->str = ptr;
    str->alloc = len;

    return true;
}
// --------------------------------------------------------------------------------

bool trim_string(string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return false;
    }
    
    // If already at minimum size, nothing to do
    if (str->len + 1 == str->alloc) {
        return true;
    }
    
    // Sanity check for corrupted string_t
    if (str->len + 1 > str->alloc) {
        errno = EINVAL;
        return false;
    }

    char *ptr = realloc(str->str, str->len + 1);
    if (ptr == NULL) {
        errno = ENOMEM;
        fprintf(stderr, "ERROR: Failure to allocate memory for 'char*' in trim_string()\n");
        return false;
    }
    
    str->str = ptr;
    str->alloc = str->len + 1;
    return true;
}
// -------------------------------------------------------------------------------- 

char* first_char_occurance(string_t* str, char value) {
    if (!str || !str->str) {
        errno = EINVAL;
        return NULL;
    }
    
    char* current = str->str;
    while (*current != '\0') {
        if (*current == value) {
            return current;
        }
        current++;
    }
    
    return NULL;
}
// -------------------------------------------------------------------------------- 

char* last_char_occurance(string_t* str, char value) {
    if (!str || !str->str) {
        errno = EINVAL;
        return NULL;
    }
    
    // Start from last character (before null terminator)
    char* current = str->str + str->len - 1;
    
    // Continue until we reach the beginning of the string
    while (current >= str->str) {
        if (*current == value) {
            return current;
        }
        current--;
    }
    
    return NULL;
}
// --------------------------------------------------------------------------------

char* first_lit_substr_occurrence(string_t* str, char* sub_str) {
    if (!str || !str->str || !sub_str) {
        errno = EINVAL;
        return NULL;
    }
    
    size_t sub_len = strlen(sub_str);
    
    // Check if substring is longer than main string
    if (sub_len > str->len) {
        return NULL;
    }
    
    char* min_ptr = first_char(str);
    // Only need to check up to the point where substring could still fit
    char* max_ptr = min_ptr + (str->len - sub_len);
    
    for (char* it = min_ptr; it <= max_ptr; it++) {
        size_t j;
        for (j = 0; j < sub_len; j++) {
            if (sub_str[j] != *(it + j)) {
                break;
            }
        }
        if (j == sub_len) {
            return it;
        }
    }
    
    return NULL;
}
// -------------------------------------------------------------------------------- 

char* first_string_substr_occurrence(string_t* str, string_t* sub_str) {
    if (!str || !str->str || !sub_str || !sub_str->str) {
        errno = EINVAL;
        return NULL;
    }
    
    size_t sub_len = sub_str->len;
    
    // Check if substring is longer than main string
    if (sub_len > str->len) {
        return NULL;
    }
    
    char* min_ptr = first_char(str);
    // Only need to check up to the point where substring could still fit
    char* max_ptr = min_ptr + (str->len - sub_len);
    
    for (char* it = min_ptr; it <= max_ptr; it++) {
        size_t j;
        for (j = 0; j < sub_len; j++) {
            if (sub_str->str[j] != *(it + j)) {
                break;
            }
        }
        if (j == sub_len) {
            return it;
        }
    }
    
    return NULL;
}
// --------------------------------------------------------------------------------

char* last_lit_substr_occurrence(string_t* str, char* sub_str) {
    if (!str || !str->str || !sub_str) {
        errno = EINVAL;
        return NULL;
    }
    
    size_t sub_len = strlen(sub_str);
    
    // Check if substring is longer than main string
    if (sub_len > str->len) {
        return NULL;
    }
    
    char* min_ptr = first_char(str);
    // Only need to check up to the point where substring could still fit
    char* max_ptr = min_ptr + (str->len - sub_len);
    
    for (char* it = max_ptr; it >= min_ptr; it--) {
        size_t j;
        for (j = 0; j < sub_len; j++) {
            if (sub_str[j] != *(it + j)) {
                break;
            }
        }
        if (j == sub_len) {
            return it;
        }
    }
    
    return NULL;
}
// -------------------------------------------------------------------------------- 

char* last_string_substr_occurrence(string_t* str, string_t* sub_str) {
   if (!str || !str->str || !sub_str || !sub_str->str) {
       errno = EINVAL;
       return NULL;
   }
   
   size_t sub_len = sub_str->len;
   
   // Check if substring is longer than main string
   if (sub_len > str->len) {
       return NULL;
   }
   
   char* min_ptr = first_char(str);
   // Only need to check up to the point where substring could still fit
   char* max_ptr = min_ptr + (str->len - sub_len);
   
   for (char* it = max_ptr; it >= min_ptr; it--) {
       size_t j;
       for (j = 0; j < sub_len; j++) {
           if (sub_str->str[j] != *(it + j)) {
               break;
           }
       }
       if (j == sub_len) {
           return it;
       }
   }
   
   return NULL;
}
// --------------------------------------------------------------------------------

char* first_char(string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return NULL;
    }
    return str->str;
}
// --------------------------------------------------------------------------------

char* last_char(string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return NULL;
    }
    return str->str + str->len - 1;
}
// --------------------------------------------------------------------------------

bool is_string_ptr(string_t* str, char* ptr) {
    if (!str || !str->str || !ptr) {
        errno = EINVAL;
        return false;  // Changed from NULL to false
    }
    
    char* start = first_char(str);
    char* end = start + str->len;  // Points one past the last character
    
    return (ptr >= start && ptr < end);
}
// -------------------------------------------------------------------------------- 

bool drop_lit_substr(string_t* string, const char* substring, char* min_ptr, char* max_ptr) {
    if (!string || !substring || !string->str) {
        errno = EINVAL;
        return false;
    }
    if (!is_string_ptr(string, min_ptr) || !is_string_ptr(string, max_ptr)) {
        errno = ERANGE;
        return false;
    }
    if (max_ptr <= min_ptr) {
        errno = EINVAL;
        return false;
    }
    
    size_t substr_len = strlen(substring);
    if (string->len < substr_len) return true;
    
    char* ptr;
    while ((ptr = _last_literal_between_ptrs(substring, min_ptr, max_ptr))) {
        size_t drop_len = substr_len;
        // Check if there's a space after the substring and it's within bounds
        if (ptr + substr_len < string->str + string->len && 
            *(ptr + substr_len) == ' ') {
            drop_len++;  // Include the space
        }
        
        size_t move_length = max_ptr - (ptr + drop_len) + 1;
        memmove(ptr, ptr + drop_len, move_length);
        
        string->len -= drop_len;
        max_ptr -= drop_len;
        *(string->str + string->len) = '\0';
    }
    return true;
}
// --------------------------------------------------------------------------------

bool drop_string_substr(string_t* string, const string_t* substring, char* min_ptr, char* max_ptr) {
    if (!string || !substring || !string->str || !substring->str) {
        errno = EINVAL;
        return false;
    }
    if (!is_string_ptr(string, min_ptr) || !is_string_ptr(string, max_ptr)) {
        errno = ERANGE;
        return false;
    }
    if (max_ptr <= min_ptr) {
        errno = EINVAL;
        return false;
    }
    
    size_t substr_len = substring->len;
    if (string->len < substr_len) return true;
    
    char* ptr;
    while ((ptr = _last_literal_between_ptrs(substring->str, min_ptr, max_ptr))) {
        size_t drop_len = substr_len;
        // Check if there's a space after the substring and it's within bounds
        if (ptr + substr_len < string->str + string->len && 
            *(ptr + substr_len) == ' ') {
            drop_len++;  // Include the space
        }
        
        size_t move_length = max_ptr - (ptr + drop_len) + 1;
        memmove(ptr, ptr + drop_len, move_length);
        
        string->len -= drop_len;
        max_ptr -= drop_len;
        *(string->str + string->len) = '\0';
    }
    return true;
}
// -------------------------------------------------------------------------------- 

bool replace_lit_substr(string_t* string, const char* pattern, const char* replace_string,
                        char* min_ptr, char* max_ptr) {
    if (!string || !string->str || !pattern ||  !replace_string || 
        !min_ptr || !max_ptr) {
        errno = EINVAL;
        return false;
    }
   
    if (!is_string_ptr(string, min_ptr) || !is_string_ptr(string, max_ptr)) {
        errno = ERANGE;
        return false;
    } 

    if (min_ptr > max_ptr) {
        errno = ERANGE;
        return false;
    }
    
    // Calculate the delta between the pattern and replacement lengths.
    int delta = strlen(replace_string) - strlen(pattern);

    size_t pattern_len = strlen(pattern);
    size_t replace_len = strlen(replace_string);

    // Pre-allocate if we need more space
    size_t count = 0;
    if (delta > 0) {
        char* search_ptr = min_ptr;
        char* end_ptr = max_ptr;
        while ((search_ptr = _last_literal_between_ptrs(pattern, min_ptr, end_ptr))) {
            count++;
            if (search_ptr <= min_ptr) break;  // Found match at start, we're done
            end_ptr = search_ptr - 1;
        } 
        size_t new_size = string->len + (delta * count) + 1;
        if (new_size > string->alloc) {
            size_t min_pos = min_ptr - string->str;
            size_t max_pos = max_ptr - string->str;
            
            char* new_data = realloc(string->str, new_size);
            if (!new_data) {
                errno = ENOMEM;
                fprintf(stderr, "ERROR: Realloc failed in replace_lit_substring\n");
                return false;
            }
            string->str = new_data;
            string->alloc = new_size;
            min_ptr = string->str + min_pos;
            max_ptr = string->str + max_pos;
        }
    }
    
    char* ptr = _last_literal_between_ptrs(pattern, min_ptr, max_ptr);
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

            size_t tail_length = string->str + string->len - (ptr + pattern_len);
            memmove(ptr + replace_len, ptr + pattern_len, tail_length);

            string->len += delta; // delta is negative, so it reduces string->len

            string->str[string->len] = '\0';

            max_ptr -= pattern_len - replace_len;
        }
        else {
            memmove(ptr + replace_len,
                    ptr + pattern_len,
                    string->len - (ptr - string->str) - pattern_len);
            memcpy(ptr, replace_string, replace_len);
            string->len += delta;
            string->str[string->len] = '\0';
            ptr += delta;
            max_ptr = ptr;
        }        
         // Find the next pattern instance within the updated pointer bounds.
         if (min_ptr < max_ptr)
            ptr = _last_literal_between_ptrs(pattern, min_ptr, max_ptr);
         else
            ptr = NULL;
    }

    return true;
}
// --------------------------------------------------------------------------------

bool replace_string_substr(string_t* string, const string_t* pattern, const string_t* replace_string,
                           char* min_ptr, char* max_ptr) {
    if (!string || !string->str || !pattern || !pattern->str || 
        !replace_string || !replace_string->str || !min_ptr || !max_ptr) {
        errno = EINVAL;
        return false;
    }
  
    if (!is_string_ptr(string, min_ptr) || !is_string_ptr(string, max_ptr)) {
        errno = ERANGE;
        return false;
    }

    if (min_ptr > max_ptr) {
        errno = ERANGE;
        return false;
    }
   
    // Calculate the delta between the pattern and replacement lengths.
    int delta = replace_string->len - pattern->len;
   
    // Count occurrences within specified range if we need to grow the string
    size_t count = 0;
    if (delta > 0) {
        char* search_ptr = min_ptr;
        char* end_ptr = max_ptr;
        while ((search_ptr = _last_literal_between_ptrs(pattern->str, min_ptr, end_ptr))) {
            count++;
            if (search_ptr <= min_ptr) break;  // Found match at start, we're done
            end_ptr = search_ptr - 1;
        }
       
        // Pre-allocate all needed memory
        size_t new_size = string->len + (delta * count) + 1;
        if (new_size > string->alloc) {
            size_t min_pos = min_ptr - string->str;
            size_t max_pos = max_ptr - string->str;
           
            char* new_data = realloc(string->str, new_size);
            if (!new_data) {
                errno = ENOMEM;
                fprintf(stderr, "ERROR: Realloc failed in replace_string_substring\n");
                return false;
            }
            string->str = new_data;
            string->alloc = new_size;
            min_ptr = string->str + min_pos;
            max_ptr = string->str + max_pos;
        }
    }
   
    char* ptr = _last_literal_between_ptrs(pattern->str, min_ptr, max_ptr);
    while (ptr) {
        if (delta == 0) {
            memcpy(ptr, replace_string->str, replace_string->len);
        }
        else if (delta < 0) {
            memcpy(ptr, replace_string->str, replace_string->len);
            memmove(ptr + replace_string->len, 
                    ptr + pattern->len,
                    string->len - (ptr - string->str) - pattern->len + 1);
            string->len += delta;
        }
        else {  // delta > 0
            memmove(ptr + replace_string->len,
                    ptr + pattern->len,
                    string->len - (ptr - string->str) - pattern->len + 1);
            memcpy(ptr, replace_string->str, replace_string->len);
            string->len += delta;
        }
       
        max_ptr = ptr - 1;
        if (min_ptr >= max_ptr) break;
        ptr = _last_literal_between_ptrs(pattern->str, min_ptr, max_ptr);
    }
   
    string->str[string->len] = '\0';
    return true;
}
// --------------------------------------------------------------------------------

void to_upper_char(char* val) {
    if (!val) {
        errno = EINVAL;
        return;
    }
    if (*val >= 'a' && *val <= 'z') *val -= 32; 
}
// --------------------------------------------------------------------------------

void to_lower_char(char* val) {
    if (!val) {
        errno = EINVAL;
        return;
    }
    if (*val >= 'A' && *val <= 'Z') *val += 32; 
}
// --------------------------------------------------------------------------------

void to_uppercase(string_t *s) {
    if(!s || !s->str) {
        errno = EINVAL;
        return;
    }
    char* begin = s->str;
    char* end = s->str + s->len;
    for (char* i =  begin; i != end; i++) {
        if (*i >= 'a' && *i <= 'z') *i -= 32;
    }
}
// --------------------------------------------------------------------------------

void to_lowercase(string_t *s) {
    if(!s || !s->str) {
        errno = EINVAL;
        return;
    }
    char* begin = s->str;
    char* end = s->str + s->len;
    for (char* i =  begin; i != end; i++) {
        if (*i >= 'A' && *i <= 'Z') *i += 32;
    }
}
// --------------------------------------------------------------------------------

string_t* pop_string_token(string_t* str_struct, char token) {
    if (!str_struct || !str_struct->str) {
        errno = EINVAL;
        return NULL;
    }
    if (str_struct->len == 0) {
        return NULL;
    }
    for (int i = str_struct->len - 1; i >= 0; i--) {
        if (str_struct->str[i] == token) {
            // Handle case where token is last character
            if (i == (int)str_struct->len - 1) {
                str_struct->str[i] = '\0';
                str_struct->len = i;
                return init_string("");
            }
            
            string_t *one = init_string(str_struct->str + (i + 1));
            str_struct->str[i] = '\0';
            str_struct->len = i;
            return one;
        }
    }
    return NULL;
}
// --------------------------------------------------------------------------------

size_t token_count(const string_t* str, const char* delim) {
    if (!str || !str->str || !delim) {
        errno = EINVAL;
        return 0;
    }
    
    if (str->len == 0 || strlen(delim) == 0) {
        return 0;
    }

    size_t count = 0;
    const char* ptr = str->str;
    bool in_token = false;

    while (*ptr) {
        bool is_delim = false;
        // Check if current character is a delimiter
        for (size_t i = 0; delim[i] != '\0'; i++) {
            if (*ptr == delim[i]) {
                is_delim = true;
                break;
            }
        }

        if (!is_delim && !in_token) {
            // Start of new token
            count++;
            in_token = true;
        }
        else if (is_delim) {
            // End of current token
            in_token = false;
        }
        ptr++;
    }

    return count;
}
// --------------------------------------------------------------------------------

char get_char(string_t* str, size_t index) {
    if (!str || !str->str) {
        errno = EINVAL;
        return 0;
    }
    if (index > str->len - 1) {
        errno = ERANGE;
        return 0;
    }
    return str->str[index];
}
// --------------------------------------------------------------------------------

void replace_char(string_t* str, size_t index, char value) {
    if (!str || !str->str) {
        errno = EINVAL;
        return;
    }
    if (index > str->len - 1) {
        errno = ERANGE;
        return;
    }
    str->str[index] = value;
}
// --------------------------------------------------------------------------------

void trim_leading_whitespace(string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return;
    }
    
    if (str->len == 0) {
        return;
    }
    
    // Find first non-whitespace character
    char* ptr = str->str;
    while (*ptr && isspace(*ptr)) {
        ptr++;
    }
    
    // If no leading whitespace found, return
    if (ptr == str->str) {
        return;
    }
    
    // Calculate number of whitespace characters
    size_t whitespace_count = ptr - str->str;
    
    // Move remaining string to front
    memmove(str->str, ptr, str->len - whitespace_count + 1);  // +1 for null terminator
    
    // Update length
    str->len -= whitespace_count;
    
    return;
}
// -------------------------------------------------------------------------------- 

void trim_trailing_whitespace(string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return;
    }
    
    if (str->len == 0) {
        return;
    }
    
    // Start from last character (before null terminator)
    char* ptr = str->str + str->len - 1;
    
    // Move backwards while finding whitespace
    while (ptr >= str->str && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')) {
        ptr--;
    }
    
    // Set new end of string
    ptr++;  // Move to one after last non-whitespace char
    *ptr = '\0';
    
    // Update length (ptr - str->str gives new length)
    str->len = ptr - str->str;
}
// --------------------------------------------------------------------------------

void trim_all_whitespace(string_t* str) {
    if (!str || !str->str) {
        errno = EINVAL;
        return;
    }
    
    if (str->len == 0) {
        return;
    }
    
    char* read = str->str;
    char* write = str->str;
    
    while (*read) {
        if (*read != ' ' && *read != '\t' && *read != '\n') {
            *write = *read;
            write++;
        }
        read++;
    }
    
    // Null terminate the string at the new position
    *write = '\0';
    
    // Update length (write - str->str gives new length)
    str->len = write - str->str;
    
    return;
}
// ================================================================================ 
// ================================================================================ 

static char* _str_end(string_t* s) {
    if (!s || !s->str) {
        return NULL;
    }
    return s->str + s->len;
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

str_iter init_str_iter() {
    str_iter iter;

    iter.begin = first_char;
    iter.end = _str_end;
    iter.next = _str_next;
    iter.prev = _str_prev;
    iter.get = _str_get;
    return iter;
}
// ================================================================================ 
// ================================================================================ 

struct string_v {
    string_t* data;
    size_t len;
    size_t alloc;
};
// -------------------------------------------------------------------------------- 

string_v* init_str_vector(size_t buff) {
    string_v* struct_ptr = malloc(sizeof(string_v));
    if (struct_ptr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
   
    string_t* data_ptr = malloc(buff * sizeof(string_t));
    if (data_ptr == NULL) {
        free(struct_ptr);
        errno = ENOMEM;
        return NULL; 
    }
   
    // Initialize all elements
    memset(data_ptr, 0, buff * sizeof(string_t));
   
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

void free_str_vector(string_v* vec) {
   if (!vec) {
       errno = EINVAL;
       return;
   }
   
   // Free each string in the vector
   if (vec->data) {
       for (size_t i = 0; i < vec->len; i++) {
           free(vec->data[i].str);
       }
       free(vec->data);
   }
   free(vec);
}
// --------------------------------------------------------------------------------

void _free_str_vector(string_v** vec) {
    if (vec && *vec) {
        free_str_vector(*vec);
        *vec = NULL;
    }
}
// -------------------------------------------------------------------------------- 

bool push_back_str_vector(string_v* vec, const char* value) {
    if (!vec || !vec->data || !value) {
        errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        // Allocate more space for the array of str structs
        string_t* new_data = realloc(vec->data, new_alloc * sizeof(string_t));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(string_t));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
   
    // Allocate and copy the new string
    size_t str_len = strlen(value);
    vec->data[vec->len].str = malloc(str_len + 1);
    if (!vec->data[vec->len].str) {
        errno = ENOMEM;
        return false;
    }
   
    strcpy(vec->data[vec->len].str, value);
    vec->data[vec->len].alloc = str_len + 1;
    vec->data[vec->len].len = str_len;
    vec->len++;
   
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_str_vector(string_v* vec, const char* value) {
    if (!vec || !vec->data || !value) {
        errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        string_t* new_data = realloc(vec->data, new_alloc * sizeof(string_t));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(string_t));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Move existing elements right
    memmove(vec->data + 1, vec->data, vec->len * sizeof(string_t));
    
    // Initialize new first element space
    memset(vec->data, 0, sizeof(string_t));
    
    // Allocate and copy the new string
    size_t str_len = strlen(value);
    vec->data[0].str = malloc(str_len + 1);
    if (!vec->data[0].str) {
        errno = ENOMEM;
        memmove(vec->data, vec->data + 1, vec->len * sizeof(string_t));
        return false;
    }
    
    strcpy(vec->data[0].str, value);
    vec->data[0].alloc = str_len + 1;
    vec->data[0].len = str_len;
    vec->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_str_vector(string_v* vec, const char* str, size_t index) {
    if (!vec || !vec->data || !str) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        string_t* new_data = realloc(vec->data, new_alloc * sizeof(string_t));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(string_t));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    
    // Move existing elements right
    if (index < vec->len) {  // Only move if not appending
        memmove(vec->data + index + 1, vec->data + index, 
                (vec->len - index) * sizeof(string_t));
    }
    
    // Initialize new element space
    memset(vec->data + index, 0, sizeof(string_t));
    
    // Allocate and copy the new string
    size_t str_len = strlen(str);
    vec->data[index].str = malloc(str_len + 1);
    if (!vec->data[index].str) {
        errno = ENOMEM;
        if (index < vec->len) {  // Only restore if not appending
            memmove(vec->data + index, vec->data + index + 1, 
                    (vec->len - index) * sizeof(string_t));
        }
        return false;
    }
    
    strcpy(vec->data[index].str, str);
    vec->data[index].alloc = str_len + 1;
    vec->data[index].len = str_len;
    vec->len++;
    return true;
}
// --------------------------------------------------------------------------------

string_t* pop_back_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    
    if (vec->len == 0) {
        errno = EINVAL;
        return NULL;
    }
    
    // Create copy of last element
    string_t* temp = copy_string(&vec->data[vec->len - 1]);
    if (!temp) {
        return NULL;  // copy_string will set errno
    }
    
    // Clear the popped element for future reuse
    free(vec->data[vec->len - 1].str);
    vec->data[vec->len - 1].str = NULL;
    vec->data[vec->len - 1].len = 0;
    vec->data[vec->len - 1].alloc = 0;
    
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

string_t* pop_front_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
   
    if (vec->len == 0) {
        errno = EINVAL;
        return NULL;
    }
   
    // Create copy of first element
    string_t* temp = copy_string(&vec->data[0]);
    if (!temp) {
        return NULL;  // copy_string will set errno
    }
   
    // Free the first element
    free(vec->data[0].str);
   
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(string_t));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(string_t));
   
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

string_t* pop_any_str_vector(string_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
   
    if (vec->len == 0) {
        errno = EINVAL;
        return NULL;
    }

    if (index >= vec->len) {
        errno = ERANGE;
        return NULL;
    }

    // Create copy of element to pop
    string_t* temp = copy_string(&vec->data[index]);
    if (!temp) {
        return NULL;  // copy_string will set errno
    }
   
    // Free the element being removed
    free(vec->data[index].str);
    
    // Shift remaining elements left
    memmove(&vec->data[index], &vec->data[index + 1], 
            (vec->len - index - 1) * sizeof(string_t));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(string_t));
    
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

bool delete_back_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return false;
    }
    
    if (vec->len == 0) {
        errno = EINVAL;
        return false;
    }
    
    // Clear the popped element for future reuse
    free(vec->data[vec->len - 1].str);
    vec->data[vec->len - 1].str = NULL;
    vec->data[vec->len - 1].len = 0;
    vec->data[vec->len - 1].alloc = 0;
    
    vec->len--;
    return true;
}
// --------------------------------------------------------------------------------

bool delete_front_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return false;
    }
   
    if (vec->len == 0) {
        errno = EINVAL;
        return false;
    }
   
    // Free the first element
    free(vec->data[0].str);
   
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(string_t));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(string_t));
   
    vec->len--;
    return true;
}
// --------------------------------------------------------------------------------

bool delete_any_str_vector(string_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return false;
    }
   
    if (vec->len == 0) {
        errno = EINVAL;
        return false;
    }

    if (index >= vec->len) {
        errno = ERANGE;
        return false;
    }

    // Free the element being removed
    free(vec->data[index].str);
    
    // Shift remaining elements left
    memmove(&vec->data[index], &vec->data[index + 1], 
            (vec->len - index - 1) * sizeof(string_t));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(string_t));
    
    vec->len--;
    return true;
}
// --------------------------------------------------------------------------------

const string_t* str_vector_index(const string_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return NULL;
    }
    return &vec->data[index];
}
// --------------------------------------------------------------------------------

size_t str_vector_size(const string_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t str_vector_alloc(const string_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->alloc;
}
// --------------------------------------------------------------------------------

void reverse_str_vector(string_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_string(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// ================================================================================
// ================================================================================ 
// QUICKSORT

void swap_string(string_t* a, string_t* b) {
    if (!a || !b) {
        errno = EINVAL;
        return;
    }
    string_t temp = *a;
    *a = *b;
    *b = temp;
}
// --------------------------------------------------------------------------------

static string_t* _median_of_three(string_t* a, string_t* b, string_t* c, iter_dir direction) {
    if ((direction == FORWARD && compare_strings_string(a, b) < 0) ||
        (direction == REVERSE && compare_strings_string(a, b) > 0)) {
        if ((direction == FORWARD && compare_strings_string(b, c) < 0) ||
            (direction == REVERSE && compare_strings_string(b, c) > 0))
            return b;
        if ((direction == FORWARD && compare_strings_string(a, c) < 0) ||
            (direction == REVERSE && compare_strings_string(a, c) > 0))
            return c;
        return a;
    }
    if ((direction == FORWARD && compare_strings_string(a, c) < 0) ||
        (direction == REVERSE && compare_strings_string(a, c) > 0))
        return a;
    if ((direction == FORWARD && compare_strings_string(b, c) < 0) ||
        (direction == REVERSE && compare_strings_string(b, c) > 0))
        return c;
    return b;
}
// --------------------------------------------------------------------------------

static void _insertion_sort(string_t* vec, int low, int high, iter_dir direction) {
    for (int i = low + 1; i <= high; i++) {
        string_t key = vec[i];
        int j = i - 1;
        while (j >= low && ((direction == FORWARD && compare_strings_string(&vec[j], &key) > 0) ||
                           (direction == REVERSE && compare_strings_string(&vec[j], &key) < 0))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = key;
    }
}
// --------------------------------------------------------------------------------

static int _partition_string(string_t* vec, int low, int high, iter_dir direction) {
    // Choose pivot using median of three
    int mid = low + (high - low) / 2;
    string_t* pivot_ptr = _median_of_three(&vec[low], &vec[mid], &vec[high], direction);
    
    // Move pivot to end
    if (pivot_ptr != &vec[high])
        swap_string(pivot_ptr, &vec[high]);
    
    string_t pivot = vec[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && compare_strings_string(&vec[j], &pivot) < 0) ||
            (direction == REVERSE && compare_strings_string(&vec[j], &pivot) > 0)) {
            i++;
            swap_string(&vec[i], &vec[j]);
        }
    }
    swap_string(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// --------------------------------------------------------------------------------

static void _quicksort_str_vector(string_t* vec, int low, int high, iter_dir direction) {
    while (low < high) {
        // Use insertion sort for small arrays
        if (high - low < 10) {
            _insertion_sort(vec, low, high, direction);
            break;
        }
        
        int pi = _partition_string(vec, low, high, direction);
        
        // Tail recursion elimination
        if (pi - low < high - pi) {
            _quicksort_str_vector(vec, low, pi - 1, direction);
            low = pi + 1;
        } else {
            _quicksort_str_vector(vec, pi + 1, high, direction);
            high = pi - 1;
        }
    }
}
// --------------------------------------------------------------------------------

void sort_str_vector(string_v* vec, iter_dir direction) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    if (vec->len < 2) return;
    
    _quicksort_str_vector(vec->data, 0, vec->len - 1, direction);
}
// --------------------------------------------------------------------------------

string_v* tokenize_string(const string_t* str, const char* delim) {
    if (!str || !str->str || !delim) {
        errno = EINVAL;
        return NULL;
    }
    
    // Initialize vector to store tokens
    size_t count = token_count(str, delim);
    string_v* tokens = init_str_vector(count);
    if (!tokens) {
        return NULL;
    }
    
    const char* start = str->str;
    const char* end = str->str + str->len;
    const char* current = start;
    
    while (current < end) {
        // Skip delimiters
        while (current < end) {
            bool is_delim = false;
            for (const char* d = delim; *d; d++) {
                if (*current == *d) {
                    is_delim = true;
                    break;
                }
            }
            if (!is_delim) break;
            current++;
        }
        
        if (current >= end) break;
        
        // Find end of token
        const char* token_end = current;
        while (token_end < end) {
            bool is_delim = false;
            for (const char* d = delim; *d; d++) {
                if (*token_end == *d) {
                    is_delim = true;
                    break;
                }
            }
            if (is_delim) break;
            token_end++;
        }
        
        // Create temporary null-terminated string for token
        size_t token_len = token_end - current;
        char temp[token_len + 1];
        memcpy(temp, current, token_len);
        temp[token_len] = '\0';
        
        if (!push_back_str_vector(tokens, temp)) {
            free_str_vector(tokens);
            return NULL;
        }
        
        current = token_end;
    }
    
    return tokens;
}
// ================================================================================
// ================================================================================
// eof
