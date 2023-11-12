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
        fprintf(stderr, "Error: Struct Malloc failure in init_char_vector\n");
        return NULL;
    }
    char* data_ptr = malloc(buff * sizeof(char));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_char_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_uchar_vector\n");
        return NULL;
    }
    unsigned char* data_ptr = malloc(buff * sizeof(unsigned char));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_uchar_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_short_vector\n");
        return NULL;
    }
    short int* data_ptr = malloc(buff * sizeof(short int));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_short_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_ushort_vector\n");
        return NULL;
    }
    unsigned short int* data_ptr = malloc(buff * sizeof(unsigned short int));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_ushort_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_int_vector\n");
        return NULL;
    }
    int* data_ptr = malloc(buff * sizeof(int));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_int_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_uint_vector\n");
        return NULL;
    }
    unsigned int* data_ptr = malloc(buff * sizeof(unsigned int));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_uint_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_long_vector\n");
        return NULL;
    }
    long int* data_ptr = malloc(buff * sizeof(long int));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_long_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_ulong_vector\n");
        return NULL;
    }
    unsigned long int* data_ptr = malloc(buff * sizeof(unsigned long int));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_ulong_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_llong_vector\n");
        return NULL;
    }
    long long int* data_ptr = malloc(buff * sizeof(long long int));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_llong_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_ullong_vector\n");
        return NULL;
    }
    unsigned long long int* data_ptr = malloc(buff * sizeof(unsigned long long int));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_ullong_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_float_vector\n");
        return NULL;
    }
    float* data_ptr = malloc(buff * sizeof(float));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_float_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_double_vector\n");
        return NULL;
    }
    double* data_ptr = malloc(buff * sizeof(double));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_double_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_ldouble_vector\n");
        return NULL;
    }
    long double* data_ptr = malloc(buff * sizeof(long double));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_ldouble_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_bool_vector\n");
        return NULL;
    }
    bool* data_ptr = malloc(buff * sizeof(bool));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_bool_vector\n");
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
        fprintf(stderr, "Error: Struct Malloc failure in init_string_vector\n");
        return NULL;
    }
    str* data_ptr = malloc(buff * sizeof(str));
    if ( data_ptr == NULL) {
        fprintf(stderr, "Error: data Malloc failure in init_string_vector\n");
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
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_uchar_vector(uchar_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_short_vector(short_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ushort_vector(ushort_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_int_vector(int_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_uint_vector(uint_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_long_vector(long_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ulong_vector(ulong_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_llong_vector(llong_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ullong_vector(ullong_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_float_vector(float_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_double_vector(double_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_ldouble_vector(ldouble_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_bool_vector(bool_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free
    // Now that we know vec is not NULL, it's safe to check vec->data.
    if (vec->data != NULL) {
        free(vec->data); // Free the data pointer if it's not NULL.
        vec->data = NULL; // Set the data pointer to NULL to avoid double-free.
    }
    free(vec); // Free the struct pointer itself.
}
// --------------------------------------------------------------------------------

void free_string_vector(string_v* vec) {
    if (vec == NULL) return; // If the struct pointer is NULL, there's nothing to free

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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_char_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_char_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_uchar_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_uchar_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_short_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_short_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_ushort_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_ushort_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_int_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_int_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_uint_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_long_vector\n");
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

        long int* new_data = realloc(vec->data, new_alloc * sizeof(long int));
        if (!new_data) {
            fprintf(stderr, "Error: Realloc failed in push_long_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_ulong_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_ulong_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_llong_vector\n");
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

        long long int* new_data = realloc(vec->data, new_alloc * sizeof(long long int));
        if (!new_data) {
            fprintf(stderr, "Error: Realloc failed in push_llong_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_ullong_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_ullong_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_float_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_float_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_float_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_double_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_ldouble_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_ldouble_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_bool_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_bool_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_string_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_string_vector\n");
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
    if (!vec || index > vec->len) {
        fprintf(stderr, "Error: Invalid pointer or index passed to push_str_vector\n");
        return false;
    }

    if ( !var || !var->data ) {
        fprintf(stderr, "Error: Null str pointer passed to push_str_vector\n");
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
            fprintf(stderr, "Error: Realloc failed in push_str_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to get_char_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_char_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned char get_uchar_vector(uchar_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_uchar_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_uchar_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

short int get_short_vector(short_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_short_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_short_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned short int get_ushort_vector(ushort_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_ushort_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_ushort_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

int get_int_vector(int_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_int_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_int_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned int get_uint_vector(uint_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_uint_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_uint_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

long int get_long_vector(long_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_long_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_long_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned long int get_ulong_vector(ulong_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_ulong_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_ulong_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

long long int get_llong_vector(llong_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_llong_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_llong_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

unsigned long long int get_ullong_vector(ullong_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_ulong_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_ullong_vector\n");
        errno = ERANGE; // Result not within range
        return 0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

float get_float_vector(float_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_float_vector\n");
        errno = EINVAL;
        return 0.0f;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_float_vector\n");
        errno = ERANGE; // Result not within range
        return 0.0f; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

double get_double_vector(double_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_double_vector\n");
        errno = EINVAL;
        return 0.0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_double_vector\n");
        errno = ERANGE; // Result not within range
        return 0.0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

long double get_ldouble_vector(ldouble_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_ldouble_vector\n");
        errno = EINVAL;
        return 0.0;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_ldoble_vector\n");
        errno = ERANGE; // Result not within range
        return 0.0; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

bool get_bool_vector(bool_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_bool_vector\n");
        errno = EINVAL;
        return false;
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_bool_vector\n");
        errno = ERANGE; // Result not within range
        return false; // Return value is irrelevant as we're signaling an error with errno
    }
    errno = 0;
    return vec->data[index];
}
// --------------------------------------------------------------------------------

char* get_string_vector(string_v* vec, size_t index) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to get_string_vector\n");
        errno = EINVAL;
        return "\0";
    }
    if (index >= vec->len) {
        fprintf(stderr, "Error: Index out of bounds in get_string_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to char_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t uchar_vector_length(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to uchar_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t short_vector_length(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to short_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t ushort_vector_length(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to ushort_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t int_vector_length(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to int_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t uint_vector_length(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to uint_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t long_vector_length(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to long_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t ulong_vector_length(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to ulong_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t llong_vector_length(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to llong_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t ullong_vector_length(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to ullong_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t float_vector_length(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to float_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t double_vector_length(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to double_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t ldouble_vector_length(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to ldouble_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t bool_vector_length(bool_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to bool_vector_length\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->len;
}
// --------------------------------------------------------------------------------

size_t string_vector_length(string_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to string_vector_length\n");
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
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t uchar_vector_memory(uchar_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t short_vector_memory(short_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t ushort_vector_memory(ushort_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t int_vector_memory(int_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------


size_t uint_vector_memory(uint_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t long_vector_memory(long_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t ulong_vector_memory(ulong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t llong_vector_memory(llong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t ullong_vector_memory(ullong_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t float_vector_memory(float_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t double_vector_memory(double_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t ldouble_vector_memory(ldouble_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t bool_vector_memory(bool_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return vec->alloc;
}
// --------------------------------------------------------------------------------

size_t string_vector_memory(string_v* vec) {
    if ( vec == NULL || vec->data == NULL ) {
        fprintf(stderr, "Error: Null pointer passed to char_vector_memory\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_char_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_char_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_uchar_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_uchar_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_ushort_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_ushort_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_ushort_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_ushort_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_int_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_int_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_uint_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_uint_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_long_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_long_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_ulong_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_ulong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_llong_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_llong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_ullong_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_ullong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_float_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_float_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_double_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_double_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_ldouble_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_ldouble_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_bool_vector\n");
        errno = EINVAL;
        return 0;
    }
    if (index > vec->len -1) {
        fprintf(stderr, "Error: Index out of bounds in pop_bool_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to pop_string_vector\n");
        errno = EINVAL;
        return NULL;
    }
    if (index > vec->len - 1) {
        fprintf(stderr, "Error: Index out of bounds in pop_string_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_char_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_uchar_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_short_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_ushort_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_int_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_uint_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_long_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_ulong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_llong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_ullong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_double_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_double_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_ldouble_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_bool_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to reverse_string_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_char_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_uchar_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_short_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_ushort_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_int_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_uint_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to min_char_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_ulong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_llong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_ullong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_float_vector\n");
        errno = EINVAL;
        return 0.0f;
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
        fprintf(stderr, "Error: Null pointer passed to max_double_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to max_ldouble_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to max_char_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_uchar_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_short_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_ushort_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_int_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_uint_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to min_char_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_ulong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_llong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_ullong_vector\n");
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
        fprintf(stderr, "Error: Null pointer passed to max_float_vector\n");
        errno = EINVAL;
        return 0.0f;
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
        fprintf(stderr, "Error: Null pointer passed to max_double_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to max_ldouble_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_char_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_uchar_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to short_uchar_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_ushort_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_int_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_uint_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_long_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_ulong_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_llong_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_ullong_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_float_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_double_vector\n");
        errno = EINVAL;
        return 0.0;
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
        fprintf(stderr, "Error: Null pointer passed to sum_ldouble_vector\n");
        errno = EINVAL;
        return 0.0;
    }
    long double sum = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        sum += vec->data[i];
    }
    return sum;
}
// ================================================================================
// ================================================================================
// eof
