#ifndef STRIP_H
#define STRIP_H

#include "resource.h"

#include <QObject>

namespace pb {
class Strip;
}

namespace stripext {
class BadJsStripExt;
class AudioStripExt;
class TextStripExt;
class LabelStripExt;
class PlaylistStripExt;
}

class Document;

class Strip : public QObject
{
    Q_OBJECT

    Document& d_;

    // XXX This is hacky. Perhaps have constructor take all the placement values
    //     so we'll never be "not placed".
    bool hasPlaced_ = false;

    int channel_ = 0;
    int startFrame_ = 0;
    int length_ = 0;

    Resource resource_;

    stripext::BadJsStripExt* badJs_ = nullptr;
    stripext::AudioStripExt* audio_ = nullptr;
    stripext::TextStripExt* text_ = nullptr;
    stripext::LabelStripExt* label_ = nullptr;
    stripext::PlaylistStripExt* playlist_ = nullptr;

    Q_PROPERTY(int channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(int startFrame READ startFrame WRITE setStartFrame NOTIFY startFrameChanged)
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(Resource* resource READ resource CONSTANT)
    Q_PROPERTY(stripext::BadJsStripExt* badJs READ badJs WRITE setBadJs NOTIFY badJsChanged)
    Q_PROPERTY(stripext::AudioStripExt* audio READ audio WRITE setAudio NOTIFY audioChanged)
    Q_PROPERTY(stripext::TextStripExt* text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(stripext::LabelStripExt* label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(stripext::PlaylistStripExt* playlist READ playlist WRITE setPlaylist NOTIFY playlistChanged)
public:
    explicit Strip(Document& d, QObject *parent = nullptr);

    Q_INVOKABLE void markAsPlaced();

    void toPb(pb::Strip& pb) const;
    void fromPb(const pb::Strip& pb);

    int channel() const;
    void setChannel(int channel);

    int startFrame() const;
    void setStartFrame(int startFrame);

    int length() const;
    void setLength(int length);

    Resource* resource();

    stripext::BadJsStripExt *badJs() const;
    void setBadJs(stripext::BadJsStripExt *badJs);
    Q_INVOKABLE stripext::BadJsStripExt* mutableBadJs();

    stripext::AudioStripExt *audio() const;
    void setAudio(stripext::AudioStripExt *audio);
    Q_INVOKABLE stripext::AudioStripExt *mutableAudio();

    stripext::TextStripExt *text() const;
    void setText(stripext::TextStripExt *text);
    Q_INVOKABLE stripext::TextStripExt* mutableText();

    stripext::LabelStripExt *label() const;
    void setLabel(stripext::LabelStripExt *label);
    Q_INVOKABLE stripext::LabelStripExt* mutableLabel();

    stripext::PlaylistStripExt *playlist() const;
    void setPlaylist(stripext::PlaylistStripExt *playlist);
    Q_INVOKABLE stripext::PlaylistStripExt *mutablePlaylist();

signals:

    void channelChanged();
    void startFrameChanged();
    void lengthChanged();
    void badJsChanged();
    void audioChanged();
    void textChanged();
    void labelChanged();
    void playlistChanged();

public slots:
};

Q_DECLARE_METATYPE(Strip*)

#endif // STRIP_H
