// ================================================================================
// ================================================================================
// - File:    array.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 21, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <string.h>
#include "c_array.h"

#if defined(__AVX512BW__)
#  include "simd_avx512_uint8.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_uint8.inl"
#elif defined(__AVX__)
#  include "simd_avx_uint8.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_uint8.inl"
#elif defined(__SSSE3__)
#  include "simd_sse3_uint8.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_uint8.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_uint8.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_uint8.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_uint8.inl"
#else
#  include "simd_scalar_uint8.inl"
#endif
// ================================================================================ 
// ================================================================================ 
// Internal growth strategy
//
// Tiered to avoid runaway allocation at large sizes while maintaining fast
// ramp-up at small sizes:
//   0 elements       -> 1
//   < 1024 elements  -> 2x
//   < 8192 elements  -> 1.5x  (alloc + alloc/2)
//   < 65536 elements -> 1.25x (alloc + alloc/4)
//   >= 65536 elements -> linear increment of 256
// ================================================================================

static size_t _compute_new_alloc(size_t current) {
    if (current == 0u)    return 1u;
    if (current < 1024u)  return current * 2u;
    if (current < 8192u)  return current + current / 2u;
    if (current < 65536u) return current + current / 4u;
    return current + 256u;
}

// ================================================================================
// Internal grow helper
//
// Reallocates the data buffer to new_alloc elements using the cached data_size.
// Returns NO_ERROR on success. On failure the array is left untouched.
// ================================================================================

static error_code_t _grow_array(array_t* array, size_t new_alloc) {
    /* Overflow guard: new_alloc * data_size must not wrap */
    if (new_alloc > SIZE_MAX / array->data_size) return LENGTH_OVERFLOW;

    size_t old_bytes = array->alloc * array->data_size;
    size_t new_bytes = new_alloc    * array->data_size;

    void_ptr_expect_t result = array->alloc_v.reallocate(
        array->alloc_v.ctx,
        array->data,
        old_bytes,
        new_bytes,
        false
    );

    if (result.has_value == false) return OUT_OF_MEMORY;

    array->data  = (uint8_t*)result.u.value;
    array->alloc = new_alloc;
    return NO_ERROR;
}
// ================================================================================
// Internal capacity helper
//
// Ensures at least one free slot exists, growing if permitted.
// Returns NO_ERROR when there is room to write, or an error code otherwise.
// ================================================================================

static error_code_t _ensure_capacity(array_t* array) {
    if (array->len < array->alloc) return NO_ERROR;

    if (array->growth == false)              return CAPACITY_OVERFLOW;
    if (array->alloc_v.reallocate == NULL)   return CAPACITY_OVERFLOW;

    size_t new_alloc = _compute_new_alloc(array->alloc);
    return _grow_array(array, new_alloc);
}

// ================================================================================
// Initialization and teardown
// ================================================================================

array_expect_t init_array(size_t             capacity,
                          dtype_id_t         dtype,
                          bool               growth,
                          allocator_vtable_t alloc_v) {
    if (alloc_v.allocate == NULL) {
        return (array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }
    if (capacity == 0u) {
        return (array_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }
    if (dtype == UNKNOWN_TYPE) {
        return (array_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }
    if (init_dtype_registry() == false) {
        return (array_expect_t){ .has_value = false, .u.error = ILLEGAL_STATE };
    }

    const dtype_t* desc = lookup_dtype(dtype);
    if (desc == NULL) {
        return (array_expect_t){ .has_value = false, .u.error = TYPE_MISMATCH };
    }

    size_t data_size = desc->data_size;
    if (capacity > SIZE_MAX / data_size) {
        return (array_expect_t){ .has_value = false, .u.error = LENGTH_OVERFLOW };
    }

    /* Allocate the array_t struct */
    void_ptr_expect_t struct_result = alloc_v.allocate(
        alloc_v.ctx, sizeof(array_t), true
    );
    if (struct_result.has_value == false) {
        return (array_expect_t){ .has_value = false, .u.error = BAD_ALLOC };
    }

    array_t* array = (array_t*)struct_result.u.value;

    /* Allocate the data buffer */
    void_ptr_expect_t data_result = alloc_v.allocate(
        alloc_v.ctx, capacity * data_size, false
    );
    if (data_result.has_value == false) {
        alloc_v.return_element(alloc_v.ctx, array);
        return (array_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    array->data      = (uint8_t*)data_result.u.value;
    array->len       = 0u;
    array->alloc     = capacity;
    array->data_size = data_size;
    array->dtype     = dtype;
    array->growth    = growth;
    array->alloc_v   = alloc_v;

    return (array_expect_t){ .has_value = true, .u.value = array };
}

// --------------------------------------------------------------------------------

void return_array(array_t* array) {
    if (array == NULL) return;

    if (array->data != NULL) {
        array->alloc_v.return_element(array->alloc_v.ctx, array->data);
        array->data = NULL;
    }

    array->alloc_v.return_element(array->alloc_v.ctx, array);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_array(array_t*    array,
                             const void* data,
                             dtype_id_t  dtype) {
    if (array == NULL || data == NULL) return NULL_POINTER;
    if (dtype != array->dtype)         return TYPE_MISMATCH;

    error_code_t err = _ensure_capacity(array);
    if (err != NO_ERROR) return err;

    memcpy(array->data + array->len * array->data_size, data, array->data_size);
    array->len++;
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t push_front_array(array_t*    array,
                              const void* data,
                              dtype_id_t  dtype) {
    if (array == NULL || data == NULL) return NULL_POINTER;
    if (dtype != array->dtype)         return TYPE_MISMATCH;

    /* Grow before shifting so the buffer pointer is stable during memmove */
    error_code_t err = _ensure_capacity(array);
    if (err != NO_ERROR) return err;

    if (array->len > 0u) {
        memmove(array->data + array->data_size,
                array->data,
                array->len * array->data_size);
    }

    memcpy(array->data, data, array->data_size);
    array->len++;
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t push_at_array(array_t*    array,
                           const void* data,
                           size_t      index,
                           dtype_id_t  dtype) {
    if (array == NULL || data == NULL) return NULL_POINTER;
    if (dtype != array->dtype)         return TYPE_MISMATCH;
    if (index > array->len)            return OUT_OF_BOUNDS;

    if (index == 0u)         return push_front_array(array, data, dtype);
    if (index == array->len) return push_back_array(array, data, dtype);

    /* Grow before shifting so the buffer pointer is stable during memmove */
    error_code_t err = _ensure_capacity(array);
    if (err != NO_ERROR) return err;

    memmove(array->data + (index + 1u) * array->data_size,
            array->data + index * array->data_size,
            (array->len - index) * array->data_size);

    memcpy(array->data + index * array->data_size, data, array->data_size);
    array->len++;
    return NO_ERROR;
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_array_index(const array_t* array,
                             size_t         index,
                             void*          out,
                             dtype_id_t     dtype) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    if (dtype != array->dtype)        return TYPE_MISMATCH;
    if (index >= array->len)          return OUT_OF_BOUNDS;

    memcpy(out, array->data + index * array->data_size, array->data_size);
    return NO_ERROR;
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_array(array_t*   array,
                            void*      out,
                            dtype_id_t dtype) {
    if (array == NULL)         return NULL_POINTER;
    if (dtype != array->dtype) return TYPE_MISMATCH;
    if (array->len == 0u)      return EMPTY;

    array->len--;

    if (out != NULL) {
        memcpy(out, array->data + array->len * array->data_size, array->data_size);
    }

    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t pop_front_array(array_t*   array,
                             void*      out,
                             dtype_id_t dtype) {
    if (array == NULL)         return NULL_POINTER;
    if (dtype != array->dtype) return TYPE_MISMATCH;
    if (array->len == 0u)      return EMPTY;

    if (out != NULL) {
        memcpy(out, array->data, array->data_size);
    }

    array->len--;

    /* Shift all remaining elements one slot toward the front */
    if (array->len > 0u) {
        memmove(array->data,
                array->data + array->data_size,
                array->len * array->data_size);
    }

    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t pop_any_array(array_t*   array,
                           void*      out,
                           size_t     index,
                           dtype_id_t dtype) {
    if (array == NULL)         return NULL_POINTER;
    if (dtype != array->dtype) return TYPE_MISMATCH;
    if (array->len == 0u)      return EMPTY;
    if (index >= array->len)   return OUT_OF_BOUNDS;

    if (index == 0u)              return pop_front_array(array, out, dtype);
    if (index == array->len - 1u) return pop_back_array(array, out, dtype);

    if (out != NULL) {
        memcpy(out, array->data + index * array->data_size, array->data_size);
    }

    array->len--;

    /* Shift elements after index one slot toward the front */
    memmove(array->data + index * array->data_size,
            array->data + (index + 1u) * array->data_size,
            (array->len - index) * array->data_size);

    return NO_ERROR;
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_array(array_t* array) {
    if (array == NULL) return NULL_POINTER;

    memset(array->data, 0, array->len * array->data_size);
    array->len = 0u;
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t set_array_index(array_t*    array,
                             size_t      index,
                             const void* data,
                             dtype_id_t  dtype) {
    if (array == NULL || data == NULL) return NULL_POINTER;
    if (dtype != array->dtype)         return TYPE_MISMATCH;
    if (index >= array->len)           return OUT_OF_BOUNDS;

    memcpy(array->data + index * array->data_size, data, array->data_size);
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

array_expect_t copy_array(const array_t*     src,
                          allocator_vtable_t alloc_v) {
    if (src == NULL || alloc_v.allocate == NULL) {
        return (array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    /* Allocate the new array_t struct */
    void_ptr_expect_t struct_result = alloc_v.allocate(
        alloc_v.ctx, sizeof(array_t), true
    );
    if (struct_result.has_value == false) {
        return (array_expect_t){ .has_value = false, .u.error = BAD_ALLOC };
    }

    array_t* dst = (array_t*)struct_result.u.value;

    /* Allocate the new data buffer sized to src->alloc so capacity is preserved */
    void_ptr_expect_t data_result = alloc_v.allocate(
        alloc_v.ctx, src->alloc * src->data_size, false
    );
    if (data_result.has_value == false) {
        alloc_v.return_element(alloc_v.ctx, dst);
        return (array_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    dst->data      = (uint8_t*)data_result.u.value;
    dst->len       = src->len;
    dst->alloc     = src->alloc;
    dst->data_size = src->data_size;
    dst->dtype     = src->dtype;
    dst->growth    = src->growth;
    dst->alloc_v   = alloc_v;

    /* Copy only the live elements */
    memcpy(dst->data, src->data, src->len * src->data_size);

    return (array_expect_t){ .has_value = true, .u.value = dst };
}

// ================================================================================
// Type query and concatenation
// ================================================================================

dtype_id_t array_type(const array_t* array) {
    if (array == NULL) return UNKNOWN_TYPE;
    return array->dtype;
}

// --------------------------------------------------------------------------------

error_code_t concat_array(array_t*       dst,
                          const array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    if (dst->dtype != src->dtype)   return TYPE_MISMATCH;

    /* Nothing to do if source is empty */
    if (src->len == 0u) return NO_ERROR;

    /* Overflow guard: dst->len + src->len must not wrap */
    if (src->len > SIZE_MAX - dst->len) return LENGTH_OVERFLOW;

    size_t needed = dst->len + src->len;

    /* Grow dst once to cover all of src rather than element-by-element */
    if (needed > dst->alloc) {
        if (dst->growth == false)            return CAPACITY_OVERFLOW;
        if (dst->alloc_v.reallocate == NULL) return CAPACITY_OVERFLOW;

        /* Grow at least to needed, but apply the tiered strategy if it
           would produce a larger allocation to avoid thrashing later */
        size_t tiered = _compute_new_alloc(dst->alloc);
        size_t new_alloc = (tiered > needed) ? tiered : needed;

        error_code_t err = _grow_array(dst, new_alloc);
        if (err != NO_ERROR) return err;
    }

    memcpy(dst->data + dst->len * dst->data_size,
           src->data,
           src->len * src->data_size);

    dst->len = needed;
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

array_expect_t slice_array(const array_t*     src,
                           size_t             start,
                           size_t             end,
                           allocator_vtable_t alloc_v) {
    if (src == NULL || alloc_v.allocate == NULL) {
        return (array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }
    if (start >= end) {
        return (array_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }
    if (end > src->len) {
        return (array_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };
    }

    size_t slice_len = end - start;

    /* Allocate the new array_t struct */
    void_ptr_expect_t struct_result = alloc_v.allocate(
        alloc_v.ctx, sizeof(array_t), true
    );
    if (struct_result.has_value == false) {
        return (array_expect_t){ .has_value = false, .u.error = BAD_ALLOC };
    }

    array_t* dst = (array_t*)struct_result.u.value;

    /* Allocate data buffer sized exactly to the slice length */
    void_ptr_expect_t data_result = alloc_v.allocate(
        alloc_v.ctx, slice_len * src->data_size, false
    );
    if (data_result.has_value == false) {
        alloc_v.return_element(alloc_v.ctx, dst);
        return (array_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    dst->data      = (uint8_t*)data_result.u.value;
    dst->len       = slice_len;
    dst->alloc     = slice_len;
    dst->data_size = src->data_size;
    dst->dtype     = src->dtype;
    dst->growth    = src->growth;
    dst->alloc_v   = alloc_v;

    memcpy(dst->data,
           src->data + start * src->data_size,
           slice_len * src->data_size);

    return (array_expect_t){ .has_value = true, .u.value = dst };
}
// ================================================================================
// array_contains
// ================================================================================

size_expect_t array_contains(const array_t* array,
                             const void*   needle,
                             size_t        start,
                             size_t        end,
                             dtype_id_t    dtype) {
    if (array == NULL || needle == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (dtype != array->dtype)
        return (size_expect_t){ .has_value = false, .u.error = TYPE_MISMATCH };
    if (start >= end)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (end > array->len)
        return (size_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };

    size_t idx = simd_contains_uint8(array->data,
                                     start,
                                     end,
                                     array->data_size,
                                     (const uint8_t*)needle);

    if (idx == SIZE_MAX)
        return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };

    return (size_expect_t){ .has_value = true, .u.value = idx };
}
// ================================================================================
// Introspection
// ================================================================================

size_t array_size(const array_t* array) {
    if (array == NULL) return 0u;
    return array->len;
}

// --------------------------------------------------------------------------------

size_t array_alloc(const array_t* array) {
    if (array == NULL) return 0u;
    return array->alloc;
}

// --------------------------------------------------------------------------------

size_t array_data_size(const array_t* array) {
    if (array == NULL) return 0u;
    return array->data_size;
}

// --------------------------------------------------------------------------------

bool is_array_empty(const array_t* array) {
    if (array == NULL) return true;
    return array->len == 0u;
}

// --------------------------------------------------------------------------------

bool is_array_full(const array_t* array) {
    if (array == NULL) return true;
    return array->len == array->alloc;
}
// -------------------------------------------------------------------------------- 

error_code_t reverse_array(array_t* array) {
    if (array == NULL)   return NULL_POINTER;
    if (array->len < 2u) return EMPTY;

    simd_reverse_uint8(array->data, array->len, array->data_size);
    return NO_ERROR;
}
// ================================================================================
// Sorting
// ================================================================================

/* Swap two elements of data_size bytes using a stack buffer. */
static inline void _swap_elements(uint8_t* a, uint8_t* b, size_t data_size) {
    uint8_t tmp[256];

    if (data_size <= sizeof(tmp)) {
        memcpy(tmp, a,   data_size);
        memcpy(a,   b,   data_size);
        memcpy(b,   tmp, data_size);
    } else {
        /* data_size > 256: byte-by-byte swap to avoid VLA */
        for (size_t i = 0u; i < data_size; i++) {
            uint8_t byte = a[i];
            a[i] = b[i];
            b[i] = byte;
        }
    }
}

// --------------------------------------------------------------------------------

/* Apply direction to a raw comparator result. */
static inline int _apply_dir(int cmp_result, direction_t dir) {
    return (dir == FORWARD) ? cmp_result : -cmp_result;
}

// --------------------------------------------------------------------------------

/* Median-of-three pivot selection. Returns pointer to the chosen element. */
static uint8_t* _median_of_three(uint8_t*  a,
                                  uint8_t*  b,
                                  uint8_t*  c,
                                  int     (*cmp)(const void*, const void*),
                                  direction_t  dir) {
    int ab = _apply_dir(cmp(a, b), dir);
    int bc = _apply_dir(cmp(b, c), dir);
    int ac = _apply_dir(cmp(a, c), dir);

    /* Return the median of the three */
    if (ab <= 0) {
        /* a <= b */
        if (bc <= 0) return b;   /* a <= b <= c -> b is median */
        if (ac <= 0) return c;   /* a <= c < b  -> c is median */
        return a;                /* c < a <= b  -> a is median */
    }
    /* a > b */
    if (ab >= 0 && ac <= 0) return a; /* b < a <= c -> a is median */
    if (bc <= 0) return c;            /* b <= c < a -> c is median */
    return b;                         /* c < b < a  -> b is median */
}

// --------------------------------------------------------------------------------

/* Insertion sort for small partitions. Operates on element indices [lo, hi]. */
static void _insertion_sort(uint8_t* data,
                             size_t   lo,
                             size_t   hi,
                             size_t   data_size,
                             int    (*cmp)(const void*, const void*),
                             direction_t dir) {
    uint8_t tmp[256];

    for (size_t i = lo + 1u; i <= hi; i++) {
        uint8_t* key_ptr = data + i * data_size;

        /* Copy key element into tmp buffer */
        if (data_size <= sizeof(tmp)) {
            memcpy(tmp, key_ptr, data_size);
        } else {
            /* For oversized elements fall back to swap-based insertion */
            size_t j = i;
            while (j > lo &&
                   _apply_dir(cmp(data + (j - 1u) * data_size,
                                  data + j * data_size), dir) > 0) {
                _swap_elements(data + (j - 1u) * data_size,
                               data + j * data_size,
                               data_size);
                j--;
            }
            continue;
        }

        size_t j = i;
        while (j > lo &&
               _apply_dir(cmp(data + (j - 1u) * data_size, tmp), dir) > 0) {
            /* Shift element at j-1 forward to position j */
            memcpy(data + j * data_size,
                   data + (j - 1u) * data_size,
                   data_size);
            j--;
        }
        memcpy(data + j * data_size, tmp, data_size);
    }
}

// --------------------------------------------------------------------------------

/*
 * Partition the sub-array data[lo..hi] (inclusive indices) around a pivot
 * chosen by median-of-three. Pivot is moved to data[hi] before partitioning.
 * Returns the final index of the pivot.
 */
static size_t _partition(uint8_t* data,
                          size_t   lo,
                          size_t   hi,
                          size_t   data_size,
                          int    (*cmp)(const void*, const void*),
                          direction_t dir) {
    size_t   mid      = lo + (hi - lo) / 2u;
    uint8_t* pivot_ptr = _median_of_three(data + lo  * data_size,
                                           data + mid * data_size,
                                           data + hi  * data_size,
                                           cmp, dir);

    /* Move pivot to end so it is out of the way during partitioning */
    if (pivot_ptr != data + hi * data_size)
        _swap_elements(pivot_ptr, data + hi * data_size, data_size);

    uint8_t* pivot = data + hi * data_size;
    size_t   i     = lo;   /* index of last element known to be < pivot */

    for (size_t j = lo; j < hi; j++) {
        if (_apply_dir(cmp(data + j * data_size, pivot), dir) < 0) {
            _swap_elements(data + i * data_size,
                           data + j * data_size,
                           data_size);
            i++;
        }
    }

    /* Place pivot in its final position */
    _swap_elements(data + i * data_size, data + hi * data_size, data_size);
    return i;
}

// --------------------------------------------------------------------------------

/*
 * Iterative quicksort with median-of-three pivot, insertion sort fallback
 * for small partitions, and tail-call optimisation to keep stack depth O(log n).
 */
static void _quicksort(uint8_t* data,
                        size_t   lo,
                        size_t   hi,
                        size_t   data_size,
                        int    (*cmp)(const void*, const void*),
                        direction_t dir) {
#define INSERTION_THRESHOLD 10u

    while (lo < hi) {
        if (hi - lo < INSERTION_THRESHOLD) {
            _insertion_sort(data, lo, hi, data_size, cmp, dir);
            break;
        }

        size_t pi = _partition(data, lo, hi, data_size, cmp, dir);

        /*
         * Recurse into the smaller partition, iterate into the larger.
         * This keeps worst-case stack depth at O(log n).
         */
        if (pi > lo && pi - lo <= hi - pi) {
            _quicksort(data, lo, pi - 1u, data_size, cmp, dir);
            lo = pi + 1u;
        } else {
            if (pi + 1u < hi)
                _quicksort(data, pi + 1u, hi, data_size, cmp, dir);
            if (pi == 0u) break;
            hi = pi - 1u;
        }
    }

#undef INSERTION_THRESHOLD
}

// --------------------------------------------------------------------------------

error_code_t sort_array(array_t* array,
                         int    (*cmp)(const void*, const void*),
                         direction_t dir) {
    if (array == NULL || cmp == NULL) return NULL_POINTER;
    if (array->len < 2u)              return EMPTY;

    _quicksort(array->data, 0u, array->len - 1u, array->data_size, cmp, dir);
    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

bool is_array_ptr(const array_t* array, const void* ptr) {
    if (array == NULL || !array->data || ptr == NULL) return false;

    const uint8_t* p     = (const uint8_t*)ptr;
    const uint8_t* start = array->data;
    const uint8_t* end   = array->data + array->len * array->data_size;

    if (p < start || p >= end) return false;

    /* Verify the pointer falls on an element boundary */
    return ((size_t)(p - start) % array->data_size) == 0u;
}
// ================================================================================
// ================================================================================
// eof
