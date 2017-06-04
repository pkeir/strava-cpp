include (ExternalProject)

ExternalProject_Add(CPR
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/cpr 
    GIT_REPOSITORY "https://github.com/whoshuu/cpr"
    GIT_TAG "master"
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(CPR install_dir)
ExternalProject_Get_Property(CPR source_dir)

set(CBR_LIB_PATH ${install_dir}/src/CPR-build/lib/Debug/)
set(CPR_INCLUDE ${source_dir}/include CACHE INTERNAL "")
set(CPR_LIBRARIES ${CBR_LIB_PATH}cpr.lib ${CBR_LIB_PATH}libcurl_imp.lib CACHE INTERNAL "")