#include "audiochannel.h"

#include <eventsequencer.pb.h>

namespace channel {

AudioChannel::AudioChannel(ChannelIndex channelIndex, Document& d, QObject* parent)
    : ChannelBase(channelIndex, d, parent)
{

}

void AudioChannel::toPb(pb::ChannelData &pb) const
{
    pb.mutable_audio();
}

void AudioChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_audio());
}

ChannelType::Enum AudioChannel::channelType() const
{
    return ChannelType::Audio;
}

} // namespace channel
