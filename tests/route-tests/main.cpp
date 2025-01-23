
#include <strava.hpp>
#include <iostream>
#include <gtest/gtest.h>

strava::oauth auth = {
    18035,
    "8a08050aaf532074ab06bdacf3297b3ecc86d640",
    "005ed679943cd3eee63861f595863cda58591b41"
};

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
