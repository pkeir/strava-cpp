
#include <strava.hpp>
#include <iostream>

using namespace strava;

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
        std::cout << request_access(client_id, scope_view_private_write) << std::endl;
        std::cin >> code;
        std::cin.ignore();
    }

    auto access_token = exchange_token(client_id, client_secret, code);
    auto auth_info = oauth{ client_id, client_secret, access_token };

    auto me = athlete::current(auth_info);
    auto next = athlete::retrieve(auth_info, me.id + 1);


    auto stream = stream::retrieve_activity<stream::types::latlng>();

    auto friends = athlete::list_athlete_friends(auth_info, next, { 1, 1 });
    auto races = races::list(auth_info, 0);

    std::cin.get();
}