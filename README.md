
<img src='icon.png' width='150' height='150' align='right' />

# Strava Cpp

C++ API bindings to v3 of the Strava API. Currently a work in progress.

## Example 

```cpp
#include <strava.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    strava::authenticate({
        "<access_token>",
        "<redirect_url>",
        "<client_secret>",
        "<client_id>"
    });
    
    strava::athlete me;
    strava::athlete::current(me);

    std::cout << me.name << std::endl;
}
```

## Dependencies

You can build via CMake or use the prebuilt binaries available in each release. The library relies on Poco for HTTPS support, Lest for unit testing and OpenSSL because it is required when building Poco with HTTPS support.

* [Poco](https://github.com/pocoproject/poco)
* [Lest](https://github.com/martinmoene/lest)  
* OpenSSL

We have to use HTTPS for requests to Strava so the OpenSSL dependency is not optional. You can install it pretty easily though on MacOS and Linux. On Windows you can install via these [installers](http://slproweb.com/products/Win32OpenSSL.html).

**Linux**
```
sudo apt-get install libssl-dev
```

**MacOS**
```
brew install openssl
```

## Objectives

TBD by Monday the 19th of June.

* Athlete functionality
* Athlete tests

## Documentation

* [Project Specification](SPECIFICATION.md)
* [Strava Documentation](http://strava.github.io/api/)

## License

TBD


