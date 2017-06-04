
include_directories(../include)
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${JSON_INCLUDE})
include_directories(${CPR_INCLUDE})


link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Json Example
add_executable(json-sample json/main.cpp)

target_link_libraries(json-sample strava)
target_link_libraries(json-sample ${OPENSSL_LIBRARIES})
target_link_libraries(json-sample ${OPENSSL_SSL_LIBRARY})
target_link_libraries(json-sample ${OPENSSL_CRYPTO_LIBRARY})

add_dependencies(json-sample JSON)

# Http Example
add_executable(http-sample https/main.cpp)

target_link_libraries(http-sample strava)
target_link_libraries(http-sample ${OPENSSL_LIBRARIES})
target_link_libraries(http-sample ${OPENSSL_SSL_LIBRARY})
target_link_libraries(http-sample ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(http-sample ${CPR_LIBRARIES})

add_dependencies(http-sample JSON)
add_dependencies(json-sample CPR)