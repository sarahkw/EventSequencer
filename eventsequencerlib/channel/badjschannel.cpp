#include "badjschannel.h"

#include <eventsequencer.pb.h>

namespace channel {

BadJsChannel::BadJsChannel(ChannelIndex channelIndex, Document& d, QObject* parent)
    : ChannelBase(channelIndex, d, parent)
{

}

void BadJsChannel::toPb(pb::ChannelData &pb) const
{
    pb.mutable_badjs();
}

void BadJsChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_badjs());
}

ChannelType::Enum BadJsChannel::channelType() const
{
    return ChannelType::BadJs;
}

} // namespace channel
