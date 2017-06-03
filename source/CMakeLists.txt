
include_directories(../include)

set(SOURCE strava.cpp)

add_library(strava ${SOURCE})
#add_dependencies(strava POCO)