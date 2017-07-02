
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
    CASE("activities length test")
    {
        auto activities = strava::activity::list(auth);
        EXPECT(activities.size() > 0);
    },

    CASE("comments length test")
    {
        auto activities = strava::activity::list(auth);
        auto activity = activities.front();
        auto comments = strava::activity::list_comments(auth, activity.id);

        EXPECT(comments.size() > 0);
    },

    CASE("activity test")
    {
        auto activities = strava::activity::list(auth);
        auto activity = activities.front();

        EXPECT(activity.resource_state != int{});
        EXPECT(activity.type != std::string{});
        EXPECT(activity.id != int{});
    },

    CASE("activity comment test")
    {
        auto activities = strava::activity::list(auth);
        auto activity = activities.front();
        auto comments = strava::activity::list_comments(auth, activity.id);
        auto first = comments.front();
        
        EXPECT(first.id != 0);
        EXPECT(first.resource_state != int{});
        EXPECT(first.resource_state != int{});
        EXPECT(first.text.length() > 0);

        EXPECT(first.athlete.id != int{});
        EXPECT(first.athlete.resource_state != int{};

        EXPECT(first.created_at.time_string.length() > 0);
        EXPECT(first.created_at.time_epoch > 0);
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}