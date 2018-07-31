#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include <QObject>

class AudioFormatHolder;
class SessionAudio;

class AudioControl : public QObject
{
    Q_OBJECT

protected:

    AudioFormatHolder* audioFormatHolder_ = nullptr;
    SessionAudio* sessionAudio_ = nullptr;

    Q_PROPERTY(QObject* audioFormatHolder READ audioFormatHolder WRITE setAudioFormatHolder NOTIFY audioFormatHolderChanged)
    Q_PROPERTY(QObject* sessionAudio READ sessionAudio WRITE setSessionAudio NOTIFY sessionAudioChanged)

    virtual void updateAudioInput() = 0;
    
public:
    explicit AudioControl(QObject *parent = nullptr);

    QObject *audioFormatHolder() const;
    void setAudioFormatHolder(QObject *audioFormatHolder);
    void clearAudioFormatHolder();

    QObject *sessionAudio() const;
    void setSessionAudio(QObject *sessionAudio);
    void clearSessionAudio();

signals:

    void audioFormatHolderChanged();
    void sessionAudioChanged();

public slots:
};

#endif // AUDIOCONTROL_H
