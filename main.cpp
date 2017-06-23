
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
    }

    auto access_token = strava::exchange_token(client_id, client_secret, code);
    auto auth_info = strava::oauth { client_id, client_secret, access_token };
    auto me = strava::athlete::current(auth_info);
    auto next_to_me = strava::athlete::retrieve(auth_info, me.id - 10);

    auto more_friends = strava::athlete::list_athlete_friends(auth_info, next_to_me);
    auto more_followers = strava::athlete::list_athlete_followers(auth_info, next_to_me);

    //auto both_following = strava::athlete::list_both_following(auth_info, me);
    //auto updated_me = strava::athlete::update(auth_info, me, { { "weight", "50.0" } });
    //auto my_zones = strava::athlete::get_zones(auth_info);

    auto my_stats = strava::athlete::get_stats(auth_info, me.id);
    auto my_koms = strava::athlete::get_koms(auth_info, me.id);

    std::cout << next_to_me.firstname << ", " << next_to_me.lastname << std::endl;
    std::cout << "Friends = " << more_friends.size() << std::endl;
    std::cin.ignore();
    std::cin.get();
}