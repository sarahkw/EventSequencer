#ifndef FRAMESANDSECONDS_H
#define FRAMESANDSECONDS_H

#include <QObject>

class FramesAndSeconds : public QObject
{
    Q_OBJECT
public:
    explicit FramesAndSeconds(QObject *parent = nullptr);

    Q_INVOKABLE static QString framesToSeconds(int fps, int frames);

signals:

public slots:
};

#endif // FRAMESANDSECONDS_H
