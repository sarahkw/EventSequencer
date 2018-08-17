#include "samplemodifyingiodevice.h"

qint64 SampleModifyingIODevice::readData(char *data, qint64 maxlen)
{
    return -1;
}

qint64 SampleModifyingIODevice::writeData(const char *data, qint64 len)
{
    return -1;
}

bool SampleModifyingIODevice::open(QIODevice::OpenMode mode)
{
    return QIODevice::open(mode);
}

void SampleModifyingIODevice::close()
{
    QIODevice::close();
}

SampleModifyingIODevice::SampleModifyingIODevice(
    QIODevice* inferior, unsigned bytesPerUnit,
    SampleModifyingIODevice::ModifierFunction modifierFn)
    : inferior_(inferior), bytesPerUnit_(bytesPerUnit), modifierFn_(modifierFn)
{
    incompleteBuffer_.resize(bytesPerUnit_);
}

SampleModifyingIODevice::~SampleModifyingIODevice()
{
    delete inferior_;
}
