#include "badaudiochannel.h"

#include <eventsequencer.pb.h>

namespace channel {

BadAudioChannel::BadAudioChannel(QObject *parent) : ChannelBase(parent)
{

}

void BadAudioChannel::toPb(pb::ChannelData &pb) const
{
    pb.mutable_badaudio();
}

void BadAudioChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_badaudio());
}

ChannelType::Enum BadAudioChannel::channelType() const
{
    return ChannelType::Audio;
}

} // namespace channel
