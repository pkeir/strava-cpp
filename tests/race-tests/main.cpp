
#include <strava.hpp>
#include <gtest/gtest.h>

strava::oauth auth = { };

TEST(RacesTest, Length)
{
    auto races = strava::races::list(auth);
    EXPECT_TRUE(races.size() > 0);
}

TEST(RacesTest, NameURL)
{
    auto races = strava::races::list(auth);
    auto first = strava::races::retrieve(auth, races.front().id);

    EXPECT_TRUE(!first.name.empty());
    EXPECT_TRUE(!first.url.empty());
}

TEST(RacesTest, Location)
{
    auto races = strava::races::list(auth);
    auto first = races.front();

    EXPECT_TRUE(!first.country.empty());
    EXPECT_TRUE(!first.city.empty());
}

TEST(RacesTest, MetaData)
{
    auto races = strava::races::list(auth);
    auto first = strava::races::retrieve(auth, races.front().id);

    EXPECT_TRUE(first.id != int{});
    EXPECT_TRUE(first.resource_state != int{});
}

int main(int argc, char * argv[])
{
    return RUN_ALL_TESTS();
}
