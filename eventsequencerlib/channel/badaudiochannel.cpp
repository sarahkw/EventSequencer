#include "badaudiochannel.h"

#include <eventsequencer.pb.h>

namespace channel {

BadAudioChannel::BadAudioChannel(QObject *parent) : ChannelBase(parent)
{

}

ChannelType::Enum BadAudioChannel::channelType() const
{
    return ChannelType::BadAudio;
}

} // namespace channel
