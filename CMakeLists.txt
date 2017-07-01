# Minimum version
cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

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
set(CMAKE_MACOSX_RPATH 1)
set(ENABLE_TESTS true)

set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_STANDARD 14)

# Project Options
option(ENABLE_SAMPLES "Enable Samples" ON)
option(ENABLE_TESTS "Enable Tests" ON)

# Sub directories
add_subdirectory(external/lest)
add_subdirectory(external/poco)
add_subdirectory(source)

# Add Tests
if(ENABLE_TESTS)
    include(CTest)
    add_subdirectory(tests)
    message(STATUS "Including Tests")
endif(ENABLE_TESTS)