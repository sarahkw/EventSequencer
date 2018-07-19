#include "collatechannel.h"

#include <eventsequencer.pb.h>
#include <document.h>
#include <strip.h>

#include "collatechannelrefreshevent.h"

#include <QDebug>
#include <QCoreApplication>

namespace channel {

int CollateChannel::channelFrom() const
{
    return channelFrom_;
}

void CollateChannel::setChannelFrom(int channelFrom)
{
    if (channelFrom_ != channelFrom) {
        channelFrom_ = channelFrom;
        emit channelFromChanged();
        recalculate();
    }
}

int CollateChannel::channelTo() const
{
    return channelTo_;
}

void CollateChannel::setChannelTo(int channelTo)
{
    if (channelTo_ != channelTo) {
        channelTo_ = channelTo;
        emit channelToChanged();
        recalculate();
    }
}

bool CollateChannel::event(QEvent *event)
{
    if (event->type() == CollateChannelRefreshEvent::s_CustomType) {
        refreshPending_ = false;
        recalculate();
        return true;
    }
    return ChannelBase::event(event);
}

CollateChannel::CollateChannel(Document& d, QObject *parent)
    : ChannelBase(parent), d_(d)
{
    QObject::connect(&d, &Document::stripAfterPlaced, this, &CollateChannel::stripAfterAdd);
    QObject::connect(&d, &Document::stripBeforeDelete, this, &CollateChannel::stripBeforeDelete);
    QObject::connect(&d, &Document::stripMoved, this, &CollateChannel::stripMoved);
}

void CollateChannel::toPb(pb::ChannelData &pb) const
{
    auto mut = pb.mutable_collate();
    mut->set_channelfrom(channelFrom());
    mut->set_channelto(channelTo());
}

void CollateChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_collate());
    setChannelFrom(pb.collate().channelfrom());
    setChannelTo(pb.collate().channelto());
}

ChannelType::Enum CollateChannel::channelType() const
{
    return ChannelType::Collate;
}

void CollateChannel::stripAfterAdd(Strip *strip)
{
    channelAffected(strip->channel());
}

void CollateChannel::stripBeforeDelete(Strip *strip)
{
    channelAffected(strip->channel());
}

void CollateChannel::stripMoved(Strip *strip, int previousChannel, int previousStartFrame, int previousLength)
{
    Q_UNUSED(previousStartFrame)
    Q_UNUSED(previousLength)
    channelAffected(strip->channel());
    channelAffected(previousChannel);
}

void CollateChannel::channelAffected(int channel)
{
    if (refreshPending_) {
        return;
    }

    bool needRefresh = channel >= channelFrom() && channel < channelTo();

    if (needRefresh) {
        refreshPending_ = true;
        QCoreApplication::postEvent(this, new CollateChannelRefreshEvent);
    }
}

void CollateChannel::recalculate()
{
    // TODO
    qInfo() << __PRETTY_FUNCTION__ << "TODO";
}

} // namespace channel
