
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

    auto more_friends = strava::athlete::list_athlete_friends(auth_info, me);
    auto friends = strava::athlete::list_athlete_friends(auth_info);
    auto more_followers = strava::athlete::list_athlete_followers(auth_info, me);
    auto followers = strava::athlete::list_athlete_followers(auth_info);

    auto both_following = strava::athlete::list_both_following(auth_info, me);
    auto updated_me = strava::athlete::update(auth_info, me, {{ "weight", "50.0" }});

    std::cout << me.firstname << ", " << me.lastname << std::endl;
    std::cout << "Friends = " << friends.size() << std::endl;
    std::cin.ignore();
    std::cin.get();

    /*
  
    strava::detailed::athlete updated_me;
    strava::athlete::update(me, updated_me);

    std::cin.get();
*/

}