#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H

#include "audiocontrol.h"

class PlayerControl : public AudioControl
{
    Q_OBJECT

protected:
    void updateAudioObject() override;
public:
    explicit PlayerControl(QObject* parent = nullptr);
};

#endif // PLAYERCONTROL_H
