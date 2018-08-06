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

TEST_F(TestVisualPositionManager, Simple)
{
    vpm_.del(0);
}
