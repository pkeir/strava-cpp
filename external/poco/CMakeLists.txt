include (ExternalProject)

ExternalProject_Add(POCO
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/poco
    GIT_REPOSITORY "https://github.com/pocoproject/poco"
    GIT_TAG "main"
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
    CMAKE_ARGS -DENABLE_NETSSL=ON
)

ExternalProject_Get_Property(POCO install_dir)
ExternalProject_Get_Property(POCO source_dir)
ExternalProject_Get_Property(POCO binary_dir)

set(POCO_INCLUDE ${source_dir} CACHE INTERNAL "")

if(WIN32)
  set(POCO_LIB_DIR "${binary_dir}/lib/Debug" CACHE INTERNAL "")
  set(prefix "")
  set(suffix ".lib")
  set(POCO_LIBS "")
else()
  set(POCO_LIB_DIR "${binary_dir}/lib" CACHE INTERNAL "")
  set(prefix "lib")
  set(suffix ".so")
  set(POCO_LIBS
    ${prefix}PocoNetSSL${suffix}
    ${prefix}PocoNet${suffix}
    ${prefix}PocoUtil${suffix}
    ${prefix}PocoJSON${suffix}
    ${prefix}PocoXML${suffix}
    ${prefix}PocoFoundation${suffix}
    CACHE INTERNAL ""
)
endif()

