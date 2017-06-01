
include_directories(../include)

include_directories(${POCO_INCLUDE}/Foundation/include)
include_directories(${POCO_INCLUDE}/Util/include)
include_directories(${POCO_INCLUDE}/JSON/include)
include_directories(${POCO_INCLUDE}/NetSSL_OpenSSL/include)
include_directories(${POCO_INCLUDE}/Crypto/include)
include_directories(${POCO_INCLUDE}/Net/include)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
link_directories(${POCO_LIB})

add_executable(strava-app strava-app/main.cpp strava-app/main.hpp)

target_link_libraries(strava-app strava)