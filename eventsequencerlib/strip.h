#ifndef STRIP_H
#define STRIP_H

#include <QObject>

namespace pb {
class Strip;
}

namespace stripext {
class BadJsStripExt;
class BadAudioStripExt;
class TextStripExt;
class LabelStripExt;
}

class Strip : public QObject
{
    Q_OBJECT

    int channel_ = 0;
    int startFrame_ = 0;
    int length_ = 0;

    stripext::BadJsStripExt* badJs_ = nullptr;
    stripext::BadAudioStripExt* badAudio_ = nullptr;
    stripext::TextStripExt* text_ = nullptr;
    stripext::LabelStripExt* label_ = nullptr;

    Q_PROPERTY(int channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(int startFrame READ startFrame WRITE setStartFrame NOTIFY startFrameChanged)
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(stripext::BadJsStripExt* badJs READ badJs WRITE setBadJs NOTIFY badJsChanged)
    Q_PROPERTY(stripext::BadAudioStripExt* badAudio READ badAudio WRITE setBadAudio NOTIFY badAudioChanged)
    Q_PROPERTY(stripext::TextStripExt* text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(stripext::LabelStripExt* label READ label WRITE setLabel NOTIFY labelChanged)

public:
    explicit Strip(QObject *parent = nullptr);

    void toPb(pb::Strip& pb) const;
    void fromPb(const pb::Strip& pb);

    int channel() const;
    void setChannel(int channel);

    int startFrame() const;
    void setStartFrame(int startFrame);

    int length() const;
    void setLength(int length);

    stripext::BadJsStripExt *badJs() const;
    void setBadJs(stripext::BadJsStripExt *badJs);
    Q_INVOKABLE stripext::BadJsStripExt* mutableBadJs();

    stripext::BadAudioStripExt *badAudio() const;
    void setBadAudio(stripext::BadAudioStripExt *badAudio);
    Q_INVOKABLE stripext::BadAudioStripExt *mutableBadAudio();

    stripext::TextStripExt *text() const;
    void setText(stripext::TextStripExt *text);
    Q_INVOKABLE stripext::TextStripExt* mutableText();

    stripext::LabelStripExt *label() const;
    void setLabel(stripext::LabelStripExt *label);
    Q_INVOKABLE stripext::LabelStripExt* mutableLabel();

signals:

    void channelChanged();
    void startFrameChanged();
    void lengthChanged();
    void badJsChanged();
    void badAudioChanged();
    void textChanged();
    void labelChanged();

public slots:
};

Q_DECLARE_METATYPE(Strip*)

#endif // STRIP_H
