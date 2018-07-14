#ifndef BADAUDIOCHANNEL_H
#define BADAUDIOCHANNEL_H

#include "channelbase.h"

#include <QObject>

namespace channel {

class BadAudioChannel : public ChannelBase
{
    Q_OBJECT

    Q_PROPERTY(channel::ChannelType::Enum channelType READ channelType CONSTANT)

public:
    explicit BadAudioChannel(QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

signals:

public slots:
};

} // namespace channel

#endif // BADAUDIOCHANNEL_H
