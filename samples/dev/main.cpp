
#include <strava.hpp>
#include <iostream>

int main(int argc, char*argv[])
{
    // starts session and authenticates
    strava::authenticate({
        "1fbc8877efd758b7744774a1217bab6e864e251b ",
        "",
        "",
        ""
    });
    
    strava::athelete me;
    strava::atheletes::current(me);

    std::cout << me.name << std::endl;
    std::cin.get();
}