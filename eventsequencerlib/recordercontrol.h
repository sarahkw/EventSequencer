#ifndef RECORDERCONTROL_H
#define RECORDERCONTROL_H

#include <QObject>
#include <QAudio>
#include <QVariant>

class AudioFormatHolder;
class SessionAudio;
class QAudioInput;
class QFile;

class RecorderControl : public QObject
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

    QAudioInput* audioInput_ = nullptr;
    QString audioInputReadyStatus_;
    void updateAudioInput();
    Q_PROPERTY(bool audioInputReady          READ audioInputReady       NOTIFY audioInputReadyStatusChanged)
    Q_PROPERTY(QString audioInputReadyStatus READ audioInputReadyStatus NOTIFY audioInputReadyStatusChanged)

    QString error_;
    AudioState audioState_ = AudioState::Unset;
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(QVariant audioState READ audioState NOTIFY audioStateChanged)

    QFile* outputFile_ = nullptr;

    AudioFormatHolder* audioFormatHolder_ = nullptr;
    SessionAudio* sessionAudio_ = nullptr;

    Q_PROPERTY(QObject* audioFormatHolder READ audioFormatHolder WRITE setAudioFormatHolder NOTIFY audioFormatHolderChanged)
    Q_PROPERTY(QObject* sessionAudio READ sessionAudio WRITE setSessionAudio NOTIFY sessionAudioChanged)

    bool allowOverwrite_ = false;
    Q_PROPERTY(bool allowOverwrite READ allowOverwrite WRITE setAllowOverwrite NOTIFY allowOverwriteChanged)

public:
    explicit RecorderControl(QObject *parent = nullptr);
    ~RecorderControl();

    bool audioInputReady() const;
    QString audioInputReadyStatus() const;

    QObject *audioFormatHolder() const;
    void setAudioFormatHolder(QObject *audioFormatHolder);
    void clearAudioFormatHolder();

    QObject *sessionAudio() const;
    void setSessionAudio(QObject *sessionAudio);
    void clearSessionAudio();

    Q_INVOKABLE void record(QString fileName);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void debug();

    QString error() const;
private:
    void setError(const QString &error);
public:

    QVariant audioState() const;
private:
    void updateAudioState();
public:

    bool allowOverwrite() const;
    void setAllowOverwrite(bool allowOverwrite);

signals:

    void audioInputReadyStatusChanged();

    void audioStateChanged();
    void errorChanged();

    void audioFormatHolderChanged();
    void sessionAudioChanged();

    void allowOverwriteChanged();

public slots:
};

#endif // RECORDERCONTROL_H
