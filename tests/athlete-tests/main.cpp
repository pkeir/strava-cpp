
#include <strava.hpp>
#include <gtest/gtest.h>

strava::oauth auth = { };

TEST(AthleteTest, Name)
{
    auto me = strava::athlete::current(auth);

    EXPECT_TRUE(!me.firstname.empty());
    EXPECT_TRUE(!me.lastname.empty());
}

TEST(AthleteTest, Location)
{
    auto me = strava::athlete::current(auth);

    EXPECT_TRUE(!me.country.empty());
    EXPECT_TRUE(!me.state.empty());
}

TEST(AthleteTest, Image)
{
    auto me = strava::athlete::current(auth);

    EXPECT_TRUE(!me.profile_medium.empty());
    EXPECT_TRUE(!me.profile.empty());
}

TEST(AthleteTest, Date)
{
    auto me = strava::athlete::current(auth);

    EXPECT_TRUE(!me.created_at.time_string.empty());
    EXPECT_TRUE(me.created_at.time_epoch > 0);

    EXPECT_TRUE(!me.updated_at.time_string.empty());
    EXPECT_TRUE(me.updated_at.time_epoch > 0);
}

int main(int argc, char * argv[])
{
    return RUN_ALL_TESTS();
}
