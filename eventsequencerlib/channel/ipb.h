#ifndef CHANNEL_IPB_H
#define CHANNEL_IPB_H

namespace pb {
class ChannelData;
}

namespace channel {

class IPb
{
public:
    virtual void toPb(pb::ChannelData& pb) const = 0;
    virtual void fromPb(const pb::ChannelData& pb) = 0;
};

} // namespace channel

#endif // CHANNEL_IPB_H
