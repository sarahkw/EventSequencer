#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <strip.h>

using namespace testing;

TEST(Strip, SimpleTest)
{
    Strip s;
    s.setChannel(10);
    EXPECT_EQ(s.channel(), 10);
}
