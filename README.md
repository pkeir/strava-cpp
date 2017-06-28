
<img src='icon.png' width='150' height='150' align='right' />

# Strava Cpp

C++ API bindings to V3 of the Strava API. This API supports reading and updating of the Strava Dataset however file uploads and webhook events are not yet supported. The following operations are supported.

## Example 

```cpp
#include <strava.hpp>
#include <iostream>

using namespace strava;

int main(int argc, char* argv[])
{
    auto id = 0;        // <client_id>
    auto secret = "";   // <client_secret>
    auto web_url = request_access(id, scope_view_private_write);

    // Open url to authenticate and get code
    std::string code;
    std::cout << web_url << std::endl;
    std::cin >> code;
    
    // Acquire access token to access data
    auto access_token = exchange_token(id, secret, code);
    auto auth_info = oauth{ id, secret, access_token };
    auto myself = athlete::current(auth_info);

    std::cout << myself.firstname << std::endl;
    std::cout << myself.lastname << std::endl;
    std::cout << myself.country << std::endl;
}
```

## Features

The API provides access to the most prominant areas of the dataset. If you are just wanting info from the API or want to update some values, this API will meet your requirements.

* Authentication
* Athletes
* Activities
* Clubs
* Routes
* Running Races
* Segments
* Segment Efforts
* Streams

## Not Supported

The following sections of the API are reserved for a later date.

* Uploads (Revolves around bespoke file types)
* Webhook Events (Need Strava inc permissions)

## Dependencies

You can build via CMake or use the prebuilt binaries available in each release. The library relies on Poco for HTTPS support, Lest for unit testing and OpenSSL as it is required when building Poco with HTTPS support.

* [Poco](https://github.com/pocoproject/poco)
* [Lest](https://github.com/martinmoene/lest)  
* OpenSSL

HTTPS is a hard requirement for requests to Strava so the OpenSSL dependency is not optional. You can install it pretty easily though on MacOS and Linux. On Windows you can install via these [installers](http://slproweb.com/products/Win32OpenSSL.html).

**Linux & MacOS**
```
sudo apt-get install libssl-dev
brew install openssl
```

## License

TBD


