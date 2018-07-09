#ifndef CHANNELBASE_H
#define CHANNELBASE_H

#include <QObject>

namespace channel {

class ChannelBase : public QObject
{
    Q_OBJECT
public:
    explicit ChannelBase(QObject *parent = nullptr);

signals:

public slots:
};

} // namespace channel

#endif // CHANNELBASE_H
