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
project(csalt)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Option for static build
option(BUILD_STATIC "Build static library" OFF)

# Set compiler flags
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic -O3 -march=native")
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")
    add_definitions(/wd4996 /D_CRT_SECURE_NO_WARNINGS)
endif()

# Platform-specific defines
if(UNIX AND NOT APPLE)
    add_definitions(-D_GNU_SOURCE)
elseif(APPLE)
    add_definitions(-D_DARWIN_C_SOURCE)
elseif(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601 -DWIN32_LEAN_AND_MEAN)
endif()

# Determine library type
set(LIB_TYPE STATIC)
if(NOT BUILD_STATIC)
    set(LIB_TYPE)
endif()

# Create library
add_library(csalt ${LIB_TYPE}
    c_string.c
    include/c_string.h
    c_float.c 
    include/c_float.h
)

target_include_directories(csalt
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(csalt PUBLIC m)

# Set output directories
if(BUILD_STATIC)
    set_target_properties(csalt PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
elseif(WIN32)
    set_target_properties(csalt PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    )
endif()

# Add tests if not building static
if(NOT BUILD_STATIC)
    enable_testing()
    add_subdirectory(test)
endif()
# ================================================================================
# ================================================================================
# eof
