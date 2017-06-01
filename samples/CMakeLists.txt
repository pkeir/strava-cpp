
include_directories(../include)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_executable(sample-01 sample01/main.cpp)
target_link_libraries(sample-01 strava)