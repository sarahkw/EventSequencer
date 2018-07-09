#ifndef STRIP_H
#define STRIP_H

#include <QObject>

namespace pb {
class Strip;
}

namespace stripext {
class BadJsStripExt;
class AudioStripExt;
}

class Strip : public QObject
{
    Q_OBJECT

    int channel_ = 0;
    int startFrame_ = 0;
    int length_ = 0;

    stripext::BadJsStripExt* badJs_ = nullptr;
    stripext::AudioStripExt* audio_ = nullptr;

    Q_PROPERTY(int channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(int startFrame READ startFrame WRITE setStartFrame NOTIFY startFrameChanged)
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(stripext::BadJsStripExt* badJs READ badJs WRITE setBadJs NOTIFY badJsChanged)
    Q_PROPERTY(stripext::AudioStripExt* audio READ audio WRITE setAudio NOTIFY audioChanged)

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
    Q_INVOKABLE void initBadJs();

    stripext::AudioStripExt *audio() const;
    void setAudio(stripext::AudioStripExt *audio);
    Q_INVOKABLE void initAudio();

signals:

    void channelChanged();
    void startFrameChanged();
    void lengthChanged();
    void badJsChanged();
    void audioChanged();

public slots:
};

Q_DECLARE_METATYPE(Strip*)

#endif // STRIP_H
