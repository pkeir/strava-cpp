
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
#include <map>

Poco::Net::Context::Ptr ssl_session = nullptr;

strava::oauth authentication;

namespace endpoints
{
    const std::string athlete_url = "/api/v3/athlete";
    const std::string activities_url = "/api/v3/activities/";
    const std::string clubs_url = "/api/v3/clubs/";
    const std::string gear_url = "/api/v3/gear/";
    const std::string routes_url = "/api/v3/clubs/";
    const std::string segments_url = "/api/v3/segments/";
    const std::string uploads_url = "/api/v3/uploads";
};

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
    const auto format = "%Y-%m-%d %H:%M:%S";
    const auto size = 20;

    char buffer[20];
    strftime(buffer, 20, format, localtime(&time));
    return buffer;
}

Poco::JSON::Object::Ptr request(Poco::Net::Context::Ptr context, std::string url, std::string body = "")
{
    Poco::URI uri("https://www.strava.com");

    try
    {
        Poco::Net::HTTPSClientSession ssl_session(uri.getHost(), uri.getPort(), context);
        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest request;

        request.setMethod(body.empty() ? Poco::Net::HTTPRequest::HTTP_GET : Poco::Net::HTTPRequest::HTTP_POST);
        request.set("Authorization", "Bearer " + authentication.access_token);
        request.setURI(url);

        ssl_session.setPort(443);
        ssl_session.setTimeout(Poco::Timespan(10L, 0L));

        auto& os = ssl_session.sendRequest(request);
        auto ss = std::stringstream();

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
        {
            os << body;
        }

        Poco::StreamCopier::copyStream(ssl_session.receiveResponse(response), ss);
        Poco::JSON::Parser parser;
        return parser.parse(ss.str()).extract<Poco::JSON::Object::Ptr>();
    }
    catch (const Poco::Net::SSLException& e)
    {
        std::cerr << e.what() << ": " << e.message() << std::endl;
    }
}

int num(Poco::JSON::Object::Ptr object, std::string key)
{
    auto value = object->get(key);

    if (value.isEmpty())
    {
        return 0;
    }

    auto v = int{ 0 };
    value.convert(v);
    return v;
}

std::string str(Poco::JSON::Object::Ptr object, std::string key)
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

void club_from_json(Poco::JSON::Object::Ptr json, strava::club& club)
{
    club = { 0 };
}

void bike_from_json(Poco::JSON::Object::Ptr json, strava::bike& bike)
{
    bike = { 0 };
}

void shoe_from_json(Poco::JSON::Object::Ptr json, strava::shoe& shoe)
{
    shoe = { 0 };
}

void athlete_from_json(Poco::JSON::Object::Ptr json, strava::athlete& athlete)
{
    auto created_timestamp = str(json, "created_at");
    auto updated_timestamp = str(json, "created_at");
    auto clubs = json->getArray("clubs");
    auto bikes = json->getArray("bikes");
    auto shoes = json->getArray("shoes");

    athlete = { 0 };
    athlete.firstname = str(json, "firstname");
    athlete.lastname = str(json, "lastname");
    athlete.profile = str(json, "profile");
    athlete.profile_medium = str(json, "profile_medium");
    athlete.city = str(json, "city");
    athlete.state = str(json, "state");
    athlete.country = str(json, "country");
    athlete.sex = str(json, "sex");
    athlete.is_friend = str(json, "friend");
    athlete.follower = str(json, "follower");
    athlete.date_preference = str(json, "date_preference");
    athlete.measurement_preference = str(json, "measurement_preference");
    athlete.email = str(json, "email");
    athlete.created_at = to_time_t(created_timestamp, "%Y-%m-%dT%H:%M:%SZ");
    athlete.updated_at = to_time_t(updated_timestamp, "%Y-%m-%dT%H:%M:%SZ");
    athlete.id = num(json, "id");
    athlete.resource_state = num(json, "resource_state");
    athlete.follower_count = num(json, "follower_count");
    athlete.friend_count = num(json, "friend_count");
    athlete.mutual_friend_count = num(json, "mutual_friend_count");
    athlete.athlete_type = num(json, "athlete_type");
    athlete.ftp = num(json, "ftp");
    athlete.weight = num(json, "weight");
    athlete.premium = num(json, "premium") != 0;
    athlete.bikes.reserve(bikes->size());
    athlete.shoes.reserve(shoes->size());
    athlete.clubs.reserve(clubs->size());

    for (auto& b : *bikes)
    {
        strava::bike bike;
        bike_from_json(b, bike);
        athlete.bikes.push_back(bike);
    }

    for (auto& c : *clubs)
    {
        strava::club club;
        club_from_json(c, club);
        athlete.clubs.push_back(club);
    }

    for (auto& s : *shoes)
    {
        strava::shoe shoe;
        shoe_from_json(s, shoe);
        athlete.shoes.push_back(shoe);
    }
}

void strava::authenticate(strava::oauth&& autho, bool skip_init)
{
    authentication = autho;

    if (!skip_init && ssl_session.isNull())
    {
        using namespace Poco::Net;

        ssl_session = Context::Ptr(new Context(Context::CLIENT_USE, ""));

        SSLManager::InvalidCertificateHandlerPtr handler(new AcceptCertificateHandler(false));
        SSLManager::instance().initializeClient(0, handler, ssl_session);
    }
}

void strava::athletes::current(strava::athlete& athlete)
{
    auto response = request(ssl_session, endpoints::athlete_url);
    athlete_from_json(response, athlete);
}