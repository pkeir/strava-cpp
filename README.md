
<img src='icon.png' width='150' height='150' align='right' />

# Strava Cpp

C++ API bindings to v3 of the Strava API.

## Example 

```cpp
#include <strava.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    strava::athelete me;
    strava::setupSession();
    strava::authenticate("your_token");
    strava::athletes::current(me);

    std::cout << me.name << std::endl;
}
```

## Dependencies

You can build via CMake or use the prebuilt binaries available in each release. You will also need OpenSSL for compiling Poco HTTPS code. You can install this on MacOS via brew, it should come preinstalled on Linux and you can download a Window installer [here]().

* [Poco](https://github.com/pocoproject/poco)
* [Lest](https://github.com/martinmoene/lest)  
* OpenSSL

## Documentation

* [Project Specification](SPECIFICATION.md)
* [Strava Documentation](http://strava.github.io/api/)

## License

TBD


