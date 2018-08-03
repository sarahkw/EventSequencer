#include "collatechannel.h"

#include <eventsequencer.pb.h>
#include <document.h>
#include <strip.h>

#include "collatechannelrefreshevent.h"
#include "collatenonoverlappingsegments.h"

#include <QDebug>
#include <QCoreApplication>
#include <QSortFilterProxyModel>

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

/**/ class CollateChannelFilterModel : public QSortFilterProxyModel {
/**/     int start_;
/**/     int length_;
/**/ public:
/**/     CollateChannelFilterModel(int start, int length);
/**/     bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
/**/ };
/**/ 
/**/ CollateChannelFilterModel::CollateChannelFilterModel(int start, int length) : start_(start), length_(length)
/**/ {
/**/ }
/**/ 
/**/ bool CollateChannelFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
/**/ {
/**/     QModelIndex index0 = sourceModel()->index(source_row, 0, source_parent);
/**/     int theStart = sourceModel()->data(index0, CollateChannelModel::CustomRoles::SegmentStartRole).toInt();
/**/     int theLength = sourceModel()->data(index0, CollateChannelModel::CustomRoles::SegmentLengthRole).toInt();
/**/     bool collides = (theStart < start_ ?
/**/                          (theStart + theLength > start_) :
/**/                          (start_ + length_ > theStart));
/**/     return collides;
/**/ }

QAbstractItemModel *CollateChannelModel::makeFilterModel(int start, int length)
{
    auto ret = new CollateChannelFilterModel(start, length);
    ret->setSourceModel(this);
    return ret;
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

const DocumentStripsOnChannel::StripSet *CollateChannel::stripSet()
{
    // Need to forcefully call recalculate() each time this is read. Otherwise,
    // we could be returning a set with a strip that was deleted.
    if (refreshPending_) {
        refreshPending_ = false;
        recalculate();
    }

    return &stripSet_;
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
    if (channel >= channelFrom() && channel < channelTo()) {
        triggerRefresh();
        emit stripSetChanged();
    }
}

void CollateChannel::recalculate()
{
    using CnosType = CollateNonOverlappingSegments<const Strip*>;
    CnosType cnos(CnosType::BoundaryMode::HasBounds,
                  d_.startFrame(),
                  d_.endFrame() - d_.startFrame());
    
    {
        for (int channel = channelTo() - 1; channel >= channelFrom(); --channel) {
            auto stripSet = d_.stripsOnChannel().stripsForChannel(channel);
            if (stripSet != nullptr) {
                for (auto& s : *stripSet) {
                    cnos.mergeSegment(s.strip->startFrame(),
                                      s.strip->length(),
                                      CnosType::ReplaceMode::No,
                                      s.strip);
                }
            }
        }
    }

    model_.beginResetModel();
    segments_.clear();
    stripSet_.clear();

    for (auto& segment : cnos.segments(CnosType::WantEmpties::DoWantEmpties)) {
        if (segment.type == CnosType::Segment::Type::Chosen) {
            stripSet_.insert({segment.data->startFrame(), segment.data});
        }
        
        QColor col;
        switch (segment.type) {
        case CnosType::Segment::Type::Empty:
            col = Qt::black;
            break;
        case CnosType::Segment::Type::Chosen:
            col = Qt::green;
            break;
        case CnosType::Segment::Type::Conflict:
            col = Qt::gray;
            break;
        }

        segments_.push_back({segment.start, segment.length, col});
    }

    model_.endResetModel();
}

} // namespace channel
