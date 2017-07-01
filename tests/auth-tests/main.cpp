
#include <strava.hpp>
#include <lest.hpp>
#include <string>

const lest::test specification[] =
{
    CASE("request_access function test (public)")
    {
        auto url = strava::request_access(0, strava::scope_public);
        
        EXPECT(url.find("https://") != std::string::npos);
        EXPECT(url.find("public") != std::string::npos);
        EXPECT(url.empty() == false);
    },

    CASE("request_access function test (write)")
    {
        auto url = strava::request_access(0, strava::scope_write);
        
        EXPECT(url.find("https://") != std::string::npos);
        EXPECT(url.find("write") != std::string::npos);
        EXPECT(url.empty() == false);
    },

    CASE("struct oauth layout test")
    {
        auto auth = strava::oauth { 0, "client_secret", "access_token" };

        EXPECT(auth.client_id == 0);
        EXPECT(auth.client_secret == "client_secret");
        EXPECT(auth.access_token == "access_token");
    }
};

int main(int argc, char * argv[])
{
    /*
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
    auto my_activity = activity::list(auth_info).front();
    auto comments = activity::list_comments(auth_info, my_activity.id);
    
    std::cin.get();
    */
    
    return lest::run(specification, argc, argv);
}