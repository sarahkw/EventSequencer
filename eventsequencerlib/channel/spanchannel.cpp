#include "spanchannel.h"

#include "document.h"
#include "visualpositionmanager.h"

#include <eventsequencer.pb.h>

namespace channel {

int SpanChannel::count() const
{
    return count_;
}

void SpanChannel::setCount(int count)
{
    if (count < 0) {
        count = 0;
    }
    if (count_ != count) {
        count_ = count;
        emit countChanged();
        emit setSpan(channelIndex_, count);
    }
}

std::vector<Strip *> SpanChannel::strips()
{
    return {};
}

std::vector<Strip *> SpanChannel::multiChannelStrips()
{
    std::vector<Strip*> ret;
    if (!channelIndex_.hasSecond()) {
        auto pair = d_.stripsOnChannel().stripsChildOfChannel(channelIndex_.first());
        for (auto it = pair.first; it != pair.second; ++it) {
            for (auto& sh : it->second) {
                ret.push_back(sh.strip);
            }
        }
    }
    return ret;
}

void SpanChannel::channelStripSetChanged(ChannelIndex channelIndex)
{
    if (!channelIndex_.hasSecond()) {
        if (channelIndex.hasSecond() && channelIndex.first() == channelIndex_.first()) {
            emit stripsChanged();
        }
    }
}

void SpanChannel::channelStripLocationChanged(ChannelIndex channelIndex, Strip *whichStrip)
{
    channelStripSetChanged(channelIndex);
}

SpanChannel::SpanChannel(ChannelIndex channelIndex, Document& d, QObject* parent)
    : ChannelBase(channelIndex, d, parent), channelIndex_(channelIndex), d_(d)
{
    QObject::connect(this, &SpanChannel::setSpan,
                     &d.channelPositionManager(),
                     qOverload<ChannelIndex, unsigned>(&VisualPositionManager::setSpan));
}

SpanChannel::~SpanChannel()
{
    emit setSpan(channelIndex_, 0);
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
