#ifndef CHANNELFACTORY_H
#define CHANNELFACTORY_H

#include "channeltype.h"

class QObject;

class Document;

namespace pb {
class ChannelData;
}

namespace channel {

class ChannelBase;

class ChannelFactory
{
    ChannelFactory() = delete;
public:

    static ChannelBase* Create(const pb::ChannelData& pb, Document& d, QObject* parent=nullptr);
    static ChannelBase* Create(ChannelType::Enum type, Document& d, QObject* parent=nullptr);

};

} // namespace channel

#endif // CHANNELFACTORY_H
