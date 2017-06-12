
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

You can build via CMake or use the prebuilt binaries available in each release. You will also need OpenSSL for compiling Poco HTTPS code. You can install this on MacOS via brew, it should come preinstalled on Linux and you can download a Window installer [here]().

* [Poco](https://github.com/pocoproject/poco)
* [Lest](https://github.com/martinmoene/lest)  
* OpenSSL

## Objectives

* CI with travis + appveyor
* Badges in README
* Athlete functionality
* Athlete tests

TBD by Monday the 19th of June.

## Documentation

* [Project Specification](SPECIFICATION.md)
* [Strava Documentation](http://strava.github.io/api/)

## License

TBD


