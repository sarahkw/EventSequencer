#ifndef COLLATENONOVERLAPPINGSEGMENTS_H
#define COLLATENONOVERLAPPINGSEGMENTS_H

#include <map>
#include <set>

#include <iostream>

template <typename T>
class CollateNonOverlappingSegments
{
public:
    enum class ReplaceMode {
        No,
        IfFitsInEmptySpace
    };

    struct Range {
        int start;
        int length;
    };

    struct CompareRanges {
        bool operator()(const Range& a, const Range& b) const
        {
            bool collides = (a.start < b.start ?
                                 (a.start + a.length > b.start) :
                                 (b.start + b.length > a.start));
            if (collides) return false;
            return a.start < b.start;
        }
    };

    struct Segment {
        int start;
        int length;
        bool representsOverlappedSpace;
        T data;
    };

    using ChosenRangesType = std::map<Range, T, CompareRanges>;
    using OccupiedRangesType = std::set<Range, CompareRanges>;

    class SegmentIterator : public std::iterator<std::input_iterator_tag, Segment> {
        typename ChosenRangesType::const_iterator crIter_;
        typename OccupiedRangesType::const_iterator orIter_;
    public:
        SegmentIterator(typename ChosenRangesType::const_iterator crIter,
                        typename OccupiedRangesType::const_iterator orIter)
            : crIter_(crIter), orIter_(orIter)
        {
        }
        SegmentIterator& operator++()
        {
            return *this;
        }
        bool operator!=(const SegmentIterator& other)
        {
            return !(crIter_ == other.crIter_ && orIter_ == other.orIter_);
        }
        const Segment& operator*()
        {
            static Segment placeholder;
            return placeholder;
        }
    };

    using value_type = Segment;
    using const_iterator = SegmentIterator;

private:
    ChosenRangesType chosenRanges_;
    OccupiedRangesType occupiedRanges_;

public:

    SegmentIterator begin() const
    {
        return SegmentIterator(chosenRanges_.begin(), occupiedRanges_.begin());
    }

    SegmentIterator end() const
    {
        return SegmentIterator(chosenRanges_.end(), occupiedRanges_.end());
    }

    void mergeSegment(int start, int length,
                      ReplaceMode mode=ReplaceMode::No, T data=T())
    {
        Range me{start, length};

        if (chosenRanges_.count(me) == 0) {
            if (mode == ReplaceMode::IfFitsInEmptySpace ||
                    (mode == ReplaceMode::No && occupiedRanges_.count(me) == 0)) {
                chosenRanges_.insert(std::make_pair(me, data));
            }
        }

        int earliest = start;
        int latest = start + length;
        auto intersection = occupiedRanges_.equal_range(me);
        if (intersection.first != occupiedRanges_.end()) {
            auto other = intersection.second;
            --other;
            earliest = std::min(earliest, intersection.first->start);
            latest = std::max(latest, intersection.second->start + intersection.second->length);
        }
        occupiedRanges_.erase(intersection.first, intersection.second);
        occupiedRanges_.insert({earliest, latest - earliest});

#if 0
        Segment me{false, start, length, data};
        if (segments_.count(me) == 0) {
            segments_.insert(std::move(me));
            return;
        }

        auto intersect = segments_.equal_range(me);

        if (intersect.first != intersect.second) {
            auto iter = intersect.first;
            ++iter;
            for (; iter != intersect.second; ++iter) {

            }
        }
#endif


#if 0
        std::cout << "mergeSegment" << std::endl;
        IntersectsWith iw(segments_, start, length);
        if (!iw.hasNext()) {
            std::cout << "noHasNext\n";
            segments_.insert({start, {false, start, length, data}});
        } else {
            // We need to subtract us from all the intersections. And then create
            // new segments.

            std::cout << "hasNext\n";

            auto iter = iw.next();

            if (iter->first > start) {
                std::cout << "isInsertFirst\n";
                segments_.insert({start, {true, start, iter->first - start, T()}});
            }

            while (iw.hasNext()) {
                auto theNext = iw.next();
                auto myStart = iter->first + iter->second.length;
                auto myEnd = theNext->first;
                segments_.insert({myStart, {true, myStart, myEnd - myStart, T()}});
                iter = theNext;
            }

            {
                auto myEnd = iter->first + iter->second.length;
                auto segEnd = start + length;
                if (myEnd < segEnd) {
                    segments_.insert({myEnd, {true, myEnd, segEnd - myEnd, T()}});
                }
            }
        }
#endif
    }
};

#endif // COLLATENONOVERLAPPINGSEGMENTS_H
