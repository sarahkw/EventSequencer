#ifndef COLLATECHANNEL_H
#define COLLATECHANNEL_H

#include "channelbase.h"
#include "waitfor.h"

#include <QObject>
#include <QAbstractListModel>

#include <memory>

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

    ChannelIndex channel_;
    Q_PROPERTY(ChannelIndex channel READ channel WRITE setChannel NOTIFY channelChanged)
    std::unique_ptr<WaitFor> waitForChannel_;
    channel::ChannelBase* sourceChannel_ = nullptr;

    struct Segment {
        int segmentStart;
        int segmentLength;
        SegmentType segmentType;
    };
    std::vector<Segment> segments_;
    std::vector<Strip*> stripSet_;

    void setupCurrentChannel();

public:
    explicit CollateChannel(ChannelIndex channelIndex, Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

    QAbstractListModel* model();

    ChannelIndex channel() const;
    void setChannel(const ChannelIndex &channel);

    bool event(QEvent *event) override;

    std::vector<Strip*> strips() override;

    virtual Strip* createStrip(int startFrame, int length) override;

signals:

    void channelChanged();

private slots:

    void channelWaitForResultChanged();
    void channelWaitForStripsChanged();

    void triggerRefresh();

    void channelStripSetChanged(ChannelIndex channelIndex) override;
    void channelStripLocationChanged(ChannelIndex channelIndex, Strip* whichStrip) override;

private:

    void recalculate();

public slots:
};

} // namespace channel

#endif // COLLATECHANNEL_H
