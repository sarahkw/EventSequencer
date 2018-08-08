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
        case SegmentTypeRole: return QVariant::fromValue(cc_.segments_[index.row()].segmentType);
        }
    }

    return QVariant();
}

QHash<int, QByteArray> CollateChannelModel::roleNames() const
{
    return {
        {SegmentStartRole, "segmentStart"},
        {SegmentLengthRole, "segmentLength"},
        {SegmentTypeRole, "segmentType"}
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

ChannelIndex CollateChannel::channel() const
{
    return channel_;
}

void CollateChannel::setChannel(const ChannelIndex &channel)
{
    if (channel_ != channel) {
        channel_ = channel;
        emit channelChanged();
        waitForChannel_.reset(d_.waitForChannelIndex(channel));
        QObject::connect(waitForChannel_.get(), &WaitFor::resultChanged,
                         this, &CollateChannel::channelWaitForResultChanged);
        channelWaitForResultChanged();
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

std::vector<Strip*> CollateChannel::strips()
{
    // TODO: Recursion guard

    // Need to forcefully call recalculate() each time this is read. Otherwise,
    // we could be returning a set with a strip that was deleted.
    if (refreshPending_) {
        refreshPending_ = false;
        recalculate();
    }

    return stripSet_;
}

void CollateChannel::channelWaitForResultChanged()
{
    if (sourceChannel_ != nullptr) {
        sourceChannel_->disconnect(this);
    }

    sourceChannel_ = qobject_cast<channel::ChannelBase*>(waitForChannel_->result());
    if (sourceChannel_ != nullptr) {
        QObject::connect(sourceChannel_, &ChannelBase::stripsChanged,
                         this, &CollateChannel::channelWaitForStripsChanged);
    }
    channelWaitForStripsChanged();
}

void CollateChannel::channelWaitForStripsChanged()
{
    triggerRefresh();
    emit stripsChanged();
}

void CollateChannel::triggerRefresh()
{
    if (!refreshPending_) {
        refreshPending_ = true;
        QCoreApplication::postEvent(this, new CollateChannelRefreshEvent);
    }
}

void CollateChannel::channelStripSetChanged(ChannelIndex channelIndex)
{
    // Disable base impl
}

void CollateChannel::channelStripLocationChanged(ChannelIndex channelIndex, Strip *whichStrip)
{
    // Disable base impl
}

CollateChannel::CollateChannel(ChannelIndex channelIndex, Document& d, QObject *parent)
    : ChannelBase(channelIndex, d, parent), model_(*this), d_(d)
{   
    QObject::connect(&d, &Document::startFrameChanged, this, &CollateChannel::triggerRefresh);
    QObject::connect(&d, &Document::endFrameChanged, this, &CollateChannel::triggerRefresh);

    triggerRefresh();
}

void CollateChannel::toPb(pb::ChannelData &pb) const
{
    auto mut = pb.mutable_collate();
    channel().toPb(*mut->mutable_channel());
}

void CollateChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_collate());
    setChannel(ChannelIndex::makeFromPb(pb.collate().channel()));
}

ChannelType::Enum CollateChannel::channelType() const
{
    return ChannelType::Collate;
}

QAbstractListModel *CollateChannel::model()
{
    return &model_;
}

void CollateChannel::recalculate()
{
    using CnosType = CollateNonOverlappingSegments<Strip*>;
    CnosType cnos(CnosType::BoundaryMode::HasBounds,
                  d_.startFrame(),
                  d_.endFrame() - d_.startFrame());
    
    if (sourceChannel_ != nullptr) {
        std::vector<Strip*> strips = sourceChannel_->multiChannelStrips();
        // Relying on strips() to be sorted, by channel index first.
        for (auto iter = strips.rbegin();
             iter != strips.rend();
             ++iter) {
            Strip* s = *iter;
            cnos.mergeSegment(s->startFrame(), s->length(),
                              CnosType::ReplaceMode::No,
                              s);
        }
    }

    model_.beginResetModel();
    segments_.clear();
    stripSet_.clear();

    for (auto& segment : cnos.segments(CnosType::WantEmpties::DoWantEmpties)) {
        if (segment.type == CnosType::Segment::Type::Chosen) {
            stripSet_.push_back(segment.data);
        }
        
        SegmentType segmentType;
        switch (segment.type) {
        case CnosType::Segment::Type::Empty:
            segmentType = SegmentType::Empty;
            break;
        case CnosType::Segment::Type::Chosen:
            segmentType = SegmentType::Chosen;
            break;
        case CnosType::Segment::Type::Conflict:
            segmentType = SegmentType::Conflict;
            break;
        }

        segments_.push_back({segment.start, segment.length, segmentType});
    }

    model_.endResetModel();
}

} // namespace channel
