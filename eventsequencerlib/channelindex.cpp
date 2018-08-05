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

int ChannelIndex::first() const
{
    return first_;
}

bool ChannelIndex::operator<(const ChannelIndex &o) const
{
    return first_ < o.first_;
}

bool ChannelIndex::operator==(const ChannelIndex &o) const
{
    return first_ == o.first_;
}

bool ChannelIndex::operator!=(const ChannelIndex &o) const
{
    return !operator==(o);
}

QString ChannelIndex::toDebugString() const
{
    return QString("(ChannelIndex %1)").arg(first_);
}
