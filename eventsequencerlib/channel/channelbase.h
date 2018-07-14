#ifndef CHANNELBASE_H
#define CHANNELBASE_H

#include <QObject>

#include "channeltype.h"
#include "ipb.h"

namespace pb {
class ChannelData;
}

namespace channel {

class ChannelBase : public QObject, public virtual IPb
{
    Q_OBJECT

    Q_PROPERTY(channel::ChannelType::Enum channelType READ channelType CONSTANT)

public:
    explicit ChannelBase(QObject *parent = nullptr);

    virtual ChannelType::Enum channelType() const = 0;

signals:

public slots:
};

} // namespace channel

#endif // CHANNELBASE_H
