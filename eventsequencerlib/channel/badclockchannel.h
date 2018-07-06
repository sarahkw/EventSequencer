#ifndef BADCLOCKCHANNEL_H
#define BADCLOCKCHANNEL_H

#include "channeltype.h"

#include <QObject>

namespace channel {

class BadClockChannel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(channel::ChannelType::Enum channelType READ channelType CONSTANT)

public:
    explicit BadClockChannel(QObject *parent = nullptr);

    ChannelType::Enum channelType() const;

signals:

public slots:
};

} // namespace channel

#endif // BADCLOCKCHANNEL_H
