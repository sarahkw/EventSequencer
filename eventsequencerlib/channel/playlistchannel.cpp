#include "playlistchannel.h"

#include <eventsequencer.pb.h>

namespace channel {

PlaylistChannel::PlaylistChannel(QObject *parent) : ChannelBase(parent)
{

}

void PlaylistChannel::toPb(pb::ChannelData &pb) const
{
    pb.mutable_playlist();
}

void PlaylistChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_playlist());
}

ChannelType::Enum PlaylistChannel::channelType() const
{
    return ChannelType::Playlist;
}

} // namespace channel
