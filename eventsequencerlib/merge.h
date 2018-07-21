#ifndef MERGE_H
#define MERGE_H

#include <iterator>

enum class MergeRangeSelect {
    Range1,
    Range2,
    RangeEqual
};

template <typename T1, typename T2>
struct MergeDefaultCompare {
    bool operator()(const T1& a, const T2& b) const
    {
        return a < b;
    }
};

template <typename R1Iter, typename R2Iter,
          typename Comp=MergeDefaultCompare<typename R1Iter::value_type, typename R2Iter::value_type>>
class Merge {
    R1Iter r1Begin_;
    R1Iter r1End_;
    R2Iter r2Begin_;
    R2Iter r2End_;
public:

    struct value_type {
        MergeRangeSelect select;
        R1Iter range1;
        R2Iter range2;
    };

    class const_iterator : public std::iterator<std::input_iterator_tag, value_type> {
        const Merge& parent_;
        value_type me_;
        bool classified_ = false;

        MergeRangeSelect classify() const
        {
            if (me_.range1 == parent_.r1End_) {
                if (me_.range2 == parent_.r2End_) {
                    return MergeRangeSelect::RangeEqual;
                } else {
                    return MergeRangeSelect::Range2;
                }
            } else if (me_.range2 == parent_.r2End_) {
                return MergeRangeSelect::Range1;
            } else if (Comp()(*me_.range1, *me_.range2)) {
                return MergeRangeSelect::Range1;
            } else if (Comp()(*me_.range2, *me_.range1)) {
                return MergeRangeSelect::Range2;
            } else {
                return MergeRangeSelect::RangeEqual;
            }
        }

    public:
        const_iterator(const Merge& parent,
                       R1Iter range1,
                       R2Iter range2) : parent_(parent)
        {
            me_.range1 = range1;
            me_.range2 = range2;
        }
        const_iterator& operator++()
        {
            if (!classified_) {
                me_.select = classify();
                classified_ = true;
            }
            switch (me_.select) {
            case MergeRangeSelect::Range1:
                ++me_.range1;
                break;
            case MergeRangeSelect::Range2:
                ++me_.range2;
                break;
            case MergeRangeSelect::RangeEqual:
                ++me_.range1;
                ++me_.range2;
                break;
            }
            classified_ = false;
            return *this;
        }
        bool operator==(const const_iterator& other)
        {
            return (me_.range1 == other.me_.range1 &&
                    me_.range2 == other.me_.range2);
        }
        bool operator!=(const const_iterator& other)
        {
            return !operator==(other);
        }
        const value_type& operator*()
        {
            if (!classified_) {
                me_.select = classify();
                classified_ = true;
            }
            return me_;
        }
    };

    Merge(R1Iter r1Begin, R1Iter r1End, R2Iter r2Begin, R2Iter r2End)
        : r1Begin_(r1Begin), r1End_(r1End), r2Begin_(r2Begin), r2End_(r2End)
    {
    }

    const_iterator begin() const
    {
        return const_iterator(*this, r1Begin_, r2Begin_);
    }

    const_iterator end() const
    {
        return const_iterator(*this, r1End_, r2End_);
    }
};

template <typename R1Iter, typename R2Iter>
Merge<R1Iter, R2Iter> makeMerge(R1Iter r1Begin, R1Iter r1End, R2Iter r2Begin,
                                R2Iter r2End)
{
    return Merge<R1Iter, R2Iter>(r1Begin, r1End, r2Begin, r2End);
}

template <typename Comp, typename R1Iter, typename R2Iter>
Merge<R1Iter, R2Iter, Comp> makeMergeComp(R1Iter r1Begin, R1Iter r1End,
                                          R2Iter r2Begin, R2Iter r2End)
{
    return Merge<R1Iter, R2Iter, Comp>(r1Begin, r1End, r2Begin, r2End);
}

#endif // MERGE_H
