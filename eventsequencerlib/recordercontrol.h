#ifndef RECORDERCONTROL_H
#define RECORDERCONTROL_H

#include <QObject>

class AudioFormatHolder;
class SessionAudio;
class QAudioInput;

class RecorderControl : public QObject
{
    Q_OBJECT

    QAudioInput* audioInput_ = nullptr;
    QString audioInputReadyStatus_;
    void updateAudioInput();
    Q_PROPERTY(bool audioInputReady          READ audioInputReady       NOTIFY audioInputReadyStatusChanged)
    Q_PROPERTY(QString audioInputReadyStatus READ audioInputReadyStatus NOTIFY audioInputReadyStatusChanged)

    AudioFormatHolder* audioFormatHolder_ = nullptr;
    SessionAudio* sessionAudio_ = nullptr;

    Q_PROPERTY(QObject* audioFormatHolder READ audioFormatHolder WRITE setAudioFormatHolder NOTIFY audioFormatHolderChanged)
    Q_PROPERTY(QObject* sessionAudio READ sessionAudio WRITE setSessionAudio NOTIFY sessionAudioChanged)

public:
    explicit RecorderControl(QObject *parent = nullptr);
    ~RecorderControl();

    bool audioInputReady() const;
    QString audioInputReadyStatus() const;

    QObject *audioFormatHolder() const;
    void setAudioFormatHolder(QObject *audioFormatHolder);

    QObject *sessionAudio() const;
    void setSessionAudio(QObject *sessionAudio);

    Q_INVOKABLE void debug();

signals:

    void audioInputReadyStatusChanged();

    void audioFormatHolderChanged();
    void sessionAudioChanged();

public slots:
};

#endif // RECORDERCONTROL_H
