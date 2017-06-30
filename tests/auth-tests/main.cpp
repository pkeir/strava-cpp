
#include <strava.hpp>
#include <lest.hpp>
#include <string>

using namespace std;

const lest::test specification[] =
{
    CASE("request_access() public test url")
    {
        auto url = strava::request_access(0, strava::scope_public);
        
        EXPECT(url.find("https://") != std::string::npos);
        EXPECT(url.find("public") != std::string::npos);
        EXPECT(url.empty() == false);
    },

    CASE("request_access() write test url")
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
    return lest::run(specification, argc, argv);
}