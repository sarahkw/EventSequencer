#ifndef SPANCHANNEL_H
#define SPANCHANNEL_H

#include "channelbase.h"
#include "waitfor.h"

#include <QObject>
#include <memory>

namespace channel {

class SpanChannel : public ChannelBase
{
    Q_OBJECT

    ChannelIndex channelIndex_;
    Document& d_;

    std::vector<std::unique_ptr<WaitFor>> waiters_;

    int count_ = 0;
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

    ChannelType::Enum defaultChannelType_ = ChannelType::UNSET;
    Q_PROPERTY(ChannelType::Enum defaultChannelType READ defaultChannelType WRITE setDefaultChannelType NOTIFY defaultChannelTypeChanged)

    void waiterResultChanged(QObject* channel);
    void waiterResultAboutToUnset(QObject* channel);

public:
    explicit SpanChannel(ChannelIndex channelIndex, Document& d, QObject *parent = nullptr);
    ~SpanChannel() override;

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

    int count() const;
    void setCount(int count);

    ChannelType::Enum defaultChannelType() const;
    void setDefaultChannelType(const ChannelType::Enum &defaultChannelType);

    std::vector<Strip*> strips() override;
    std::vector<Strip*> multiChannelStrips() override;

    // For CollateChannel
    const std::vector<std::unique_ptr<WaitFor>>& waitersForChildChannels() const;

signals:

    void countChanged();
    void defaultChannelTypeChanged();

    void setSpan(ChannelIndex channelIndex, unsigned span);

private slots:

    void channelStripSetChanged(ChannelIndex channelIndex) override;
    void channelStripLocationChanged(ChannelIndex channelIndex, Strip* whichStrip) override;

public slots:
};

} // namespace channel

#endif // SPANCHANNEL_H
