#include "concatiodevice.h"

#include "concatiodevicenotifyevent.h"

#include <QCoreApplication>

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

    // TODO We are relying on the underlying QIODevice* to always try
    //      to return the amount of data that was asked
    //      for. Currently, we don't do the same in the case where we
    //      exhaust a stream and are waiting for the next one.
    Child& child = inputs_.front();
    if (!child.called) {
        if (!!child.callback) {
            QCoreApplication::postEvent(this, new ConcatIODeviceNotifyEvent(child.callback));
        }
        child.called = true;
    }
    qint64 bytesRead = child.iod->read(data, maxlen);
    if (bytesRead == 0) {
        child.iod->close(); // "If you are in doubt, call close() before destroying the QIODevice."
        delete child.iod;
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

ConcatIODevice::~ConcatIODevice()
{
    for (Child& child : inputs_) {
        delete child.iod;
    }
}

bool ConcatIODevice::event(QEvent *event)
{
    if (event->type() == ConcatIODeviceNotifyEvent::s_CustomType) {
        auto* ciodne = static_cast<ConcatIODeviceNotifyEvent*>(event);
        ciodne->call();
        return true;
    }
    return QIODevice::event(event);
}

void ConcatIODevice::append(QIODevice *iod, std::function<void ()> callback)
{
    inputs_.push_back({iod, callback, false});
}
