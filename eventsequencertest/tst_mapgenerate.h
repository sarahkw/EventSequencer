#include "mapgenerate.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

TEST(MapGenerate, Sample)
{
    std::vector<int> inputs = {10, 20, 30};
    struct GenerateLogic {
        using value_type = int;
        int state = 0;
        bool nextState()
        {
            ++state;
            if (state == 2) {
                state = 0;
                return false;
            } else {
                return true;
            }
        }
        int mapGenerate(int src)
        {
            return src + state;
        }
        bool operator==(const GenerateLogic& gl) const
        {
            return state == gl.state;
        }
    };

    auto mg = makeMapGenerate<GenerateLogic>(inputs.begin(), inputs.end());

    EXPECT_THAT(mg, testing::ElementsAre(10, 11, 20, 21, 30, 31));

    for (int i : mg) {
        static_cast<void>(i);
        // This should compile
    }
}
