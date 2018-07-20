#ifndef COLLATENONOVERLAPPINGSEGMENTS_H
#define COLLATENONOVERLAPPINGSEGMENTS_H

#include <map>

#include <iostream>

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

    class IntersectsWith {
        const std::map<int, Segment>& segments_;
        int start_;
        int length_;
        using IterType = typename std::map<int, Segment>::const_iterator;
        IterType iter_;
        bool hasNext_ = false;
    public:
        IntersectsWith(const std::map<int, Segment>& segments, int start,
                       int length)
            : segments_(segments), start_(start), length_(length)
        {
            if (segments_.empty()) {
                hasNext_ = false;
                return;
            }

            // Find the left-most item that collides with us. It could be before
            // lower bound, or at the lower bound.

            iter_ = segments_.lower_bound(start);
            if (iter_ != segments_.begin()) {
                --iter_;
                // Does iter_ run into us?
                if (iter_->first + iter_->second.length > start_) {
                    std::cout << "ran into us" << std::endl;
                    hasNext_ = true;
                    return;
                }
                ++iter_;
            }

            if (iter_ != segments_.end() && start_ + length_ > iter_->first) {
                std::cout << "run into them\n";
                hasNext_ = true;
                return;
            }

            hasNext_ = false;
        }
        bool hasNext() const
        {
            return hasNext_;
        }
        IterType next()
        {
            auto ret = iter_;
            ++iter_;
            if (iter_ == segments_.end()) {
                hasNext_ = false;
            } else if (start_ + length_ > iter_->first) {
                hasNext_ = true;
            } else {
                hasNext_ = false;
            }
            return ret;
        }
    };

public:

    const std::map<int, Segment>& segments()
    {
        return segments_;
    }

    void mergeSegment(int start, int length,
                      ReplaceMode mode=ReplaceMode::No, T data=T())
    {
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
    }
};

#endif // COLLATENONOVERLAPPINGSEGMENTS_H
