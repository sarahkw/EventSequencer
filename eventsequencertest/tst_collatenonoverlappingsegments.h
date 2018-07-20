#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "collatenonoverlappingsegments.h"

using namespace testing;

inline void PrintTo(const CollateNonOverlappingSegments<int>::Segment& s,
                    ::std::ostream *os)
{
    *os << "(Segment " << s.start << " " << s.length << " " <<
           s.representsOverlappedSpace << ")";
}

MATCHER_P2(IsRangeN, start, length, "") {
    return arg.start == start && arg.length == length && !arg.representsOverlappedSpace;
}

MATCHER_P2(IsRangeY, start, length, "") {
    return arg.start == start && arg.length == length && arg.representsOverlappedSpace;
}

TEST(CollateNonOverlappingSegments, NonOverlapping)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(2, 5 - 2);
    cnos.mergeSegment(5, 10 - 5);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(Pair(_, IsRangeN(2, 5 - 2)),
                            Pair(_, IsRangeN(5, 10 - 5))));
}

TEST(CollateNonOverlappingSegments, OverlapBefore)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(2, 5 - 2);
    cnos.mergeSegment(4, 10 - 4);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(Pair(_, IsRangeN(2, 5 - 2)),
                            Pair(_, IsRangeY(5, 10 - 5))));
}

TEST(CollateNonOverlappingSegments, OverlapAfter)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(4, 10 - 4);
    cnos.mergeSegment(2, 5 - 2);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(Pair(_, IsRangeY(2, 4 - 2)),
                            Pair(_, IsRangeN(4, 10 - 4))));
}

TEST(CollateNonOverlappingSegments, OverlapBetween)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(2, 5 - 2);
    cnos.mergeSegment(6, 10 - 6);
    cnos.mergeSegment(3, 7 - 3);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(Pair(_, IsRangeN(2, 5 - 2)),
                            Pair(_, IsRangeY(5, 6 - 5)),
                            Pair(_, IsRangeN(6, 10 - 6))));
}

TEST(CollateNonOverlappingSegments, OverlapWrapped)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(2, 5 - 2);
    cnos.mergeSegment(0, 10 - 0);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(Pair(_, IsRangeY(0, 2 - 0)),
                            Pair(_, IsRangeN(2, 5 - 2)),
                            Pair(_, IsRangeY(5, 10 - 5))));
}

TEST(CollateNonOverlappingSegments, Replace)
{
    using Cnos = CollateNonOverlappingSegments<int>;
    Cnos cnos;
    cnos.mergeSegment(0, 5 - 0, Cnos::ReplaceMode::IfFitsInEmptySpace);
    cnos.mergeSegment(2, 10 - 2, Cnos::ReplaceMode::IfFitsInEmptySpace); // Make empty space
    cnos.mergeSegment(6, 7 - 6, Cnos::ReplaceMode::IfFitsInEmptySpace);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(Pair(_, IsRangeN(0, 5 - 0)),
                            Pair(_, IsRangeY(5, 6 - 5)),
                            Pair(_, IsRangeN(6, 7 - 6)),
                            Pair(_, IsRangeY(7, 10 - 7))));
}

TEST(CollateNonOverlappingSegments, NoReplace)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(0, 5 - 0);
    cnos.mergeSegment(2, 10 - 2); // Make empty space
    cnos.mergeSegment(6, 7 - 6);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(Pair(_, IsRangeN(0, 5 - 0)),
                            Pair(_, IsRangeY(5, 10 - 5))));
}

TEST(CollateNonOverlappingSegments, ExpandEmptySpace)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(0, 5 - 0);
    cnos.mergeSegment(2, 10 - 2);
    cnos.mergeSegment(7, 15 - 7);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(Pair(_, IsRangeN(0, 5 - 0)),
                            Pair(_, IsRangeY(5, 15 - 5))));
}
