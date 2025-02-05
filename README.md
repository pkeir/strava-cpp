<img src='icon.png' width='150' height='150' align='right' />

# Strava Cpp

C++ API bindings to V3 of the Strava API. This API supports reading and updating of the Strava Dataset however file uploads and webhook events are not yet supported.

## Example

```cpp
#include <strava.hpp>
#include <iostream>
#include <cstdlib>

// g++ -std=c++17 -I include example.cpp -L build -lSTRAVA -lPocoFoundation -lPocoNet -lPocoNetSSL -lPocoJSON

int main(int argc, const char* argv[])
{
    auto id = std::atoi(std::getenv("STRAVA_CLIENT_ID"));
    auto secret = std::getenv("STRAVA_CLIENT_SECRET");
    auto scope = strava::oauth_scope::scope_view_private_write;
    auto web_url = strava::request_access(id, scope);

    // Open url to authenticate and get code
    std::string code;
    std::cout << web_url << std::endl;
    std::cin >> code;

    // Acquire access token to access data
    auto access_token = strava::exchange_token(id, secret, code);
    auto auth_info = strava::oauth{ id, secret, access_token };
    auto myself = strava::athlete::current(auth_info);

    std::cout << myself.firstname << std::endl;
    std::cout << myself.lastname << std::endl;
}
```

## Features

The API provides access to the most prominent areas of the dataset. If you are just wanting info from the API or want to update some values, this API will meet your requirements.

* Authentication
* Athletes
* Activities
* Clubs
* Routes
* Running Races
* Segments
* Efforts

## Not Supported

The following sections of the API are reserved for a later date.

* Streams (Unable to get test set)
* Uploads (Revolves around bespoke file types)
* Webhook Events (Need Strava corporate permissions)

## Dependencies

You can build via CMake or use the prebuilt binaries available in each release. The library relies on Poco for HTTPS support; GTest for unit testing; and OpenSSL as it is required when building Poco with HTTPS support.

* [Poco](https://github.com/pocoproject/poco)
* [GoogleTest](https://github.com/google/googletest)
* [OpenSSL](https://www.openssl.org/)

HTTPS is a hard requirement for requests to Strava so the OpenSSL dependency is not optional. You can install it pretty easily though on MacOS and Linux. On Windows you can install via these [installers](http://slproweb.com/products/Win32OpenSSL.html).

**Linux**
```
sudo apt-get install libssl-dev libpoco-dev libgtest-dev
```

**MacOS**
```
brew install openssl
```

## License

MIT License
