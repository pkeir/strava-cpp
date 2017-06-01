include (ExternalProject)

set(POCO_STATIC ON)

ExternalProject_Add(poco
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/poco 
    GIT_REPOSITORY "https://github.com/pocoproject/poco"
    GIT_TAG "master"
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(poco install_dir)
ExternalProject_Get_Property(poco source_dir)
ExternalProject_Get_Property(poco binary_dir)

set(POCO_INCLUDE ${source_dir} CACHE INTERNAL "")
set(POCO_LIB ${binary_dir}/lib/Debug CACHE INTERNAL "")
