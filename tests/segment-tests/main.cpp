
#include <strava.hpp>
#include <iostream>
#include <gtest/gtest.h>

strava::oauth auth =
{
    18035,
    "8a08050aaf532074ab06bdacf3297b3ecc86d640",
    "005ed679943cd3eee63861f595863cda58591b41"
};

strava::segments::bounds area =
{
    37.821362, -122.505373,
    37.842038, -122.465977
};

TEST(SegmentsTest, Length)
{
    auto segments = strava::segments::explore(auth, area);

    EXPECT_TRUE(segments.size() > 0);
}

TEST(SegmentsTest, MetaData)
{
    auto segments = strava::segments::explore(auth, area);
    auto segment = segments.front();

    EXPECT_TRUE(segment.id != int{});
    EXPECT_TRUE(segment.resource_state != int{});
}

TEST(SegmentsTest, Name)
{
    auto segments = strava::segments::explore(auth, area);
    auto segment = segments.front();

    EXPECT_TRUE(!segment.name.empty());
}

TEST(SegmentsTest, LatLong)
{
    auto segments = strava::segments::explore(auth, area);
    auto segment = segments.front();

    EXPECT_TRUE(segment.start_latlng[0] != float{});
    EXPECT_TRUE(segment.start_latlng[1] != float{});

    EXPECT_TRUE(segment.end_latlng[0] != float{});
    EXPECT_TRUE(segment.end_latlng[1] != float{});
}

TEST(SegmentsTest, Map)
{
    auto segments = strava::segments::explore(auth, area);
    auto segment = strava::segments::retrieve(auth, segments.front().id);

    EXPECT_TRUE(segment.map.resource_state != int{});
    EXPECT_TRUE(!segment.map.id.empty());
    EXPECT_TRUE(!segment.map.polyline.empty());
}

TEST(SegmentsTest, Distance)
{
    auto segments = strava::segments::explore(auth, area);
    auto segment = segments.front();

    EXPECT_TRUE(segment.distance != float{});
}

int main(int argc, char * argv[])
{
    return RUN_ALL_TESTS();
}
