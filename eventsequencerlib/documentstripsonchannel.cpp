#include "documentstripsonchannel.h"

#include "strip.h"

DocumentStripsOnChannel::DocumentStripsOnChannel(QObject* parent)
    : QObject(parent)
{
}

const DocumentStripsOnChannel::StripSet*
DocumentStripsOnChannel::stripsForChannel(ChannelIndex channelIndex) const
{
    auto iter = data_.find(channelIndex);
    if (iter == data_.end()) {
        return nullptr;
    }
    return &iter->second;
}

std::pair<DocumentStripsOnChannel::ChannelStripsMap::const_iterator,
          DocumentStripsOnChannel::ChannelStripsMap::const_iterator>
DocumentStripsOnChannel::stripsBetweenChannels(ChannelIndex from,
                                               ChannelIndex toExclusive) const
{
    return std::make_pair(data_.lower_bound(from),
                          data_.lower_bound(toExclusive));
}

std::pair<DocumentStripsOnChannel::ChannelStripsMap::const_iterator,
          DocumentStripsOnChannel::ChannelStripsMap::const_iterator>
DocumentStripsOnChannel::stripsGreaterChannel(
    ChannelIndex fromExclusive) const
{
    return std::make_pair(data_.upper_bound(fromExclusive),
                          data_.end());
}

std::pair<DocumentStripsOnChannel::ChannelStripsMap::const_iterator,
          DocumentStripsOnChannel::ChannelStripsMap::const_iterator>
DocumentStripsOnChannel::stripsChildOfChannel(int channelIndexFirst) const
{
    return std::make_pair(data_.upper_bound(ChannelIndex::make1(channelIndexFirst)),
                          data_.lower_bound(ChannelIndex::make1(channelIndexFirst + 1)));
}

void DocumentStripsOnChannel::stripAfterPlaced(Strip *strip)
{
    data_[strip->channelIndex()].insert({strip->startFrame(), strip});
    emit channelStripSetChanged(strip->channelIndex());
}

void DocumentStripsOnChannel::stripBeforeDelete(Strip *strip)
{
    data_[strip->channelIndex()].erase({strip->startFrame(), strip});
    emit channelStripSetChanged(strip->channelIndex());
}

void DocumentStripsOnChannel::stripMoved(Strip* strip,
                                         ChannelIndex previousChannelIndex,
                                         int previousStartFrame,
                                         int previousLength)
{
    Q_UNUSED(previousLength);

    // XXX It kinda sucks that each time a strip moves we have to delete and
    //     readd. That's the price of keeping a set sorted by startFrame.
    //     Maybe consider not doing so, and just sorting it downstream if needed.

    // Delete from old
    {
        auto& stripset = data_[previousChannelIndex];
        auto iterFound = stripset.end();
        for (auto iter =
                 stripset.lower_bound({previousStartFrame, nullptr});
             iter != stripset.end() &&
             iter->startFrame == previousStartFrame;
             ++iter) {
            // We should only iterate once unless there's an overlap.
            if (iter->strip == strip) {
                iterFound = iter;
                break;
            }
        }

        if (iterFound != stripset.end()) {
            stripset.erase(iterFound);
        } else {
            Q_ASSERT(iterFound != stripset.end());
        }
    }

    // Add to new
    data_[strip->channelIndex()].insert({strip->startFrame(), strip});

    // Emit signals
    if (strip->channelIndex() == previousChannelIndex) {
        emit channelStripLocationChanged(strip->channelIndex(), strip);
    } else {
        emit channelStripSetChanged(previousChannelIndex);
        emit channelStripSetChanged(strip->channelIndex());
    }
}

bool DocumentStripsOnChannel::Compare::operator()(const StripHolder& a, const StripHolder& b) const
{
    if (a.startFrame < b.startFrame) {
        return true;
    } else if (a.startFrame > b.startFrame) {
        return false;
    }
    return a.strip < b.strip;
}
