

/// Placeholder license (MIT) cough cough cough...
///
///  Copyright(c) 2017 Paul Keir, William Taylor
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files(the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions :
///
///  The above copyright notice and this permission notice shall be included in all
///  copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///  SOFTWARE.

#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <array>
#include <ctime>
#include <map>

///
/// Entire API is found in the strava namespace,
/// don't import it int64_to the global namespace,
/// as many objects and methods have opaque
/// names which could lead to conflicts
///
namespace strava
{
    ///
    /// No standard time object in C++, so this struct
    /// provides the time string from the Strava API
    /// and a copy of that time as a time_t type.
    ///
    struct time
    {
        std::string time_string;
        std::time_t time_epoch;

        time(std::string iso_string);
        time(std::time_t timestamp);
        time() = default;
    };

    ///
    /// Time range struct for date queries.
    ///
    struct time_range
    {
        time start;
        time end;
    };

    ///
    /// Simple pagination struct for specifying
    /// pagination for large requests.
    ///
    struct pagination
    {
        int64_t page;
        int64_t per_page;

        pagination(int64_t page = 0, int64_t per_page = 30);
        bool enabled();
    };

    ///
    /// Polyline struct whic details a path
    ///
    struct polyline
    {
        std::string id;
        std::string line;
        int64_t resource_state;
    };

    ///
    /// Custom excpetion cast which mirrors
    /// the error object returned from Strava
    /// when something goes wrong.
    /// 
    class error : public std::runtime_error
    {
    public:
        struct error_code
        {
            std::string resource;
            std::string field;
            std::string code;
        };
    private:
        std::vector<error_code> error_codes;
        std::string message;
    public:
        error(const std::string& msg, const std::vector<error_code>& codes);
        const char* what() const override;
        const std::vector<error_code>& codes();
    };

    ///
    /// Simple struct that sets out details
    /// needed for authentication.
    ///
    struct oauth
    {
        int64_t client_id;
        std::string client_secret;
        std::string access_token;
    };

    ///
    /// Scope enum for each type
    /// of permission.
    ///
    enum oauth_scope
    {
        scope_public,
        scope_write,
        scope_view_write,
        scope_view_private_write
    };

    ///
    /// Generates a url for you to view in a browser
    /// to grant access to your data.
    ///
    /// int64_t client_id - The client id for your application
    /// oauth_scope scope - Access level for the athletes data
    ///
    std::string request_access(int64_t client_id, oauth_scope scope);

    ///
    /// Generates a url for you to view in a browser
    /// to grant access to your data.
    ///
    /// int64_t client_id - The client id for your application
    /// std::string client_secret - The client secret for your application
    /// std::string token - The token you received by opening the url from request_access
    ///
    std::string exchange_token(int64_t client_id, const std::string& client_secret, const std::string& token);

    ///
    /// Call this method to deauthorize an access_token returned
    /// by the function 'exchange_token'. Once you call this,
    /// the access token will no longer be able to access data.
    ///
    std::string deauthorization(const oauth& auth);

    ///
    /// Namespace of constants specified by the Strava API.
    ///
    namespace constants
    {
        /// See https://strava.github.io/api/v3/running_races/ distances section
        const auto km_100 = 100000.0;
        const auto km_50 = 50000.0;
        const auto km_10 = 10000.0;
        const auto km_5 = 5000.0;

        const auto mi_10 = 16093.4;
        const auto mi_5 = 8046.70;
        const auto mi_1 = 1609.34;

        const auto half_marathon = 21097.0;
        const auto marathon = 42195.0;
    }

    ///
    /// You get three types of representation with strava, a meta repr a summary repr and
    /// a detailed repr. Here they are split int64_to seperate namespaces
    /// for clarity.
    ///
    namespace meta
    {
        /// Athlete metadata info
        struct athlete
        {
            int64_t id;
            int64_t resource_state;
        };

        /// Activity metadata info
        struct activity
        {
            int64_t id;
            int64_t resouce_state;
        };

        /// Club metadata info
        struct club
        {
            int64_t id;
            int64_t resource_state;
            std::string name;
        };

        /// Route metadata info
        struct route
        {
            int64_t id;
            int64_t resource_state;
            std::string name;
            polyline map;
        };

        /// Race metadata info
        struct race
        {
        };

        /// Gear metadata info
        struct gear
        {
        };

        /// Segment metadata info
        struct segment
        {
        };

        /// Effort metadata info
        struct segment_effort
        {
        };
    }

    ///
    /// You get three types of representation with strava, a meta repr a summary repr and
    /// a detailed repr. Here they are split int64_to seperate namespaces
    /// for clarity.
    ///
    namespace summary
    {
        /// Athlete summary info
        struct athlete : public meta::athlete
        {
            std::string firstname;
            std::string lastname;
            std::string profile_medium;
            std::string profile;
            std::string city;
            std::string state;
            std::string country;
            std::string sex;
            std::string is_friend;
            std::string follower;

            bool premium;

            time created_at;
            time updated_at;
        };

        /// Club summary info
        struct club : public meta::club
        {
            int64_t member_count;

            std::string profile_medium;
            std::string profile;
            std::string cover_photo;
            std::string cover_photo_small;
            std::string sport_type;
            std::string city;
            std::string state;
            std::string country;
            std::string url;

            bool is_private;
            bool verified;
            bool featured;
        };

        /// Gear summary info
        struct gear : public meta::gear
        {
            bool primary;
            double distance;
            int64_t resource_state;
            std::string id;
            std::string name;
        };

        /// Segment summary info
        struct segment : public meta::segment
        {
            int64_t id;
            int64_t resource_state;
            int64_t climb_category;

            std::string name;
            std::string activity_type;
            std::string city;
            std::string state;
            std::string country;

            float distance;
            float average_grade;
            float maximum_grade;
            float elevation_high;
            float elevation_low;

            std::array<int64_t, 2> start_latlng;
            std::array<int64_t, 2> end_latlng;

            bool is_private;
            bool hazardous;
            bool starred;
        };

        /// Effort summary info
        struct segment_effort
        {
            std::int64_t id;
            std::string name;

            summary::segment segment;
            meta::activity activity;
            meta::athlete athlete;

            int64_t resource_state;
            int64_t elapsed_time;
            int64_t moving_time;
            int64_t start_index;
            int64_t end_index;
            int64_t max_heartrate;
            int64_t kom_rank;
            int64_t pr_rank;

            time start_date;
            time start_date_local;

            float distance;
            float average_cadence;
            float average_watts;
            float average_heartrate;

            bool device_watts;
            bool hidden;
        };

        /// Route summary info
        struct route : public meta::route
        {
            summary::athlete athlete;

            float distance;
            float elevation_gain;

            std::string description;
            std::string type;
            std::string sub_type;

            bool is_private;
            bool starred;

            int64_t timestamp;
            int64_t estimated_moving_time;
        };

        /// Race summary info 
        struct race : public meta::race
        {
            int64_t id;
            int64_t resource_state;
            int64_t running_race_type;

            std::string name;
            std::string city;
            std::string state;
            std::string country;
            std::string measurement_preference;
            std::string url;

            float distance;

            time start_date_local;
        };
    }

    ///
    /// You get three types of representation with strava, a meta repr a summary repr and
    /// a detailed repr. Here they are split int64_to seperate namespaces
    /// for clarity.
    ///
    namespace detailed
    {
        /// Athlete detailed info
        struct athlete : public summary::athlete
        {
            int64_t follower_count;
            int64_t mutual_friend_count;
            int64_t friend_count;
            int64_t athlete_type;
            int64_t weight;
            int64_t ftp;

            std::string date_preference;
            std::string measurement_preference;
            std::string email;

            std::vector<summary::club> clubs;
            std::vector<summary::gear> bikes;
            std::vector<summary::gear> shoes;
        };

        /// Club detailed info
        struct club : public summary::club
        {
            int64_t following_count;

            std::string membership;
            std::string club_type;

            bool admin;
            bool owner;
        };

        /// Gear detailed info
        struct gear : public summary::gear
        {
            std::string brand_name;
            std::string model_name;
            std::string frame_type;
            std::string description;
        };

        /// Segment detailed info
        struct segment : public summary::segment
        {
            int64_t effort_count;
            int64_t athlete_count;
            int64_t star_count;

            polyline map;

            time created_at;
            time updated_at;

            float total_elevation_gain;
        };

        /// Effort detailed info
        struct segment_effort : public summary::segment_effort
        {
            /// Empty as detailed + summary representation is the same 
        };

        /// Route detailed info
        struct route : public summary::route
        {
            std::vector<summary::segment> segments;
        };

        /// Route detailed info
        struct race : public summary::race
        {
            std::vector<int64_t> route_ids;
            std::string website_url;
        };
    }

    ///
    /// Athlete namespace for each method which
    /// grabs some important athlete info from 
    /// the API.
    ///
    namespace athlete
    {
        ///
        /// Lists friends for the current athlete. Pagination is supported.
        ///
        /// const oauth& auth - Authorization info
        /// int64_t page - The page to display (disabled by default)
        /// int64_t per_page - The number of entries per page
        ///
        std::vector<summary::athlete> list_athlete_friends(const oauth& auth, pagination page_opt = {});

        ///
        /// Lists friends for the provided athlete. Pagination is supported.
        ///
        /// const oauth& auth - Authorization info
        /// meta::athlete& athlete - The athlete to get friends from
        /// int64_t page - The page to display (disabled by default)
        /// int64_t per_page - The number of entries per page
        ///
        std::vector<summary::athlete> list_athlete_friends(const oauth& auth, meta::athlete& athlete, pagination page_opt = {});

        ///
        /// Lists followers for the current athlete. Pagination is supported.
        ///
        /// const oauth& auth - Authorization info
        /// int64_t page - The page to display (disabled by default)
        /// int64_t per_page - The number of entries per page
        ///
        std::vector<summary::athlete> list_athlete_followers(const oauth& auth, meta::athlete& athlete, pagination page_opt = {});

        ///
        /// Lists followers for the provided athlete. Pagination is supported.
        ///
        /// const oauth& auth - Authorization info
        /// meta::athlete& athlete - The athlete to get followers from
        /// int64_t page - The page to display (disabled by default)
        /// int64_t per_page - The number of entries per page
        ///
        std::vector<summary::athlete> list_athlete_followers(const oauth& auth, pagination page_opt = {});

        ///
        ///  List athletes that both the current athlete and given athlete
        ///  are following.
        ///
        /// const oauth& auth - Authorization info
        /// meta::athlete& athlete - The athlete to find shared followers for
        /// int64_t page - The page to display (disabled by default)
        /// int64_t per_page - The number of entries per page
        ///  
        std::vector<summary::athlete> list_both_following(const oauth& auth, meta::athlete& athlete, pagination page_opt = {});

        ///
        /// Returns the current athlete.
        ///
        /// const oauth& auth - Authorization info
        ///
        detailed::athlete current(const oauth& auth);

        ///
        /// Gets an athlete by id.
        ///
        /// const oauth& auth - Authorization info
        /// int64_t id - The athlete to get
        ///
        summary::athlete retrieve(const oauth& auth, int64_t id);

        ///
        /// Updates the athlete by id.
        ///
        /// const oauth& auth - Authorization info
        /// meta::athlete athlete - The athlete to update
        /// std::map<std::string, std::string> - Map of updates e.g {"weight", "50.0"}
        ///
        detailed::athlete update(const oauth& auth, meta::athlete athlete, std::map<std::string, std::string> updates);

        ///
        /// Zone attached to an athlete which
        /// denotes both a min and max value.
        ///
        struct zone
        {
            int64_t min;
            int64_t max;
        };

        ///
        /// Heart rate zone struct which is 
        /// part of the zones object returned
        /// by athlete list zones.
        ///
        struct athlete_heart_rate
        {
            bool custom_zones;
            std::vector<zone> zones;
        };

        ///
        /// Power zone struct which is 
        /// part of the zones object returned
        /// by athlete list zones.
        ///
        struct athlete_power
        {
            std::vector<zone> zones;
        };

        ///
        /// Zones is a combination of both heart_rate
        /// zones and power zones. (Power zones is a 
        /// premium feature).
        ///
        struct zones
        {
            athlete_heart_rate heart_rate;
            athlete_power power;
        };

        ///
        /// Recent stats within 4 weeks which is tied
        /// to a given athlete.
        ///
        struct stats
        {
            /// Two types of entries one with an achievment count,
            /// and one without. These are split int64_to two structs
            /// total and detailed_total.
            struct total
            {
                double distance;
                double elevation_gain;

                int64_t count;
                int64_t moving_time;
                int64_t eleapsed_time;
                int64_t achievement_count;
            };

            /// See above comment
            struct detailed_total : public total
            {
                int64_t achievement_count;
            };

            detailed_total recent_ride_totals;
            detailed_total recent_swim_totals;
            detailed_total recent_run_totals;

            double biggest_ride_distance;
            double biggest_climb_elevation_gain;

            total ytd_swim_totals;
            total ytd_ride_totals;
            total ytd_run_totals;

            total all_ride_totals;
            total all_swim_totals;
            total all_run_totals;
        };

        ///
        /// Gets an zones for the current athlete.
        ///
        /// const oauth& auth - Authorization info
        ///
        zones get_zones(const oauth& auth);

        ///
        /// Gets an stats for the current athlete.
        ///
        /// const oauth& auth - Authorization info
        ///
        stats get_stats(const oauth& auth, int64_t id);

        ///
        /// Gets an koms for the given athlete.
        ///
        /// const oauth& auth - Authorization info
        /// int64_t id - The athlete to get
        ///
        std::vector<strava::detailed::segment_effort> get_koms(const oauth& auth, int64_t id, pagination page_opt = {});
    }

    ///
    ///
    ///
    namespace activity
    {
        // list photos
        // list comments
        // list kudos
        // create activity
        // retrieve activity
        // update an activity
        // list athlete activities
        // list related activities
        // list friends activities
        // list activity zones
        // list activity laps
    }

    ///
    ///
    ///
    namespace clubs
    {
        // retrieve club
        // list club announcments
        // list athlete clubs
        // list club members
        // list club admins
        // list clubs activities
        // join club
        // leave club
    }

    ///
    /// Gear functionality wrapped in the gear namespace
    ///
    namespace gear
    {
        ///
        /// Retrieves a gear via id, representation
        /// returned is detailed and not a summary.
        ///
        /// const oauth& auth - Authorization info 
        /// const std::string& id - The gear id to get
        ///
        detailed::gear retrieve(const oauth& auth, const std::string& id);
    }

    ///
    /// Route functionality wrapped in the route namespace
    ///
    namespace routes
    {
        ///
        /// Returns a vector of routes tied to the athletes id.
        ///
        /// const oauth& auth - Authorization info
        /// int64_t id - The athlete id
        ///
        std::vector<summary::route> list(const oauth& auth, int64_t id);

        ///
        /// Returns a detailed route by the given id.
        ///
        /// const oauth& auth - Authorization info
        /// int64_t route_id - The route's id
        ///
        detailed::route retrieve(const oauth& auth, int64_t route_id);
    }

    ///
    /// Race functionality wrapped in namespace
    ///
    namespace races
    {
        ///
        /// Returns a detailed race by the given id.
        ///
        /// const oauth& auth - Authorization info
        /// int64_t race_id - The race's id
        ///
        detailed::race retrieve(const oauth& auth, int64_t race_id);

        ///
        /// Returns a vector of races
        ///
        /// const oauth& auth - Authorization info
        /// int64_t year - The year to get races for
        ///
        std::vector<summary::race> list(const oauth& auth, int64_t year = 0);
    }

    ///
    /// Segment efforts namespace for each method which
    /// provdes segment effort info.
    ///
    namespace segment_efforts
    {
        ///
        /// Gets segment effort by id.
        ///
        /// const oauth& auth - Authorization info
        /// int64_t id - The segment effort to get
        ///
        detailed::segment_effort retrieve(const oauth& auth, std::int64_t id);
    }

    ///
    /// Segment functionality wrapped in namespace
    ///
    namespace segments
    {
        ///
        /// Leaderboard struct returned from the
        /// the leaderboard segment endpoint.
        ///
        struct leaderboard
        {
            ///
            /// Individual entries in this table
            /// mirror this structure.
            ///
            struct entry
            {
                int64_t athlete_id, activity_id, effort_id;
                int64_t elapsed_time, moving_time, rank;

                std::string athlete_name;
                std::string athlete_gender;
                std::string athlete_profile;

                double average_watts, average_hr;
                double distance;

                time start_date, start_date_local;
            };

            int64_t entry_count;
            std::vector<entry> entries;
        };

        ///
        /// LatLng quad for exploring for segments
        ///
        struct bounds
        {
            double ne_lat, ne_lng;
            double sw_lat, sw_lng;
        };

        ///
        /// Params for querying the leaderboard endpoint.
        ///
        struct leaderbord_params
        {
            int64_t club_id;
            int64_t context_entries;

            bool following;

            std::string gender;
            std::string age_group;
            std::string weight_class;
            std::string date_range;
        };

        ///
        /// Retrieves a segment via id. Segments can be
        /// retrieved via routes which can be listed
        /// via athlete id.
        ///
        detailed::segment retrieve(const oauth& auth, int64_t id);

        ///
        /// Listed starred segments for the given athlete
        ///
        std::vector<summary::segment> list_starred(const oauth& auth, int64_t athlete_id, pagination page_option = {});

        ///
        /// Listed starred segments for the current athlete
        ///
        std::vector<summary::segment> list_starred(const oauth& auth, pagination page_option = {});

        ///
        /// Stars a segment for the current athlete
        ///
        detailed::segment star(const oauth& auth, int64_t id, bool starred);

        ///
        /// Lists segment efforts.
        ///
        std::vector<summary::segment_effort> efforts(const oauth& auth, int64_t id, int64_t athlete = 0, time_range range = {}, pagination page_option = {});

        ///
        /// Gets leaderboard for a given segment
        ///
        leaderboard get_leaderboard(const oauth& auth, int64_t id, leaderbord_params params = {}, pagination page_option = {});

        ///
        /// Finds segments in a given latlng bounding box.
        ///
        std::vector<summary::segment> explore(const oauth& auth, bounds bound, std::string activity_type = "", int64_t min_cat = 0, int64_t max_cat = 0);
    }

    ///
    ///
    ///
    namespace stream
    {
        ///
        /// Types namespace to denote vector type for 
        /// stream::object<T>
        ///
        namespace types
        {
            struct latlng { using type = std::array<int64_t, 2>; };
            struct time { using type = int64_t; };
        }

        template<typename T>
        struct object
        {
            std::string type;
            std::vector<T> data;
            std::string series_type;
            int64_t original_size;
            std::string resolution;
        };

        template<typename T>
        object<typename T::type> retrieve_activity() {
            object<typename T::type> out;
            out.data.push_back(typename T::type{});
            return out;
        }

        // retrieve effort stream
        void retrieve_effort();

        // retrieve segment stream
        void retrieve_segment();

        // retrieve route stream
        void retrieve_route();
    }
}