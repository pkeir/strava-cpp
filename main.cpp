
#include <strava.hpp>
#include <iostream>

using scope = strava::oauth_scope;

int main(int argc, char* argv[])
{
    strava::setup_client();

    auto url = strava::token_url(18035, scope::scope_public);

    strava::exchange_token("71ced2a8e18ad25f6546605809f78382d12ad5ab", 18035, "8a08050aaf532074ab06bdacf3297b3ecc86d640");
    /*
    strava::authenticate({
        "44e10826ce68c67f1f155fb4989b6b92cbbf5ae4",  // access_token
        "http://localhost:3000",                     // redirect_url
        "8a08050aaf532074ab06bdacf3297b3ecc86d640 ", // client_secret
        "18035"                                      // client_id
    });
    
    strava::detailed::athlete me;
    strava::athlete::current(me);

    auto more_friends = strava::athlete::list_athlete_friends(me);
    auto friends = strava::athlete::list_athlete_friends();

    auto more_followers = strava::athlete::list_athlete_followers(me);
    auto followers = strava::athlete::list_athlete_followers();

    auto both_following = strava::athlete::list_both_following(me);

    strava::detailed::athlete updated_me;
    strava::athlete::update(me, updated_me);
*/
    std::cin.get();

}