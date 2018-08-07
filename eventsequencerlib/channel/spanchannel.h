#ifndef SPANCHANNEL_H
#define SPANCHANNEL_H

#include "channelbase.h"

#include <QObject>

namespace channel {

class SpanChannel : public ChannelBase
{
    Q_OBJECT

    ChannelIndex channelIndex_;

    int count_ = 0;
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit SpanChannel(ChannelIndex channelIndex, Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

    int count() const;
    void setCount(int count);

signals:

    void countChanged();
    void setSpan(ChannelIndex channelIndex, unsigned span);

public slots:
};

} // namespace channel

#endif // SPANCHANNEL_H
