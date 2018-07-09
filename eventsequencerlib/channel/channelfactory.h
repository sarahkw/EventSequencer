#ifndef CHANNELFACTORY_H
#define CHANNELFACTORY_H

#include "channeltype.h"

class QObject;

namespace pb {
class ChannelData;
}

namespace channel {

class ChannelBase;

class ChannelFactory
{
    ChannelFactory() = delete;
public:

    static ChannelBase* Create(const pb::ChannelData& pb, QObject* parent=nullptr);
    static ChannelBase* Create(ChannelType::Enum type, QObject* parent=nullptr);

};

} // namespace channel

#endif // CHANNELFACTORY_H
