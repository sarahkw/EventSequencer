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
        struct BrokenDown {
            enum class WhichRange {
                Chosen,
                Occupied
            } whichRange;
            enum class WhichPoint {
                Start,
                End
            } whichPoint;
            int thePoint;
            typename ChosenRangesType::const_iterator chosen;
            typename OccupiedRangesType::const_iterator occupied;
        };

        struct BrokenDownCompare {
            bool operator()(const BrokenDown& a, const BrokenDown& b) const
            {
                return a.thePoint < b.thePoint;
            }
        };

        class MapGenerateChosenRange {
            bool start = true;
        public:
            struct UseIteratorFlag {};
            using value_type = BrokenDown;

            bool nextState() {
                start = !start;
                return start; // Return true when flipping to true.
            }
            BrokenDown mapGenerateIterator(typename ChosenRangesType::const_iterator iter)
            {
                BrokenDown ret;
                ret.whichRange = BrokenDown::WhichRange::Chosen;
                ret.chosen = iter;
                if (start) {
                    ret.whichPoint = BrokenDown::WhichPoint::Start;
                    ret.thePoint = iter->first.start;
                } else {
                    ret.whichPoint = BrokenDown::WhichPoint::End;
                    ret.thePoint = iter->first.start + iter->first.length;
                }
                return ret;
            }
            bool operator==(const MapGenerateChosenRange& o) const
            {
                return start == o.start;
            }
        };

        class MapGenerateOccupiedRange {
            bool start = true;
        public:
            struct UseIteratorFlag {};
            using value_type = BrokenDown;
            bool nextState() {
                start = !start;
                return start; // Return true when flipping to true.
            }
            BrokenDown mapGenerateIterator(typename OccupiedRangesType::const_iterator iter)
            {
                BrokenDown ret;
                ret.whichRange = BrokenDown::WhichRange::Occupied;
                ret.occupied = iter;
                if (start) {
                    ret.whichPoint = BrokenDown::WhichPoint::Start;
                    ret.thePoint = iter->start;
                } else {
                    ret.whichPoint = BrokenDown::WhichPoint::End;
                    ret.thePoint = iter->start + iter->length;
                }
                return ret;
            }
            bool operator==(const MapGenerateOccupiedRange& o) const
            {
                return start == o.start;
            }
        };

        auto bdChosen = makeMapGenerate<MapGenerateChosenRange>(
                    chosenRanges_.begin(), chosenRanges_.end());
        auto bdOccupied = makeMapGenerate<MapGenerateOccupiedRange>(
                    occupiedRanges_.begin(), occupiedRanges_.end());

        auto merge = makeMergeComp<BrokenDownCompare>(
                    bdChosen.begin(), bdChosen.end(),
                    bdOccupied.begin(), bdOccupied.end());

#if 1
        struct GetKeyFunctor {
            int operator()(typename decltype(merge)::value_type& e) const
            {
                return e.derefHelper().thePoint;
            }
        };
#endif

        auto groupby = makeGroupBy<int, GetKeyFunctor>(merge.begin(), merge.end());

        std::vector<Segment> segments;

        BrokenDown currentChosen;
        bool hasCurrentChosen = false;

        int occupiedSince = 0;
        bool hasCurrentOccupied = false;

        for (auto& group : groupby) {
            const BrokenDown* startedChosen = nullptr;
            const BrokenDown* endedChosen = nullptr;
            const BrokenDown* startedOccupied = nullptr;
            const BrokenDown* endedOccupied = nullptr;

            for (auto& itemIter : group.items) {
                BrokenDown& bd = itemIter->derefHelper();
                const BrokenDown** target = nullptr;
                     if (bd.whichPoint == BrokenDown::WhichPoint::Start && bd.whichRange == BrokenDown::WhichRange::Chosen  ) target = &startedChosen;
                else if (bd.whichPoint == BrokenDown::WhichPoint::End   && bd.whichRange == BrokenDown::WhichRange::Chosen  ) target = &endedChosen;
                else if (bd.whichPoint == BrokenDown::WhichPoint::Start && bd.whichRange == BrokenDown::WhichRange::Occupied) target = &startedOccupied;
                else if (bd.whichPoint == BrokenDown::WhichPoint::End   && bd.whichRange == BrokenDown::WhichRange::Occupied) target = &endedOccupied;
                Q_ASSERT(target != nullptr);
                Q_ASSERT(*target == nullptr);
                *target = &bd;
            }
            Q_ASSERT(startedChosen != nullptr || endedChosen != nullptr || startedOccupied != nullptr || endedOccupied != nullptr);

            auto emitOccupied = [&]() {
                segments.push_back({ occupiedSince, group.key - occupiedSince, Segment::Type::Conflict, T() });
            };

            auto emitChosen = [&]() {
                segments.push_back({ currentChosen.chosen->first.start, currentChosen.chosen->first.length, Segment::Type::Chosen, currentChosen.chosen->second });
            };

            if (endedOccupied != nullptr) {
                Q_ASSERT(hasCurrentOccupied);
                if (!hasCurrentChosen) {
                    emitOccupied();
                }
                hasCurrentOccupied = false;
            }

            if (endedChosen != nullptr) {
                Q_ASSERT(hasCurrentChosen);
                emitChosen();
                hasCurrentChosen = false;

                if (hasCurrentOccupied) {
                    occupiedSince = group.key;
                }
            }

            if (startedChosen != nullptr) {
                if (hasCurrentOccupied) {
                    emitOccupied();
                }
                
                Q_ASSERT(!hasCurrentChosen);
                currentChosen = *startedChosen;
                hasCurrentChosen = true;
            }

            if (startedOccupied != nullptr) {
                occupiedSince = group.key;
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
