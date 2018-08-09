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
    if (channelIndex_.hasSecond()) {
        // If we're a span inside a span, just stop here. It's not supported.
        return;
    }
    if (count < 0) {
        count = 0;
    }
    if (count_ != count) {
        count_ = count;
        emit countChanged();
        emit setSpan(channelIndex_, count);

        if (count_ > waiters_.size()) {
            for (int i = waiters_.size(); i < count_; ++i) {
                WaitFor* wf = d_.waitForChannelIndex(
                            ChannelIndex::make2(channelIndex_.first(), i));
                QObject::connect(wf, &WaitFor::resultChanged, this, &SpanChannel::waiterResultChanged);
                QObject::connect(wf, &WaitFor::resultAboutToUnset, this, &SpanChannel::waiterResultAboutToUnset);
                waiters_.emplace_back(wf);
                waiterResultChanged(wf->result());
            }
        } else if (count_ < waiters_.size()) {

            // We expect the waiters that we're removing are already nullptr,
            // because decreasing the span should have destroyed them already.
            for (int i = count_; i < waiters_.size(); ++i) {
                if (waiters_[i]->result() != nullptr) {
                    qWarning("SpanChannel expected subchannel to already be one...");
                }
            }

            waiters_.resize(count_);
        }
    }
}

std::vector<Strip *> SpanChannel::strips()
{
    return {};
}

std::vector<Strip *> SpanChannel::multiChannelStrips()
{
    std::vector<Strip*> ret;

    for (auto& waiteruptr : waiters_) {
        ChannelBase* cb = qobject_cast<ChannelBase*>(waiteruptr->result());
        if (cb != nullptr) {
            for (Strip* s : cb->strips()) {
                ret.push_back(s);
            }
        }
    }

    return ret;
}

void SpanChannel::channelStripSetChanged(ChannelIndex channelIndex)
{
    // No-op override
}

void SpanChannel::channelStripLocationChanged(ChannelIndex channelIndex, Strip *whichStrip)
{
    // No-op override
}

void SpanChannel::waiterResultChanged(QObject *channel)
{
    ChannelBase* castChannel = qobject_cast<ChannelBase*>(channel);
    if (castChannel != nullptr) {
        QObject::connect(castChannel, &ChannelBase::stripsChanged,
                         this, &SpanChannel::stripsChanged);

    }
    emit stripsChanged();
}

void SpanChannel::waiterResultAboutToUnset(QObject *channel)
{
    if (channel != nullptr) {
        channel->disconnect(this);
    }
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
