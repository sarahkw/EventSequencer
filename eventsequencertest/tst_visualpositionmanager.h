#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "visualpositionmanager.h"

struct TestVisualPositionManager : public testing::Test {
    MOCK_METHOD2(visualPositionChangedAfter, void(int, int));
    MOCK_METHOD2(visualPositionChangedBefore, void(int, int));
    VisualPositionManager vpm_;
    TestVisualPositionManager()
    {
        QObject::connect(
            &vpm_, &VisualPositionManager::visualPositionChangedAfter,
            [this](int a, int b) { this->visualPositionChangedAfter(a, b); });
        QObject::connect(
            &vpm_, &VisualPositionManager::visualPositionChangedBefore,
            [this](int a, int b) { this->visualPositionChangedBefore(a, b); });
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

TEST_F(TestVisualPositionManager, SignalsOnDelete)
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

TEST_F(TestVisualPositionManager, SignalsOnUpdate)
{
    using testing::_;

    EXPECT_CALL(*this, visualPositionChangedAfter(5, 3));
    vpm_.setSpan(5, 3);
    EXPECT_CALL(*this, visualPositionChangedAfter(5, -1));
    vpm_.setSpan(5, 2);

    EXPECT_CALL(*this, visualPositionChangedBefore(-5, 3));
    vpm_.setSpan(-5, 3);
    EXPECT_CALL(*this, visualPositionChangedBefore(-5, -2));
    vpm_.setSpan(5, 1);
}

//TEST_F(TestVisualPositionManager, Simple)
//{
//    EXPECT_EQ(vpm_.chanIdxToVisualPosition(0), 0);
//    EXPECT_EQ(vpm_.chanIdxToVisualPosition(10), 10);

//    vpm_.setSpan(5, 5);
//    EXPECT_CALL(*this, visualPositionChangedAfter(5, 5));

//    EXPECT_EQ(vpm_.chanIdxToVisualPosition(5), 5);

//}
