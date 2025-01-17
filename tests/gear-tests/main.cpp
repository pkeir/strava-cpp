
#include <strava.hpp>
#include <iostream>
#include <lest.hpp>

strava::oauth auth = {
    18035,
    "8a08050aaf532074ab06bdacf3297b3ecc86d640",
    "005ed679943cd3eee63861f595863cda58591b41"
};

const lest::test specification[] =
{
    CASE("gear length test")
    {
        auto me = strava::athlete::current(auth);
        EXPECT(me.shoes.size() > 0);
    },

    CASE("gear name test")
    {
        auto me = strava::athlete::current(auth);
        auto shoes = me.shoes.front();

        EXPECT(shoes.id.size() > 0);
        EXPECT(shoes.name.size() > 0);
    },

    CASE("gear struct test")
    {
        auto me = strava::athlete::current(auth);
        auto shoes = strava::gear::retrieve(auth, me.shoes.front().id);

        EXPECT(shoes.description.size() > 0);
        EXPECT(shoes.brand_name.size() > 0);
        EXPECT(shoes.model_name.size() > 0);
    }
};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}
