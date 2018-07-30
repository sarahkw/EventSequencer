#ifndef RECORDERCONTROL_H
#define RECORDERCONTROL_H

#include <QObject>

class AudioFormatHolder;
class SessionAudio;

class RecorderControl : public QObject
{
    Q_OBJECT

    AudioFormatHolder* audioFormat_ = nullptr;
    SessionAudio* sessionAudio_ = nullptr;

    Q_PROPERTY(QObject* audioFormat READ audioFormat WRITE setAudioFormat NOTIFY audioFormatChanged)
    Q_PROPERTY(QObject* sessionAudio READ sessionAudio WRITE setSessionAudio NOTIFY sessionAudioChanged)

public:
    explicit RecorderControl(QObject *parent = nullptr);

    QObject *audioFormat() const;
    void setAudioFormat(QObject *audioFormat);

    QObject *sessionAudio() const;
    void setSessionAudio(QObject *sessionAudio);

    Q_INVOKABLE void debug();

signals:

    void audioFormatChanged();
    void sessionAudioChanged();

public slots:
};

#endif // RECORDERCONTROL_H
