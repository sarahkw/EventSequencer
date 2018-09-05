#include "channelbase.h"

#include "collides.h"
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

std::vector<Strip*> ChannelBase::strips()
{
    std::vector<Strip*> ret;
    auto* sset = d_.stripsOnChannel().stripsForChannel(channelIndex_);
    if (sset != nullptr) {
        for (auto& sh : *sset) {
            ret.push_back(sh.strip);
        }
    }
    return ret;
}

std::vector<Strip *> ChannelBase::multiChannelStrips()
{
    return strips();
}

bool ChannelBase::stripWillCollide(int startFrame, int length)
{
    // I don't think we'll ever have so many strips that O(n) will be a problem.
    for (Strip* s : strips()) {
        if (Collides::startAndLength(startFrame, length,
                                     s->startFrame(),
                                     s->length())) {
            return true;
        }
    }
    return false;
}

Strip *ChannelBase::createStrip(int startFrame, int length, CreateStripFlags flags)
{
    if (stripWillCollide(startFrame, length)) {
        return nullptr;
    }

    if (!Strip::isValidLength(length)) {
        return nullptr;
    }

    Strip* ret = d_.createStrip();
    ret->setChannelIndex(channelIndex_);
    ret->setStartFrame(startFrame);
    ret->setLength(length);
    ret->markAsPlaced();
    return ret;
}

void ChannelBase::channelStripSetChanged(ChannelIndex channelIndex)
{
    if (channelIndex == channelIndex_) {
        emit stripsChanged();
    }
}

void ChannelBase::channelStripLocationChanged(ChannelIndex channelIndex, Strip *whichStrip)
{
    if (channelIndex == channelIndex_) {
        // Used to have a stripLocationChanged signal, but for
        // subclasses that remake the strips list from scratch, it's
        // hard to do. The benefits aren't worth the cost.
        emit stripsChanged();
    }
}

} // namespace channel
