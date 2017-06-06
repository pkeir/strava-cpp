
#include <strava.hpp>
#include <iostream>

int main(int argc, char*argv[])
{
    strava::athelete me;
    strava::setupSession();
    strava::authenticate(""); //
    strava::athletes::current(me);

    std::cout << me.name << std::endl;
    std::cin.get();
}