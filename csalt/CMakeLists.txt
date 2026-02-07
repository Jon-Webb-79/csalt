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

# --------------------------------------------------------------------
# Project
# --------------------------------------------------------------------
project(csalt VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)   # good for shared/static

# Build type default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# --------------------------------------------------------------------
# Options (scripts toggle these)
# --------------------------------------------------------------------
option(CSALT_BUILD_TESTS   "Build unit tests and unit_tests exe" OFF)
option(CSALT_BUILD_STATIC  "Also build a static library (in addition to shared if enabled)" OFF)

# Primary shared/static switch (shared default unless scripts override)
if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()

include(GNUInstallDirs)

# Uniform output layout for *all* targets (incl. tests)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# --------------------------------------------------------------------
# Sources / Headers
# --------------------------------------------------------------------
set(CSALT_SOURCES
  c_allocator.c
  c_string.c
  c_error.c
)

set(CSALT_PUBLIC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CSALT_PRIVATE_SIMD_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/simd)

# --------------------------------------------------------------------
# Main library target (type governed by BUILD_SHARED_LIBS)
# --------------------------------------------------------------------
add_library(csalt ${CSALT_SOURCES})
add_library(csalt::csalt ALIAS csalt)

target_include_directories(csalt
  PUBLIC
    $<BUILD_INTERFACE:${CSALT_PUBLIC_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/csalt>
  PRIVATE
    ${CSALT_PRIVATE_SIMD_DIR}
)

# Warnings / SIMD flags per compiler & config
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(csalt PRIVATE -Wall -Wextra -Wpedantic)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "csalt: Debug build with AVX/AVX2 disabled; SSE3/SSE4.1 allowed, no -Werror")
    target_compile_options(csalt PRIVATE
      -O0 -g -march=x86-64 -mno-avx -mno-avx2 -mno-fma -msse3 -msse4.1
    )
  else()
    message(STATUS "csalt: ${CMAKE_BUILD_TYPE} build with -O3 -march=native and -Werror")
    target_compile_options(csalt PRIVATE -O3 -march=native -Werror)
  endif()
elseif (MSVC)
  target_compile_options(csalt PRIVATE /W4)
  target_compile_definitions(csalt PRIVATE _CRT_SECURE_NO_WARNINGS)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(csalt PRIVATE /arch:AVX2 /WX)
  endif()
endif()

# libm on Linux
if(UNIX AND NOT APPLE)
  target_link_libraries(csalt PUBLIC m)
endif()

# SOVERSION/VERSION (for shared; harmless for static)
set_target_properties(csalt PROPERTIES
  VERSION   ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

# --------------------------------------------------------------------
# Optional additional static library
# --------------------------------------------------------------------
if(CSALT_BUILD_STATIC)
  add_library(csalt_static STATIC ${CSALT_SOURCES})
  add_library(csalt::csalt_static ALIAS csalt_static)
  set_target_properties(csalt_static PROPERTIES OUTPUT_NAME csalt)

  target_include_directories(csalt_static
    PUBLIC
      $<BUILD_INTERFACE:${CSALT_PUBLIC_INCLUDE_DIR}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/csalt>
    PRIVATE
      ${CSALT_PRIVATE_SIMD_DIR}
  )

  if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(csalt_static PRIVATE -Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(csalt_static PRIVATE
        -O0 -g -march=x86-64 -mno-avx -mno-avx2 -mno-fma -msse3 -msse4.1
      )
    else()
      target_compile_options(csalt_static PRIVATE -O3 -march=native -Werror)
    endif()
  elseif(MSVC)
    target_compile_options(csalt_static PRIVATE /W4)
    target_compile_definitions(csalt_static PRIVATE _CRT_SECURE_NO_WARNINGS)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(csalt_static PRIVATE /arch:AVX2 /WX)
    endif()
  endif()

  if(UNIX AND NOT APPLE)
    target_link_libraries(csalt_static PUBLIC m)
  endif()
endif()

# --------------------------------------------------------------------
# Installation (used by install.zsh / static.zsh; tests are OFF there)
# --------------------------------------------------------------------
install(TARGETS csalt
  EXPORT csaltTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(CSALT_BUILD_STATIC)
  install(TARGETS csalt_static
    EXPORT csaltTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
endif()

# Public headers → include/csalt/
install(DIRECTORY ${CSALT_PUBLIC_INCLUDE_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/csalt
  FILES_MATCHING PATTERN "*.h"
)

# CMake package config for find_package(csalt)
install(EXPORT csaltTargets
  FILE csaltTargets.cmake
  NAMESPACE csalt::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/csalt
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/csaltConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/csaltConfig.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/csaltTargets.cmake\")\n")

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/csaltConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/csaltConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/csalt
)

# --------------------------------------------------------------------
# Tests (only when debug.zsh asks for them)
# --------------------------------------------------------------------
if(CSALT_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)   # test/CMakeLists.txt must create 'unit_tests'
endif()
# ================================================================================
# ================================================================================
# eof
