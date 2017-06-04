
include (ExternalProject)

ExternalProject_Add(JSON
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/json 
    GIT_REPOSITORY "https://github.com/miloyip/rapidjson/"
    GIT_TAG "master"
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
)

ExternalProject_Get_Property(JSON install_dir)
ExternalProject_Get_Property(JSON source_dir)

# Header only library
set(JSON_INCLUDE ${source_dir}/include CACHE INTERNAL "")