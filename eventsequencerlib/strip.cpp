#include "strip.h"

#include "document.h"

#include "stripext/badjsstripext.h"
#include "stripext/audiostripext.h"
#include "stripext/textstripext.h"
#include "stripext/labelstripext.h"
#include "stripext/playliststripext.h"

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

unsigned int Strip::mediaFrameSkipBegin() const
{
    return mediaFrameSkipBegin_;
}

void Strip::setMediaFrameSkipBegin(unsigned int mediaFrameSkipBegin)
{
    if (mediaFrameSkipBegin_ != mediaFrameSkipBegin) {
        mediaFrameSkipBegin_ = mediaFrameSkipBegin;
        emit mediaFrameSkipBeginChanged();
    }
}

unsigned int Strip::mediaFrameSkipEnd() const
{
    return mediaFrameSkipEnd_;
}

void Strip::setMediaFrameSkipEnd(unsigned int mediaFrameSkipEnd)
{
    if (mediaFrameSkipEnd_ != mediaFrameSkipEnd) {
        mediaFrameSkipEnd_ = mediaFrameSkipEnd;
        emit mediaFrameSkipEndChanged();
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

stripext::PlaylistStripExt *Strip::playlist() const
{
    return playlist_;
}

void Strip::setPlaylist(stripext::PlaylistStripExt *playlist)
{
    if (playlist_ != playlist) {
        playlist_ = playlist;
        emit playlistChanged();
    }
}

stripext::PlaylistStripExt *Strip::mutablePlaylist()
{
    if (playlist() == nullptr) {
        setPlaylist(new stripext::PlaylistStripExt(this));
    }
    return playlist();
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
    pb.set_mediaframeskipbegin(mediaFrameSkipBegin_);
    pb.set_mediaframeskipend(mediaFrameSkipEnd_);
    if (badJs() != nullptr) {
        badJs()->toPb(*pb.mutable_badjs());
    }
    if (audio() != nullptr) {
        audio()->toPb(*pb.mutable_audio());
    }
    if (text() != nullptr) {
        text()->toPb(*pb.mutable_text());
    }
    if (label() != nullptr) {
        label()->toPb(*pb.mutable_label());
    }
    if (playlist() != nullptr) {
        playlist()->toPb(*pb.mutable_playlist());
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
    setMediaFrameSkipBegin(pb.mediaframeskipbegin());
    setMediaFrameSkipEnd(pb.mediaframeskipend());

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
    if (pb.has_playlist()) {
        auto tmp = new stripext::PlaylistStripExt(this);
        tmp->fromPb(pb.playlist());
        setPlaylist(tmp);
    }
}
