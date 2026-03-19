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
 
// ================================================================================
// ================================================================================
// eof
