# ================================================================================
# ================================================================================
# - File:    CMakeLists.txt
# - Purpose: Primary CMake file for C build system
#
# Source Metadata
# - Author:  Jonathan A. Webb
# - Date:    October 20, 2023
# - Version: 1.0
# - Copyright: Copyright 2023, Jonathan A. Webb Inc.
# ================================================================================
# ================================================================================
cmake_minimum_required(VERSION 3.27.7)
project(csalt)

# Set the compiler (Optional)
# set(CMAKE_C_COMPILER "/usr/bin/gcc")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

# Set compiler flags
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic")
endif()

if(APPLE)
    # Check for M1 architecture
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        include_directories(/opt/homebrew/include)
        link_directories(/opt/homebrew/lib)
    else()
        # for Intel Macs or older versions of macOS, adjust as needed
        include_directories(/usr/local/include)
        link_directories(/usr/local/lib)
    endif()
endif()


# Define the library
add_library(csalt
	str.c
    print.c)
target_include_directories(csalt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define the tests
add_executable(unit_tests
	test/unit_test.c
	test/test_string.c)
target_link_libraries(unit_tests csalt cmocka)
add_test(NAME unit_tests COMMAND unit_test)

# ================================================================================
# ================================================================================
# eof
