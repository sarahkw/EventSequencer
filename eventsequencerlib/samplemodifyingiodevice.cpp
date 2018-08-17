#include "samplemodifyingiodevice.h"

qint64 SampleModifyingIODevice::readData(char *data, qint64 maxlen)
{
    return -1;
}

qint64 SampleModifyingIODevice::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return -1;
}

SampleModifyingIODevice::SampleModifyingIODevice()
{

}

