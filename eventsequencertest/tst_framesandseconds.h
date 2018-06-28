#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <framesandseconds.h>

using namespace testing;

////////////////////////////////////////////////////////////////////////////////

// https://stackoverflow.com/a/42597727
QT_BEGIN_NAMESPACE
inline void PrintTo(const QString &qString, ::std::ostream *os)
{
    *os << qUtf8Printable(qString);
}
inline void PrintTo(const QVariant &qVar, ::std::ostream *os)
{
    // XXX I wrote this without much research/thought.
    *os << "(QVariant " << qVar.typeName() << " "
        << qUtf8Printable(qVar.toString()) << ")";
}
QT_END_NAMESPACE

////////////////////////////////////////////////////////////////////////////////

TEST(FramesAndSeconds, ToSeconds_Seconds)
{
    const int fps = 5;
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, 0), "0+0");
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, 1), "0+1");
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, 5), "1+0");
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, 6), "1+1");
}

TEST(FramesAndSeconds, ToSeconds_Minutes)
{
    const int fps = 5;
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, fps * 60 * 1), "1:00+0");
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, fps * 60 * 1 + fps * 5), "1:05+0");
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps, fps * 60 * 1 + fps * 5 + 1), "1:05+1");
}

TEST(FramesAndSeconds, ToSeconds_Hours)
{
    const int fps = 5;
    EXPECT_EQ(FramesAndSeconds::framesToSeconds(fps,
                                                fps * 60 * 60 * 1 +
                                                fps * 60 * 1 +
                                                fps * 5 +
                                                1), "1:01:05+1");
}

////////////////////////////////////////////////////////////////////////////////

TEST(FramesAndSeconds, ToFrames_FrameNumberOnly)
{
    const int fps = 5;
    EXPECT_THAT(FramesAndSeconds::secondsToFrames(fps, "10"),
                ElementsAre(true, 10));
    EXPECT_THAT(FramesAndSeconds::secondsToFrames(fps, "hello"),
                ElementsAre(false));
}

TEST(FramesAndSeconds, ToFrames_Seconds)
{
    const int fps = 5;
    EXPECT_THAT(FramesAndSeconds::secondsToFrames(fps, "0+0"),
                ElementsAre(true, 0));
    EXPECT_THAT(FramesAndSeconds::secondsToFrames(fps, "0+1"),
                ElementsAre(true, 1));
    EXPECT_THAT(FramesAndSeconds::secondsToFrames(fps, "1+0"),
                ElementsAre(true, 5));
    EXPECT_THAT(FramesAndSeconds::secondsToFrames(fps, "6+1"),
                ElementsAre(true, 31));

    EXPECT_THAT(FramesAndSeconds::secondsToFrames(fps, "6+"),
                ElementsAre(true, 30));
}

//TEST(FramesAndSeconds, ToFrames_Minutes)
//{
//    const int fps = 1;
//    EXPECT_THAT(FramesAndSeconds::secondsToFrames(fps, "1:00+0"),
//                ElementsAre(true, 60));
//}
