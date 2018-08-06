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

//TEST_F(TestVisualPositionManager, Simple)
//{
//    EXPECT_EQ(vpm_.chanIdxToVisualPosition(0), 0);
//    EXPECT_EQ(vpm_.chanIdxToVisualPosition(10), 10);

//    vpm_.setSpan(5, 5);
//    EXPECT_CALL(*this, visualPositionChangedAfter(5, 5));

//    EXPECT_EQ(vpm_.chanIdxToVisualPosition(5), 5);

//}
