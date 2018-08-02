#ifndef CHANNELBASE_H
#define CHANNELBASE_H

#include <QObject>
#include <set>

#include "documentstripsonchannel.h"
#include "channeltype.h"
#include "strip.h"

namespace pb {
class ChannelData;
}
class Document;

namespace channel {

class ChannelBase : public QObject
{
    Q_OBJECT

    int channelIndex_ = 0;
    Document& d_;

    Q_PROPERTY(channel::ChannelType::Enum channelType READ channelType CONSTANT)

public:
    explicit ChannelBase(int channelIndex, Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const = 0;
    virtual void fromPb(const pb::ChannelData& pb) = 0;

    virtual ChannelType::Enum channelType() const = 0;

    virtual const DocumentStripsOnChannel::StripSet* stripSet();

signals:

    void stripSetChanged();

private slots:

    virtual void channelStripSetChanged(int channel);
    virtual void channelStripLocationChanged(int channel, Strip* whichStrip);

public slots:
};

} // namespace channel

#endif // CHANNELBASE_H
