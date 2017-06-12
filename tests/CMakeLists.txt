
include_directories(${LEST_INCLUDE})

# Add tests written with lest
add_executable(athlete-tests    athlete-tests/main.cpp)
add_executable(activity-tests   activity-tests/main.cpp)
add_executable(club-tests       club-tests/main.cpp)
add_executable(gear-tests       gear-tests/main.cpp)
add_executable(race-tests       race-tests/main.cpp)
add_executable(route-tests      route-tests/main.cpp)
add_executable(segment-tests    segment-tests/main.cpp)
add_executable(stream-tests     stream-tests/main.cpp)
add_executable(upload-tests     upload-tests/main.cpp)
add_executable(webhook-tests    webhook-tests/main.cpp)

# add to ctest
add_test(NAME athlete-tests     COMMAND athlete-tests)
add_test(NAME activity-tests    COMMAND activity-tests)
add_test(NAME club-tests        COMMAND club-tests)
add_test(NAME gear-tests        COMMAND gear-tests)
add_test(NAME race-tests        COMMAND race-tests)
add_test(NAME route-tests       COMMAND route-tests)
add_test(NAME segment-tests     COMMAND segment-tests)
add_test(NAME stream-tests      COMMAND stream-tests)
add_test(NAME upload-tests      COMMAND upload-tests)
add_test(NAME webhook-tests     COMMAND webhook-tests)
