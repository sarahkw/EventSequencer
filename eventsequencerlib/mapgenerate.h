#ifndef MAPGENERATE_H
#define MAPGENERATE_H

#include <iterator>

template <typename SourceIter, typename GenerateLogic>
class MapGenerate {
    SourceIter sourceBegin_;
    SourceIter sourceEnd_;
public:

    using value_type = typename GenerateLogic::value_type;

    class const_iterator : public std::iterator<std::input_iterator_tag, value_type> {
        SourceIter me_;
        GenerateLogic genLogic_;
        value_type value_;
        bool valueIsSet_ = false;
    public:
        const_iterator(SourceIter me) : me_(me)
        {
        }
        const_iterator& operator++()
        {
            if (!genLogic_.nextState()) {
                ++me_;
            }
            valueIsSet_ = false;
            return *this;
        }
        bool operator==(const const_iterator& other)
        {
            return me_ == other.me_ && genLogic_ == other.genLogic_;
        }
        bool operator!=(const const_iterator& other)
        {
            return !operator==(other);
        }
        const value_type& operator*()
        {
            if (!valueIsSet_) {
                value_ = genLogic_.mapGenerate(*me_);
                valueIsSet_ = true;
            }
            return value_;
        }
    };

    MapGenerate(SourceIter sourceBegin, SourceIter sourceEnd)
        : sourceBegin_(sourceBegin), sourceEnd_(sourceEnd)
    {
    }

    const_iterator begin() const
    {
        return const_iterator(sourceBegin_);
    }

    const_iterator end() const
    {
        return const_iterator(sourceEnd_);
    }
};

template <typename GenerateLogic, typename SourceIter>
MapGenerate<SourceIter, GenerateLogic> makeMapGenerate(SourceIter sourceBegin,
                                                       SourceIter sourceEnd)
{
    return MapGenerate<SourceIter, GenerateLogic>(sourceBegin, sourceEnd);
}

#endif // MAPGENERATE_H
