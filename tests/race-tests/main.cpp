
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
    CASE("races length test")
    {
        auto races = strava::races::list(auth);
        EXPECT(races.size() > 0);
    },

    CASE("race name & url test")
    {
        auto races = strava::races::list(auth);
        auto first = strava::races::retrieve(auth, races.front().id);

        EXPECT(!first.name.empty());
        EXPECT(!first.url.empty());
    },

    CASE("race location test")
    {
        auto races = strava::races::list(auth);
        auto first = races.front();

        EXPECT(!first.country.empty());
        EXPECT(!first.city.empty());
    },

    CASE("race metadata test")
    {
        auto races = strava::races::list(auth);
        auto first = strava::races::retrieve(auth, races.front().id);

        EXPECT(first.id != 0);
        EXPECT(first.resource_state != 0);
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}