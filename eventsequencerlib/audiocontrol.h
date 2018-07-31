#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include <QObject>

class AudioControl : public QObject
{
    Q_OBJECT
public:
    explicit AudioControl(QObject *parent = nullptr);

signals:

public slots:
};

#endif // AUDIOCONTROL_H