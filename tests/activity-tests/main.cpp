
#include <strava.hpp>
#include <string>
#include "gtest/gtest.h"

strava::oauth auth = { };

TEST(ActivityTest, Length)
{
    auto activities = strava::activity::list(auth);
    EXPECT_TRUE(activities.size() > 0);
}

TEST(ActivityTest, CommentLength)
{
    auto activities = strava::activity::list(auth);
    auto activity = activities.front();
    auto comments = strava::activity::list_comments(auth, activity.id);

    EXPECT_TRUE(comments.size() > 0);
}

TEST(ActivityTest, Activity)
{
    auto activities = strava::activity::list(auth);
    auto activity = activities.front();

    EXPECT_TRUE(activity.resource_state != int{});
    EXPECT_TRUE(activity.type != std::string{});
    EXPECT_TRUE(activity.id != int{});
}

TEST(ActivityTest, Comment)
{
    auto activities = strava::activity::list(auth);
    auto activity = activities.front();
    auto comments = strava::activity::list_comments(auth, activity.id);
    auto first = comments.front();

    EXPECT_TRUE(first.id != 0);
    EXPECT_TRUE(first.resource_state != int{});
    EXPECT_TRUE(first.resource_state != int{});
    EXPECT_TRUE(first.text.length() > 0);

    EXPECT_TRUE(first.athlete.id != int{});
    EXPECT_TRUE(first.athlete.resource_state != int{});

    EXPECT_TRUE(first.created_at.time_string.length() > 0);
    EXPECT_TRUE(first.created_at.time_epoch > 0);
}

int main(int argc, char * argv[])
{
    return RUN_ALL_TESTS();
}
