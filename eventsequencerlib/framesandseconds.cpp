#include "framesandseconds.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

FramesAndSeconds::FramesAndSeconds(QObject *parent) : QObject(parent)
{

}

QString FramesAndSeconds::framesToSeconds(int fps, int frames)
{
    const bool isNegative = frames < 0;
    int f = frames * (isNegative ? -1 : 1);

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

    if (isNegative) {
        s = QString("-%1").arg(s);
    }

    return s;
}

QVariantList FramesAndSeconds::secondsToFrames(int fps, QString input)
{
    // TODO Don't keep compiling re, save it somewhere.
    QRegularExpression re("^(((?<h>\\d*):)?(?<m>\\d*):)?(?<s>\\d*)(\\+(?<f>\\d*))?$");
    QRegularExpressionMatch match = re.match(input);
    if (match.hasMatch()) {
        const int h = match.captured("h").toInt();
        const int m = match.captured("m").toInt();
        const int s = match.captured("s").toInt();
        const int f = match.captured("f").toInt();

        const int result = (((h * 60) + m) * 60 + s) * fps + f;

        return {true, result};
    } else {
        return {false};
    }
}
