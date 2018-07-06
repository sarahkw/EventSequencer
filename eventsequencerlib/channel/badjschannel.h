#ifndef BADJSCHANNEL_H
#define BADJSCHANNEL_H

#include "channeltype.h"

#include <QObject>

namespace channel {

class BadJsChannel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(channel::ChannelType::Enum channelType READ channelType CONSTANT)

public:
    explicit BadJsChannel(QObject *parent = nullptr);

    ChannelType::Enum channelType() const;

signals:

public slots:
};

} // namespace channel

#endif // BADJSCHANNEL_H
