
#include <strava.hpp>
#include <iostream>
#include <lest.hpp>

strava::oauth auth = {
    18035,
    "8a08050aaf532074ab06bdacf3297b3ecc86d640",
    "005ed679943cd3eee63861f595863cda58591b41"
};

const lest::test specification[] =
{
    CASE("routes length test")
    {
        auto me = strava::athlete::current(auth);
        auto routes = strava::routes::list(auth, me.id);

        EXPECT(routes.size() > 0);
    },

    CASE("route name/desc test")
    {
        auto me = strava::athlete::current(auth);
        auto routes = strava::routes::list(auth, me.id);
        auto route = strava::routes::retrieve(auth, routes.front().id);
     
        EXPECT(!route.name.empty());
        EXPECT(!route.description.empty());
    },

    CASE("route id test")
    {
        auto me = strava::athlete::current(auth);
        auto routes = strava::routes::list(auth, me.id);
        auto route = strava::routes::retrieve(auth, routes.front().id);

        EXPECT(route.id != 0);
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}