#ifndef GROUPBY_H
#define GROUPBY_H

#include <iterator>
#include <list>

// Just like python's itertools.groupby
template <typename SourceIter, typename KeyType, typename KeyFunctor>
class GroupBy {
    SourceIter begin_;
    SourceIter end_;
public:

    struct Group {
        KeyType key;
        std::list<SourceIter> items;
    };

    using value_type = Group;

    class const_iterator : public std::iterator<std::input_iterator_tag, Group> {
        const GroupBy* parent_ = nullptr;
        SourceIter position_;
        Group group_;
        SourceIter nextPosition_;
        bool populated_ = false;

        void populate()
        {
            group_.items.clear();

            auto iter = position_;
            group_.key = KeyFunctor()(*iter);
            group_.items.push_back(iter);
            ++iter;
            for (;;) {
                if (iter == parent_->end_) {
                    nextPosition_ = iter;
                    break;
                } else if (KeyFunctor()(*iter) == group_.key) {
                    group_.items.push_back(iter);
                    ++iter;
                } else {
                    nextPosition_ = iter;
                    break;
                }
            }
            populated_ = true;
        }

    public:
        const_iterator() {}
        const_iterator(const GroupBy& parent, SourceIter position)
            : parent_(&parent), position_(position)
        {
        }
        const_iterator& operator++()
        {
            if (!populated_) {
                populate();
            }
            position_ = nextPosition_;
            populated_ = false;
            return *this;
        }
        bool operator==(const const_iterator& other) const
        {
            return position_ == other.position_;
        }
        bool operator!=(const const_iterator& other) const
        {
            return !operator==(other);
        }
        Group& operator*()
        {
            if (!populated_) {
                populate();
            }
            return group_;
        }
        Group* operator->()
        {
            return &operator*();
        }
    };

    GroupBy(SourceIter begin, SourceIter end) : begin_(begin), end_(end) {}

    const_iterator begin() const
    {
        return const_iterator(*this, begin_);
    }

    const_iterator end() const
    {
        return const_iterator(*this, end_);
    }

};

template <typename KeyType, typename KeyFunctor, typename SourceIter>
GroupBy<SourceIter, KeyType, KeyFunctor> makeGroupBy(SourceIter begin,
                                                     SourceIter end)
{
    return GroupBy<SourceIter, KeyType, KeyFunctor>(begin, end);
}

#endif // GROUPBY_H
