
#include <strava.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    strava::authenticate({
        "1fbc8877efd758b7744774a1217bab6e864e251b",  // access_token
        "http://localhost:3000",                     // redirect_url
        "8a08050aaf532074ab06bdacf3297b3ecc86d640 ", // client_secret
        "18035"                                      // client_id
    });
    
    strava::detailed::athlete me;
    strava::athlete::current(me);

    auto more_friends = strava::athlete::list_athlete_friends(me);
    auto friends = strava::athlete::list_athlete_friends();

    std::cin.get();
}