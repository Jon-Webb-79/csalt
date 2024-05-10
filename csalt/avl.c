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

// Macro private to this file
#define MAX_POD(a, b) ((a) > (b) ? (a) : (b)) 
// ================================================================================
// ================================================================================
// PRIVATE GENERIC HELPER FUNCTIONS


static int _get_height(AVLNode* node) {
    if (!node)
        return 0;
    return node->height;
}
// --------------------------------------------------------------------------------

static int _balance_factor(AVLNode* node) {
    if (!node)
        return 0;
    return _get_height(node->left) - _get_height(node->right);
}
// --------------------------------------------------------------------------------

static void _update_height(AVLNode* node) {
    if (node)
        node->height = 1 + MAX_POD(_get_height(node->left), 
                                   _get_height(node->right));
}
// --------------------------------------------------------------------------------

static AVLNode* _rotate_left(AVLNode* node) {
    AVLNode* x = node->right;
    AVLNode* T2 = x->left;
    
    x->left = node;
    node->right = T2;

    _update_height(node);
    _update_height(x);

    return x;
}
// --------------------------------------------------------------------------------

static AVLNode* _rotate_right(AVLNode* node) {
    AVLNode* x = node->left;
    AVLNode* T2 = x->right;
    
    x->right = node;
    node->left = T2;

    _update_height(node);
    _update_height(x);

    return x;
}
// --------------------------------------------------------------------------------

static AVLNode* _balance(AVLNode* node) {
    _update_height(node);
    int balance = _balance_factor(node);

    // Left Left Case or Left Right Case
    if (balance > 1) {
        if (_balance_factor(node->left) < 0) {
            node->left = _rotate_left(node->left);
        }
        return _rotate_right(node);
    }
    // Right Right Case or Right Left Case
    if (balance < -1) {
        if (_balance_factor(node->right) > 0) {
            node->right = _rotate_right(node->right);
        }
        return _rotate_left(node);
    }
    return node;  // Return the node either unchanged or after rotation
}
// --------------------------------------------------------------------------------

static AVLNode* _find_min_node(AVLNode* node) {
    AVLNode* current = node;
    while (current && current->left)
        current = current->left;
    return current;
}
// --------------------------------------------------------------------------------

void _free_avltree(AVLNode* root) {
    if (!root) return;
    _free_avltree(root->right);
    _free_avltree(root->left);
    free(root);
}
// --------------------------------------------------------------------------------

void _free_string_avltree(AVLNode* root) {
    if (!root) return;
    _free_string_avltree(root->left);
    _free_string_avltree(root->right);

    stringAVLNode* strNode = (stringAVLNode*)root;
    free_string(strNode->data);
    free(strNode);
}
// ================================================================================
// ================================================================================ 

charAVLTree* init_char_avltree(bool duplicates) {
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

ucharAVLTree* init_uchar_avltree(bool duplicates) {
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

shortAVLTree* init_short_avltree(bool duplicates) {
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

ushortAVLTree* init_ushort_avltree(bool duplicates) {
    ushortAVLTree* ptr = malloc(sizeof(ushortAVLTree));
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
 
intAVLTree* init_int_avltree(bool duplicates) {
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

uintAVLTree* init_uint_avltree(bool duplicates) {
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

longAVLTree* init_long_avltree(bool duplicates) {
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

ulongAVLTree* init_ulong_avltree(bool duplicates) {
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

llongAVLTree* init_llong_avltree(bool duplicates) {
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

ullongAVLTree* init_ullong_avltree(bool duplicates) {
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

floatAVLTree* init_float_avltree(bool duplicates) {
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

doubleAVLTree* init_double_avltree(bool duplicates) {
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

ldoubleAVLTree* init_ldouble_avltree(bool duplicates) {
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

boolAVLTree* init_bool_avltree(bool duplicates) {
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

stringAVLTree* init_string_avltree(bool duplicates) {
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
// CREATE NODE 

static charAVLNode* _create_char_node(char value) {
    charAVLNode* ptr = malloc(sizeof(charAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static ucharAVLNode* _create_uchar_node(unsigned char value) {
    ucharAVLNode* ptr = malloc(sizeof(ucharAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static shortAVLNode* _create_short_node(short int value) {
    shortAVLNode* ptr = malloc(sizeof(shortAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static ushortAVLNode* _create_ushort_node(unsigned short int value) {
    ushortAVLNode* ptr = malloc(sizeof(ushortAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static intAVLNode* _create_int_node(int value) {
    intAVLNode* ptr = malloc(sizeof(intAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static uintAVLNode* _create_uint_node(unsigned int value) {
    uintAVLNode* ptr = malloc(sizeof(uintAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static longAVLNode* _create_long_node(long int value) {
    longAVLNode* ptr = malloc(sizeof(longAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static ulongAVLNode* _create_ulong_node(unsigned long int value) {
    ulongAVLNode* ptr = malloc(sizeof(ulongAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static llongAVLNode* _create_llong_node(long long int value) {
    llongAVLNode* ptr = malloc(sizeof(llongAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static ullongAVLNode* _create_ullong_node(unsigned long long int value) {
    ullongAVLNode* ptr = malloc(sizeof(ullongAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static floatAVLNode* _create_float_node(float value) {
    floatAVLNode* ptr = malloc(sizeof(floatAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static doubleAVLNode* _create_double_node(double value) {
    doubleAVLNode* ptr = malloc(sizeof(doubleAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static ldoubleAVLNode* _create_ldouble_node(long double value) {
    ldoubleAVLNode* ptr = malloc(sizeof(ldoubleAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static boolAVLNode* _create_bool_node(bool value) {
    boolAVLNode* ptr = malloc(sizeof(boolAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->data = value;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// --------------------------------------------------------------------------------

static stringAVLNode* _create_string_node(char* value) {
    stringAVLNode* ptr = malloc(sizeof(stringAVLNode));
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }
    str* val = init_string_nol(value);
    ptr->data = val;
    ptr->base.left = NULL;
    ptr->base.right = NULL;
    ptr->base.height = 1;
    return ptr;
}
// ================================================================================
// ================================================================================

static AVLNode* _insert_char(charAVLTree* tree, AVLNode* node, char value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_char_node(value);  
    }

    charAVLNode* datNode = (charAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_char(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_char(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_char(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_uchar(ucharAVLTree* tree, AVLNode* node, unsigned char value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_uchar_node(value);  
    }

    ucharAVLNode* datNode = (ucharAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_uchar(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_uchar(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_uchar(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_short(shortAVLTree* tree, AVLNode* node, short int value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_short_node(value);  
    }

    shortAVLNode* datNode = (shortAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_short(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_short(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_short(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_ushort(ushortAVLTree* tree, AVLNode* node, unsigned short int value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_ushort_node(value);  
    }

    ushortAVLNode* datNode = (ushortAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_ushort(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_ushort(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_ushort(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_int(intAVLTree* tree, AVLNode* node, int value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_int_node(value);  
    }

    intAVLNode* datNode = (intAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_int(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_int(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_int(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_uint(uintAVLTree* tree, AVLNode* node, unsigned int value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_uint_node(value);  
    }

    uintAVLNode* datNode = (uintAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_uint(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_uint(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_uint(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_long(longAVLTree* tree, AVLNode* node, long int value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_long_node(value);  
    }

    longAVLNode* datNode = (longAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_long(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_long(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_long(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_ulong(ulongAVLTree* tree, AVLNode* node, unsigned long int value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_ulong_node(value);  
    }

    ulongAVLNode* datNode = (ulongAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_ulong(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_ulong(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_ulong(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_llong(llongAVLTree* tree, AVLNode* node, long long int value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_llong_node(value);  
    }

    llongAVLNode* datNode = (llongAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_llong(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_llong(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_llong(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_ullong(ullongAVLTree* tree, AVLNode* node, unsigned long long int value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_ullong_node(value);  
    }

    ullongAVLNode* datNode = (ullongAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_ullong(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_ullong(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_ullong(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_float(floatAVLTree* tree, AVLNode* node, float value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_float_node(value);  
    }

    floatAVLNode* datNode = (floatAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_float(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_float(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_float(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_double(doubleAVLTree* tree, AVLNode* node, double value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_double_node(value);  
    }

    doubleAVLNode* datNode = (doubleAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_double(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_double(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_double(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_ldouble(ldoubleAVLTree* tree, AVLNode* node, long double value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_ldouble_node(value);  
    }

    ldoubleAVLNode* datNode = (ldoubleAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_ldouble(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_ldouble(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_ldouble(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_bool(boolAVLTree* tree, AVLNode* node, bool value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_bool_node(value);  
    }

    boolAVLNode* datNode = (boolAVLNode*) node;  

    if (value < datNode->data) {
        datNode->base.left = _insert_bool(tree, datNode->base.left, value);
    } else if (value > datNode->data) {
        datNode->base.right = _insert_bool(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_bool(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _insert_string(stringAVLTree* tree, AVLNode* node, char* value) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    if (!node) {
        tree->len++;
        return (AVLNode*) _create_string_node(value);  
    }

    stringAVLNode* datNode = (stringAVLNode*) node;  
    int cmp = strcmp(value, datNode->data->data);
    if (cmp < 0) {
        datNode->base.left = _insert_string(tree, datNode->base.left, value);
    } else if (cmp > 0) {
        datNode->base.right = _insert_string(tree, datNode->base.right, value);
    } else {
        if (tree->allow_duplicates) {
            // Duplicates allowed 
            datNode->base.right = _insert_string(tree, datNode->base.right, value);
        } else {
            return node;  // No action needed if the value already exists
        }
    }
    // Use the generic balance function
    return _balance(node);
}
// ================================================================================ 
// ================================================================================

static AVLNode* _remove_char_node(charAVLTree* tree, AVLNode* node, char data) {
    if (!node) return NULL;  // Base case: the key was not found.

    charAVLNode* charNode = (charAVLNode*) node;

    // Determine where to search the data.
    if (data < charNode->data) {
        charNode->base.left = _remove_char_node(tree, charNode->base.left, data);
    } else if (data > charNode->data) {
        charNode->base.right = _remove_char_node(tree, charNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!charNode->base.left || !charNode->base.right) {
            AVLNode* temp = charNode->base.left ? charNode->base.left : charNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *charNode = *(charAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            charAVLNode* temp = (charAVLNode*)_find_min_node(charNode->base.right);
            charNode->data = temp->data;
            charNode->base.right = _remove_char_node(tree, charNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_uchar_node(ucharAVLTree* tree, AVLNode* node, unsigned char data) {
    if (!node) return NULL;  // Base case: the key was not found.

    ucharAVLNode* datNode = (ucharAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_uchar_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_uchar_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(ucharAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            ucharAVLNode* temp = (ucharAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_uchar_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_short_node(shortAVLTree* tree, AVLNode* node, short int data) {
    if (!node) return NULL;  // Base case: the key was not found.

    shortAVLNode* datNode = (shortAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_short_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_short_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(shortAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            shortAVLNode* temp = (shortAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_short_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_ushort_node(ushortAVLTree* tree, AVLNode* node, unsigned short int data) {
    if (!node) return NULL;  // Base case: the key was not found.

    ushortAVLNode* datNode = (ushortAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_ushort_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_ushort_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(ushortAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            ushortAVLNode* temp = (ushortAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_ushort_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_int_node(intAVLTree* tree, AVLNode* node, int data) {
    if (!node) return NULL;  // Base case: the key was not found.

    intAVLNode* datNode = (intAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_int_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_int_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(intAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            intAVLNode* temp = (intAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_int_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_uint_node(uintAVLTree* tree, AVLNode* node, unsigned int data) {
    if (!node) return NULL;  // Base case: the key was not found.

    uintAVLNode* datNode = (uintAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_uint_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_uint_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(uintAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            uintAVLNode* temp = (uintAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_uint_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_long_node(longAVLTree* tree, AVLNode* node, long int data) {
    if (!node) return NULL;  // Base case: the key was not found.

    longAVLNode* datNode = (longAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_long_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_long_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(longAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            longAVLNode* temp = (longAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_long_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_ulong_node(ulongAVLTree* tree, AVLNode* node, unsigned long int data) {
    if (!node) return NULL;  // Base case: the key was not found.

    ulongAVLNode* datNode = (ulongAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_ulong_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_ulong_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(ulongAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            ulongAVLNode* temp = (ulongAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_ulong_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_llong_node(llongAVLTree* tree, AVLNode* node, long long int data) {
    if (!node) return NULL;  // Base case: the key was not found.

    llongAVLNode* datNode = (llongAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_llong_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_llong_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(llongAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            llongAVLNode* temp = (llongAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_llong_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_ullong_node(ullongAVLTree* tree, AVLNode* node, unsigned long long int data) {
    if (!node) return NULL;  // Base case: the key was not found.

    ullongAVLNode* datNode = (ullongAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_ullong_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_ullong_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(ullongAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            ullongAVLNode* temp = (ullongAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_ullong_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_float_node(floatAVLTree* tree, AVLNode* node, float data) {
    if (!node) return NULL;  // Base case: the key was not found.

    floatAVLNode* datNode = (floatAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_float_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_float_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(floatAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            floatAVLNode* temp = (floatAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_float_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_double_node(doubleAVLTree* tree, AVLNode* node, double data) {
    if (!node) return NULL;  // Base case: the key was not found.

    doubleAVLNode* datNode = (doubleAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_double_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_double_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(doubleAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            doubleAVLNode* temp = (doubleAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_double_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_ldouble_node(ldoubleAVLTree* tree, AVLNode* node, long double data) {
    if (!node) return NULL;  // Base case: the key was not found.

    ldoubleAVLNode* datNode = (ldoubleAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_ldouble_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_ldouble_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(ldoubleAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            ldoubleAVLNode* temp = (ldoubleAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_ldouble_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_bool_node(boolAVLTree* tree, AVLNode* node, bool data) {
    if (!node) return NULL;  // Base case: the key was not found.

    boolAVLNode* datNode = (boolAVLNode*) node;

    // Determine where to search the data.
    if (data < datNode->data) {
        datNode->base.left = _remove_bool_node(tree, datNode->base.left, data);
    } else if (data > datNode->data) {
        datNode->base.right = _remove_bool_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            if (!temp) {
                // No child case
                temp = node;
                node = NULL;
                free(temp);
                tree->len--;
            } else {
                // One child case
                *datNode = *(boolAVLNode*)temp;  // Copy the contents of the non-empty child
                free(temp);
                tree->len--;
            }
        } else {
            // Node with two children: Get the inorder successor
            boolAVLNode* temp = (boolAVLNode*)_find_min_node(datNode->base.right);
            datNode->data = temp->data;
            datNode->base.right = _remove_bool_node(tree, datNode->base.right, temp->data);
        }
    }

    if (!node) return node;

    // Update height and balance the tree
    node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    return _balance(node);
}
// --------------------------------------------------------------------------------

static AVLNode* _remove_string_node(stringAVLTree* tree, AVLNode* node, char* data) {
    if (!node) return NULL;  // Base case: the key was not found.

    stringAVLNode* datNode = (stringAVLNode*) node;
    int cmp = strcmp(data, datNode->data->data);

    // Search for the node to remove
    if (cmp < 0) {
        datNode->base.left = _remove_string_node(tree, datNode->base.left, data);
    } else if (cmp > 0) {
        datNode->base.right = _remove_string_node(tree, datNode->base.right, data);
    } else {
        // Node with only one child or no child
        if (!datNode->base.left || !datNode->base.right) {
            AVLNode* temp = datNode->base.left ? datNode->base.left : datNode->base.right;
            free_string(datNode->data);
            free(datNode);
            tree->len--;
            return temp;
        } else {
            // Node with two children: Get the inorder successor
            stringAVLNode* temp = (stringAVLNode*)_find_min_node(datNode->base.right);
            char* tempData = temp->data->data;  // Temporarily store data to be removed
            temp->data->data = datNode->data->data;  // Swap data
            datNode->data->data = tempData;

            // Recursively delete the inorder successor
            datNode->base.right = _remove_string_node(tree, datNode->base.right, tempData);
        }
    }

    if (node) {
        node = _balance(node);
        node->height = 1 + MAX_POD(_get_height(node->left), _get_height(node->right));
    }
    return node;
}
// ================================================================================
// ================================================================================
// INSERT FUNCTIONS 

void insert_char_avltree(charAVLTree* tree, char value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_char(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_uchar_avltree(ucharAVLTree* tree, unsigned char value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_uchar(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_short_avltree(shortAVLTree* tree, short int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_short(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_ushort_avltree(ushortAVLTree* tree, unsigned short value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_ushort(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_int_avltree(intAVLTree* tree, int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_int(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_uint_avltree(uintAVLTree* tree, unsigned int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_uint(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_long_avltree(longAVLTree* tree, long int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_long(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_ulong_avltree(ulongAVLTree* tree, unsigned long int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_ulong(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_llong_avltree(llongAVLTree* tree, long long int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_llong(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_ullong_avltree(ullongAVLTree* tree, unsigned long long int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_ullong(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_float_avltree(floatAVLTree* tree, float value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_float(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_double_avltree(doubleAVLTree* tree, double value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_double(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_ldouble_avltree(ldoubleAVLTree* tree, long double value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_ldouble(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_bool_avltree(boolAVLTree* tree, bool value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_bool(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_string_avltree(stringAVLTree* tree, char* value) {
    if (!tree || !value) {
        errno = EINVAL;
        return;
    }
    tree->root = _insert_string(tree, tree->root, value);
}
// ================================================================================
// ================================================================================

void remove_char_avltree(charAVLTree* tree, char value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_char_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_uchar_avltree(ucharAVLTree* tree, unsigned char value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_uchar_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_short_avltree(shortAVLTree* tree, short int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_short_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_ushort_avltree(ushortAVLTree* tree, unsigned short int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_ushort_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_int_avltree(intAVLTree* tree, int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_int_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_uint_avltree(uintAVLTree* tree, unsigned int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_uint_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_long_avltree(longAVLTree* tree, long int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_long_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_ulong_avltree(ulongAVLTree* tree, unsigned long int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_ulong_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_llong_avltree(llongAVLTree* tree, long long int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_llong_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_ullong_avltree(ullongAVLTree* tree, unsigned long long int value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_ullong_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_float_avltree(floatAVLTree* tree, float value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_float_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_double_avltree(doubleAVLTree* tree, double value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_double_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_ldouble_avltree(ldoubleAVLTree* tree, long double value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_ldouble_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_bool_avltree(boolAVLTree* tree, bool value) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_bool_node(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void remove_string_avltree(stringAVLTree* tree, char* value) {
    if (!tree || !value) {
        errno = EINVAL;
        return;
    }
    tree->root = _remove_string_node(tree, tree->root, value);
}
// ================================================================================
// ================================================================================

void free_char_avltree(charAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_uchar_avltree(ucharAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_short_avltree(shortAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_ushort_avltree(ushortAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_int_avltree(intAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_uint_avltree(uintAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// --------------------------------------------------------------------------------

void free_long_avltree(longAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_ulong_avltree(ulongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_llong_avltree(llongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// --------------------------------------------------------------------------------

void free_ullong_avltree(ullongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_float_avltree(floatAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// --------------------------------------------------------------------------------

void free_double_avltree(doubleAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_ldouble_avltree(ldoubleAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_bool_avltree(boolAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_avltree(tree->root);
    free(tree);
}
// -------------------------------------------------------------------------------- 

void free_string_avltree(stringAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return;
    }
    _free_string_avltree(tree->root);
    free(tree);
}
// ================================================================================
// ================================================================================
// ENABLE GARBAGE COLLECTION WITH GCC AND CLANG

void _freeCharAvltree(charAVLTree** root) {
    if (root && *root) {
        free_char_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeUCharAvltree(ucharAVLTree** root) {
    if (root && *root) {
        free_uchar_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeShortAvltree(shortAVLTree** root) {
    if (root && *root) {
        free_short_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeUShortAvltree(ushortAVLTree** root) {
    if (root && *root) {
        free_ushort_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeIntAvltree(intAVLTree** root) {
    if (root && *root) {
        free_int_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeUIntAvltree(uintAVLTree** root) {
    if (root && *root) {
        free_uint_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeLongAvltree(longAVLTree** root) {
    if (root && *root) {
        free_long_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeULongAvltree(ulongAVLTree** root) {
    if (root && *root) {
        free_ulong_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeLLongAvltree(llongAVLTree** root) {
    if (root && *root) {
        free_llong_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeULLongAvltree(ullongAVLTree** root) {
    if (root && *root) {
        free_ullong_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeFloatAvltree(floatAVLTree** root) {
    if (root && *root) {
        free_float_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeDoubleAvltree(doubleAVLTree** root) {
    if (root && *root) {
        free_double_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeLDoubleAvltree(ldoubleAVLTree** root) {
    if (root && *root) {
        free_ldouble_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeBoolAvltree(boolAVLTree** root) {
    if (root && *root) {
        free_bool_avltree(*root);
    }
}
// --------------------------------------------------------------------------------

void _freeStringAvltree(stringAVLTree** root) {
    if (root && *root) {
        free_string_avltree(*root);
    }
}
// ================================================================================ 
// ================================================================================

size_t char_avltree_size(charAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t uchar_avltree_size(ucharAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t short_avltree_size(shortAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t ushort_avltree_size(ushortAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t int_avltree_size(intAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t uint_avltree_size(uintAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t long_avltree_size(longAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// -------------------------------------------------------------------------------- 

size_t ulong_avltree_size(ulongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t llong_avltree_size(llongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t ullong_avltree_size(ullongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t float_avltree_size(floatAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t double_avltree_size(doubleAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t ldouble_avltree_size(ldoubleAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t bool_avltree_size(boolAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// --------------------------------------------------------------------------------

size_t string_avltree_size(stringAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return 0;
    }
    return tree->len;
}
// ================================================================================
// ================================================================================

static char _char_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return CHAR_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((charAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned char _uchar_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return UCHAR_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((ucharAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static short int _short_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return SHRT_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((shortAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned short int _ushort_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((ushortAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static int _int_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return INT_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((intAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned int _uint_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((uintAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static long int _long_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return LONG_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((longAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned long int _ulong_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return ULONG_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((ulongAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static long long int _llong_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return LLONG_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((llongAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned long long int _ullong_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return ULLONG_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((ullongAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static float _float_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return FLT_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((floatAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static double _double_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return DBL_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((doubleAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static long double _ldouble_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((ldoubleAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static bool _bool_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return false;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((boolAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static str* _string_min_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return NULL;
    }
    AVLNode* current = node;
    while (current->left) {
        current = current->left;
    }
    return ((stringAVLNode*)current)->data;
}
// ================================================================================

char min_char_avltree(charAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return CHAR_MAX;
    }
    return _char_min_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned char min_uchar_avltree(ucharAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return UCHAR_MAX;
    }
    return _uchar_min_val(tree->root);
}
// --------------------------------------------------------------------------------

short int min_short_avltree(shortAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return SHRT_MAX;
    }
    return _short_min_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned short int min_ushort_avltree(ushortAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    return _ushort_min_val(tree->root);
}
// --------------------------------------------------------------------------------

int min_int_avltree(intAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return INT_MAX;
    }
    return _int_min_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned int min_uint_avltree(uintAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return UINT_MAX;
    }
    return _uint_min_val(tree->root);
}
// --------------------------------------------------------------------------------

long int min_long_avltree(longAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return _long_min_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned long int min_ulong_avltree(ulongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return ULONG_MAX;
    }
    return _ulong_min_val(tree->root);
}
// --------------------------------------------------------------------------------

long long int min_llong_avltree(llongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return LLONG_MAX;
    }
    return _llong_min_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned long long int min_ullong_avltree(ullongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return ULLONG_MAX;
    }
    return _ullong_min_val(tree->root);
}
// --------------------------------------------------------------------------------

float min_float_avltree(floatAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return FLT_MAX;
    }
    return _float_min_val(tree->root);
}
// --------------------------------------------------------------------------------

double min_double_avltree(doubleAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return DBL_MAX;
    }
    return _double_min_val(tree->root);
}
// --------------------------------------------------------------------------------

long double min_ldouble_avltree(ldoubleAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    return _ldouble_min_val(tree->root);
}
// --------------------------------------------------------------------------------

bool min_bool_avltree(boolAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return false;
    }
    return _bool_min_val(tree->root);
}
// --------------------------------------------------------------------------------

str* min_string_avltree(stringAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    return _string_min_val(tree->root);
}
// ================================================================================
// ================================================================================
static char _char_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return CHAR_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((charAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned char _uchar_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return UCHAR_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((ucharAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static short int _short_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return SHRT_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((shortAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned short int _ushort_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((ushortAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static int _int_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return INT_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((intAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned int _uint_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((uintAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static long int _long_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return LONG_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((longAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned long int _ulong_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return ULONG_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((ulongAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static long long int _llong_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return LLONG_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((llongAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static unsigned long long int _ullong_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return ULLONG_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((ullongAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static float _float_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return FLT_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((floatAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static double _double_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return DBL_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((doubleAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static long double _ldouble_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((ldoubleAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static bool _bool_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return false;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((boolAVLNode*)current)->data;
}
// --------------------------------------------------------------------------------

static str* _string_max_val(AVLNode* node) {
    if (!node) {
        errno = EINVAL;
        return NULL;
    }
    AVLNode* current = node;
    while (current->right) {
        current = current->right;
    }
    return ((stringAVLNode*)current)->data;
}
// ================================================================================

char max_char_avltree(charAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return CHAR_MAX;
    }
    return _char_max_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned char max_uchar_avltree(ucharAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return UCHAR_MAX;
    }
    return _uchar_max_val(tree->root);
}
// --------------------------------------------------------------------------------

short int max_short_avltree(shortAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return SHRT_MAX;
    }
    return _short_max_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned short int max_ushort_avltree(ushortAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    return _ushort_max_val(tree->root);
}
// --------------------------------------------------------------------------------

int max_int_avltree(intAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return INT_MAX;
    }
    return _int_max_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned int max_uint_avltree(uintAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return UINT_MAX;
    }
    return _uint_max_val(tree->root);
}
// --------------------------------------------------------------------------------

long int max_long_avltree(longAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return _long_max_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned long int max_ulong_avltree(ulongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return ULONG_MAX;
    }
    return _ulong_max_val(tree->root);
}
// --------------------------------------------------------------------------------

long long int max_llong_avltree(llongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return LLONG_MAX;
    }
    return _llong_max_val(tree->root);
}
// --------------------------------------------------------------------------------

unsigned long long int max_ullong_avltree(ullongAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return ULLONG_MAX;
    }
    return _ullong_max_val(tree->root);
}
// --------------------------------------------------------------------------------

float max_float_avltree(floatAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return FLT_MAX;
    }
    return _float_max_val(tree->root);
}
// --------------------------------------------------------------------------------

double max_double_avltree(doubleAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return DBL_MAX;
    }
    return _double_max_val(tree->root);
}
// --------------------------------------------------------------------------------

long double max_ldouble_avltree(ldoubleAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    return _ldouble_max_val(tree->root);
}
// --------------------------------------------------------------------------------

bool max_bool_avltree(boolAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return false;
    }
    return _bool_max_val(tree->root);
}
// --------------------------------------------------------------------------------

str* max_string_avltree(stringAVLTree* tree) {
    if (!tree) {
        errno = EINVAL;
        return NULL;
    }
    return _string_max_val(tree->root);
}
// ================================================================================
// ================================================================================

static void traverse_char_and_add(char_v* vec, AVLNode* node, Boolean comp, char value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_char_and_add(vec, node->left, comp, value);

    char nodeValue = ((charAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_char_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_char_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_char_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_char_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_char_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_char_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_char_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_uchar_and_add(uchar_v* vec, AVLNode* node, Boolean comp, unsigned char value) {
    if (node == NULL) return;

    unsigned char nodeValue = ((ucharAVLNode*)node)->data;
    // Recursively traverse the left subtree
    traverse_uchar_and_add(vec, node->left, comp, value);

    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_uchar_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_uchar_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_uchar_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_uchar_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_uchar_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_uchar_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_uchar_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_short_and_add(short_v* vec, AVLNode* node, Boolean comp, short int value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_short_and_add(vec, node->left, comp, value);
    short int nodeValue = ((shortAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_short_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_short_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_short_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_short_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_short_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_short_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_short_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_ushort_and_add(ushort_v* vec, AVLNode* node, Boolean comp, unsigned short int value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_ushort_and_add(vec, node->left, comp, value);

    unsigned short int nodeValue = ((ushortAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_ushort_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_ushort_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_ushort_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_ushort_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_ushort_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_ushort_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_ushort_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_int_and_add(int_v* vec, AVLNode* node, Boolean comp, int value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_int_and_add(vec, node->left, comp, value);

    int nodeValue = ((intAVLNode*)node)->data;

    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_int_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_int_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_int_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_int_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_int_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_int_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_int_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_uint_and_add(uint_v* vec, AVLNode* node, Boolean comp, unsigned int value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_uint_and_add(vec, node->left, comp, value);

    unsigned int nodeValue = ((uintAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_uint_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_uint_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_uint_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_uint_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_uint_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_uint_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_uint_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_long_and_add(long_v* vec, AVLNode* node, Boolean comp, long int value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_long_and_add(vec, node->left, comp, value);

    long int nodeValue = ((longAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_long_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_long_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_long_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_long_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_long_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_long_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_long_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_ulong_and_add(ulong_v* vec, AVLNode* node, Boolean comp, unsigned long value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_ulong_and_add(vec, node->left, comp, value);

    unsigned long int nodeValue = ((ulongAVLNode*)node)->data;

    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_ulong_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_ulong_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_ulong_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_ulong_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_ulong_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_ulong_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_ulong_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_llong_and_add(llong_v* vec, AVLNode* node, Boolean comp, long long int value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_llong_and_add(vec, node->left, comp, value);

    long long int nodeValue = ((llongAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_llong_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_llong_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_llong_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_llong_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_llong_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_llong_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_llong_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_ullong_and_add(ullong_v* vec, AVLNode* node, Boolean comp, unsigned long long int value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_ullong_and_add(vec, node->left, comp, value);

    unsigned long long int nodeValue = ((ullongAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_ullong_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_ullong_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_ullong_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_ullong_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_ullong_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_ullong_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_ullong_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_float_and_add(float_v* vec, AVLNode* node, Boolean comp, float value) {
    if (node == NULL) return;
    float epsilon = 0.000001;
    // Recursively traverse the left subtree
    traverse_float_and_add(vec, node->left, comp, value);
    float nodeValue = ((floatAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_float_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_float_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue > value || fabs(nodeValue - value) < epsilon) {
                push_float_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue < value || fabs(nodeValue - value) < epsilon) {
                push_float_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (fabs(nodeValue - value) < epsilon) {
                push_float_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_float_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_float_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_double_and_add(double_v* vec, AVLNode* node, Boolean comp, double value) {
    if (node == NULL) return;
    double epsilon = 0.000001;
    // Recursively traverse the left subtree
    traverse_double_and_add(vec, node->left, comp, value);
    double nodeValue = ((doubleAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_double_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_double_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue > value || fabs(nodeValue - value) < epsilon) {
                push_double_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue < value || fabs(nodeValue - value) < epsilon) {
                push_double_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (fabs(nodeValue - value) < epsilon) { 
                push_double_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_double_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_double_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_ldouble_and_add(ldouble_v* vec, AVLNode* node, Boolean comp, long double value) {
    if (node == NULL) return;
    long double epsilon = 0.000001;
    // Recursively traverse the left subtree
    traverse_ldouble_and_add(vec, node->left, comp, value);
    long double nodeValue = ((ldoubleAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_ldouble_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_ldouble_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue > value || fabsl(nodeValue - value) < epsilon) {
                push_ldouble_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue < value || fabsl(nodeValue - value) < epsilon) {
                push_ldouble_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (fabsl(nodeValue - value) < epsilon) {
                push_ldouble_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_ldouble_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_ldouble_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_bool_and_add(bool_v* vec, AVLNode* node, Boolean comp, bool value) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_bool_and_add(vec, node->left, comp, value);
    bool nodeValue = ((boolAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (nodeValue > value) {
                push_bool_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (nodeValue < value) {
                push_bool_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (nodeValue >= value) {
                push_bool_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (nodeValue <= value) {
                push_bool_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (nodeValue == value) {
                push_bool_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_bool_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_bool_and_add(vec, node->right, comp, value);
}
// --------------------------------------------------------------------------------

static void traverse_string_and_add(string_v* vec, AVLNode* node, Boolean comp, char* value) {
    if (node == NULL) return;
    if (!value) return;

    // Recursively traverse the left subtree
    traverse_string_and_add(vec, node->left, comp, value);
    char* nodeValue = ((stringAVLNode*)node)->data->data;
    int cmp = strcmp(value, nodeValue);
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT:
            if (cmp < 0) {
                push_string_vector(vec, nodeValue, vec->len);
            }
            break;
        case LT:
            if (cmp > 0) {
                push_string_vector(vec, nodeValue, vec->len);
            }
            break;
        case GTE:
            if (cmp <= 0) {
                push_string_vector(vec, nodeValue, vec->len);
            }
            break;
        case LTE:
            if (cmp >= 0) {
                push_string_vector(vec, nodeValue, vec->len);
            }
            break;
        case EQ:
            if (cmp == 0) {
                push_string_vector(vec, nodeValue, vec->len);
            }
            break;
        case ALL:
            push_string_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_string_and_add(vec, node->right, comp, value);
}
// ================================================================================

char_v* avltree_char_values(charAVLTree* tree, char value, Boolean comp) {
    // Initialize a dynamic array to store results
    char_v* result = init_char_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_char_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

uchar_v* avltree_uchar_values(ucharAVLTree* tree, unsigned char value, Boolean comp) {
    // Initialize a dynamic array to store results
    uchar_v* result = init_uchar_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_uchar_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

short_v* avltree_short_values(shortAVLTree* tree, short value, Boolean comp) {
    // Initialize a dynamic array to store results
    short_v* result = init_short_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_short_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

ushort_v* avltree_ushort_values(ushortAVLTree* tree, unsigned short int value, Boolean comp) {
    // Initialize a dynamic array to store results
    ushort_v* result = init_ushort_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_ushort_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

int_v* avltree_int_values(intAVLTree* tree, int value, Boolean comp) {
    // Initialize a dynamic array to store results
    int_v* result = init_int_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_int_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

uint_v* avltree_uint_values(uintAVLTree* tree, unsigned int value, Boolean comp) {
    // Initialize a dynamic array to store results
    uint_v* result = init_uint_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_uint_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

long_v* avltree_long_values(longAVLTree* tree, long value, Boolean comp) {
    // Initialize a dynamic array to store results
    long_v* result = init_long_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_long_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

ulong_v* avltree_ulong_values(ulongAVLTree* tree, unsigned long value, Boolean comp) {
    // Initialize a dynamic array to store results
    ulong_v* result = init_ulong_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_ulong_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

llong_v* avltree_llong_values(llongAVLTree* tree, long long value, Boolean comp) {
    // Initialize a dynamic array to store results
    llong_v* result = init_llong_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_llong_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

ullong_v* avltree_ullong_values(ullongAVLTree* tree, unsigned long long value, Boolean comp) {
    // Initialize a dynamic array to store results
    ullong_v* result = init_ullong_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_ullong_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

float_v* avltree_float_values(floatAVLTree* tree, float value, Boolean comp) {
    // Initialize a dynamic array to store results
    float_v* result = init_float_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_float_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

double_v* avltree_double_values(doubleAVLTree* tree, double value, Boolean comp) {
    // Initialize a dynamic array to store results
    double_v* result = init_double_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_double_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

ldouble_v* avltree_ldouble_values(ldoubleAVLTree* tree, long double value, Boolean comp) {
    // Initialize a dynamic array to store results
    ldouble_v* result = init_ldouble_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_ldouble_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

bool_v* avltree_bool_values(boolAVLTree* tree, bool value, Boolean comp) {
    // Initialize a dynamic array to store results
    bool_v* result = init_bool_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_bool_and_add(result, tree->root, comp, value);

    return result;
}
// --------------------------------------------------------------------------------

string_v* avltree_string_values(stringAVLTree* tree, char* value, Boolean comp) {
    // Initialize a dynamic array to store results
    string_v* result = init_string_vector(10);

    // Start traversal from the root of the provided AVL tree
    if (tree && tree->root)
        traverse_string_and_add(result, tree->root, comp, value);

    return result;
}
// ================================================================================
// ================================================================================

static void traverse_char_and_or_add(char_v* vec, AVLNode* node, AndOrBoolean comp, char a, char b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_char_and_or_add(vec, node->left, comp, a, b);

    char nodeValue = ((charAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_char_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_char_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_char_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_char_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_char_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_char_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_char_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_char_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_char_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_uchar_and_or_add(uchar_v* vec, AVLNode* node, AndOrBoolean comp, unsigned char a, unsigned char b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_uchar_and_or_add(vec, node->left, comp, a, b);

    unsigned char nodeValue = ((ucharAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_uchar_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_uchar_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_uchar_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_uchar_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_uchar_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_uchar_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_uchar_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_uchar_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_uchar_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_short_and_or_add(short_v* vec, AVLNode* node, AndOrBoolean comp, 
                                      short int a, short int b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_short_and_or_add(vec, node->left, comp, a, b);

    short int nodeValue = ((shortAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_short_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_short_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_short_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_short_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_short_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_short_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_short_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_short_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_short_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_ushort_and_or_add(ushort_v* vec, AVLNode* node, AndOrBoolean comp, 
                                      unsigned short int a, unsigned short int b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_ushort_and_or_add(vec, node->left, comp, a, b);

    unsigned short int nodeValue = ((ushortAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_ushort_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_ushort_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_ushort_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_ushort_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_ushort_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_ushort_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_ushort_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_ushort_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_ushort_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_int_and_or_add(int_v* vec, AVLNode* node, AndOrBoolean comp, 
                                    int a, int b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_int_and_or_add(vec, node->left, comp, a, b);

    int nodeValue = ((intAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_int_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_int_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_int_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_int_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_int_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_int_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_int_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_int_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_int_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_uint_and_or_add(uint_v* vec, AVLNode* node, AndOrBoolean comp, 
                                    unsigned int a, unsigned int b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_uint_and_or_add(vec, node->left, comp, a, b);

    unsigned int nodeValue = ((uintAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_uint_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_uint_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_uint_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_uint_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_uint_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_uint_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_uint_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_uint_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_uint_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_long_and_or_add(long_v* vec, AVLNode* node, AndOrBoolean comp, 
                                     long int a, long int b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_long_and_or_add(vec, node->left, comp, a, b);

    long int nodeValue = ((longAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_long_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_long_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_long_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_long_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_long_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_long_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_long_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_long_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_long_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_ulong_and_or_add(ulong_v* vec, AVLNode* node, AndOrBoolean comp, 
                                      unsigned long int a, unsigned long int b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_ulong_and_or_add(vec, node->left, comp, a, b);

    unsigned long int nodeValue = ((ulongAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_ulong_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_ulong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_ulong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_ulong_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_ulong_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_ulong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_ulong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_ulong_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_ulong_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_llong_and_or_add(llong_v* vec, AVLNode* node, AndOrBoolean comp, 
                                      long long int a, long long int b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_llong_and_or_add(vec, node->left, comp, a, b);

    long long int nodeValue = ((llongAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_llong_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_llong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_llong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_llong_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_llong_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_llong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_llong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_llong_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_llong_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_ullong_and_or_add(ullong_v* vec, AVLNode* node, AndOrBoolean comp, 
                                       unsigned long long int a, unsigned long long int b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_ullong_and_or_add(vec, node->left, comp, a, b);

    unsigned long long int nodeValue = ((ullongAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_ullong_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_ullong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_ullong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_ullong_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_ullong_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_ullong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_ullong_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_ullong_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_ullong_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_float_and_or_add(float_v* vec, AVLNode* node, AndOrBoolean comp, 
                                      float a, float b) {
    if (node == NULL) return;
    float epsilon = 0.000001;
    // Recursively traverse the left subtree
    traverse_float_and_or_add(vec, node->left, comp, a, b);

    float nodeValue = ((floatAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_float_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue < b || fabs(nodeValue - b) < epsilon)
                push_float_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue > a || fabs(nodeValue - a) < epsilon || nodeValue < b)
                push_float_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue > a || fabs(nodeValue - a) < epsilon || nodeValue < b || fabs(nodeValue - b) < epsilon)
                push_float_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_float_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && (nodeValue < b || fabs(nodeValue - b) < epsilon))
                push_float_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if ((nodeValue > a || fabs(nodeValue - a) < epsilon) && nodeValue < b)
                push_float_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if ((nodeValue > a || fabs(nodeValue - a) < epsilon) && (nodeValue < b || fabs(nodeValue - b) < epsilon))
                push_float_vector(vec, nodeValue, vec->len);
            break;
    } 

    // Recursively traverse the right subtree
    traverse_float_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_double_and_or_add(double_v* vec, AVLNode* node, AndOrBoolean comp, 
                                       double a, double b) {
    if (node == NULL) return;
    double epsilon = 0.000001;
    // Recursively traverse the left subtree
    traverse_double_and_or_add(vec, node->left, comp, a, b);

    double nodeValue = ((doubleAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_double_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue < b || fabs(nodeValue - b) < epsilon)
                push_double_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue > a || fabs(nodeValue - a) < epsilon || nodeValue < b)
                push_double_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue > a || fabs(nodeValue - a) < epsilon || nodeValue < b || fabs(nodeValue - b) < epsilon)
                push_double_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_double_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && (nodeValue < b || fabs(nodeValue - b) < epsilon))
                push_double_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if ((nodeValue > a || fabs(nodeValue - a) < epsilon) && nodeValue < b)
                push_double_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if ((nodeValue > a || fabs(nodeValue - a) < epsilon) && (nodeValue < b || fabs(nodeValue - b) < epsilon))
                push_double_vector(vec, nodeValue, vec->len);
            break;
    } 

    // Recursively traverse the right subtree
    traverse_double_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_ldouble_and_or_add(ldouble_v* vec, AVLNode* node, AndOrBoolean comp, 
                                       long double a, long double b) {
    if (node == NULL) return;
    long double epsilon = 0.000001;
    // Recursively traverse the left subtree
    traverse_ldouble_and_or_add(vec, node->left, comp, a, b);

    long double nodeValue = ((ldoubleAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_ldouble_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue < b || fabsl(nodeValue - b) < epsilon)
                push_ldouble_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue > a || fabsl(nodeValue - a) < epsilon || nodeValue < b)
                push_ldouble_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue > a || fabsl(nodeValue - a) < epsilon || nodeValue < b || fabsl(nodeValue - b) < epsilon)
                push_ldouble_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_ldouble_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && (nodeValue < b || fabsl(nodeValue - b) < epsilon))
                push_ldouble_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if ((nodeValue > a || fabsl(nodeValue - a) < epsilon) && nodeValue < b)
                push_ldouble_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if ((nodeValue > a || fabsl(nodeValue - a) < epsilon) && (nodeValue < b || fabsl(nodeValue - b) < epsilon))
                push_ldouble_vector(vec, nodeValue, vec->len);
            break;
    } 

    // Recursively traverse the right subtree
    traverse_ldouble_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_bool_and_or_add(bool_v* vec, AVLNode* node, AndOrBoolean comp, 
                                     bool a, bool b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_bool_and_or_add(vec, node->left, comp, a, b);

    bool nodeValue = ((boolAVLNode*)node)->data;
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (nodeValue > a || nodeValue < b)
                push_bool_vector(vec, nodeValue, vec->len);
            break;
        case GT_OR_LTE:
            if (nodeValue > a || nodeValue <= b)
                push_bool_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LT:
            if (nodeValue >= a || nodeValue < b)
                push_bool_vector(vec, nodeValue, vec->len);
            break;
        case GTE_OR_LTE:
            if (nodeValue >= a || nodeValue <= b)
                push_bool_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LT:
            if (nodeValue > a && nodeValue < b)
                push_bool_vector(vec, nodeValue, vec->len);
            break;
        case GT_AND_LTE:
            if (nodeValue > a && nodeValue <= b)
                push_bool_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LT:
            if (nodeValue >= a && nodeValue < b)
                push_bool_vector(vec, nodeValue, vec->len);
            break;
        case GTE_AND_LTE:
            if (nodeValue >= a && nodeValue <= b)
                push_bool_vector(vec, nodeValue, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_bool_and_or_add(vec, node->right, comp, a, b);
}
// --------------------------------------------------------------------------------

static void traverse_string_and_or_add(string_v* vec, AVLNode* node, AndOrBoolean comp, 
                                       char* a, char* b) {
    if (node == NULL) return;

    // Recursively traverse the left subtree
    traverse_string_and_or_add(vec, node->left, comp, a, b);

    str* nodeValue = ((stringAVLNode*)node)->data;
    int compa = compare_strings_lit(nodeValue, a);
    int compb = compare_strings_lit(nodeValue, b);
    // Perform comparison and add value to the result vector if the condition is met
    switch (comp) {
        case GT_OR_LT:
            if (compa > 0 || compb < 0)
                push_string_vector(vec, nodeValue->data, vec->len);
            break;
        case GT_OR_LTE:
            if (compa > 0 || compb <= 0)
                push_string_vector(vec, nodeValue->data, vec->len);
            break;
        case GTE_OR_LT:
            if (compa >= 0 || compb < 0)
                push_string_vector(vec, nodeValue->data, vec->len);
            break;
        case GTE_OR_LTE:
            if (compa >= 0 || compb <= 0)
                push_string_vector(vec, nodeValue->data, vec->len);
            break;
        case GT_AND_LT:
            if (compa > 0 && compb < 0)
                push_string_vector(vec, nodeValue->data, vec->len);
            break;
        case GT_AND_LTE:
            if (compa > 0 && compb <= 0)
                push_string_vector(vec, nodeValue->data, vec->len);
            break;
        case GTE_AND_LT:
            if (compa >= 0 && compb < 0)
                push_string_vector(vec, nodeValue->data, vec->len);
            break;
        case GTE_AND_LTE:
            if (compa >= 0 && compb <= 0)
                push_string_vector(vec, nodeValue->data, vec->len);
            break;
    }

    // Recursively traverse the right subtree
    traverse_string_and_or_add(vec, node->right, comp, a, b);
}
// ================================================================================
// ================================================================================ 

char_v* avltree_char_and_or(charAVLTree* tree, char a, char b, AndOrBoolean comp) {
    char_v* result = init_char_vector(10);

    if (tree && tree->root)
        traverse_char_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

uchar_v* avltree_uchar_and_or(ucharAVLTree* tree, unsigned char a, unsigned char b, AndOrBoolean comp) {
    uchar_v* result = init_uchar_vector(10);

    if (tree && tree->root)
        traverse_uchar_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

short_v* avltree_short_and_or(shortAVLTree* tree, short int a, short int b, AndOrBoolean comp) {
    short_v* result = init_short_vector(10);

    if (tree && tree->root)
        traverse_short_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

ushort_v* avltree_ushort_and_or(ushortAVLTree* tree, unsigned short int a, unsigned short int b, AndOrBoolean comp) {
    ushort_v* result = init_ushort_vector(10);

    if (tree && tree->root)
        traverse_ushort_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

int_v* avltree_int_and_or(intAVLTree* tree, int a, int b, AndOrBoolean comp) {
    int_v* result = init_int_vector(10);

    if (tree && tree->root)
        traverse_int_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

uint_v* avltree_uint_and_or(uintAVLTree* tree, unsigned int a, unsigned int b, AndOrBoolean comp) {
    uint_v* result = init_uint_vector(10);

    if (tree && tree->root)
        traverse_uint_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

long_v* avltree_long_and_or(longAVLTree* tree, long int a, long int b, AndOrBoolean comp) {
    long_v* result = init_long_vector(10);

    if (tree && tree->root)
        traverse_long_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

ulong_v* avltree_ulong_and_or(ulongAVLTree* tree, unsigned long int a, unsigned long int b, AndOrBoolean comp) {
    ulong_v* result = init_ulong_vector(10);

    if (tree && tree->root)
        traverse_ulong_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

llong_v* avltree_llong_and_or(llongAVLTree* tree, long long int a, long long int b, AndOrBoolean comp) {
    llong_v* result = init_llong_vector(10);

    if (tree && tree->root)
        traverse_llong_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// -------------------------------------------------------------------------------- 

ullong_v* avltree_ullong_and_or(ullongAVLTree* tree, unsigned long long int a, unsigned long long int b, AndOrBoolean comp) {
    ullong_v* result = init_ullong_vector(10);

    if (tree && tree->root)
        traverse_ullong_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

float_v* avltree_float_and_or(floatAVLTree* tree, float a, float b, AndOrBoolean comp) {
    float_v* result = init_float_vector(10);

    if (tree && tree->root)
        traverse_float_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

double_v* avltree_double_and_or(doubleAVLTree* tree, double a, double b, AndOrBoolean comp) {
    double_v* result = init_double_vector(10);

    if (tree && tree->root)
        traverse_double_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

ldouble_v* avltree_ldouble_and_or(ldoubleAVLTree* tree, long double a, long double b, AndOrBoolean comp) {
    ldouble_v* result = init_ldouble_vector(10);

    if (tree && tree->root)
        traverse_ldouble_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

bool_v* avltree_bool_and_or(boolAVLTree* tree, bool a, bool b, AndOrBoolean comp) {
    bool_v* result = init_bool_vector(10);

    if (tree && tree->root)
        traverse_bool_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// --------------------------------------------------------------------------------

string_v* avltree_string_and_or(stringAVLTree* tree, char* a, char* b, AndOrBoolean comp) {
    string_v* result = init_string_vector(10);

    if (tree && tree->root)
        traverse_string_and_or_add(result, tree->root, comp, a, b);
    return result;
}
// ================================================================================
// ================================================================================
// eof
