#include "concatiodevicenotifyevent.h"

const QEvent::Type ConcatIODeviceNotifyEvent::s_CustomType =
        static_cast<QEvent::Type>(QEvent::registerEventType());

ConcatIODeviceNotifyEvent::ConcatIODeviceNotifyEvent(std::function<void()> callback)
    : QEvent(s_CustomType), callback_(callback)
{

}

void ConcatIODeviceNotifyEvent::call()
{
    callback_();
}
