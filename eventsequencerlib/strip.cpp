#include "strip.h"

#include "stripext/badjsstripext.h"
#include "stripext/audiostripext.h"

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

stripext::BadJsStripExt *Strip::badJs() const
{
    return badJs_;
}

void Strip::setBadJs(stripext::BadJsStripExt *badJs)
{
    if (badJs_ != badJs) {
        badJs_ = badJs;
        emit badJsChanged();
    }
}

stripext::BadJsStripExt* Strip::mutableBadJs()
{
    if (badJs() == nullptr) {
        setBadJs(new stripext::BadJsStripExt(this));
    }
    return badJs();
}

stripext::AudioStripExt *Strip::audio() const
{
    return audio_;
}

void Strip::setAudio(stripext::AudioStripExt *audio)
{
    if (audio_ != audio) {
        audio_ = audio;
        emit audioChanged();
    }
}

stripext::AudioStripExt *Strip::mutableAudio()
{
    if (audio() == nullptr) {
        setAudio(new stripext::AudioStripExt(this));
    }
    return audio();
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
    if (audio() != nullptr) {
        audio()->toPb(*pb.mutable_audio());
    }
}

void Strip::fromPb(const pb::Strip &pb)
{
    // Calling setters to fire changed signals.
    setChannel(pb.channel());
    setStartFrame(pb.startframe());
    setLength(pb.length());
    if (pb.has_badjs()) {
        auto tmp = new stripext::BadJsStripExt(this);
        tmp->fromPb(pb.badjs());
        setBadJs(tmp);
    }
    if (pb.has_audio()) {
        auto tmp = new stripext::AudioStripExt(this);
        tmp->fromPb(pb.audio());
        setAudio(tmp);
    }
}
