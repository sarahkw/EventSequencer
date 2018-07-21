#include "groupby.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

MATCHER_P(IteratorEquals, val, "")
{
    return *arg == val;
}

struct KeyFunctor {
    int operator()(const std::string& s)
    {
        return s.size();
    }

};

TEST(GroupBy, Simple)
{
    std::vector<std::string> in = {
        "hi",
        "ho",
        "fee",
        "fye",
        "fouu",
        "fumm"
    };

    auto groupby = makeGroupBy<int, KeyFunctor>(in.begin(), in.end());
    auto it = groupby.begin();
    
    EXPECT_EQ(it->key, 2);
    EXPECT_THAT(it->items, testing::ElementsAre(IteratorEquals("hi"),
                                                IteratorEquals("ho")));
    ++it;

    EXPECT_EQ(it->key, 3);
    EXPECT_THAT(it->items, testing::ElementsAre(IteratorEquals("fee"),
                                                IteratorEquals("fye")));
    ++it;

    EXPECT_EQ(it->key, 4);
    EXPECT_THAT(it->items, testing::ElementsAre(IteratorEquals("fouu"),
                                                IteratorEquals("fumm")));
    ++it;

    EXPECT_EQ(it, groupby.end());
}

TEST(GroupBy, Single)
{
    std::vector<std::string> in = {
        "hi"
    };

    auto groupby = makeGroupBy<int, KeyFunctor>(in.begin(), in.end());
    auto it = groupby.begin();
    
    EXPECT_EQ(it->key, 2);
    EXPECT_THAT(it->items, testing::ElementsAre(IteratorEquals("hi")));
    ++it;

    EXPECT_EQ(it, groupby.end());
}


TEST(GroupBy, Blank)
{
    std::vector<std::string> in = {
    };

    auto groupby = makeGroupBy<int, KeyFunctor>(in.begin(), in.end());
    auto it = groupby.begin();

    EXPECT_EQ(it, groupby.end());
}
