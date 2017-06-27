
#include <Poco/Net/HTTPSClientSession.h>
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
#include <string>
#include <map>

using namespace std::string_literals;

using json_object = Poco::JSON::Object::Ptr;
using json_array = Poco::JSON::Array::Ptr;

const auto activities_url = "/api/v3/activities"s;
const auto segments_url = "/api/v3/segments"s;
const auto athletes_url = "/api/v3/athletes"s;
const auto athlete_url = "/api/v3/athlete"s;
const auto uploads_url = "/api/v3/uploads"s;
const auto routes_url = "/api/v3/routes"s;
const auto clubs_url = "/api/v3/clubs"s;
const auto gear_url = "/api/v3/gear"s;


///
/// Client struct which keeps
/// a Poco https session and the context
/// for SSL https server/client
///
struct poco_client
{
    Poco::SharedPtr<Poco::Net::HTTPSClientSession> client;
    Poco::AutoPtr<Poco::Net::Context> context;
} client;


///
///
///
struct http_request
{
    std::string method, url, access_token;
    std::map<std::string, std::string> form;
    std::map<std::string, std::string> data;
    strava::pagination page_options;
};


///
/// Constructor for setting the default values
/// allowing overwriting when wanting pagination
/// functionality
///
strava::pagination::pagination(int page, int per_page) :
    per_page(per_page),
    page(page)
{
}

///
/// Tells if the user passed in paginatino parameters
///
bool strava::pagination::enabled()
{
    return page != 0;
}

///
/// Error constructor takes a vector of error codes from Strava API
/// and the error message as well.
///
strava::error::error(const std::string& msg, const std::vector<error_code>& codes) :
    std::runtime_error(msg),
    error_codes(codes),
    message(msg)
{
}

///
/// Returns the message given by the Strava API.
///
const char* strava::error::what() const
{
    return message.c_str();
}

///
/// Returns the error codes given by the Strava API.
///
const std::vector<strava::error::error_code>& strava::error::codes()
{
    return error_codes;
}

///
/// Constructor for time object from ISO 8601 standard.
///
strava::time::time(std::string timestr)
{
    std::tm time;
    std::istringstream input(timestr);
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(&time, "%Y-%m-%dT%H:%M:%SZ");

    auto failed = input.fail();
    time_string = failed ? "" : timestr;
    time_epoch = failed ? 0 : mktime(&time);
}

///
/// Constructor for time object from std::time_t
///
strava::time::time(std::time_t num)
{
    const auto format = "%Y-%m-%dT%H:%M:%SZ";//"%Y-%m-%d %H:%M:%S";
    const auto size = 20;

    char buffer[size];
    strftime(buffer, size, format, localtime(&num));
    time_string = buffer;
    time_epoch = num;
}

///
///
///
void lazy_start_session()
{
    using namespace Poco::Net;

    if (client.context.isNull())
    {
        client.context = new Context(Context::CLIENT_USE, "");
    }

    if (client.client.isNull() && !client.context.isNull())
    {
        Poco::URI uri("https://www.strava.com");

        client.client = new HTTPSClientSession(uri.getHost(), uri.getPort(), client.context);
        client.client->setPort(443);
        client.client->setTimeout(Poco::Timespan(10L, 0L));

        SSLManager::InvalidCertificateHandlerPtr handler(new AcceptCertificateHandler(false));
        SSLManager::instance().initializeClient(0, handler, client.context);
    }
}

template<typename T>
std::string join(std::string prefix, T value, std::string suffix = "")
{
    std::stringstream ss;
    ss << prefix;
    ss << value;
    ss << suffix;
    return ss.str();
}

Poco::Dynamic::Var send(http_request& info)
{
    Poco::URI uri(info.url);
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var value = {};

    if (info.page_options.enabled())
    {
        info.data["per_page"] = std::to_string(info.page_options.per_page);
        info.data["page"] = std::to_string(info.page_options.page);
    }

    for (auto& data : info.data)
    {
        uri.addQueryParameter(data.first, data.second);
    }

    lazy_start_session();

    try
    {
        Poco::Net::HTTPResponse response;
        Poco::Net::HTTPRequest request;
        Poco::Net::HTMLForm form;

        request.setMethod(info.method);
        request.setURI(uri.toString());
        request.setKeepAlive(true);

        if (!info.access_token.empty())
        {
            request.set("Authorization", "Bearer " + info.access_token);
        }

        if (!info.form.empty())
        {
            for (auto& p : info.form)
            {
                form.set(p.first, p.second);
            }

            form.prepareSubmit(request);
            form.write(client.client->sendRequest(request));
        }
        else
        {
            auto& os = client.client->sendRequest(request);
        }

        std::stringstream ss;
        std::istream& is = client.client->receiveResponse(response);

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
T cast(Poco::JSON::Object::Ptr json, std::string key)
{
    auto value = json->get(key);

    if (value.isEmpty())
    {
        return T{};
    }

    auto v = T{};
    value.convert(v);
    return v;
}

template<typename T>
std::string stringify(T json)
{
    std::stringstream ss;
    json->stringify(ss, 4, 0);
    return ss.str();
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

Poco::Dynamic::Var& check(Poco::Dynamic::Var& response)
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
    club.id = cast<int>(json, "id");
    club.resource_state = cast<int>(json, "resource_state");
    club.name = cast<std::string>(json, "name");
    club.profile = cast<std::string>(json, "profile");
    club.profile_medium = cast<std::string>(json, "profile_medium");
    club.cover_photo = cast<std::string>(json, "cover_photo");
    club.cover_photo_small = cast<std::string>(json, "cover_photo_small");
    club.sport_type = cast<std::string>(json, "sport_type");
    club.city = cast<std::string>(json, "city");
    club.state = cast<std::string>(json, "state");
    club.country = cast<std::string>(json, "country");
    club.is_private = cast<bool>(json, "private");
    club.member_count = cast<int>(json, "member_count");
    club.featured = cast<bool>(json, "featured");
    club.url = cast<std::string>(json, "url");
}

void gear_from_json(Poco::JSON::Object::Ptr json, strava::summary::gear& gear)
{
    gear.resource_state = cast<int>(json, "resource_state");
    gear.distance = cast<double>(json, "distance");
    gear.primary = cast<bool>(json, "primary");
    gear.name = cast<std::string>(json, "name");
    gear.id = cast<std::string>(json, "id");
}

void gear_from_json(Poco::JSON::Object::Ptr json, strava::detailed::gear& gear)
{
    gear_from_json(json, (strava::summary::gear&)gear);

    gear.brand_name = cast<std::string>(json, "brand_name");
    gear.model_name = cast<std::string>(json, "model_name");
    gear.frame_type = cast<std::string>(json, "frame_type");
    gear.description = cast<std::string>(json, "description");
}

void athlete_from_json(Poco::JSON::Object::Ptr json, strava::meta::athlete& athlete)
{
    athlete.id = cast<int>(json, "id");
    athlete.resource_state = cast<int>(json, "resource_state");
}

void activity_from_json(Poco::JSON::Object::Ptr json, strava::meta::activity& activity)
{
    activity.id = cast<int>(json, "id");
    activity.resouce_state = cast<int>(json, "resource_state");
}

void athlete_from_json(Poco::JSON::Object::Ptr json, strava::summary::athlete& athlete)
{
    athlete_from_json(json, (strava::meta::athlete&)athlete);

    athlete.firstname = cast<std::string>(json, "firstname");
    athlete.lastname = cast<std::string>(json, "lastname");
    athlete.profile_medium = cast<std::string>(json, "profile_medium");
    athlete.profile = cast<std::string>(json, "profile");
    athlete.city = cast<std::string>(json, "city");
    athlete.state = cast<std::string>(json, "state");
    athlete.country = cast<std::string>(json, "country");
    athlete.sex = cast<std::string>(json, "sex");
    athlete.follower = cast<std::string>(json, "follower");
    athlete.is_friend = cast<std::string>(json, "friend");

    athlete.premium = cast<bool>(json, "premium");

    athlete.created_at = strava::time(cast<std::string>(json, "created_at"));
    athlete.updated_at = strava::time(cast<std::string>(json, "updated_at"));
}

void athlete_from_json(Poco::JSON::Object::Ptr json, strava::detailed::athlete& athlete)
{
    athlete_from_json(json, (strava::summary::athlete&)athlete);

    auto clubs = json->getArray("clubs");
    auto bikes = json->getArray("bikes");
    auto shoes = json->getArray("shoes");

    athlete.measurement_preference = cast<std::string>(json, "measurement_preference");
    athlete.date_preference = cast<std::string>(json, "date_preference");
    athlete.email = cast<std::string>(json, "email");

    athlete.mutual_friend_count = cast<int>(json, "mutual_friend_count");
    athlete.follower_count = cast<int>(json, "follower_count");
    athlete.friend_count = cast<int>(json, "friend_count");
    athlete.athlete_type = cast<int>(json, "athlete_type");
    athlete.weight = cast<int>(json, "weight");
    athlete.ftp = cast<int>(json, "ftp");

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
    out.heart_rate.custom_zones = cast<bool>(json, "custom_zones");

    auto maybe_zones = json->get("zones");

    if (maybe_zones.isArray())
    {
        auto zones = maybe_zones.extract<Poco::JSON::Array::Ptr>();

        for (auto& zone : *zones)
        {
            auto obj = zone.extract<Poco::JSON::Object::Ptr>();
            out.heart_rate.zones.push_back({
                cast<int>(obj, "min"),
                cast<int>(obj, "max")
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
                cast<int>(obj, "min"),
                cast<int>(obj, "max")
            });
        }
    }
}

void total_from_json(Poco::JSON::Object::Ptr json, strava::athlete::stats::total& total)
{
    total = {};
    total.distance = cast<double>(json, "distance");
    total.elevation_gain = cast<double>(json, "elevation_gain");
    total.eleapsed_time = cast<int>(json, "eleapsed_time");
    total.moving_time = cast<int>(json, "moving_time");
    total.count = cast<int>(json, "count");
}

void total_with_ac_from_json(Poco::JSON::Object::Ptr json, strava::athlete::stats::detailed_total& total)
{
    total_from_json(json, total);
    total.achievement_count = cast<int>(json, "achievement_count");
}

void stats_from_json(Poco::JSON::Object::Ptr json, strava::athlete::stats& out)
{
    out = {};
    out.biggest_ride_distance = cast<double>(json, "biggest_ride_distance");
    out.biggest_climb_elevation_gain = cast<double>(json, "biggest_climb_elevation_gain");

    total_with_ac_from_json(json->getObject("recent_ride_totals"), out.recent_ride_totals);
    total_with_ac_from_json(json->getObject("recent_swim_totals"), out.recent_swim_totals);
    total_with_ac_from_json(json->getObject("recent_run_totals"), out.recent_run_totals);

    total_from_json(json->getObject("ytd_ride_totals"), out.ytd_ride_totals);
    total_from_json(json->getObject("ytd_run_totals"), out.ytd_run_totals);
    total_from_json(json->getObject("ytd_swim_totals"), out.ytd_swim_totals);
    total_from_json(json->getObject("all_ride_totals"), out.all_ride_totals);
    total_from_json(json->getObject("all_swim_totals"), out.all_swim_totals);
    total_from_json(json->getObject("all_run_totals"), out.all_run_totals);
}

void map_from_json(Poco::JSON::Object::Ptr json, strava::polyline& out)
{
    out = {};
    out.id = cast<std::string>(json, "id");
    out.line = cast<std::string>(json, "polyline");
    out.resource_state = cast<int>(json, "resource_state");
}

void segment_from_json(Poco::JSON::Object::Ptr json, strava::summary::segment& out)
{
    out.id = cast<int>(json, "id");
    out.resource_state = cast<int>(json, "resource_state");
    out.climb_category = cast<int>(json, "climb_category");

    out.distance = cast<float>(json, "distance");
    out.average_grade = cast<float>(json, "average_grade");
    out.maximum_grade = cast<float>(json, "maximum_grade");
    out.elevation_high = cast<float>(json, "elevation_high");
    out.elevation_low = cast<float>(json, "elevation_low");

    out.name = cast<std::string>(json, "name");
    out.activity_type = cast<std::string>(json, "activity_type");
    out.city = cast<std::string>(json, "city");
    out.state = cast<std::string>(json, "state");
    out.country = cast<std::string>(json, "country");

    auto start_elements = json->getArray("start_latlng");
    auto end_elements = json->getArray("start_latlng");

    out.start_latlng[0] = start_elements->get(0).convert<int>();
    out.start_latlng[1] = start_elements->get(1).convert<int>();

    out.end_latlng[0] = end_elements->get(0).convert<int>();
    out.end_latlng[1] = end_elements->get(1).convert<int>();

    out.is_private = cast<bool>(json, "is_private");
    out.hazardous = cast<bool>(json, "hazardous");
    out.starred = cast<bool>(json, "starred");
}

void segment_from_json(Poco::JSON::Object::Ptr json, strava::detailed::segment& out)
{
    segment_from_json(json, (strava::summary::segment&) out);
    map_from_json(json, out.map);

    out.created_at = strava::time(cast<std::string>(json, "created_at"));
    out.updated_at = strava::time(cast<std::string>(json, "updated_at"));
    out.total_elevation_gain = cast<int>(json, "total_elevation_gain");
    out.athlete_count = cast<int>(json, "athlete_count");
    out.effort_count = cast<int>(json, "effort_count");
    out.star_count = cast<int>(json, "star_count");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::summary::segment_effort& out)
{

}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::detailed::segment_effort& out)
{
    out = {};
    out.id = cast<std::int64_t>(json, "id");
    out.name = cast<std::string>(json, "name");

    activity_from_json(json, out.activity);
    athlete_from_json(json, out.athlete);
    segment_from_json(json, out.segment);

    out.start_date_local = strava::time(cast<std::string>(json, "start_date_local"));
    out.start_date = strava::time(cast<std::string>(json, "start_date"));
    out.average_heartrate = cast<float>(json, "average_heartrate");
    out.average_cadence = cast<float>(json, "average_cadence");
    out.average_watts = cast<float>(json, "average_watts");
    out.distance = cast<float>(json, "distance");

    out.resource_state = cast<int>(json, "resource_state");
    out.max_heartrate = cast<int>(json, "max_heartrate");
    out.elapsed_time = cast<int>(json, "elapsed_time");
    out.moving_time = cast<int>(json, "moving_time");
    out.start_index = cast<int>(json, "start_index");
    out.end_index = cast<int>(json, "end_index");
    out.kom_rank = cast<int>(json, "kom_rank");
    out.pr_rank = cast<int>(json, "pr_rank");

    out.device_watts = cast<bool>(json, "device_watts");
    out.hidden = cast<bool>(json, "hidden");
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

std::string strava::exchange_token(int client_id, const std::string& client_secret, const std::string& token)
{
    auto form = std::map<std::string, std::string>
    {
        { "client_id", std::to_string(client_id) },
        { "client_secret", client_secret },
        { "code", token }
    };

    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_POST,
        "/oauth/token",
        "", form, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<Poco::JSON::Object::Ptr>();

    return json->get("access_token").toString();
}

std::string strava::deauthorization(const strava::oauth& auth)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/oauth/deauthorize",
        auth.access_token,
        {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<Poco::JSON::Object::Ptr>();

    return json->get("access_token").toString();
}

std::vector<strava::summary::athlete> strava::athlete::list_athlete_friends(const strava::oauth& auth, meta::athlete& athlete, pagination page_opt)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", athlete.id, "/friends"),
        auth.access_token, {},
        {}, page_opt
    };

    auto parser = [](auto& j, auto& a) { athlete_from_json(j, a); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json, parser);
}

std::vector<strava::summary::athlete> strava::athlete::list_athlete_friends(const strava::oauth& auth, pagination page_options)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/athlete/friends",
        auth.access_token, {},
        {}, page_options
    };

    auto parser = [](auto& j, auto& a) { athlete_from_json(j, a); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json, parser);
}

std::vector<strava::summary::athlete> strava::athlete::list_athlete_followers(const oauth& auth, meta::athlete& athlete, pagination page_option)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", athlete.id, "/followers"),
        auth.access_token, {},
        {}, page_option
    };

    auto parser = [](auto& j, auto& a) { athlete_from_json(j, a); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json, parser);
}

std::vector<strava::summary::athlete> strava::athlete::list_athlete_followers(const oauth& auth, pagination page_option)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/athlete/followers",
        auth.access_token, {},
        {}, page_option
    };

    auto parser = [](auto& j, auto& a) { athlete_from_json(j, a); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json, parser);
}

std::vector<strava::summary::athlete> strava::athlete::list_both_following(const oauth& auth, meta::athlete& athlete, pagination page_options)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", athlete.id, "/both-following"),
        auth.access_token, {},
        {}, page_options
    };

    auto parser = [](auto& j, auto& a) { athlete_from_json(j, a); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json, parser);
}

strava::detailed::athlete strava::athlete::current(const strava::oauth& auth)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/athlete",
        auth.access_token,
        {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    strava::detailed::athlete out;
    athlete_from_json(json, out);
    return out;
}

strava::summary::athlete strava::athlete::retrieve(const oauth& auth, int id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", id),
        auth.access_token,
        {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    strava::summary::athlete out;
    athlete_from_json(json, out);
    return out;
}

strava::detailed::athlete strava::athlete::update(const oauth& auth, meta::athlete athlete, std::map<std::string, std::string> updates)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_PUT,
        "/api/v3/athlete",
        auth.access_token,
        updates,
        {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::athlete value;
    athlete_from_json(json, value);
    return value;
}

strava::athlete::zones strava::athlete::get_zones(const oauth& auth)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/athlete/zones",
        auth.access_token,
        {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    strava::athlete::zones out;
    heart_rate_from_json(json->getObject("heart_rate"), out);
    power_from_json(json->getObject("power"), out);
    return out;
}

strava::athlete::stats strava::athlete::get_stats(const oauth& auth, int id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", id, "/stats"),
        auth.access_token,
        {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    strava::athlete::stats out;
    stats_from_json(json, out);
    return out;
}

std::vector<strava::detailed::segment_effort> strava::athlete::get_koms(const oauth& auth, int id, pagination page_options)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", id, "/koms"),
        auth.access_token, {},
        {}, page_options
    };

    auto parser = [](auto& j, auto& s) { parse_from_json(j, s); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<detailed::segment_effort>(json, parser);
}

strava::detailed::gear strava::gear::retrieve(const oauth& auth, const std::string& id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/gear/", id),
        auth.access_token,
        {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::gear out;
    gear_from_json(json, out);
    return out;
}

strava::detailed::segment_effort strava::segment_efforts::retrieve(const oauth& auth, std::int64_t id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/segment_efforts/", id),
        auth.access_token, {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::segment_effort value;
    parse_from_json(json, value);
    return value;
}

void parse_from_json(json_object json, strava::meta::route& route)
{
    route = {};
    route.id = cast<int>(json, "id");
    route.resource_state = cast<int>(json, "resource_state");
    route.name = cast<std::string>(json, "name");

    map_from_json(json, route.map); // fix this
}

void parse_from_json(json_object json, strava::summary::route& route)
{
    parse_from_json(json, (strava::meta::route&)route);

    athlete_from_json(json, route.athlete);

    route.description = cast<std::string>(json, "description");
    route.sub_type = cast<std::string>(json, "sub_type");
    route.type = cast<std::string>(json, "type");

    route.estimated_moving_time = cast<int>(json, "estimated_moving_time");
    route.timestamp = cast<int>(json, "timestamp");

    route.is_private = cast<bool>(json, "private");
    route.starred = cast<bool>(json, "starred");

    route.elevation_gain = cast<float>(json, "elevation_gain");
    route.distance = cast<float>(json, "distance");

}

void parse_from_json(json_object json, strava::detailed::route& route)
{
    parse_from_json(json, (strava::summary::route&)route);

    auto segments = json->getArray("segments");
    route.segments.reserve(segments->size());

    for (auto& s : *segments)
    {
        strava::detailed::segment value;
        segment_from_json(s.extract<json_object>(), value);
        route.segments.push_back(value);
    }
}

std::vector<strava::summary::route> strava::routes::list(const oauth& auth, int id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", id, "/routes"),
        auth.access_token, {}, {}
    };

    auto parser = [](auto& j, auto& s) { parse_from_json(j, s); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::route>(json, parser);
}

strava::detailed::route strava::routes::retrieve(const oauth& auth, int route_id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/routes/", route_id),
        auth.access_token, {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::route value;
    parse_from_json(json, value);
    return value;
}

void parse_from_json(json_object json, strava::summary::race& out)
{
    out.id = cast<int>(json, "id");
    out.resource_state = cast<int>(json, "id");
    out.running_race_type = cast<int>(json, "running_race_type");

    out.measurement_preference = cast<std::string>(json, "measurement_preference");
    out.country = cast<std::string>(json, "country");
    out.state = cast<std::string>(json, "state");
    out.city = cast<std::string>(json, "city");
    out.name = cast<std::string>(json, "name");
    out.url = cast<std::string>(json, "url");

    out.start_date_local = strava::time(cast<std::string>(json, "start_date_local"));
    out.distance = cast<float>(json, "distance");
}

void parse_from_json(json_object json, strava::detailed::race& out)
{
    auto integers = json->getArray("route_ids");
    parse_from_json(json, (strava::summary::race&)out);

    out.website_url = cast<std::string>(json, "website_url");
    out.route_ids.reserve(integers->size());

    for (auto& i : *integers)
    {
        out.route_ids.push_back(i.convert<int>());
    }
}

strava::detailed::race strava::races::retrieve(const oauth& auth, int race_id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/running_races/", race_id),
        auth.access_token, {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::race value;
    parse_from_json(json, value);
    return value;
}

std::vector<strava::summary::race> strava::races::list(const oauth& auth, int year)
{
    auto data = std::map<std::string, std::string>{};

    if (year != 0)
    {
        data["year"] = std::to_string(year);
    }

    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/running_races",
        auth.access_token,
        {}, data, {}
    };

    auto parser = [](auto& j, auto& s) { parse_from_json(j, s); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::race>(json, parser);
}

strava::detailed::segment strava::segments::retrieve(const oauth& auth, int id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/segments/", id),
        auth.access_token, {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::segment value;
    segment_from_json(json, value);
    return value;
}

std::vector<strava::summary::segment> strava::segments::list_starred(const oauth& auth, int athlete_id, pagination page_options)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", athlete_id, "/segments/starred"),
        auth.access_token, {},
        {}, page_options
    };

    auto parser = [](auto& j, auto& s) { segment_from_json(j, s); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::segment>(json, parser);
}

std::vector<strava::summary::segment> strava::segments::list_starred(const oauth& auth, pagination page_options)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/segments/starred",
        auth.access_token,{},
        {}, page_options
    };

    auto parser = [](auto& j, auto& s) { segment_from_json(j, s); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::segment>(json, parser);
}

strava::detailed::segment strava::segments::star(const oauth& auth, int id, bool starred)
{
    auto data = std::map<std::string, std::string>{ {"starred", (starred ? "true" : "false")} };
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_PUT,
        join("/api/v3/segments/", id, "/starred"),
        auth.access_token, {}, data, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::segment value;
    segment_from_json(json, value);
    return value;
}

std::vector<strava::summary::segment_effort> strava::segments::efforts(const oauth& auth, int id, int athlete, time_range range, pagination paging)
{
    auto data = std::map<std::string, std::string>{};

    if (athlete != 0)
    {
        data["athlete_id"] = std::to_string(athlete);
    }

    if (range.start.time_epoch != 0 && range.end.time_epoch != 0)
    {
        data["start_date_local"] = range.start.time_string;
        data["end_date_local"] = range.end.time_string;
    }

    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/segments/", id, "/all_efforts"),
        auth.access_token,
        {}, data, paging
    };

    auto parser = [](auto& j, auto& s) { parse_from_json(j, s); };
    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::segment_effort>(json, parser);
}


std::vector<strava::segments::leaderboard_entry> strava::segments::leaderboard(const oauth& auth, int id, leaderbord_params params, pagination page_option)
{
    return{};
}


std::vector<strava::summary::segment> strava::segments::explore(const oauth& auth, bounds bound, std::string activity_type, int min_cat, int max_cat)
{
    return{};
}