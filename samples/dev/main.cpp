
#include <strava.hpp>
#include <iostream>

int main(int argc, char*argv[])
{
    strava::authenticate({
        "<access_token>",
        "<redirect_url>",
        "<client_secret>",
        "<client_id>"
    });
    
    strava::athlete me;
    strava::athletes::current(me);

    std::cout << me.name << std::endl;
    std::cin.get();
}