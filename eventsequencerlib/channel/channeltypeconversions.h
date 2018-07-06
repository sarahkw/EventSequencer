#ifndef CHANNELTYPECONVERSIONS_H
#define CHANNELTYPECONVERSIONS_H

#include "channeltype.h"
#include "../eventsequencer.pb.h"

namespace channel {

class ChannelTypeConversions
{
    ChannelTypeConversions() = delete;
public:

    static ::pb::ChannelData::ChannelCase ToPb(ChannelType::Enum ct);
    static ChannelType::Enum ToCc(::pb::ChannelData::ChannelCase cc);

};

} // namespace channel

#endif // CHANNELTYPECONVERSIONS_H
