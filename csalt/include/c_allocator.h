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

// Compile for static or dynamic memory allocation for MISRA compliance
#ifndef ARENA_ENABLE_DYNAMIC
#  ifdef STATIC_ONLY
#    define ARENA_ENABLE_DYNAMIC 0
#  else
#    define ARENA_ENABLE_DYNAMIC 1
#  endif
#endif

#if defined(STATIC_ONLY) && defined(ARENA_ENABLE_DYNAMIC) && (ARENA_ENABLE_DYNAMIC+0)!=0
#  error "STATIC_ONLY set but ARENA_ENABLE_DYNAMIC != 0"
#endif

/* ================= Convenience macro switch =================
   Define -DARENA_USE_CONVENIENCE_MACROS=0 (or -DNO_ARENA_MACROS)
   to prevent these function-like macros from being defined. */
#ifndef ARENA_USE_CONVENIENCE_MACROS
#  ifdef NO_FUNCITON_MACROS
#    define ARENA_USE_CONVENIENCE_MACROS 0
#  else
#    define ARENA_USE_CONVENIENCE_MACROS 1
#  endif
#endif

/* Sanity: if someone set both, ensure consistency */
#if defined(NO_ARENA_MACROS) && (ARENA_USE_CONVENIENCE_MACROS+0)!=0
#  error "NO_ARENA_MACROS set but ARENA_USE_CONVENIENCE_MACROS != 0"
#endif
// ================================================================================ 
// ================================================================================ 

#include <stdlib.h>  // size_t 
#include <stdbool.h> // true and false
#include <stdint.h>  // unit8_t
#include <errno.h>   // errno
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 
// DATA STRUCTURS 

// Forward declarations for opaque data types
typedef struct arena_t arena_t;
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

#if ARENA_ENABLE_DYNAMIC
arena_t* init_dynamic_arena(size_t bytes, bool resize, size_t min_chunk_in, size_t padding_in);
#endif
// -------------------------------------------------------------------------------- 

arena_t* init_static_arena(void* buffer, size_t bytes, size_t passing_in);
// -------------------------------------------------------------------------------- 

#if ARENA_ENABLE_DYNAMIC
arena_t* init_darena(size_t bytes, bool resize);
#endif

// Stubs to return error values if user calls functions with DYNAMIC MEMORY allocation turned off
#if !ARENA_ENABLE_DYNAMIC
Arena* init_dynamic_arena(size_t, bool, size_t, size_t) { errno = ENOTSUP; return NULL; }
Arena* init_darena(size_t, bool) { errno = ENOTSUP; return NULL; }
void toggle_arena_resize(Arena*, bool) { errno = ENOTSUP; }
#endif

// -------------------------------------------------------------------------------- 

arena_t* init_sarena(void* buffer, size_t bytes);
// -------------------------------------------------------------------------------- 

void free_arena(arena_t* arena);
// ================================================================================ 
// ================================================================================ 
// ALLOCATION FUNCTIONS 

void* alloc_arena(arena_t* arena, size_t bytes, bool zeroed);
// ================================================================================ 
// ================================================================================ 
// UTILITY FUNCTIONS 

bool is_arena_ptr(const arena_t* arena, const void *ptr);
// -------------------------------------------------------------------------------- 

bool is_arena_ptr_sized(const arena_t* arena, const void* ptr, size_t size);
// -------------------------------------------------------------------------------- 

void reset_arena(arena_t* arena, bool trim_extra_chunks);
// -------------------------------------------------------------------------------- 

ArenaCheckPoint save_arena(const arena_t* arena);
// -------------------------------------------------------------------------------- 

bool restore_arena(arena_t *arena, ArenaCheckPoint cp);
// ================================================================================ 
// ================================================================================ 
// GETTER FUNCTIONS 

size_t arena_remaining(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_chunk_count(const arena_t* arena);
// -------------------------------------------------------------------------------- 

alloc_t arena_mtype(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_size(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_alloc(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t total_arena_alloc(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_alignment(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_min_chunk_size(const arena_t* arena);
// ================================================================================ 
// ================================================================================ 
// SETTER FUNCTIONS 

#if ARENA_ENABLE_DYNAMIC
void toggle_arena_resize(arena_t* arena, bool toggle);
#endif
// ================================================================================ 
// ================================================================================ 
// LOG FUNCTIONS 

bool arena_stats(const arena_t* arena, char* buffer, size_t buffer_size);
// ================================================================================ 
// ================================================================================ 
// MACROS 

#if ARENA_USE_CONVENIENCE_MACROS
    #define arena_alloc_type(arena, type) \
        (type*)alloc_arena((arena), sizeof(type), false)
    #define arena_alloc_array(arena, type, count) \
        (type*)alloc_arena((arena), sizeof(type) * (count), false)
    #define arena_alloc_type_zeroed(arena, type) \
        (type*)alloc_arena((arena), sizeof(type), true)
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
