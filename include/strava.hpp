
#pragma once

#include <string>
#include <vector>

namespace strava
{
    struct oauth
    {
        std::string access_token;
        std::string redirect_url;
        std::string client_secret;
        std::string client_id;
    };

    struct bike
    {
        std::string id;
        bool primary;
        std::string name;
        double distance;
        int resource_state;
    };

    struct club
    {
        int id;
        int resource_state;
        std::string name;
        std::string profile_medium;
        std::string profile;
        std::string cover_photo;
        std::string cover_photo_small;
        std::string sport_type;
        std::string city;
        std::string state;
        std::string country;
        bool is_private;
        int member_count;
        bool featured;
        std::string url;
    };

    struct shoe
    {
        std::string id;
        bool primary;
        std::string name;
        double distance;
        int resource_state;
    };

    struct athlete
    {
        int id;
        int resource_state;
        std::string firstname;
        std::string lastname;
        std::string profile_medium;
        std::string profile;
        std::string city;
        std::string state;
        std::string country;
        std::string sex;
        std::string is_friend;
        std::string follower;
        bool premium;
        time_t created_at;
        time_t updated_at;
        int follower_count;
        int friend_count;
        int mutual_friend_count;
        int athlete_type;
        std::string date_preference;
        std::string measurement_preference;
        std::string email;
        int ftp;
        int weight;

        std::vector<club> clubs;
        std::vector<bike> bikes;
        std::vector<shoe> shoes;
    };

    namespace athletes
    {
        void current(athlete& out);
    }

    void authenticate(oauth&& info, bool skip_init = false);
}