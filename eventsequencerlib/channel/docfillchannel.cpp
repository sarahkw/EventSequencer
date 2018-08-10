#include "docfillchannel.h"

#include <eventsequencer.pb.h>

namespace channel {

ChannelIndex DocFillChannel::textChannel() const
{
    return textChannel_;
}

void DocFillChannel::setTextChannel(const ChannelIndex &textChannel)
{
    if (textChannel_ != textChannel) {
        textChannel_ = textChannel;
        emit textChannelChanged();
    }
}

ChannelIndex DocFillChannel::resourceChannel() const
{
    return resourceChannel_;
}

void DocFillChannel::setResourceChannel(const ChannelIndex &resourceChannel)
{
    if (resourceChannel_ != resourceChannel) {
        resourceChannel_ = resourceChannel;
        emit resourceChannelChanged();
    }
}

DocFillChannel::DocFillChannel(ChannelIndex channelIndex, Document& d, QObject* parent)
    : ChannelBase(channelIndex, d, parent)
{

}

void DocFillChannel::toPb(pb::ChannelData &pb) const
{
    auto* mut = pb.mutable_docfill();
    textChannel().toPb(*mut->mutable_textchannel());
    resourceChannel().toPb(*mut->mutable_resourcechannel());
}

void DocFillChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_docfill());
    setTextChannel(ChannelIndex::makeFromPb(pb.docfill().textchannel()));
    setResourceChannel(ChannelIndex::makeFromPb(pb.docfill().resourcechannel()));
}

ChannelType::Enum DocFillChannel::channelType() const
{
    return ChannelType::DocFill;
}

} // namespace channel
