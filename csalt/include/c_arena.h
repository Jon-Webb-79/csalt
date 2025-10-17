// ================================================================================
// ================================================================================
// - File:    arena.h
// - Purpose: This file contains prototypes for generalized C allocators
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 15, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef arena_H
#define arena_H
// ================================================================================ 
// ================================================================================ 

#include <stdlib.h>  // size_t 
#include <stdbool.h> // true and false
#include <stdint.h>  // unit8_t
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 
// HIGH-LEVEL DATA STRUCTURS 

// Forward declarations for opaque data types
typedef struct Arena Arena;
typedef struct Chunk Chunk;
// -------------------------------------------------------------------------------- 

typedef struct {
    uintptr_t _priv[3];
} ArenaCheckPoint;
// -------------------------------------------------------------------------------- 

#ifndef ALLOC_H 
#define ALLOC_H 

    typedef enum {
        ALLOC_INVALID = -1,
        STATIC = 0,
        DYNAMIC = 1
    } alloc_t;

#endif /*ALLOC_H*/
// ================================================================================ 
// ================================================================================ 
// FUNCTION PROTOTYPES 

Arena* init_dynamic_arena(size_t bytes, bool resize);
// -------------------------------------------------------------------------------- 

Arena* init_static_arena(void* buffer, size_t bytes);
// -------------------------------------------------------------------------------- 

void set_default_arena_alignment(size_t alignment);
// -------------------------------------------------------------------------------- 

size_t default_arena_alignment(void);
// -------------------------------------------------------------------------------- 

void reset_default_arena_alignment(void);
// -------------------------------------------------------------------------------- 

void* alloc_arena(Arena* arena, size_t bytes, bool zeroed);
// -------------------------------------------------------------------------------- 

void* alloc_arena_aligned(Arena* arena, size_t bytes, size_t alignment, bool zeroed);
// -------------------------------------------------------------------------------- 

void free_arena(Arena* arena);
// -------------------------------------------------------------------------------- 

bool is_arena_ptr(const Arena* arena, const void *ptr);
// -------------------------------------------------------------------------------- 

bool is_arena_ptr_sized(const Arena* arena, const void* ptr, size_t size);
// -------------------------------------------------------------------------------- 

size_t arena_size(const Arena* arena);
// -------------------------------------------------------------------------------- 

size_t arena_alloc(const Arena* arena);
// -------------------------------------------------------------------------------- 

size_t total_arena_alloc(const Arena* arena);
// -------------------------------------------------------------------------------- 

alloc_t arena_mtype(const Arena* arena);
// -------------------------------------------------------------------------------- 

void reset_arena(Arena* arena, bool trim_extra_chunks);
// -------------------------------------------------------------------------------- 

ArenaCheckPoint save_arena(const Arena* arena);
// -------------------------------------------------------------------------------- 

bool restore_arena(Arena *arena, ArenaCheckPoint cp);
// -------------------------------------------------------------------------------- 

bool arena_stats(const Arena* arena, char* buffer, size_t buffer_size);
// -------------------------------------------------------------------------------- 

size_t arena_remaining(const Arena* arena);
// -------------------------------------------------------------------------------- 

size_t arena_chunk_count(const Arena* arena);
// ================================================================================ 
// ================================================================================ 
// MACROS 

#define arena_alloc_type(arena, type) \
    (type*)alloc_arena((arena), sizeof(type), false)

#define arena_alloc_array(arena, type, count) \
    (type*)alloc_arena((arena), sizeof(type) * (count), false)

#define arena_alloc_type_zeroed(arena, type) \
    (type*)alloc_arena((arena), sizeof(type), true)

#define arena_alloc_array_zeroed(arena, type, count) \
    (type*)alloc_arena_zeroed((arena), sizeof(type) * (count), true)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* arena_H */
// ================================================================================
// ================================================================================
// eof
//
// TODO: ADD COP_ARENA
// TODO: ADD RESET_MIM_CHUNK 
// TODO: ADD FUNCTION TO TURN resize on 
// TODO: ADD FUNCTION TO TURN resize off
