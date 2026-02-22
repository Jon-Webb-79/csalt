// ================================================================================
// ================================================================================
// - File:    dtypes.h
// - Purpose: This file contains prototypes for data type registration functions
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 22, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here
#ifndef dtypes_H
#define dtypes_H

#include <stdint.h> 
#include <stdlib.h> 
#include <stdbool.h>
// ================================================================================ 
// ================================================================================ 

#ifdef __cplusplus
extern "C" {
#endif 
// ================================================================================ 
// ================================================================================ 

/**
 * @typedef dtype_id_t
 * @brief Unique identifier for a registered data type.
 *
 * A 32-bit unsigned integer used to identify data types throughout the library.
 * Built-in type IDs are defined as constants below. User-defined types must use
 * IDs >= USER_BASE_TYPE to avoid conflicts with built-in and reserved IDs.
 */
typedef uint32_t dtype_id_t;

/**
 * @defgroup builtin_types Built-in Type IDs
 * @brief Predefined type identifiers for common C data types.
 *
 * These IDs are registered automatically when init_dtype_registry() is called.
 * Do not reuse these values for user-defined types.
 * @{
 */
#define UNKNOWN_TYPE   0u   /**< Uninitialized or invalid type identifier.         */
#define FLOAT_TYPE     1u   /**< IEEE 754 single-precision floating point (float)  */
#define DOUBLE_TYPE    2u   /**< IEEE 754 double-precision floating point (double) */
#define LDOUBLE_TYPE   3u   /**< Extended precision floating point (long double)   */
#define CHAR_TYPE      4u   /**< Signed character (char)                           */
#define UCHAR_TYPE     5u   /**< Unsigned character (unsigned char)                */
#define INT8_TYPE      6u   /**< Signed 8-bit integer (int8_t)                    */
#define UINT8_TYPE     7u   /**< Unsigned 8-bit integer (uint8_t)                 */
#define INT16_TYPE     8u   /**< Signed 16-bit integer (int16_t)                  */
#define UINT16_TYPE    9u   /**< Unsigned 16-bit integer (uint16_t)               */
#define INT32_TYPE     10u  /**< Signed 32-bit integer (int32_t)                  */
#define UINT32_TYPE    11u  /**< Unsigned 32-bit integer (uint32_t)               */
#define INT64_TYPE     12u  /**< Signed 64-bit integer (int64_t)                  */
#define UINT64_TYPE    13u  /**< Unsigned 64-bit integer (uint64_t)               */
#define BOOL_TYPE      14u  /**< Boolean type (bool)                              */
#define SIZE_T_TYPE    15u  /**< Unsigned size type (size_t)                      */
/** @} */

/**
 * @defgroup reserved_types Reserved and User Type Ranges
 * @brief ID ranges reserved for future use and user-defined types.
 * @{
 */

/** @brief IDs 16u - 999u are reserved for future built-in type expansion.
 *         Do not use these values for user-defined types. */

/**
 * @brief First valid ID for user-defined types.
 *
 * User-defined type IDs must be >= USER_BASE_TYPE to avoid conflicts with
 * built-in and reserved IDs. Each user-defined ID must be unique within
 * the registry. Example:
 * @code
 * #define MY_VEC3_TYPE  (USER_BASE_TYPE + 1u)
 * #define MY_QUAT_TYPE  (USER_BASE_TYPE + 2u)
 * @endcode
 */
#define USER_BASE_TYPE  1000u

/**
 * @brief Maximum total number of types that can be registered.
 *
 * Applies to built-in and user-defined types combined. Built-in types consume
 * the first 16 slots on initialization, leaving (MAX_DTYPES - 16) slots
 * available for user-defined types. Registration will fail if this limit
 * is exceeded. Check available_dtype_slots() before registering if capacity
 * is a concern.
 */
#define MAX_DTYPES      128u
/** @} */
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Descriptor for a registered data type.
 *
 * Holds the identity, size, and name of a data type. Each data structure in
 * the library uses this descriptor to perform type-safe operations on
 * type-erased byte buffers. Built-in descriptors are registered automatically
 * by init_dtype_registry(). User-defined descriptors must be registered
 * explicitly via ensure_dtype_registered() or register_dtype() before use.
 *
 * Example of a user-defined descriptor:
 * @code
 * static const dtype_t vec3_desc = {
 *     USER_BASE_TYPE + 1u, sizeof(vec3_t), "vec3"
 * };
 * @endcode
 */
typedef struct {
    dtype_id_t  id;
    size_t      data_size;
    const char* name;
} dtype_t;
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Initialize the dtype registry and register all built-in types.
 *
 * Must be called before any other registry function. Safe to call multiple
 * times — subsequent calls return true immediately without re-registering
 * built-in types. All data structure init functions in this library call
 * this function internally, so explicit calls are only necessary if the
 * registry is needed before any data structure is initialized.
 *
 * @return true  Registry initialized successfully and all built-in types registered.
 * @return false Initialization failed; registry should be considered unusable.
 */
bool          init_dtype_registry(void);
// -------------------------------------------------------------------------------- 

/**
 * @brief Register a new data type in the dtype registry.
 *
 * Adds a dtype_t descriptor to the registry so it can be used with any
 * data structure in the library. Registration fails if the ID is already
 * taken, the registry is full, or the descriptor is invalid. Prefer
 * ensure_dtype_registered() over this function when the type may have
 * already been registered by a prior call.
 *
 * @param desc Pointer to a dtype_t descriptor. Must not be NULL.
 *             desc->id must be != UNKNOWN_TYPE and not already registered.
 *             desc->data_size must be > 0.
 *             desc->name should be a valid non-NULL string for debugging.
 *
 * @return true  Type registered successfully.
 * @return false Registration failed. Possible reasons:
 *               - desc is NULL
 *               - desc->data_size is 0
 *               - desc->id is already registered
 *               - Registry is full (see available_dtype_slots())
 */
bool          register_dtype(const dtype_t* desc);
// -------------------------------------------------------------------------------- 

/**
 * @brief Look up a registered type descriptor by ID.
 *
 * Searches the registry for a dtype_t with the given ID. Returns a pointer
 * to the internal descriptor if found. The returned pointer is valid for
 * the lifetime of the registry and must not be modified or freed by the caller.
 *
 * @param id The dtype_id_t value to search for.
 *
 * @return Pointer to the matching dtype_t descriptor if found.
 * @return NULL if no type with the given ID has been registered.
 */
const dtype_t* lookup_dtype(dtype_id_t id);
// -------------------------------------------------------------------------------- 

/**
 * @brief Ensure a type is registered, initializing the registry if needed.
 *
 * Combines init_dtype_registry(), duplicate detection, and register_dtype()
 * into a single safe call. This is the preferred registration function for
 * typed wrapper init functions, as it is idempotent — calling it multiple
 * times with the same descriptor is safe and has no side effects after the
 * first successful registration.
 *
 * Typical usage in a typed init function:
 * @code
 * static const dtype_t float_desc = { FLOAT_TYPE, sizeof(float), "float" };
 *
 * float_array_t* init_float_array(size_t indices, bool growth) {
 *     if (ensure_dtype_registered(&float_desc) == false) return NULL;
 *     return init_array(indices, FLOAT_TYPE, growth);
 * }
 * @endcode
 *
 * @param desc Pointer to a dtype_t descriptor. Must not be NULL.
 *
 * @return true  Type is registered and ready for use (either was already
 *               registered or was just registered successfully).
 * @return false Registration failed or registry could not be initialized.
 */
bool          ensure_dtype_registered(const dtype_t* desc);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of remaining slots available in the dtype registry.
 *
 * The total registry capacity is MAX_DTYPES. Built-in types registered by
 * init_dtype_registry() consume slots from this total. This function can be
 * used to check available capacity before attempting to register user-defined
 * types, particularly in applications that register many custom types.
 *
 * @return Number of unused registration slots remaining.
 *         Returns 0 if the registry is full.
 */
size_t        available_dtype_slots(void);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* dtypes_H */
// ================================================================================
// ================================================================================
// eof
