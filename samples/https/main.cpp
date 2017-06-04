
#include <cpr/cpr.h>
#include <iostream>

int main(int argc, const char* argv[])
{
    auto response = cpr::Get(cpr::Url{ "http://www.williamsamtaylor.co.uk:3001/" });
    std::cout << response.text << std::endl;
    std::cin.get();
}