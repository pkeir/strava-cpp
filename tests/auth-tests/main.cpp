
#include <strava.hpp>
#include <string>
#include <gtest/gtest.h>

TEST(AuthTest, FunctionPublic)
{
    auto url = strava::request_access(0, strava::scope_public);

    EXPECT_TRUE(url.find("https://") != std::string::npos);
    EXPECT_TRUE(url.find("public") != std::string::npos);
    EXPECT_TRUE(url.empty() == false);
}

TEST(AuthTest, FunctionWrite)
{
    auto url = strava::request_access(0, strava::scope_write);

    EXPECT_TRUE(url.find("https://") != std::string::npos);
    EXPECT_TRUE(url.find("write") != std::string::npos);
    EXPECT_TRUE(url.empty() == false);
}

TEST(AuthTest, Layout)
{
    auto auth = strava::oauth { 0, "client_secret", "access_token" };

    EXPECT_TRUE(auth.client_id == 0);
    EXPECT_TRUE(auth.client_secret == "client_secret");
    EXPECT_TRUE(auth.access_token == "access_token");
}

int main(int argc, char * argv[])
{
    return RUN_ALL_TESTS();
}
