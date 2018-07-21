#include "mapgenerate.h"

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

TEST(MapGenerate, Sample)
{
    std::vector<int> inputs = {10, 20, 30};
    struct GenerateLogic {
        struct NoIteratorFlag {};
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

    {
        auto it = mg.begin();

        EXPECT_EQ(it++.beforeMap(), 10);
        EXPECT_EQ(it++.beforeMap(), 10);
        EXPECT_EQ(it++.beforeMap(), 20);
        EXPECT_EQ(it++.beforeMap(), 20);
        EXPECT_EQ(it++.beforeMap(), 30);
        EXPECT_EQ(it++.beforeMap(), 30);
    }
}


TEST(MapGenerate, NoGenerate)
{
    std::vector<int> inputs = {10, 20, 30};
    struct Func {
        using SrcType = int;
        using DstType = std::string;
        DstType operator()(const SrcType& src)
        {
            return std::to_string(src) + " cats";
        }
    };

    auto mg = makeMapGenerateNoGenerate<Func>(inputs.begin(), inputs.end());

    EXPECT_THAT(mg, testing::ElementsAre("10 cats", "20 cats", "30 cats"));
}

TEST(MapGenerate, MapUsingIterator)
{
    std::vector<int> inputs = {10, 20, 30};
    struct GenerateLogic {
        struct UseIteratorFlag {};
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
        int mapGenerateIterator(std::vector<int>::const_iterator iter)
        {
            return *iter + state;
        }
        bool operator==(const GenerateLogic& gl) const
        {
            return state == gl.state;
        }
    };

    auto mg = makeMapGenerate<GenerateLogic>(inputs.begin(), inputs.end());

    EXPECT_THAT(mg, testing::ElementsAre(10, 11, 20, 21, 30, 31));
}
