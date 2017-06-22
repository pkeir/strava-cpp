
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/NetSSL.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTMLForm.h>
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

strava::client session;

const std::string activities_url = "/api/v3/activities";
const std::string segments_url = "/api/v3/segments";
const std::string athlete_url = "/api/v3/athlete";
const std::string uploads_url = "/api/v3/uploads";
const std::string routes_url = "/api/v3/routes";
const std::string clubs_url = "/api/v3/clubs";
const std::string gear_url = "/api/v3/gear";


strava::error::error(const std::string& msg, const std::vector<error_code>& codes) :
    std::runtime_error(msg),
    error_codes(codes),
    message(msg)
{
}

const char* strava::error::what() const
{
    return message.c_str();
}

const std::vector<strava::error::error_code>& strava::error::codes()
{
    return error_codes;
}

std::time_t to_time_t(std::string timestring, std::string format)
{
    std::tm time;
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

void requires_https()
{
    using namespace Poco::Net;

    if (session.context.isNull())
    {
        session.context = new Context(Context::CLIENT_USE, "");
    }

    if (session.session.isNull() && !session.context.isNull())
    {
        Poco::URI uri("https://www.strava.com");

        session.session = new HTTPSClientSession(uri.getHost(), uri.getPort(), session.context);
        session.session->setPort(443);
        session.session->setTimeout(Poco::Timespan(10L, 0L));

        SSLManager::InvalidCertificateHandlerPtr handler(new AcceptCertificateHandler(false));
        SSLManager::instance().initializeClient(0, handler, session.context);
    }
}

Poco::Dynamic::Var get(std::string url, std::string access_token)
{
    requires_https();

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var value = {};

    try
    {
        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest request;

        request.setMethod(Poco::Net::HTTPRequest::HTTP_GET);
        request.set("Authorization", "Bearer " + access_token);
        request.setURI(url);

        std::stringstream ss;
        session.session->sendRequest(request);

        Poco::StreamCopier::copyStream(session.session->receiveResponse(response), ss);
        value = parser.parse(ss.str());
    }
    catch (const Poco::Net::SSLException& e)
    {
        std::cerr << e.what() << ": " << e.message() << std::endl;
    }

    return value;
}

Poco::Dynamic::Var put(std::string url, std::string access_token, std::map<std::string, std::string> form_entries)
{
    requires_https();

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var value = {};

    try
    {
        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest request;
        Poco::Net::HTMLForm form;

        request.setMethod(Poco::Net::HTTPRequest::HTTP_PUT);
        request.setURI(url);

        if (!access_token.empty())
        {
            request.set("Authorization", "Bearer " + access_token);
        }

        if (!form_entries.empty())
        {
            for (auto& p : form_entries)
            {
                form.set(p.first, p.second);
            }

            form.prepareSubmit(request);
            form.write(session.session->sendRequest(request));
        }
        else
        {
            session.session->sendRequest(request);
        }

        std::stringstream ss;
        std::istream& is = session.session->receiveResponse(response);

        Poco::StreamCopier::copyStream(is, ss);
        value = parser.parse(ss.str());
    }
    catch (const Poco::Net::SSLException& e)
    {
        std::cerr << e.what() << ": " << e.message() << std::endl;
    }

    return value;
}

Poco::Dynamic::Var post(std::string url, std::string access_token = "", std::map<std::string, std::string> form_entries = {})
{
    requires_https();

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var value = {};

    try
    {
        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest request;
        Poco::Net::HTMLForm form;

        request.setMethod(Poco::Net::HTTPRequest::HTTP_POST);
        request.setURI(url);

        if (!access_token.empty())
        {
            request.set("Authorization", "Bearer " + access_token);
        }

        if (!form_entries.empty())
        {
            for (auto& p : form_entries)
            {
                form.set(p.first, p.second);
            }

            form.prepareSubmit(request);
            form.write(session.session->sendRequest(request));
        }
        else
        {
            session.session->sendRequest(request);
        }

        std::stringstream ss;
        std::istream& is = session.session->receiveResponse(response);

        Poco::StreamCopier::copyStream(is, ss);
        value = parser.parse(ss.str());
    }
    catch (const Poco::Net::SSLException& e)
    {
        std::cerr << e.what() << ": " << e.message() << std::endl;
    }

    return value;
}

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

template<typename T, typename F>
std::vector<T> json_to_vector(Poco::JSON::Array::Ptr list, F functor)
{
    std::vector<T> elements;
    elements.reserve(list->size());

    for (auto& e : *list)
    {
        T data;
        functor(e.extract<Poco::JSON::Object::Ptr>(), data);
        elements.push_back(data);
    }

    return elements;
}

Poco::Dynamic::Var& throw_on_error(Poco::Dynamic::Var& response)
{
    if (response.type() == typeid(Poco::JSON::Object::Ptr))
    {
        auto json = response.extract<Poco::JSON::Object::Ptr>();
        auto error_array = json->getArray("errors");

        if (!error_array.isNull())
        {
            auto error_message = json->get("message").toString();
            auto error_codes = std::vector<strava::error::error_code>();

            for (auto& e : *error_array)
            {
                auto eo = e.extract<Poco::JSON::Object::Ptr>();
                error_codes.push_back({
                    eo->get("resource").toString(),
                    eo->get("field").toString(),
                    eo->get("code").toString()
                });
            }

            throw strava::error(error_message, error_codes);
        }
    }

    return response;
}

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

void gear_from_json(Poco::JSON::Object::Ptr json, strava::summary::gear& gear)
{
    gear.resource_state = cast<int>(json, "resource_state", 0);
    gear.distance = cast<double>(json, "distance", 0.0);
    gear.primary = cast<bool>(json, "primary", false);
    gear.name = cast<std::string>(json, "name", "");
    gear.id = cast<std::string>(json, "id", "");
}

void gear_from_json(Poco::JSON::Object::Ptr json, strava::detailed::gear& gear)
{
    gear_from_json(json, (strava::summary::gear&)gear);

    gear.brand_name = cast<std::string>(json, "brand_name", "");
    gear.model_name = cast<std::string>(json, "model_name", "");
    gear.frame_type = cast<std::string>(json, "frame_type", "");
    gear.description = cast<std::string>(json, "description", "");
}

void athlete_from_json(Poco::JSON::Object::Ptr json, strava::meta::athlete& athlete)
{
    athlete.id = cast<int>(json, "id", 0);
    athlete.resource_state = cast<int>(json, "resource_state", 0);
}

void athlete_from_json(Poco::JSON::Object::Ptr json, strava::summary::athlete& athlete)
{
    athlete.firstname = cast<std::string>(json, "firstname", "");
    athlete.lastname = cast<std::string>(json, "lastname", "");
    athlete.profile_medium = cast<std::string>(json, "profile_medium", "");
    athlete.profile = cast<std::string>(json, "profile", "");
    athlete.city = cast<std::string>(json, "city", "");
    athlete.state = cast<std::string>(json, "state", "");
    athlete.country = cast<std::string>(json, "country", "");
    athlete.sex = cast<std::string>(json, "sex", "");
    athlete.follower = cast<std::string>(json, "follower", "");
    athlete.is_friend = cast<std::string>(json, "friend", "");

    athlete.premium = cast<bool>(json, "premium", false);

    auto created_timestamp = cast<std::string>(json, "created_at", "");
    auto updated_timestamp = cast<std::string>(json, "updated_at", "");

    athlete.created_at = to_time_t(created_timestamp, "%Y-%m-%dT%H:%M:%SZ");
    athlete.updated_at = to_time_t(updated_timestamp, "%Y-%m-%dT%H:%M:%SZ");
}

void athlete_from_json(Poco::JSON::Object::Ptr json, strava::detailed::athlete& athlete)
{
    athlete_from_json(json, (strava::meta::athlete&)athlete);
    athlete_from_json(json, (strava::summary::athlete&)athlete);

    auto clubs = json->getArray("clubs");
    auto bikes = json->getArray("bikes");
    auto shoes = json->getArray("shoes");

    athlete.measurement_preference = cast<std::string>(json, "measurement_preference", "");
    athlete.date_preference = cast<std::string>(json, "date_preference", "");
    athlete.email = cast<std::string>(json, "email", "");

    athlete.mutual_friend_count = cast<int>(json, "mutual_friend_count", 0);
    athlete.follower_count = cast<int>(json, "follower_count", 0);
    athlete.friend_count = cast<int>(json, "friend_count", 0);
    athlete.athlete_type = cast<int>(json, "athlete_type", 0);
    athlete.weight = cast<int>(json, "weight", 0);
    athlete.ftp = cast<int>(json, "ftp", 0);

    if (!bikes.isNull() && !shoes.isNull() && !clubs.isNull())
    {
        athlete.bikes.reserve(bikes->size());
        athlete.shoes.reserve(shoes->size());
        athlete.clubs.reserve(clubs->size());
    }

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

void heart_rate_from_json(Poco::JSON::Object::Ptr json, strava::athlete::zones& out)
{
    if (json.isNull())
    {
        return;
    }

    out.heart_rate = {};
    out.heart_rate.custom_zones = cast<bool>(json, "custom_zones", false); 

    auto maybe_zones = json->get("zones");

    if (maybe_zones.isArray())
    {
        auto zones = maybe_zones.extract<Poco::JSON::Array::Ptr>();

        for (auto& zone : *zones)
        {
            auto obj = zone.extract<Poco::JSON::Object::Ptr>();
            out.heart_rate.zones.push_back({
                cast<int>(obj, "min", 0),
                cast<int>(obj, "max", 0)
            });
        }
    }
}

void power_from_json(Poco::JSON::Object::Ptr json, strava::athlete::zones& out)
{
    if (json.isNull())
    {
        return;
    }

    out.power = {};

    auto maybe_zones = json->get("zones");

    if (maybe_zones.isArray())
    {
        auto zones = maybe_zones.extract<Poco::JSON::Array::Ptr>();

        for (auto& zone : *zones)
        {
            auto obj = zone.extract<Poco::JSON::Object::Ptr>();
            out.power.zones.push_back({
                cast<int>(obj, "min", 0),
                cast<int>(obj, "max", 0)
            });
        }
    }
}

std::string strava::request_access(int client_id, oauth_scope scope)
{
    std::stringstream url_builder;
    url_builder << "https://www.strava.com/oauth/authorize?";
    url_builder << "client_id=" << client_id << "&";
    url_builder << "response_type=code&";
    url_builder << "redirect_uri=http://localhost/exchange_token";
    url_builder << "&approval_prompt=force&scope=";

    switch (scope)
    {
    case strava::oauth_scope::scope_public:
        url_builder << "public";
        break;
    case strava::oauth_scope::scope_write:
        url_builder << "write";
        break;
    case strava::oauth_scope::scope_view_write:
        url_builder << "view_write";
        break;
    case strava::oauth_scope::scope_view_private_write:
        url_builder << "view_private,write";
        break;
    default:
        break;
    }

    return url_builder.str();
}

std::string strava::exchange_token(int client_id, std::string client_secret, std::string token)
{
    auto parameters = std::map<std::string, std::string>
    {
        { "client_id", std::to_string(client_id) },
        { "client_secret", client_secret },
        { "code", token }
    };

    auto response = throw_on_error(post("/oauth/token", "", parameters));
    auto json = response.extract<Poco::JSON::Object::Ptr>();

    return json->get("access_token").toString();
}

std::string strava::deauthorization(const strava::oauth& auth)
{
    auto response = throw_on_error(post("/oauth/deauthorize", auth.access_token));
    auto json = response.extract<Poco::JSON::Object::Ptr>();

    return json->get("access_token").toString();
}

std::vector<strava::summary::athlete> strava::athlete::list_athlete_friends(const strava::oauth& auth, meta::athlete& athlete, int page, int per_page)
{
    auto url = std::string("/api/v3/athletes/" + std::to_string(athlete.id) + "/friends");
    auto response = throw_on_error(get(url, auth.access_token));
    auto json = response.extract<Poco::JSON::Array::Ptr>();

    return json_to_vector<summary::athlete>(json, [](auto& j, auto& a) { athlete_from_json(j, a); });
}

std::vector<strava::summary::athlete> strava::athlete::list_athlete_friends(const strava::oauth& auth, int page, int per_page)
{
    auto url = std::string("/api/v3/athlete/friends");
    auto response = throw_on_error(get(url, auth.access_token));
    auto json = response.extract<Poco::JSON::Array::Ptr>();

    return json_to_vector<summary::athlete>(json, [](auto& j, auto& a) { athlete_from_json(j, a); });
}

std::vector<strava::summary::athlete> strava::athlete::list_athlete_followers(const oauth& auth, meta::athlete& athlete, int page, int per_page)
{
    auto url = std::string("/api/v3/athletes/" + std::to_string(athlete.id) + "/followers");
    auto response = throw_on_error(get(url, auth.access_token));
    auto json = response.extract<Poco::JSON::Array::Ptr>();

    return json_to_vector<summary::athlete>(json, [](auto& j, auto& a) { athlete_from_json(j, a); });
}

std::vector<strava::summary::athlete> strava::athlete::list_athlete_followers(const oauth& auth, int page, int per_page)
{
    auto url = std::string("/api/v3/athlete/followers");
    auto response = throw_on_error(get(url, auth.access_token));
    auto json = response.extract<Poco::JSON::Array::Ptr>();

    return json_to_vector<summary::athlete>(json, [](auto& j, auto& a) { athlete_from_json(j, a); });
}

std::vector<strava::summary::athlete> strava::athlete::list_both_following(const oauth& auth, meta::athlete& athlete, int page, int per_page)
{
    auto url = std::string("/api/v3/athletes/" + std::to_string(athlete.id) + "/both-following");
    auto response = throw_on_error(get(url, auth.access_token));
    auto json = response.extract<Poco::JSON::Array::Ptr>();

    return json_to_vector<summary::athlete>(json, [](auto& j, auto& a) { athlete_from_json(j, a); });
}

strava::detailed::athlete strava::athlete::current(const strava::oauth& auth)
{
    auto response = throw_on_error(get(athlete_url, auth.access_token));
    auto json = response.extract<Poco::JSON::Object::Ptr>();

    strava::detailed::athlete out;
    athlete_from_json(json, out);
    return out;
}

strava::summary::athlete strava::athlete::retrieve(const oauth& auth, int id)
{
    std::stringstream ss;
    ss << athlete_url << "/";
    ss << std::to_string(id);

    auto response = throw_on_error(get(ss.str(), auth.access_token));
    auto json = response.extract<Poco::JSON::Object::Ptr>();

    strava::summary::athlete out;
    athlete_from_json(json, out);
    return out;
}

strava::detailed::gear strava::gear::retrieve(const oauth& auth_info, const std::string& id)
{
    auto url = gear_url + "/" + id;
    auto response = throw_on_error(get(url, auth_info.access_token));
    auto json = response.extract<Poco::JSON::Object::Ptr>();

    detailed::gear out;
    gear_from_json(json, out);
    return out;
}

strava::detailed::athlete strava::athlete::update(const oauth& auth_info, meta::athlete athlete, std::map<std::string, std::string> updates)
{
    auto response = throw_on_error(put("/api/v3/athlete", auth_info.access_token, updates));
    auto json = response.extract<Poco::JSON::Object::Ptr>();

    detailed::athlete value;
    athlete_from_json(json, value);
    return value;
}

strava::athlete::zones strava::athlete::get_zones(const oauth& auth_info)
{
    auto response = throw_on_error(get("/api/v3/athlete/zones", auth_info.access_token));
    auto json = response.extract<Poco::JSON::Object::Ptr>();

    strava::athlete::zones out;
    heart_rate_from_json(json->getObject("heart_rate"), out);
    power_from_json(json->getObject("power"), out);
    return out;
}

void strava::athlete::get_stats(detailed::athlete& athlete, stats& stats)
{
    // TODO:
}

void strava::athlete::list_kqom_cr(detailed::athlete& athlete, kqom_c& out)
{
    // TODO:
}