#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include <QObject>
#include <QAudio>
#include <QVariant>

class AudioFormatHolder;
class SessionAudio;

class AudioControl : public QObject
{
    Q_OBJECT

public:
    enum class AudioState {
        Unset,
        Active,
        Suspended,
        Stopped,
        Idle,
        Interrupted
    };
    Q_ENUM(AudioState)
private:
    AudioState audioState_ = AudioState::Unset;
    Q_PROPERTY(QVariant audioState READ audioState NOTIFY audioStateChanged)
protected:

    AudioFormatHolder* audioFormatHolder_ = nullptr;
    SessionAudio* sessionAudio_ = nullptr;

    Q_PROPERTY(QObject* audioFormatHolder READ audioFormatHolder WRITE setAudioFormatHolder NOTIFY audioFormatHolderChanged)
    Q_PROPERTY(QObject* sessionAudio READ sessionAudio WRITE setSessionAudio NOTIFY sessionAudioChanged)

    virtual void updateAudioInput() = 0;

public:
    explicit AudioControl(QObject *parent = nullptr);

    QVariant audioState() const;
protected:
    void setAudioState(AudioState audioState);
    void setAudioState(QAudio::State audioState);
public:

    QObject *audioFormatHolder() const;
    void setAudioFormatHolder(QObject *audioFormatHolder);
    void clearAudioFormatHolder();

    QObject *sessionAudio() const;
    void setSessionAudio(QObject *sessionAudio);
    void clearSessionAudio();

signals:

    void audioStateChanged();

    void audioFormatHolderChanged();
    void sessionAudioChanged();

public slots:
};

#endif // AUDIOCONTROL_H
