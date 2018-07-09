#include "badjschannel.h"

#include <eventsequencer.pb.h>

namespace channel {

BadJsChannel::BadJsChannel(QObject *parent) : ChannelBase(parent)
{

}

void BadJsChannel::toPb(pb::ChannelData &pb) const
{
    pb.mutable_badjs();
}

ChannelType::Enum BadJsChannel::channelType() const
{
    return ChannelType::BadJs;
}

} // namespace channel
