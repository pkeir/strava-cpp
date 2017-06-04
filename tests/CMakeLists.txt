
include_directories(${LEST_INCLUDE})

add_executable(test01 test01/main.cpp)
add_test(NAME test01 COMMAND test01)
