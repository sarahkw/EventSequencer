#include "strip.h"

#include "document.h"

#include "stripext/badjsstripext.h"
#include "stripext/badaudiostripext.h"
#include "stripext/textstripext.h"
#include "stripext/labelstripext.h"

#include "eventsequencer.pb.h"

int Strip::channel() const
{
    return channel_;
}

void Strip::setChannel(int channel)
{
    if (channel_ != channel) {
        auto oldChannel = channel_;
        channel_ = channel;
        emit channelChanged();
        if (hasPlaced_) {
            emit d_.stripMoved(this, oldChannel, startFrame_, length_);
        }
    }
}

int Strip::startFrame() const
{
    return startFrame_;
}

void Strip::setStartFrame(int startFrame)
{
    if (startFrame_ != startFrame) {
        auto oldStartFrame = startFrame_;
        startFrame_ = startFrame;
        emit startFrameChanged();
        if (hasPlaced_) {
            emit d_.stripMoved(this, channel_, oldStartFrame, length_);
        }
    }
}

int Strip::length() const
{
    return length_;
}

void Strip::setLength(int length)
{
    if (length < 1) {
        length = 1;
    }
    if (length_ != length) {
        auto oldLength = length_;
        length_ = length;
        emit lengthChanged();
        if (hasPlaced_) {
            emit d_.stripMoved(this, channel_, startFrame_, oldLength);
        }
    }
}


Resource* Strip::resource()
{
    return &resource_;
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

stripext::BadAudioStripExt *Strip::badAudio() const
{
    return badAudio_;
}

void Strip::setBadAudio(stripext::BadAudioStripExt *badAudio)
{
    if (badAudio_ != badAudio) {
        badAudio_ = badAudio;
        emit badAudioChanged();
    }
}

stripext::BadAudioStripExt *Strip::mutableBadAudio()
{
    if (badAudio() == nullptr) {
        setBadAudio(new stripext::BadAudioStripExt(this));
    }
    return badAudio();
}

stripext::TextStripExt *Strip::text() const
{
    return text_;
}

void Strip::setText(stripext::TextStripExt *text)
{
    if (text_ != text) {
        text_ = text;
        emit textChanged();
    }
}

stripext::TextStripExt *Strip::mutableText()
{
    if (text() == nullptr) {
        setText(new stripext::TextStripExt(this));
    }
    return text();
}

stripext::LabelStripExt *Strip::label() const
{
    return label_;
}

void Strip::setLabel(stripext::LabelStripExt *label)
{
    if (label_ != label) {
        label_ = label;
        emit labelChanged();
    }
}

stripext::LabelStripExt *Strip::mutableLabel()
{
    if (label() == nullptr) {
        setLabel(new stripext::LabelStripExt(this));
    }
    return label();
}

Strip::Strip(Document& d, QObject *parent) : QObject(parent), d_(d)
{
    
}

void Strip::markAsPlaced()
{
    hasPlaced_ = true;
    emit d_.stripAfterPlaced(this);
}

void Strip::toPb(pb::Strip &pb) const
{
    pb.set_channel(channel_);
    pb.set_startframe(startFrame_);
    pb.set_length(length_);
    resource_.toPb(*pb.mutable_resource());
    if (badJs() != nullptr) {
        badJs()->toPb(*pb.mutable_badjs());
    }
    if (badAudio() != nullptr) {
        badAudio()->toPb(*pb.mutable_badaudio());
    }
    if (text() != nullptr) {
        text()->toPb(*pb.mutable_text());
    }
    if (label() != nullptr) {
        label()->toPb(*pb.mutable_label());
    }
}

void Strip::fromPb(const pb::Strip &pb)
{
    // Calling setters to fire changed signals.
    setChannel(pb.channel());
    setStartFrame(pb.startframe());
    setLength(pb.length());
    markAsPlaced();
    resource_.fromPb(pb.resource());

    if (pb.has_badjs()) {
        auto tmp = new stripext::BadJsStripExt(this);
        tmp->fromPb(pb.badjs());
        setBadJs(tmp);
    }
    if (pb.has_badaudio()) {
        auto tmp = new stripext::BadAudioStripExt(this);
        tmp->fromPb(pb.badaudio());
        setBadAudio(tmp);
    }
    if (pb.has_text()) {
        auto tmp = new stripext::TextStripExt(this);
        tmp->fromPb(pb.text());
        setText(tmp);
    }
    if (pb.has_label()) {
        auto tmp = new stripext::LabelStripExt(this);
        tmp->fromPb(pb.label());
        setLabel(tmp);
    }
}
