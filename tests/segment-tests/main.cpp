
#include <strava.hpp>
#include <iostream>
#include <lest.hpp>

strava::oauth auth = 
{
    18035,
    "8a08050aaf532074ab06bdacf3297b3ecc86d640",
    "005ed679943cd3eee63861f595863cda58591b41"
};

strava::segments::bounds area = 
{
    37.821362, -122.505373,
    37.842038, -122.465977
};

const lest::test specification[] =
{
    CASE("segments length test")
    {
        auto segments = strava::segments::explore(auth, area);

        EXPECT(segments.size() > 0);
    },

    CASE("segment metadata test")
    {
        auto segments = strava::segments::explore(auth, area);
        auto segment = segments.front();

        EXPECT(segment.id != int{});
        EXPECT(segment.resource_state != int{});
    },

    CASE("segment name test")
    {
        auto segments = strava::segments::explore(auth, area);
        auto segment = segments.front();

        EXPECT(!segment.name.empty());
    },

    CASE("segment latlng test")
    {
        auto segments = strava::segments::explore(auth, area);
        auto segment = segments.front();

        EXPECT(segment.start_latlng[0] != float{});
        EXPECT(segment.start_latlng[1] != float{});

        EXPECT(segment.end_latlng[0] != float{});
        EXPECT(segment.end_latlng[1] != float{});
    },

    CASE("segment map test")
    {
        auto segments = strava::segments::explore(auth, area);
        auto segment = strava::segments::retrieve(auth, segments.front().id);

        EXPECT(segment.map.resource_state != int{});
        EXPECT(!segment.map.id.empty());
        EXPECT(!segment.map.polyline.empty());
    },

    CASE("segment distance test")
    {
        auto segments = strava::segments::explore(auth, area);
        auto segment = segments.front();

        EXPECT(segment.distance != float{});
    },
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}
