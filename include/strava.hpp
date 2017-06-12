
// Placeholder license (MIT) cough cough cough...
//
//  Copyright(c) <year> <copyright holders>
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files(the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions :
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#pragma once

#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/NetSSL.h>
#include <Poco/SharedPtr.h>
#include <memory>
#include <string>
#include <vector>
#include <time.h>

namespace strava
{
    ///
    /// Client struct which keeps
    /// a Poco https session and the context
    /// for SSL https server/client
    ///
    struct client
    {
        // Poco::AutoPtr = std::unqiue_ptr
        Poco::SharedPtr<Poco::Net::HTTPSClientSession> session;
        Poco::AutoPtr<Poco::Net::Context> context;
    };

    ///
    /// Simple struct that sets out details
    /// needed for authentication.
    ///
    struct oauth
    {
        std::string access_token;
        std::string redirect_url;
        std::string client_secret;
        std::string client_id;
    };

    ///
    /// Authenticates and sets up the https client.
    ///
    /// oauth&& info   - The auth info requred by the Strava v3 API
    /// bool skip_init - Should we initialise the https client (default: false) 
    /// 
    void authenticate(oauth&& info, bool skip_init = false);

    ///
    /// You get three types of representation with strava, a meta repr a summary repr and
    /// a detailed repr. Here they are split into seperate namespaces
    /// for clarity.
    ///
    namespace meta
    {
        ///
        ///
        ///
        struct athlete
        {
            int id;
            int resource_state;
        };

        ///
        ///
        ///
        struct club
        {
            int id;
            int resource_state;
            std::string name;
        };

        ///
        ///
        ///
        struct route
        {
            int id;
            int resource_state;
            std::string name;
            // map object??? http://strava.github.io/api/v3/routes/#maps
        };

        ///
        /// Race metadata empty
        ///
        struct race
        {
        };

        ///
        /// Gear metadata empty
        ///
        struct gear
        {
        };

        ///
        /// Segment metadata empty
        ///
        struct segment
        {
        };

        ///
        /// Segment Effort metadata empty
        ///
        struct segment_effort
        {
        };
    }

    ///
    /// You get three types of representation with strava, a meta repr a summary repr and
    /// a detailed repr. Here they are split into seperate namespaces
    /// for clarity.
    ///
    namespace summary
    {
        ///
        ///
        ///
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

            time_t created_at;
            time_t updated_at;
        };

        ///
        ///
        ///
        struct club : public meta::club
        {
            int id;
            int resource_state;
            int member_count;

            std::string name;
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
            bool featured;
        };

        ///
        ///
        ///
        struct gear : public meta::gear
        {
            bool primary;
            double distance;
            int resource_state;
            std::string id;
            std::string name;
        };
    }

    ///
    /// You get three types of representation with strava, a meta repr a summary repr and
    /// a detailed repr. Here they are split into seperate namespaces
    /// for clarity.
    ///
    namespace detailed
    {
        ///
        ///
        ///
        struct athlete : public summary::athlete
        {
            int follower_count;
            int mutual_friend_count;
            int friend_count;
            int athlete_type;
            int weight;
            int ftp;

            std::string date_preference;
            std::string measurement_preference;
            std::string email;

            std::vector<summary::club> clubs;
            std::vector<summary::gear> bikes;
            std::vector<summary::gear> shoes;
        };

        ///
        ///
        ///
        struct club : public summary::club
        {

        };

        ///
        ///
        ///
        struct gear : public summary::gear
        {

        };
    }

    namespace update
    {
        struct athlete
        {
            float weight;
            std::string city;
            std::string state;
            std::string country;
            std::string sex;
        };
    }

    ///
    ///
    ///
    namespace athlete
    {
        ///
        ///
        ///
        struct zones
        {
            ///
            ///
            ///
            struct zone { int min, max; };

            ///
            ///
            ///
            struct heart_rate_struct
            {
                bool custom_zones;
                std::vector<zone> zones;
            };

            ///
            ///
            ///
            struct power_struct
            {
                std::vector<zone> zones;
            };

            heart_rate_struct heart_rate;
            power_struct power;
        };


        ///
        ///
        ///
        struct statistics
        {
            ///
            ///
            ///
            struct total_no_ac
            {
                double distance;
                double elevation_gain;

                int count;
                int moving_time;
                int eleapsed_time;
                int achievement_count;
            };

            ///
            ///
            ///
            struct total : public total_no_ac
            {
                int achievement_count;
            };

            double biggest_ride_distance;
            double biggest_climb_elevation_gain;

            std::vector<total_no_ac> ytd_swim_totals;
            std::vector<total_no_ac> ytd_ride_totals;
            std::vector<total_no_ac> ytd_run_totals;
            std::vector<total_no_ac> all_ride_totals;
            std::vector<total_no_ac> all_swim_totals;
            std::vector<total_no_ac> all_run_totals;
            std::vector<total> recent_ride_totals;
            std::vector<total> recent_swim_totals;
            std::vector<total> recent_run_totals;
        };

        struct kqom_c
        {
            // Array of segment efforts http://strava.github.io/api/v3/efforts/
        };

        ///
        /// 
        ///
        void retrieve(int id, summary::athlete& out);

        ///
        /// 
        ///
        void current(detailed::athlete& out);

        ///
        /// 
        ///
        void update(update::athlete update, detailed::athlete updated_out);

        ///
        /// 
        ///
        void get_zones(zones& out);

        ///
        /// 
        ///
        void get_stats(detailed::athlete& athlete, statistics& stats);

        ///
        /// 
        ///
        void list_kqom_cr(detailed::athlete& athlete, kqom_c& out);
    }

    ///
    ///
    ///
    namespace activities
    {

    }

    ///
    ///
    ///
    namespace clubs
    {

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
        void retrieve(int id, detailed::gear& out);
    }

    ///
    ///
    ///
    namespace routes
    {

    }

    ///
    ///
    ///
    namespace races
    {

    }

    ///
    ///
    ///
    namespace segments
    {

    }

    ///
    ///
    ///
    namespace streams
    {

    }

    ///
    ///
    ///
    namespace uploads
    {

    }
}