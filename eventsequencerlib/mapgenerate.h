#ifndef MAPGENERATE_H
#define MAPGENERATE_H

#include <iterator>

template <typename MapFunctor>
struct MapGenerateNoGenerate {
    struct NoIteratorFlag {};
    using value_type = typename MapFunctor::DstType;
    bool nextState() { return false; }
    bool operator==(const MapGenerateNoGenerate&) const { return true; }
    typename MapFunctor::DstType mapGenerate(const typename MapFunctor::SrcType& src)
    {
        return MapFunctor()(src);
    }
};

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

        template <typename GL>
        const value_type& dispatchDeref(typename GL::UseIteratorFlag* =nullptr)
        {
            if (!valueIsSet_) {
                value_ = genLogic_.mapGenerateIterator(me_);
                valueIsSet_ = true;
            }
            return value_;
        }

        template <typename GL>
        const value_type& dispatchDeref(typename GL::NoIteratorFlag* =nullptr)
        {
            if (!valueIsSet_) {
                value_ = genLogic_.mapGenerate(*me_);
                valueIsSet_ = true;
            }
            return value_;
        }

    public:
        const_iterator()
        {
        }
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
        const_iterator operator++(int) // Postfix
        {
            const_iterator ret(*this);
            operator++();
            return ret;
        }
        bool operator==(const const_iterator& other) const
        {
            return me_ == other.me_ && genLogic_ == other.genLogic_;
        }
        bool operator!=(const const_iterator& other) const
        {
            return !operator==(other);
        }
        const value_type& operator*()
        {
            return dispatchDeref<GenerateLogic>(); // SFINAE
        }
        const typename SourceIter::reference beforeMap()
        {
            return *me_;
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

template <typename Func, typename SourceIter>
MapGenerate<SourceIter, MapGenerateNoGenerate<Func>> makeMapGenerateNoGenerate(
    SourceIter sourceBegin, SourceIter sourceEnd)
{
    return MapGenerate<SourceIter, MapGenerateNoGenerate<Func>>(sourceBegin,
                                                                sourceEnd);
}

#endif // MAPGENERATE_H
