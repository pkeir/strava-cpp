
include (ExternalProject)

ExternalProject_Add(REST
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/rest 
    GIT_REPOSITORY "https://github.com/Corvusoft/restbed"
    GIT_TAG "master"
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(REST install_dir)
ExternalProject_Get_Property(REST source_dir)

set(REST_INCLUDE ${source_dir}/include/rest CACHE INTERNAL "")