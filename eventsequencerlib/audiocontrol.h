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
        Unconfigured,
        Active,
        Suspended,
        Stopped,
        Idle,
        Interrupted
    };
    Q_ENUM(AudioState)
private:
    AudioState audioState_ = AudioState::Unconfigured;
    Q_PROPERTY(QVariant audioState READ audioState NOTIFY audioStateChanged)

    QString error_;
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
protected:

    AudioFormatHolder* audioFormatHolder_ = nullptr;
    SessionAudio* sessionAudio_ = nullptr;

    Q_PROPERTY(QObject* audioFormatHolder READ audioFormatHolder WRITE setAudioFormatHolder NOTIFY audioFormatHolderChanged)
    Q_PROPERTY(QObject* sessionAudio READ sessionAudio WRITE setSessionAudio NOTIFY sessionAudioChanged)

    virtual void updateAudioObject() = 0;


    QString fileResourceDirectory_;
    Q_PROPERTY(QString fileResourceDirectory READ fileResourceDirectory WRITE setFileResourceDirectory NOTIFY fileResourceDirectoryChanged)

public:
    explicit AudioControl(QObject *parent = nullptr);

    QVariant audioState() const;
protected:
    void setAudioState(AudioState audioState);
    void setAudioState(QAudio::State audioState);
public:

    QString error() const;
protected:
    void setError(const QString &error);
    void setError(QAudio::Error error);
public:

    QObject *audioFormatHolder() const;
    void setAudioFormatHolder(QObject *audioFormatHolder);
    void clearAudioFormatHolder();

    QObject *sessionAudio() const;
    void setSessionAudio(QObject *sessionAudio);
    void clearSessionAudio();

    QString fileResourceDirectory() const;
    void setFileResourceDirectory(const QString &fileResourceDirectory);

signals:

    void audioStateChanged();
    void errorChanged();

    void audioFormatHolderChanged();
    void sessionAudioChanged();

    void fileResourceDirectoryChanged();

public slots:
};

#endif // AUDIOCONTROL_H
