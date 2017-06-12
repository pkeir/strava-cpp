
#pragma once

#include <string>
#include <vector>
#include <time.h>

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
        bool primary;
        double distance;
        int resource_state;
        std::string id;
        std::string name;
    };

    struct club
    {
        int id;
        int resource_state;
        int member_count;

        std::string name;
        std::string profile_medium;
        std::string profile;
        std::string cover_photo;
        std::string cover_photo_small;
        std::string sport_type;
        std::string city;
        std::string state;
        std::string country;
        std::string url;

        bool is_private;
        bool featured;       
    };

    struct shoe
    {
        bool primary;
        double distance;
        int resource_state;
        std::string id;
        std::string name;
    };

    struct athlete
    {
        int id;
        int resource_state;
        int follower_count;
        int friend_count;
        int mutual_friend_count;
        int athlete_type;
        int ftp;
        int weight;

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
        std::string date_preference;
        std::string measurement_preference;
        std::string email;

        time_t created_at;
        time_t updated_at;

        std::vector<club> clubs;
        std::vector<bike> bikes;
        std::vector<shoe> shoes;

        bool premium;
    };

    namespace athletes
    {
        void current(athlete& out);
    }

    void authenticate(oauth&& info, bool skip_init = false);
}