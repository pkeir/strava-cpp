
include_directories(${STRAVA_INC})

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
link_directories(${POCO_LIB})

add_executable(athlete athlete/main.cpp)
add_executable(dev dev/main.cpp)

target_link_libraries(athlete ${STRAVA_LIB})
target_link_libraries(dev ${STRAVA_LIB})