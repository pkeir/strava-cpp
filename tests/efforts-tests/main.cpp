
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
    CASE("effort test")
    {
        auto segments = strava::segments::explore(auth, area);

        EXPECT(segments.size() > 0);
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}