#include "concatiodevice.h"


qint64 ConcatIODevice::readData(char *data, qint64 maxlen)
{
    if (inputs_.empty()) {
        return -1;
    }
    if (maxlen == 0) {
        // Audio input can request maxlen of 0. Need this special case because
        // we rely on QFile::read to try to fill the whole buffer, and return 0
        // if it can't.
        return 0;
    }
    // TODO This error handling isn't so great.
    QIODevice* front = inputs_.front();
    qint64 bytesRead = front->read(data, maxlen);
    if (bytesRead == 0) {
        delete front;
        inputs_.pop_front();
        return readData(data, maxlen);
    } else if (bytesRead == -1) {
        errorCondition_ = true;
        return -1;
    } else {
        return bytesRead;
    }
}

qint64 ConcatIODevice::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return -1;
}

ConcatIODevice::ConcatIODevice()
{
}

ConcatIODevice::ConcatIODevice(std::list<QIODevice *> *inputs)
{
    inputs_.swap(*inputs);
}

ConcatIODevice::~ConcatIODevice()
{
    for (QIODevice* iod : inputs_) {
        delete iod;
    }
}

void ConcatIODevice::append(QIODevice *iod)
{
    inputs_.push_back(iod);
}
