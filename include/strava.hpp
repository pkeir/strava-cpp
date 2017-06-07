
#pragma once

#include <Poco/Net/Context.h>
#include <string>

namespace strava
{
    struct session
    {
        Poco::Net::Context::Ptr context;
    };

    struct auth_info
    {
        std::string redirect_url;
        std::string access_token;
        std::string client_secret;
        std::string client_id;
    };

    struct athelete
    {
        std::string name;
    };

    namespace athletes
    {
        void current(athelete& out);
    }

    void authenticate(std::string token);
    void setupSession();
}