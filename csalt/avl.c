// ================================================================================
// ================================================================================
// - File:    avl.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 27, 2024
// - Version: 1.0
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/avl.h"
// ================================================================================
// ================================================================================ 

charAVLTree* init_char_tree(bool duplicates) {
    charAVLTree* ptr = malloc(sizeof(charAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

ucharAVLTree* init_uchar_tree(bool duplicates) {
    ucharAVLTree* ptr = malloc(sizeof(ucharAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

shortAVLTree* init_short_tree(bool duplicates) {
    shortAVLTree* ptr = malloc(sizeof(shortAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------
 
intAVLTree* init_int_tree(bool duplicates) {
    intAVLTree* ptr = malloc(sizeof(intAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

uintAVLTree* init_uint_tree(bool duplicates) {
    uintAVLTree* ptr = malloc(sizeof(uintAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

longAVLTree* init_long_tree(bool duplicates) {
    longAVLTree* ptr = malloc(sizeof(longAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

ulongAVLTree* init_ulong_tree(bool duplicates) {
    ulongAVLTree* ptr = malloc(sizeof(ulongAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

llongAVLTree* init_llong_tree(bool duplicates) {
    llongAVLTree* ptr = malloc(sizeof(llongAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// -------------------------------------------------------------------------------- 

ullongAVLTree* init_ullong_tree(bool duplicates) {
    ullongAVLTree* ptr = malloc(sizeof(ullongAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

floatAVLTree* init_float_tree(bool duplicates) {
    floatAVLTree* ptr = malloc(sizeof(floatAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

doubleAVLTree* init_double_tree(bool duplicates) {
    doubleAVLTree* ptr = malloc(sizeof(doubleAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

ldoubleAVLTree* init_ldouble_tree(bool duplicates) {
    ldoubleAVLTree* ptr = malloc(sizeof(ldoubleAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

boolAVLTree* init_bool_tree(bool duplicates) {
    boolAVLTree* ptr = malloc(sizeof(boolAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

stringAVLTree* init_string_tree(bool duplicates) {
    stringAVLTree* ptr = malloc(sizeof(stringAVLTree));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->root = NULL;
    ptr->allow_duplicates = duplicates;
    return ptr; 
}
// --------------------------------------------------------------------------------

InitAVLFunc init_avltree(dtype dat_type) {
    switch(dat_type) {
        case dChar: return (InitAVLFunc)init_char_avltree;
        case dUChar: return (InitAVLFunc)init_uchar_avltree;
        case dShort: return (InitAVLFunc)init_short_avltree;
        case dUShort: return (InitAVLFunc)init_ushort_avltree;
        case dInt: return (InitAVLFunc)init_int_avltree;
        case dUInt: return (InitAVLFunc)init_uint_avltree;
        case dLong: return (InitAVLFunc)init_long_avltree;
        case dULong: return (InitAVLFunc)init_ulong_avltree;
        case dLongLong: return (InitAVLFunc)init_llong_avltree;
        case dULongLong: return (InitAVLFunc)init_ullong_avltree;
        case dFloat: return (InitAVLFunc)init_float_avltree;
        case dDouble: return (InitAVLFunc)init_double_avltree;
        case dLDouble: return (InitAVLFunc)init_ldouble_avltree;
        case dBool: return (InitAVLFunc)init_bool_avltree;
        case dString: return (InitAVLFunc)init_string_avltree;
        default: return NULL;
    } 
}
// ================================================================================
// ================================================================================
// eof
