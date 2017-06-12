
#pragma once

#include <string>
#include <vector>
#include <time.h>

namespace strava
{
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

        };

        ///
        ///
        ///
        struct shoe
        {

        };

        ///
        ///
        ///
        struct bike
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
        struct bike : public meta::bike
        {
            bool primary;
            double distance;
            int resource_state;
            std::string id;
            std::string name;
        };

        ///
        ///
        ///
        struct shoe : public meta::shoe
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
            std::vector<summary::bike> bikes;
            std::vector<summary::shoe> shoes;
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
        struct shoe : public summary::shoe
        {

        };

        ///
        ///
        ///
        struct bike : public summary::bike
        {

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
        void retrieve(int id, summary::athlete& out);

        ///
        /// 
        ///
        void current(detailed::athlete& out);
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
    ///
    ///
    namespace gear 
    {

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