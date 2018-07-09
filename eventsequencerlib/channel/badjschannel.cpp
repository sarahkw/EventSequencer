#include "badjschannel.h"

namespace channel {

BadJsChannel::BadJsChannel(QObject *parent) : ChannelBase(parent)
{

}

ChannelType::Enum BadJsChannel::channelType() const
{
    return ChannelType::BadJs;
}

} // namespace channel
