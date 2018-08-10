#include "channeltype.h"

namespace channel {

ChannelType::ChannelType(QObject *parent) : QObject(parent)
{

}

int ChannelType::toInt(ChannelType::Enum value)
{
    return static_cast<int>(value);
}

ChannelType::Enum ChannelType::fromInt(int value)
{
    if (value >= 0 && value <= ChannelType::MAX) {
        return static_cast<Enum>(value);
    } else {
        qWarning("Cannot convert invalid value to enum: %d", value);
        return Enum::UNSET;
    }
}

} // namespace channel
