#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <document.h>
#include <strip.h>

using namespace testing;

TEST(Strip, SimpleTest)
{
    Document d;
    Strip s(d);
    s.setChannel(10);
    EXPECT_EQ(s.channel(), 10);
}
