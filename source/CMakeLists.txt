
include_directories(../include)
include_directories(${JSON_INCLUDE})

set(SOURCE strava.cpp)

add_library(STRAVA ${SOURCE})