
include_directories(${LEST_INCLUDE})

add_executable(athlete-test test01/main.cpp)
add_test(NAME athlete-test COMMAND athlete-test )

add_executable(activities-test test01/main.cpp)
add_test(NAME activities-test COMMAND activities-test)
