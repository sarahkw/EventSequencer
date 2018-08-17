#ifndef SAMPLEMODIFYINGIODEVICE_H
#define SAMPLEMODIFYINGIODEVICE_H

#include <QIODevice>

class SampleModifyingIODevice : public QIODevice
{
protected:

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

public:
    SampleModifyingIODevice();
};

#endif // SAMPLEMODIFYINGIODEVICE_H
