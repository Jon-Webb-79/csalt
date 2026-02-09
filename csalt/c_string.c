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
// -------------------------------------------------------------------------------- 

bool str_concat(string_t* s, const char* str) {
    if (!s || !s->str || !str) return false;

    allocator_vtable_t* a = &s->allocator;

    // We need at least allocate + return_element; reallocate is optional (fallback below).
    if (!a->allocate || !a->return_element) return false;

    size_t const len1 = s->len;
    size_t const len2 = strlen(str);

    if (len2 == 0u) return true;

    // overflow guard: len1 + len2 + 1
    if (len2 > (SIZE_MAX - 1u - len1)) return false;

    size_t const needed = len1 + len2 + 1u; // bytes including NUL

    // Check whether source pointer aliases current string buffer (including NUL)
    const char* const s_begin = s->str;
    const char* const s_end   = s->str + (len1 + 1u);
    bool const overlaps = (str >= s_begin && str < s_end);

    const char* src = str;
    char* temp = NULL;

    // If we must grow and src aliases s->str, we must copy src aside before growing.
    if (needed > s->alloc && overlaps) {
        void_ptr_expect_t tr = a->allocate(a->ctx, len2, false);
        if (!tr.has_value) return false;

        temp = (char*)tr.u.value;
        memcpy(temp, str, len2);
        src = temp;
    }

    // Ensure capacity
    if (needed > s->alloc) {
        if (a->reallocate) {
            // Preferred: reallocate in-place or move
            void_ptr_expect_t rr =
                a->reallocate(a->ctx, s->str, s->alloc, needed, false);
            if (!rr.has_value) {
                if (temp) a->return_element(a->ctx, temp);
                return false;
            }
            s->str = (char*)rr.u.value;
            s->alloc = needed;
        } else {
            // Fallback: allocate new buffer + copy old + append + (attempt) return old buffer
            void_ptr_expect_t nr = a->allocate(a->ctx, needed, false);
            if (!nr.has_value) {
                if (temp) a->return_element(a->ctx, temp);
                return false;
            }

            char* newbuf = (char*)nr.u.value;

            // Copy old content including NUL
            memcpy(newbuf, s->str, len1 + 1u);

            // Best-effort return old buffer (no-op for arenas)
            a->return_element(a->ctx, s->str);

            s->str = newbuf;
            s->alloc = needed;
        }
    }

    // Append safely (memmove handles overlap)
    memmove(s->str + len1, src, len2);
    s->str[len1 + len2] = '\0';
    s->len = len1 + len2;

    if (temp) a->return_element(a->ctx, temp);
    return true;
}
// -------------------------------------------------------------------------------- 

bool string_concat(string_t* s, const string_t* str) {
    if (!str) return false;
    return str_concat(s, const_string(str));
}
// ================================================================================
// ================================================================================
// eof
