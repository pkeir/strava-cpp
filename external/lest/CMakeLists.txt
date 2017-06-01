include (ExternalProject)

set(LEST_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(LEST_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(LEST_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)

ExternalProject_Add(lest
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/lest 
    GIT_REPOSITORY "https://github.com/martinmoene/lest"
    GIT_TAG "master"
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
)

ExternalProject_Get_Property(lest install_dir)
ExternalProject_Get_Property(lest source_dir)

set(LEST_INCLUDE ${source_dir}/include/lest CACHE INTERNAL "")