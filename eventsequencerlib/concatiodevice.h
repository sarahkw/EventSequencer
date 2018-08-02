#ifndef CONCATIODEVICE_H
#define CONCATIODEVICE_H

#include <QIODevice>
#include <list>

class ConcatIODevice : public QIODevice
{
    // We'll take ownership and delete when the device is exhausted.
    std::list<QIODevice*> inputs_;

    bool errorCondition_ = false;

protected:

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

public:
    ConcatIODevice(std::list<QIODevice*>* inputs);
    ~ConcatIODevice() override;
};

#endif // CONCATIODEVICE_H
