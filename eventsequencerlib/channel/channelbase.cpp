#include "channelbase.h"

#include "document.h"

namespace channel {

ChannelBase::ChannelBase(ChannelIndex channelIndex, Document& d, QObject* parent)
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

void ChannelBase::channelStripSetChanged(ChannelIndex channelIndex)
{
    if (channelIndex == channelIndex_) {
        emit stripSetChanged();
    }
}

void ChannelBase::channelStripLocationChanged(ChannelIndex channelIndex, Strip *whichStrip)
{
    if (channelIndex == channelIndex_) {
        // Used to have a stripLocationChanged signal, but for
        // subclasses that remake the strips list from scratch, it's
        // hard to do. The benefits aren't worth the cost.
        emit stripSetChanged();
    }
}

} // namespace channel
