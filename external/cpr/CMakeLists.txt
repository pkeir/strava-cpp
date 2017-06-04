include (ExternalProject)

ExternalProject_Add(CPR
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/cpr 
    GIT_REPOSITORY "https://github.com/whoshuu/cpr"
    GIT_TAG "master"
    INSTALL_COMMAND ""
)