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

bool push_char_front_dllist(char_dl* list, char dat) {
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

bool push_uchar_front_dllist(uchar_dl* list, unsigned char dat) {
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

bool push_short_front_dllist(short_dl* list, short int dat) {
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

bool push_ushort_front_dllist(ushort_dl* list, unsigned short int dat) {
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

bool push_int_front_dllist(int_dl* list, int dat) {
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

bool push_uint_front_dllist(uint_dl* list, unsigned int dat) {
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

bool push_long_front_dllist(long_dl* list, long int dat) {
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

bool push_ulong_front_dllist(ulong_dl* list, unsigned long int dat) {
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

bool push_llong_front_dllist(llong_dl* list, long long int dat) {
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

bool push_ullong_front_dllist(ullong_dl* list, unsigned long long int dat) {
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

bool push_float_front_dllist(float_dl* list, float dat) {
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

bool push_double_front_dllist(double_dl* list, double dat) {
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

bool push_ldouble_front_dllist(ldouble_dl* list, long double dat) {
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

bool push_bool_front_dllist(bool_dl* list, bool dat) {
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

bool push_string_front_dllist(string_dl* list, char *dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    if (!dat) {
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

bool push_str_front_dllist(string_dl* list, str *dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    if (!dat || !dat->data) {
        errno = EINVAL;
        return false;
    }
    string_dlnode *newNode = malloc(sizeof(string_dlnode));
    if (!newNode) {
        errno = ENOMEM;
        return false;
    }
    str* val = init_string_nol(dat->data);
    if (!val->data) {
        errno = ENOMEM;
        free(newNode);
        return false;
    }
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
// ================================================================================
// ================================================================================

void free_char_dllist(char_dl* list) {
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

void free_uchar_dllist(uchar_dl* list) {
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

void free_short_dllist(short_dl* list) {
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

void free_ushort_dllist(ushort_dl* list) {
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

void free_int_dllist(int_dl* list) {
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

void free_uint_dllist(uint_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return;
    }
    uint_dlnode *current = list->head;
    uint_dlnode *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
// --------------------------------------------------------------------------------

void free_long_dllist(long_dl* list) {
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

void free_ulong_dllist(ulong_dl* list) {
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

void free_llong_dllist(llong_dl* list) {
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

void free_ullong_dllist(ullong_dl* list) {
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

void free_float_dllist(float_dl* list) {
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

void free_double_dllist(double_dl* list) {
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

void free_ldouble_dllist(ldouble_dl* list) {
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

void free_bool_dllist(bool_dl* list) {
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

void free_string_dllist(string_dl* list) {
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

size_t size_char_dlist(char_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_uchar_dlist(uchar_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_short_dlist(short_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_ushort_dlist(ushort_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_int_dlist(int_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_uint_dlist(uint_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_long_dlist(long_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_ulong_dlist(ulong_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_llong_dlist(llong_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_ullong_dlist(ullong_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_float_dlist(float_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_double_dlist(double_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_ldouble_dlist(ldouble_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_bool_dlist(bool_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// --------------------------------------------------------------------------------

size_t size_string_dlist(string_dl* list) {
    if (!list) {
        errno = ENOMEM;
        return SIZE_MAX;
    }
    return list->len;
}
// ================================================================================
// ================================================================================
// Functions for garbage collection 

void _free_char_dllist(char_dl** vec) {
    if (vec && *vec) {
        free_char_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_uchar_dllist(uchar_dl** vec) {
    if (vec && *vec) {
        free_uchar_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_short_dllist(short_dl** vec) {
    if (vec && *vec) {
        free_short_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ushort_dllist(ushort_dl** vec) {
    if (vec && *vec) {
        free_ushort_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_int_dllist(int_dl** vec) {
    if (vec && *vec) {
        free_int_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_uint_dllist(uint_dl** vec) {
    if (vec && *vec) {
        free_uint_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_long_dllist(long_dl** vec) {
    if (vec && *vec) {
        free_long_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ulong_dllist(ulong_dl** vec) {
    if (vec && *vec) {
        free_ulong_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_llong_dllist(llong_dl** vec) {
    if (vec && *vec) {
        free_llong_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ullong_dllist(ullong_dl** vec) {
    if (vec && *vec) {
        free_ullong_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_float_dllist(float_dl** vec) {
    if (vec && *vec) {
        free_float_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_double_dllist(double_dl** vec) {
    if (vec && *vec) {
        free_double_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_ldouble_dllist(ldouble_dl** vec) {
    if (vec && *vec) {
        free_ldouble_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_bool_dllist(bool_dl** vec) {
    if (vec && *vec) {
        free_bool_dllist(*vec);
    }
}
// --------------------------------------------------------------------------------

void _free_sstring_dllist(string_dl** vec) {
    if (vec && *vec) {
        free_string_dllist(*vec);
    }
}
// ================================================================================
// ================================================================================

bool push_char_back_dllist(char_dl* list, char dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    char_dlnode* ptr = malloc(sizeof(char_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_uchar_back_dllist(uchar_dl* list, unsigned char dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    uchar_dlnode* ptr = malloc(sizeof(uchar_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_short_back_dllist(short_dl* list, short dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    short_dlnode* ptr = malloc(sizeof(short_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ushort_back_dllist(ushort_dl* list, unsigned short int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    ushort_dlnode* ptr = malloc(sizeof(ushort_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_int_back_dllist(int_dl* list, int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    int_dlnode* ptr = malloc(sizeof(int_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_uint_back_dllist(uint_dl* list, unsigned int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    uint_dlnode* ptr = malloc(sizeof(uint_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_long_back_dllist(long_dl* list, long int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    long_dlnode* ptr = malloc(sizeof(long_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ulong_back_dllist(ulong_dl* list, unsigned long int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    ulong_dlnode* ptr = malloc(sizeof(ulong_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_llong_back_dllist(llong_dl* list, long long int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    llong_dlnode* ptr = malloc(sizeof(llong_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ullong_back_dllist(ullong_dl* list, unsigned long long int dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    ullong_dlnode* ptr = malloc(sizeof(ullong_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_float_back_dllist(float_dl* list, float dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    float_dlnode* ptr = malloc(sizeof(float_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_double_back_dllist(double_dl* list, double dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    double_dlnode* ptr = malloc(sizeof(double_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_ldouble_back_dllist(ldouble_dl* list, long double dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    ldouble_dlnode* ptr = malloc(sizeof(ldouble_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_bool_back_dllist(bool_dl* list, bool dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    bool_dlnode* ptr = malloc(sizeof(bool_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    ptr->data = dat;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_string_back_dllist(string_dl* list, char *dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    string_dlnode* ptr = malloc(sizeof(string_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    str *string = init_string_nol(dat);
    ptr->data = string;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool push_str_back_dllist(string_dl* list, str *dat) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    string_dlnode* ptr = malloc(sizeof(string_dlnode));
    if (!ptr) {
        errno = ENOMEM;
        return false;
    }
    str *string = init_string_nol(dat->data);
    ptr->data = string;
    if (list->len == 0) {
        ptr->next = NULL;
        ptr->prev = NULL;
        list->head = ptr;
        list->tail = ptr;
    } else {
        ptr->next = NULL;
        ptr->prev = list->tail;
        list->tail->next = ptr;
        list->tail = ptr;
    }
    list->len++;
    return true;
}
// ================================================================================
// ================================================================================

bool insert_char_dllist(char_dl* list, char dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_char_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_char_back_dllist(list, dat);
        return true;
    }
    char_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    char_dlnode *node = malloc(sizeof(char_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_uchar_dllist(uchar_dl* list, unsigned char dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_uchar_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_uchar_back_dllist(list, dat);
        return true;
    }
    uchar_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    uchar_dlnode *node = malloc(sizeof(uchar_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_short_dllist(short_dl* list, short dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_short_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_short_back_dllist(list, dat);
        return true;
    }
    short_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    short_dlnode *node = malloc(sizeof(short_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_ushort_dllist(ushort_dl* list, unsigned short dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_ushort_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_ushort_back_dllist(list, dat);
        return true;
    }
    ushort_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    ushort_dlnode *node = malloc(sizeof(ushort_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_int_dllist(int_dl* list, int dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_int_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_int_back_dllist(list, dat);
        return true;
    }
    int_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    int_dlnode *node = malloc(sizeof(int_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_uint_dllist(uint_dl* list, unsigned int dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_uint_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_uint_back_dllist(list, dat);
        return true;
    }
    uint_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    uint_dlnode *node = malloc(sizeof(uint_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_long_dllist(long_dl* list, long int dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_long_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_long_back_dllist(list, dat);
        return true;
    }
    long_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    long_dlnode *node = malloc(sizeof(long_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_ulong_dllist(ulong_dl* list, unsigned long int dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_ulong_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_ulong_back_dllist(list, dat);
        return true;
    }
    ulong_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    ulong_dlnode *node = malloc(sizeof(ulong_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_llong_dllist(llong_dl* list, long long int dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_llong_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_llong_back_dllist(list, dat);
        return true;
    }
    llong_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    llong_dlnode *node = malloc(sizeof(llong_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_ullong_dllist(ullong_dl* list, unsigned long long int dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_ullong_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_ullong_back_dllist(list, dat);
        return true;
    }
    ullong_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    ullong_dlnode *node = malloc(sizeof(ullong_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_float_dllist(float_dl* list, float dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_float_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_float_back_dllist(list, dat);
        return true;
    }
    float_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    float_dlnode *node = malloc(sizeof(float_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_double_dllist(double_dl* list, double dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_double_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_double_back_dllist(list, dat);
        return true;
    }
    double_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    double_dlnode *node = malloc(sizeof(double_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_ldouble_dllist(ldouble_dl* list, long double dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_ldouble_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_ldouble_back_dllist(list, dat);
        return true;
    }
    ldouble_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    ldouble_dlnode *node = malloc(sizeof(ldouble_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_bool_dllist(bool_dl* list, bool dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_bool_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_bool_back_dllist(list, dat);
        return true;
    }
    bool_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    bool_dlnode *node = malloc(sizeof(bool_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    node->data = dat;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_string_dllist(string_dl* list, char *dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_string_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_string_back_dllist(list, dat);
        return true;
    }
    string_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    string_dlnode *node = malloc(sizeof(string_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    str *string = init_string_nol(dat);
    node->data = string;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_str_dllist(string_dl* list, str *dat, size_t index) {
    if (index > list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) {
        push_str_front_dllist(list, dat);
        return true;
    }
    if (index == list->len) {
        push_str_back_dllist(list, dat);
        return true;
    }
    string_dlnode *temp = NULL;
    if (index <= list->len / 2) { 
        temp = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }

    string_dlnode *node = malloc(sizeof(char_dlnode));
    if (!node) {
        errno = ENOMEM;
        return false;
    }
    str *string = init_string_nol(dat->data);
    node->data = string;
    if (temp->next) {
        node->next = temp->next;
        node->next->prev = node;
    }
    node->prev = temp;
    node->prev->next = node;
    list->len++;
    return true;
}
// ================================================================================
// ================================================================================

char pop_char_front_dllist(char_dl* list) {
    if (!list) {
        errno = EINVAL;
        return SCHAR_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return SCHAR_MAX;
    }
    char dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        char_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned char pop_uchar_front_dllist(uchar_dl* list) {
    if (!list) {
        errno = EINVAL;
        return CHAR_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return CHAR_MAX;
    }
    unsigned char dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        uchar_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

short pop_short_front_dllist(short_dl* list) {
    if (!list) {
        errno = EINVAL;
        return SHRT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return SHRT_MAX;
    }
    short dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        short_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned short int pop_ushort_front_dllist(ushort_dl* list) {
    if (!list) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return USHRT_MAX;
    }
    unsigned short int dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        ushort_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

int pop_int_front_dllist(int_dl* list) {
    if (!list) {
        errno = EINVAL;
        return INT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return INT_MAX;
    }
    int dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        int_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned int pop_uint_front_dllist(uint_dl* list) {
    if (!list) {
        errno = EINVAL;
        return UINT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return UINT_MAX;
    }
    unsigned int dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        uint_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long int pop_long_front_dllist(long_dl* list) {
    if (!list) {
        errno = EINVAL;
        return LONG_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return LONG_MAX;
    }
    long int dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        long_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned long pop_ulong_front_dllist(ulong_dl* list) {
    if (!list) {
        errno = EINVAL;
        return ULONG_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return ULONG_MAX;
    }
    unsigned long dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        ulong_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long long int pop_llong_front_dllist(llong_dl* list) {
    if (!list) {
        errno = EINVAL;
        return LLONG_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return LLONG_MAX;
    }
    long long int dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        llong_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned long long pop_ullong_front_dllist(ullong_dl* list) {
    if (!list) {
        errno = EINVAL;
        return ULLONG_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return ULLONG_MAX;
    }
    unsigned long long dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        ullong_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

float pop_float_front_dllist(float_dl* list) {
    if (!list) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return FLT_MAX;
    }
    float dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        float_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

double pop_double_front_dllist(double_dl* list) {
    if (!list) {
        errno = EINVAL;
        return DBL_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return DBL_MAX;
    }
    double dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        double_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long double pop_ldouble_front_dllist(ldouble_dl* list) {
    if (!list) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return LDBL_MAX;
    }
    long double dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        ldouble_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

bool pop_bool_front_dllist(bool_dl* list) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return false;
    }
    bool dat = list->head->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        bool_dlnode *temp = list->head->next;
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

str* pop_string_front_dllist(string_dl* list) {
    if (!list) {
        errno = EINVAL;
        return NULL;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return NULL;
    }
    str* dat = init_string_nol(list->head->data->data);
    if (list->len == 1) {
        free_string(list->head->data);
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        string_dlnode *temp = list->head->next;
        free_string(list->head->data);
        free(list->head);
        temp->prev = NULL;
        list->head = temp;
    }
    list->len--;
    return dat;
}
// ================================================================================
// ================================================================================

char pop_char_back_dllist(char_dl* list) {
    if (!list) {
        errno = EINVAL;
        return SCHAR_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return SCHAR_MAX;
    }
    char dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned char pop_uchar_back_dllist(uchar_dl* list) {
    if (!list) {
        errno = EINVAL;
        return CHAR_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return CHAR_MAX;
    }
    unsigned char dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

short pop_short_back_dllist(short_dl* list) {
    if (!list) {
        errno = EINVAL;
        return SHRT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return SHRT_MAX;
    }
    short dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned short pop_ushort_back_dllist(ushort_dl* list) {
    if (!list) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return USHRT_MAX;
    }
    unsigned short dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

int pop_int_back_dllist(int_dl* list) {
    if (!list) {
        errno = EINVAL;
        return INT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return INT_MAX;
    }
    int dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned int pop_uint_back_dllist(uint_dl* list) {
    if (!list) {
        errno = EINVAL;
        return UINT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return UINT_MAX;
    }
    unsigned int dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long int pop_long_back_dllist(long_dl* list) {
    if (!list) {
        errno = EINVAL;
        return LONG_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
return LONG_MAX;
    }
    long int dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned long int pop_ulong_back_dllist(ulong_dl* list) {
    if (!list) {
        errno = EINVAL;
        return ULONG_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return ULONG_MAX;
    }
    unsigned long int dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long long int pop_llong_back_dllist(llong_dl* list) {
    if (!list) {
        errno = EINVAL;
        return LLONG_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return LLONG_MAX;
    }
    long long int dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned long long pop_ullong_back_dllist(ullong_dl* list) {
    if (!list) {
        errno = EINVAL;
        return ULLONG_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return ULLONG_MAX;
    }
    unsigned long long int dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

float pop_float_back_dllist(float_dl* list) {
    if (!list) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return FLT_MAX;
    }
    float dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

double pop_double_back_dllist(double_dl* list) {
    if (!list) {
        errno = EINVAL;
        return DBL_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return DBL_MAX;
    }
    double dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long double pop_ldouble_back_dllist(ldouble_dl* list) {
    if (!list) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return LDBL_MAX;
    }
    long double dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

bool pop_bool_back_dllist(bool_dl* list) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return false;
    }
    bool dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        free(list->tail->next);
        list->tail->next = NULL;
    }
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

str* pop_string_back_dllist(string_dl* list) {
    if (!list) {
        errno = EINVAL;
        return NULL;
    }
    if (list->len == 0) {
        errno = ERANGE;
        return NULL;
    }
    str* dat = list->tail->data;
    if (list->len == 1) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        string_dlnode* temp = list->tail;
        list->tail = list->tail->prev;
        list->tail->next = NULL;
        free(temp);
    }
    list->len--;
    return dat;
}
// ================================================================================
// ================================================================================

char pop_char_at_dllist(char_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return SCHAR_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return SCHAR_MAX;
    }
    if (index == 0) return pop_char_front_dllist(list);
    if (index == list->len - 1) {
        return pop_char_back_dllist(list);
    }
    char_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    char dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned char pop_uchar_at_dllist(uchar_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return CHAR_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return CHAR_MAX;
    }
    if (index == 0) return pop_uchar_front_dllist(list);
    if (index == list->len - 1) return pop_uchar_back_dllist(list);
    uchar_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    unsigned char dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

short pop_short_at_dllist(short_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return SHRT_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return SHRT_MAX;
    }
    if (index == 0) return pop_short_front_dllist(list);
    if (index == list->len - 1) return pop_short_back_dllist(list);
    short_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    short int dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned short int pop_ushort_at_dllist(ushort_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return USHRT_MAX;
    }
    if (index == 0) return pop_ushort_front_dllist(list);
    if (index == list->len - 1) return pop_ushort_back_dllist(list);
    ushort_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    unsigned short int dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

int pop_int_at_dllist(int_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return INT_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return INT_MAX;
    }
    if (index == 0) return pop_int_front_dllist(list);
    if (index == list->len - 1) return pop_int_back_dllist(list);
    int_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    int dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned int pop_uint_at_dllist(uint_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return UINT_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return UINT_MAX;
    }
    if (index == 0) return pop_uint_front_dllist(list);
    if (index == list->len - 1) return pop_uint_back_dllist(list);
    uint_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    unsigned int dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long int pop_long_at_dllist(long_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return LONG_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return LONG_MAX;
    }
    if (index == 0) return pop_long_front_dllist(list);
    if (index == list->len - 1) return pop_long_back_dllist(list);
    long_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    long int dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned long pop_ulong_at_dllist(ulong_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return ULONG_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return ULONG_MAX;
    }
    if (index == 0) return pop_ulong_front_dllist(list);
    if (index == list->len - 1) return pop_ulong_back_dllist(list);
    ulong_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    unsigned long dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long long int pop_llong_at_dllist(llong_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return LLONG_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return LLONG_MAX;
    }
    if (index == 0) return pop_llong_front_dllist(list);
    if (index == list->len - 1) return pop_llong_back_dllist(list);
    llong_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    long long int dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

unsigned long long int pop_ullong_at_dllist(ullong_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return ULLONG_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return ULLONG_MAX;
    }
    if (index == 0) return pop_ullong_front_dllist(list);
    if (index == list->len - 1) return pop_ullong_back_dllist(list);
    ullong_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    unsigned long long int dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

float pop_float_at_dllist(float_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return FLT_MAX;
    }
    if (index == 0) return pop_float_front_dllist(list);
    if (index == list->len - 1) return pop_float_back_dllist(list);
    float_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    float dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

double pop_double_at_dllist(double_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return DBL_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return DBL_MAX;
    }
    if (index == 0) return pop_double_front_dllist(list);
    if (index == list->len - 1) return pop_double_back_dllist(list);
    double_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    double dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

long double pop_ldouble_at_dllist(ldouble_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return LDBL_MAX;
    }
    if (index == 0) return pop_ldouble_front_dllist(list);
    if (index == list->len - 1) return pop_ldouble_back_dllist(list);
    ldouble_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    long double dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

bool pop_bool_at_dllist(bool_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return false;
    }
    if (index == 0) return pop_bool_front_dllist(list);
    if (index == list->len - 1) return pop_bool_back_dllist(list);
    bool_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    bool dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// --------------------------------------------------------------------------------

str* pop_string_at_dllist(string_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return NULL;
    }
    if (list->len == 0 || index >= list->len) {
        errno = ERANGE;
        return NULL;
    }
    if (index == 0) return pop_string_front_dllist(list);
    if (index == list->len - 1) return pop_string_back_dllist(list);
    string_dlnode *temp = NULL;
    if (index <= list->len / 2 ) { 
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    str* dat = temp->data;
    if (temp->prev)
        temp->prev->next = temp->next;
    if (temp->next)
        temp->next->prev = temp->prev;
    free(temp);
    list->len--;
    return dat;
}
// ================================================================================ 
// ================================================================================

char get_char_dllist(char_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return SCHAR_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return SCHAR_MAX;
    }
    char_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
          //  printf("%hhu\n", temp->data);
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

unsigned char get_uchar_dllist(uchar_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return CHAR_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return CHAR_MAX;
    }
    uchar_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

short int get_short_dllist(short_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return SHRT_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return SHRT_MAX;
    }
    short_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// -------------------------------------------------------------------------------- 

unsigned short int get_ushort_dllist(ushort_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return USHRT_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return USHRT_MAX;
    }
    ushort_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

int get_int_dllist(int_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return INT_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return INT_MAX;
    }
    int_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

unsigned int get_uint_dllist(uint_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return UINT_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return UINT_MAX;
    }
    uint_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

long int get_long_dllist(long_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return UINT_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return UINT_MAX;
    }
    long_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

unsigned long get_ulong_dllist(ulong_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return ULONG_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return ULONG_MAX;
    }
    ulong_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

long long int get_llong_dllist(llong_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return LLONG_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return LLONG_MAX;
    }
    llong_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

unsigned long long int get_ullong_dllist(ullong_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return ULLONG_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return ULLONG_MAX;
    }
    ullong_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

float get_float_dllist(float_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return FLT_MAX;
    }
    float_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

double get_double_dllist(double_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return DBL_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return DBL_MAX;
    }
    double_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

long double get_ldouble_dllist(ldouble_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return LDBL_MAX;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return LDBL_MAX;
    }
    ldouble_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

bool get_bool_dllist(bool_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return false;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return false;
    }
    bool_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return temp->data;
}
// --------------------------------------------------------------------------------

str* get_string_dllist(string_dl* list, size_t index) {
    if (!list) {
        errno = EINVAL;
        return NULL;
    }
    if (index > list->len - 1) {
        errno = ERANGE;
        return NULL;
    }
    string_dlnode *temp = NULL;
    if (index <= list->len / 2) {
        temp = list->head;
        for (size_t i = 0; i <= index - 1; i++) {
            temp = temp->next;
        }
    } else {
        temp = list->tail;
        for (size_t i = list->len - 1; i > index; i--) {
            temp = temp->prev;
        }
    }
    return init_string_nol(temp->data->data);
}
// ================================================================================
// ================================================================================
// eof
