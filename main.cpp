
#include <strava.hpp>
#include <iostream>

using scope = strava::oauth_scope;

int main(int argc, char* argv[])
{
    auto code = std::string(argc > 3 ? argv[3] : "");
    auto client_secret = std::string(argc > 2 ? argv[2] : "");  
    auto client_id = argc > 1 ? atoi(argv[1]) : 0;

    if (client_id == 0 || client_secret == "")
    {
        std::cerr << "No parameters provided, closing" << std::endl;
        return 1;
    }

    if (code.empty())
    {
        std::cout << strava::request_access(client_id, scope::scope_view_private_write) << std::endl;
        std::cin >> code;
        std::cin.ignore();
    }

    auto access_token = strava::exchange_token(client_id, client_secret, code);
    auto auth_info = strava::oauth { client_id, client_secret, access_token };
    auto me = strava::athlete::current(auth_info);
    
    auto races = strava::races::list(auth_info, 0);
    auto race = strava::races::retrieve(auth_info, races.front().id);
    std::cin.get();
}