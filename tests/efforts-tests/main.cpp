
#include <strava.hpp>
#include <gtest/gtest.h>

strava::oauth auth = { };

TEST(EffortsTest, Length)
{
    auto me = strava::athlete::current(auth);
    auto routes = strava::routes::list(auth, me.id);
    auto route = strava::routes::retrieve(auth, routes.front().id);

    EXPECT_TRUE(route.segments.size() > 0);
}

TEST(EffortsTest, NameResourceState)
{
    auto me = strava::athlete::current(auth);
    auto routes = strava::routes::list(auth, me.id);
    auto route = strava::routes::retrieve(auth, routes.front().id);
    auto effort = route.segments.front();

    EXPECT_TRUE(effort.id != int{});
    EXPECT_TRUE(effort.resource_state != int{});
}

TEST(EffortsTest, Misc)
{
    auto me = strava::athlete::current(auth);
    auto routes = strava::routes::list(auth, me.id);
    auto route = strava::routes::retrieve(auth, routes.front().id);
    auto effort = strava::segment_efforts::retrieve(auth, route.segments.front().id);

    EXPECT_TRUE(effort.distance != float{});
    EXPECT_TRUE(!effort.name.empty());

    EXPECT_TRUE(effort.activity.id != int{});
    EXPECT_TRUE(effort.segment.id != int{});
    EXPECT_TRUE(effort.athlete.id != int{});
}

int main(int argc, char * argv[])
{
    return RUN_ALL_TESTS();
}
