include (ExternalProject)

ExternalProject_Add(CURL
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/curl 
    GIT_REPOSITORY "https://github.com/curl/curl"
    GIT_TAG "master"
    INSTALL_COMMAND ""
    BUILD_COMMAND ""
)