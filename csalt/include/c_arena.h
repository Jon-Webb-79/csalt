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
// DATA STRUCTURS 

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
        ALLOC_INVALID = 0,
        STATIC = 1,
        DYNAMIC = 2
    } alloc_t;

#endif /*ALLOC_H*/
// ================================================================================ 
// ================================================================================ 
// INITIALIZE AND DEALLOCATE FUNCTIONS

Arena* init_dynamic_arena(size_t bytes, bool resize, size_t min_chunk_in, size_t padding_in);
// -------------------------------------------------------------------------------- 

Arena* init_static_arena(void* buffer, size_t bytes, size_t passing_in);
// -------------------------------------------------------------------------------- 

Arena* init_darena(size_t bytes, bool resize);
// -------------------------------------------------------------------------------- 

Arena* init_sarena(void* buffer, size_t bytes);
// -------------------------------------------------------------------------------- 

void free_arena(Arena* arena);
// ================================================================================ 
// ================================================================================ 
// ALLOCATION FUNCTIONS 

void* alloc_arena(Arena* arena, size_t bytes, bool zeroed);
// -------------------------------------------------------------------------------- 

void* alloc_arena_aligned(Arena* arena, size_t bytes, size_t alignment, bool zeroed);
// ================================================================================ 
// ================================================================================ 
// UTILITY FUNCTIONS 

bool is_arena_ptr(const Arena* arena, const void *ptr);
// -------------------------------------------------------------------------------- 

bool is_arena_ptr_sized(const Arena* arena, const void* ptr, size_t size);
// -------------------------------------------------------------------------------- 

void reset_arena(Arena* arena, bool trim_extra_chunks);
// -------------------------------------------------------------------------------- 

ArenaCheckPoint save_arena(const Arena* arena);
// -------------------------------------------------------------------------------- 

bool restore_arena(Arena *arena, ArenaCheckPoint cp);
// -------------------------------------------------------------------------------- 

void toggle_arena_resize(Arena* arena, bool toggle);
// ================================================================================ 
// ================================================================================ 
// GETTER FUNCTIONS 

size_t arena_remaining(const Arena* arena);
// -------------------------------------------------------------------------------- 

size_t arena_chunk_count(const Arena* arena);
// -------------------------------------------------------------------------------- 

alloc_t arena_mtype(const Arena* arena);
// -------------------------------------------------------------------------------- 

size_t arena_size(const Arena* arena);
// -------------------------------------------------------------------------------- 

size_t arena_alloc(const Arena* arena);
// -------------------------------------------------------------------------------- 

size_t total_arena_alloc(const Arena* arena);
// -------------------------------------------------------------------------------- 

size_t arena_alignment(Arena* arena);
// -------------------------------------------------------------------------------- 

size_t arena_min_chunk_size(Arena* arena);
// ================================================================================ 
// ================================================================================ 
// SETTER FUNCTIONS 

void toggle_arena_resize(Arena* arena, bool toggle);
// ================================================================================ 
// ================================================================================ 
// LOG FUNCTIONS 

bool arena_stats(const Arena* arena, char* buffer, size_t buffer_size);
// ================================================================================ 
// ================================================================================ 
// MACROS 

#if !defined(_NO_GENERICS__)
    #define arena_alloc_type(arena, type) \
        (type*)alloc_arena((arena), sizeof(type), false)
#endif 
#if !defined(__NO_GENERICS__)  
    #define arena_alloc_array(arena, type, count) \
        (type*)alloc_arena((arena), sizeof(type) * (count), false)
#endif
#if !defined(__NO_GENERICS__) 
    #define arena_alloc_type_zeroed(arena, type) \
        (type*)alloc_arena((arena), sizeof(type), true)
#endif
#if !defined(__NO_GENERICS__) 
    #define arena_alloc_array_zeroed(arena, type, count) \
        (type*)alloc_arena((arena), sizeof(type) * (count), true)
#endif 
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
// TODO: ADD FUNCTION TO TURN resize on 
// TODO: ADD FUNCTION TO TURN resize off
