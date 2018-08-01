#include "collatechannel.h"

#include <eventsequencer.pb.h>
#include <document.h>
#include <strip.h>

#include "collatechannelrefreshevent.h"
#include "collatenonoverlappingsegments.h"

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

void CollateChannel::channelStripSetChanged(int channel)
{
    channelAffected(channel);
}

void CollateChannel::channelStripLocationChanged(int channel, Strip *whichStrip)
{
    Q_UNUSED(whichStrip);
    channelAffected(channel);
}

CollateChannel::CollateChannel(int channelIndex, Document& d, QObject *parent)
    : ChannelBase(channelIndex, d, parent), model_(*this), d_(d)
{   
    QObject::connect(&d.stripsOnChannel(), &DocumentStripsOnChannel::channelStripSetChanged, this, &CollateChannel::channelStripSetChanged);
    QObject::connect(&d.stripsOnChannel(), &DocumentStripsOnChannel::channelStripLocationChanged, this, &CollateChannel::channelStripLocationChanged);

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
    CollateNonOverlappingSegments<int> cnos(cnos.BoundaryMode::HasBounds,
                                            d_.startFrame(),
                                            d_.endFrame() - d_.startFrame());
    {
        for (int channel = channelTo() - 1; channel >= channelFrom(); --channel) {
            auto stripSet = d_.stripsOnChannel().stripsForChannel(channel);
            if (stripSet != nullptr) {
                for (auto& s : *stripSet) {
                    cnos.mergeSegment(s.strip->startFrame(), s.strip->length());
                }
            }
        }
    }

    model_.beginResetModel();
    segments_.clear();

    for (auto& segment : cnos.segments(cnos.WantEmpties::DoWantEmpties)) {
        QColor col;
        switch (segment.type) {
        case segment.Type::Empty:
            col = Qt::black;
            break;
        case segment.Type::Chosen:
            col = Qt::green;
            break;
        case segment.Type::Conflict:
            col = Qt::gray;
            break;
        }

        segments_.push_back({segment.start, segment.length, col});
    }

    model_.endResetModel();

}

} // namespace channel
