#include "strip.h"

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

Strip::Strip(QObject *parent) : QObject(parent)
{

}
