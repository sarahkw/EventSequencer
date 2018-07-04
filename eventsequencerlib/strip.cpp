#include "strip.h"

#include "stripext/badjs.h"

#include "eventsequencer.pb.h"

int Strip::channel() const
{
    return channel_;
}

void Strip::setChannel(int channel)
{
    if (channel_ != channel) {
        channel_ = channel;
        emit channelChanged();
    }
}

int Strip::startFrame() const
{
    return startFrame_;
}

void Strip::setStartFrame(int startFrame)
{
    if (startFrame_ != startFrame) {
        startFrame_ = startFrame;
        emit startFrameChanged();
    }
}

int Strip::length() const
{
    return length_;
}

void Strip::setLength(int length)
{
    if (length_ != length) {
        length_ = length;
        emit lengthChanged();
    }
}

stripext::BadJs *Strip::badJs() const
{
    return badJs_;
}

void Strip::setBadJs(stripext::BadJs *badJs)
{
    if (badJs_ != badJs) {
        badJs_ = badJs;
        emit badJsChanged();
    }
}

void Strip::initBadJs()
{
    setBadJs(new stripext::BadJs(this));
}

Strip::Strip(QObject *parent) : QObject(parent)
{

}

void Strip::toPb(pb::Strip &pb) const
{
    pb.set_channel(channel_);
    pb.set_startframe(startFrame_);
    pb.set_length(length_);
    if (badJs() != nullptr) {
        badJs()->toPb(*pb.mutable_badjs());
    }
}

void Strip::fromPb(const pb::Strip &pb)
{
    // Calling setters to fire changed signals.
    setChannel(pb.channel());
    setStartFrame(pb.startframe());
    setLength(pb.length());
    if (pb.has_badjs()) {
        auto tmp = new stripext::BadJs(this);
        tmp->fromPb(pb.badjs());
        setBadJs(tmp);
    }
}
