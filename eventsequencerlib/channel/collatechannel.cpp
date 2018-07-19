#include "collatechannel.h"

#include <eventsequencer.pb.h>
#include <document.h>
#include <strip.h>

namespace channel {

CollateChannel::CollateChannel(Document& d, QObject *parent)
    : ChannelBase(parent), d_(d)
{
    QObject::connect(&d, &Document::stripMoved, this, &CollateChannel::stripMoved);
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

void CollateChannel::stripMoved(Strip *strip, int previousChannel, int previousStartFrame, int previousLength)
{
    Q_UNUSED(strip)
    Q_UNUSED(previousChannel)
    Q_UNUSED(previousStartFrame)
    Q_UNUSED(previousLength)
    // TODO Fill me in
}

} // namespace channel
