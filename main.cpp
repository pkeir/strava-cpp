
#include <strava.hpp>
#include <iostream>

using scope = strava::oauth_scope;

int main(int argc, char* argv[])
{
    const auto client_secret = argc > 2 ? argv[2] : "";  // "8a08050aaf532074ab06bdacf3297b3ecc86d640" 
    const auto client_id = argc > 1 ? atoi(argv[1]) : 0; // 18035

    if (client_id == 0 || client_secret == "")
    {
        std::cerr << "No parameters provided, closing" << std::endl;
        return 1;
    }

    auto token = strava::request_access(client_id, scope::scope_public);
    auto access_token = strava::exchange_token(client_id, client_secret, token);
    auto auth_info = strava::oauth{ client_id, client_secret, access_token };

    //strava::detailed::athlete me;
    //strava::athlete::current(me);

    /*
    auto more_friends = strava::athlete::list_athlete_friends(me);
    auto friends = strava::athlete::list_athlete_friends();

    auto more_followers = strava::athlete::list_athlete_followers(me);
    auto followers = strava::athlete::list_athlete_followers();

    auto both_following = strava::athlete::list_both_following(me);

    strava::detailed::athlete updated_me;
    strava::athlete::update(me, updated_me);

    std::cin.get();
*/

}