include (ExternalProject)

ExternalProject_Add(POCO
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/poco 
    GIT_REPOSITORY "https://github.com/pocoproject/poco"
    GIT_TAG "master"
    INSTALL_COMMAND ""
    CMAKE_ARGS -DENABLE_NETSSL=ON -DPOCO_STATIC=ON
)

ExternalProject_Get_Property(POCO install_dir)
ExternalProject_Get_Property(POCO source_dir)
ExternalProject_Get_Property(POCO binary_dir)

set(POCO_INCLUDE ${source_dir} CACHE INTERNAL "")
set(POCO_LIB ${binary_dir}/lib/Debug CACHE INTERNAL "")