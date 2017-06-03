
include_directories(../include)
include_directories(${OPENSSL_INCLUDE_DIR})

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_executable(helloworld helloworld/main.cpp)

target_link_libraries(helloworld strava)
target_link_libraries(helloworld ${OPENSSL_LIBRARIES})
target_link_libraries(helloworld ${OPENSSL_SSL_LIBRARY})
target_link_libraries(helloworld ${OPENSSL_CRYPTO_LIBRARY})

add_dependencies(helloworld LEST)