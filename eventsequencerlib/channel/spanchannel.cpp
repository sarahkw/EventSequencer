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
                const ChannelIndex myNewIndex = ChannelIndex::make2(channelIndex_.first(), i);

                if (defaultChannelType() != ChannelType::UNSET) {
                    d_.createChannel(myNewIndex, defaultChannelType());
                }

                WaitFor* wf = d_.waitForChannelIndex(myNewIndex);
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

ChannelType::Enum SpanChannel::defaultChannelType() const
{
    return defaultChannelType_;
}

void SpanChannel::setDefaultChannelType(const ChannelType::Enum &defaultChannelType)
{
    if (defaultChannelType_ != defaultChannelType) {
        defaultChannelType_ = defaultChannelType;
        emit defaultChannelTypeChanged();
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

Strip *SpanChannel::createStrip(int startFrame, int length)
{
    return nullptr;
}

const std::vector<std::unique_ptr<WaitFor> > &SpanChannel::waitersForChildChannels() const
{
    return waiters_;
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
    mut->set_defaultchanneltype(ChannelType::toInt(defaultChannelType_));
}

void SpanChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_span());
    setCount(pb.span().count());
    setDefaultChannelType(ChannelType::fromInt(pb.span().defaultchanneltype()));
}

ChannelType::Enum SpanChannel::channelType() const
{
    return ChannelType::Span;
}

} // namespace channel
