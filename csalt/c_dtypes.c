// ================================================================================
// ================================================================================
// - File:    dtypes.c
// - Purpose: This file contains implementations for functions that are used 
//            to register data types
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 22, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_dtypes.h"
#include <stdio.h>
// ================================================================================ 
// ================================================================================ 
// CONSTANTS FOR TRANSLATION UNIT 

static dtype_t registry[MAX_DTYPES];
static size_t  registry_count  = 0u;
static bool    reg_initialized = false;
// ================================================================================ 
// ================================================================================ 

bool init_dtype_registry(void) {
    if (reg_initialized == true) return true;

    static const dtype_t builtins[] = {
        { FLOAT_TYPE,   sizeof(float),          "float"         },
        { DOUBLE_TYPE,  sizeof(double),         "double"        },
        { LDOUBLE_TYPE, sizeof(long double),    "long double"   },  // fixed size
        { CHAR_TYPE,    sizeof(char),           "char"          },
        { UCHAR_TYPE,   sizeof(unsigned char),  "unsigned char" },
        { INT8_TYPE,    sizeof(int8_t),         "int8_t"        },
        { UINT8_TYPE,   sizeof(uint8_t),        "uint8_t"       },  // was duplicated
        { INT16_TYPE,   sizeof(int16_t),        "int16_t"       },
        { UINT16_TYPE,  sizeof(uint16_t),       "uint16_t"      },
        { INT32_TYPE,   sizeof(int32_t),        "int32_t"       },
        { UINT32_TYPE,  sizeof(uint32_t),       "uint32_t"      },
        { INT64_TYPE,   sizeof(int64_t),        "int64_t"       },
        { UINT64_TYPE,  sizeof(uint64_t),       "uint64_t"      },
        { BOOL_TYPE,    sizeof(bool),           "bool"          },
        { SIZE_T_TYPE,  sizeof(size_t),         "size_t"        },  // was missing
    };
    size_t num_builtins = sizeof(builtins) / sizeof(builtins[0]);
    for (size_t i = 0u; i < num_builtins; i++) {
        if (register_dtype(&builtins[i]) == false) return false;
    }
    reg_initialized = true;
    return true;
}
// --------------------------------------------------------------------------------

bool register_dtype(const dtype_t* desc) {
    if (desc == NULL)                    return false;
    if (desc->id == UNKNOWN_TYPE)        return false;   // add this
    if (desc->data_size == 0u)           return false;
    if (registry_count >= MAX_DTYPES)    return false;
    if (lookup_dtype(desc->id) != NULL)  return false;
    registry[registry_count] = *desc;
    registry_count++;
    return true;
}
// --------------------------------------------------------------------------------

const dtype_t* lookup_dtype(dtype_id_t id) {
    for (size_t i = 0u; i < registry_count; i++) {
        if (registry[i].id == id) return &registry[i];
    }
    return NULL;
}
// --------------------------------------------------------------------------------

bool ensure_dtype_registered(const dtype_t* desc) {
    if (desc == NULL)                   return false;
    if (init_dtype_registry() == false) return false;
    if (lookup_dtype(desc->id) != NULL) return true;
    return register_dtype(desc);
}
// --------------------------------------------------------------------------------

size_t available_dtype_slots(void) {
    return MAX_DTYPES - registry_count;
}
// ================================================================================
// ================================================================================
// eof
