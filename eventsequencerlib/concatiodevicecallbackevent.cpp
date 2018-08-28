#include "concatiodevicecallbackevent.h"

const QEvent::Type ConcatIODeviceCallbackEvent::s_CustomType =
        static_cast<QEvent::Type>(QEvent::registerEventType());

ConcatIODeviceCallbackEvent::ConcatIODeviceCallbackEvent(std::function<void()> callback)
    : QEvent(s_CustomType), callback_(callback)
{

}

void ConcatIODeviceCallbackEvent::call()
{
    callback_();
}
