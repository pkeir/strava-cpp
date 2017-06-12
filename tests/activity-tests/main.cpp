
#include <lest.hpp>

using namespace std;

const lest::test specification[] =
{
    CASE("Basic Zero Test")
    {
        EXPECT(0 == 0);
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}