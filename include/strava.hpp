
#pragma once

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/NetSSL.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SSLException.h>

#include <Poco/SharedPtr.h>
#include <Poco/StreamCopier.h>
#include <Poco/FileStream.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/URI.h>

#include <algorithm>
#include <sstream>
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