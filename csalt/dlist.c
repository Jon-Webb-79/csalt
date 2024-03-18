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

static char_dlnode* _char_dl_begin(char_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static char_dlnode* _char_dl_end(char_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _char_dl_next(char_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _char_dl_prev(char_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static char _char_dl_get(char_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static uchar_dlnode* _uchar_dl_begin(uchar_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static uchar_dlnode* _uchar_dl_end(uchar_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _uchar_dl_next(uchar_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _uchar_dl_prev(uchar_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static unsigned char _uchar_dl_get(uchar_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static short_dlnode* _short_dl_begin(short_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static short_dlnode* _short_dl_end(short_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _short_dl_next(short_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _short_dl_prev(short_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static short int _short_dl_get(short_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static ushort_dlnode* _ushort_dl_begin(ushort_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static ushort_dlnode* _ushort_dl_end(ushort_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _ushort_dl_next(ushort_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _ushort_dl_prev(ushort_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static unsigned short _ushort_dl_get(ushort_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static int_dlnode* _int_dl_begin(int_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static int_dlnode* _int_dl_end(int_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _int_dl_next(int_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _int_dl_prev(int_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static int _int_dl_get(int_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static uint_dlnode* _uint_dl_begin(uint_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static uint_dlnode* _uint_dl_end(uint_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _uint_dl_next(uint_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _uint_dl_prev(uint_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static unsigned int _uint_dl_get(uint_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================ 

static long_dlnode* _long_dl_begin(long_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static long_dlnode* _long_dl_end(long_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _long_dl_next(long_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _long_dl_prev(long_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static long int _long_dl_get(long_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static ulong_dlnode* _ulong_dl_begin(ulong_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static ulong_dlnode* _ulong_dl_end(ulong_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _ulong_dl_next(ulong_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _ulong_dl_prev(ulong_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static unsigned long int _ulong_dl_get(ulong_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static llong_dlnode* _llong_dl_begin(llong_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static llong_dlnode* _llong_dl_end(llong_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _llong_dl_next(llong_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _llong_dl_prev(llong_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static long long int _llong_dl_get(llong_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static ullong_dlnode* _ullong_dl_begin(ullong_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static ullong_dlnode* _ullong_dl_end(ullong_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _ullong_dl_next(ullong_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _ullong_dl_prev(ullong_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static unsigned long long int _ullong_dl_get(ullong_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static float_dlnode* _float_dl_begin(float_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static float_dlnode* _float_dl_end(float_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _float_dl_next(float_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _float_dl_prev(float_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static float _float_dl_get(float_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static double_dlnode* _double_dl_begin(double_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static double_dlnode* _double_dl_end(double_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _double_dl_next(double_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _double_dl_prev(double_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static double _double_dl_get(double_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================ 

static ldouble_dlnode* _ldouble_dl_begin(ldouble_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static ldouble_dlnode* _ldouble_dl_end(ldouble_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _ldouble_dl_next(ldouble_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _ldouble_dl_prev(ldouble_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static long double _ldouble_dl_get(ldouble_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static bool_dlnode* _bool_dl_begin(bool_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static bool_dlnode* _bool_dl_end(bool_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _bool_dl_next(bool_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _bool_dl_prev(bool_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static bool _bool_dl_get(bool_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    return current->data;
}
// ================================================================================

static string_dlnode* _string_dl_begin(string_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->head;
}
// --------------------------------------------------------------------------------

static string_dlnode* _string_dl_end(string_dl* s) {
    if (!s) {
        errno = EINVAL;
        return NULL;
    }
    return s->tail; // End is represented as NULL in a linked list
}
// --------------------------------------------------------------------------------

static void _string_dl_next(string_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->next;
}
// --------------------------------------------------------------------------------

static void _string_dl_prev(string_dlnode** current) {
    if (current == NULL || *current == NULL) {
        errno = EINVAL;
        return;
    }
    *current = (*current)->prev;
}
// --------------------------------------------------------------------------------

static str* _string_dl_get(string_dlnode* current) {
    if (current == NULL) {
        errno = EINVAL;
        return 0;
    }
    str* val = init_string_nol(current->data->data);
    return val;
}
// ================================================================================
// ================================================================================

char_dl_iterator init_char_dllist_iterator() {
    char_dl_iterator iter;

    iter.begin = _char_dl_begin;
    iter.end = _char_dl_end;
    iter.next = _char_dl_next;
    iter.prev = _char_dl_prev;
    iter.get = _char_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

uchar_dl_iterator init_uchar_dllist_iterator() {
    uchar_dl_iterator iter;

    iter.begin = _uchar_dl_begin;
    iter.end = _uchar_dl_end;
    iter.next = _uchar_dl_next;
    iter.prev = _uchar_dl_prev;
    iter.get = _uchar_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

short_dl_iterator init_short_dllist_iterator() {
    short_dl_iterator iter;

    iter.begin = _short_dl_begin;
    iter.end = _short_dl_end;
    iter.next = _short_dl_next;
    iter.prev = _short_dl_prev;
    iter.get = _short_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

ushort_dl_iterator init_ushort_dllist_iterator() {
    ushort_dl_iterator iter;

    iter.begin = _ushort_dl_begin;
    iter.end = _ushort_dl_end;
    iter.next = _ushort_dl_next;
    iter.prev = _ushort_dl_prev;
    iter.get = _ushort_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

int_dl_iterator init_int_dllist_iterator() {
    int_dl_iterator iter;

    iter.begin = _int_dl_begin;
    iter.end = _int_dl_end;
    iter.next = _int_dl_next;
    iter.prev = _int_dl_prev;
    iter.get = _int_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

uint_dl_iterator init_uint_dllist_iterator() {
    uint_dl_iterator iter;

    iter.begin = _uint_dl_begin;
    iter.end = _uint_dl_end;
    iter.next = _uint_dl_next;
    iter.prev = _uint_dl_prev;
    iter.get = _uint_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

long_dl_iterator init_long_dllist_iterator() {
    long_dl_iterator iter;

    iter.begin = _long_dl_begin;
    iter.end = _long_dl_end;
    iter.next = _long_dl_next;
    iter.prev = _long_dl_prev;
    iter.get = _long_dl_get;
    return iter;
}
// -------------------------------------------------------------------------------- 

ulong_dl_iterator init_ulong_dllist_iterator() {
    ulong_dl_iterator iter;

    iter.begin = _ulong_dl_begin;
    iter.end = _ulong_dl_end;
    iter.next = _ulong_dl_next;
    iter.prev = _ulong_dl_prev;
    iter.get = _ulong_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

llong_dl_iterator init_llong_dllist_iterator() {
    llong_dl_iterator iter;

    iter.begin = _llong_dl_begin;
    iter.end = _llong_dl_end;
    iter.next = _llong_dl_next;
    iter.prev = _llong_dl_prev;
    iter.get = _llong_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

ullong_dl_iterator init_ullong_dllist_iterator() {
    ullong_dl_iterator iter;

    iter.begin = _ullong_dl_begin;
    iter.end = _ullong_dl_end;
    iter.next = _ullong_dl_next;
    iter.prev = _ullong_dl_prev;
    iter.get = _ullong_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

float_dl_iterator init_float_dllist_iterator() {
    float_dl_iterator iter;

    iter.begin = _float_dl_begin;
    iter.end = _float_dl_end;
    iter.next = _float_dl_next;
    iter.prev = _float_dl_prev;
    iter.get = _float_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

double_dl_iterator init_double_dllist_iterator() {
    double_dl_iterator iter;

    iter.begin = _double_dl_begin;
    iter.end = _double_dl_end;
    iter.next = _double_dl_next;
    iter.prev = _double_dl_prev;
    iter.get = _double_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

ldouble_dl_iterator init_ldouble_dllist_iterator() {
    ldouble_dl_iterator iter;

    iter.begin = _ldouble_dl_begin;
    iter.end = _ldouble_dl_end;
    iter.next = _ldouble_dl_next;
    iter.prev = _ldouble_dl_prev;
    iter.get = _ldouble_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

bool_dl_iterator init_bool_dllist_iterator() {
    bool_dl_iterator iter;

    iter.begin = _bool_dl_begin;
    iter.end = _bool_dl_end;
    iter.next = _bool_dl_next;
    iter.prev = _bool_dl_prev;
    iter.get = _bool_dl_get;
    return iter;
}
// --------------------------------------------------------------------------------

string_dl_iterator init_string_dllist_iterator() {
    string_dl_iterator iter;

    iter.begin = _string_dl_begin;
    iter.end = _string_dl_end;
    iter.next = _string_dl_next;
    iter.prev = _string_dl_prev;
    iter.get = _string_dl_get;
    return iter;
}
// ================================================================================
// ================================================================================

static void _bubble_sort_char_dllist(char_dl* list, size_t length, iter_dir direction) {
    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        char_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_char function
                swap_char(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_uchar_dllist(uchar_dl* list, size_t length, iter_dir direction) {
    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        uchar_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_uchar function
                swap_uchar(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_short_dllist(short_dl* list, size_t length, iter_dir direction) {
    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        short_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_short function
                swap_short(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ushort_dllist(ushort_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        ushort_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_ushort function
                swap_ushort(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_int_dllist(int_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        int_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_int function
                swap_int(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_uint_dllist(uint_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        uint_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_uint function
                swap_uint(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_long_dllist(long_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        long_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_long function
                swap_long(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ulong_dllist(ulong_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        ulong_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_ulong function
                swap_ulong(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_llong_dllist(llong_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        llong_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_llong function
                swap_llong(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ullong_dllist(ullong_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        ullong_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_ullong function
                swap_ullong(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_float_dllist(float_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        float_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_float function
                swap_float(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_double_dllist(double_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        double_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_double function
                swap_double(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_ldouble_dllist(ldouble_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        ldouble_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_ldouble function
                swap_ldouble(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_bool_dllist(bool_dl* list, size_t length, iter_dir direction) {

    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        bool_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare = (direction == FORWARD) ? (current->data > current->next->data) : (current->data < current->next->data);

            if (compare) {
                // Swap data using the swap_bool function
                swap_bool(&current->data, &current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length since the last element is in its correct place
    } while (swapped);
}
// --------------------------------------------------------------------------------

static void _bubble_sort_string_dllist(string_dl* list, size_t length, iter_dir direction) {
    if (!list || length < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int swapped;
    do {
        swapped = 0;
        string_dlnode* current = list->head;
        for (size_t i = 0; i < length - 1 && current && current->next; ++i) {
            // Decide direction of comparison
            int compare;
            if (direction == FORWARD) {
                compare = strcmp(current->data->data, current->next->data->data) > 0;
            } else { // REVERSE
                compare = strcmp(current->data->data, current->next->data->data) < 0;
            }

            if (compare) {
                // Swap data using the swap_string function
                swap_string(current->data, current->next->data);
                swapped = 1; // Mark as swapped
            }
            current = current->next; // Move to next node
        }
        length--; // Reduce length as the last element is now in correct place
    } while (swapped);
}
// ================================================================================

static void _selection_sort_char_dllist(char_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    char_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_char to swap the data
            swap_char(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_uchar_dllist(uchar_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    uchar_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_uchar to swap the data
            swap_uchar(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_short_dllist(short_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    short_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_short to swap the data
            swap_short(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_ushort_dllist(ushort_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    ushort_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_ushort to swap the data
            swap_ushort(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_int_dllist(int_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_int to swap the data
            swap_int(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_uint_dllist(uint_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    uint_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_uint to swap the data
            swap_uint(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_long_dllist(long_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    long_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_long to swap the data
            swap_long(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_ulong_dllist(ulong_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    ulong_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_ulong to swap the data
            swap_ulong(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_llong_dllist(llong_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    llong_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_llong to swap the data
            swap_llong(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_ullong_dllist(ullong_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    ullong_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_ullong to swap the data
            swap_ullong(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_float_dllist(float_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    float_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_float to swap the data
            swap_float(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_double_dllist(double_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    double_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_double to swap the data
            swap_double(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_ldouble_dllist(ldouble_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    ldouble_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_ldouble to swap the data
            swap_ldouble(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_bool_dllist(bool_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    bool_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            int compare = (direction == FORWARD) ? 
                          (next->data < min_max->data) : 
                          (next->data > min_max->data);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Use swap_bool to swap the data
            swap_bool(&(current->data), &(min_max->data));
        }
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _selection_sort_string_dllist(string_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    string_dlnode *current, *next, *min_max;
    current = list->head;

    for (size_t i = 0; i < length - 1; ++i) {
        min_max = current;
        next = current->next;
        for (size_t j = i + 1; j < length && next; ++j) {
            // Comparing string values
            int compare = (direction == FORWARD) ? 
                          (strcmp(next->data->data, min_max->data->data) < 0) : 
                          (strcmp(next->data->data, min_max->data->data) > 0);

            if (compare) {
                min_max = next;
            }
            next = next->next;
        }
        if (min_max != current) {
            // Swap the str* pointers between nodes
            swap_string(current->data, min_max->data);
        }
        current = current->next;
    }
}
// ================================================================================

static void _insertion_sort_char_dllist(char_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    char_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        char data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_uchar_dllist(uchar_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    uchar_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        unsigned char data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_short_dllist(short_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    short_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        short int data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_ushort_dllist(ushort_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    ushort_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        unsigned short data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_int_dllist(int_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    int_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        int data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_uint_dllist(uint_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    uint_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        unsigned int data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_long_dllist(long_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    long_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        long int data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_ulong_dllist(ulong_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    ulong_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        unsigned long int data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_llong_dllist(llong_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    llong_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        long long int data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_ullong_dllist(ullong_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    ullong_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        unsigned long long int data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_float_dllist(float_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    float_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        float data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_double_dllist(double_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    double_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        double data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_ldouble_dllist(ldouble_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    ldouble_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        long double data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_bool_dllist(bool_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    bool_dlnode *current, *insertionPoint;
    current = list->head->next; // Start from the second element

    for (size_t i = 1; i < length; ++i) {
        bool data = current->data;
        insertionPoint = current->prev;

        if (direction == FORWARD) {
            while (insertionPoint != NULL && insertionPoint->data > data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        } else { // REVERSE direction
            while (insertionPoint != NULL && insertionPoint->data < data) {
                insertionPoint->next->data = insertionPoint->data; // Shift data right
                insertionPoint = insertionPoint->prev;
            }
        }
        if (insertionPoint == NULL) { // Insertion at the beginning
            list->head->data = data;
        } else {
            insertionPoint->next->data = data;
        }

        // Move to the next element to sort
        current = current->next;
    }
}
// --------------------------------------------------------------------------------

static void _insertion_sort_string_dllist(string_dl* list, size_t length, iter_dir direction) {
    if (!list || list->len < 2) {
        return; // No need to sort if the list is empty or has a single element
    }

    string_dlnode* sortedEnd = list->head;
    while (sortedEnd->next != NULL) {
        string_dlnode* curr = sortedEnd->next;
        str* data = curr->data;

        if ((direction == FORWARD && strcmp(data->data, list->head->data->data) <= 0) ||
            (direction == REVERSE && strcmp(data->data, list->head->data->data) >= 0)) {
            // Move current node to the beginning
            sortedEnd->next = curr->next;
            curr->next = list->head;
            list->head = curr;
        } else {
            string_dlnode* search = list->head;
            while (search->next != curr && ((direction == FORWARD && strcmp(data->data, search->next->data->data) > 0) ||
                                            (direction == REVERSE && strcmp(data->data, search->next->data->data) < 0))) {
                search = search->next;
            }

            if (search->next != curr) {
                // Relink nodes to insert curr at the correct position
                sortedEnd->next = curr->next;
                curr->next = search->next;
                search->next = curr;
            } else {
                // Current is already in the correct position
                sortedEnd = curr;
            }
        }
    }
}
// ================================================================================ 

static void _split_char_list(char_dlnode* source, char_dlnode** frontRef, char_dlnode** backRef) {
    char_dlnode* fast;
    char_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static char_dlnode* _merge_char_list(char_dlnode* a, char_dlnode* b, iter_dir direction) {
    char_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_char_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_char_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_char_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_char_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_char_dllist(char_dlnode** headRef, iter_dir direction) {
    char_dlnode* head = *headRef;
    char_dlnode* a;
    char_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_char_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_char_dllist(&a, direction);
    _merge_sort_char_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_char_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_uchar_list(uchar_dlnode* source, uchar_dlnode** frontRef, uchar_dlnode** backRef) {
    uchar_dlnode* fast;
    uchar_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static uchar_dlnode* _merge_uchar_list(uchar_dlnode* a, uchar_dlnode* b, iter_dir direction) {
    uchar_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_uchar_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_uchar_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_uchar_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_uchar_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_uchar_dllist(uchar_dlnode** headRef, iter_dir direction) {
    uchar_dlnode* head = *headRef;
    uchar_dlnode* a;
    uchar_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_uchar_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_uchar_dllist(&a, direction);
    _merge_sort_uchar_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_uchar_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_short_list(short_dlnode* source, short_dlnode** frontRef, short_dlnode** backRef) {
    short_dlnode* fast;
    short_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static short_dlnode* _merge_short_list(short_dlnode* a, short_dlnode* b, iter_dir direction) {
    short_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_short_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_short_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_short_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_short_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_short_dllist(short_dlnode** headRef, iter_dir direction) {
    short_dlnode* head = *headRef;
    short_dlnode* a;
    short_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_short_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_short_dllist(&a, direction);
    _merge_sort_short_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_short_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_ushort_list(ushort_dlnode* source, ushort_dlnode** frontRef, ushort_dlnode** backRef) {
    ushort_dlnode* fast;
    ushort_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static ushort_dlnode* _merge_ushort_list(ushort_dlnode* a, ushort_dlnode* b, iter_dir direction) {
    ushort_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_ushort_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_ushort_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_ushort_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_ushort_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_ushort_dllist(ushort_dlnode** headRef, iter_dir direction) {
    ushort_dlnode* head = *headRef;
    ushort_dlnode* a;
    ushort_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_ushort_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_ushort_dllist(&a, direction);
    _merge_sort_ushort_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_ushort_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_int_list(int_dlnode* source, int_dlnode** frontRef, int_dlnode** backRef) {
    int_dlnode* fast;
    int_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static int_dlnode* _merge_int_list(int_dlnode* a, int_dlnode* b, iter_dir direction) {
    int_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_int_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_int_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_int_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_int_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_int_dllist(int_dlnode** headRef, iter_dir direction) {
    int_dlnode* head = *headRef;
    int_dlnode* a;
    int_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_int_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_int_dllist(&a, direction);
    _merge_sort_int_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_int_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_uint_list(uint_dlnode* source, uint_dlnode** frontRef, uint_dlnode** backRef) {
    uint_dlnode* fast;
    uint_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static uint_dlnode* _merge_uint_list(uint_dlnode* a, uint_dlnode* b, iter_dir direction) {
    uint_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_uint_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_uint_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_uint_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_uint_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_uint_dllist(uint_dlnode** headRef, iter_dir direction) {
    uint_dlnode* head = *headRef;
    uint_dlnode* a;
    uint_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_uint_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_uint_dllist(&a, direction);
    _merge_sort_uint_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_uint_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_long_list(long_dlnode* source, long_dlnode** frontRef, long_dlnode** backRef) {
    long_dlnode* fast;
    long_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static long_dlnode* _merge_long_list(long_dlnode* a, long_dlnode* b, iter_dir direction) {
    long_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_long_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_long_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_long_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_long_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_long_dllist(long_dlnode** headRef, iter_dir direction) {
    long_dlnode* head = *headRef;
    long_dlnode* a;
    long_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_long_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_long_dllist(&a, direction);
    _merge_sort_long_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_long_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_ulong_list(ulong_dlnode* source, ulong_dlnode** frontRef, ulong_dlnode** backRef) {
    ulong_dlnode* fast;
    ulong_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static ulong_dlnode* _merge_ulong_list(ulong_dlnode* a, ulong_dlnode* b, iter_dir direction) {
    ulong_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_ulong_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_ulong_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_ulong_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_ulong_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_ulong_dllist(ulong_dlnode** headRef, iter_dir direction) {
    ulong_dlnode* head = *headRef;
    ulong_dlnode* a;
    ulong_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_ulong_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_ulong_dllist(&a, direction);
    _merge_sort_ulong_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_ulong_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_llong_list(llong_dlnode* source, llong_dlnode** frontRef, llong_dlnode** backRef) {
    llong_dlnode* fast;
    llong_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static llong_dlnode* _merge_llong_list(llong_dlnode* a, llong_dlnode* b, iter_dir direction) {
    llong_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_llong_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_llong_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_llong_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_llong_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_llong_dllist(llong_dlnode** headRef, iter_dir direction) {
    llong_dlnode* head = *headRef;
    llong_dlnode* a;
    llong_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_llong_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_llong_dllist(&a, direction);
    _merge_sort_llong_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_llong_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_ullong_list(ullong_dlnode* source, ullong_dlnode** frontRef, ullong_dlnode** backRef) {
    ullong_dlnode* fast;
    ullong_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static ullong_dlnode* _merge_ullong_list(ullong_dlnode* a, ullong_dlnode* b, iter_dir direction) {
    ullong_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_ullong_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_ullong_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_ullong_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_ullong_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_ullong_dllist(ullong_dlnode** headRef, iter_dir direction) {
    ullong_dlnode* head = *headRef;
    ullong_dlnode* a;
    ullong_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_ullong_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_ullong_dllist(&a, direction);
    _merge_sort_ullong_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_ullong_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_float_list(float_dlnode* source, float_dlnode** frontRef, float_dlnode** backRef) {
    float_dlnode* fast;
    float_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static float_dlnode* _merge_float_list(float_dlnode* a, float_dlnode* b, iter_dir direction) {
    float_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_float_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_float_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_float_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_float_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_float_dllist(float_dlnode** headRef, iter_dir direction) {
    float_dlnode* head = *headRef;
    float_dlnode* a;
    float_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_float_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_float_dllist(&a, direction);
    _merge_sort_float_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_float_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_double_list(double_dlnode* source, double_dlnode** frontRef, double_dlnode** backRef) {
    double_dlnode* fast;
    double_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static double_dlnode* _merge_double_list(double_dlnode* a, double_dlnode* b, iter_dir direction) {
    double_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_double_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_double_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_double_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_double_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_double_dllist(double_dlnode** headRef, iter_dir direction) {
    double_dlnode* head = *headRef;
    double_dlnode* a;
    double_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_double_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_double_dllist(&a, direction);
    _merge_sort_double_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_double_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_ldouble_list(ldouble_dlnode* source, ldouble_dlnode** frontRef, ldouble_dlnode** backRef) {
    ldouble_dlnode* fast;
    ldouble_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static ldouble_dlnode* _merge_ldouble_list(ldouble_dlnode* a, ldouble_dlnode* b, iter_dir direction) {
    ldouble_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_ldouble_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_ldouble_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_ldouble_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_ldouble_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_ldouble_dllist(ldouble_dlnode** headRef, iter_dir direction) {
    ldouble_dlnode* head = *headRef;
    ldouble_dlnode* a;
    ldouble_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_ldouble_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_ldouble_dllist(&a, direction);
    _merge_sort_ldouble_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_ldouble_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_bool_list(bool_dlnode* source, bool_dlnode** frontRef, bool_dlnode** backRef) {
    bool_dlnode* fast;
    bool_dlnode* slow;
    if (source == NULL || source->next == NULL) { // Base cases
        *frontRef = source;
        *backRef = NULL;
    } else {
        slow = source;
        fast = source->next;
        // 'fast' advances two nodes, and 'slow' advances one node
        while (fast != NULL) {
            fast = fast->next;
            if (fast != NULL) {
                slow = slow->next;
                fast = fast->next;
            }
        }
        // 'slow' is before the midpoint in the list, so split it in two at that point.
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}
// --------------------------------------------------------------------------------

static bool_dlnode* _merge_bool_list(bool_dlnode* a, bool_dlnode* b, iter_dir direction) {
    bool_dlnode* result = NULL;

    // Base cases
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    // Pick either a or b, and recur
    if (direction == FORWARD) {
        if (a->data <= b->data) {
            result = a;
            result->next = _merge_bool_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_bool_list(a, b->next, direction);
        }
    } else { // REVERSE
        if (a->data >= b->data) {
            result = a;
            result->next = _merge_bool_list(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_bool_list(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_bool_dllist(bool_dlnode** headRef, iter_dir direction) {
    bool_dlnode* head = *headRef;
    bool_dlnode* a;
    bool_dlnode* b;

    // Base case -- length 0 or 1
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // Split head into 'a' and 'b' sublists
    _split_bool_list(head, &a, &b);

    // Recursively sort the sublists
    _merge_sort_bool_dllist(&a, direction);
    _merge_sort_bool_dllist(&b, direction);

    // Merge the two sorted lists together
    *headRef = _merge_bool_list(a, b, direction);
}
// --------------------------------------------------------------------------------

static void _split_string_list(string_dlnode* source, string_dlnode** frontRef, string_dlnode** backRef) {
    string_dlnode* fast;
    string_dlnode* slow;
    if (source == NULL || source->next == NULL) {
        *frontRef = source;
        *backRef = NULL;
    } else {
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
}
// --------------------------------------------------------------------------------

static string_dlnode* _merge_string_dllist(string_dlnode* a, string_dlnode* b, iter_dir direction) {
    string_dlnode* result = NULL;

    if (a == NULL) return b;
    else if (b == NULL) return a;

    if (direction == FORWARD) {
        if (strcmp(a->data->data, b->data->data) <= 0) {
            result = a;
            result->next = _merge_string_dllist(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_string_dllist(a, b->next, direction);
        }
    } else { // REVERSE
        if (strcmp(a->data->data, b->data->data) > 0) {
            result = a;
            result->next = _merge_string_dllist(a->next, b, direction);
        } else {
            result = b;
            result->next = _merge_string_dllist(a, b->next, direction);
        }
    }
    return result;
}
// --------------------------------------------------------------------------------

static void _merge_sort_string_dllist(string_dlnode** headRef, iter_dir direction) {
    string_dlnode* head = *headRef;
    string_dlnode* a;
    string_dlnode* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    _split_string_list(head, &a, &b);
    _merge_sort_string_dllist(&a, direction);
    _merge_sort_string_dllist(&b, direction);

    *headRef = _merge_string_dllist(a, b, direction);
}
// ================================================================================

void sort_char_dllist(char_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_char_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_char_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_char_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_char_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_uchar_dllist(uchar_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_uchar_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_uchar_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_uchar_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_uchar_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_short_dllist(short_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_short_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_short_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_short_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_short_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ushort_dllist(ushort_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_ushort_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_ushort_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_ushort_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_ushort_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_int_dllist(int_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_int_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_int_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_int_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_int_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_uint_dllist(uint_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_uint_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_uint_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_uint_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_uint_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_long_dllist(long_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_long_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_long_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_long_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_long_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ulong_dllist(ulong_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_ulong_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_ulong_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_ulong_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_ulong_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_llong_dllist(llong_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_llong_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_llong_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_llong_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_llong_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ullong_dllist(ullong_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_ullong_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_ullong_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_ullong_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_ullong_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_float_dllist(float_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_float_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_float_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_float_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_float_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_double_dllist(double_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_double_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_double_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_double_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_double_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_ldouble_dllist(ldouble_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_ldouble_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_ldouble_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_ldouble_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_ldouble_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_bool_dllist(bool_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_bool_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_bool_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_bool_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_bool_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// --------------------------------------------------------------------------------

void sort_string_dllist(string_dl* list, sort_type stype, iter_dir direction) {
    if (!list || !list->head || list->len < 2) {
        errno = EINVAL;
        return; // No need to sort if the list is empty or has only one element
    }
    switch(stype) {
        case BUBBLE:
            _bubble_sort_string_dllist(list, list->len, direction);
            break;
        case SELECTION:
            _selection_sort_string_dllist(list, list->len, direction);
            break;
        case INSERT:
            _insertion_sort_string_dllist(list, list->len, direction);
            break;
        case MERGE:
            _merge_sort_string_dllist(&list->head, direction);
        default:
            errno = EINVAL;
            return;
    }
}
// ================================================================================
// ================================================================================
// eof
