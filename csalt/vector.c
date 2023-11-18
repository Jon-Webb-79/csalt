// ================================================================================
// ================================================================================
// - File:    vector.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    November 06, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/vector.h"
#include <linux/limits.h>

// ================================================================================
// ================================================================================
// Constants for memory management

const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
// ================================================================================
// ================================================================================
// INIT FUNCTIONS

char_v* init_char_vector(size_t buff) {
    char_v* struct_ptr = malloc(sizeof(char_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    char* data_ptr = malloc(buff * sizeof(char));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

uchar_v* init_uchar_vector(size_t buff) {
    uchar_v* struct_ptr = malloc(sizeof(uchar_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned char* data_ptr = malloc(buff * sizeof(unsigned char));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

short_v* init_short_vector(size_t buff) {
    short_v* struct_ptr = malloc(sizeof(short_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    short int* data_ptr = malloc(buff * sizeof(short int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

ushort_v* init_ushort_vector(size_t buff) {
    ushort_v* struct_ptr = malloc(sizeof(ushort_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned short int* data_ptr = malloc(buff * sizeof(unsigned short int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

int_v* init_int_vector(size_t buff) {
    int_v* struct_ptr = malloc(sizeof(int_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    int* data_ptr = malloc(buff * sizeof(int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

uint_v* init_uint_vector(size_t buff) {
    uint_v* struct_ptr = malloc(sizeof(uint_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned int* data_ptr = malloc(buff * sizeof(unsigned int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

long_v* init_long_vector(size_t buff) {
    long_v* struct_ptr = malloc(sizeof(long_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    long int* data_ptr = malloc(buff * sizeof(long int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

ulong_v* init_ulong_vector(size_t buff) {
    ulong_v* struct_ptr = malloc(sizeof(ulong_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned long int* data_ptr = malloc(buff * sizeof(unsigned long int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

llong_v* init_llong_vector(size_t buff) {
    llong_v* struct_ptr = malloc(sizeof(llong_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    long long int* data_ptr = malloc(buff * sizeof(long long int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

ullong_v* init_ullong_vector(size_t buff) {
    ullong_v* struct_ptr = malloc(sizeof(ullong_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    unsigned long long int* data_ptr = malloc(buff * sizeof(unsigned long long int));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

float_v* init_float_vector(size_t buff) {
    float_v* struct_ptr = malloc(sizeof(float_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    float* data_ptr = malloc(buff * sizeof(float));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

double_v* init_double_vector(size_t buff) {
    double_v* struct_ptr = malloc(sizeof(double_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    double* data_ptr = malloc(buff * sizeof(double));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

ldouble_v* init_ldouble_vector(size_t buff) {
    ldouble_v* struct_ptr = malloc(sizeof(ldouble_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    long double* data_ptr = malloc(buff * sizeof(long double));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

bool_v* init_bool_vector(size_t buff) {
    bool_v* struct_ptr = malloc(sizeof(bool_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    bool* data_ptr = malloc(buff * sizeof(bool));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

string_v* init_string_vector(size_t buff) {
    string_v* struct_ptr = malloc(sizeof(string_v));
    if ( struct_ptr == NULL) {
        return NULL;
    }
    str* data_ptr = malloc(buff * sizeof(str));
    if ( data_ptr == NULL) {
        free(struct_ptr); // Free un-necessary memory allocation
        return NULL; 
    }
    for (size_t i = 0; i < buff; i++) {
        data_ptr[i].data = NULL;
        data_ptr[i].len = 0;
        data_ptr[i].alloc = 0;
    }
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

InitVecFunc init_vector(dtype dat_type) {
    switch(dat_type) {
        case dChar: return (InitVecFunc)init_char_vector;
        case dUChar: return (InitVecFunc)init_uchar_vector;
        case dShort: return (InitVecFunc)init_short_vector;
        case dUShort: return (InitVecFunc)init_ushort_vector;
        case dInt: return (InitVecFunc)init_int_vector;
        case dUInt: return (InitVecFunc)init_uint_vector;
        case dLong: return (InitVecFunc)init_long_vector;
        case dULong: return (InitVecFunc)init_ulong_vector;
        case dLongLong: return (InitVecFunc)init_llong_vector;
        case dULongLong: return (InitVecFunc)init_ullong_vector;
        case dFloat: return (InitVecFunc)init_float_vector;
        case dDouble: return (InitVecFunc)init_double_vector;
        case dLDouble: return (InitVecFunc)init_ldouble_vector;
        case dBool: return (InitVecFunc)init_bool_vector;
        case dString: return (InitVecFunc)init_string_vector;
        default: return NULL;
    }
}
// ================================================================================
// ================================================================================
// PRIVATE FUNCTION FOR FREE MEMORY 

static void _free_str(str *s) {
    if ( s == NULL ) return;
    free(s->data);
    s->data = NULL;
}
// ================================================================================
// ================================================================================
// FREE FUNCTIONS 

void free_char_vector(char_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_uchar_vector(uchar_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_short_vector(short_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ushort_vector(ushort_v* vec) {
    if (vec == NULL) { 
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_int_vector(int_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_uint_vector(uint_v* vec) {
    if (vec == NULL) { 
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_long_vector(long_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ulong_vector(ulong_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_llong_vector(llong_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ullong_vector(ullong_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_float_vector(float_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_double_vector(double_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ldouble_vector(ldouble_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_bool_vector(bool_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_string_vector(string_v* vec) {
    if (vec == NULL) {
        errno = EINVAL;
        return; 
    }

    // Free each individual string in the vector
    for (size_t i = 0; i < vec->len; ++i) {
        // Assume _free_string takes a pointer to a str struct and frees it
        _free_str(&vec->data[i]);
    }

    // Now that all the str data has been freed, it's safe to free the array
    free(vec->data); // Free the array of str structs
    vec->data = NULL; // Set the data pointer to NULL to avoid use-after-free

    // Finally, free the string_v struct itself
    free(vec); // Free the string_v struct pointer
}
// ================================================================================
// ================================================================================
// Functions for garbage collection 

void _free_char_vector(char_v** vec) {
    if (vec && *vec) {
        free_char_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_uchar_vector(uchar_v** vec) {
    if (vec && *vec) {
        free_uchar_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_short_vector(short_v** vec) {
    if (vec && *vec) {
        free_short_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ushort_vector(ushort_v** vec) {
    if (vec && *vec) {
        free_ushort_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_int_vector(int_v** vec) {
    if (vec && *vec) {
        free_int_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_uint_vector(uint_v** vec) {
    if (vec && *vec) {
        free_uint_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_long_vector(long_v** vec) {
    if (vec && *vec) {
        free_long_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ulong_vector(ulong_v** vec) {
    if (vec && *vec) {
        free_ulong_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_llong_vector(llong_v** vec) {
    if (vec && *vec) {
        free_llong_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ullong_vector(ullong_v** vec) {
    if (vec && *vec) {
        free_ullong_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_float_vector(float_v** vec) {
    if (vec && *vec) {
        free_float_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_double_vector(double_v** vec) {
    if (vec && *vec) {
        free_double_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ldouble_vector(ldouble_v** vec) {
    if (vec && *vec) {
        free_ldouble_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_bool_vector(bool_v** vec) {
    if (vec && *vec) {
        free_bool_vector(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_sstring_vector(string_v** vec) {
    if (vec && *vec) {
        free_string_vector(*vec);
    }
}
// ================================================================================
// ================================================================================

bool push_char_vector(char_v* vec, char var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        char* new_data = realloc(vec->data, new_alloc * sizeof(char));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(char));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_uchar_vector(uchar_v* vec, unsigned char var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        unsigned char* new_data = realloc(vec->data, new_alloc * sizeof(unsigned char));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(unsigned char));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_short_vector(short_v* vec, short int var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        short int* new_data = realloc(vec->data, new_alloc * sizeof(short int));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(short int));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_ushort_vector(ushort_v* vec, unsigned short int var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        unsigned short int* new_data = realloc(vec->data, new_alloc * sizeof(unsigned short int));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(unsigned short int));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_int_vector(int_v* vec, int var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        int* new_data = realloc(vec->data, new_alloc * sizeof(int));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(int));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_uint_vector(uint_v* vec, unsigned int var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        unsigned int* new_data = realloc(vec->data, new_alloc * sizeof(unsigned int));
        if (!new_data) {
            fprintf(stderr, "Error: Realloc failed in push_uint_vector\n");
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(unsigned int));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_long_vector(long_v* vec, long int var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        long int* new_data = realloc(vec->data, new_alloc * sizeof(long int));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(long int));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_ulong_vector(ulong_v* vec, unsigned long int var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        unsigned long int* new_data = realloc(vec->data, new_alloc * sizeof(unsigned long int));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(unsigned long int));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_llong_vector(llong_v* vec, long long int var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        long long int* new_data = realloc(vec->data, new_alloc * sizeof(long long int));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(long long int));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_ullong_vector(ullong_v* vec, unsigned long long int var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        unsigned long long int* new_data = realloc(vec->data, new_alloc * sizeof(unsigned long long int));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(unsigned long long int));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_float_vector(float_v* vec, float var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(float));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_double_vector(double_v* vec, double var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }
   
    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        double* new_data = realloc(vec->data, new_alloc * sizeof(double));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(double));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_ldouble_vector(ldouble_v* vec, long double var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        long double* new_data = realloc(vec->data, new_alloc * sizeof(long double));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(long double));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_bool_vector(bool_v* vec, bool var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }
    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        bool* new_data = realloc(vec->data, new_alloc * sizeof(bool));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Increment length first if inserting not at the end
    if (index < vec->len) {
        vec->len++;
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index - 1) * sizeof(bool));
    } else {
        // If we're adding to the end, increment length after insertion
        vec->len++;
    }

    vec->data[index] = var;

    return true;
}
// --------------------------------------------------------------------------------

bool push_string_vector(string_v* vec, const char* var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }
   
    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        // Allocate more space for the array of str structs
        str* new_data = realloc(vec->data, new_alloc * sizeof(str));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Make space for the new element
    if (index < vec->len) {
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index) * sizeof(str));
    }

    // Allocate memory for the string and copy it
    size_t str_len = strlen(var);
    vec->data[index].data = malloc(str_len + 1); // +1 for the null terminator
    if (!vec->data[index].data) {
        fprintf(stderr, "Error: String malloc failed in push_string_vector\n");
        return false;
    }
    strcpy(vec->data[index].data, var);
    vec->data[index].alloc = str_len + 1;
    vec->data[index].len = str_len;

    // Increment the vector length only after successful insertion
    vec->len++;

    return true;
}
// --------------------------------------------------------------------------------

bool push_str_vector(string_v* vec, str* var, size_t index) {
    if ( !vec || !vec->data ) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }

    // Check if there is enough space, if not, then realloc
    if (vec->alloc <= vec->len) {
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }

        // Allocate more space for the array of str structs
        str* new_data = realloc(vec->data, new_alloc * sizeof(str));
        if (!new_data) {
            return false;
        }
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Make space for the new element
    if (index < vec->len) {
        memmove(vec->data + index + 1,
                vec->data + index,
                (vec->len - index) * sizeof(str));
    }

    // Allocate memory for the string and copy it
    size_t str_len = strlen(var->data);
    vec->data[index].data = malloc(var->alloc); // +1 for the null terminator
    if (!vec->data[index].data) {
        fprintf(stderr, "Error: String malloc failed in push_string_vector\n");
        return false;
    }
    strcpy(vec->data[index].data, var->data);
    vec->data[index].alloc = var->alloc;
    vec->data[index].len = str_len;

    // Increment the vector length only after successful insertion
    vec->len++;

    return true;
}
// ================================================================================
// ================================================================================
// GET_VECTOR FUNCTIONS

char get_char_vector(char_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned char get_uchar_vector(uchar_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

short int get_short_vector(short_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned short int get_ushort_vector(ushort_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

int get_int_vector(int_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned int get_uint_vector(uint_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

long int get_long_vector(long_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned long int get_ulong_vector(ulong_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

long long int get_llong_vector(llong_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned long long int get_ullong_vector(ullong_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

float get_float_vector(float_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.0f;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0.0f; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

double get_double_vector(double_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0.0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

long double get_ldouble_vector(ldouble_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.0;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return 0.0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

bool get_bool_vector(bool_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return false;
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return false; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

char* get_string_vector(string_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return "\0";
    }
    if (index >= vec->len) {
        errno = ERANGE; // Result not within range
        return "\0"; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index].data;
}
// ================================================================================
// ================================================================================
// VECTOR_LENGTH FUNCTIONS

size_t char_vector_length(char_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t uchar_vector_length(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t short_vector_length(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t ushort_vector_length(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t int_vector_length(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t uint_vector_length(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t long_vector_length(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t ulong_vector_length(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t llong_vector_length(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t ullong_vector_length(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t float_vector_length(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t double_vector_length(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t ldouble_vector_length(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t bool_vector_length(bool_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t string_vector_length(string_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// ================================================================================
// ================================================================================

size_t char_vector_memory(char_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t uchar_vector_memory(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t short_vector_memory(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t ushort_vector_memory(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t int_vector_memory(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------


size_t uint_vector_memory(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t long_vector_memory(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t ulong_vector_memory(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t llong_vector_memory(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t ullong_vector_memory(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t float_vector_memory(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t double_vector_memory(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t ldouble_vector_memory(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t bool_vector_memory(bool_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t string_vector_memory(string_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// ================================================================================
// ================================================================================
//POP VECTOR FUNCTIONS

char pop_char_vector(char_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    char val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(char)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned char pop_uchar_vector(uchar_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned char val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(unsigned char)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

short int pop_short_vector(short_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    short int val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(short int)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned short int pop_ushort_vector(ushort_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned short int val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(unsigned short int)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

int pop_int_vector(int_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    int val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(int)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned int pop_uint_vector(uint_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned int val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(unsigned int)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

long int pop_long_vector(long_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    long int val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(long int)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned long int pop_ulong_vector(ulong_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned long int val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(unsigned long int)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

long long int pop_llong_vector(llong_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    long long int val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(long long int)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

unsigned long long int pop_ullong_vector(ullong_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0;
    }
    unsigned long long int val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(unsigned long long int)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

float pop_float_vector(float_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0.f;
    }
    float val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(float)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

double pop_double_vector(double_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0.;
    }
    double val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(double)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

long double pop_ldouble_vector(ldouble_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return 0.;
    }
    long double val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(long double)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

bool pop_bool_vector(bool_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len -1) {
        errno = ERANGE;
        return false;
    }
    bool val = vec->data[index];
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(bool)); 
    vec->len -= 1;
    return val;
}
// --------------------------------------------------------------------------------

str* pop_string_vector(string_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return NULL;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return NULL;
    }
    str* val = init_string_nol(vec->data[index].data);
    free(vec->data[index].data);
    memmove(vec->data + index, 
            vec->data + index + 1, 
            (vec->len - index - 1) * sizeof(str)); 
    vec->len -= 1;
    return val;
}
// ================================================================================
// ================================================================================
// REVERSE FUNCTIONS 

void reverse_char_vector(char_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_char(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_uchar_vector(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_uchar(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_short_vector(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_short(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_ushort_vector(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_ushort(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_int_vector(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_int(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_uint_vector(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_uint(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_long_vector(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_long(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_ulong_vector(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_ulong(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_llong_vector(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_llong(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_ullong_vector(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_ullong(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_float_vector(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_float(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_double_vector(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_double(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_ldouble_vector(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_ldouble(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_bool_vector(bool_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return;
    } 
    int i = 0;
    int j = vec->len - 1;
    while (i < j) {
       swap_bool(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// --------------------------------------------------------------------------------

void reverse_string_vector(string_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
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

char max_char_vector(char_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    char max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned char max_uchar_vector(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned char max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

short int max_short_vector(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    short int max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned short int max_ushort_vector(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned short int max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

int max_int_vector(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    int max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned int max_uint_vector(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned int max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

long int max_long_vector(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long int max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned long int max_ulong_vector(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long int max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

long long int max_llong_vector(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long int max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

unsigned long long int max_ullong_vector(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long long int max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

float max_float_vector(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.f;
    }
    float max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

double max_double_vector(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    double max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// --------------------------------------------------------------------------------

long double max_ldouble_vector(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    long double max_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max_value) max_value = vec->data[i];
    }
    return max_value;
}
// ================================================================================
// ================================================================================
// MIN VECTOR FUNCTIONS 

char min_char_vector(char_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    char min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned char min_uchar_vector(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned char min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

short int min_short_vector(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    short int min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned short int min_ushort_vector(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned short int min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

int min_int_vector(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    int min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned int min_uint_vector(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned int min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

long int min_long_vector(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long int min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned long int min_ulong_vector(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long int min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

long long int min_llong_vector(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long int min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

unsigned long long int min_ullong_vector(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long long int min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

float min_float_vector(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.f;
    }
    float min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

double min_double_vector(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    double min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// --------------------------------------------------------------------------------

long double min_ldouble_vector(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    long double min_value = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min_value) min_value = vec->data[i];
    }
    return min_value;
}
// ================================================================================
// ================================================================================
// SUM VECTOR FUNCTIONS 

char sum_char_vector(char_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    char sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

unsigned char sum_uchar_vector(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned char sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

short int sum_short_vector(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    short int sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

unsigned short int sum_ushort_vector(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned short int sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

int sum_int_vector(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    int sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

unsigned int sum_uint_vector(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned int sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

long int sum_long_vector(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long int sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

unsigned long int sum_ulong_vector(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long int sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

long long int sum_llong_vector(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long int sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

unsigned long long int sum_ullong_vector(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    unsigned long long int sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

float sum_float_vector(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.f;
    }
    float sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

double sum_double_vector(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    double sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// --------------------------------------------------------------------------------

long double sum_ldouble_vector(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    long double sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// ================================================================================
// ================================================================================
// AVERAGE VECTOR FUNCTIONS 

float average_char_vector(char_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (float)sum / vec->len;
}
// --------------------------------------------------------------------------------

float average_uchar_vector(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (float)sum / vec->len;
}
// --------------------------------------------------------------------------------

float average_short_vector(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (float)sum / vec->len;
}
// --------------------------------------------------------------------------------

float average_ushort_vector(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (float)sum / vec->len;
}
// --------------------------------------------------------------------------------

double average_int_vector(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (double)sum / vec->len;
}
// --------------------------------------------------------------------------------

double average_uint_vector(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (double)sum / vec->len;
}
// --------------------------------------------------------------------------------

double average_long_vector(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (double)sum / vec->len;
}
// --------------------------------------------------------------------------------

double average_ulong_vector(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (double)sum / vec->len;
}
// --------------------------------------------------------------------------------

long double average_llong_vector(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (long double)sum / vec->len;
}
// --------------------------------------------------------------------------------

long double average_ullong_vector(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long long sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return (long double)sum / vec->len;
}
// --------------------------------------------------------------------------------

float average_float_vector(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.f;
    }
    float sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum / vec->len;
}
// --------------------------------------------------------------------------------

double average_double_vector(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    double sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum / (double)vec->len;
}
// --------------------------------------------------------------------------------

long double average_ldouble_vector(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    long double sum = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum / (long double)vec->len;
}
// ================================================================================
// ================================================================================
// STDEV VECTOR FUNCTIONS 

float stdev_char_vector(char_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    float avg = average_char_vector(vec);
    float sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

float stdev_uchar_vector(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    float avg = average_uchar_vector(vec);
    float sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

float stdev_short_vector(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    float avg = average_short_vector(vec);
    float sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

float stdev_ushort_vector(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    float avg = average_ushort_vector(vec);
    float sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

double stdev_int_vector(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    double avg = average_int_vector(vec);
    double sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

double stdev_uint_vector(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    double avg = average_uint_vector(vec);
    double sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

double stdev_long_vector(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    double avg = average_long_vector(vec);
    double sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

double stdev_ulong_vector(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    double avg = average_ulong_vector(vec);
    double sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

long double stdev_llong_vector(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long double avg = average_llong_vector(vec);
    long double sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

long double stdev_ullong_vector(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0;
    }
    long double avg = average_ullong_vector(vec);
    long double sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

float stdev_float_vector(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.f;
    }
    float avg = average_float_vector(vec);
    float sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

double stdev_double_vector(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    double avg = average_double_vector(vec);
    double sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// --------------------------------------------------------------------------------

long double stdev_ldouble_vector(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return 0.;
    }
    long double avg = average_ldouble_vector(vec);
    long double sum_sq_diff = 0;
    for (size_t i = 0; i < vec->len; i++) {
        sum_sq_diff += (vec->data[i] - avg) * (vec->data[i] - avg);
    }
    return sqrt(sum_sq_diff / vec->len);
}
// ================================================================================
// ================================================================================
// CUMULATIVE SUM FUNCTIONS 

char_v* cumsum_char_vector(char_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_char_vector(1);
    }
    char_v* new_vec = init_char_vector(vec->len);
    char dat = vec->data[0];
    push_char_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_char_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

uchar_v* cumsum_uchar_vector(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_uchar_vector(1);
    }
    uchar_v* new_vec = init_uchar_vector(vec->len);
    unsigned char dat = vec->data[0];
    push_uchar_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_uchar_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

short_v* cumsum_short_vector(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_short_vector(1);
    }
    short_v* new_vec = init_short_vector(vec->len);
    short int dat = vec->data[0];
    push_short_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_short_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

ushort_v* cumsum_ushort_vector(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_ushort_vector(1);
    }
    ushort_v* new_vec = init_ushort_vector(vec->len);
    unsigned short int dat = vec->data[0];
    push_ushort_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_ushort_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

int_v* cumsum_int_vector(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_int_vector(1);
    }
    int_v* new_vec = init_int_vector(vec->len);
    int dat = vec->data[0];
    push_int_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_int_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

uint_v* cumsum_uint_vector(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_uint_vector(1);
    }
    uint_v* new_vec = init_uint_vector(vec->len);
    unsigned int dat = vec->data[0];
    push_uint_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_uint_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

long_v* cumsum_long_vector(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_long_vector(1);
    }
    long_v* new_vec = init_long_vector(vec->len);
    long int dat = vec->data[0];
    push_long_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_long_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

ulong_v* cumsum_ulong_vector(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_ulong_vector(1);
    }
    ulong_v* new_vec = init_ulong_vector(vec->len);
    unsigned long int dat = vec->data[0];
    push_ulong_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_ulong_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

llong_v* cumsum_llong_vector(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_llong_vector(1);
    }
    llong_v* new_vec = init_llong_vector(vec->len);
    long long int dat = vec->data[0];
    push_llong_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_llong_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

ullong_v* cumsum_ullong_vector(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_ullong_vector(1);
    }
    ullong_v* new_vec = init_ullong_vector(vec->len);
    unsigned long long int dat = vec->data[0];
    push_ullong_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_ullong_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

float_v* cumsum_float_vector(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_float_vector(1);
    }
    float_v* new_vec = init_float_vector(vec->len);
    float dat = vec->data[0];
    push_float_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_float_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

double_v* cumsum_double_vector(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_double_vector(1);
    }
    double_v* new_vec = init_double_vector(vec->len);
    double dat = vec->data[0];
    push_double_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_double_vector(new_vec, dat, i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

ldouble_v* cumsum_ldouble_vector(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        errno = EINVAL;
        return init_ldouble_vector(1);
    }
    ldouble_v* new_vec = init_ldouble_vector(vec->len);
    double dat = vec->data[0];
    push_ldouble_vector(new_vec, dat, 0);
    for (size_t i = 1; i < vec->len; i++) {
        dat += vec->data[i];
        push_ldouble_vector(new_vec, dat, i);
    }
    return new_vec;
}
// ================================================================================
// ================================================================================
// COPY VECTOR FUNCTIONS 

char_v* copy_char_vector(char_v* vec) {
    char_v* new_vec = init_char_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_char_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

uchar_v* copy_uchar_vector(uchar_v* vec) {
    uchar_v* new_vec = init_uchar_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_uchar_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

short_v* copy_short_vector(short_v* vec) {
    short_v* new_vec = init_short_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_short_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

ushort_v* copy_ushort_vector(ushort_v* vec) {
    ushort_v* new_vec = init_ushort_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_ushort_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

int_v* copy_int_vector(int_v* vec) {
    int_v* new_vec = init_int_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_int_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

uint_v* copy_uint_vector(uint_v* vec) {
    uint_v* new_vec = init_uint_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_uint_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

long_v* copy_long_vector(long_v* vec) {
    long_v* new_vec = init_long_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_long_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

ulong_v* copy_ulong_vector(ulong_v* vec) {
    ulong_v* new_vec = init_ulong_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_ulong_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

llong_v* copy_llong_vector(llong_v* vec) {
    llong_v* new_vec = init_llong_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_llong_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

ullong_v* copy_ullong_vector(ullong_v* vec) {
    ullong_v* new_vec = init_ullong_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_ullong_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

float_v* copy_float_vector(float_v* vec) {
    float_v* new_vec = init_float_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_float_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

double_v* copy_double_vector(double_v* vec) {
    double_v* new_vec = init_double_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_double_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

ldouble_v* copy_ldouble_vector(ldouble_v* vec) {
    ldouble_v* new_vec = init_ldouble_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_ldouble_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

bool_v* copy_bool_vector(bool_v* vec) {
    bool_v* new_vec = init_bool_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_bool_vector(new_vec, vec->data[i], i);
    }
    return new_vec;
}
// --------------------------------------------------------------------------------

string_v* copy_string_vector(string_v* vec) {
    string_v* new_vec = init_string_vector(vec->alloc);
    for (size_t i = 0; i < vec->len; i++) {
        push_string_vector(new_vec, vec->data[i].data, i);
    }
    return new_vec;
}
// ================================================================================
// ================================================================================
// TRIM VECTOR FUNCTIONS 

bool trim_char_vector(char_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    char* ptr = realloc(vec->data, sizeof(char) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_uchar_vector(uchar_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    unsigned char* ptr = realloc(vec->data, sizeof(unsigned char) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_short_vector(short_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    short int* ptr = realloc(vec->data, sizeof(short int) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_ushort_vector(ushort_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    unsigned short int* ptr = realloc(vec->data, sizeof(unsigned short int) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_int_vector(int_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    int* ptr = realloc(vec->data, sizeof(int) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_uint_vector(uint_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    unsigned int* ptr = realloc(vec->data, sizeof(unsigned int) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_long_vector(long_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    long int* ptr = realloc(vec->data, sizeof(long int) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_ulong_vector(ulong_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    unsigned long int* ptr = realloc(vec->data, sizeof(unsigned long int) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_llong_vector(llong_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    long long int* ptr = realloc(vec->data, sizeof(long long int) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_ullong_vector(ullong_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    unsigned long long int* ptr = realloc(vec->data, sizeof(unsigned long long int) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_float_vector(float_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    float* ptr = realloc(vec->data, sizeof(float) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_double_vector(double_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    double* ptr = realloc(vec->data, sizeof(double) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_ldouble_vector(ldouble_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    long double* ptr = realloc(vec->data, sizeof(long double) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_bool_vector(bool_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    bool* ptr = realloc(vec->data, sizeof(bool) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// --------------------------------------------------------------------------------

bool trim_string_vector(string_v* vec) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    // If vec->len is already equal to vec->alloc, no resizing needed
    if (vec->len == vec->alloc) {
        return true;
    }
    str* ptr = realloc(vec->data, sizeof(str) * vec->len);
    if (ptr == NULL) {
        // realloc failed, but vec->data still points to the original memory
        return false;
    }
    // Update the vector to use the newly allocated memory
    vec->data = ptr;
    vec->alloc = vec->len;
    return true;
}
// ================================================================================
// ================================================================================
// REPLACE INDEX FUNCTIONS 

bool replace_char_vector_index(char_v* vec, char dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_uchar_vector_index(uchar_v* vec, unsigned char dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_short_vector_index(short_v* vec, short int dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_ushort_vector_index(ushort_v* vec, unsigned short int dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_int_vector_index(int_v* vec, int dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_uint_vector_index(uint_v* vec, unsigned int dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_long_vector_index(long_v* vec, long int dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_ulong_vector_index(ulong_v* vec, unsigned long dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_llong_vector_index(llong_v* vec, long long int dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_ullong_vector_index(ullong_v* vec, unsigned long long dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_float_vector_index(float_v* vec, float dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_double_vector_index(double_v* vec, double dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_ldouble_vector_index(ldouble_v* vec, long double dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_bool_vector_index(bool_v* vec, bool dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    vec->data[index] = dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_string_vector_index(string_v* vec, char* dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    char* new_dat = malloc(sizeof(char) * strlen(dat) + 1);
    if (new_dat == NULL) {
        return false;
    }
    strcpy(new_dat, dat);
    free(vec->data[index].data);
    vec->data[index].data = new_dat;
    return true;
}
// --------------------------------------------------------------------------------

bool replace_str_vector_index(string_v* vec, str* dat, size_t index) {
    if (vec == NULL || vec->data == NULL || vec->len == 0) {
        // Handle invalid input or empty vector
        errno = EINVAL;
        return false;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return false;
    }
    char* new_dat = malloc(sizeof(char) * dat->len + 1);
    if (new_dat == NULL) {
        return false;
    }
    strcpy(new_dat, dat->data);
    free(vec->data[index].data);
    vec->data[index].data = new_dat;
    return true;
}
// ================================================================================
// ================================================================================
// ITERATOR SUPPORT (PRIVATE FUNCTIONS) 

static char* _char_v_begin(char_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static char* _char_v_end(char_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _char_v_next(char** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _char_v_prev(char** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static char _char_v_get(char** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return '\0';
    }
    return **current;
}
// ================================================================================

static unsigned char* _uchar_v_begin(uchar_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static unsigned char* _uchar_v_end(uchar_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _uchar_v_next(unsigned char** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _uchar_v_prev(unsigned char** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static unsigned char _uchar_v_get(unsigned char** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return '\0';
    }
    return **current;
}
// ================================================================================

static short int* _short_v_begin(short_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static short int* _short_v_end(short_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _short_v_next(short int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _short_v_prev(short int** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static short int _short_v_get(short int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0;
    }
    return **current;
}
// ================================================================================

static unsigned short int* _ushort_v_begin(ushort_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static unsigned short int* _ushort_v_end(ushort_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _ushort_v_next(unsigned short int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _ushort_v_prev(unsigned short int** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static unsigned short int _ushort_v_get(unsigned short int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0;
    }
    return **current;
}
// ================================================================================

static int* _int_v_begin(int_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static int* _int_v_end(int_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _int_v_next(int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _int_v_prev(int** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static int _int_v_get(int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0;
    }
    return **current;
}
// ================================================================================

static unsigned int* _uint_v_begin(uint_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static unsigned int* _uint_v_end(uint_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _uint_v_next(unsigned int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _uint_v_prev(unsigned int** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static unsigned int _uint_v_get(unsigned int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0;
    }
    return **current;
}
// ================================================================================

static long int* _long_v_begin(long_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static long int* _long_v_end(long_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _long_v_next(long int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _long_v_prev(long int** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static long int _long_v_get(long int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0;
    }
    return **current;
}
// ================================================================================

static unsigned long int* _ulong_v_begin(ulong_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static unsigned long int* _ulong_v_end(ulong_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _ulong_v_next(unsigned long int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _ulong_v_prev(unsigned long int** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static unsigned long int _ulong_v_get(unsigned long int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0;
    }
    return **current;
}
// ================================================================================

static long long int* _llong_v_begin(llong_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static long long int* _llong_v_end(llong_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _llong_v_next(long long int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _llong_v_prev(long long int** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static long long int _llong_v_get(long long int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0;
    }
    return **current;
}
// ================================================================================

static unsigned long long int* _ullong_v_begin(ullong_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static unsigned long long int* _ullong_v_end(ullong_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _ullong_v_next(unsigned long long int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _ullong_v_prev(unsigned long long int** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static unsigned long long int _ullong_v_get(unsigned long long int** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0;
    }
    return **current;
}
// ================================================================================

static float* _float_v_begin(float_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static float* _float_v_end(float_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _float_v_next(float** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _float_v_prev(float** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static float _float_v_get(float** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0.0f;
    }
    return **current;
} 
// ================================================================================

static double* _double_v_begin(double_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static double* _double_v_end(double_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _double_v_next(double** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _double_v_prev(double** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static double _double_v_get(double** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0.0;
    }
    return **current;
}
// ================================================================================

static long double* _ldouble_v_begin(ldouble_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static long double* _ldouble_v_end(ldouble_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _ldouble_v_next(long double** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _ldouble_v_prev(long double** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static long double _ldouble_v_get(long double** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return 0.0;
    }
    return **current;
}
// ================================================================================

static bool* _bool_v_begin(bool_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static bool* _bool_v_end(bool_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _bool_v_next(bool** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _bool_v_prev(bool** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static bool _bool_v_get(bool** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return false;
    }
    return **current;
}
// ================================================================================

static str* _string_v_begin(string_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data;
}
// --------------------------------------------------------------------------------

static str* _string_v_end(string_v* s) {
    if (!s || !s->data) {
        errno = EINVAL;
        return NULL;
    }
    return s->data + s->len;
}
// --------------------------------------------------------------------------------

static void _string_v_next(str** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)++;
}
// --------------------------------------------------------------------------------

static void _string_v_prev(str** current) {
 if ( current == NULL ) {
        errno = EINVAL;
        return;
    }
    (*current)--;
}
// --------------------------------------------------------------------------------

static char* _string_v_get(str** current) {
    if ( current == NULL ) {
        errno = EINVAL;
        return NULL;
    }
    return (*current)->data;
}
// ================================================================================
// ================================================================================
// INIT VECTOR ITERATOR FUNCTIONS 

char_v_iterator init_char_vector_iterator() {
    char_v_iterator iter;

    iter.begin = _char_v_begin;
    iter.end = _char_v_end;
    iter.next = _char_v_next;
    iter.prev = _char_v_prev;
    iter.get = _char_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

uchar_v_iterator init_uchar_vector_iterator() {
    uchar_v_iterator iter;

    iter.begin = _uchar_v_begin;
    iter.end = _uchar_v_end;
    iter.next = _uchar_v_next;
    iter.prev = _uchar_v_prev;
    iter.get = _uchar_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

short_v_iterator init_short_vector_iterator() {
    short_v_iterator iter;

    iter.begin = _short_v_begin;
    iter.end = _short_v_end;
    iter.next = _short_v_next;
    iter.prev = _short_v_prev;
    iter.get = _short_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

ushort_v_iterator init_ushort_vector_iterator() {
    ushort_v_iterator iter;

    iter.begin = _ushort_v_begin;
    iter.end = _ushort_v_end;
    iter.next = _ushort_v_next;
    iter.prev = _ushort_v_prev;
    iter.get = _ushort_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

int_v_iterator init_int_vector_iterator() {
    int_v_iterator iter;

    iter.begin = _int_v_begin;
    iter.end = _int_v_end;
    iter.next = _int_v_next;
    iter.prev = _int_v_prev;
    iter.get = _int_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

uint_v_iterator init_uint_vector_iterator() {
    uint_v_iterator iter;

    iter.begin = _uint_v_begin;
    iter.end = _uint_v_end;
    iter.next = _uint_v_next;
    iter.prev = _uint_v_prev;
    iter.get = _uint_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

long_v_iterator init_long_vector_iterator() {
    long_v_iterator iter;

    iter.begin = _long_v_begin;
    iter.end = _long_v_end;
    iter.next = _long_v_next;
    iter.prev = _long_v_prev;
    iter.get = _long_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

ulong_v_iterator init_ulong_vector_iterator() {
    ulong_v_iterator iter;

    iter.begin = _ulong_v_begin;
    iter.end = _ulong_v_end;
    iter.next = _ulong_v_next;
    iter.prev = _ulong_v_prev;
    iter.get = _ulong_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

llong_v_iterator init_llong_vector_iterator() {
    llong_v_iterator iter;

    iter.begin = _llong_v_begin;
    iter.end = _llong_v_end;
    iter.next = _llong_v_next;
    iter.prev = _llong_v_prev;
    iter.get = _llong_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

ullong_v_iterator init_ullong_vector_iterator() {
    ullong_v_iterator iter;

    iter.begin = _ullong_v_begin;
    iter.end = _ullong_v_end;
    iter.next = _ullong_v_next;
    iter.prev = _ullong_v_prev;
    iter.get = _ullong_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

float_v_iterator init_float_vector_iterator() {
    float_v_iterator iter;

    iter.begin = _float_v_begin;
    iter.end = _float_v_end;
    iter.next = _float_v_next;
    iter.prev = _float_v_prev;
    iter.get = _float_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

double_v_iterator init_double_vector_iterator() {
    double_v_iterator iter;

    iter.begin = _double_v_begin;
    iter.end = _double_v_end;
    iter.next = _double_v_next;
    iter.prev = _double_v_prev;
    iter.get = _double_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

ldouble_v_iterator init_ldouble_vector_iterator() {
    ldouble_v_iterator iter;

    iter.begin = _ldouble_v_begin;
    iter.end = _ldouble_v_end;
    iter.next = _ldouble_v_next;
    iter.prev = _ldouble_v_prev;
    iter.get = _ldouble_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

bool_v_iterator init_bool_vector_iterator() {
    bool_v_iterator iter;

    iter.begin = _bool_v_begin;
    iter.end = _bool_v_end;
    iter.next = _bool_v_next;
    iter.prev = _bool_v_prev;
    iter.get = _bool_v_get;
    return iter; 
}
// --------------------------------------------------------------------------------

string_v_iterator init_string_vector_iterator() {
    string_v_iterator iter;

    iter.begin = _string_v_begin;
    iter.end = _string_v_end;
    iter.next = _string_v_next;
    iter.prev = _string_v_prev;
    iter.get = _string_v_get;
    return iter; 
}
// ================================================================================
// ================================================================================
// eof
