
#include <strava.hpp>
#include <iostream>

int main(int argc, char*argv[])
{
    // starts session and authenticates
    strava::authenticate({
        "",
        "",
        "",
        ""
    });
    
    strava::athelete me;
    strava::atheletes::current(me);

    std::cout << me.name << std::endl;
    std::cin.get();
}