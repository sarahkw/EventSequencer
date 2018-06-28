#include "framesandseconds.h"

FramesAndSeconds::FramesAndSeconds(QObject *parent) : QObject(parent)
{

}

QString FramesAndSeconds::framesToSeconds(int fps, int frames)
{
    int f = frames;

    const int hours = f / (fps * 60 * 60);
    f %= (fps * 60 * 60);

    const int minutes = f / (fps * 60);
    f %= (fps * 60);

    const int seconds = f / fps;
    f %= fps;

    const int framesLeft = f;

    const bool hasMinutes = minutes > 0;

    QString s = QString("%1+%2")
                    .arg(seconds, hasMinutes ? 2 : 0, 10, QLatin1Char('0'))
                    .arg(framesLeft);

    const bool hasHours = hours > 0;

    if (hasMinutes) {
        s = QString("%1:%2")
                .arg(minutes, hasHours ? 2 : 0, 10, QLatin1Char('0'))
                .arg(s);
    }

    if (hasHours) {
        s = QString("%1:%2").arg(hours).arg(s);
    }

    return s;
}
