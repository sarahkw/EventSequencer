#include "samplemodifyingiodevice.h"

qint64 SampleModifyingIODevice::readData(char *data, qint64 maxlen)
{
    return -1;
}

qint64 SampleModifyingIODevice::writeData(const char *data, qint64 len)
{
    Q_ASSERT(false); // Not impl
    return -1;
}

bool SampleModifyingIODevice::open(QIODevice::OpenMode mode)
{
    if (inferior_->open(mode)) {
        // Base open never fails. Not worth supporting handling of base failure.
        QIODevice::open(mode);
        return true;
    } else {
        setErrorString(QString("Inferior device failed to open: %1")
                           .arg(inferior_->errorString()));
        return false;
    }
}

void SampleModifyingIODevice::close()
{
    if (inferior_->isOpen()) {
        inferior_->close();
    }
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
