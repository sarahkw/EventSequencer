#include "badclockchannel.h"

#include <eventsequencer.pb.h>

namespace channel {

BadClockChannel::BadClockChannel(QObject *parent) : ChannelBase(parent)
{

}

void BadClockChannel::toPb(pb::ChannelData &pb) const
{
    pb.mutable_badclock();
}

void BadClockChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_badclock());
}

ChannelType::Enum BadClockChannel::channelType() const
{
    return ChannelType::BadClock;
}

} // namespace channel
