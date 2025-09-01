# ================================================================================
# ================================================================================
# - File:    CMakeLists.txt
# - Purpose: Primary CMake file for C build system
#
# Source Metadata
# - Author:  Jonathan A. Webb
# - Date:    December 30, 2024
# - Version: 1.0
# - Copyright: Copyright 2024, Jonathan A. Webb Inc.
# ================================================================================
# ================================================================================

cmake_minimum_required(VERSION 3.31.3)
project(csalt C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

option(BUILD_STATIC "Build static library" OFF)

# Create library
add_library(csalt
  c_string.c
  c_float.c
  c_double.c
  c_ldouble.c
  c_int.c
  # (you can list headers too, but it's optional)
)

# Make public headers visible to dependents; keep simd private to this target
target_include_directories(csalt
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/simd
)

# Compiler/options per target (cleaner than global CMAKE_C_FLAGS)
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  # Common warnings
  target_compile_options(csalt PRIVATE -Wall -Werror -Wpedantic)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring Debug build with AVX/AVX2 disabled; SSE2+ ok")
    # x86-64 implies SSE2; you also enable SSE3/SSE4.1 explicitly here
    target_compile_options(csalt PRIVATE
      -O0 -g -march=x86-64 -mno-avx -mno-avx2 -mno-fma -msse3 -msse4.1
    )
  else()
    message(STATUS "Configuring Release/custom build with SIMD enabled (native)")
    target_compile_options(csalt PRIVATE -O3 -march=native)
  endif()

elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(csalt PRIVATE /W4 /WX)
  target_compile_definitions(csalt PRIVATE _CRT_SECURE_NO_WARNINGS)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # SSE2 is default on x64; explicitly disable AVX2 if you want parity with GCC/Clang debug
    # MSVC has no /arch:NOAVX2; if you must ensure no AVX2 codegen, build for x86 or avoid AVX2 code paths.
  else()
    # Enable AVX2 in Release (x64)
    target_compile_options(csalt PRIVATE /arch:AVX2)
  endif()
endif()

# Link libm on Unix (no-op on MSVC)
if(NOT WIN32)
  target_link_libraries(csalt PUBLIC m)
endif()

# Choose static/shared output style
if(BUILD_STATIC)
  set_target_properties(csalt PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

# Tests
if(NOT BUILD_STATIC)
  enable_testing()
  add_subdirectory(test)
endif()

# ================================================================================
# ================================================================================
# eof
