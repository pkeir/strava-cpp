
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
#include <functional>
#include <exception>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <map>

///
/// 
///
strava::client session;
strava::oauth authentication;

///
/// Change to capital letters 
///
const std::string activities_url = "/api/v3/activities";
const std::string segments_url = "/api/v3/segments";
const std::string athlete_url = "/api/v3/athlete";
const std::string uploads_url = "/api/v3/uploads";
const std::string routes_url = "/api/v3/routes";
const std::string clubs_url = "/api/v3/clubs";
const std::string gear_url = "/api/v3/gear";

///
/// 
///
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

///
/// 
///
std::string pretty_time_t(time_t time)
{
    const auto format = "%Y-%m-%d %H:%M:%S";
    const auto size = 20;

    char buffer[20];
    strftime(buffer, 20, format, localtime(&time));
    return buffer;
}

///
/// 
///
Poco::JSON::Object::Ptr request(std::string url, std::string body = "")
{
    Poco::JSON::Parser parser;
    Poco::JSON::Object::Ptr value = {};

    try
    {
        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest request;

        request.setMethod(body.empty() ? Poco::Net::HTTPRequest::HTTP_GET : Poco::Net::HTTPRequest::HTTP_POST);
        request.set("Authorization", "Bearer " + authentication.access_token);
        request.setURI(url);

        auto& os = session.session->sendRequest(request);
        auto ss = std::stringstream();

        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
        {
            os << body;
        }

        Poco::StreamCopier::copyStream(session.session->receiveResponse(response), ss);
        value = parser.parse(ss.str()).extract<Poco::JSON::Object::Ptr>();
    }
    catch (const Poco::Net::SSLException& e)
    {
        std::cerr << e.what() << ": " << e.message() << std::endl;
    }

    return value;
}

///
/// 
///
template<typename T>
T cast(Poco::JSON::Object::Ptr json, std::string key, T on_empty)
{
    auto value = json->get(key);

    if (value.isEmpty())
    {
        return on_empty;
    }

    auto v = on_empty;
    value.convert(v);
    return v;
}

///
/// 
///
void club_from_json(Poco::JSON::Object::Ptr json, strava::summary::club& club)
{
    club.id = cast<int>(json, "id", 0);
    club.resource_state = cast<int>(json, "resource_state", 0);
    club.name = cast<std::string>(json, "name", "");
    club.profile = cast<std::string>(json, "profile", "");
    club.profile_medium = cast<std::string>(json, "profile_medium", "");
    club.cover_photo = cast<std::string>(json, "cover_photo", "");
    club.cover_photo_small = cast<std::string>(json, "cover_photo_small", "");
    club.sport_type = cast<std::string>(json, "sport_type", "");
    club.city = cast<std::string>(json, "city", "");
    club.state = cast<std::string>(json, "state", "");
    club.country = cast<std::string>(json, "country", "");
    club.is_private = cast<bool>(json, "private", false);
    club.member_count = cast<int>(json, "member_count", 0);
    club.featured = cast<bool>(json, "featured", false);
    club.url = cast<std::string>(json, "url", "");
}

///
/// 
///
void gear_from_json(Poco::JSON::Object::Ptr json, strava::summary::gear& gear)
{
    gear.resource_state = cast<int>(json, "resource_state", 0);
    gear.distance = cast<double>(json, "distance", 0.0);
    gear.primary = cast<bool>(json, "primary", false);
    gear.name = cast<std::string>(json, "name", "");
    gear.id = cast<std::string>(json, "id", "");
}

///
///
///
void gear_from_json(Poco::JSON::Object::Ptr json, strava::detailed::gear& gear)
{
    gear_from_json(json, (strava::summary::gear&)gear); 

    gear.brand_name = cast<std::string>(json, "brand_name", "");
    gear.model_name = cast<std::string>(json, "model_name", "");
    gear.frame_type = cast<std::string>(json, "frame_type", "");
    gear.description = cast<std::string>(json, "description", "");
}

///
/// 
///
void athlete_from_json(Poco::JSON::Object::Ptr json, strava::detailed::athlete& athlete)
{
    auto created_timestamp = cast<std::string>(json, "created_at", "");
    auto updated_timestamp = cast<std::string>(json, "updated_at", "");
    auto clubs = json->getArray("clubs");
    auto bikes = json->getArray("bikes");
    auto shoes = json->getArray("shoes");

    athlete.measurement_preference = cast<std::string>(json, "measurement_preference", "");
    athlete.date_preference = cast<std::string>(json, "date_preference", "");
    athlete.profile_medium = cast<std::string>(json, "profile_medium", "");
    athlete.firstname = cast<std::string>(json, "firstname", "");
    athlete.lastname = cast<std::string>(json, "lastname", "");
    athlete.follower = cast<std::string>(json, "follower", "");
    athlete.is_friend = cast<std::string>(json, "friend", "");
    athlete.profile = cast<std::string>(json, "profile", "");
    athlete.country = cast<std::string>(json, "country", "");
    athlete.state = cast<std::string>(json, "state", "");
    athlete.email = cast<std::string>(json, "email", "");
    athlete.city = cast<std::string>(json, "city", "");
    athlete.sex = cast<std::string>(json, "sex", "");

    athlete.created_at = to_time_t(created_timestamp, "%Y-%m-%dT%H:%M:%SZ");
    athlete.updated_at = to_time_t(updated_timestamp, "%Y-%m-%dT%H:%M:%SZ");

    athlete.mutual_friend_count = cast<int>(json, "mutual_friend_count", 0);
    athlete.resource_state = cast<int>(json, "resource_state", 0);
    athlete.follower_count = cast<int>(json, "follower_count", 0);
    athlete.friend_count = cast<int>(json, "friend_count", 0);
    athlete.athlete_type = cast<int>(json, "athlete_type", 0);
    athlete.weight = cast<int>(json, "weight", 0);
    athlete.ftp = cast<int>(json, "ftp", 0);
    athlete.id = cast<int>(json, "id", 0);

    athlete.premium = cast<bool>(json, "premium", false);

    athlete.bikes.reserve(bikes->size());
    athlete.shoes.reserve(shoes->size());
    athlete.clubs.reserve(clubs->size());

    for (auto& c : *clubs)
    {
        strava::summary::club club;
        club_from_json(c.extract<Poco::JSON::Object::Ptr>(), club);
        athlete.clubs.push_back(club);
    }

    for (auto& s : *shoes)
    {
        strava::summary::gear shoe;
        gear_from_json(s.extract<Poco::JSON::Object::Ptr>(), shoe);
        athlete.shoes.push_back(shoe);
    }

    for (auto& b : *bikes)
    {
        strava::summary::gear bike;
        gear_from_json(b.extract<Poco::JSON::Object::Ptr>(), bike);
        athlete.bikes.push_back(bike);
    }
}

///
/// 
///
void strava::authenticate(strava::oauth&& autho, bool skip_init)
{
    authentication = autho;

    if (!skip_init && session.context.isNull())
    {
        using namespace Poco::Net;

        Poco::URI uri("https://www.strava.com");

        session.context = new Context(Context::CLIENT_USE, "");
        session.session =  new HTTPSClientSession(uri.getHost(), uri.getPort(), session.context);
        session.session->setPort(443);
        session.session->setTimeout(Poco::Timespan(10L, 0L));

        SSLManager::InvalidCertificateHandlerPtr handler(new AcceptCertificateHandler(false));
        SSLManager::instance().initializeClient(0, handler, session.context);
    }
}

///
/// 
///
void strava::athlete::retrieve(int id, summary::athlete& out)
{
    auto response = request(athlete_url);
}

///
/// 
///
void strava::athlete::current(detailed::athlete& out)
{
    auto response = request(athlete_url);
    athlete_from_json(response, out);
}

///
/// 
///
void strava::gear::retrieve(const std::string& id, detailed::gear& out)
{
    auto response = request(gear_url + "/" + id);
    gear_from_json(response, out);
}

///
/// 
///
void strava::athlete::update(update::athlete update, detailed::athlete& updated_out)
{

}

///
/// 
///
void strava::athlete::get_zones(athlete::zones& out)
{

}

///
/// 
///
void strava::athlete::get_stats(detailed::athlete& athlete, statistics& stats)
{

}


///
/// 
///
void strava::athlete::list_kqom_cr(detailed::athlete& athlete, kqom_c& out)
{

}