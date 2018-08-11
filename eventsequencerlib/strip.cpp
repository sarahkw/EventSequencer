#include "strip.h"

#include "document.h"

#include "stripext/badjsstripext.h"
#include "stripext/audiostripext.h"
#include "stripext/textstripext.h"
#include "stripext/labelstripext.h"
#include "stripext/playliststripext.h"

#include "eventsequencer.pb.h"

ChannelIndex Strip::channelIndex() const
{
    return channelIndex_;
}

void Strip::setChannelIndex(ChannelIndex channelIndex)
{
    if (channelIndex_ != channelIndex) {
        auto oldChannelIndex = channelIndex_;
        channelIndex_ = channelIndex;
        emit channelIndexChanged();
        emit channelPositionChanged();
        if (hasPlaced_) {
            emit d_.stripMoved(this, oldChannelIndex, startFrame_, length_);
        }
    }
}

int Strip::channelPosition() const
{
    return d_.channelPositionManager().chanIdxToVisualPosition(channelIndex());
}

void Strip::setChannelPosition(int channelPosition)
{
    setChannelIndex(d_.channelPositionManager().visualPositionToChanIdx(channelPosition));
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
            emit d_.stripMoved(this, channelIndex_, oldStartFrame, length_);
        }
    }
}

int Strip::length() const
{
    return length_;
}

void Strip::setLength(int length)
{
    if (!(length >= 1)) {
        qWarning("Length needs to be >= 1. Ignoring length change.");
        emit lengthChanged(); // QML to re-read length.
        return;
    }
    if (length_ != length) {
        auto oldLength = length_;
        length_ = length;
        emit lengthChanged();
        if (hasPlaced_) {
            emit d_.stripMoved(this, channelIndex_, startFrame_, oldLength);
        }
    }
}

bool Strip::isValidLength(int length)
{
    return length > 0;
}

QUrl Strip::resourceUrl() const
{
    return resourceUrl_;
}

void Strip::setResourceUrl(QUrl resourceUrl)
{
    if (resourceUrl_ != resourceUrl) {
        resourceUrl_ = resourceUrl;
        emit resourceUrlChanged();
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
    pb.set_channel(channelIndex_.first());
    pb.set_startframe(startFrame_);
    pb.set_length(length_);
    pb.set_resourceurl(resourceUrl_.toString().toStdString());
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
    setChannelIndex(ChannelIndex::make1(pb.channel()));
    setStartFrame(pb.startframe());
    setLength(pb.length());
    markAsPlaced();
    setResourceUrl(QString::fromStdString(pb.resourceurl()));

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
