# Minimum version
cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

# Set project name
project (Strava)

# Dependencies
find_package(OpenSSL REQUIRED)

# Setting output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set values
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MACOSX_RPATH 1)
set(ENABLE_SAMPLES true)
set(ENABLE_TESTS true)

# Project Options
option(ENABLE_SAMPLES "Enable Samples" ON)
option(ENABLE_TESTS "Enable Tests" ON)

# ESub directories
add_subdirectory(external/lest)
add_subdirectory(external/poco)
add_subdirectory(source)

# Add Samples
if(ENABLE_SAMPLES)
    add_subdirectory(samples)
    message(STATUS "Including Samples")
endif(ENABLE_SAMPLES)

# Add Tests
if(ENABLE_TESTS)
    include(CTest)
    add_subdirectory(tests)
    message(STATUS "Including Tests")
endif(ENABLE_TESTS)
