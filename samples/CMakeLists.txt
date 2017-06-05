
include_directories(../include)
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${JSON_INCLUDE})

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# JSON Example
add_executable(json-sample json/main.cpp)
add_dependencies(json-sample JSON)

# CURL Example
add_executable(curl-sample curl/main.cpp)

target_link_libraries(curl-sample strava)
target_link_libraries(curl-sample ${OPENSSL_LIBRARIES})
target_link_libraries(curl-sample ${OPENSSL_SSL_LIBRARY})
target_link_libraries(curl-sample ${OPENSSL_CRYPTO_LIBRARY})

add_dependencies(curl-sample JSON)