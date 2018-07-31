#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H

#include "audiocontrol.h"

class QAudioOutput;

class PlayerControl : public AudioControl
{
    Q_OBJECT

    QAudioOutput* audioOutput_ = nullptr;
    QString audioOutputReadyStatus_;
    void updateAudioObject() override;
    Q_PROPERTY(bool audioOutputReady          READ audioOutputReady       NOTIFY audioOutputReadyStatusChanged)
    Q_PROPERTY(QString audioOutputReadyStatus READ audioOutputReadyStatus NOTIFY audioOutputReadyStatusChanged)

    void updateAudioState();

public:
    explicit PlayerControl(QObject* parent = nullptr);
    ~PlayerControl() override;

    bool audioOutputReady() const;
    QString audioOutputReadyStatus() const;

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

signals:

    void audioOutputReadyStatusChanged();

};

#endif // PLAYERCONTROL_H
