// ================================================================================
// ================================================================================
// - File:    c_ldouble.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 08, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here
#include <string.h>
#include "c_ldouble.h"
// ================================================================================ 
// ================================================================================ 

static inline ldouble_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (ldouble_array_expect_t){
            .has_value = true,
            .u.value   = (ldouble_array_t*)e.u.value
        };
    }
    return (ldouble_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

/*
 * Three-way long double comparator. The (va > vb) - (va < vb) idiom is safe
 * for all finite values and signed zeros under IEEE 754. NaN produces 0 for
 * both sub-expressions, giving unspecified ordering — callers must ensure no
 * NaN is present.
 */
static int _cmp_ldouble(const void* a, const void* b) {
    long double va = *(const long double*)a;
    long double vb = *(const long double*)b;
    return (va > vb) - (va < vb);
}

static void _add_ldouble(void* accum, const void* element) {
    *(long double*)accum += *(const long double*)element;
}

// ================================================================================
// Initialization and teardown
// ================================================================================

ldouble_array_expect_t init_ldouble_array(size_t             capacity,
                                          bool               growth,
                                          allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, LDOUBLE_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_ldouble_array(ldouble_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_ldouble_array(ldouble_array_t* array, long double value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, LDOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_ldouble_array(ldouble_array_t* array, long double value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, LDOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_ldouble_array(ldouble_array_t* array,
                                   size_t           index,
                                   long double      value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, LDOUBLE_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_ldouble_array_index(const ldouble_array_t* array,
                                     size_t                 index,
                                     long double*           out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, LDOUBLE_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_ldouble_array(ldouble_array_t* array, long double* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, LDOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_ldouble_array(ldouble_array_t* array, long double* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, LDOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_ldouble_array(ldouble_array_t* array,
                                   long double*     out,
                                   size_t           index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, LDOUBLE_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_ldouble_array(ldouble_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_ldouble_array_index(ldouble_array_t* array,
                                     size_t           index,
                                     long double      value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, LDOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

ldouble_array_expect_t copy_ldouble_array(const ldouble_array_t* src,
                                          allocator_vtable_t     alloc_v) {
    if (src == NULL) {
        return (ldouble_array_expect_t){ .has_value = false,
                                         .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_ldouble_array(ldouble_array_t*       dst,
                                  const ldouble_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

ldouble_array_expect_t slice_ldouble_array(const ldouble_array_t* src,
                                           size_t                 start,
                                           size_t                 end,
                                           allocator_vtable_t     alloc_v) {
    if (src == NULL) {
        return (ldouble_array_expect_t){ .has_value = false,
                                         .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_ldouble_array(ldouble_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// ================================================================================
// Sort
// ================================================================================

error_code_t sort_ldouble_array(ldouble_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_ldouble, dir);
}

// ================================================================================
// Reduction
// ================================================================================

ldouble_expect_t ldouble_array_min(const ldouble_array_t* array) {
    if (array == NULL)
        return (ldouble_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_ldouble, LDOUBLE_TYPE);
    if (!idx.has_value)
        return (ldouble_expect_t){ .has_value = false, .u.error = idx.u.error };
    long double val = 0.0L;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val,
                                       LDOUBLE_TYPE);
    if (err != NO_ERROR)
        return (ldouble_expect_t){ .has_value = false, .u.error = err };
    return (ldouble_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

ldouble_expect_t ldouble_array_max(const ldouble_array_t* array) {
    if (array == NULL)
        return (ldouble_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_ldouble, LDOUBLE_TYPE);
    if (!idx.has_value)
        return (ldouble_expect_t){ .has_value = false, .u.error = idx.u.error };
    long double val = 0.0L;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val,
                                       LDOUBLE_TYPE);
    if (err != NO_ERROR)
        return (ldouble_expect_t){ .has_value = false, .u.error = err };
    return (ldouble_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

ldouble_expect_t ldouble_array_sum(const ldouble_array_t* array) {
    if (array == NULL)
        return (ldouble_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    long double total = 0.0L;
    error_code_t err = array_sum(&array->base, &total, _add_ldouble,
                                 LDOUBLE_TYPE);
    if (err != NO_ERROR)
        return (ldouble_expect_t){ .has_value = false, .u.error = err };
    return (ldouble_expect_t){ .has_value = true, .u.value = total };
}

// --------------------------------------------------------------------------------

ldouble_array_expect_t cumulative_ldouble_array(const ldouble_array_t* src,
                                                allocator_vtable_t     alloc) {
    if (src == NULL)
        return (ldouble_array_expect_t){ .has_value = false,
                                         .u.error   = NULL_POINTER };
    array_expect_t r = cumulative_array(&src->base, _add_ldouble, alloc,
                                        LDOUBLE_TYPE);
    if (!r.has_value)
        return (ldouble_array_expect_t){ .has_value = false,
                                         .u.error   = r.u.error };
    return (ldouble_array_expect_t){ .has_value = true,
                                     .u.value   = (ldouble_array_t*)r.u.value };
}

// ================================================================================
// Search
//
// No SIMD path exists for long double on any architecture. Both contains and
// binary_search are implemented as portable scalar loops directly in this
// translation unit.
// ================================================================================

size_expect_t ldouble_array_contains(const ldouble_array_t* array,
                                     long double            value,
                                     size_t                 start,
                                     size_t                 end,
                                     long double            abs_tol,
                                     long double            rel_tol) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (abs_tol < 0.0L || rel_tol < 0.0L)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (start >= end)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (end > array->base.len)
        return (size_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };

    const long double* data = (const long double*)array->base.data;

    if (abs_tol == 0.0L && rel_tol == 0.0L) {
        /*
         * Exact mode: value-level equality with explicit sign-of-zero and NaN
         * handling. We cannot use _biteq_ldouble here because stored long
         * doubles may have unpredictable padding bytes beyond the 10
         * significant x87 bytes (written by memcpy from arbitrary stack slots
         * in push_back_array), which would cause false negatives.
         *
         *   NaN:   isnanl(value) — a stored NaN is never found.
         *   -0.0L vs +0.0L: signbitl distinguishes them; neither matches the
         *                    other, matching the documented bitwise semantics.
         *   All other values: simple == comparison is correct.
         */
        if (isnanl(value)) {
            return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
        }
        bool value_is_negzero = (value == 0.0L && signbit(value));
        for (size_t i = start; i < end; i++) {
            if (isnanl(data[i])) continue;
            bool elem_is_negzero = (data[i] == 0.0L && signbit(data[i]));
            if (value_is_negzero != elem_is_negzero) continue;
            if (data[i] == value)
                return (size_expect_t){ .has_value = true, .u.value = i };
        }
    } else {
        /* Tolerance mode. */
        for (size_t i = start; i < end; i++) {
            long double diff      = fabsl(data[i] - value);
            long double threshold = fmaxl(abs_tol,
                                          rel_tol * fmaxl(fabsl(data[i]),
                                                          fabsl(value)));
            if (diff <= threshold)
                return (size_expect_t){ .has_value = true, .u.value = i };
        }
    }

    return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
}

// --------------------------------------------------------------------------------

size_expect_t ldouble_array_binary_search(ldouble_array_t* array,
                                          long double      value,
                                          bool             sort,
                                          long double      abs_tol,
                                          long double      rel_tol) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (abs_tol < 0.0L || rel_tol < 0.0L)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (array->base.len == 0u)
        return (size_expect_t){ .has_value = false, .u.error = EMPTY };

    if (sort)
        sort_array(&array->base, _cmp_ldouble, FORWARD);

    const long double* data = (const long double*)array->base.data;
    size_t             len  = array->base.len;

    /* ── Phase 1: exact bisection to the landing position ── */
    size_t lo = 0u, hi = len - 1u, mid = 0u;
    while (lo < hi) {
        mid = lo + (hi - lo) / 2u;
        int c = _cmp_ldouble(data + mid, &value);
        if      (c < 0) lo = mid + 1u;
        else if (c > 0) { if (mid == 0u) { lo = hi + 1u; break; } hi = mid; }
        else            { lo = hi = mid; break; }
    }
    if (lo > hi) {
        return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
    }
    mid = lo;

    /* ── Phase 2: scalar fan-out in both directions ── */
    size_t best = SIZE_MAX;

    if (abs_tol == 0.0L && rel_tol == 0.0L) {
        /*
         * Exact mode: use _cmp_ldouble(a, b) == 0 rather than _biteq_ldouble.
         *
         * _biteq_ldouble compares raw bytes including padding, but stored long
         * doubles may have unpredictable padding beyond the 10 significant x87
         * bytes (written by memcpy from arbitrary stack slots in push_back_array),
         * causing false negatives on otherwise identical values.
         *
         * _cmp_ldouble uses (va > vb) - (va < vb) which is safe for ±INFINITY
         * (no subtraction, so no inf-inf = NaN) and matches IEEE 754 value
         * equality. This is appropriate here because binary_search operates on
         * a sorted array: -0.0L and +0.0L sort to the same position and are
         * treated as equal, consistent with the sort comparator used.
         */
        if (_cmp_ldouble(data + mid, &value) == 0) best = mid;

        size_t L = mid;
        while (L > 0u && _cmp_ldouble(data + L - 1u, &value) == 0) {
            best = --L;
        }
        if (best == SIZE_MAX) {
            size_t R = mid + 1u;
            while (R < len && _cmp_ldouble(data + R, &value) == 0) {
                best = (best == SIZE_MAX) ? R : best;
                R++;
            }
        }
    } else {
        /*
         * Tolerance mode: match when
         *   fabsl(element - value) <= fmaxl(abs_tol,
         *                                   rel_tol * fmaxl(|element|, |value|))
         *
         * Fan left from mid to find the lowest-index match, then right only
         * if nothing was found at or to the left of mid.
         */
        long double abs_value = fabsl(value);

#define _IN_TOL_LD(idx) \
    (fabsl(data[(idx)] - value) <= \
         fmaxl(abs_tol, rel_tol * fmaxl(fabsl(data[(idx)]), abs_value)))

        if (_IN_TOL_LD(mid)) best = mid;

        size_t L = mid;
        while (L > 0u && _IN_TOL_LD(L - 1u)) { best = --L; }

        if (best == SIZE_MAX) {
            size_t R = mid + 1u;
            while (R < len && _IN_TOL_LD(R)) {
                best = (best == SIZE_MAX) ? R : best;
                R++;
            }
        }

#undef _IN_TOL_LD
    }

    if (best == SIZE_MAX)
        return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
    return (size_expect_t){ .has_value = true, .u.value = best };
}

// --------------------------------------------------------------------------------

bracket_expect_t ldouble_array_binary_bracket(ldouble_array_t* array,
                                              long double      value,
                                              bool             sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_ldouble, sort,
                                LDOUBLE_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t ldouble_array_size(const ldouble_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t ldouble_array_alloc(const ldouble_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t ldouble_array_data_size(const ldouble_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_ldouble_array_empty(const ldouble_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_ldouble_array_full(const ldouble_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_ldouble_array_ptr(const ldouble_array_t* array,
                          const long double*     ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// -------------------------------------------------------------------------------- 

error_code_t print_ldouble_array(const ldouble_array_t* array, FILE* stream) {
    if (array == NULL || stream == NULL) return NULL_POINTER;

    const long double* data = (const long double*)array->base.data;
    size_t len = array->base.len;
    size_t col = 0u;

    fputs("[ ", stream);
    col = 2u;

    if (len == 0u) {
        fputs("]", stream);
        return NO_ERROR;
    }

    for (size_t i = 0u; i < len; ++i) {
        char buf[64];
        int n = snprintf(buf, sizeof(buf), "%Lg", data[i]);
        size_t value_len = (size_t)n;

        if (i == 0u) {
            if (col + value_len > 70u) {
                fputc('\n', stream);
                fputs("  ", stream);
                col = 2u;
            }
            fputs(buf, stream);
            col += value_len;
        } else {
            size_t token_len = 2u + value_len;
            if (col + token_len > 70u) {
                fputs(",\n  ", stream);
                col = 2u;
                fputs(buf, stream);
                col += value_len;
            } else {
                fputs(", ", stream);
                fputs(buf, stream);
                col += token_len;
            }
        }
    }

    if (col + 2u > 70u) {
        fputc('\n', stream);
        fputs("  ]", stream);
    } else {
        fputs(" ]", stream);
    }

    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

bool ldouble_array_equal(const ldouble_array_t* a,
                         const ldouble_array_t* b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    /* Fast path: same object */
    if (a == b) {
        return true;
    }

    /* Length must match */
    if (a->base.len != b->base.len) {
        return false;
    }

    /* Empty arrays are equal */
    if (a->base.len == 0u) {
        return true;
    }

    for (size_t i = 0u; i < a->base.len; ++i) {
        if (a->base.data[i] != b->base.data[i]) {
            return false;
        }
    }

    return true;
}
// ================================================================================ 
// ================================================================================ 

static inline dict_key_t _key(const char* s, size_t len) {
    return (dict_key_t){ .data = s, .len = len };
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
ldouble_dict_expect_t init_ldouble_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(long double),
                                LDOUBLE_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (ldouble_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (ldouble_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_ldouble_dict(ldouble_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_ldouble_dict(ldouble_dict_t*     dict,
                                 const char*        key,
                                 long double           value,
                                 allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_ldouble_dict_n(ldouble_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   long double           value,
                                   allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_ldouble_dict(ldouble_dict_t* dict,
                              const char*    key,
                              long double*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_ldouble_dict_n(ldouble_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                long double*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_ldouble_dict(ldouble_dict_t* dict,
                                 const char*    key,
                                 long double       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_ldouble_dict_n(ldouble_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   long double       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_ldouble_dict_value(const ldouble_dict_t* dict,
                                    const char*          key,
                                    long double*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_ldouble_dict_value_n(const ldouble_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      long double*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const long double* get_ldouble_dict_ptr(const ldouble_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const long double*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const long double* get_ldouble_dict_ptr_n(const ldouble_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const long double*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_ldouble_dict_key(const ldouble_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_ldouble_dict_key_n(const ldouble_dict_t* dict,
                            const char*          key,
                            size_t               key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_ldouble_dict(ldouble_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
ldouble_dict_expect_t copy_ldouble_dict(const ldouble_dict_t* src,
                                      allocator_vtable_t   alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (ldouble_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (ldouble_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
ldouble_dict_expect_t merge_ldouble_dict(const ldouble_dict_t* a,
                                       const ldouble_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (ldouble_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (ldouble_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
typedef struct {
    ldouble_dict_iter_fn typed_fn;
    void*               user_data;
} _ldouble_iter_ctx_t;
 
static void _ldouble_iter_shim(dict_entry_t e, void* ctx) {
    const _ldouble_iter_ctx_t* c = (const _ldouble_iter_ctx_t*)ctx;
    long double value;
    memcpy(&value, e.value, sizeof(long double));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_ldouble_dict(const ldouble_dict_t* dict,
                                  ldouble_dict_iter_fn  fn,
                                  void*                user_data) {
    if (fn == NULL) return NULL_POINTER;
    _ldouble_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _ldouble_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t ldouble_dict_size(const ldouble_dict_t* dict) {
    return dict_size(dict);
}
 
size_t ldouble_dict_hash_size(const ldouble_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t ldouble_dict_alloc(const ldouble_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_ldouble_dict_empty(const ldouble_dict_t* dict) {
    return is_dict_empty(dict);
}
// -------------------------------------------------------------------------------- 

typedef struct {
    FILE*  stream;
    size_t col;
    bool   first;
} _ldouble_dict_print_ctx_t;

static void _print_ldouble_dict_entry(const char* key,
                                      size_t      key_len,
                                      long double value,
                                      void*       user_data) {
    _ldouble_dict_print_ctx_t* ctx = (_ldouble_dict_print_ctx_t*)user_data;
    char val_buf[64];
    int val_n = snprintf(val_buf, sizeof(val_buf), "%Lg", value);
    size_t value_len = (val_n > 0) ? (size_t)val_n : 0u;

    size_t token_len = key_len + value_len + 4u;
    if (!ctx->first) {
        token_len += 2u;
    }

    if (ctx->col + token_len > 70u) {
        if (!ctx->first) {
            fputs(",\n  ", ctx->stream);
            ctx->col = 2u;
        } else {
            fputc('\n', ctx->stream);
            fputs("  ", ctx->stream);
            ctx->col = 2u;
        }
    } else if (!ctx->first) {
        fputs(", ", ctx->stream);
        ctx->col += 2u;
    }

    fprintf(ctx->stream, "\"%.*s\": %s", (int)key_len, key, val_buf);
    ctx->col += key_len + value_len + 4u;
    ctx->first = false;
}

error_code_t print_ldouble_dict(const ldouble_dict_t* dict, FILE* stream) {
    if (dict == NULL || stream == NULL) return NULL_POINTER;

    _ldouble_dict_print_ctx_t ctx;
    ctx.stream = stream;
    ctx.col    = 2u;
    ctx.first  = true;

    fputs("{ ", stream);

    error_code_t err = foreach_ldouble_dict(dict, _print_ldouble_dict_entry, &ctx);
    if (err != NO_ERROR) return err;

    if (ctx.first) {
        fputs("}", stream);
        return NO_ERROR;
    }

    if (ctx.col + 2u > 70u) {
        fputc('\n', stream);
        fputs("  }", stream);
    } else {
        fputs(" }", stream);
    }

    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// ================================================================================
// ldouble_matrix_t — type-safe ldouble wrapper implementations
//
// Every function below is a thin delegation to the corresponding generic
// matrix_t function, fixing the dtype to ldouble_TYPE and converting between
// ldouble / ldouble* and the void* interface expected by c_matrix.h.
// ================================================================================
// ================================================================================

// ================================================================================
// Internal helper: rewrap matrix_expect_t as ldouble_matrix_expect_t
// ================================================================================

static inline ldouble_matrix_expect_t _wrap_matrix_expect(matrix_expect_t e) {
    if (e.has_value) {
        return (ldouble_matrix_expect_t){
            .has_value = true,
            .u.value   = (ldouble_matrix_t*)e.u.value
        };
    }
    return (ldouble_matrix_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

ldouble_matrix_expect_t init_ldouble_dense_matrix(size_t             rows,
                                                  size_t             cols,
                                                  allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_dense_matrix(rows, cols, LDOUBLE_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

ldouble_matrix_expect_t init_ldouble_coo_matrix(size_t             rows,
                                                size_t             cols,
                                                size_t             capacity,
                                                bool               growth,
                                                allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_coo_matrix(rows, cols, capacity, LDOUBLE_TYPE, growth, alloc_v)
    );
}

// --------------------------------------------------------------------------------

void return_ldouble_matrix(ldouble_matrix_t* mat) {
    return_matrix(mat);
}

// ================================================================================
// Element access
// ================================================================================

error_code_t get_ldouble_matrix(const ldouble_matrix_t* mat,
                                size_t                row,
                                size_t                col,
                                long double*                out) {
    if (mat == NULL || out == NULL) return NULL_POINTER;
    return get_matrix(mat, row, col, out);
}

// --------------------------------------------------------------------------------

error_code_t set_ldouble_matrix(ldouble_matrix_t* mat,
                                size_t          row,
                                size_t          col,
                                long double           value) {
    if (mat == NULL) return NULL_POINTER;
    return set_matrix(mat, row, col, &value);
}

// ================================================================================
// COO assembly helpers
// ================================================================================

error_code_t reserve_ldouble_coo_matrix(ldouble_matrix_t* mat,
                                        size_t          capacity) {
    return reserve_coo_matrix(mat, capacity);
}

// --------------------------------------------------------------------------------

error_code_t push_back_ldouble_coo_matrix(ldouble_matrix_t* mat,
                                          size_t          row,
                                          size_t          col,
                                          long double           value) {
    if (mat == NULL) return NULL_POINTER;
    return push_back_coo_matrix(mat, row, col, &value);
}

// --------------------------------------------------------------------------------

error_code_t sort_ldouble_coo_matrix(ldouble_matrix_t* mat) {
    return sort_coo_matrix(mat);
}

// ================================================================================
// Lifecycle / structural operations
// ================================================================================

error_code_t clear_ldouble_matrix(ldouble_matrix_t* mat) {
    return clear_matrix(mat);
}

// --------------------------------------------------------------------------------

ldouble_matrix_expect_t copy_ldouble_matrix(const ldouble_matrix_t* src,
                                            allocator_vtable_t    alloc_v) {
    return _wrap_matrix_expect(copy_matrix(src, alloc_v));
}

// --------------------------------------------------------------------------------

static inline size_t scatter_csr_row(const long double* row_data,
                                     size_t       cols,
                                     size_t       col_offset,
                                     size_t*      col_idx,
                                     long double*       values,
                                     size_t       k) {
    for (size_t j = 0u; j < cols; ++j) {
        if (row_data[j] != 0.0f) {
            col_idx[k] = col_offset + j;
            values[k]  = row_data[j];
            ++k;
        }
    }
    return k;
}

ldouble_matrix_expect_t convert_ldouble_matrix(const ldouble_matrix_t* src,
                                               matrix_format_t       target,
                                               allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (ldouble_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /* SIMD fast path: dense ldouble -> CSR */
    if (src->format == DENSE_MATRIX && target == CSR_MATRIX) {
        const long double* data = NULL;
        size_t total = 0u;
        size_t nnz = 0u;
        size_t row_ptr_bytes = 0u;
        size_t col_idx_bytes = 0u;
        size_t values_bytes = 0u;
        size_t* row_ptr = NULL;
        size_t* col_idx = NULL;
        long double* values = NULL;
        ldouble_matrix_t* dst = NULL;
        void_ptr_expect_t mr;
        void_ptr_expect_t rp_r;
        void_ptr_expect_t ci_r;
        void_ptr_expect_t vl_r;

        /* Validate allocator interface required by this fast path */
        if (alloc_v.allocate == NULL || alloc_v.return_element == NULL) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = NULL_POINTER
            };
        }

        /* Defensive validation of dense backing storage */
        if ((src->rows > 0u) && (src->cols > 0u) && (src->rep.dense.data == NULL)) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = ILLEGAL_STATE
            };
        }

        /* Overflow check for rows * cols */
        if ((src->rows != 0u) && (src->cols > (SIZE_MAX / src->rows))) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        total = src->rows * src->cols;
        data = (const long double*)src->rep.dense.data;

        /* Pass 1: count nonzeros (SIMD-accelerated) */
        nnz = 0u;
        for (size_t i = 0u; i < total; ++i) {
            if (data[i] != 0.0L) ++nnz;
        }

        /* Overflow checks for byte counts */
        if ((src->rows + 1u) < src->rows) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if ((src->rows + 1u) > (SIZE_MAX / sizeof(size_t))) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(size_t))) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(long double))) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }

        /* Allocate the matrix header */
        mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = BAD_ALLOC
            };
        }

        dst = (ldouble_matrix_t*)mr.u.value;
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = LDOUBLE_TYPE;
        dst->data_size = sizeof(long double);
        dst->format    = CSR_MATRIX;
        dst->alloc_v   = alloc_v;

        dst->rep.csr.nnz     = 0u;
        dst->rep.csr.row_ptr = NULL;
        dst->rep.csr.col_idx = NULL;
        dst->rep.csr.values  = NULL;

        row_ptr_bytes = (src->rows + 1u) * sizeof(size_t);
        col_idx_bytes = nnz * sizeof(size_t);
        values_bytes  = nnz * sizeof(long double);

        /*
         * For nnz == 0, allocate a minimum of 1 byte for col_idx/values so the
         * allocator is never asked for a zero-size block. The logical nnz stays 0.
         */
        rp_r = alloc_v.allocate(alloc_v.ctx, row_ptr_bytes, true);
        ci_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? col_idx_bytes : 1u, true);
        vl_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? values_bytes  : 1u, true);

        if (!rp_r.has_value || !ci_r.has_value || !vl_r.has_value) {
            if (rp_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, rp_r.u.value);
            }
            if (ci_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, ci_r.u.value);
            }
            if (vl_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, vl_r.u.value);
            }
            alloc_v.return_element(alloc_v.ctx, dst);

            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = OUT_OF_MEMORY
            };
        }

        row_ptr = (size_t*)rp_r.u.value;
        col_idx = (size_t*)ci_r.u.value;
        values  = (long double*)vl_r.u.value;

        /* Pass 2: scatter nonzeros into CSR arrays */
        row_ptr[0] = 0u;
        if (nnz > 0u) {
            size_t k = 0u;

            for (size_t i = 0u; i < src->rows; ++i) {
                const long double* row_data = data + (i * src->cols);
                k = scatter_csr_row(
                    row_data, src->cols, 0u, col_idx, values, k
                );
                row_ptr[i + 1u] = k;
            }

            dst->rep.csr.nnz = k;
        } else {
            for (size_t i = 0u; i < src->rows; ++i) {
                row_ptr[i + 1u] = 0u;
            }
            dst->rep.csr.nnz = 0u;
        }

        dst->rep.csr.row_ptr = row_ptr;
        dst->rep.csr.col_idx = col_idx;
        dst->rep.csr.values  = (uint8_t*)values;

        return (ldouble_matrix_expect_t){
            .has_value = true,
            .u.value   = dst
        };
    }

    /* All other conversions: delegate to generic path */
    return _wrap_matrix_expect(convert_matrix(src, target, alloc_v));
}

// --------------------------------------------------------------------------------

ldouble_matrix_expect_t transpose_ldouble_matrix(const ldouble_matrix_t* src,
                                                 allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (ldouble_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }
    return _wrap_matrix_expect(transpose_matrix(src, alloc_v)); 
}
// ================================================================================
// Fill and zero
// ================================================================================

error_code_t fill_ldouble_matrix(ldouble_matrix_t* mat,
                                 long double           value) {
    if (mat == NULL) return NULL_POINTER;
 
    /* Zero fill: delegate to clear_matrix for all formats */
    if (value == 0.0L) return clear_matrix(mat);
    return fill_matrix(mat, &value); 
}
// ================================================================================
// Shape and compatibility queries
// ================================================================================

bool ldouble_matrix_has_same_shape(const ldouble_matrix_t* a,
                                   const ldouble_matrix_t* b) {
    return matrix_has_same_shape(a, b);
}

// --------------------------------------------------------------------------------

bool ldouble_matrix_is_square(const ldouble_matrix_t* mat) {
    return matrix_is_square(mat);
}

// --------------------------------------------------------------------------------

bool ldouble_matrix_is_sparse(const ldouble_matrix_t* mat) {
    return matrix_is_sparse(mat);
}

// --------------------------------------------------------------------------------

bool ldouble_matrix_is_zero(const ldouble_matrix_t* mat) {
    if (mat == NULL) return true;
    return is_zero_matrix(mat);
}

// --------------------------------------------------------------------------------

static inline bool arrays_equal(const long double* a,
                                const long double* b,
                                size_t       count) {
    if (a == NULL || b == NULL) return false;

    for (size_t i = 0u; i < count; ++i) {
        if (!(a[i] == b[i])) {
            return false;
        }
    }
    return true;
}

bool ldouble_matrix_equal(const ldouble_matrix_t* a,
                          const ldouble_matrix_t* b) {
    if (a == NULL || b == NULL) return false;
    if (!matrix_has_same_shape((const matrix_t*)a, (const matrix_t*)b)) return false;
    if (a->dtype != LDOUBLE_TYPE || b->dtype != LDOUBLE_TYPE) return false;

    /* Fast path: both dense */
    if (a->format == DENSE_MATRIX && b->format == DENSE_MATRIX) {
        const long double* da = (const long double*)a->rep.dense.data;
        const long double* db = (const long double*)b->rep.dense.data;
        return arrays_equal(da, db, a->rows * a->cols);
    }

    /* Fast path: both COO and sorted */
    if (a->format == COO_MATRIX && b->format == COO_MATRIX &&
        a->rep.coo.sorted && b->rep.coo.sorted) {

        size_t nnz = a->rep.coo.nnz;
        if (nnz != b->rep.coo.nnz) return false;

        if (nnz > 0u) {
            if (memcmp(a->rep.coo.row_idx, b->rep.coo.row_idx,
                       nnz * sizeof(size_t)) != 0) {
                return false;
            }

            if (memcmp(a->rep.coo.col_idx, b->rep.coo.col_idx,
                       nnz * sizeof(size_t)) != 0) {
                return false;
            }

            if (!arrays_equal(
                    (const long double*)a->rep.coo.values,
                    (const long double*)b->rep.coo.values,
                    nnz)) {
                return false;
            }
        }

        return true;
    }

    /* General logical comparison */
    for (size_t i = 0u; i < a->rows; ++i) {
        for (size_t j = 0u; j < a->cols; ++j) {
            long double va = 0.0L;
            long double vb = 0.0L;

            if (get_matrix((const matrix_t*)a, i, j, &va) != NO_ERROR) return false;
            if (get_matrix((const matrix_t*)b, i, j, &vb) != NO_ERROR) return false;

            if (!(va == vb)) return false;
        }
    }

    return true;
}
// --------------------------------------------------------------------------------

bool ldouble_matrix_is_add_compatible(const ldouble_matrix_t* a,
                                      const ldouble_matrix_t* b) {
    return matrix_is_add_compatible(a, b);
}

// --------------------------------------------------------------------------------

bool ldouble_matrix_is_multiply_compatible(const ldouble_matrix_t* a,
                                           const ldouble_matrix_t* b) {
    return matrix_is_multiply_compatible(a, b);
}

// ================================================================================
// Row / column swaps
// ================================================================================

error_code_t swap_ldouble_matrix_rows(ldouble_matrix_t* mat,
                                      size_t          r1,
                                      size_t          r2) {
    return swap_matrix_rows(mat, r1, r2);
}

// --------------------------------------------------------------------------------

error_code_t swap_ldouble_matrix_cols(ldouble_matrix_t* mat,
                                      size_t          c1,
                                      size_t          c2) {
    return swap_matrix_cols(mat, c1, c2);
}

// ================================================================================
// Special matrix constructors
// ================================================================================

ldouble_matrix_expect_t init_ldouble_identity_matrix(size_t             n,
                                                     allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_identity_matrix(n, LDOUBLE_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

ldouble_matrix_expect_t init_ldouble_row_vector(size_t             length,
                                                allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_row_vector(length, LDOUBLE_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

ldouble_matrix_expect_t init_ldouble_col_vector(size_t             length,
                                                allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_col_vector(length, LDOUBLE_TYPE, alloc_v)
    );
}

// ================================================================================
// Vector shape queries
// ================================================================================

bool ldouble_matrix_is_row_vector(const ldouble_matrix_t* mat) {
    return matrix_is_row_vector(mat);
}

// --------------------------------------------------------------------------------

bool ldouble_matrix_is_col_vector(const ldouble_matrix_t* mat) {
    return matrix_is_col_vector(mat);
}

// --------------------------------------------------------------------------------

bool ldouble_matrix_is_vector(const ldouble_matrix_t* mat) {
    return matrix_is_vector(mat);
}

// --------------------------------------------------------------------------------

size_t ldouble_matrix_vector_length(const ldouble_matrix_t* mat) {
    return matrix_vector_length(mat);
}
// ================================================================================
// convert_ldouble_matrix_zero
// ================================================================================

// ================================================================================
// Internal helper
// ================================================================================

static inline bool _ldouble_value_is_zero(long double value, ldouble_zero_fn is_zero) {
    if (is_zero != NULL) {
        return is_zero(value);
    }
    return (value == 0.0L);
}

// ================================================================================
// Semantic-zero conversion for ldouble matrices
// ================================================================================

ldouble_matrix_expect_t convert_ldouble_matrix_zero(const ldouble_matrix_t* src,
                                                    matrix_format_t       target,
                                                    allocator_vtable_t    alloc_v,
                                                    ldouble_zero_fn         is_zero) {
    if (src == NULL) {
        return (ldouble_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /*
     * The semantic-zero callback only matters for dense -> sparse conversion.
     * For all other cases, delegate to the normal ldouble conversion path.
     */
    if (src->format != DENSE_MATRIX ||
        (target != COO_MATRIX && target != CSR_MATRIX && target != CSC_MATRIX)) {
        return convert_ldouble_matrix(src, target, alloc_v);
    }

    /* Validate allocator interface needed by the direct allocation path */
    if (alloc_v.allocate == NULL || alloc_v.return_element == NULL) {
        return (ldouble_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /* Defensive validation of dense backing storage */
    if ((src->rows > 0u) && (src->cols > 0u) && (src->rep.dense.data == NULL)) {
        return (ldouble_matrix_expect_t){
            .has_value = false,
            .u.error   = ILLEGAL_STATE
        };
    }

    /* Overflow check for rows * cols */
    if ((src->rows != 0u) && (src->cols > (SIZE_MAX / src->rows))) {
        return (ldouble_matrix_expect_t){
            .has_value = false,
            .u.error   = LENGTH_OVERFLOW
        };
    }

    const long double* data  = (const long double*)src->rep.dense.data;
    size_t total       = src->rows * src->cols;
    size_t nnz         = 0u;

    /* Pass 1: count semantic nonzeros */
    for (size_t i = 0u; i < total; ++i) {
        if (!_ldouble_value_is_zero(data[i], is_zero)) {
            ++nnz;
        }
    }

    // ----------------------------------------------------------------------------
    // dense -> COO
    // ----------------------------------------------------------------------------
    if (target == COO_MATRIX) {
        ldouble_matrix_expect_t r = init_ldouble_coo_matrix(
            src->rows, src->cols, (nnz > 0u) ? nnz : 1u, false, alloc_v
        );
        if (!r.has_value) {
            return r;
        }

        ldouble_matrix_t* dst = r.u.value;

        for (size_t row = 0u; row < src->rows; ++row) {
            for (size_t col = 0u; col < src->cols; ++col) {
                long double value = data[row * src->cols + col];
                if (!_ldouble_value_is_zero(value, is_zero)) {
                    error_code_t ec = push_back_ldouble_coo_matrix(dst, row, col, value);
                    if (ec != NO_ERROR) {
                        return_ldouble_matrix(dst);
                        return (ldouble_matrix_expect_t){
                            .has_value = false,
                            .u.error   = ec
                        };
                    }
                }
            }
        }

        /*
         * Row-major traversal naturally inserts in sorted order, but make the
         * intent explicit and robust.
         */
        (void)sort_ldouble_coo_matrix(dst);
        return r;
    }

    // ----------------------------------------------------------------------------
    // dense -> CSR
    // ----------------------------------------------------------------------------
    if (target == CSR_MATRIX) {
        size_t row_ptr_bytes = 0u;
        size_t col_idx_bytes = 0u;
        size_t values_bytes  = 0u;

        void_ptr_expect_t mr;
        void_ptr_expect_t rp_r;
        void_ptr_expect_t ci_r;
        void_ptr_expect_t vl_r;

        size_t* row_ptr = NULL;
        size_t* col_idx = NULL;
        long double*  values  = NULL;
        ldouble_matrix_t* dst = NULL;

        if ((src->rows + 1u) < src->rows) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if ((src->rows + 1u) > (SIZE_MAX / sizeof(size_t))) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(size_t)) ||
            nnz > (SIZE_MAX / sizeof(long double))) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }

        row_ptr_bytes = (src->rows + 1u) * sizeof(size_t);
        col_idx_bytes = nnz * sizeof(size_t);
        values_bytes  = nnz * sizeof(long double);

        mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = BAD_ALLOC
            };
        }

        dst = (ldouble_matrix_t*)mr.u.value;
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = LDOUBLE_TYPE;
        dst->data_size = sizeof(long double);
        dst->format    = CSR_MATRIX;
        dst->alloc_v   = alloc_v;

        dst->rep.csr.nnz     = 0u;
        dst->rep.csr.row_ptr = NULL;
        dst->rep.csr.col_idx = NULL;
        dst->rep.csr.values  = NULL;

        rp_r = alloc_v.allocate(alloc_v.ctx, row_ptr_bytes, true);
        ci_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? col_idx_bytes : 1u, true);
        vl_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? values_bytes  : 1u, true);

        if (!rp_r.has_value || !ci_r.has_value || !vl_r.has_value) {
            if (rp_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, rp_r.u.value);
            }
            if (ci_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, ci_r.u.value);
            }
            if (vl_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, vl_r.u.value);
            }
            alloc_v.return_element(alloc_v.ctx, dst);

            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = OUT_OF_MEMORY
            };
        }

        row_ptr = (size_t*)rp_r.u.value;
        col_idx = (size_t*)ci_r.u.value;
        values  = (long double*)vl_r.u.value;

        row_ptr[0] = 0u;

        size_t k = 0u;
        for (size_t row = 0u; row < src->rows; ++row) {
            for (size_t col = 0u; col < src->cols; ++col) {
                long double value = data[row * src->cols + col];
                if (!_ldouble_value_is_zero(value, is_zero)) {
                    col_idx[k] = col;
                    values[k]  = value;
                    ++k;
                }
            }
            row_ptr[row + 1u] = k;
        }

        dst->rep.csr.nnz     = k;
        dst->rep.csr.row_ptr = row_ptr;
        dst->rep.csr.col_idx = col_idx;
        dst->rep.csr.values  = (uint8_t*)values;

        return (ldouble_matrix_expect_t){
            .has_value = true,
            .u.value   = dst
        };
    }

    // ----------------------------------------------------------------------------
    // dense -> CSC
    // ----------------------------------------------------------------------------
    if (target == CSC_MATRIX) {
        size_t col_ptr_bytes = 0u;
        size_t row_idx_bytes = 0u;
        size_t values_bytes  = 0u;

        void_ptr_expect_t mr;
        void_ptr_expect_t cp_r;
        void_ptr_expect_t ri_r;
        void_ptr_expect_t vl_r;

        size_t* col_ptr = NULL;
        size_t* row_idx = NULL;
        long double*  values  = NULL;
        ldouble_matrix_t* dst = NULL;

        if ((src->cols + 1u) < src->cols) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if ((src->cols + 1u) > (SIZE_MAX / sizeof(size_t))) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(size_t)) ||
            nnz > (SIZE_MAX / sizeof(long double))) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }

        col_ptr_bytes = (src->cols + 1u) * sizeof(size_t);
        row_idx_bytes = nnz * sizeof(size_t);
        values_bytes  = nnz * sizeof(long double);

        mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = BAD_ALLOC
            };
        }

        dst = (ldouble_matrix_t*)mr.u.value;
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = LDOUBLE_TYPE;
        dst->data_size = sizeof(long double);
        dst->format    = CSC_MATRIX;
        dst->alloc_v   = alloc_v;

        dst->rep.csc.nnz     = 0u;
        dst->rep.csc.col_ptr = NULL;
        dst->rep.csc.row_idx = NULL;
        dst->rep.csc.values  = NULL;

        cp_r = alloc_v.allocate(alloc_v.ctx, col_ptr_bytes, true);
        ri_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? row_idx_bytes : 1u, true);
        vl_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? values_bytes  : 1u, true);

        if (!cp_r.has_value || !ri_r.has_value || !vl_r.has_value) {
            if (cp_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, cp_r.u.value);
            }
            if (ri_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, ri_r.u.value);
            }
            if (vl_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, vl_r.u.value);
            }
            alloc_v.return_element(alloc_v.ctx, dst);

            return (ldouble_matrix_expect_t){
                .has_value = false,
                .u.error   = OUT_OF_MEMORY
            };
        }

        col_ptr = (size_t*)cp_r.u.value;
        row_idx = (size_t*)ri_r.u.value;
        values  = (long double*)vl_r.u.value;

        col_ptr[0] = 0u;

        size_t k = 0u;
        for (size_t col = 0u; col < src->cols; ++col) {
            for (size_t row = 0u; row < src->rows; ++row) {
                float value = data[row * src->cols + col];
                if (!_ldouble_value_is_zero(value, is_zero)) {
                    row_idx[k] = row;
                    values[k]  = value;
                    ++k;
                }
            }
            col_ptr[col + 1u] = k;
        }

        dst->rep.csc.nnz     = k;
        dst->rep.csc.col_ptr = col_ptr;
        dst->rep.csc.row_idx = row_idx;
        dst->rep.csc.values  = (uint8_t*)values;

        return (ldouble_matrix_expect_t){
            .has_value = true,
            .u.value   = dst
        };
    }

    return (ldouble_matrix_expect_t){
        .has_value = false,
        .u.error   = ILLEGAL_STATE
    };
}
// -------------------------------------------------------------------------------- 

static error_code_t _print_ldouble_dense_matrix(const ldouble_matrix_t* mat,
                                                FILE*                   stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;
    if (mat->format != DENSE_MATRIX)   return ILLEGAL_STATE;

    const long double* data = (const long double*)mat->rep.dense.data;

    if (fputs("[ ", stream) == EOF) return ILLEGAL_STATE;

    for (size_t i = 0u; i < mat->rows; ++i) {
        if (i == 0u) {
            if (fputs("[ ", stream) == EOF) return ILLEGAL_STATE;
        } else {
            if (fputs("\n  [ ", stream) == EOF) return ILLEGAL_STATE;
        }

        for (size_t j = 0u; j < mat->cols; ++j) {
            size_t idx = (i * mat->cols) + j;

            if (fprintf(stream, "%Lg", data[idx]) < 0) {
                return ILLEGAL_STATE;
            }

            if (j + 1u < mat->cols) {
                if (fputs(", ", stream) == EOF) return ILLEGAL_STATE;
            }
        }

        if (fputs(" ]", stream) == EOF) return ILLEGAL_STATE;

        if (i + 1u < mat->rows) {
            if (fputc(',', stream) == EOF) return ILLEGAL_STATE;
        }
    }

    if (fputs(" ]", stream) == EOF) return ILLEGAL_STATE;
    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

static error_code_t _print_ldouble_sparse_row_major(const ldouble_matrix_t* mat,
                                                    FILE*                   stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;

    size_t col = 0u;
    bool first = true;

    if (fputs("[ ", stream) == EOF) return ILLEGAL_STATE;
    col = 2u;

    for (size_t i = 0u; i < mat->rows; ++i) {
        for (size_t j = 0u; j < mat->cols; ++j) {
            long double value = 0.0L;

            error_code_t err = get_ldouble_matrix(mat, i, j, &value);
            if (err != NO_ERROR) return err;

            if (value == 0.0L) continue;

            char token[160];
            int n = snprintf(token, sizeof(token),
                             "(%zu, %zu): %Lg", i, j, value);
            size_t token_len = (n > 0) ? (size_t)n : 0u;

            if (!first) {
                if ((col + 2u + token_len) > 70u) {
                    if (fputs(",\n  ", stream) == EOF) return ILLEGAL_STATE;
                    col = 2u;
                } else {
                    if (fputs(", ", stream) == EOF) return ILLEGAL_STATE;
                    col += 2u;
                }
            } else if ((col + token_len) > 70u) {
                if (fputc('\n', stream) == EOF) return ILLEGAL_STATE;
                if (fputs("  ", stream) == EOF) return ILLEGAL_STATE;
                col = 2u;
            }

            if (fputs(token, stream) == EOF) return ILLEGAL_STATE;
            col += token_len;
            first = false;
        }
    }

    if (first) {
        if (fputs("]", stream) == EOF) return ILLEGAL_STATE;
        return NO_ERROR;
    }

    if ((col + 2u) > 70u) {
        if (fputc('\n', stream) == EOF) return ILLEGAL_STATE;
        if (fputs("  ]", stream) == EOF) return ILLEGAL_STATE;
    } else {
        if (fputs(" ]", stream) == EOF) return ILLEGAL_STATE;
    }

    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

static error_code_t _print_ldouble_coo_matrix(const ldouble_matrix_t* mat,
                                              FILE*                   stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;
    if (mat->format != COO_MATRIX)     return ILLEGAL_STATE;
    return _print_ldouble_sparse_row_major(mat, stream);
}

// -----------------------------------------------------------------------------

static error_code_t _print_ldouble_csr_matrix(const ldouble_matrix_t* mat,
                                              FILE*                   stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;
    if (mat->format != CSR_MATRIX)     return ILLEGAL_STATE;
    return _print_ldouble_sparse_row_major(mat, stream);
}

// -----------------------------------------------------------------------------

static error_code_t _print_ldouble_csc_matrix(const ldouble_matrix_t* mat,
                                              FILE*                   stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;
    if (mat->format != CSC_MATRIX)     return ILLEGAL_STATE;
    return _print_ldouble_sparse_row_major(mat, stream);
}
// -------------------------------------------------------------------------------- 

error_code_t print_ldouble_matrix(const ldouble_matrix_t* mat, FILE* stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;

    switch (mat->format) {
        case DENSE_MATRIX:
            return _print_ldouble_dense_matrix(mat, stream);

        case COO_MATRIX:
            return _print_ldouble_coo_matrix(mat, stream);

        case CSR_MATRIX:
            return _print_ldouble_csr_matrix(mat, stream);

        case CSC_MATRIX:
            return _print_ldouble_csc_matrix(mat, stream);

        default:
            return ILLEGAL_STATE;
    }
}
// -------------------------------------------------------------------------------- 

ldouble_expect_t ldouble_matrix_min(const ldouble_matrix_t* mat) {
    if (mat == NULL) {
        return (ldouble_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    size_expect_t idx = matrix_min(mat, _cmp_ldouble, LDOUBLE_TYPE);
    if (!idx.has_value) {
        return (ldouble_expect_t){ .has_value = false, .u.error = idx.u.error };
    }

    switch (mat->format) {
        case DENSE_MATRIX: {
            const long double* vals = (const long double*)mat->rep.dense.data;
            long double val = vals[idx.u.value];
            return (ldouble_expect_t){ .has_value = true, .u.value = val };
        }

        case COO_MATRIX: {
            const long double* vals = (const long double*)mat->rep.coo.values;
            long double val = vals[idx.u.value];
            return (ldouble_expect_t){ .has_value = true, .u.value = val };
        }

        case CSR_MATRIX: {
            const long double* vals = (const long double*)mat->rep.csr.values;
            long double val = vals[idx.u.value];
            return (ldouble_expect_t){ .has_value = true, .u.value = val };
        }

        case CSC_MATRIX: {
            const long double* vals = (const long double*)mat->rep.csc.values;
            long double val = vals[idx.u.value];
            return (ldouble_expect_t){ .has_value = true, .u.value = val };
        }

        default:
            return (ldouble_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }
}
// ================================================================================
// ================================================================================
// eof
