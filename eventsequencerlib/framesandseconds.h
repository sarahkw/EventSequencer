#ifndef FRAMESANDSECONDS_H
#define FRAMESANDSECONDS_H

#include <QObject>
#include <QVariantList>

class FramesAndSeconds : public QObject
{
    Q_OBJECT
public:
    explicit FramesAndSeconds(QObject *parent = nullptr);

    Q_INVOKABLE static QString framesToSeconds(
        int fps, int frames, bool hideFramesWhenHoursShown = false);

    Q_INVOKABLE static QVariantList secondsToFrames(int fps, QString input);

signals:

public slots:
};

#endif // FRAMESANDSECONDS_H
