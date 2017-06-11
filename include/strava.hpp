
#pragma once

#include <string>

namespace strava
{
    struct oauth
    {
        std::string access_token;
        std::string redirect_url;
        std::string client_secret;
        std::string client_id;
    };

    struct athlete
    {
        int id;
        int resource_state;
        std::string firstname;
        std::string lastname;
        std::string profile_medium;
        std::string profile;
    };

    namespace athletes
    {
        void current(athlete& out);
    }

    void authenticate(oauth&& info, bool skip_init = false);
}