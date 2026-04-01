// ================================================================================
// ================================================================================
// - File:    c_list.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 30, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_list.h"

// ================================================================================ 
// ================================================================================ 
 
// ================================================================================
// Internal helpers
// ================================================================================

static inline size_t _node_stride(size_t data_size) {
    size_t raw   = sizeof(sNode) + data_size;
    size_t align = _Alignof(sNode);
    return (raw + align - 1u) & ~(align - 1u);
}
// --------------------------------------------------------------------------------

static inline bool _ptr_in_slab(const slist_t* l, const sNode* node) {
    const uint8_t* p  = (const uint8_t*)node;
    const uint8_t* lo = l->slab;
    const uint8_t* hi = l->slab + l->slab_cap * _node_stride(l->data_size);
    return p >= lo && p < hi;
}
// --------------------------------------------------------------------------------

static inline void _push_slab_free(slist_t* l, sNode* node) {
    if (!l || !node) return;

    node->next   = l->slab_free;
    l->slab_free = node;
    l->slab_free_count++;
}
// --------------------------------------------------------------------------------

static inline sNode* _pop_slab_free(slist_t* l) {
    if (!l || !l->slab_free) return NULL;

    sNode* node  = l->slab_free;
    l->slab_free = l->slab_free->next;
    node->next   = NULL;

    if (l->slab_free_count > 0u)
        l->slab_free_count--;

    return node;
}
// --------------------------------------------------------------------------------

static sNode* _alloc_node(slist_t* l) {
    if (!l) return NULL;

    /* 1. Reuse recycled slab node first */
    if (l->slab_free) {
        sNode* node = _pop_slab_free(l);
        if (node) {
            l->slab_used++;
        }
        return node;
    }

    /*
     * 2. Consume a fresh slab slot.
     *
     * slab_used       = currently live slab-backed nodes
     * slab_free_count = previously used but currently free slab nodes
     *
     * Their sum is therefore the number of slab slots that have been touched
     * so far, which is the correct next fresh slot index.
     */
    {
        size_t touched = l->slab_used + l->slab_free_count;
        if (touched < l->slab_cap) {
            sNode* node = (sNode*)(l->slab + touched * _node_stride(l->data_size));
            node->next  = NULL;
            l->slab_used++;
            return node;
        }
    }

    /* 3. Fall back to overflow allocation */
    if (!l->allow_overflow)
        return NULL;

    size_t            sz = sizeof(sNode) + l->data_size;
    void_ptr_expect_t r  = l->alloc_v.allocate(l->alloc_v.ctx, sz, /*zeroed=*/true);
    if (!r.has_value) return NULL;

    sNode* node = r.u.value;
    node->next  = NULL;
    l->overflow_live++;
    return node;
}
// --------------------------------------------------------------------------------

static inline error_code_t _alloc_node_error(const slist_t* l) {
    if (!l) return NULL_POINTER;

    return ((l->slab_used + l->slab_free_count) >= l->slab_cap &&
            l->slab_free_count == 0u &&
            !l->allow_overflow)
           ? CAPACITY_OVERFLOW
           : OUT_OF_MEMORY;
}
// --------------------------------------------------------------------------------

static inline void _return_node(slist_t* l, sNode* node) {
    if (!l || !node) return;

    if (_ptr_in_slab(l, node)) {
        if (l->slab_used > 0u)
            l->slab_used--;
        _push_slab_free(l, node);
        return;
    }

    l->alloc_v.return_element(l->alloc_v.ctx, node);

    if (l->overflow_live > 0u)
        l->overflow_live--;
}

// ================================================================================
// init_slist
// ================================================================================
 
slist_expect_t init_slist(size_t             num_nodes,
                          size_t             data_size,
                          dtype_id_t         dtype,
                          bool               allow_overflow,
                          allocator_vtable_t alloc_v) {
 
    if (!alloc_v.allocate)
        return (slist_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (num_nodes == 0 || data_size == 0)
        return (slist_expect_t){ .has_value = false, .u.error = INVALID_ARG };
 
    void_ptr_expect_t hr = alloc_v.allocate(alloc_v.ctx, sizeof(slist_t),
                                            /*zeroed=*/true);
    if (!hr.has_value)
        return (slist_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
 
    slist_t* l = hr.u.value;
 
    size_t            slab_sz = num_nodes * _node_stride(data_size);
    void_ptr_expect_t sr      = alloc_v.allocate(alloc_v.ctx, slab_sz,
                                                 /*zeroed=*/true);
    if (!sr.has_value) {
        alloc_v.return_element(alloc_v.ctx, l);
        return (slist_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }
 
    l->slab            = sr.u.value;
    l->slab_cap        = num_nodes;
    l->slab_used       = 0u;
    l->slab_free       = NULL;
    l->slab_free_count = 0u;
    l->overflow_live   = 0u;
    l->data_size       = data_size;
    l->dtype           = dtype;
    l->len             = 0u;
    l->head            = NULL;
    l->tail            = NULL;
    l->allow_overflow  = allow_overflow;
    l->alloc_v         = alloc_v;
 
    return (slist_expect_t){ .has_value = true, .u.value = l };
}
 
// ================================================================================
// return_slist
// ================================================================================
 
void return_slist(slist_t* list) {
    if (!list) return;
 
    sNode* node = list->head;
    while (node) {
        sNode* next = node->next;
        _return_node(list, node);
        node = next;
    }
 
    list->alloc_v.return_element(list->alloc_v.ctx, list->slab);
    list->alloc_v.return_element(list->alloc_v.ctx, list);
}
 
// ================================================================================
// get_slist
// ================================================================================
 
error_code_t get_slist(const slist_t* list, size_t index, void* out_value) {
    if (!list || !out_value) return NULL_POINTER;
    if (index >= list->len)  return NOT_FOUND;
 
    sNode* node = list->head;
    for (size_t i = 0; i < index; i++)
        node = node->next;
 
    memcpy(out_value, node->data, list->data_size);
    return NO_ERROR;
}
 
// ================================================================================
// peek_front_slist / peek_back_slist
// ================================================================================
 
error_code_t peek_front_slist(const slist_t* list, void* out_value) {
    if (!list || !out_value) return NULL_POINTER;
    if (!list->head)         return NOT_FOUND;
    memcpy(out_value, list->head->data, list->data_size);
    return NO_ERROR;
}
 
error_code_t peek_back_slist(const slist_t* list, void* out_value) {
    if (!list || !out_value) return NULL_POINTER;
    if (!list->tail)         return NOT_FOUND;
    memcpy(out_value, list->tail->data, list->data_size);
    return NO_ERROR;
}
 
// ================================================================================
// push_back_slist
// ================================================================================
 
error_code_t push_back_slist(slist_t* list, const void* value) {
    if (!list || !value) return NULL_POINTER;
 
    sNode* node = _alloc_node(list);
    if (!node) return _alloc_node_error(list);
 
    memcpy(node->data, value, list->data_size);
 
    if (!list->tail) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail       = node;
    }
 
    list->len++;
    return NO_ERROR;
}
 
// ================================================================================
// push_front_slist
// ================================================================================
 
error_code_t push_front_slist(slist_t* list, const void* value) {
    if (!list || !value) return NULL_POINTER;
 
    sNode* node = _alloc_node(list);
    if (!node) return _alloc_node_error(list);
 
    memcpy(node->data, value, list->data_size);
    node->next = list->head;
    list->head = node;
 
    if (!list->tail)
        list->tail = node;
 
    list->len++;
    return NO_ERROR;
}
 
// ================================================================================
// push_at_slist
// ================================================================================
 
error_code_t push_at_slist(slist_t* list, size_t index, const void* value) {
    if (!list || !value)   return NULL_POINTER;
    if (index > list->len) return INVALID_ARG;
 
    if (index == 0)         return push_front_slist(list, value);
    if (index == list->len) return push_back_slist(list, value);
 
    sNode* node = _alloc_node(list);
    if (!node) return _alloc_node_error(list);
 
    memcpy(node->data, value, list->data_size);
 
    sNode* prev = list->head;
    for (size_t i = 0; i < index - 1u; i++)
        prev = prev->next;
 
    node->next = prev->next;
    prev->next = node;
    list->len++;
    return NO_ERROR;
}
 
// ================================================================================
// pop_front_slist
// ================================================================================
 
error_code_t pop_front_slist(slist_t* list, void* out_value) {
    if (!list)       return NULL_POINTER;
    if (!list->head) return NOT_FOUND;
 
    sNode* old_head = list->head;
 
    if (out_value)
        memcpy(out_value, old_head->data, list->data_size);
 
    list->head = old_head->next;
    if (!list->head)
        list->tail = NULL;
 
    _return_node(list, old_head);
    list->len--;
    return NO_ERROR;
}
 
// ================================================================================
// pop_back_slist
// ================================================================================
 
error_code_t pop_back_slist(slist_t* list, void* out_value) {
    if (!list)       return NULL_POINTER;
    if (!list->head) return NOT_FOUND;
 
    if (list->head == list->tail)
        return pop_front_slist(list, out_value);
 
    sNode* prev = list->head;
    while (prev->next != list->tail)
        prev = prev->next;
 
    sNode* old_tail = list->tail;
 
    if (out_value)
        memcpy(out_value, old_tail->data, list->data_size);
 
    prev->next = NULL;
    list->tail = prev;
 
    _return_node(list, old_tail);
    list->len--;
    return NO_ERROR;
}
 
// ================================================================================
// pop_at_slist
// ================================================================================
 
error_code_t pop_at_slist(slist_t* list, size_t index, void* out_value) {
    if (!list)              return NULL_POINTER;
    if (!list->head)        return NOT_FOUND;
    if (index >= list->len) return INVALID_ARG;
 
    if (index == 0)              return pop_front_slist(list, out_value);
    if (index == list->len - 1u) return pop_back_slist(list, out_value);
 
    sNode* prev = list->head;
    for (size_t i = 0; i < index - 1u; i++)
        prev = prev->next;
 
    sNode* target = prev->next;
 
    if (out_value)
        memcpy(out_value, target->data, list->data_size);
 
    prev->next = target->next;
 
    _return_node(list, target);
    list->len--;
    return NO_ERROR;
}
 
// ================================================================================
// clear_slist
// ================================================================================
 
error_code_t clear_slist(slist_t* list) {
    if (!list) return NULL_POINTER;
 
    sNode* node = list->head;
    while (node) {
        sNode* next = node->next;
        _return_node(list, node);
        node = next;
    }
 
    list->head            = NULL;
    list->tail            = NULL;
    list->len             = 0u;
    list->slab_used       = 0u;
    list->slab_free       = NULL;
    list->slab_free_count = 0u;
    list->overflow_live   = 0u;
 
    return NO_ERROR;
}
 
// ================================================================================
// copy_slist
// ================================================================================
 
slist_expect_t copy_slist(const slist_t* src, allocator_vtable_t alloc_v) {
    if (!src)
        return (slist_expect_t){ .has_value = false, .u.error = NULL_POINTER };
 
    /* Initialise a new list with the same slab capacity and settings. */
    slist_expect_t r = init_slist(src->slab_cap, src->data_size, src->dtype,
                                  src->allow_overflow, alloc_v);
    if (!r.has_value) return r;
 
    slist_t* dst  = r.u.value;
    sNode*   node = src->head;
 
    while (node) {
        error_code_t err = push_back_slist(dst, node->data);
        if (err != NO_ERROR) {
            return_slist(dst);
            return (slist_expect_t){ .has_value = false, .u.error = err };
        }
        node = node->next;
    }
 
    return (slist_expect_t){ .has_value = true, .u.value = dst };
}
 
// ================================================================================
// contains_slist
// ================================================================================
 
slist_index_expect_t contains_slist(const slist_t* list, const void* needle) {
    if (!list || !needle)
        return (slist_index_expect_t){ .has_value = false, .u.error = NULL_POINTER };
 
    sNode* node = list->head;
    for (size_t i = 0; node; i++, node = node->next) {
        if (memcmp(node->data, needle, list->data_size) == 0)
            return (slist_index_expect_t){ .has_value = true, .u.value = i };
    }
 
    return (slist_index_expect_t){ .has_value = false, .u.error = NOT_FOUND };
}
 
// ================================================================================
// reverse_slist
// ================================================================================
 
error_code_t reverse_slist(slist_t* list) {
    if (!list) return NULL_POINTER;
 
    /* Nothing to do for an empty list or a single node. */
    if (list->len <= 1) return NO_ERROR;
 
    sNode* prev     = NULL;
    sNode* current  = list->head;
    sNode* new_tail = list->head;   /* head becomes tail after reversal */
 
    while (current) {
        sNode* next   = current->next;
        current->next = prev;
        prev          = current;
        current       = next;
    }
 
    list->head = prev;      /* prev is the old tail, now the new head */
    list->tail = new_tail;
    return NO_ERROR;
}
 
// ================================================================================
// concat_slist
// ================================================================================
 
error_code_t concat_slist(slist_t* dst, const slist_t* src) {
    if (!dst || !src) return NULL_POINTER;
    if (dst->data_size != src->data_size) return INVALID_ARG;
 
    sNode* node = src->head;
    while (node) {
        error_code_t err = push_back_slist(dst, node->data);
        if (err != NO_ERROR) return err;
        node = node->next;
    }
 
    return NO_ERROR;
}
 
// ================================================================================
// foreach_slist
// ================================================================================
 
error_code_t foreach_slist(const slist_t* list,
                           slist_iter_fn  fn,
                           void*          user_data) {
    if (!list || !fn) return NULL_POINTER;
 
    sNode* node = list->head;
    for (size_t i = 0; node; i++, node = node->next)
        fn(node->data, i, user_data);
 
    return NO_ERROR;
} 
// ================================================================================
// ================================================================================
// eof
