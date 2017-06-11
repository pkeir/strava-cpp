
#include <strava.hpp>
#include <iostream>

int main(int argc, char*argv[])
{
    strava::authenticate({
        "1fbc8877efd758b7744774a1217bab6e864e251b",
        "<redirect_url>",
        "<client_secret>",
        "<client_id>"
    });
    
    strava::athlete me;
    strava::athletes::current(me);

    std::cout << me.firstname << " " << me.lastname << std::endl;
    std::cin.get();
}