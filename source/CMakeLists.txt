
include_directories(../include)
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${POCO_INCLUDE}/Util/include)
include_directories(${POCO_INCLUDE}/Net/include)
include_directories(${POCO_INCLUDE}/NetSSL_OpenSSL/include)
include_directories(${POCO_INCLUDE}/Crypto/include)
include_directories(${POCO_INCLUDE}/JSON/include)
include_directories(${POCO_INCLUDE}/Foundation/include)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
link_directories(${POCO_LIB_DIR})

set(STRAVA_SRC strava.cpp ../include/strava.hpp)
set(POCO_LIBS
    Poco::NetSSL
    Poco::Net
    Poco::Util
    Poco::JSON
    Poco::XML
    Poco::Foundation
    CACHE INTERNAL ""
)
set (GTEST_LIBS GTest::Main GTest::GTest CACHE INTERNAL "")

add_library(STRAVA ${STRAVA_SRC})
#add_dependencies(STRAVA POCO)
add_dependencies(STRAVA ${POCO_LIBS} ${GTEST_LIBS})

set(STRAVA_LIB
    STRAVA
    ${OPENSSL_LIBRARIES}
    ${OPENSSL_SSL_LIBRARY}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${POCO_LIBS}
    ${GTEST_LIBS}
    CACHE INTERNAL ""
)

set(STRAVA_INC
    ../include
    ${LEST_INCLUDE}
    ${OPENSSL_INCLUDE_DIR}
    ${POCO_INCLUDE}/Util/include
    ${POCO_INCLUDE}/Net/include
    ${POCO_INCLUDE}/NetSSL_OpenSSL/include
    ${POCO_INCLUDE}/Crypto/include
    ${POCO_INCLUDE}/JSON/include
    ${POCO_INCLUDE}/Foundation/include
    CACHE INTERNAL ""
)
