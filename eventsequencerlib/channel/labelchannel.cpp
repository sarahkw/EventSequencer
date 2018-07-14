#include "labelchannel.h"

#include <eventsequencer.pb.h>

namespace channel {

LabelChannel::LabelChannel(QObject *parent) : ChannelBase(parent)
{

}

void LabelChannel::toPb(pb::ChannelData &pb) const
{
    pb.mutable_label();
}

void LabelChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_label());
}

ChannelType::Enum LabelChannel::channelType() const
{
    return ChannelType::Label;
}

} // namespace channel
