#include "collatechannel.h"

#include <eventsequencer.pb.h>

namespace channel {

CollateChannel::CollateChannel(QObject *parent) : ChannelBase(parent)
{

}

void CollateChannel::toPb(pb::ChannelData &pb) const
{
    pb.mutable_collate();
}

void CollateChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_collate());
}

ChannelType::Enum CollateChannel::channelType() const
{
    return ChannelType::Collate;
}

} // namespace channel
