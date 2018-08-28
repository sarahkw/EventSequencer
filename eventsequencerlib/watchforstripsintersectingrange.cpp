#include "watchforstripsintersectingrange.h"

#include "collides.h"

QObject *WatchForStripsIntersectingRange::channel() const
{
    return channel_;
}

void WatchForStripsIntersectingRange::setChannel(QObject *channel)
{
    channel::ChannelBase* castChannel = qobject_cast<channel::ChannelBase*>(channel);
    if (channel_ != castChannel) {
        if (channel_ != nullptr) {
            channel_->disconnect(this);
        }
        channel_ = castChannel;
        if (channel_ != nullptr) {
            QObject::connect(channel_, &QObject::destroyed,
                             this, &WatchForStripsIntersectingRange::destroyingChannel);
            QObject::connect(channel_, &channel::ChannelBase::stripsChanged,
                             this, &WatchForStripsIntersectingRange::updateStrips);
        }
        emit channelChanged();
        updateStrips();
    }
}

void WatchForStripsIntersectingRange::destroyingChannel()
{
    channel_ = nullptr;
    updateStrips();
}

int WatchForStripsIntersectingRange::startFrame() const
{
    return startFrame_;
}

void WatchForStripsIntersectingRange::setStartFrame(int startFrame)
{
    if (startFrame_ != startFrame) {
        startFrame_ = startFrame;
        emit startFrameChanged();
        updateStrips();
    }
}

int WatchForStripsIntersectingRange::length() const
{
    return length_;
}

void WatchForStripsIntersectingRange::setLength(int length)
{
    if (length_ != length) {
        length_ = length;
        emit lengthChanged();
        updateStrips();
    }
}

QVariantList WatchForStripsIntersectingRange::strips() const
{
    return strips_;
}

bool WatchForStripsIntersectingRange::isStripInView(Strip *s) const
{
    for (const QVariant& v : strips_) {
        Strip* foo = v.value<Strip*>();
        if (foo != nullptr) {
            if (foo == s) {
                return true;
            }
        }
    }
    return false;
}

void WatchForStripsIntersectingRange::updateStrips()
{
    // XXX This sucks but I have to ship.
    strips_.clear();

    if (channel_ != nullptr) {
        for (Strip* s : channel_->multiChannelStrips()) {
            if (Collides::startAndLength(s->startFrame(), s->length(),
                                         this->startFrame_, this->length_)) {
                strips_.append(QVariant::fromValue(s));
            }
        }
    }

    emit stripsChanged();
}

WatchForStripsIntersectingRange::WatchForStripsIntersectingRange(QObject *parent) : QObject(parent)
{

}
