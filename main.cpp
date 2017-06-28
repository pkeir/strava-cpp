
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

    auto myself = athlete::current(auth_info);
    auto my_activities = activity::list(auth_info);
    auto my_activity = activity::retrieve(auth_info, my_activities.front().id);

    auto stream = stream::integer_stream(auth_info, 123, stream::source::activity, strava::stream::integer_types::time);
    
    std::cin.get();
}