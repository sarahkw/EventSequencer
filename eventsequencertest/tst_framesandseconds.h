#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <framesandseconds.h>

using namespace testing;

TEST(FramesAndSeconds, BasicFormat)
{
    const int fps = 5;
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, 0), "0+0");
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, 1), "0+1");
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, 5), "1+0");
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, 6), "1+1");
}
