#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "collatenonoverlappingsegments.h"

using namespace testing;

inline void PrintTo(const CollateNonOverlappingSegments<int>::Segment& s,
                    ::std::ostream *os)
{
    std::string typeName;
    switch (s.type) {
    case s.Type::Chosen:
        typeName = "chosen";
        break;
    case s.Type::Conflict:
        typeName = "conflict";
        break;
    case s.Type::Empty:
        typeName = "empty";
        break;
    }

    *os << "(Segment " << s.start << " " << s.length << " " <<
           typeName << ")";
}

MATCHER_P3(IsRange, start, length, type, "") {
    return arg.start == start && arg.length == length && arg.type == type;
}

using SegmentType = CollateNonOverlappingSegments<int>::Segment::Type;

TEST(CollateNonOverlappingSegments, NonOverlapping)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(2, 5 - 2);
    cnos.mergeSegment(5, 10 - 5);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(IsRange(2, 5 - 2, SegmentType::Chosen),
                            IsRange(5, 10 - 5, SegmentType::Chosen)));
}

TEST(CollateNonOverlappingSegments, OverlapBefore)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(2, 5 - 2);
    cnos.mergeSegment(4, 10 - 4);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(IsRange(2, 5 - 2, SegmentType::Chosen),
                            IsRange(5, 10 - 5, SegmentType::Conflict)));
}

TEST(CollateNonOverlappingSegments, OverlapAfter)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(4, 10 - 4);
    cnos.mergeSegment(2, 5 - 2);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(IsRange(2, 4 - 2, SegmentType::Conflict),
                            IsRange(4, 10 - 4, SegmentType::Chosen)));
}

TEST(CollateNonOverlappingSegments, OverlapBetween)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(2, 5 - 2);
    cnos.mergeSegment(6, 10 - 6);
    cnos.mergeSegment(3, 7 - 3);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(IsRange(2, 5 - 2, SegmentType::Chosen),
                            IsRange(5, 6 - 5, SegmentType::Conflict),
                            IsRange(6, 10 - 6, SegmentType::Chosen)));
}

TEST(CollateNonOverlappingSegments, OverlapWrapped)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(2, 5 - 2);
    cnos.mergeSegment(0, 10 - 0);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(IsRange(0, 2 - 0, SegmentType::Conflict),
                            IsRange(2, 5 - 2, SegmentType::Chosen),
                            IsRange(5, 10 - 5, SegmentType::Conflict)));
}

TEST(CollateNonOverlappingSegments, Replace)
{
    using Cnos = CollateNonOverlappingSegments<int>;
    Cnos cnos;
    cnos.mergeSegment(0, 5 - 0, Cnos::ReplaceMode::IfFitsInEmptySpace);
    cnos.mergeSegment(2, 10 - 2, Cnos::ReplaceMode::IfFitsInEmptySpace); // Make empty space
    cnos.mergeSegment(6, 7 - 6, Cnos::ReplaceMode::IfFitsInEmptySpace);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(IsRange(0, 5 - 0, SegmentType::Chosen),
                            IsRange(5, 6 - 5, SegmentType::Conflict),
                            IsRange(6, 7 - 6, SegmentType::Chosen),
                            IsRange(7, 10 - 7, SegmentType::Conflict)));
}

TEST(CollateNonOverlappingSegments, NoReplace)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(0, 5 - 0);
    cnos.mergeSegment(2, 10 - 2); // Make empty space
    cnos.mergeSegment(6, 7 - 6);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(IsRange(0, 5 - 0, SegmentType::Chosen),
                            IsRange(5, 10 - 5, SegmentType::Conflict)));
}

TEST(CollateNonOverlappingSegments, ExpandEmptySpace)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(0, 5 - 0);
    cnos.mergeSegment(2, 10 - 2);
    cnos.mergeSegment(7, 15 - 7);

    ASSERT_THAT(cnos.segments(),
                ElementsAre(IsRange(0, 5 - 0, SegmentType::Chosen),
                            IsRange(5, 15 - 5, SegmentType::Conflict)));
}

TEST(CollateNonOverlappingSegments, ObservedProblem)
{
    // It turned out to be a mistake doing iteration. Test isn't significant.
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(26, 31 - 26);
    cnos.mergeSegment(17, 22 - 17);
    cnos.mergeSegment(20, 25 - 20);
    

    ASSERT_THAT(cnos.segments(),
                ElementsAre(IsRange(17, 22 - 17, SegmentType::Chosen),
                            IsRange(22, 25 - 22, SegmentType::Conflict),
                            IsRange(26, 31 - 26, SegmentType::Chosen)));
}

TEST(CollateNonOverlappingSegments, EmptyInBetween)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(0, 10 - 0);
    cnos.mergeSegment(20, 30 - 20);


    ASSERT_THAT(cnos.segments(cnos.WantEmpties::DoWantEmpties),
                ElementsAre(IsRange(0, 10 - 0, SegmentType::Chosen),
                            IsRange(10, 20 - 10, SegmentType::Empty),
                            IsRange(20, 30 - 20, SegmentType::Chosen)));
}

TEST(CollateNonOverlappingSegments, BoundaryEmptyBlank)
{
    CollateNonOverlappingSegments<int> cnos;

    ASSERT_THAT(cnos.segments(cnos.WantEmpties::DoWantBoundaryEmpties, 10, 90),
                ElementsAre(IsRange(10, 90, SegmentType::Empty)));
}

TEST(CollateNonOverlappingSegments, BoundaryEmptySurround)
{
    CollateNonOverlappingSegments<int> cnos;
    cnos.mergeSegment(10, 20 - 10);

    ASSERT_THAT(cnos.segments(cnos.WantEmpties::DoWantBoundaryEmpties, 0, 30),
                ElementsAre(IsRange(0, 10, SegmentType::Empty),
                            IsRange(10, 20 - 10, SegmentType::Chosen),
                            IsRange(20, 30 - 20, SegmentType::Empty)));
}
