#include <gtest/gtest.h>

#include "channelindex.h"

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
