
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

    auto my_routes = routes::list(auth_info, me.id);
    auto only_route = routes::retrieve(auth_info, my_routes.front().id);

    auto first_segment = only_route.segments.front();
    auto checking = segments::retrieve(auth_info, first_segment.id);
    auto updated = segments::star(auth_info, checking.id, true);

    auto starred = segments::list_starred(auth_info);
    auto leaderboard = segments::get_leaderboard(auth_info, checking.id);
    auto area = strava::segments::bounds{ 55.777683, -3.651581, 55.994877, -2.937469 };
    auto found = segments::explore(auth_info, area);
    
    std::cin.get();
}