#include "channelindex.h"

ChannelIndex::ChannelIndex()
{

}

ChannelIndex ChannelIndex::make1(int first)
{
    ChannelIndex cidx;
    cidx.first_ = first;
    return cidx;
}

ChannelIndex ChannelIndex::make2(int first, int second)
{
    ChannelIndex cidx;
    cidx.first_ = first;
    cidx.hasSecond_ = true;
    cidx.second_ = second;
    return cidx;
}

int ChannelIndex::first() const
{
    return first_;
}

bool ChannelIndex::hasSecond() const
{
    return hasSecond_;
}

int ChannelIndex::second() const
{
    return second_;
}

bool ChannelIndex::operator<(const ChannelIndex &o) const
{
    if (first_ < o.first_) return true;
    else if (first_ > o.first_) return false;
    else if (hasSecond_ && !o.hasSecond_) return false;
    else if (!hasSecond_ && o.hasSecond_) return true;
    else if (hasSecond_ && o.hasSecond_) return second_ < o.second_;
    else return false; // Neither has second and first is equal -- Equal
}

bool ChannelIndex::operator==(const ChannelIndex &o) const
{
    return first_ == o.first_ && hasSecond_ == o.hasSecond_ && second_ == o.second_;
}

bool ChannelIndex::operator!=(const ChannelIndex &o) const
{
    return !operator==(o);
}

QString ChannelIndex::toDebugString() const
{
    return QString("(ChannelIndex %1)").arg(first_);
}
