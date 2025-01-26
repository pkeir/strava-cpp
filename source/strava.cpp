
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
#include <Poco/JSON/Object.h>
#include <Poco/JSON/JSON.h>
#include <Poco/Dynamic/Var.h>
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

struct poco_client
{
    Poco::SharedPtr<Poco::Net::HTTPSClientSession> client;
    Poco::AutoPtr<Poco::Net::Context> context;
} client;

struct http_request
{
    std::string method, url, access_token;
    std::map<std::string, std::string> form;
    std::map<std::string, std::string> data;
    strava::pagination page_options;
};

strava::pagination::pagination(int64_t page, int64_t per_page) :
    per_page(per_page),
    page(page)
{
}

bool strava::pagination::enabled()
{
    return page != 0;
}

strava::error::error(const std::string& msg, const std::vector<error_code>& codes) :
    std::runtime_error(msg),
    error_codes(codes),
    message(msg)
{
}

const char* strava::error::what() const throw()
{
    return message.c_str();
}

const std::vector<strava::error::error_code>& strava::error::codes()
{
    return error_codes;
}

strava::datetime::datetime(std::string timestr)
{
    std::tm datetime;
    std::istringstream input(timestr);
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(&datetime, "%Y-%m-%dT%H:%M:%SZ"); // ISO 8601

    auto failed = input.fail();
    time_string = failed ? "" : timestr;
    time_epoch = failed ? 0 : mktime(&datetime);
}

strava::datetime::datetime(std::time_t num)
{
    const auto format = "%Y-%m-%dT%H:%M:%S";
    const auto size = 20;

    char buffer[size];
    strftime(buffer, size, format, localtime(&num));
    time_string = buffer;
    time_epoch = num;
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

    if (client.context.isNull())
    {
        client.context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "");
    }

    if (client.client.isNull() && !client.context.isNull())
    {
        Poco::URI uri("https://www.strava.com");

        client.client = new Poco::Net::HTTPSClientSession(uri.getHost(), uri.getPort(), client.context);
        client.client->setPort(443);
        client.client->setTimeout(Poco::Timespan(10L, 0L));

        Poco::Net::SSLManager::InvalidCertificateHandlerPtr handler(new Poco::Net::AcceptCertificateHandler(false));
        Poco::Net::SSLManager::instance().initializeClient(0, handler, client.context);
    }

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
    auto json_value = json->get(key);
    auto value = T{};

    if (json_value.isEmpty())
    {
        return value;
    }

    json_value.convert(value);
    return value;
}

template<typename T>
std::string stringify(T json)
{
    std::stringstream ss;
    json->stringify(ss, 4, 0);
    return ss.str();
}

Poco::Dynamic::Var check(Poco::Dynamic::Var response)
{
    if (response.type() == typeid(json_object))
    {
        auto json = response.extract<json_object>();
        auto error_array = json->getArray("errors");

        if (!error_array.isNull())
        {
            auto error_message = json->get("message").toString();
            auto error_codes = std::vector<strava::error::error_code>();

            for (auto& e : *error_array)
            {
                auto eo = e.extract<json_object>();
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

void parse_from_json(Poco::JSON::Object::Ptr json, strava::map_polyline& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.id = cast<std::string>(json, "id");
    out.polyline = cast<std::string>(json, "polyline");
    out.summary_polyline = cast<std::string>(json, "summary_polyline");
    out.resource_state = cast<int64_t>(json, "resource_state");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::meta::club& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.id = cast<int64_t>(json, "id");
    out.resource_state = cast<int64_t>(json, "resource_state");
    out.name = cast<std::string>(json, "name");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::summary::club& club)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::meta::club&)club);

    club.member_count = cast<int64_t>(json, "member_count");

    club.is_private = cast<bool>(json, "private");
    club.verified = cast<bool>(json, "verified");
    club.featured = cast<bool>(json, "featured");

    club.cover_photo_small = cast<std::string>(json, "cover_photo_small");
    club.profile_medium = cast<std::string>(json, "profile_medium");
    club.cover_photo = cast<std::string>(json, "cover_photo");
    club.sport_type = cast<std::string>(json, "sport_type");
    club.profile = cast<std::string>(json, "profile");
    club.country = cast<std::string>(json, "country");
    club.state = cast<std::string>(json, "state");
    club.city = cast<std::string>(json, "city");
    club.url = cast<std::string>(json, "url");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::detailed::club& club)
{
    parse_from_json(json, (strava::summary::club&)club);

    club.following_count = cast<std::int64_t>(json, "following_count");    
    club.membership = cast<std::string>(json, "membership");
    club.club_type = cast<std::string>(json, "club_type");
    club.owner = cast<bool>(json, "owner");
    club.admin = cast<bool>(json, "admin");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::summary::gear& gear)
{
    if (json.isNull())
    {
        return;
    }

    gear.resource_state = cast<int64_t>(json, "resource_state");
    gear.distance = cast<double>(json, "distance");
    gear.primary = cast<bool>(json, "primary");
    gear.name = cast<std::string>(json, "name");
    gear.id = cast<std::string>(json, "id");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::detailed::gear& gear)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::summary::gear&)gear);

    gear.brand_name = cast<std::string>(json, "brand_name");
    gear.model_name = cast<std::string>(json, "model_name");
    gear.frame_type = cast<std::string>(json, "frame_type");
    gear.description = cast<std::string>(json, "description");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::meta::athlete& athlete)
{
    if (json.isNull())
    {
        return;
    }

    athlete.id = cast<int64_t>(json, "id");
    athlete.resource_state = cast<int64_t>(json, "resource_state");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::meta::activity& out)
{
    if (json.isNull())
    {
        return;
    }

    out.id = cast<int64_t>(json, "id");
    out.resource_state = cast<int64_t>(json, "resource_state");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::summary::activity& out)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::meta::activity&)out);
    parse_from_json(json->getObject("athlete"), out.athlete);
    parse_from_json(json->getObject("map"), out.map);

    auto start_latlng = json->getArray("start_latlng");
    auto end_latlng = json->getArray("end_latlng");

    if (!start_latlng.isNull())
    {
        out.start_latlng[0] = start_latlng->get(0).extract<float>();
        out.start_latlng[1] = start_latlng->get(1).extract<float>();
    }

    if (!end_latlng.isNull())
    {
        out.end_latlng[0] = end_latlng->get(0).extract<float>();
        out.end_latlng[1] = end_latlng->get(1).extract<float>();
    }

    out.external_id = cast<std::string>(json, "external_id");
    out.name = cast<std::string>(json, "name");
    out.description = cast<std::string>(json, "description");
    out.type = cast<std::string>(json, "type");
    out.gear_id = cast<std::string>(json, "gear_id");
    out.timezone = cast<std::string>(json, "timezone");

    out.start_date_local = strava::datetime(cast<std::string>(json, "start_date_local"));
    out.start_date = strava::datetime(cast<std::string>(json, "start_date"));

    out.achievement_count = cast<std::int64_t>(json, "achievement_count");
    out.kudos_count = cast<std::int64_t>(json, "kudos_count");
    out.comment_count = cast<std::int64_t>(json, "comment_count");
    out.athlete_count = cast<std::int64_t>(json, "athlete_count");
    out.photo_count = cast<std::int64_t>(json, "photo_count");
    out.total_photo_count = cast<std::int64_t>(json, "total_photo_count");
    out.upload_id = cast<std::int64_t>(json, "upload_id");
    out.moving_time = cast<std::int64_t>(json, "moving_time");
    out.elapsed_time = cast<std::int64_t>(json, "elapsed_time");
    out.max_watts = cast<std::int64_t>(json, "max_watts");
    out.weighted_average_watts = cast<std::int64_t>(json, "weighted_average_watts");
    out.max_heartrate = cast<std::int64_t>(json, "max_heartrate");
    out.suffer_score = cast<std::int64_t>(json, "suffer_score");
    out.workout_type = cast<std::int64_t>(json, "workout_type");

    out.trainer = cast<bool>(json, "trainer");
    out.commute = cast<bool>(json, "commute");
    out.manual = cast<bool>(json, "manual");
    out.flagged = cast<bool>(json, "flagged");
    out.is_private = cast<bool>(json, "private");
    out.device_watts = cast<bool>(json, "device_watts");
    out.has_heartrate = cast<bool>(json, "has_heartrate");
    out.has_kudoed = cast<bool>(json, "has_kudoed");

    out.distance = cast<float>(json, "distance");
    out.total_elevation_gain = cast<float>(json, "total_elevation_gain");
    out.elev_high = cast<float>(json, "elev_high");
    out.elev_low = cast<float>(json, "elev_low");
    out.kilojoules = cast<float>(json, "kilojoules");
    out.average_speed = cast<float>(json, "average_speed");
    out.max_speed = cast<float>(json, "max_speed");
    out.average_cadence = cast<float>(json, "average_cadence");
    out.average_temp = cast<float>(json, "average_temp");
    out.average_watts = cast<float>(json, "average_watts");
    out.average_heartrate = cast<float>(json, "average_heartrate");
    out.calories = cast<float>(json, "calories");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::split_standard& out)
{
    out = {};
    out.elevation_difference = cast<float>(json, "elevation_difference");
    out.distance = cast<float>(json, "distance");

    out.elapsed_time = cast<std::int64_t>(json, "elapsed_time");
    out.moving_time = cast<std::int64_t>(json, "moving_time");
    out.split = cast<std::int64_t>(json, "split");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::summary::athlete& athlete)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::meta::athlete&)athlete);

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

    athlete.created_at = strava::datetime(cast<std::string>(json, "created_at"));
    athlete.updated_at = strava::datetime(cast<std::string>(json, "updated_at"));
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::detailed::athlete& athlete)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::summary::athlete&)athlete);

    athlete.measurement_preference = cast<std::string>(json, "measurement_preference");
    athlete.date_preference = cast<std::string>(json, "date_preference");
    athlete.email = cast<std::string>(json, "email");

    athlete.mutual_friend_count = cast<int64_t>(json, "mutual_friend_count");
    athlete.follower_count = cast<int64_t>(json, "follower_count");
    athlete.friend_count = cast<int64_t>(json, "friend_count");
    athlete.athlete_type = cast<int64_t>(json, "athlete_type");
    athlete.weight = cast<int64_t>(json, "weight");
    athlete.ftp = cast<int64_t>(json, "ftp");

    auto clubs = json->getArray("clubs");
    auto bikes = json->getArray("bikes");
    auto shoes = json->getArray("shoes");

    if (!clubs.isNull())
    {
        athlete.clubs.reserve(clubs->size());
        for (auto& c : *clubs)
        {
            strava::summary::club club;
            parse_from_json(c.extract<json_object>(), club);
            athlete.clubs.push_back(club);
        }
    }

    if (!shoes.isNull())
    {
        athlete.shoes.reserve(shoes->size());
        for (auto& s : *shoes)
        {
            strava::summary::gear shoe;
            parse_from_json(s.extract<json_object>(), shoe);
            athlete.shoes.push_back(shoe);
        }
    }

    if (!bikes.isNull())
    {
        athlete.bikes.reserve(bikes->size());
        for (auto& b : *bikes)
        {
            strava::summary::gear bike;
            parse_from_json(b.extract<json_object>(), bike);
            athlete.bikes.push_back(bike);
        }
    }
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::athlete::zones& out)
{
    if (json.isNull())
    {
        return;
    }

    auto heart_rate = json->getObject("heart_rate");
    auto power = json->getObject("power");
    auto heart_zones = heart_rate->get("zones");
    auto power_zones = power->get("zones");

    out.heart_rate = {};
    out.heart_rate.custom_zones = cast<bool>(json, "custom_zones");
    out.power = {};

    if (heart_zones.isArray())
    {
        for (auto& zone : *heart_zones.extract<json_array>())
        {
            auto obj = zone.extract<json_object>();
            out.heart_rate.zones.push_back({
                cast<int64_t>(obj, "min"),
                cast<int64_t>(obj, "max")
            });
        }
    }

    if (power_zones.isArray())
    {
        for (auto& zone : *power_zones.extract<json_array>())
        {
            auto obj = zone.extract<json_object>();
            out.power.zones.push_back({
                cast<int64_t>(obj, "min"),
                cast<int64_t>(obj, "max")
            });
        }
    }
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::athlete::stats::total& total)
{
    if (json.isNull())
    {
        return;
    }

    total = {};
    total.distance = cast<double>(json, "distance");
    total.elevation_gain = cast<double>(json, "elevation_gain");
    total.elapsed_time = cast<int64_t>(json, "elapsed_time");
    total.moving_time = cast<int64_t>(json, "moving_time");
    total.count = cast<int64_t>(json, "count");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::athlete::stats::detailed_total& total)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::athlete::stats::total&)total);
    total.achievement_count = cast<int64_t>(json, "achievement_count");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::athlete::stats& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.biggest_climb_elevation_gain = cast<double>(json, "biggest_climb_elevation_gain");
    out.biggest_ride_distance = cast<double>(json, "biggest_ride_distance");

    parse_from_json(json->getObject("recent_ride_totals"), out.recent_ride_totals);
    parse_from_json(json->getObject("recent_swim_totals"), out.recent_swim_totals);
    parse_from_json(json->getObject("recent_run_totals"), out.recent_run_totals);

    parse_from_json(json->getObject("ytd_ride_totals"), out.ytd_ride_totals);
    parse_from_json(json->getObject("ytd_swim_totals"), out.ytd_swim_totals);
    parse_from_json(json->getObject("ytd_run_totals"), out.ytd_run_totals);

    parse_from_json(json->getObject("all_ride_totals"), out.all_ride_totals);
    parse_from_json(json->getObject("all_swim_totals"), out.all_swim_totals);
    parse_from_json(json->getObject("all_run_totals"), out.all_run_totals);
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::summary::segment& out)
{
    if (json.isNull())
    {
        return;
    }

    out.id = cast<int64_t>(json, "id");
    out.resource_state = cast<int64_t>(json, "resource_state");
    out.climb_category = cast<int64_t>(json, "climb_category");

    out.name = cast<std::string>(json, "name");
    out.activity_type = cast<std::string>(json, "activity_type");
    out.city = cast<std::string>(json, "city");
    out.state = cast<std::string>(json, "state");
    out.country = cast<std::string>(json, "country");

    out.distance = cast<float>(json, "distance");
    out.average_grade = cast<float>(json, "average_grade");
    out.maximum_grade = cast<float>(json, "maximum_grade");
    out.elevation_high = cast<float>(json, "elevation_high");
    out.elevation_low = cast<float>(json, "elevation_low");

    auto start_elements = json->getArray("start_latlng");
    auto end_elements = json->getArray("end_latlng");

    if (!start_elements.isNull())
    {
        out.start_latlng[0] = start_elements->get(0).convert<float>();
        out.start_latlng[1] = start_elements->get(1).convert<float>();
    }

    if (!end_elements.isNull())
    {
        out.end_latlng[0] = end_elements->get(0).convert<float>();
        out.end_latlng[1] = end_elements->get(1).convert<float>();
    }

    out.is_private = cast<bool>(json, "is_private");
    out.hazardous = cast<bool>(json, "hazardous");
    out.starred = cast<bool>(json, "starred");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::detailed::segment& out)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::summary::segment&)out);
    parse_from_json(json->getObject("map"), out.map);

    out.total_elevation_gain = cast<float>(json, "total_elevation_gain");

    out.created_at = strava::datetime(cast<std::string>(json, "created_at"));
    out.updated_at = strava::datetime(cast<std::string>(json, "updated_at"));

    out.athlete_count = cast<int64_t>(json, "athlete_count");
    out.effort_count = cast<int64_t>(json, "effort_count");
    out.star_count = cast<int64_t>(json, "star_count");
}

void parse_from_json(json_object json, strava::meta::route& route)
{
    if (json.isNull())
    {
        return;
    }

    route = {};
    route.id = cast<int64_t>(json, "id");
    route.resource_state = cast<int64_t>(json, "resource_state");
    route.name = cast<std::string>(json, "name");

    parse_from_json(json->getObject("map"), route.map);
}


void parse_from_json(Poco::JSON::Object::Ptr json, strava::summary::segment_effort& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.id = cast<std::int64_t>(json, "id");
    out.name = cast<std::string>(json, "name");

    parse_from_json(json->getObject("activity"), out.activity);
    parse_from_json(json->getObject("athlete"), out.athlete);
    parse_from_json(json->getObject("segment"), out.segment);

    out.resource_state = cast<int64_t>(json, "resource_state");
    out.max_heartrate = cast<int64_t>(json, "max_heartrate");
    out.elapsed_time = cast<int64_t>(json, "elapsed_time");
    out.moving_time = cast<int64_t>(json, "moving_time");
    out.start_index = cast<int64_t>(json, "start_index");
    out.end_index = cast<int64_t>(json, "end_index");
    out.kom_rank = cast<int64_t>(json, "kom_rank");
    out.pr_rank = cast<int64_t>(json, "pr_rank");

    out.start_date_local = strava::datetime(cast<std::string>(json, "start_date_local"));
    out.start_date = strava::datetime(cast<std::string>(json, "start_date"));

    out.average_heartrate = cast<float>(json, "average_heartrate");
    out.average_cadence = cast<float>(json, "average_cadence");
    out.average_watts = cast<float>(json, "average_watts");
    out.distance = cast<float>(json, "distance");

    out.device_watts = cast<bool>(json, "device_watts");
    out.hidden = cast<bool>(json, "hidden");
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::detailed::segment_effort& out)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::summary::segment_effort&)out);
}

void parse_from_json(json_object json, strava::clubs::club_announcement& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.id = cast<std::int64_t>(json, "id");
    out.resource_state = cast<std::int64_t>(json, "resource_state");
    out.club_id = cast<std::int64_t>(json, "club_id");

    parse_from_json(json->getObject("athlete"), out.athlete);

    out.created_at = strava::datetime(cast<std::string>(json, "created_at"));
    out.message = cast<std::string>(json, "message");
}

void parse_from_json(json_object json, strava::clubs::join_response& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.active = cast<bool>(json, "active");
    out.success = cast<bool>(json, "success");
    out.membership = cast<std::string>(json, "membership");
}

void parse_from_json(json_object json, strava::clubs::leave_response& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.active = cast<bool>(json, "active");
    out.success = cast<bool>(json, "success");
}

void parse_from_json(json_object json, strava::permissions& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.edit = cast<bool>(json, "edit");
}

void parse_from_json(json_object json, strava::summary::club_event& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.id = cast<std::int64_t>(json, "id");
    out.resource_state = cast<std::int64_t>(json, "resource_state");
    out.skill_levels = cast<std::int64_t>(json, "skill_levels");
    out.terrain = cast<std::int64_t>(json, "terrain");

    out.created_at = strava::datetime(cast<std::string>(json, "created_at"));

    out.title = cast<std::string>(json, "title");
    out.description = cast<std::string>(json, "description");
    out.activity_type = cast<std::string>(json, "activity_type");
    out.address = cast<std::string>(json, "address");
    out.zone = cast<std::string>(json, "zone");

    parse_from_json(json->getObject("organizing_athlete"), out.organizing_athlete);
    parse_from_json(json->getObject("route"), out.route);
    parse_from_json(json->getObject("club"), out.club);

    auto occurrences = json->getArray("upcoming_occurrences");

    if(!occurrences.isNull())
    { 
        for (auto i = 0; i < out.upcoming_occurrences.size(); i++)
        {
            if (i < occurrences->size())
            {
                out.upcoming_occurrences[i] = occurrences->get(i).extract<std::string>();
            }
        }
    }

    auto latlng = json->getArray("start_latlng");

    if (!latlng.isNull())
    {
        out.start_latlng[0] = latlng->get(0).extract<float>();
        out.start_latlng[1] = latlng->get(1).extract<float>();
    }

    out.woman_only = cast<bool>(json, "woman_only");
    out.is_private = cast<bool>(json, "private");
    out.joined = cast<bool>(json, "joined");
}

void parse_from_json(json_object json, strava::detailed::club_event& out)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::summary::club_event&)out);
    parse_from_json(json->getObject("viewer_permissions"), out.viewer_permissions);

    out.start_datetime = strava::datetime(cast<std::string>(json, "start_datetime"));
    out.weekly_interval = cast<std::int64_t>(json, "weekly_interval");
    out.week_of_month = cast<std::int64_t>(json, "week_of_month");
    out.frequency = cast<std::string>(json, "frequency");

    auto days = json->getArray("days_of_week");

    if (!days.isNull())
    {
        out.days_of_week.reserve(days->size());

        for (auto& d : *days)
        {
            out.days_of_week.push_back(d.extract<std::string>());
        }
    }
}

void parse_from_json(json_object json, strava::summary::race& out)
{
    if (json.isNull())
    {
        return;
    }

    out.id = cast<int64_t>(json, "id");
    out.resource_state = cast<int64_t>(json, "resource_state");
    out.running_race_type = cast<int64_t>(json, "running_race_type");

    out.measurement_preference = cast<std::string>(json, "measurement_preference");
    out.country = cast<std::string>(json, "country");
    out.state = cast<std::string>(json, "state");
    out.city = cast<std::string>(json, "city");
    out.name = cast<std::string>(json, "name");
    out.url = cast<std::string>(json, "url");

    out.start_date_local = strava::datetime(cast<std::string>(json, "start_date_local"));
    out.distance = cast<float>(json, "distance");
}

void parse_from_json(json_object json, strava::detailed::race& out)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::summary::race&)out);

   
    out.website_url = cast<std::string>(json, "website_url");
   
    auto route_ids = json->getArray("route_ids");

    if (!route_ids.isNull())
    {
        out.route_ids.reserve(route_ids->size());

        for (auto& i : *route_ids)
        {
            out.route_ids.push_back(i.convert<int64_t>());
        }
    }
}

void parse_from_json(json_object json, strava::summary::route& route)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::meta::route&)route);
    parse_from_json(json->getObject("athlete"), route.athlete);

    route.description = cast<std::string>(json, "description");
    route.sub_type = cast<std::string>(json, "sub_type");
    route.type = cast<std::string>(json, "type");
    route.estimated_moving_time = cast<int64_t>(json, "estimated_moving_time");
    route.timestamp = cast<int64_t>(json, "timestamp");
    route.is_private = cast<bool>(json, "private");
    route.starred = cast<bool>(json, "starred");
    route.elevation_gain = cast<float>(json, "elevation_gain");
    route.distance = cast<float>(json, "distance");
}

void parse_from_json(json_object json, strava::detailed::route& route)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::summary::route&)route);

    auto segments = json->getArray("segments");

    if (!segments.isNull())
    {
        route.segments.reserve(segments->size());
        for (auto& s : *segments)
        {
            strava::detailed::segment value;
            parse_from_json(s.extract<json_object>(), value);
            route.segments.push_back(value);
        }
    }
}

void parse_from_json(json_object json, strava::segments::leaderboard::entry& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.athlete_profile = cast<std::string>(json, "athlete_profile");
    out.athlete_gender = cast<std::string>(json, "athlete_gender");
    out.athlete_name = cast<std::string>(json, "athlete_name");

    out.average_watts = cast<double>(json, "average_watts");
    out.average_hr = cast<double>(json, "athlete_hr");
    out.distance = cast<double>(json, "distance");

    out.athlete_id = cast<int64_t>(json, "athlete_id");
    out.activity_id = cast<int64_t>(json, "activity_id");
    out.effort_id = cast<int64_t>(json, "effort_id");
    out.elapsed_time = cast<int64_t>(json, "elapsed_time");
    out.moving_time = cast<int64_t>(json, "moving_time");
    out.rank = cast<int64_t>(json, "rank");

    out.start_date_local = strava::datetime(cast<std::string>(json, "start_date_local"));
    out.start_date = strava::datetime(cast<std::string>(json, "start_date"));
}

void parse_from_json(json_object json, strava::segments::leaderboard& out)
{
    if (json.isNull())
    {
        return;
    }
  
    out = {};
    out.entry_count = cast<int64_t>(json, "entry_count");

    auto jsonArray = json->getArray("entries");

    if (!jsonArray.isNull())
    {
        out.entries.reserve(jsonArray->size());

        for (auto& e : *jsonArray)
        {
            strava::segments::leaderboard::entry entry;
            parse_from_json(e.extract<json_object>(), entry);
            out.entries.push_back(entry);
        }
    }
}

void parse_from_json(json_object json, strava::comment& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.id = cast<std::int64_t>(json, "id");
    out.resource_state = cast<std::int64_t>(json, "resource_state");
    out.activity_id = cast<std::int64_t>(json, "activity_id");

    out.created_at = strava::datetime(cast<std::string>(json, "created_at"));
    out.text = cast<std::string>(json, "text");

    parse_from_json(json->getObject("athlete"), out.athlete);
}

void parse_from_json(json_object json, strava::photo& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.id = cast<std::int64_t>(json, "id");
    out.activity_id = cast<std::int64_t>(json, "activity_id");
    out.resource_state = cast<std::int64_t>(json, "resource_state");

    out.caption = cast<std::string>(json, "caption");
    out.type = cast<std::string>(json, "type");
    out.ref = cast<std::string>(json, "ref");
    out.uid = cast<std::string>(json, "uid");

    out.uploaded_at = strava::datetime(cast<std::string>(json, "uploaded_at"));
    out.created_at = strava::datetime(cast<std::string>(json, "created_at"));

    auto array = json->getArray("location");

    if (!array.isNull())
    {
        out.location[0] = array->get(0).extract<float>();
        out.location[1] = array->get(1).extract<float>();
    }
}

void parse_from_json(json_object json, strava::lap_effort& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.id = cast<std::int64_t>(json, "id");
    out.resource_state = cast<std::int64_t>(json, "resource_state");
    out.elapsed_time = cast<std::int64_t>(json, "elapsed_time");
    out.moving_time = cast<std::int64_t>(json, "moving_time");
    out.start_index = cast<std::int64_t>(json, "start_index");
    out.lap_index = cast<std::int64_t>(json, "lap_index");
    out.end_index = cast<std::int64_t>(json, "end_index");

    out.name = cast<std::string>(json, "name");

    out.start_date_local = strava::datetime(cast<std::string>(json, "start_date_local"));
    out.start_date = strava::datetime(cast<std::string>(json, "start_date"));

    out.distance = cast<float>(json, "distance");
    out.total_elevation_gain = cast<float>(json, "total_elevation_gain");
    out.average_speed = cast<float>(json, "average_speed");
    out.max_speed = cast<float>(json, "max_speed");
    out.average_cadence = cast<float>(json, "average_cadence");
    out.average_watts = cast<float>(json, "average_watts");
    out.average_heartrate = cast<float>(json, "average_heartrate");
    out.max_heartrate = cast<float>(json, "max_heartrate");

    parse_from_json(json->getObject("activity"), out.activity);
    parse_from_json(json->getObject("athlete"), out.athlete);
}

void parse_from_json(Poco::JSON::Object::Ptr json, strava::detailed::activity& out)
{
    if (json.isNull())
    {
        return;
    }

    parse_from_json(json, (strava::summary::activity&)out);
    parse_from_json(json->getObject("gear"), out.gear);

    out.calories = cast<float>(json, "calories");
    out.description = cast<std::string>(json, "description");
    out.device_name = cast<std::string>(json, "device_name");
    out.embed_token = cast<std::string>(json, "embed_token");

    auto splits_standard = json->getArray("splits_standard");
    auto segment_efforts = json->getArray("segment_efforts"); 
    auto splits_metric = json->getArray("splits_metric");
    auto best_efforts = json->getArray("best_efforts");
    auto laps = json->getArray("laps");

    if (!segment_efforts.isNull())
    {
        for (auto& se : *segment_efforts)
        {
            strava::summary::segment_effort value;
            parse_from_json(se.extract<json_object>(), value);
            out.segment_efforts.push_back(value);
        }
    }

    if (!best_efforts.isNull())
    {
        for (auto& se : *best_efforts)
        {
            strava::summary::segment_effort value;
            parse_from_json(se.extract<json_object>(), value);
            out.best_efforts.push_back(value);
        }
    }

    if (!splits_standard.isNull())
    {
        for (auto& se : *splits_standard)
        {
            strava::split_standard value;
            parse_from_json(se.extract<json_object>(), value);
            out.splits_standard.push_back(value);
        }
    }

    if (!splits_metric.isNull())
    {
        for (auto& se : *splits_metric)
        {
            strava::split_metric value;
            parse_from_json(se.extract<json_object>(), value);
            out.splits_metric.push_back(value);
        }
    }

    if (!laps.isNull())
    {
        for (auto& se : *laps)
        {
            strava::lap_effort value;
            parse_from_json(se.extract<json_object>(), value);
            out.laps.push_back(value);
        }
    }
}

void parse_from_json(json_object json, strava::distribution_bucket& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.min = cast<std::int64_t>(json, "min");
    out.max = cast<std::int64_t>(json, "max");
    out.time = cast<std::int64_t>(json, "time");
}

void parse_from_json(json_object json, strava::zone& out)
{
    if (json.isNull())
    {
        return;
    }

    out = {};
    out.score = cast<std::int64_t>(json, "score");
    out.resource_state = cast<std::int64_t>(json, "resource_state");
    out.points = cast<std::int64_t>(json, "points");
    out.max = cast<std::int64_t>(json, "id");

    out.sensor_based = cast<bool>(json, "sensor_based");
    out.custom_zones = cast<bool>(json, "custom_zones");
    out.type = cast<std::string>(json, "type");

    auto buckets = json->getArray("distribution_buckets");

    if (!buckets.isNull())
    {
        for (auto& b : *buckets)
        {
            strava::distribution_bucket value;
            parse_from_json(b.extract<json_object>(), value);
            out.distribution_buckets.push_back(value);
        }
    }

    out.athlete_weight = cast<double>(json, "athlete_weight");
    out.bike_weight = cast<double>(json, "bike_weight");
}

template<typename T>
std::vector<T> json_to_vector(Poco::JSON::Array::Ptr list)
{
    if (list.isNull())
    {
        return {};
    }

    std::vector<T> elements;
    elements.reserve(list->size());

    for (auto& e : *list)
    {
        T data;
        parse_from_json(e.extract<json_object>(), data);
        elements.push_back(data);
    }

    return elements;
}

std::string strava::request_access(int64_t client_id, oauth_scope scope)
{
    std::stringstream url_builder;
    url_builder << "https://www.strava.com/oauth/authorize?";
    url_builder << "client_id=" << client_id << "&";
    url_builder << "response_type=code&";
    url_builder << "redirect_uri=http://localhost/exchange_token";
    url_builder << "&approval_prompt=force&scope=";

    switch (scope)
    {
    case strava::oauth_scope::read:
        url_builder << "read";
        break;
    case strava::oauth_scope::read_all:
        url_builder << "read_all";
        break;
    case strava::oauth_scope::profile_read_all:
        url_builder << "profile:read_all";
        break;
    case strava::oauth_scope::profile_write:
        url_builder << "profile:write";
        break;
    case strava::oauth_scope::activity_read:
        url_builder << "activity:read";
        break;
    case strava::oauth_scope::activity_read_all:
        url_builder << "activity:read_all";
        break;
    case strava::oauth_scope::activity_write:
        url_builder << "activity:write";
        break;
    default:
        break;
    }

    return url_builder.str();
}

std::string strava::exchange_token(int64_t client_id, const std::string& client_secret, const std::string& token)
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
    auto json = resp.extract<json_object>();

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
    auto json = resp.extract<json_object>();

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

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json);
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

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json);
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

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json);
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

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json);
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

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json);
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
    parse_from_json(json, out);
    return out;
}

strava::summary::athlete strava::athlete::retrieve(const oauth& auth, int64_t id)
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
    parse_from_json(json, out);
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
    parse_from_json(json, value);
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
    parse_from_json(json, out);
    return out;
}

strava::athlete::stats strava::athlete::get_stats(const oauth& auth, int64_t id)
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
    parse_from_json(json, out);
    return out;
}

std::vector<strava::detailed::segment_effort> strava::athlete::get_koms(const oauth& auth, int64_t id, pagination page_options)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", id, "/koms"),
        auth.access_token, {},
        {}, page_options
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<detailed::segment_effort>(json);
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
    parse_from_json(json, out);
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

std::vector<strava::summary::route> strava::routes::list(const oauth& auth, int64_t id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", id, "/routes"),
        auth.access_token, {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::route>(json);
}

strava::detailed::route strava::routes::retrieve(const oauth& auth, int64_t route_id)
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

strava::detailed::race strava::races::retrieve(const oauth& auth, int64_t race_id)
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

std::vector<strava::summary::race> strava::races::list(const oauth& auth, int64_t year)
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

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::race>(json);
}

strava::detailed::segment strava::segments::retrieve(const oauth& auth, int64_t id)
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
    parse_from_json(json, value);
    return value;
}

std::vector<strava::summary::segment> strava::segments::list_starred(const oauth& auth, int64_t athlete_id, pagination page_options)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/athletes/", athlete_id, "/segments/starred"),
        auth.access_token, {},
        {}, page_options
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::segment>(json);
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

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::segment>(json);
}

strava::detailed::segment strava::segments::star(const oauth& auth, int64_t id, bool starred)
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
    parse_from_json(json, value);
    return value;
}

std::vector<strava::summary::segment_effort> strava::segments::efforts(const oauth& auth, int64_t id, int64_t athlete, datetime_range range, pagination paging)
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

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::segment_effort>(json);
}

auto params_to_map(strava::segments::leaderboard_params& params)
{
    auto data = std::map<std::string, std::string>{};

    if (params.club_id != 0)
        data["club_id"] = std::to_string(params.club_id);
    if (params.context_entries != 0)
        data["context_entries"] = std::to_string(params.context_entries);
    if (!params.age_group.empty())
        data["age_group"] = params.age_group;
    if (!params.gender.empty())
        data["gender"] = params.gender;
    if (!params.weight_class.empty())
        data["weight_class"] = params.weight_class;
    if (!params.date_range.empty())
        data["date_range"] = params.date_range;
    if (params.following)
        data["following"] = "true";

    return data;
}

strava::segments::leaderboard strava::segments::get_leaderboard(const oauth& auth, int64_t id, leaderboard_params params, pagination paging)
{
    auto data = params_to_map(params);
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/segments/", id, "/leaderboard"),
        auth.access_token,
        {}, data, paging
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    leaderboard value;
    parse_from_json(json, value);
    return value;
}

std::vector<strava::summary::segment> strava::segments::explore(const oauth& auth, bounds bound, std::string activity_type, int64_t min_cat, int64_t max_cat)
{
    auto data = std::map<std::string, std::string>{};
    auto ss = std::stringstream{};

    ss << std::setprecision(17);
    ss << bound.ne_lat << ",";
    ss << bound.ne_lng << ",";
    ss << bound.sw_lat << ",";
    ss << bound.sw_lng;

    data["bounds"] = ss.str();

    if (activity_type != "")
    {
        data["activity_type"] = activity_type;
    }

    if (min_cat != 0)
    {
        data["min_cat"] = std::to_string(min_cat);
    }

    if (max_cat != 0)
    {
        data["max_cat"] = std::to_string(max_cat);
    }

    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/segments/explore",
        auth.access_token,
        {}, data, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();
    auto segments = json->getArray("segments");

    return json_to_vector<summary::segment>(segments);
}

strava::summary::club_event strava::clubs::events::retrieve(const oauth& auth, std::int64_t id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/group_events/", id),
        auth.access_token,
        {}, {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    strava::summary::club_event value;
    parse_from_json(json, value);
    return value;
}

std::vector<strava::summary::club_event> strava::clubs::events::list(const oauth& auth, std::int64_t club_id, bool upcoming)
{
    auto data = std::map<std::string, std::string>{};
    data["upcoming"] = upcoming ? "true" : "false";

    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/clubs/", club_id, "/group_events"),
        auth.access_token,
        {}, data, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::club_event>(json);
}

bool strava::clubs::events::join_event(const oauth& auth, std::int64_t event_id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_POST,
        join("/api/v3/group_events/", event_id, "/rsvps"),
        auth.access_token,
        {},{},{}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    return cast<bool>(json, "joined");
}

bool strava::clubs::events::leave_event(const oauth& auth, std::int64_t event_id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_DELETE,
        join("/api/v3/group_events/", event_id, "/rsvps"),
        auth.access_token,
        {},{},{}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    return cast<bool>(json, "joined");
}

void strava::clubs::events::delete_event(const oauth& auth, std::int64_t event_id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_DELETE,
        join("/api/v3/group_events/", event_id),
        auth.access_token,
        {},{},{}
    };

    check(send(request));
}

strava::detailed::club strava::clubs::retrieve(const oauth& auth, std::int64_t id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/clubs/", id),
        auth.access_token,
        {},{},{}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::club value;
    parse_from_json(json, value);
    return value;
}

std::vector<strava::clubs::club_announcement> strava::clubs::list_announcements(const oauth& auth, std::int64_t club_id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/clubs/", club_id),
        auth.access_token,
        {},{},{}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<club_announcement>(json);
}

std::vector<strava::summary::athlete> strava::clubs::events::list_joined_athletes(const oauth& auth, std::int64_t event_id, pagination pagination)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/group_events/", event_id, "/athletes"),
        auth.access_token, {}, {},
        pagination
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json);
}

std::vector<strava::summary::club> strava::clubs::list_athlete_clubs(const oauth& auth)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/athlete/clubs",
        auth.access_token,
        {},{},{}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::club>(json);
}

std::vector<strava::summary::athlete> strava::clubs::list_club_members(const oauth& auth, std::int64_t club_id, pagination pagination)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/clubs/", club_id, "/members"),
        auth.access_token,
        {}, {}, pagination
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json);
}

std::vector<strava::summary::athlete> strava::clubs::list_club_admin(const oauth& auth, std::int64_t club_id, pagination pagination)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/clubs/", club_id, "/admins"),
        auth.access_token,
        {}, {}, pagination
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::athlete>(json);
}

std::vector<strava::summary::activity> strava::clubs::list_club_activities(const oauth& auth, std::int64_t club_id, datetime before, pagination pagination)
{
    auto data = std::map<std::string, std::string>{};

    if (before.time_epoch != 0)
    {
        data["before"] = std::to_string(before.time_epoch);
    }

    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/clubs/", club_id, "/activities"),
        auth.access_token,
        {}, data, pagination
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::activity>(json);
}

strava::clubs::join_response strava::clubs::join_club(const oauth& auth, std::int64_t club_id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/clubs/", club_id, "/join"),
        auth.access_token,
        {}, {}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    join_response value;
    parse_from_json(json, value);
    return value;
}

strava::clubs::leave_response strava::clubs::leave_club(const oauth& auth, std::int64_t club_id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/clubs/", club_id, "/leave"),
        auth.access_token,
        {},{},{}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    leave_response value;
    parse_from_json(json, value);
    return value;
}

std::vector<strava::comment> strava::activity::list_comments(const oauth& auth, std::int64_t id, pagination paging)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/activities/", id, "/comments"),
        auth.access_token,
        {}, {}, paging
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<strava::comment>(json);
}

std::vector<strava::summary::activity> strava::activity::list_kudos(const oauth& auth, std::int64_t id, pagination paging)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/activities/", id, "/kudos"),
        auth.access_token,
        {}, {}, paging
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::activity>(json);
}

std::vector<strava::photo> strava::activity::list_photos(const oauth& auth, std::int64_t id, bool photo_source, std::int64_t size)
{
    auto data = std::map<std::string, std::string>{};
    data["photo_source"] = photo_source ? "true" : "false";

    if (size != 0)
    {
        data["size"] = std::to_string(size);
    }

    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/activities/", id, "/photos"),
        auth.access_token,
        {},{}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<photo>(json);
}

strava::detailed::activity strava::activity::retrieve(const oauth& auth, std::int64_t id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/activities/", id),
        auth.access_token,
        {},{},{}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::activity value;
    parse_from_json(json, value);
    return value;
}

strava::detailed::activity strava::activity::update(const oauth& auth, std::int64_t id, std::map<std::string, std::string> updates)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_PUT,
        join("/api/v3/activities/", id),
        auth.access_token,
        updates, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_object>();

    detailed::activity value;
    parse_from_json(json, value);
    return value;
}

std::vector<strava::summary::activity> strava::activity::list(const oauth& auth, datetime before, datetime after, pagination pagination)
{
    auto data = std::map<std::string, std::string>{};

    if (before.time_epoch != 0)
    {
        data["before"] = std::to_string(before.time_epoch);
    }

    if (after.time_epoch != 0)
    {
        data["after"] = std::to_string(after.time_epoch);
    }

    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/athlete/activities",
        auth.access_token,
        {}, data, pagination
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::activity>(json);
}

std::vector<strava::summary::activity> strava::activity::list_related(const oauth& auth, std::int64_t id, pagination pagination)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/activities/", id, "/related"),
        auth.access_token,
        {},{}, pagination
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::activity>(json);
}

std::vector<strava::summary::activity> strava::activity::list_friends(const oauth& auth, pagination pagination)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        "/api/v3/athlete/activities",
        auth.access_token,
        {}, {}, pagination
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<summary::activity>(json);
}

std::vector<strava::zone> strava::activity::list_zones(const oauth& auth, std::int64_t id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/activities/", id, "/zones"),
        auth.access_token,
        {},{}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<zone>(json);
}

std::vector<strava::lap_effort> strava::activity::list_laps(const oauth& auth, std::int64_t id)
{
    auto request = http_request
    {
        Poco::Net::HTTPRequest::HTTP_GET,
        join("/api/v3/activities/", id, "/laps"),
        auth.access_token,
        {},{}, {}
    };

    auto resp = check(send(request));
    auto json = resp.extract<json_array>();

    return json_to_vector<lap_effort>(json);
}
