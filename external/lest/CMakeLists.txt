include (ExternalProject)

ExternalProject_Add(LEST
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/lest 
    GIT_REPOSITORY "https://github.com/martinmoene/lest"
    GIT_TAG "master"
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
    CMAKE_ARGS -DLEST_BUILD_TEST=OFF -DLEST_BUILD_EXAMPLE=OFF -DLEST_BUILD_CONTRIB=OFF
)

ExternalProject_Get_Property(LEST install_dir)
ExternalProject_Get_Property(LEST source_dir)

set(LEST_INCLUDE ${source_dir}/include/lest CACHE INTERNAL "")