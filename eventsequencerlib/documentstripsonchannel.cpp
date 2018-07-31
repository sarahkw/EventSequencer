#include "documentstripsonchannel.h"

#include "strip.h"

DocumentStripsOnChannel::DocumentStripsOnChannel(QObject *parent) : QObject(parent)
{

}

const std::set<Strip *>* DocumentStripsOnChannel::stripsForChannel(int channel) const
{
    auto iter = data_.find(channel);
    if (iter == data_.end()) {
        return nullptr;
    }
    return &iter->second;
}

void DocumentStripsOnChannel::stripAfterPlaced(Strip *strip)
{
    data_[strip->channel()].insert(strip);
    emit channelStripSetChanged(strip->channel());
}

void DocumentStripsOnChannel::stripBeforeDelete(Strip *strip)
{
    data_[strip->channel()].erase(strip);
    emit channelStripSetChanged(strip->channel());
}

void DocumentStripsOnChannel::stripMoved(Strip *strip, int previousChannel, int previousStartFrame, int previousLength)
{
    Q_UNUSED(previousStartFrame);
    Q_UNUSED(previousLength);

    if (strip->channel() == previousChannel) {
        emit channelStripLocationChanged(strip->channel(), strip);
    } else {
        data_[previousChannel].erase(strip);
        emit channelStripSetChanged(previousChannel);
        data_[strip->channel()].insert(strip);
        emit channelStripSetChanged(strip->channel());
    }
}
