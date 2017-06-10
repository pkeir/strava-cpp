
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

    struct athelete
    {
        std::string name;
    };

    namespace atheletes
    {
        void current(athelete& out);
    }

    void authenticate(oauth&& info, bool skip_init = false);
}