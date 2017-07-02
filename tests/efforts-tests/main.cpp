
#include <strava.hpp>
#include <iostream>
#include <lest.hpp>

strava::oauth auth = 
{
    18035,
    "8a08050aaf532074ab06bdacf3297b3ecc86d640",
    "005ed679943cd3eee63861f595863cda58591b41"
};

const lest::test specification[] =
{
    CASE("efforts length test")
    {
        auto me = strava::athlete::current(auth);
        auto routes = strava::routes::list(auth, me.id);
        auto route = strava::routes::retrieve(auth, routes.front().id);

        EXPECT(route.segments.size() > 0);
    },

    CASE("efforts name/resource_state test")
    {
        auto me = strava::athlete::current(auth);
        auto routes = strava::routes::list(auth, me.id);
        auto route = strava::routes::retrieve(auth, routes.front().id);
        auto effort = route.segments.front();

        EXPECT(effort.id != 0);
        EXPECT(effort.resource_state != 0);
    },

    CASE("efforts misc test")
    {
        auto me = strava::athlete::current(auth);
        auto routes = strava::routes::list(auth, me.id);
        auto route = strava::routes::retrieve(auth, routes.front().id);
        auto effort = strava::segment_efforts::retrieve(auth, route.segments.front().id);

        EXPECT(effort.distance != float{});
        EXPECT(!effort.name.empty());

        EXPECT(effort.activity.id != int{});
        EXPECT(effort.segment.id != int{});
        EXPECT(effort.athlete.id != int{});
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}