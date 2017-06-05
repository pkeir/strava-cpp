
include_directories(../include)
include_directories(${JSON_INCLUDE})

set(SOURCE strava.cpp ../include/strava.hpp)

add_library(STRAVA ${SOURCE})