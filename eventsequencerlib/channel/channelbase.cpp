#include "channelbase.h"

#include "document.h"

namespace channel {

ChannelBase::ChannelBase(int channelIndex, Document& d, QObject* parent)
    : QObject(parent), channelIndex_(channelIndex), d_(d)
{
    QObject::connect(&d_.stripsOnChannel(), &DocumentStripsOnChannel::channelStripSetChanged,
                     this, &ChannelBase::channelStripSetChanged);
    QObject::connect(&d_.stripsOnChannel(), &DocumentStripsOnChannel::channelStripLocationChanged,
                     this, &ChannelBase::channelStripLocationChanged);
}

const DocumentStripsOnChannel::StripSet *ChannelBase::stripSet()
{
    return d_.stripsOnChannel().stripsForChannel(channelIndex_);
}

void ChannelBase::channelStripSetChanged(int channel)
{
    if (channel == channelIndex_) {
        emit stripSetChanged();
    }
}

void ChannelBase::channelStripLocationChanged(int channel, Strip *whichStrip)
{
    if (channel == channelIndex_) {
        emit stripLocationChanged(whichStrip);
    }
}

} // namespace channel
