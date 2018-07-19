#include "collatechannel.h"

#include <eventsequencer.pb.h>
#include <document.h>
#include <strip.h>

#include "collatechannelrefreshevent.h"

#include <QDebug>
#include <QCoreApplication>

namespace channel {

int CollateChannelModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 0;
}

QVariant CollateChannelModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    return QVariant();
}

QHash<int, QByteArray> CollateChannelModel::roleNames() const
{
    return {{ModelDataRole, "modelData"}};
}

/******************************************************************************/

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

void CollateChannel::triggerRefresh()
{
    if (!refreshPending_) {
        refreshPending_ = true;
        QCoreApplication::postEvent(this, new CollateChannelRefreshEvent);
    }
}

CollateChannel::CollateChannel(Document& d, QObject *parent)
    : ChannelBase(parent), model_(*this), d_(d)
{
    QObject::connect(&d, &Document::stripAfterPlaced, this, &CollateChannel::stripAfterAdd);
    QObject::connect(&d, &Document::stripBeforeDelete, this, &CollateChannel::stripBeforeDelete);
    QObject::connect(&d, &Document::stripMoved, this, &CollateChannel::stripMoved);
    QObject::connect(&d, &Document::startFrameChanged, this, &CollateChannel::triggerRefresh);
    QObject::connect(&d, &Document::endFrameChanged, this, &CollateChannel::triggerRefresh);

    triggerRefresh();
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

QAbstractListModel *CollateChannel::model()
{
    return &model_;
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
    if (refreshPending_) { // Optimization
        return;
    }

    if (channel >= channelFrom() && channel < channelTo()) {
        triggerRefresh();
    }
}

void CollateChannel::recalculate()
{
    d_.startFrame();
    d_.endFrame();

    // TODO
    qInfo() << __PRETTY_FUNCTION__ << "TODO";
}

} // namespace channel
