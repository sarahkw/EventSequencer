#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H

#include "audiocontrol.h"
#include "playable/playablebase.h"

class QAudioOutput;
class QIODevice;

class PlayerControl : public AudioControl
{
    Q_OBJECT

    QAudioOutput* audioOutput_ = nullptr;
    QString audioOutputReadyStatus_;
    void updateAudioObject() override;
    Q_PROPERTY(bool audioOutputReady          READ audioOutputReady       NOTIFY audioOutputReadyStatusChanged)
    Q_PROPERTY(QString audioOutputReadyStatus READ audioOutputReadyStatus NOTIFY audioOutputReadyStatusChanged)

    bool autoStopOnIdle_ = false;
    Q_PROPERTY(bool autoStopOnIdle READ autoStopOnIdle WRITE setAutoStopOnIdle NOTIFY autoStopOnIdleChanged)

    void updateAudioState();

    playable::PlayableBase* playable_ = nullptr;
    Q_PROPERTY(QObject* playable READ playable WRITE setPlayable NOTIFY playableChanged)

    QIODevice* playingDevice_ = nullptr;

public:
    explicit PlayerControl(QObject* parent = nullptr);
    ~PlayerControl() override;

    bool audioOutputReady() const;
    QString audioOutputReadyStatus() const;

    bool autoStopOnIdle() const;
    void setAutoStopOnIdle(bool autoStopOnIdle);

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

    QObject *playable() const;
    void setPlayable(QObject *playable);
    void clearPlayable();

signals:

    void audioOutputReadyStatusChanged();
    void autoStopOnIdleChanged();

    void playableChanged();
};

#endif // PLAYERCONTROL_H
