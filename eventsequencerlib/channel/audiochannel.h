#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

#include "channelbase.h"

#include <QObject>

namespace channel {

class AudioChannel : public ChannelBase
{
    Q_OBJECT

public:
    explicit AudioChannel(ChannelIndex channelIndex, Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

signals:

public slots:
};

} // namespace channel

#endif // AUDIOCHANNEL_H
