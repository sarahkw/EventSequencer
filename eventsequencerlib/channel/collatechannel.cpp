#include "collatechannel.h"

#include <eventsequencer.pb.h>

namespace channel {

CollateChannel::CollateChannel(Document& d, QObject *parent)
    : ChannelBase(parent), d_(d)
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
