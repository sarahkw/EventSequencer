#ifndef RECORDERCONTROL_H
#define RECORDERCONTROL_H

#include <QObject>

class RecorderControl : public QObject
{
    Q_OBJECT
public:
    explicit RecorderControl(QObject *parent = nullptr);

signals:

public slots:
};

#endif // RECORDERCONTROL_H