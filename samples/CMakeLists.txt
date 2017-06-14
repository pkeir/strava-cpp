
# Lib Header
include_directories(../include)

# OpenSSL
include_directories(${OPENSSL_INCLUDE_DIR})

# Poco
include_directories(${POCO_INCLUDE}/Util/include)
include_directories(${POCO_INCLUDE}/Net/include)
include_directories(${POCO_INCLUDE}/NetSSL_OpenSSL/include)
include_directories(${POCO_INCLUDE}/Crypto/include)
include_directories(${POCO_INCLUDE}/JSON/include)
include_directories(${POCO_INCLUDE}/Foundation/include)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
link_directories(${POCO_LIB})

add_executable(dev dev/main.cpp)

target_link_libraries(dev ${STRAVA_LIB})