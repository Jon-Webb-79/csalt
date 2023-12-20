// ================================================================================
// ================================================================================
// - File:    slist.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 17, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/slist.h"
// ================================================================================
// ================================================================================ 

char_sl* init_char_sllist() {
    char_sl* ptr = malloc(sizeof(char_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

uchar_sl* init_uchar_sllist() {
    uchar_sl* ptr = malloc(sizeof(uchar_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

short_sl* init_short_sllist() {
    short_sl* ptr = malloc(sizeof(short_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

ushort_sl* init_ushort_sllist() {
    ushort_sl* ptr = malloc(sizeof(ushort_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

int_sl* init_int_sllist() {
    int_sl* ptr = malloc(sizeof(int_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

uint_sl* init_uint_sllist() {
    uint_sl* ptr = malloc(sizeof(uint_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

long_sl* init_long_sllist() {
    long_sl* ptr = malloc(sizeof(long_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

ulong_sl* init_ulong_sllist() {
    ulong_sl* ptr = malloc(sizeof(ulong_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

llong_sl* init_llong_sllist() {
    llong_sl* ptr = malloc(sizeof(llong_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

ullong_sl* init_ullong_sllist() {
    ullong_sl* ptr = malloc(sizeof(ullong_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

float_sl* init_float_sllist() {
    float_sl* ptr = malloc(sizeof(float_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

double_sl* init_double_sllist() {
    double_sl* ptr = malloc(sizeof(double_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

ldouble_sl* init_ldouble_sllist() {
    ldouble_sl* ptr = malloc(sizeof(ldouble_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

bool_sl* init_bool_sllist() {
    bool_sl* ptr = malloc(sizeof(bool_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

string_sl* init_string_sllist() {
    string_sl* ptr = malloc(sizeof(string_sl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

InitSListFunc init_sllist(dtype dat_type) {
    switch(dat_type) {
        case dChar: return (InitSListFunc)init_char_sllist;
        case dUChar: return (InitSListFunc)init_uchar_sllist;
        case dShort: return (InitSListFunc)init_short_sllist;
        case dUShort: return (InitSListFunc)init_ushort_sllist;
        case dInt: return (InitSListFunc)init_int_sllist;
        case dUInt: return (InitSListFunc)init_uint_sllist;
        case dLong: return (InitSListFunc)init_long_sllist;
        case dULong: return (InitSListFunc)init_ulong_sllist;
        case dLongLong: return (InitSListFunc)init_llong_sllist;
        case dULongLong: return (InitSListFunc)init_ullong_sllist;
        case dFloat: return (InitSListFunc)init_float_sllist;
        case dDouble: return (InitSListFunc)init_double_sllist;
        case dLDouble: return (InitSListFunc)init_ldouble_sllist;
        case dBool: return (InitSListFunc)init_bool_sllist;
        case dString: return (InitSListFunc)init_string_sllist;
        default: return NULL;
    } 
}
// ================================================================================
// ================================================================================
// PUSH SINGLY LINKED LIST

bool push_char_sllist(char_sl* sllist, char data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    char_slnode* newNode = malloc(sizeof(char_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        char_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_uchar_sllist(uchar_sl* sllist, unsigned char data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    uchar_slnode* newNode = malloc(sizeof(uchar_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        uchar_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_short_sllist(short_sl* sllist, short int data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    short_slnode* newNode = malloc(sizeof(short_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        short_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_ushort_sllist(ushort_sl* sllist, unsigned short int data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    ushort_slnode* newNode = malloc(sizeof(ushort_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        ushort_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_int_sllist(int_sl* sllist, int data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    int_slnode* newNode = malloc(sizeof(int_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        int_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_uint_sllist(uint_sl* sllist, unsigned int data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    uint_slnode* newNode = malloc(sizeof(uint_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        uint_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_long_sllist(long_sl* sllist, long int data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    long_slnode* newNode = malloc(sizeof(long_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        long_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_ulong_sllist(ulong_sl* sllist, unsigned long int data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    ulong_slnode* newNode = malloc(sizeof(ulong_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        ulong_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_llong_sllist(llong_sl* sllist, long long int data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    llong_slnode* newNode = malloc(sizeof(llong_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        llong_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_ullong_sllist(ullong_sl* sllist, unsigned long long int data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    ullong_slnode* newNode = malloc(sizeof(ullong_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        ullong_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_float_sllist(float_sl* sllist, float data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    float_slnode* newNode = malloc(sizeof(float_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        float_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_double_sllist(double_sl* sllist, double data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    double_slnode* newNode = malloc(sizeof(double_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        double_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_ldouble_sllist(ldouble_sl* sllist, long double data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    ldouble_slnode* newNode = malloc(sizeof(ldouble_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        ldouble_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_bool_sllist(bool_sl* sllist, bool data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    bool_slnode* newNode = malloc(sizeof(bool_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = data;

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        bool_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_string_sllist(string_sl* sllist, char* data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    string_slnode* newNode = malloc(sizeof(string_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    str* ptr = malloc(sizeof(str));
    if ( !ptr ) {
        errno = ENOMEM;
        free(newNode);
        return false;
    }
    newNode->data = ptr;
    strcpy(newNode->data->data, data);

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        string_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// --------------------------------------------------------------------------------

bool push_str_sllist(string_sl* sllist, str* data, size_t index) {
    if ( !sllist ) {
        errno = EINVAL;
        return false;
    }

    if (index > sllist->len) {
        errno = ERANGE;
        return false;
    }
    string_slnode* newNode = malloc(sizeof(string_slnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    str* ptr = malloc(sizeof(str));
    if ( !ptr ) {
        errno = ENOMEM;
        free(newNode);
        return false;
    }
    newNode->data = ptr;
    strcpy(newNode->data->data, data->data);

    // Insert at the head (works for empty and non-empty list)
    if (index == 0) {
        newNode->next = sllist->head;
        sllist->head = newNode;
    }
    // Insert at the end or middle
    else {
        string_slnode* current = sllist->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    sllist->len++; 
    return true;
}
// ================================================================================
// ================================================================================
// FREE SINGLE LINKED LIST

void free_char_sllist(char_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    char_slnode* current = sllist->head;
    char_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_uchar_sllist(uchar_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    uchar_slnode* current = sllist->head;
    uchar_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_short_sllist(short_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    short_slnode* current = sllist->head;
    short_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_ushort_sllist(ushort_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    ushort_slnode* current = sllist->head;
    ushort_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_int_sllist(int_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    int_slnode* current = sllist->head;
    int_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_uint_sllist(uint_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    uint_slnode* current = sllist->head;
    uint_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_long_sllist(long_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    long_slnode* current = sllist->head;
    long_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_ulong_sllist(ulong_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    ulong_slnode* current = sllist->head;
    ulong_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_llong_sllist(llong_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    llong_slnode* current = sllist->head;
    llong_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_ullong_sllist(ullong_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    ullong_slnode* current = sllist->head;
    ullong_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_float_sllist(float_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    float_slnode* current = sllist->head;
    float_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_double_sllist(double_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    double_slnode* current = sllist->head;
    double_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_ldouble_sllist(ldouble_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    ldouble_slnode* current = sllist->head;
    ldouble_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_bool_sllist(bool_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    bool_slnode* current = sllist->head;
    bool_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// --------------------------------------------------------------------------------

void free_string_sllist(string_sl* sllist) {
    if (!sllist) {
        errno = EINVAL;
        return;
    }

    string_slnode* current = sllist->head;
    string_slnode* next;

    while (current != NULL) {
        next = current->next; // Save the pointer to the next node
        free_string(current->data); // Free string struct
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }

    free(sllist);
}
// ================================================================================
// ================================================================================
// Functions for garbage collection 

void _free_char_sllist(char_sl** vec) {
    if (vec && *vec) {
        free_char_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_uchar_sllist(uchar_sl** vec) {
    if (vec && *vec) {
        free_uchar_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_short_sllist(short_sl** vec) {
    if (vec && *vec) {
        free_short_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ushort_sllist(ushort_sl** vec) {
    if (vec && *vec) {
        free_ushort_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_int_sllist(int_sl** vec) {
    if (vec && *vec) {
        free_int_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_uint_sllist(uint_sl** vec) {
    if (vec && *vec) {
        free_uint_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_long_sllist(long_sl** vec) {
    if (vec && *vec) {
        free_long_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ulong_sllist(ulong_sl** vec) {
    if (vec && *vec) {
        free_ulong_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_llong_sllist(llong_sl** vec) {
    if (vec && *vec) {
        free_llong_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ullong_sllist(ullong_sl** vec) {
    if (vec && *vec) {
        free_ullong_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_float_sllist(float_sl** vec) {
    if (vec && *vec) {
        free_float_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_double_sllist(double_sl** vec) {
    if (vec && *vec) {
        free_double_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ldouble_sllist(ldouble_sl** vec) {
    if (vec && *vec) {
        free_ldouble_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_bool_sllist(bool_sl** vec) {
    if (vec && *vec) {
        free_bool_sllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_sstring_sllist(string_sl** vec) {
    if (vec && *vec) {
        free_string_sllist(*vec);
    }
}
// ================================================================================
// ================================================================================
// SINGLY LINKED LIST LENGTH 

size_t char_sllist_length(char_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t uchar_sllist_length(uchar_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t short_sllist_length(short_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t ushort_sllist_length(ushort_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t int_sllist_length(int_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t uint_sllist_length(uint_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t long_sllist_length(long_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t ulong_sllist_length(ulong_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t llong_sllist_length(llong_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t ullong_sllist_length(ullong_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t float_sllist_length(float_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t double_sllist_length(double_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t ldouble_sllist_length(ldouble_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t bool_sllist_length(bool_sl* list) {
    return list->len;
}
// --------------------------------------------------------------------------------

size_t string_sllist_length(string_sl* list) {
    return list->len;
}
// ================================================================================
// ================================================================================
// GET PSEUDO INDEX FOR SLLIST

char get_char_sllist(char_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    char_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
} 
// --------------------------------------------------------------------------------

unsigned char get_uchar_sllist(uchar_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    uchar_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

short int get_short_sllist(short_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    short_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

unsigned short int get_ushort_sllist(ushort_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    ushort_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

int get_int_sllist(int_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    int_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

unsigned int get_uint_sllist(uint_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    uint_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

long int get_long_sllist(long_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    long_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

unsigned long int get_ulong_sllist(ulong_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    ulong_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

long long int get_llong_sllist(llong_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    llong_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

unsigned long long int get_ullong_sllist(ullong_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0;
    }

    ullong_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

float get_float_sllist(float_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0.; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0.;
    }

    float_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0.; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

double get_double_sllist(double_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0.; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0.;
    }

    double_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0.; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

long double get_ldouble_sllist(ldouble_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return 0.; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return 0.;
    }

    ldouble_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return 0; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

bool get_bool_sllist(bool_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return false; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }

    bool_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        return current->data;
    } else {
        errno = ERANGE;
        return false; // or some other error indicator
    }
}
// --------------------------------------------------------------------------------

str* get_string_sllist(string_sl* list, size_t index) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return NULL; // or some other error indicator
    }
    if (index > list->len) {
        errno = ERANGE;
        return NULL;
    }

    string_slnode* current = list->head;
    size_t i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current != NULL && i == index) {
        str* cpy = copy_string(current->data);
        return cpy;
    } else {
        errno = ERANGE;
        return NULL; // or some other error indicator
    }
}
// ================================================================================
// ================================================================================
// eof
