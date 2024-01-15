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
    str* ptr = init_string(data);
    newNode->data = ptr;

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
// POP SINGLY LINKED LIST DATA 

char pop_char_sllist(char_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return (char)255;
    }

    char_slnode* to_delete;
    char dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        char_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned char pop_uchar_sllist(uchar_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return (char)127;
    }

    uchar_slnode* to_delete;
    unsigned char dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        uchar_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

short int pop_short_sllist(short_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return SHRT_MAX;
    }

    short_slnode* to_delete;
    short int dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        short_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned short int pop_ushort_sllist(ushort_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return USHRT_MAX;
    }

    ushort_slnode* to_delete;
    unsigned short int dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        ushort_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

int pop_int_sllist(int_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return INT_MAX;
    }

    int_slnode* to_delete;
    int dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        int_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned int pop_uint_sllist(uint_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return UINT_MAX;
    }

    uint_slnode* to_delete;
    unsigned int dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        uint_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long int pop_long_sllist(long_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return LONG_MAX;
    }

    long_slnode* to_delete;
    long int dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        long_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned long int pop_ulong_sllist(ulong_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return ULONG_MAX;
    }

    ulong_slnode* to_delete;
    unsigned long int dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        ulong_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long long int pop_llong_sllist(llong_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return LLONG_MAX;
    }

    llong_slnode* to_delete;
    long long int dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        llong_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned long long int pop_ullong_sllist(ullong_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return ULLONG_MAX;
    }

    ullong_slnode* to_delete;
    unsigned long long int dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        ullong_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

float pop_float_sllist(float_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return FLT_MAX;
    }

    float_slnode* to_delete;
    float dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        float_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

double pop_double_sllist(double_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return DBL_MAX;
    }

    double_slnode* to_delete;
    double dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        double_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long double pop_ldouble_sllist(ldouble_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return LDBL_MAX;
    }

    ldouble_slnode* to_delete;
    long double dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        ldouble_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

bool pop_bool_sllist(bool_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return SHRT_MAX;
    }

    bool_slnode* to_delete;
    bool dat;

    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        bool_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }

    free(to_delete);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

str* pop_string_sllist(string_sl* list, size_t index) {
    if (list == NULL || list->head == NULL || index >= list->len) {
        errno = (list == NULL || list->head == NULL) ? EINVAL : ERANGE;
        return NULL;
    }

    string_slnode* to_delete;
    str* dat;
    if (index == 0) {
        // Removing the head node
        to_delete = list->head;
        list->head = list->head->next;
        dat = to_delete->data;
    } else {
        string_slnode* current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = current->next->next;
        dat = to_delete->data;
    }
    free(to_delete);

    list->len--;
    return dat;
}
// ================================================================================
// ================================================================================
// REVERSE SINGLY LINKED LIST 

void reverse_char_sllist(char_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    char_slnode* prev = NULL;
    char_slnode* current = list->head;
    char_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_uchar_sllist(uchar_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    uchar_slnode* prev = NULL;
    uchar_slnode* current = list->head;
    uchar_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_short_sllist(short_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    short_slnode* prev = NULL;
    short_slnode* current = list->head;
    short_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_ushort_sllist(ushort_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    ushort_slnode* prev = NULL;
    ushort_slnode* current = list->head;
    ushort_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_int_sllist(int_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    int_slnode* prev = NULL;
    int_slnode* current = list->head;
    int_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_uint_sllist(uint_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    uint_slnode* prev = NULL;
    uint_slnode* current = list->head;
    uint_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_long_sllist(long_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    long_slnode* prev = NULL;
    long_slnode* current = list->head;
    long_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_ulong_sllist(ulong_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    ulong_slnode* prev = NULL;
    ulong_slnode* current = list->head;
    ulong_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_llong_sllist(llong_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    llong_slnode* prev = NULL;
    llong_slnode* current = list->head;
    llong_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_ullong_sllist(ullong_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    ullong_slnode* prev = NULL;
    ullong_slnode* current = list->head;
    ullong_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// -------------------------------------------------------------------------------

void reverse_float_sllist(float_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    float_slnode* prev = NULL;
    float_slnode* current = list->head;
    float_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_double_sllist(double_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    double_slnode* prev = NULL;
    double_slnode* current = list->head;
    double_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_ldouble_sllist(ldouble_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    ldouble_slnode* prev = NULL;
    ldouble_slnode* current = list->head;
    ldouble_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_bool_sllist(bool_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    bool_slnode* prev = NULL;
    bool_slnode* current = list->head;
    bool_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// --------------------------------------------------------------------------------

void reverse_string_sllist(string_sl* list) {
    if (list == NULL || list->head == NULL) {
        errno = EINVAL;
        return;
    }
    string_slnode* prev = NULL;
    string_slnode* current = list->head;
    string_slnode* next = NULL;

    while( current != NULL ) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}
// ================================================================================
// ================================================================================
// BUBBLE SORT (PRIVATE FUNCTIONS) 

static void _bubble_sort_char(char_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    char_slnode *current;
    char_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                char temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_uchar(uchar_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    uchar_slnode *current;
    uchar_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                unsigned char temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_short(short_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    short_slnode *current;
    short_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                short int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ushort(ushort_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    ushort_slnode *current;
    ushort_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                unsigned short int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_int(int_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    int_slnode *current;
    int_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_uint(uint_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    uint_slnode *current;
    uint_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                unsigned int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_long(long_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    long_slnode *current;
    long_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                long int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ulong(ulong_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    ulong_slnode *current;
    ulong_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                unsigned long int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_llong(llong_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    llong_slnode *current;
    llong_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                long long int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ullong(ullong_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    ullong_slnode *current;
    ullong_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                unsigned long long int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_float(float_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    float_slnode *current;
    float_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                float temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_double(double_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    double_slnode *current;
    double_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                double temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ldouble(ldouble_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    ldouble_slnode *current;
    ldouble_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                long double temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_bool(bool_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    bool_slnode *current;
    bool_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            if ((direction == FORWARD && current->data > current->next->data) ||
                (direction == REVERSE && current->data < current->next->data)) {
                // Swap data
                bool temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_string(string_sl* list, size_t len, iter_dir direction) {
    if (!list || !list->head || len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool swapped;
    string_slnode *current;
    string_slnode *last = NULL;

    do {
        swapped = false;
        current = list->head;

        while (current->next != last) {
            int cmp = compare_strings_str(current->data, current->next->data);
            if ((direction == FORWARD && cmp > 0) ||
                (direction == REVERSE && cmp < 0)) {
                // Swap data
                struct str *temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}
// ================================================================================
// ================================================================================
// SELECTION SORT (PRIVATE FUNCTIONS)

static void _selection_sort_char(char_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    char_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_char(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_uchar(uchar_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    uchar_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_uchar(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_short(short_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    short_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_short(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_ushort(ushort_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    ushort_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_ushort(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_int(int_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    int_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_int(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_uint(uint_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    uint_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_uint(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_long(long_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    long_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_long(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_ulong(ulong_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    ulong_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_ulong(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_llong(llong_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    llong_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_llong(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_ullong(ullong_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    ullong_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_ullong(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_float(float_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    float_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_float(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_double(double_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    double_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_double(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_ldouble(ldouble_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    ldouble_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_ldouble(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_bool(bool_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    bool_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            if ((direction == FORWARD && min->data > next->data) ||
                (direction == REVERSE && min->data < next->data)) {
                min = next;
            }
        }
        if (min != current) {
            swap_bool(&current->data, &min->data);
        }
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_string(string_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }

    string_slnode *current, *next, *min;

    for (current = list->head; current && current->next; current = current->next) {
        min = current;
        for (next = current->next; next; next = next->next) {
            int cmp = compare_strings_str(min->data, next->data);
            if ((direction == FORWARD && cmp > 0) ||
                (direction == REVERSE && cmp < 0)) {
                min = next;
            }
        }
        if (min != current) {
            struct str *temp = current->data;
            current->data = min->data;
            min->data = temp;
        }
    }
}
// ================================================================================
// ================================================================================
// INSERTION SORT (PRIVATE FUNCTION)

static void _insert_sort_char(char_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    char_slnode *sorted_last = list->head;
    char_slnode *current = list->head->next;

    while (current != NULL) {
        char_slnode *prev = NULL;
        char_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_uchar(uchar_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    uchar_slnode *sorted_last = list->head;
    uchar_slnode *current = list->head->next;

    while (current != NULL) {
        uchar_slnode *prev = NULL;
        uchar_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_short(short_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    short_slnode *sorted_last = list->head;
    short_slnode *current = list->head->next;

    while (current != NULL) {
        short_slnode *prev = NULL;
        short_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_ushort(ushort_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    ushort_slnode *sorted_last = list->head;
    ushort_slnode *current = list->head->next;

    while (current != NULL) {
        ushort_slnode *prev = NULL;
        ushort_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_int(int_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    int_slnode *sorted_last = list->head;
    int_slnode *current = list->head->next;

    while (current != NULL) {
        int_slnode *prev = NULL;
        int_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_uint(uint_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    uint_slnode *sorted_last = list->head;
    uint_slnode *current = list->head->next;

    while (current != NULL) {
        uint_slnode *prev = NULL;
        uint_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_long(long_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    long_slnode *sorted_last = list->head;
    long_slnode *current = list->head->next;

    while (current != NULL) {
        long_slnode *prev = NULL;
        long_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_ulong(ulong_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    ulong_slnode *sorted_last = list->head;
    ulong_slnode *current = list->head->next;

    while (current != NULL) {
        ulong_slnode *prev = NULL;
        ulong_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_llong(llong_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    llong_slnode *sorted_last = list->head;
    llong_slnode *current = list->head->next;

    while (current != NULL) {
        llong_slnode *prev = NULL;
        llong_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_ullong(ullong_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    ullong_slnode *sorted_last = list->head;
    ullong_slnode *current = list->head->next;

    while (current != NULL) {
        ullong_slnode *prev = NULL;
        ullong_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_float(float_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    float_slnode *sorted_last = list->head;
    float_slnode *current = list->head->next;

    while (current != NULL) {
        float_slnode *prev = NULL;
        float_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_double(double_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    double_slnode *sorted_last = list->head;
    double_slnode *current = list->head->next;

    while (current != NULL) {
        double_slnode *prev = NULL;
        double_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_ldouble(ldouble_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    ldouble_slnode *sorted_last = list->head;
    ldouble_slnode *current = list->head->next;

    while (current != NULL) {
        ldouble_slnode *prev = NULL;
        ldouble_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_bool(bool_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    bool_slnode *sorted_last = list->head;
    bool_slnode *current = list->head->next;

    while (current != NULL) {
        bool_slnode *prev = NULL;
        bool_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current && 
               ((direction == FORWARD && search->data < current->data) ||
                (direction == REVERSE && search->data > current->data))) {
            prev = search;
            search = search->next;
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// --------------------------------------------------------------------------------

static void _insert_sort_string(string_sl* list, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        return; // No need to sort if the list is empty or has only one element
    }

    string_slnode *sorted_last = list->head;
    string_slnode *current = list->head->next;

    while (current != NULL) {
        string_slnode *prev = NULL;
        string_slnode *search = list->head;

        // Find the correct position to insert
        while (search != current) {
            int cmp = compare_strings_str(search->data, current->data);
            if ((direction == FORWARD && cmp < 0) || (direction == REVERSE && cmp > 0)) {
                prev = search;
                search = search->next;
            } else {
                break;
            }
        }

        if (search != current) {
            // Insert the node at the found position
            if (prev != NULL) {
                prev->next = current;
            } else {
                list->head = current;
            }
            sorted_last->next = current->next;
            current->next = search;

            // If we inserted before the last sorted element, adjust sorted_last
            if (sorted_last == current) {
                sorted_last = sorted_last->next;
            }
        } else {
            // The node is already in the correct position
            sorted_last = current;
        }

        // Move to the next node
        current = sorted_last->next;
    }
}
// ================================================================================
// ================================================================================
// MERGE SORT (PRIVATE FUNCTIONS)

static void _split_char_list(char_slnode* source, char_slnode** frontRef, char_slnode** backRef) {
    char_slnode* fast;
    char_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static char_slnode* _sorted_char_merge(char_slnode* a, char_slnode* b, iter_dir direction) {
    char_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_char_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_char_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_char(char_slnode** headRef, iter_dir direction) {
    char_slnode* head = *headRef;
    char_slnode* a;
    char_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_char_list(head, &a, &b);
    _merge_sort_char(&a, direction);
    _merge_sort_char(&b, direction);
    *headRef = _sorted_char_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_uchar_list(uchar_slnode* source, uchar_slnode** frontRef, uchar_slnode** backRef) {
    uchar_slnode* fast;
    uchar_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static uchar_slnode* _sorted_uchar_merge(uchar_slnode* a, uchar_slnode* b, iter_dir direction) {
    uchar_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_uchar_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_uchar_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_uchar(uchar_slnode** headRef, iter_dir direction) {
    uchar_slnode* head = *headRef;
    uchar_slnode* a;
    uchar_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_uchar_list(head, &a, &b);
    _merge_sort_uchar(&a, direction);
    _merge_sort_uchar(&b, direction);
    *headRef = _sorted_uchar_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_short_list(short_slnode* source, short_slnode** frontRef, short_slnode** backRef) {
    short_slnode* fast;
    short_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static short_slnode* _sorted_short_merge(short_slnode* a, short_slnode* b, iter_dir direction) {
    short_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_short_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_short_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_short(short_slnode** headRef, iter_dir direction) {
    short_slnode* head = *headRef;
    short_slnode* a;
    short_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_short_list(head, &a, &b);
    _merge_sort_short(&a, direction);
    _merge_sort_short(&b, direction);
    *headRef = _sorted_short_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_ushort_list(ushort_slnode* source, ushort_slnode** frontRef, ushort_slnode** backRef) {
    ushort_slnode* fast;
    ushort_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static ushort_slnode* _sorted_ushort_merge(ushort_slnode* a, ushort_slnode* b, iter_dir direction) {
    ushort_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_ushort_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_ushort_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_ushort(ushort_slnode** headRef, iter_dir direction) {
    ushort_slnode* head = *headRef;
    ushort_slnode* a;
    ushort_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_ushort_list(head, &a, &b);
    _merge_sort_ushort(&a, direction);
    _merge_sort_ushort(&b, direction);
    *headRef = _sorted_ushort_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_int_list(int_slnode* source, int_slnode** frontRef, int_slnode** backRef) {
    int_slnode* fast;
    int_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static int_slnode* _sorted_int_merge(int_slnode* a, int_slnode* b, iter_dir direction) {
    int_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_int_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_int_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_int(int_slnode** headRef, iter_dir direction) {
    int_slnode* head = *headRef;
    int_slnode* a;
    int_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_int_list(head, &a, &b);
    _merge_sort_int(&a, direction);
    _merge_sort_int(&b, direction);
    *headRef = _sorted_int_merge(a, b, direction);
}
// -------------------------------------------------------------------------------- 

static void _split_uint_list(uint_slnode* source, uint_slnode** frontRef, uint_slnode** backRef) {
    uint_slnode* fast;
    uint_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static uint_slnode* _sorted_uint_merge(uint_slnode* a, uint_slnode* b, iter_dir direction) {
    uint_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_uint_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_uint_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_uint(uint_slnode** headRef, iter_dir direction) {
    uint_slnode* head = *headRef;
    uint_slnode* a;
    uint_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_uint_list(head, &a, &b);
    _merge_sort_uint(&a, direction);
    _merge_sort_uint(&b, direction);
    *headRef = _sorted_uint_merge(a, b, direction);
}

// --------------------------------------------------------------------------------

static void _split_long_list(long_slnode* source, long_slnode** frontRef, long_slnode** backRef) {
    long_slnode* fast;
    long_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static long_slnode* _sorted_long_merge(long_slnode* a, long_slnode* b, iter_dir direction) {
    long_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_long_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_long_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_long(long_slnode** headRef, iter_dir direction) {
    long_slnode* head = *headRef;
    long_slnode* a;
    long_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_long_list(head, &a, &b);
    _merge_sort_long(&a, direction);
    _merge_sort_long(&b, direction);
    *headRef = _sorted_long_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_ulong_list(ulong_slnode* source, ulong_slnode** frontRef, ulong_slnode** backRef) {
    ulong_slnode* fast;
    ulong_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static ulong_slnode* _sorted_ulong_merge(ulong_slnode* a, ulong_slnode* b, iter_dir direction) {
    ulong_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_ulong_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_ulong_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_ulong(ulong_slnode** headRef, iter_dir direction) {
    ulong_slnode* head = *headRef;
    ulong_slnode* a;
    ulong_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_ulong_list(head, &a, &b);
    _merge_sort_ulong(&a, direction);
    _merge_sort_ulong(&b, direction);
    *headRef = _sorted_ulong_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_llong_list(llong_slnode* source, llong_slnode** frontRef, llong_slnode** backRef) {
    llong_slnode* fast;
    llong_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static llong_slnode* _sorted_llong_merge(llong_slnode* a, llong_slnode* b, iter_dir direction) {
    llong_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_llong_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_llong_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_llong(llong_slnode** headRef, iter_dir direction) {
    llong_slnode* head = *headRef;
    llong_slnode* a;
    llong_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_llong_list(head, &a, &b);
    _merge_sort_llong(&a, direction);
    _merge_sort_llong(&b, direction);
    *headRef = _sorted_llong_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_ullong_list(ullong_slnode* source, ullong_slnode** frontRef, ullong_slnode** backRef) {
    ullong_slnode* fast;
    ullong_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static ullong_slnode* _sorted_ullong_merge(ullong_slnode* a, ullong_slnode* b, iter_dir direction) {
    ullong_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_ullong_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_ullong_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_ullong(ullong_slnode** headRef, iter_dir direction) {
    ullong_slnode* head = *headRef;
    ullong_slnode* a;
    ullong_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_ullong_list(head, &a, &b);
    _merge_sort_ullong(&a, direction);
    _merge_sort_ullong(&b, direction);
    *headRef = _sorted_ullong_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_float_list(float_slnode* source, float_slnode** frontRef, float_slnode** backRef) {
    float_slnode* fast;
    float_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static float_slnode* _sorted_float_merge(float_slnode* a, float_slnode* b, iter_dir direction) {
    float_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_float_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_float_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_float(float_slnode** headRef, iter_dir direction) {
    float_slnode* head = *headRef;
    float_slnode* a;
    float_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_float_list(head, &a, &b);
    _merge_sort_float(&a, direction);
    _merge_sort_float(&b, direction);
    *headRef = _sorted_float_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_double_list(double_slnode* source, double_slnode** frontRef, double_slnode** backRef) {
    double_slnode* fast;
    double_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static double_slnode* _sorted_double_merge(double_slnode* a, double_slnode* b, iter_dir direction) {
    double_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_double_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_double_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_double(double_slnode** headRef, iter_dir direction) {
    double_slnode* head = *headRef;
    double_slnode* a;
    double_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_double_list(head, &a, &b);
    _merge_sort_double(&a, direction);
    _merge_sort_double(&b, direction);
    *headRef = _sorted_double_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_ldouble_list(ldouble_slnode* source, ldouble_slnode** frontRef, ldouble_slnode** backRef) {
    ldouble_slnode* fast;
    ldouble_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static ldouble_slnode* _sorted_ldouble_merge(ldouble_slnode* a, ldouble_slnode* b, iter_dir direction) {
    ldouble_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_ldouble_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_ldouble_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_ldouble(ldouble_slnode** headRef, iter_dir direction) {
    ldouble_slnode* head = *headRef;
    ldouble_slnode* a;
    ldouble_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_ldouble_list(head, &a, &b);
    _merge_sort_ldouble(&a, direction);
    _merge_sort_ldouble(&b, direction);
    *headRef = _sorted_ldouble_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_bool_list(bool_slnode* source, bool_slnode** frontRef, bool_slnode** backRef) {
    bool_slnode* fast;
    bool_slnode* slow;
    slow = source;
    fast = source->next;

    // 'fast' advances two nodes, and 'slow' one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint in the list, so split it in two at that point
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static bool_slnode* _sorted_bool_merge(bool_slnode* a, bool_slnode* b, iter_dir direction) {
    bool_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    if ((direction == FORWARD && a->data <= b->data) || (direction == REVERSE && a->data > b->data)) {
        result = a;
        result->next = _sorted_bool_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_bool_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_bool(bool_slnode** headRef, iter_dir direction) {
    bool_slnode* head = *headRef;
    bool_slnode* a;
    bool_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_bool_list(head, &a, &b);
    _merge_sort_bool(&a, direction);
    _merge_sort_bool(&b, direction);
    *headRef = _sorted_bool_merge(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_string_list(string_slnode* source, string_slnode** frontRef, string_slnode** backRef) {
    string_slnode* fast;
    string_slnode* slow;
    slow = source;
    fast = source->next;

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}
// --------------------------------------------------------------------------------

static string_slnode* _sorted_string_merge(string_slnode* a, string_slnode* b, iter_dir direction) {
    string_slnode* result = NULL;

    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;
    int cmp = compare_strings_str(a->data, b->data);
    if ((direction == FORWARD && cmp <= 0) || (direction == REVERSE && cmp > 0)) {
        result = a;
        result->next = _sorted_string_merge(a->next, b, direction);
    } else {
        result = b;
        result->next = _sorted_string_merge(a, b->next, direction);
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_string(string_slnode** headRef, iter_dir direction) {
    string_slnode* head = *headRef;
    string_slnode* a;
    string_slnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_string_list(head, &a, &b);
    _merge_sort_string(&a, direction);
    _merge_sort_string(&b, direction);
    *headRef = _sorted_string_merge(a, b, direction);
}
// ================================================================================
// ================================================================================

void sort_char_sllist(char_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_char(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_char(list, direction);
            break;
        case INSERT:
            _insert_sort_char(list, direction);
            break;
        case MERGE:
            _merge_sort_char(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_uchar_sllist(uchar_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_uchar(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_uchar(list, direction);
            break;
        case INSERT:
            _insert_sort_uchar(list, direction);
            break;
        case MERGE:
            _merge_sort_uchar(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_short_sllist(short_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_short(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_short(list, direction);
            break;
        case INSERT:
            _insert_sort_short(list, direction);
            break;
        case MERGE:
            _merge_sort_short(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ushort_sllist(ushort_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_ushort(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_ushort(list, direction);
            break;
        case INSERT:
            _insert_sort_ushort(list, direction);
            break;
        case MERGE:
            _merge_sort_ushort(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_int_sllist(int_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_int(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_int(list, direction);
            break;
        case INSERT:
            _insert_sort_int(list, direction);
            break;
        case MERGE:
            _merge_sort_int(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_uint_sllist(uint_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_uint(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_uint(list, direction);
            break;
        case INSERT:
            _insert_sort_uint(list, direction);
            break;
        case MERGE:
            _merge_sort_uint(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_long_sllist(long_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_long(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_long(list, direction);
            break;
        case INSERT:
            _insert_sort_long(list, direction);
            break;
        case MERGE:
            _merge_sort_long(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ulong_sllist(ulong_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_ulong(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_ulong(list, direction);
            break;
        case INSERT:
            _insert_sort_ulong(list, direction);
            break;
        case MERGE:
            _merge_sort_ulong(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_llong_sllist(llong_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_llong(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_llong(list, direction);
            break;
        case INSERT:
            _insert_sort_llong(list, direction);
            break;
        case MERGE:
            _merge_sort_llong(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ullong_sllist(ullong_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_ullong(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_ullong(list, direction);
            break;
        case INSERT:
            _insert_sort_ullong(list, direction);
            break;
        case MERGE:
            _merge_sort_ullong(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_float_sllist(float_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_float(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_float(list, direction);
            break;
        case INSERT:
            _insert_sort_float(list, direction);
            break;
        case MERGE:
            _merge_sort_float(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_double_sllist(double_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_double(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_double(list, direction);
            break;
        case INSERT:
            _insert_sort_double(list, direction);
            break;
        case MERGE:
            _merge_sort_double(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ldouble_sllist(ldouble_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_ldouble(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_ldouble(list, direction);
            break;
        case INSERT:
            _insert_sort_ldouble(list, direction);
            break;
        case MERGE:
            _merge_sort_ldouble(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_bool_sllist(bool_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_bool(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_bool(list, direction);
            break;
        case INSERT:
            _insert_sort_bool(list, direction);
            break;
        case MERGE:
            _merge_sort_bool(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_string_sllist(string_sl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_string(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_string(list, direction);
            break;
        case INSERT:
            _insert_sort_string(list, direction);
            break;
        case MERGE:
            _merge_sort_string(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// ================================================================================
// ================================================================================
// eof
