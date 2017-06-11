
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/NetSSL.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/HTTPSClientSession.h>
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
#include <strava.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sstream>

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::JSON;

Poco::Net::Context::Ptr session = nullptr;

strava::oauth authentication;

time_t to_time_t(std::string timestring, std::string format) 
{
    tm time;
    std::istringstream input(timestring);
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(&time, format.c_str());

    if (input.fail()) 
    {
        return 0;
    }

    return mktime(&time);
}

std::string pretty_time_t(time_t time)
{
    const int bufferSize = 20;
    char timeBuffer[bufferSize];
    strftime(timeBuffer, bufferSize, "%Y-%m-%d %H:%M:%S", localtime(&time));
    return timeBuffer;
}

JSON::Object::Ptr request(Context::Ptr context, std::string url, std::string body = "")
{
    URI uri("https://www.strava.com");

    try
    {
        HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
        HTTPResponse response;
        HTTPRequest request;
        Parser parser;

        request.setMethod(body.empty() ? HTTPRequest::HTTP_GET : HTTPRequest::HTTP_POST);
        request.set("Authorization", "Bearer " + authentication.access_token);
        request.setURI(url);

        session.setPort(443);
        session.setTimeout(Timespan(10L, 0L));

        auto& os = session.sendRequest(request);
        auto ss = std::stringstream();

        if (request.getMethod() == HTTPRequest::HTTP_POST)
        {
            os << body;
        }
        
        StreamCopier::copyStream(session.receiveResponse(response), ss);

        return parser.parse(ss.str()).extract<Object::Ptr>();
    }
    catch (const SSLException& e)
    {
        std::cerr << e.what() << ": " << e.message() << std::endl;
    }
}

std::string str(JSON::Object::Ptr object, std::string key)
{
    auto value = object->get(key);
    
    if (value.isEmpty())
    {
        return strava::null;
    }

    auto v = std::string("");
    value.convert(v);
    return v;
}

void strava::athletes::current(strava::athlete& athlete)
{
    auto json = request(session, "/api/v3/athlete");

    athlete = {0};
    athlete.firstname = str(json, "firstname");
    athlete.lastname = str(json, "lastname");
    athlete.profile = str(json, "profile");
    athlete.profile_medium = str(json, "profile_medium");
    athlete.city = str(json, "city");
    athlete.state = str(json, "state");
    athlete.country = str(json, "country");
    athlete.sex = str(json, "sex");
    athlete.is_friend =  str(json, "friend");
    athlete.follower = str(json, "follower");
    athlete.date_preference = str(json, "date_preference");
    athlete.measurement_preference = str(json, "measurement_preference");
    athlete.email = str(json, "email");

    auto created_timestamp = str(json, "created_at");
    auto updated_timestamp = str(json, "created_at");

    athlete.created_at = to_time_t(created_timestamp, "%Y-%m-%dT%H:%M:%SZ");
    athlete.updated_at = to_time_t(updated_timestamp, "%Y-%m-%dT%H:%M:%SZ");
}

void strava::authenticate(strava::oauth&& autho, bool skip_init)
{
    authentication = autho;

    if (!skip_init && session.isNull())
    {
        session = Context::Ptr(new Context(Context::CLIENT_USE, ""));

        SSLManager::InvalidCertificateHandlerPtr handler(new AcceptCertificateHandler(false));
        SSLManager::instance().initializeClient(0, handler, session);
    }
}