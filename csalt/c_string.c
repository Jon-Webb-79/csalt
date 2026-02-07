// ================================================================================
// ================================================================================
// - File:    c_string.c
// - purpose: this file contains the implementation for functions used in the 
//            c_string library
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 30, 2024
// - Version: 0.1
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <string.h>
#include "c_string.h"
// ================================================================================ 
// ================================================================================ 

static inline string_expect_t string_error(error_code_t e) {
    string_expect_t out;
    out.has_value = false;
    out.u.error = e;
    return out;
}
// -------------------------------------------------------------------------------- 

string_expect_t init_string(const char* cstr,
                            size_t capacity_bytes,          // payload capacity (chars), excludes NUL
                            allocator_vtable_t allocator) {
    if (!cstr) return string_error(NULL_POINTER);
    if (!allocator.allocate || !allocator.return_element) return string_error(INVALID_ARG);

    size_t const src_len = strlen(cstr);                  // payload chars (no NUL)

    if (capacity_bytes == 0u) {
        capacity_bytes = src_len;                         // default: fit entire string
    }

    size_t const buf_bytes = capacity_bytes + 1u;         // always reserve NUL
    size_t const copy_len  = (src_len < capacity_bytes) ? src_len : capacity_bytes;

    // Allocate string object
    void_ptr_expect_t r = allocator.allocate(allocator.ctx, sizeof(string_t), true);
    if (!r.has_value) return string_error(r.u.error);

    string_t* s = (string_t*)r.u.value;

    // Allocate buffer
    r = allocator.allocate(allocator.ctx, buf_bytes, false);
    if (!r.has_value) {
        allocator.return_element(allocator.ctx, s);
        return string_error(r.u.error);
    }

    s->str       = (char*)r.u.value;
    s->len       = copy_len;
    s->alloc     = buf_bytes;        // store actual allocated bytes (recommended)
    s->allocator = allocator;

    if (copy_len > 0u) {
        memcpy(s->str, cstr, copy_len);
    }
    s->str[copy_len] = '\0';         // REQUIRED for truncation cases

    string_expect_t out;
    out.has_value = true;
    out.u.value = s;
    return out;
}
// -------------------------------------------------------------------------------- 

void return_string(string_t* s) {
    if (!s) return;

    // local copy avoids problems if someone overwrote s->allocator before free
    allocator_vtable_t a = s->allocator;

    if (s->str) {
        a.return_element(a.ctx, s->str);
        s->str = NULL;
    }

    a.return_element(a.ctx, s);
}
// ================================================================================
// ================================================================================
// eof
