#ifndef SPANCHANNEL_H
#define SPANCHANNEL_H

#include "channelbase.h"

#include <QObject>

namespace channel {

class SpanChannel : public ChannelBase
{
    Q_OBJECT

    ChannelIndex channelIndex_;
    Document& d_;

    int count_ = 0;
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit SpanChannel(ChannelIndex channelIndex, Document& d, QObject *parent = nullptr);
    ~SpanChannel() override;

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

    int count() const;
    void setCount(int count);

    std::vector<Strip*> strips() override;
    std::vector<Strip*> multiChannelStrips() override;

signals:

    void countChanged();
    void setSpan(ChannelIndex channelIndex, unsigned span);

private slots:

    void channelStripSetChanged(ChannelIndex channelIndex) override;
    void channelStripLocationChanged(ChannelIndex channelIndex, Strip* whichStrip) override;

public slots:
};

} // namespace channel

#endif // SPANCHANNEL_H
