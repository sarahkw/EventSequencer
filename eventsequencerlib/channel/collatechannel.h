#ifndef COLLATECHANNEL_H
#define COLLATECHANNEL_H

#include "channelbase.h"

#include <QObject>
#include <QAbstractListModel>
#include <QColor>

class Document;
class Strip;

namespace channel {

class CollateChannel;

class CollateChannelModel : public QAbstractListModel {
    Q_OBJECT
    friend class CollateChannel;
    CollateChannel& cc_;
    CollateChannelModel(CollateChannel& cc) : cc_(cc) { }

public:
    enum CustomRoles {
        SegmentStartRole = Qt::UserRole + 1,
        SegmentLengthRole,
        SegmentColorRole,
        SegmentTypeRole
    };
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;

    Q_INVOKABLE QAbstractItemModel* makeFilterModel(int start, int length);
};

class CollateChannel : public ChannelBase
{
    Q_OBJECT
public:
    enum class SegmentType {
        Empty,
        Chosen,
        Conflict
    };
    Q_ENUM(SegmentType)
private:

    friend class CollateChannelModel;
    CollateChannelModel model_;
    Q_PROPERTY(QAbstractListModel* model READ model CONSTANT)

    Document& d_;

    bool refreshPending_ = false;

    int channelFrom_ = 0;
    int channelTo_ = 0;

    Q_PROPERTY(int channelFrom READ channelFrom WRITE setChannelFrom NOTIFY channelFromChanged)
    Q_PROPERTY(int channelTo READ channelTo WRITE setChannelTo NOTIFY channelToChanged)

    struct Segment {
        int segmentStart;
        int segmentLength;
        SegmentType segmentType;
    };
    std::vector<Segment> segments_;
    DocumentStripsOnChannel::StripSet stripSet_;

public:
    explicit CollateChannel(int channelIndex, Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

    QAbstractListModel* model();

    int channelFrom() const;
    void setChannelFrom(int channelFrom);

    int channelTo() const;
    void setChannelTo(int channelTo);

    bool event(QEvent *event) override;

    const DocumentStripsOnChannel::StripSet* stripSet() override;

signals:

    void channelFromChanged();
    void channelToChanged();

private slots:

    void triggerRefresh();

    void channelStripSetChanged(int channel) override;
    void channelStripLocationChanged(int channel, Strip* whichStrip) override;

private:

    void channelAffected(int channel);
    void recalculate();

public slots:
};

} // namespace channel

#endif // COLLATECHANNEL_H
