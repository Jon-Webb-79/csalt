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
// INSERT FUNCTIONS 

void insert_char_avltree(charAVLTree* tree, char value) {
    tree->root = _insert_char(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_uchar_avltree(ucharAVLTree* tree, unsigned char value) {
    tree->root = _insert_uchar(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_short_avltree(shortAVLTree* tree, short int value) {
    tree->root = _insert_short(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_ushort_avltree(ushortAVLTree* tree, unsigned short value) {
    tree->root = _insert_ushort(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_int_avltree(intAVLTree* tree, int value) {
    tree->root = _insert_int(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_uint_avltree(uintAVLTree* tree, unsigned int value) {
    tree->root = _insert_uint(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_long_avltree(longAVLTree* tree, long int value) {
    tree->root = _insert_long(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_ulong_avltree(ulongAVLTree* tree, unsigned long int value) {
    tree->root = _insert_ulong(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_llong_avltree(llongAVLTree* tree, long long int value) {
    tree->root = _insert_llong(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_ullong_avltree(ullongAVLTree* tree, unsigned long long int value) {
    tree->root = _insert_ullong(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_float_avltree(floatAVLTree* tree, float value) {
    tree->root = _insert_float(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_double_avltree(doubleAVLTree* tree, double value) {
    tree->root = _insert_double(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_ldouble_avltree(ldoubleAVLTree* tree, long double value) {
    tree->root = _insert_ldouble(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_bool_avltree(boolAVLTree* tree, bool value) {
    tree->root = _insert_bool(tree, tree->root, value);
}
// --------------------------------------------------------------------------------

void insert_string_avltree(stringAVLTree* tree, char* value) {
    tree->root = _insert_string(tree, tree->root, value);
}
// ================================================================================
// ================================================================================
// eof
