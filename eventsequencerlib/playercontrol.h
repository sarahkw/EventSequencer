#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H

#include "audiocontrol.h"

class PlayerControl : public AudioControl
{
    Q_OBJECT
public:
    explicit PlayerControl(QObject* parent = nullptr);
};

#endif // PLAYERCONTROL_H
