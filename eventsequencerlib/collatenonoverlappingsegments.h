#ifndef COLLATENONOVERLAPPINGSEGMENTS_H
#define COLLATENONOVERLAPPINGSEGMENTS_H

#include <map>

template <typename T>
class CollateNonOverlappingSegments
{
public:
    struct Segment {
        bool representsOverlappedSpace;
        int start;
        int length;
        T data;
    };

    enum class ReplaceMode {
        No,
        IfFitsInEmptySpace
    };

private:
    std::map<int, Segment> segments_;

public:

    const std::map<int, Segment>& segments()
    {
        return segments_;
    }

    void mergeSegment(int start, int length,
                      ReplaceMode mode=ReplaceMode::No, T data=T())
    {
        segments_.insert({start, {false, start, length, data}});

        //auto lower = segments_.lower_bound(start);
        //auto higher = segments_.lower_bound(start + length);
    }
};

#endif // COLLATENONOVERLAPPINGSEGMENTS_H
