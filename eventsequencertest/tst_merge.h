#include "merge.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

void PrintTo(const typename Merge<std::vector<int>::iterator,
                                  std::vector<int>::iterator>::value_type& v,
             ::std::ostream* os)
{
    switch (v.select) {
    case MergeRangeSelect::Range1:
        *os << "(Range1: " << *v.range1 << ")";
        break;
    case MergeRangeSelect::Range2:
        *os << "(Range2: " << *v.range2 << ")";
        break;
    }
}

MATCHER_P(HasValue, val, "")
{
    switch (arg.select) {
    case MergeRangeSelect::Range1:
        return *(arg.range1) == val;
    case MergeRangeSelect::Range2:
        return *(arg.range2) == val;
    }
    return false; // Compiler happiness
}

TEST(Merge, Simple)
{
    std::vector<int> range1 = {2, 4, 6};
    std::vector<int> range2 = {1, 3, 5};

    auto merge = makeMerge(range1.begin(), range1.end(),
                           range2.begin(), range2.end());

    EXPECT_THAT(merge, testing::ElementsAre(HasValue(1),
                                            HasValue(2),
                                            HasValue(3),
                                            HasValue(4),
                                            HasValue(5),
                                            HasValue(6)));
}

TEST(Merge, OnlyLeft)
{
    std::vector<int> range1 = {2, 4, 6};
    std::vector<int> range2 = {};

    auto merge = makeMerge(range1.begin(), range1.end(),
                           range2.begin(), range2.end());

    EXPECT_THAT(merge, testing::ElementsAre(HasValue(2),
                                            HasValue(4),
                                            HasValue(6)));
}

TEST(Merge, OnlyRight)
{
    std::vector<int> range1 = {};
    std::vector<int> range2 = {1, 3, 5};

    auto merge = makeMerge(range1.begin(), range1.end(),
                           range2.begin(), range2.end());

    EXPECT_THAT(merge, testing::ElementsAre(HasValue(1),
                                            HasValue(3),
                                            HasValue(5)));
}

TEST(Merge, Duplicate)
{
    std::vector<int> range1 = {2, 4, 6};
    std::vector<int> range2 = {1, 4, 4};

    auto merge = makeMerge(range1.begin(), range1.end(),
                           range2.begin(), range2.end());

    EXPECT_THAT(merge, testing::ElementsAre(HasValue(1),
                                            HasValue(2),
                                            HasValue(4),
                                            HasValue(4),
                                            HasValue(4),
                                            HasValue(6)));
}

TEST(Merge, Blanks)
{
    std::vector<int> range1 = {};
    std::vector<int> range2 = {};

    auto merge = makeMerge(range1.begin(), range1.end(),
                           range2.begin(), range2.end());

    EXPECT_THAT(merge, testing::ElementsAre());
}
