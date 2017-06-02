
#include <iostream>
#include <exception>
#include <strava.hpp>

class not_implemented : public std::logic_error
{
public:
    not_implemented() : std::logic_error("Function not yet implemented") { };
};

void strava::athletes::current()
{
    throw not_implemented();
}