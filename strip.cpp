#include "strip.h"

int Strip::channel() const
{
    return channel_;
}

void Strip::setChannel(int channel)
{
    channel_ = channel;
}

int Strip::startFrame() const
{
    return startFrame_;
}

void Strip::setStartFrame(int startFrame)
{
    startFrame_ = startFrame;
}

int Strip::length() const
{
    return length_;
}

void Strip::setLength(int length)
{
    length_ = length;
}

Strip::Strip(QObject *parent) : QObject(parent)
{

}
