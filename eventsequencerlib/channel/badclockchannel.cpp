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

ChannelType::Enum BadClockChannel::channelType() const
{
    return ChannelType::BadClock;
}

} // namespace channel
