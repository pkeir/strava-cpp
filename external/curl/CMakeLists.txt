
include (ExternalProject)

ExternalProject_Add(CURL
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/curl 
    GIT_REPOSITORY "https://github.com/curl/curl"
    GIT_TAG "master"
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
)

ExternalProject_Get_Property(CURL install_dir)
ExternalProject_Get_Property(CURL source_dir)
