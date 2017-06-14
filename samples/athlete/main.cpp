
#include <strava.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    strava::authenticate({
        "",                     // access_token
        "",                     // redirect_url
        "",                     // client_secret
        ""                      // client_id
    });
}