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
    return cc_.segments_.size();
}

QVariant CollateChannelModel::data(const QModelIndex &index, int role) const
{
    if (index.column() == 0 && index.row() >= 0 &&
            static_cast<unsigned>(index.row()) < cc_.segments_.size()) {
        switch (role) {
        case SegmentStartRole: return cc_.segments_[index.row()].segmentStart;
        case SegmentLengthRole: return cc_.segments_[index.row()].segmentLength;
        case SegmentColorRole: return cc_.segments_[index.row()].segmentColor;
        }
    }

    return QVariant();
}

QHash<int, QByteArray> CollateChannelModel::roleNames() const
{
    return {
        {SegmentStartRole, "segmentStart"},
        {SegmentLengthRole, "segmentLength"},
        {SegmentColorRole, "segmentColor"}
    };
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
        triggerRefresh();
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
        triggerRefresh();
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
    model_.beginResetModel();
    segments_.clear();
    segments_.push_back({d_.startFrame(),
                         d_.endFrame() - d_.startFrame(),
                         QColor(Qt::black)
                        });
    model_.endResetModel();

    struct TmpSeg {
        const Strip* s;
        int startFrame;
        int length;
        TmpSeg(const Strip* s, int startFrame, int length) : s(s), startFrame(startFrame), length(length) { }
        bool collidesWith(int otherStartFrame, int otherLength) const
        {
            return (startFrame < otherStartFrame ?
                        (startFrame + length > otherStartFrame) :
                        (otherStartFrame + otherLength > startFrame));
        }
    };
    std::vector<TmpSeg> tmpSeg;

    for (int i = channelTo() - 1; i >= channelFrom(); --i) {
        for (const Strip* s : d_.strips()) {
            if (s->channel() != i) continue;

            if (std::none_of(tmpSeg.begin(), tmpSeg.end(),
                        [s](TmpSeg& ts) { return ts.collidesWith(s->startFrame(), s->length()); })) {
                tmpSeg.emplace_back(s, s->startFrame(), s->length());
            }
        }
    }

    std::sort(tmpSeg.begin(), tmpSeg.end(), [](const TmpSeg& a, const TmpSeg& b) { return a.startFrame < b.startFrame; });

    // TODO
    qInfo() << __PRETTY_FUNCTION__ << "TODO";

    for (auto& ts : tmpSeg) {
        qInfo() << ts.startFrame << ts.length;
    }

}

} // namespace channel
