#ifndef BADJSCHANNEL_H
#define BADJSCHANNEL_H

#include "channeltype.h"
#include "channelbase.h"

#include <QObject>

namespace channel {

class BadJsChannel : public ChannelBase
{
    Q_OBJECT

    Q_PROPERTY(channel::ChannelType::Enum channelType READ channelType CONSTANT)

public:
    explicit BadJsChannel(QObject *parent = nullptr);

    void toPb(pb::ChannelData& pb) const override;
    void fromPb(const pb::ChannelData& pb) override;

    ChannelType::Enum channelType() const;

signals:

public slots:
};

} // namespace channel

#endif // BADJSCHANNEL_H
