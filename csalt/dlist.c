// ================================================================================
// ================================================================================
// - File:    dlist.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 09, 2024
// - Version: 1.0
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/dlist.h"
// ================================================================================
// ================================================================================

char_dl* init_char_dllist() {
    char_dl* ptr = malloc(sizeof(char_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

uchar_dl* init_uchar_dllist() {
    uchar_dl* ptr = malloc(sizeof(uchar_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

short_dl* init_short_dllist() {
    short_dl* ptr = malloc(sizeof(short_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

ushort_dl* init_ushort_dllist() {
    ushort_dl* ptr = malloc(sizeof(ushort_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

int_dl* init_int_dllist() {
    int_dl* ptr = malloc(sizeof(int_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

uint_dl* init_uint_dllist() {
    uint_dl* ptr = malloc(sizeof(uint_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

long_dl* init_long_dllist() {
    long_dl* ptr = malloc(sizeof(long_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

ulong_dl* init_ulong_dllist() {
    ulong_dl* ptr = malloc(sizeof(ulong_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

llong_dl* init_llong_dllist() {
    llong_dl* ptr = malloc(sizeof(llong_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

ullong_dl* init_ullong_dllist() {
    ullong_dl* ptr = malloc(sizeof(ullong_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

float_dl* init_float_dllist() {
    float_dl* ptr = malloc(sizeof(float_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

double_dl* init_double_dllist() {
    double_dl* ptr = malloc(sizeof(double_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

ldouble_dl* init_ldouble_dllist() {
    ldouble_dl* ptr = malloc(sizeof(ldouble_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

bool_dl* init_bool_dllist() {
    bool_dl* ptr = malloc(sizeof(bool_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

string_dl* init_string_dllist() {
    string_dl* ptr = malloc(sizeof(string_dl));
    if ( !ptr ) {
        errno = ENOMEM;
        return NULL;
    }
    ptr->len = 0;
    ptr->head = NULL;
    ptr->tail = NULL;
    return ptr;
}
// --------------------------------------------------------------------------------

InitDListFunc init_dllist(dtype dat_type) {
    switch(dat_type) {
        case dChar: return (InitDListFunc)init_char_dllist;
        case dUChar: return (InitDListFunc)init_uchar_dllist;
        case dShort: return (InitDListFunc)init_short_dllist;
        case dUShort: return (InitDListFunc)init_ushort_dllist;
        case dInt: return (InitDListFunc)init_int_dllist;
        case dUInt: return (InitDListFunc)init_uint_dllist;
        case dLong: return (InitDListFunc)init_long_dllist;
        case dULong: return (InitDListFunc)init_ulong_dllist;
        case dLongLong: return (InitDListFunc)init_llong_dllist;
        case dULongLong: return (InitDListFunc)init_ullong_dllist;
        case dFloat: return (InitDListFunc)init_float_dllist;
        case dDouble: return (InitDListFunc)init_double_dllist;
        case dLDouble: return (InitDListFunc)init_ldouble_dllist;
        case dBool: return (InitDListFunc)init_bool_dllist;
        case dString: return (InitDListFunc)init_string_dllist;
        default: return NULL;
    } 
}
// ================================================================================
// ================================================================================

bool push_char_front_dlist(char_dl* list, char dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    char_dlnode *newNode = malloc(sizeof(char_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_uchar_front_dlist(uchar_dl* list, unsigned char dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    uchar_dlnode *newNode = malloc(sizeof(uchar_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_short_front_dlist(short_dl* list, short int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    short_dlnode *newNode = malloc(sizeof(short_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ushort_front_dlist(ushort_dl* list, unsigned short int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    ushort_dlnode *newNode = malloc(sizeof(ushort_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_int_front_dlist(int_dl* list, int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    int_dlnode *newNode = malloc(sizeof(int_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_uint_front_dlist(uint_dl* list, unsigned int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    uint_dlnode *newNode = malloc(sizeof(uint_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_long_front_dlist(long_dl* list, long int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    long_dlnode *newNode = malloc(sizeof(long_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ulong_front_dlist(ulong_dl* list, unsigned long int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    ulong_dlnode *newNode = malloc(sizeof(ulong_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_llong_front_dlist(llong_dl* list, long long int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    llong_dlnode *newNode = malloc(sizeof(llong_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ullong_front_dlist(ullong_dl* list, unsigned long long int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    ullong_dlnode *newNode = malloc(sizeof(ullong_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_float_front_dlist(float_dl* list, float dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    float_dlnode *newNode = malloc(sizeof(float_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_double_front_dlist(double_dl* list, double dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    double_dlnode *newNode = malloc(sizeof(double_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ldouble_front_dlist(ldouble_dl* list, long double dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    ldouble_dlnode *newNode = malloc(sizeof(ldouble_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_bool_front_dlist(bool_dl* list, bool dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    bool_dlnode *newNode = malloc(sizeof(bool_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_string_front_dlist(string_dl* list, char *dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    string_dlnode *newNode = malloc(sizeof(string_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    str *val = init_string_nol(dat);
    newNode->data = val;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_str_front_dlist(string_dl* list, str *dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    string_dlnode *newNode = malloc(sizeof(string_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    newNode->data = dat;
    if (list->len == 0) {
        newNode->prev = NULL;
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = NULL;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->len++;
    return true;
}
// ================================================================================
// ================================================================================

void free_char_dlist(char_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    char_dlnode *current = list->head;
    char_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_uchar_dlist(uchar_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    uchar_dlnode *current = list->head;
    uchar_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_short_dlist(short_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    short_dlnode *current = list->head;
    short_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_ushort_dlist(ushort_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    ushort_dlnode *current = list->head;
    ushort_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_int_dlist(int_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    int_dlnode *current = list->head;
    int_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_long_dlist(long_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    long_dlnode *current = list->head;
    long_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_ulong_dlist(ulong_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    ulong_dlnode *current = list->head;
    ulong_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_llong_dlist(llong_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    llong_dlnode *current = list->head;
    llong_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_ullong_dlist(ullong_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    ullong_dlnode *current = list->head;
    ullong_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_float_dlist(float_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    float_dlnode *current = list->head;
    float_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_double_dlist(double_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    double_dlnode *current = list->head;
    double_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_ldouble_dlist(ldouble_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    ldouble_dlnode *current = list->head;
    ldouble_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_bool_dlist(bool_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    bool_dlnode *current = list->head;
    bool_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_string_dlist(string_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    string_dlnode *current = list->head;
    string_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free_string(current->data);
        free(current);
        current = next;
    }
    free(list);
}
// ================================================================================
// ================================================================================
// eof
