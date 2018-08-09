#ifndef CHANNELBASE_H
#define CHANNELBASE_H

#include <QObject>

#include "channeltype.h"
#include "strip.h"
#include "channelindex.h"

namespace pb {
class ChannelData;
}
class Document;

namespace channel {

class ChannelBase : public QObject
{
    Q_OBJECT

    ChannelIndex channelIndex_;
    Document& d_;

    Q_PROPERTY(channel::ChannelType::Enum channelType READ channelType CONSTANT)

public:
    explicit ChannelBase(ChannelIndex channelIndex, Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const = 0;
    virtual void fromPb(const pb::ChannelData& pb) = 0;

    virtual ChannelType::Enum channelType() const = 0;

    virtual std::vector<Strip*> strips();
    virtual std::vector<Strip*> multiChannelStrips();

    // Creates a strip on this channel. Returns a pointer to the Strip if it's
    // successful. Otherwise, returns nullptr.
    Q_INVOKABLE virtual Strip* createStrip(int startFrame, int length);

signals:

    void stripsChanged();

private slots:

    virtual void channelStripSetChanged(ChannelIndex channelIndex);
    virtual void channelStripLocationChanged(ChannelIndex channelIndex, Strip* whichStrip);

public slots:
};

} // namespace channel

#endif // CHANNELBASE_H
