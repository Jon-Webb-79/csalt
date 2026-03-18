// ================================================================================
// ================================================================================
// - File:    c_dict.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_dict_H
#define c_dict_H

#include <stddef.h>
#include <string.h>   /* strlen — used by DICT_KEY macro */
#include <stdint.h>
#include <stdbool.h>
 
#include "c_dtypes.h"       /* dtype_id_t, error_code_t, ... */
#include "c_allocator.h"    /* allocator_vtable_t             */
#include "c_error.h"        /* NO_ERROR, NULL_POINTER, ...    */
// ================================================================================ 
// ========================================================================================== 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Key type
// ================================================================================
 
/**
 * @brief A non-owning view of a key as a byte span.
 *
 * Both string keys and arbitrary binary keys are represented the same way.
 * For null-terminated C-strings, set @p data to the string and @p len to
 * @c strlen(string).  For binary keys, set @p len to the byte count.
 *
 * The dict does **not** retain the @p data pointer — it copies the bytes
 * into its own allocation during insert.  The caller may free or reuse the
 * key memory immediately after any dict call.
 */
typedef struct {
    const void* data;   /**< Pointer to the key bytes.  Must not be NULL. */
    size_t      len;    /**< Number of bytes in the key.  Must be > 0.    */
} dict_key_t;
 
// ================================================================================
// Convenience macro — build a dict_key_t from a C-string literal
// ================================================================================
 
/**
 * @brief Construct a @ref dict_key_t from a null-terminated C-string at
 *        compile time using @c sizeof.
 *
 * The length is set to @c strlen equivalent via pointer arithmetic so that
 * the key does not include the terminating @c \\0.
 *
 * @code
 *     insert_dict(d, DICT_KEY("hello"), &value, alloc_v);
 * @endcode
 */
#define DICT_KEY(s)  ((dict_key_t){ .data = (s), .len = strlen(s) })
 
// ================================================================================
// Node and table structs
// ================================================================================
 
/**
 * @brief A single chained node in the hash table.
 *
 * Each node owns:
 * - An allocator-managed copy of the key bytes (key_data, key_len bytes).
 * - An inline value buffer of @c data_size bytes immediately following the
 *   struct in the same allocation (accessed via dict_node_value()).
 *
 * Nodes are singly-linked within a bucket.
 */
typedef struct dict_node_t {
    struct dict_node_t* next;     /**< Next node in the same bucket, or NULL. */
    size_t              key_len;  /**< Length of the key in bytes.            */
    uint8_t*            key_data; /**< Allocator-managed copy of the key bytes.  */
    /* Inline value buffer follows in memory — use dict_node_value() to access. */
} dict_node_t;
 
/**
 * @brief A bucket sentinel — each bucket[i] is one of these; its @c next
 *        pointer is the head of the chain for that bucket.
 *
 * The sentinel itself never holds a key or value; only the linked nodes do.
 */
typedef struct {
    dict_node_t* next; /**< Head of the linked list for this bucket. */
} dict_bucket_t;
 
/**
 * @brief A generic hash dictionary mapping byte-span keys to fixed-size values.
 *
 * All fields are public so that advanced users can inspect internals, but
 * they should be treated as read-only outside of the dict API.
 *
 * The value for each key is stored as @p data_size raw bytes.  Type-specific
 * wrappers cast a typed pointer to @c void* on insert and cast back on
 * retrieval.  For pointer-valued dicts (e.g. mapping strings to
 * @c str_array_t*), store the pointer itself — @p data_size ==
 * @c sizeof(void*).
 *
 * @code
 *     // Float-valued dict
 *     allocator_vtable_t a = heap_allocator();
 *     dict_expect_t r = init_dict(8, sizeof(float), FLOAT_TYPE, true, a);
 *     float x = 3.14f;
 *     insert_dict(r.u.value, DICT_KEY("pi"), &x, a);
 *
 *     // Pointer-valued dict (string_t* values)
 *     dict_expect_t r2 = init_dict(8, sizeof(string_t*), STRING_TYPE, true, a);
 * @endcode
 */
typedef struct {
    dict_bucket_t*     buckets;    /**< Array of bucket sentinels, length alloc. */
    size_t             len;        /**< Number of occupied buckets (non-empty chains). */
    size_t             hash_size;  /**< Total number of key-value pairs stored.       */
    size_t             alloc;      /**< Number of buckets allocated.                  */
    size_t             data_size;  /**< Value size in bytes, fixed at init.           */
    dtype_id_t         dtype;      /**< Type tag for the value, fixed at init.        */
    bool               growth;     /**< If true, resize automatically on high load.   */
    allocator_vtable_t alloc_v;    /**< Allocator used for all internal allocations.  */
} dict_t;
 
// ================================================================================
// Expected return type
// ================================================================================
 
/**
 * @brief Expected return type for dict_t init and copy operations.
 */
typedef struct {
    bool has_value;
    union {
        dict_t*      value;
        error_code_t error;
    } u;
} dict_expect_t;
 
// ================================================================================
// Value access helper
// ================================================================================
 
/**
 * @brief Return a pointer to the inline value buffer of a node.
 *
 * The value buffer is allocated as part of the same block as @p node, at
 * offset @c sizeof(dict_node_t).
 *
 * @param node  Must not be NULL.
 * @return Pointer to the first byte of the value.
 */
static inline uint8_t* dict_node_value(dict_node_t* node) {
    return (uint8_t*)node + sizeof(dict_node_t);
}
 
/** @brief Const-qualified variant of dict_node_value(). */
static inline const uint8_t* dict_node_value_c(const dict_node_t* node) {
    return (const uint8_t*)node + sizeof(dict_node_t);
}
 
// ================================================================================
// Iterator type
// ================================================================================
 
/**
 * @brief Read-only view of a single key-value entry, passed to the iterator
 *        callback.
 */
typedef struct {
    const void*   key;       /**< Pointer to the key bytes.            */
    size_t        key_len;   /**< Length of the key in bytes.          */
    const void*   value;     /**< Pointer to the value bytes.          */
    size_t        value_len; /**< Value size in bytes (== data_size).  */
} dict_entry_t;
 
/**
 * @brief Iterator callback type for foreach_dict().
 *
 * @param entry      Read-only view of the current key-value pair.
 * @param user_data  Caller-supplied context pointer, may be NULL.
 */
typedef void (*dict_iter_fn)(dict_entry_t entry, void* user_data);
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
/**
 * @brief Allocate and initialise a new dict_t.
 *
 * @param capacity   Initial bucket count.  Must be > 0.  Rounded up to the
 *                   next power of two internally.
 * @param data_size  Size of each value in bytes.  Must be > 0.
 * @param dtype      Type tag stored in @c dict_t::dtype.  Use one of the
 *                   @c *_TYPE constants from @c c_dtypes.h, or a user-defined
 *                   id >= @c USER_BASE_TYPE.
 * @param growth     If true, the table resizes automatically when the load
 *                   factor exceeds 0.75.
 * @param alloc_v    Allocator for all internal memory.  @c alloc_v.allocate
 *                   must not be NULL.
 *
 * @return dict_expect_t with @c has_value true and @c u.value pointing to the
 *         new dict on success; @c has_value false with @c u.error set to
 *         NULL_POINTER, INVALID_ARG, or OUT_OF_MEMORY on failure.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *     dict_expect_t r = init_dict(16, sizeof(float), FLOAT_TYPE, true, a);
 *     if (!r.has_value) { handle r.u.error }
 *     dict_t* d = r.u.value;
 * @endcode
 */
dict_expect_t init_dict(size_t             capacity,
                        size_t             data_size,
                        dtype_id_t         dtype,
                        bool               growth,
                        allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Free all memory owned by a dict_t.
 *
 * Releases every node (key copy + value buffer), the bucket array, and the
 * dict_t struct itself.  Passing NULL is safe and performs no action.
 *
 * @param dict  Dictionary to free.
 */
void return_dict(dict_t* dict);
 
// ================================================================================
// Insert / remove / update
// ================================================================================
 
/**
 * @brief Insert a new key-value pair.
 *
 * The key bytes are deep-copied via the allocator.  The value is
 * copied bytewise (@p data_size bytes) from @p value into the node.
 *
 * If the key already exists the insertion is rejected and INVALID_ARG is
 * returned.  To update an existing key use update_dict().
 *
 * If @c growth is true and the load factor exceeds 0.75, the table is
 * resized before insertion.
 *
 * @param dict    Must not be NULL.
 * @param key     Key to insert.  @p key.data must not be NULL; @p key.len
 *                must be > 0.
 * @param value   Pointer to @p dict->data_size bytes to copy as the value.
 *                Must not be NULL.
 * @param alloc_v Allocator used to allocate the new node.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG (duplicate key),
 *         CAPACITY_OVERFLOW (table full and growth == false), or OUT_OF_MEMORY.
 */
error_code_t insert_dict(dict_t*            dict,
                         dict_key_t         key,
                         const void*        value,
                         allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Remove the entry with the given key and copy its value out.
 *
 * If @p out_value is non-NULL and the key is found, @p dict->data_size bytes
 * are copied from the node's value buffer into @p out_value before the node
 * is freed.  If the key is not found, @p out_value is not written.
 *
 * @param dict       Must not be NULL.
 * @param key        Key to remove.
 * @param out_value  Destination buffer of at least @p dict->data_size bytes,
 *                   or NULL to discard the value.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 */
error_code_t pop_dict(dict_t*     dict,
                      dict_key_t  key,
                      void*       out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Overwrite the value of an existing key.
 *
 * Copies @p dict->data_size bytes from @p value into the node's inline
 * buffer.  The key is not re-hashed and no allocation is performed.
 *
 * @param dict   Must not be NULL.
 * @param key    Key to update.  Must already exist.
 * @param value  Pointer to @p dict->data_size bytes.  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 */
error_code_t update_dict(dict_t*     dict,
                         dict_key_t  key,
                         const void* value);
 
// ================================================================================
// Lookup
// ================================================================================
 
/**
 * @brief Copy the value associated with a key into a caller-supplied buffer.
 *
 * @param dict       Must not be NULL.
 * @param key        Key to look up.
 * @param out_value  Destination buffer of at least @p dict->data_size bytes.
 *                   Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 */
error_code_t get_dict_value(const dict_t* dict,
                            dict_key_t    key,
                            void*         out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return a read-only pointer directly into the node's value buffer.
 *
 * The pointer is valid until the next mutation of the dict.  The caller must
 * not free or write through it.
 *
 * @param dict  Must not be NULL.
 * @param key   Key to look up.
 *
 * @return Pointer to the value bytes on success, NULL if not found or on
 *         error.
 */
const void* get_dict_value_ptr(const dict_t* dict, dict_key_t key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Test whether a key exists in the dict without retrieving its value.
 *
 * @param dict  Must not be NULL.
 * @param key   Key to test.
 *
 * @return true if the key exists, false if not found or on error.
 */
bool has_dict_key(const dict_t* dict, dict_key_t key);
 
// ================================================================================
// Utility
// ================================================================================
 
/**
 * @brief Remove all entries without freeing the dict or its bucket array.
 *
 * Frees every node (key copy + value buffer).  The bucket array is retained
 * and zeroed, ready for reuse.  @c len and @c hash_size are reset to 0.
 *
 * @param dict  Must not be NULL.
 *
 * @return NO_ERROR or NULL_POINTER.
 */
error_code_t clear_dict(dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Allocate a deep copy of @p src.
 *
 * All nodes are rehashed into a fresh bucket array of the same capacity.
 * The copy uses @p alloc_v for all allocations; @p src->alloc_v is not
 * forwarded automatically.
 *
 * @param src     Must not be NULL.
 * @param alloc_v Allocator for the new dict and its nodes.
 *
 * @return dict_expect_t with @c has_value true on success.
 */
dict_expect_t copy_dict(const dict_t* src, allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Merge two dicts into a new dict.
 *
 * All entries from @p a are inserted first.  Entries from @p b are then
 * processed:
 * - If the key does not exist in the result, it is inserted.
 * - If the key already exists and @p overwrite is true, the value is updated.
 * - If the key already exists and @p overwrite is false, the original value
 *   is kept.
 *
 * Both source dicts must have the same @p data_size; if they differ
 * INVALID_ARG is returned.
 *
 * @param a          First source dict.  Must not be NULL.
 * @param b          Second source dict.  Must not be NULL.
 * @param overwrite  If true, @p b's values win on key conflicts.
 * @param alloc_v    Allocator for the new dict.
 *
 * @return dict_expect_t with @c has_value true on success.
 */
dict_expect_t merge_dict(const dict_t*      a,
                         const dict_t*      b,
                         bool               overwrite,
                         allocator_vtable_t alloc_v);
 
// ================================================================================
// Iteration
// ================================================================================
 
/**
 * @brief Call @p fn once for every entry in the dict.
 *
 * Traversal order is bucket order (not insertion order).  The callback
 * receives a @ref dict_entry_t view; it must not insert or remove entries
 * during traversal.
 *
 * @param dict       Must not be NULL.
 * @param fn         Callback invoked for each entry.  Must not be NULL.
 * @param user_data  Passed unchanged to @p fn; may be NULL.
 *
 * @return NO_ERROR or NULL_POINTER.
 *
 * @code
 *     static void print_entry(dict_entry_t e, void* ud) {
 *         (void)ud;
 *         float v;
 *         memcpy(&v, e.value, sizeof(float));
 *         printf("%.*s = %f\n", (int)e.key_len, (const char*)e.key, v);
 *     }
 *     foreach_dict(d, print_entry, NULL);
 * @endcode
 */
error_code_t foreach_dict(const dict_t* dict,
                          dict_iter_fn  fn,
                          void*         user_data);
 
// ================================================================================
// Introspection
// ================================================================================
 
/**
 * @brief Number of occupied buckets (chains with at least one entry).
 *
 * Returns 0 if @p dict is NULL.
 */
size_t dict_size(const dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Total number of key-value pairs stored.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t dict_hash_size(const dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Number of buckets allocated.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t dict_alloc(const dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Value size in bytes as fixed at initialisation.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t dict_data_size(const dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief true if @p dict is NULL or contains no entries.
 */
bool is_dict_empty(const dict_t* dict);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_dict_H */
// ================================================================================
// ================================================================================
// eof
