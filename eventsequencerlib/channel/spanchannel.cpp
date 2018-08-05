#include "spanchannel.h"

#include <eventsequencer.pb.h>

namespace channel {

int SpanChannel::count() const
{
    return count_;
}

void SpanChannel::setCount(int count)
{
    if (count_ != count) {
        count_ = count;
        emit countChanged();
    }
}

SpanChannel::SpanChannel(int channelIndex, Document& d, QObject* parent)
    : ChannelBase(channelIndex, d, parent)
{

}

void SpanChannel::toPb(pb::ChannelData &pb) const
{
    auto mut = pb.mutable_span();
    mut->set_count(count());
}

void SpanChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_span());
    setCount(pb.span().count());
}

ChannelType::Enum SpanChannel::channelType() const
{
    return ChannelType::Span;
}

} // namespace channel
