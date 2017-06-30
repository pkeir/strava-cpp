
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
    CASE("Basic Zero Test")
    {
        EXPECT(0 == 0);
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}