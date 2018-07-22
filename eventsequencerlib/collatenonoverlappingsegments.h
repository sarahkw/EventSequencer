#ifndef COLLATENONOVERLAPPINGSEGMENTS_H
#define COLLATENONOVERLAPPINGSEGMENTS_H

#include "mapgenerate.h"
#include "merge.h"
#include "groupby.h"

#include <map>
#include <set>
#include <vector>

#include <QDebug>
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
        enum class Type {
            //Empty, // TODO
            Chosen,
            Conflict
        } type;
        T data;
    };

    using ChosenRangesType = std::map<Range, T, CompareRanges>;
    using OccupiedRangesType = std::set<Range, CompareRanges>;

#if 0
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
#endif

private:
    ChosenRangesType chosenRanges_;
    OccupiedRangesType occupiedRanges_;

public:

#if 0
    SegmentIterator begin() const
    {
        return SegmentIterator(chosenRanges_.begin(), occupiedRanges_.begin());
    }

    SegmentIterator end() const
    {
        return SegmentIterator(chosenRanges_.end(), occupiedRanges_.end());
    }
#endif

    std::vector<Segment> segments()
    {
        class Time {
        public:
            using CI = typename ChosenRangesType::const_iterator;
            using OI = typename OccupiedRangesType::const_iterator;
        private:
            CI chosenBegin_;
            CI chosenEnd_;
            OI occupiedBegin_;
            OI occupiedEnd_;
            bool hasChosenBegin_   = false;
            bool hasChosenEnd_     = false;
            bool hasOccupiedBegin_ = false;
            bool hasOccupiedEnd_   = false;
        public:
            void setChosenBegin  (CI val) { Q_ASSERT(!hasChosenBegin_  ); chosenBegin_   = val; hasChosenBegin_   = true; }
            void setChosenEnd    (CI val) { Q_ASSERT(!hasChosenEnd_    ); chosenEnd_     = val; hasChosenEnd_     = true; }
            void setOccupiedBegin(OI val) { Q_ASSERT(!hasOccupiedBegin_); occupiedBegin_ = val; hasOccupiedBegin_ = true; }
            void setOccupiedEnd  (OI val) { Q_ASSERT(!hasOccupiedEnd_  ); occupiedEnd_   = val; hasOccupiedEnd_   = true; }
            const CI& chosenBegin  () { Q_ASSERT(hasChosenBegin_  ); return chosenBegin_  ; }
            const CI& chosenEnd    () { Q_ASSERT(hasChosenEnd_    ); return chosenEnd_    ; }
            const OI& occupiedBegin() { Q_ASSERT(hasOccupiedBegin_); return occupiedBegin_; }
            const OI& occupiedEnd  () { Q_ASSERT(hasOccupiedEnd_  ); return occupiedEnd_  ; }
            bool hasChosenBegin()   const { return hasChosenBegin_  ; }
            bool hasChosenEnd()     const { return hasChosenEnd_    ; }
            bool hasOccupiedBegin() const { return hasOccupiedBegin_; }
            bool hasOccupiedEnd()   const { return hasOccupiedEnd_  ; }
        };
        
        std::map<int, Time> times;
        for (auto iter = chosenRanges_.begin(); iter != chosenRanges_.end(); ++iter) {
            times[iter->first.start].setChosenBegin(iter);
            times[iter->first.start + iter->first.length].setChosenEnd(iter);
        }
        for (auto iter = occupiedRanges_.begin(); iter != occupiedRanges_.end(); ++iter) {
            times[iter->start].setOccupiedBegin(iter);
            times[iter->start + iter->length].setOccupiedEnd(iter);
        }

        std::vector<Segment> segments;

        typename ChosenRangesType::const_iterator currentChosen;
        bool hasCurrentChosen = false;

        int occupiedSince = 0;
        bool hasCurrentOccupied = false;

        for (auto& time : times) {
            auto emitOccupied = [&]() {
                segments.push_back({ occupiedSince, time.first - occupiedSince, Segment::Type::Conflict, T() });
            };

            auto emitChosen = [&]() {
                segments.push_back({ currentChosen->first.start, currentChosen->first.length, Segment::Type::Chosen, currentChosen->second });
            };

            if (time.second.hasChosenEnd()) {
                Q_ASSERT(hasCurrentChosen);
                emitChosen();
                hasCurrentChosen = false;

                if (hasCurrentOccupied) {
                    occupiedSince = time.first;
                }
            } else if (time.second.hasOccupiedEnd()) {
                Q_ASSERT(hasCurrentOccupied);
                emitOccupied();
            }

            if (time.second.hasOccupiedEnd()) {
                hasCurrentOccupied = false;
            }

            if (time.second.hasChosenBegin()) {
                if (hasCurrentOccupied) {
                    emitOccupied();
                }
                
                Q_ASSERT(!hasCurrentChosen);
                currentChosen = time.second.chosenBegin();
                hasCurrentChosen = true;
            }

            if (time.second.hasOccupiedBegin()) {
                occupiedSince = time.first;
                hasCurrentOccupied = true;
            }
        }

        return segments;
    }

    void mergeSegment(int start, int length,
                      ReplaceMode mode=ReplaceMode::No, T data=T())
    {
        if (length <= 0) {
            qWarning() << "Ignored segment of length" << length;
            return;
        }

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
