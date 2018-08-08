#ifndef PLAYLISTCHANNEL_H
#define PLAYLISTCHANNEL_H

#include "channelbase.h"

#include <QObject>

namespace channel {

class PlaylistChannel : public ChannelBase
{
    Q_OBJECT

public:
    explicit PlaylistChannel(ChannelIndex channelIndex, Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

signals:

public slots:
};

} // namespace channel

#endif // PLAYLISTCHANNEL_H
