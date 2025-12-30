cmake_minimum_required(VERSION 3.13)

# Project configuration
project(amp-mcu-reference 
    VERSION 1.0.0
    DESCRIPTION "Vendor-neutral AMP reference implementation for dual-core MCUs"
    LANGUAGES C ASM
)

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Options
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Platform selection
set(AMP_PLATFORM "generic" CACHE STRING "Target platform (generic, rp2350)")
set_property(CACHE AMP_PLATFORM PROPERTY STRINGS generic rp2350)

message(STATUS "AMP Platform: ${AMP_PLATFORM}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# Include platform-specific toolchain if available
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/platforms/${AMP_PLATFORM}.cmake")
    include("${CMAKE_SOURCE_DIR}/cmake/platforms/${AMP_PLATFORM}.cmake")
endif()

# Compiler flags
add_compile_options(
    -Wall
    -Wextra
    -Werror
)

# Debug/Release specific flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
endif()

# Runtime library
add_subdirectory(runtime)

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
install(DIRECTORY runtime/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Summary
message(STATUS "")
message(STATUS "=== AMP MCU Reference Configuration ===")
message(STATUS "  Platform:        ${AMP_PLATFORM}")
message(STATUS "  Build Type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build Examples:  ${BUILD_EXAMPLES}")
message(STATUS "  C Compiler:      ${CMAKE_C_COMPILER}")
message(STATUS "  Install Prefix:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=======================================")
message(STATUS "")
