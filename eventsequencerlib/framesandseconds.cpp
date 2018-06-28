#include "framesandseconds.h"

FramesAndSeconds::FramesAndSeconds(QObject *parent) : QObject(parent)
{

}

QString FramesAndSeconds::framesToSeconds(int fps, int frames)
{
    int seconds = frames / fps;
    int secondsLeftOver = frames % fps;
    return QString("%1+%2").arg(seconds).arg(secondsLeftOver);
}
