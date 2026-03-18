// ================================================================================
// ================================================================================
// - File:    c_dict.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 15, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <string.h>   /* memcpy, memset, memcmp */
#include <stdint.h>
#include <math.h>     /* ceil, log2 — for next_power_of_two */
 
#include "c_dict.h"
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Internal constants
// ================================================================================
 
#define DICT_LOAD_FACTOR    0.75
#define DICT_GROWTH_FACTOR  2u
#define DICT_LARGE_THRESH   4096u
#define DICT_LARGE_STEP     1024u
#define DICT_MIN_ALLOC      8u
#define HASH_SEED           0xdeadbeef
 
// ================================================================================
// Hash function — MurmurHash3-inspired, operates on raw bytes
// ================================================================================
 
static size_t _hash_bytes(const void* data, size_t len, uint32_t seed) {
    if (!data || len == 0) return 0u;
 
    const uint32_t c1 = 0xcc9e2d51u;
    const uint32_t c2 = 0x1b873593u;
    uint32_t h1 = seed;
 
    const unsigned char* p      = (const unsigned char*)data;
    const size_t         nblocks = len / 4u;
 
    for (size_t i = 0; i < nblocks; ++i) {
        uint32_t k1;
        memcpy(&k1, p + i * 4u, 4u);   /* strict-aliasing safe */
 
        k1 *= c1;
        k1  = (k1 << 15) | (k1 >> 17);
        k1 *= c2;
 
        h1 ^= k1;
        h1  = (h1 << 13) | (h1 >> 19);
        h1  = h1 * 5u + 0xe6546b64u;
    }
 
    /* Tail */
    const unsigned char* tail = p + nblocks * 4u;
    uint32_t k1 = 0u;
 
    switch (len & 3u) {
        case 3: k1 ^= (uint32_t)tail[2] << 16; /* fallthrough */
        case 2: k1 ^= (uint32_t)tail[1] <<  8; /* fallthrough */
        case 1: k1 ^= (uint32_t)tail[0];
                k1 *= c1;
                k1  = (k1 << 15) | (k1 >> 17);
                k1 *= c2;
                h1 ^= k1;
    }
 
    /* Finalisation */
    h1 ^= (uint32_t)len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6bu;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35u;
    h1 ^= h1 >> 16;
 
    return (size_t)h1;
}
 
// ================================================================================
// Internal helpers
// ================================================================================
 
/* Round n up to the next power of two (>= DICT_MIN_ALLOC). */
static size_t _next_pow2(size_t n) {
    if (n < DICT_MIN_ALLOC) return DICT_MIN_ALLOC;
    n--;
    n |= n >>  1;
    n |= n >>  2;
    n |= n >>  4;
    n |= n >>  8;
    n |= n >> 16;
#if SIZE_MAX > 0xFFFFFFFFu
    n |= n >> 32;
#endif
    return n + 1u;
}
 
/* Const-qualified node lookup — for read-only operations. */
static const dict_node_t* _find_node_c(const dict_bucket_t* bucket,
                                       const void*          key_data,
                                       size_t               key_len) {
    for (const dict_node_t* n = bucket->next; n != NULL; n = n->next) {
        if (n->key_len == key_len &&
            memcmp(n->key_data, key_data, key_len) == 0) {
            return n;
        }
    }
    return NULL;
}
 
/* Compute the bucket index for a key given a table of size alloc. */
static size_t _bucket_index(const void* data, size_t len, size_t alloc) {
    return _hash_bytes(data, len, HASH_SEED) % alloc;
}
 
/*
 * Allocate a new dict_node_t with an inline value buffer of data_size bytes.
 * Returns NULL on allocation failure.
 * Layout: [dict_node_t][uint8_t value_buffer[data_size]]
 */
static dict_node_t* _alloc_node(dict_key_t         key,
                                const void*        value,
                                size_t             data_size,
                                allocator_vtable_t alloc_v) {
    /* Single allocation for node header + value buffer */
    size_t node_bytes = sizeof(dict_node_t) + data_size;
    void_ptr_expect_t nr = alloc_v.allocate(alloc_v.ctx, node_bytes, true);
    if (!nr.has_value) return NULL;
    dict_node_t* node = (dict_node_t*)nr.u.value;
 
    /* Key copy */
    void_ptr_expect_t kr = alloc_v.allocate(alloc_v.ctx, key.len + 1u, true);
    if (!kr.has_value) {
        alloc_v.return_element(alloc_v.ctx, node);
        return NULL;
    }
    node->key_data = (uint8_t*)kr.u.value;
    memcpy(node->key_data, key.data, key.len);
    node->key_data[key.len] = 0;   /* null-terminate for convenience */
    node->key_len  = key.len;
    node->next     = NULL;
 
    /* Value copy into inline buffer */
    memcpy(dict_node_value(node), value, data_size);
 
    return node;
}
 
/* Free one node (key allocation + node+value allocation). */
static void _free_node(dict_node_t* node, allocator_vtable_t alloc_v) {
    if (!node) return;
    alloc_v.return_element(alloc_v.ctx, node->key_data);
    alloc_v.return_element(alloc_v.ctx, node);
}
 
/* Find a node by key within a single bucket chain. Returns NULL if not found. */
static dict_node_t* _find_node(dict_bucket_t* bucket,
                               const void*    key_data,
                               size_t         key_len) {
    for (dict_node_t* n = bucket->next; n != NULL; n = n->next) {
        if (n->key_len == key_len &&
            memcmp(n->key_data, key_data, key_len) == 0) {
            return n;
        }
    }
    return NULL;
}
 
/*
 * Resize the bucket array to new_alloc buckets.  All existing nodes are
 * rehashed into the new array.  The old bucket array is freed via alloc_v.
 * Returns NO_ERROR on success or OUT_OF_MEMORY on allocation failure.
 */
static error_code_t _resize(dict_t* dict, size_t new_alloc) {
    new_alloc = _next_pow2(new_alloc);
 
    /* Allocate new bucket array (zeroed so all next pointers are NULL). */
    void_ptr_expect_t br = dict->alloc_v.allocate(dict->alloc_v.ctx,
                                                   new_alloc * sizeof(dict_bucket_t),
                                                   true);
    if (!br.has_value) return OUT_OF_MEMORY;
    dict_bucket_t* new_buckets = (dict_bucket_t*)br.u.value;
 
    /* Rehash: walk every bucket in the old array. */
    for (size_t i = 0; i < dict->alloc; ++i) {
        dict_node_t* cur = dict->buckets[i].next;
        while (cur != NULL) {
            dict_node_t* nxt = cur->next;
            size_t idx = _bucket_index(cur->key_data, cur->key_len, new_alloc);
            cur->next = new_buckets[idx].next;
            new_buckets[idx].next = cur;
            cur = nxt;
        }
    }
 
    /* Recompute len (occupied bucket count) from scratch after rehash. */
    size_t new_len = 0u;
    for (size_t i = 0; i < new_alloc; ++i) {
        if (new_buckets[i].next != NULL) ++new_len;
    }
 
    dict->alloc_v.return_element(dict->alloc_v.ctx, dict->buckets);
    dict->buckets = new_buckets;
    dict->alloc   = new_alloc;
    dict->len     = new_len;
 
    return NO_ERROR;
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
dict_expect_t init_dict(size_t             capacity,
                        size_t             data_size,
                        dtype_id_t         dtype,
                        bool               growth,
                        allocator_vtable_t alloc_v) {
    if (alloc_v.allocate == NULL || alloc_v.return_element == NULL)
        return (dict_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (capacity == 0u || data_size == 0u)
        return (dict_expect_t){ .has_value = false, .u.error = INVALID_ARG };
 
    size_t alloc = _next_pow2(capacity);
 
    /* Allocate the dict_t struct. */
    void_ptr_expect_t dr = alloc_v.allocate(alloc_v.ctx, sizeof(dict_t), true);
    if (!dr.has_value)
        return (dict_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    dict_t* d = (dict_t*)dr.u.value;
 
    /* Allocate the bucket array (zeroed → all next pointers are NULL). */
    void_ptr_expect_t br = alloc_v.allocate(alloc_v.ctx,
                                             alloc * sizeof(dict_bucket_t), true);
    if (!br.has_value) {
        alloc_v.return_element(alloc_v.ctx, d);
        return (dict_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }
 
    d->buckets   = (dict_bucket_t*)br.u.value;
    d->len       = 0u;
    d->hash_size = 0u;
    d->alloc     = alloc;
    d->data_size = data_size;
    d->dtype     = dtype;
    d->growth    = growth;
    d->alloc_v   = alloc_v;
 
    return (dict_expect_t){ .has_value = true, .u.value = d };
}
 
// --------------------------------------------------------------------------------
 
void return_dict(dict_t* dict) {
    if (dict == NULL) return;
 
    for (size_t i = 0; i < dict->alloc; ++i) {
        dict_node_t* cur = dict->buckets[i].next;
        while (cur != NULL) {
            dict_node_t* nxt = cur->next;
            _free_node(cur, dict->alloc_v);
            cur = nxt;
        }
    }
 
    dict->alloc_v.return_element(dict->alloc_v.ctx, dict->buckets);
    dict->alloc_v.return_element(dict->alloc_v.ctx, dict);
}
 
// ================================================================================
// Insert / remove / update
// ================================================================================
 
error_code_t insert_dict(dict_t*            dict,
                         dict_key_t         key,
                         const void*        value,
                         allocator_vtable_t alloc_v) {
    if (dict == NULL || key.data == NULL || value == NULL)
        return NULL_POINTER;
    if (key.len == 0u)
        return INVALID_ARG;
 
    /* Grow if load factor exceeded. */
    if (dict->growth &&
        dict->hash_size >= (size_t)(dict->alloc * DICT_LOAD_FACTOR)) {
        size_t new_alloc = (dict->alloc < DICT_LARGE_THRESH)
                           ? dict->alloc * DICT_GROWTH_FACTOR
                           : dict->alloc + DICT_LARGE_STEP;
        error_code_t err = _resize(dict, new_alloc);
        if (err != NO_ERROR) return err;
    }
 
    if (!dict->growth && dict->hash_size >= dict->alloc)
        return CAPACITY_OVERFLOW;
 
    size_t idx = _bucket_index(key.data, key.len, dict->alloc);
 
    /* Reject duplicate. */
    if (_find_node(&dict->buckets[idx], key.data, key.len) != NULL)
        return INVALID_ARG;   /* duplicate key */
 
    dict_node_t* node = _alloc_node(key, value, dict->data_size, alloc_v);
    if (node == NULL) return OUT_OF_MEMORY;
 
    bool was_empty = (dict->buckets[idx].next == NULL);
    node->next = dict->buckets[idx].next;
    dict->buckets[idx].next = node;
 
    dict->hash_size++;
    if (was_empty) dict->len++;
 
    return NO_ERROR;
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_dict(dict_t*     dict,
                      dict_key_t  key,
                      void*       out_value) {
    if (dict == NULL || key.data == NULL)
        return NULL_POINTER;
    if (key.len == 0u)
        return INVALID_ARG;
 
    size_t        idx     = _bucket_index(key.data, key.len, dict->alloc);
    dict_node_t** prevnxt = &dict->buckets[idx].next;
    dict_node_t*  cur     = dict->buckets[idx].next;
 
    while (cur != NULL) {
        if (cur->key_len == key.len &&
            memcmp(cur->key_data, key.data, key.len) == 0) {
 
            if (out_value != NULL)
                memcpy(out_value, dict_node_value(cur), dict->data_size);
 
            *prevnxt = cur->next;
            _free_node(cur, dict->alloc_v);
 
            dict->hash_size--;
            if (dict->buckets[idx].next == NULL) dict->len--;
 
            return NO_ERROR;
        }
        prevnxt = &cur->next;
        cur     = cur->next;
    }
 
    return NOT_FOUND;
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_dict(dict_t*     dict,
                         dict_key_t  key,
                         const void* value) {
    if (dict == NULL || key.data == NULL || value == NULL)
        return NULL_POINTER;
    if (key.len == 0u)
        return INVALID_ARG;
 
    size_t        idx  = _bucket_index(key.data, key.len, dict->alloc);
    dict_node_t*  node = _find_node(&dict->buckets[idx], key.data, key.len);
 
    if (node == NULL) return NOT_FOUND;
 
    memcpy(dict_node_value(node), value, dict->data_size);
    return NO_ERROR;
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_dict_value(const dict_t* dict,
                            dict_key_t    key,
                            void*         out_value) {
    if (dict == NULL || key.data == NULL || out_value == NULL)
        return NULL_POINTER;
    if (key.len == 0u)
        return INVALID_ARG;
 
    size_t              idx  = _bucket_index(key.data, key.len, dict->alloc);
    const dict_node_t*  node = _find_node_c(&dict->buckets[idx],
                                             key.data, key.len);
 
    if (node == NULL) return NOT_FOUND;
 
    memcpy(out_value, dict_node_value_c(node), dict->data_size);
    return NO_ERROR;
}
 
// --------------------------------------------------------------------------------
 
const void* get_dict_value_ptr(const dict_t* dict, dict_key_t key) {
    if (dict == NULL || key.data == NULL || key.len == 0u) return NULL;
 
    size_t              idx  = _bucket_index(key.data, key.len, dict->alloc);
    const dict_node_t*  node = _find_node_c(&dict->buckets[idx],
                                             key.data, key.len);
 
    return (node != NULL) ? dict_node_value_c(node) : NULL;
}
 
// --------------------------------------------------------------------------------
 
bool has_dict_key(const dict_t* dict, dict_key_t key) {
    if (dict == NULL || key.data == NULL || key.len == 0u) return false;
 
    size_t idx = _bucket_index(key.data, key.len, dict->alloc);
    return _find_node_c(&dict->buckets[idx], key.data, key.len) != NULL;
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_dict(dict_t* dict) {
    if (dict == NULL) return NULL_POINTER;
 
    for (size_t i = 0; i < dict->alloc; ++i) {
        dict_node_t* cur = dict->buckets[i].next;
        while (cur != NULL) {
            dict_node_t* nxt = cur->next;
            _free_node(cur, dict->alloc_v);
            cur = nxt;
        }
        dict->buckets[i].next = NULL;
    }
 
    dict->hash_size = 0u;
    dict->len       = 0u;
    return NO_ERROR;
}
 
// --------------------------------------------------------------------------------
 
dict_expect_t copy_dict(const dict_t* src, allocator_vtable_t alloc_v) {
    if (src == NULL)
        return (dict_expect_t){ .has_value = false, .u.error = NULL_POINTER };
 
    dict_expect_t dr = init_dict(src->alloc, src->data_size,
                                 src->dtype, src->growth, alloc_v);
    if (!dr.has_value) return dr;
    dict_t* dst = dr.u.value;
 
    for (size_t i = 0; i < src->alloc; ++i) {
        for (dict_node_t* cur = src->buckets[i].next;
             cur != NULL; cur = cur->next) {
            dict_key_t k = { .data = cur->key_data, .len = cur->key_len };
            error_code_t err = insert_dict(dst, k,
                                           dict_node_value_c(cur), alloc_v);
            if (err != NO_ERROR) {
                return_dict(dst);
                return (dict_expect_t){ .has_value = false, .u.error = err };
            }
        }
    }
 
    return (dict_expect_t){ .has_value = true, .u.value = dst };
}
 
// --------------------------------------------------------------------------------
 
dict_expect_t merge_dict(const dict_t*      a,
                         const dict_t*      b,
                         bool               overwrite,
                         allocator_vtable_t alloc_v) {
    if (a == NULL || b == NULL)
        return (dict_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (a->data_size != b->data_size)
        return (dict_expect_t){ .has_value = false, .u.error = INVALID_ARG };
 
    /* Start with a copy of a. */
    dict_expect_t dr = copy_dict(a, alloc_v);
    if (!dr.has_value) return dr;
    dict_t* dst = dr.u.value;
 
    for (size_t i = 0; i < b->alloc; ++i) {
        for (dict_node_t* cur = b->buckets[i].next;
             cur != NULL; cur = cur->next) {
            dict_key_t k = { .data = cur->key_data, .len = cur->key_len };
            const void* val = dict_node_value_c(cur);
 
            if (has_dict_key(dst, k)) {
                if (overwrite) {
                    error_code_t err = update_dict(dst, k, val);
                    if (err != NO_ERROR) {
                        return_dict(dst);
                        return (dict_expect_t){ .has_value = false,
                                                .u.error   = err };
                    }
                }
            } else {
                error_code_t err = insert_dict(dst, k, val, alloc_v);
                if (err != NO_ERROR) {
                    return_dict(dst);
                    return (dict_expect_t){ .has_value = false,
                                            .u.error   = err };
                }
            }
        }
    }
 
    return (dict_expect_t){ .has_value = true, .u.value = dst };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
error_code_t foreach_dict(const dict_t* dict,
                          dict_iter_fn  fn,
                          void*         user_data) {
    if (dict == NULL || fn == NULL) return NULL_POINTER;
 
    for (size_t i = 0; i < dict->alloc; ++i) {
        for (dict_node_t* cur = dict->buckets[i].next;
             cur != NULL; cur = cur->next) {
            dict_entry_t e = {
                .key       = cur->key_data,
                .key_len   = cur->key_len,
                .value     = dict_node_value_c(cur),
                .value_len = dict->data_size
            };
            fn(e, user_data);
        }
    }
 
    return NO_ERROR;
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t dict_size(const dict_t* dict) {
    return (dict != NULL) ? dict->len : 0u;
}
 
size_t dict_hash_size(const dict_t* dict) {
    return (dict != NULL) ? dict->hash_size : 0u;
}
 
size_t dict_alloc(const dict_t* dict) {
    return (dict != NULL) ? dict->alloc : 0u;
}
 
size_t dict_data_size(const dict_t* dict) {
    return (dict != NULL) ? dict->data_size : 0u;
}
 
bool is_dict_empty(const dict_t* dict) {
    return (dict == NULL || dict->hash_size == 0u);
}
// ================================================================================
// ================================================================================
// eof
