#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "printto_channelindex.h"

#include "visualpositionmanager.h"

struct TestVisualPositionManager : public testing::Test {
    MOCK_METHOD2(visualPositionChangedAfter, void(int, int));
    MOCK_METHOD2(visualPositionChangedBefore, void(int, int));
    MOCK_METHOD2(destroyChanIdx, void(ChannelIndex, ChannelIndex));
    VisualPositionManager vpm_;
    TestVisualPositionManager()
    {
        QObject::connect(
            &vpm_, &VisualPositionManager::visualPositionChangedAfter,
            [this](int a, int b) { this->visualPositionChangedAfter(a, b); });
        QObject::connect(
            &vpm_, &VisualPositionManager::visualPositionChangedBefore,
            [this](int a, int b) { this->visualPositionChangedBefore(a, b); });
        QObject::connect(&vpm_, &VisualPositionManager::destroyChanIdx,
                         [this](ChannelIndex from, ChannelIndex to) {
                             this->destroyChanIdx(from, to);
                         });
    }
};

TEST_F(TestVisualPositionManager, IdxIsValid)
{
    EXPECT_TRUE(vpm_.chanIdxIsValid(ChannelIndex::make1(5)));
    EXPECT_FALSE(vpm_.chanIdxIsValid(ChannelIndex::make2(5, 2)));
    EXPECT_FALSE(vpm_.chanIdxIsValid(ChannelIndex::make2(5, 3)));

    vpm_.setSpan(5, 3);

    EXPECT_TRUE(vpm_.chanIdxIsValid(ChannelIndex::make1(5)));
    EXPECT_TRUE(vpm_.chanIdxIsValid(ChannelIndex::make2(5, 2)));
    EXPECT_FALSE(vpm_.chanIdxIsValid(ChannelIndex::make2(5, 3)));
}

TEST_F(TestVisualPositionManager, ChangedBeforeAndAfterSignals)
{
    using testing::_;

    EXPECT_CALL(*this, visualPositionChangedAfter(5, 3));
    vpm_.setSpan(5, 3);

    EXPECT_CALL(*this, visualPositionChangedAfter(0, 2));
    vpm_.setSpan(0, 2);

    EXPECT_CALL(*this, visualPositionChangedBefore(-1, 4));
    vpm_.setSpan(-1, 4);

    EXPECT_CALL(*this, visualPositionChangedBefore(6, 0)).Times(0);
    vpm_.setSpan(6, 0);
}

TEST_F(TestVisualPositionManager, ChangedSignalsOnDelete)
{
    using testing::_;

    EXPECT_CALL(*this, visualPositionChangedAfter(5, 3));
    vpm_.setSpan(5, 3);
    EXPECT_CALL(*this, visualPositionChangedAfter(5, -3));
    vpm_.del(5);

    EXPECT_CALL(*this, visualPositionChangedBefore(-5, 3));
    vpm_.setSpan(-5, 3);
    EXPECT_CALL(*this, visualPositionChangedBefore(-5, -3));
    vpm_.del(-5);
}

TEST_F(TestVisualPositionManager, ChangedSignalsOnUpdate)
{
    using testing::_;

    EXPECT_CALL(*this, visualPositionChangedAfter(5, 3));
    vpm_.setSpan(5, 3);
    EXPECT_CALL(*this, visualPositionChangedAfter(5, -1));
    vpm_.setSpan(5, 2);

    EXPECT_CALL(*this, visualPositionChangedBefore(-5, 3));
    vpm_.setSpan(-5, 3);
    EXPECT_CALL(*this, visualPositionChangedBefore(-5, -2));
    vpm_.setSpan(-5, 1);
}

TEST_F(TestVisualPositionManager, DestroySignals)
{
    using testing::_;
    using testing::InSequence;

    {
        testing::InSequence dummy;

        EXPECT_CALL(*this, visualPositionChangedAfter(5, 3));
        EXPECT_CALL(*this, destroyChanIdx(ChannelIndex::make2(5, 2), ChannelIndex::make2(5, 3)));
        EXPECT_CALL(*this, visualPositionChangedAfter(5, -1));
        EXPECT_CALL(*this, destroyChanIdx(ChannelIndex::make2(5, 0), ChannelIndex::make2(5, 2)));
        EXPECT_CALL(*this, visualPositionChangedAfter(5, -2));
    }

    vpm_.setSpan(5, 3);
    vpm_.setSpan(5, 2);
    vpm_.setSpan(5, 0);
}

TEST_F(TestVisualPositionManager, DestroySignals_Negative)
{
    using testing::_;
    using testing::InSequence;

    {
        testing::InSequence dummy;

        EXPECT_CALL(*this, visualPositionChangedBefore(-5, 3));
        EXPECT_CALL(*this, destroyChanIdx(ChannelIndex::make2(-5, 2), ChannelIndex::make2(-5, 1)));
        EXPECT_CALL(*this, visualPositionChangedBefore(-5, -1));
        EXPECT_CALL(*this, destroyChanIdx(ChannelIndex::make2(-5, 1), ChannelIndex::make1(-5)));
        EXPECT_CALL(*this, visualPositionChangedBefore(-5, -2));
    }

    vpm_.setSpan(-5, 3);
    vpm_.setSpan(-5, 2);
    vpm_.setSpan(-5, 0);
}

TEST_F(TestVisualPositionManager, IdxToVisPos)
{
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(9)), 9);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(10)), 10);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(11)), 11);
    vpm_.setSpan(10, 5);
    vpm_.setSpan(-10, 99); // Decoy (to test iterators)
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(9)), 9);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(10)), 10);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make2(10, 0)), 11);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make2(10, 4)), 15);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(11)), 16);

    // Another round
    vpm_.setSpan(12, 1);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(12)), 17);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make2(12, 0)), 18);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(13)), 19);
}

TEST_F(TestVisualPositionManager, IdxToVisPos_Negative)
{
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(-9)), -9);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(-10)), -10);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(-11)), -11);
    vpm_.setSpan(10, 99); // Decoy (to test iterators)
    vpm_.setSpan(-10, 5);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(-9)), -9);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(-10)), -10);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make2(-10, 0)), -11);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make2(-10, 4)), -15);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(-11)), -16);

    // Another round
    vpm_.setSpan(-12, 1);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(-12)), -17);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make2(-12, 0)), -18);
    EXPECT_EQ(vpm_.chanIdxToVisualPosition(ChannelIndex::make1(-13)), -19);
}

TEST_F(TestVisualPositionManager, VisPosToIdx)
{
    EXPECT_EQ(vpm_.visualPositionToChanIdx(9), ChannelIndex::make1(9));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(10), ChannelIndex::make1(10));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(11), ChannelIndex::make1(11));
    vpm_.setSpan(10, 5);
    vpm_.setSpan(-10, 99); // Decoy (to test iterators)
    EXPECT_EQ(vpm_.visualPositionToChanIdx(9), ChannelIndex::make1(9));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(10), ChannelIndex::make1(10));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(11), ChannelIndex::make2(10, 0));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(15), ChannelIndex::make2(10, 4));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(16), ChannelIndex::make1(11));

    // Another round
    vpm_.setSpan(12, 1);
    EXPECT_EQ(vpm_.visualPositionToChanIdx(17), ChannelIndex::make1(12));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(18), ChannelIndex::make2(12, 0));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(19), ChannelIndex::make1(13));
}

TEST_F(TestVisualPositionManager, VisPosToIdx_Negative)
{
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-9), ChannelIndex::make1(-9));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-10), ChannelIndex::make1(-10));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-11), ChannelIndex::make1(-11));
    vpm_.setSpan(10, 99); // Decoy (to test iterators)
    vpm_.setSpan(-10, 5);
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-9), ChannelIndex::make1(-9));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-10), ChannelIndex::make1(-10));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-11), ChannelIndex::make2(-10, 0));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-12), ChannelIndex::make2(-10, 1));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-13), ChannelIndex::make2(-10, 2));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-14), ChannelIndex::make2(-10, 3));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-15), ChannelIndex::make2(-10, 4));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-16), ChannelIndex::make1(-11));

    // Another round
    vpm_.setSpan(-12, 1);
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-17), ChannelIndex::make1(-12));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-18), ChannelIndex::make2(-12, 0));
    EXPECT_EQ(vpm_.visualPositionToChanIdx(-19), ChannelIndex::make1(-13));

}
