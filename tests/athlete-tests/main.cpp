
#include <strava.hpp>
#include <iostream>
#include <lest.hpp>

using namespace std;

const lest::test specification[] =
{
    CASE("Athlete name test")
    {
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}