#include "badclockchannel.h"

namespace channel {

BadClockChannel::BadClockChannel(QObject *parent) : ChannelBase(parent)
{

}

ChannelType::Enum BadClockChannel::channelType() const
{
    return ChannelType::BadClock;
}

} // namespace channel
