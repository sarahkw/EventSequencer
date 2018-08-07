#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <set>

#include "channelindex.h"

inline void PrintTo(const ChannelIndex &val, ::std::ostream *os)
{
    *os << val.toDebugString().toStdString();
}


TEST(ChannelIndex, Order)
{
    std::vector<ChannelIndex> expectedOrder{
        ChannelIndex::make1(-2),
            ChannelIndex::make2(-1, 1),
            ChannelIndex::make2(-1, 0),
            ChannelIndex::make1(-1),
            ChannelIndex::make1(0),
            ChannelIndex::make2(0, 0),
            ChannelIndex::make2(0, 1),
            ChannelIndex::make1(1),
            ChannelIndex::make2(1, 0),
            ChannelIndex::make2(1, 1),
            ChannelIndex::make1(2)
    };

    EXPECT_THAT(std::set<ChannelIndex>(expectedOrder.begin(), expectedOrder.end()),
                testing::ElementsAreArray(expectedOrder.begin(), expectedOrder.end()));
}

TEST(ChannelIndex, MakeFromPathString)
{
    bool success;
    {
        auto c = ChannelIndex::makeFromPathString("", &success);
        EXPECT_FALSE(success);
    }
    {
        auto c = ChannelIndex::makeFromPathString("hi", &success);
        EXPECT_FALSE(success);
    }
    {
        auto c = ChannelIndex::makeFromPathString("1", &success);
        EXPECT_TRUE(success);
        EXPECT_EQ(c.first(), 1);
        EXPECT_FALSE(c.hasSecond());
    }
    {
        auto c = ChannelIndex::makeFromPathString("-10", &success);
        EXPECT_TRUE(success);
        EXPECT_EQ(c.first(), -10);
        EXPECT_FALSE(c.hasSecond());
    }
    {
        auto c = ChannelIndex::makeFromPathString("1.5", &success);
        EXPECT_TRUE(success);
        EXPECT_EQ(c.first(), 1);
        EXPECT_TRUE(c.hasSecond());
        EXPECT_EQ(c.second(), 5);
    }
    {
        auto c = ChannelIndex::makeFromPathString("1.-5", &success);
        EXPECT_FALSE(success);
    }
    {
        auto c = ChannelIndex::makeFromPathString("1.", &success);
        EXPECT_FALSE(success);
    }
}
