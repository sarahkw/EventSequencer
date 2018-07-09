#ifndef BADCLOCKCHANNEL_H
#define BADCLOCKCHANNEL_H

#include "channeltype.h"
#include "iclockrole.h"
#include "channelbase.h"

#include <QObject>

namespace channel {

class BadClockChannel : public ChannelBase, public IClockRole
{
    Q_OBJECT

    Q_PROPERTY(channel::ChannelType::Enum channelType READ channelType CONSTANT)

public:
    explicit BadClockChannel(QObject *parent = nullptr);

    void toPb(pb::ChannelData& pb) const override;
    void fromPb(const pb::ChannelData& pb) override;

    ChannelType::Enum channelType() const;

signals:

public slots:
};

} // namespace channel

#endif // BADCLOCKCHANNEL_H
