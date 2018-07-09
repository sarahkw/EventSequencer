#ifndef CHANNELFACTORY_H
#define CHANNELFACTORY_H

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

};

} // namespace channel

#endif // CHANNELFACTORY_H
