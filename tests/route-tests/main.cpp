
#include <strava.hpp>
#include <gtest/gtest.h>

strava::oauth auth = { };

TEST(RouteTest, Length)
{
    auto me = strava::athlete::current(auth);
    auto routes = strava::routes::list(auth, me.id);

    EXPECT_TRUE(routes.size() > 0);
}

TEST(RouteTest, NameDesc)
{
    auto me = strava::athlete::current(auth);
    auto routes = strava::routes::list(auth, me.id);
    auto route = strava::routes::retrieve(auth, routes.front().id);

    EXPECT_TRUE(!route.name.empty());
    EXPECT_TRUE(!route.description.empty());
}

TEST(RouteTest, ID)
{
    auto me = strava::athlete::current(auth);
    auto routes = strava::routes::list(auth, me.id);
    auto route = strava::routes::retrieve(auth, routes.front().id);

    EXPECT_TRUE(route.id != int{});
}

int main(int argc, char * argv[])
{
    return RUN_ALL_TESTS();
}
