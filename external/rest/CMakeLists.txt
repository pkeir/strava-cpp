
include (ExternalProject)

ExternalProject_Add(REST
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/rest 
    GIT_REPOSITORY "https://github.com/Corvusoft/restbed"
    GIT_TAG "7f7d8030bdb3e0a606fb85267731d12d0fa53cd7"
    INSTALL_COMMAND ""
    CMAKE_ARGS -DBUILD_TESTS=NO -DBUILD_EXAMPLES=NO -DBUILD_SSL=NO
)

ExternalProject_Get_Property(REST install_dir)
ExternalProject_Get_Property(REST source_dir)

set(REST_INCLUDE ${source_dir}/source CACHE INTERNAL "")
set(REST_LIBRARIES ${install_dir}/src/REST-BUILD/Debug/restbed.lib CACHE INTERNAL "")

message(${install_dir}/src/REST-BUILD/Debug/restbed.lib)
