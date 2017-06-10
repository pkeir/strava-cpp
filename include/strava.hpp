
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
        std::string name;
    };

    namespace athletes
    {
        void current(athlete& out);
    }

    void authenticate(oauth&& info, bool skip_init = false);
}