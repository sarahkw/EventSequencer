#include "badjschannel.h"

namespace channel {

BadJsChannel::BadJsChannel(QObject *parent) : QObject(parent)
{

}

ChannelType::Enum BadJsChannel::channelType() const
{
    return ChannelType::BadJs;
}

} // namespace channel
