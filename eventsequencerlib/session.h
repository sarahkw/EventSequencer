#ifndef SESSION_H
#define SESSION_H

#include <QObject>

class SessionAudio;

class Session : public QObject
{
    Q_OBJECT

    SessionAudio* sessionAudio_ = nullptr;

    bool wantAudio_ = false;

    Q_PROPERTY(bool wantAudio READ wantAudio WRITE setWantAudio NOTIFY wantAudioChanged)
    Q_PROPERTY(QObject* audio READ audioQObject NOTIFY audioChanged)

public:
    explicit Session(QObject *parent = nullptr);
    ~Session();

    bool wantAudio() const;
    void setWantAudio(bool wantAudio);
    QObject* audioQObject();
    SessionAudio* audio();

signals:

    void wantAudioChanged();
    void audioChanged();

public slots:
};

#endif // SESSION_H
