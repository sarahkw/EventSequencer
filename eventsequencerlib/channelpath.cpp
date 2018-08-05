#include "channelpath.h"

ChannelPath::ChannelPath()
{

}

ChannelPath ChannelPath::make1(int first)
{
    ChannelPath cidx;
    cidx.first_ = first;
    return cidx;
}

int ChannelPath::first() const
{
    return first_;
}

bool ChannelPath::operator<(const ChannelPath &o) const
{
    return first_ < o.first_;
}

QString ChannelPath::toDebugString() const
{
    return QString("(ChannelIndex %1)").arg(first_);
}
