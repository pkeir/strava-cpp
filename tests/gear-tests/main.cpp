
#include <strava.hpp>
#include <gtest/gtest.h>

strava::oauth auth = { };

TEST(GearTest, Length)
{
    auto me = strava::athlete::current(auth);
    EXPECT_TRUE(me.shoes.size() > 0);
}

TEST(GearTest, Name)
{
    auto me = strava::athlete::current(auth);
    auto shoes = me.shoes.front();

    EXPECT_TRUE(shoes.id.size() > 0);
    EXPECT_TRUE(shoes.name.size() > 0);
}

TEST(GearTest, Struct)
{
    auto me = strava::athlete::current(auth);
    auto shoes = strava::gear::retrieve(auth, me.shoes.front().id);

    EXPECT_TRUE(shoes.description.size() > 0);
    EXPECT_TRUE(shoes.brand_name.size() > 0);
    EXPECT_TRUE(shoes.model_name.size() > 0);
}

int main(int argc, char * argv[])
{
    return RUN_ALL_TESTS();
}
