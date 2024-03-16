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
// eof
