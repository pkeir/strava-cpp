
#include <strava.hpp>
#include <gtest/gtest.h>

strava::oauth auth = { };

TEST(ClubTest, Length)
{
    auto clubs = strava::clubs::list_athlete_clubs(auth);
    EXPECT_TRUE(clubs.size() > 0);
}

TEST(ClubTest, Meta)
{
    auto clubs = strava::clubs::list_athlete_clubs(auth);
    auto club = clubs.front();

    EXPECT_TRUE(club.id != 0);
    EXPECT_TRUE(club.resource_state != 0);
}

TEST(ClubTest, Name)
{
    auto clubs = strava::clubs::list_athlete_clubs(auth);
    auto club = clubs.front();

    EXPECT_TRUE(club.name.length() > 0);
}

TEST(ClubTest, Member)
{
    auto clubs = strava::clubs::list_athlete_clubs(auth);
    auto club = clubs.front();

    EXPECT_TRUE(club.member_count > 0);
}

TEST(ClubTest, Image)
{
    auto clubs = strava::clubs::list_athlete_clubs(auth);
    auto club = clubs.front();

    EXPECT_TRUE(!club.profile_medium.empty());
    EXPECT_TRUE(!club.profile.empty());

    EXPECT_TRUE(!club.cover_photo_small.empty());
    EXPECT_TRUE(!club.cover_photo.empty());
}

int main(int argc, char * argv[])
{
    return RUN_ALL_TESTS();
}
