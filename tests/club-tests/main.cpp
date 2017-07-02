
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
    CASE("club length test")
    {
        auto clubs = strava::clubs::list_athlete_clubs(auth);
        EXPECT(clubs.size() > 0);
    },

    CASE("club meta test")
    {
        auto clubs = strava::clubs::list_athlete_clubs(auth);
        auto club = clubs.front();

        EXPECT(club.id != 0);
        EXPECT(club.resource_state != 0);
    },

    CASE("club name test")
    {
        auto clubs = strava::clubs::list_athlete_clubs(auth);
        auto club = clubs.front();

        EXPECT(club.name.length() > 0);
    },

    CASE("club member test")
    {
        auto clubs = strava::clubs::list_athlete_clubs(auth);
        auto club = clubs.front();

        EXPECT(club.member_count > 0);
    },

    CASE("club image test")
    {
        auto clubs = strava::clubs::list_athlete_clubs(auth);
        auto club = clubs.front();

        EXPECT(!club.profile_medium.empty());
        EXPECT(!club.profile.empty());

        EXPECT(!club.cover_photo_small.empty());
        EXPECT(!club.cover_photo.empty());
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}