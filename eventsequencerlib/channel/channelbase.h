#ifndef CHANNELBASE_H
#define CHANNELBASE_H

#include <QObject>

namespace pb {
class ChannelData;
}

namespace channel {

class ChannelBase : public QObject
{
    Q_OBJECT
public:
    explicit ChannelBase(QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const = 0;

signals:

public slots:
};

} // namespace channel

#endif // CHANNELBASE_H
