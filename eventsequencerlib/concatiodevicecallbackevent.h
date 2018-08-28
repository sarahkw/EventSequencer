#ifndef CONCATIODEVICECALLBACKEVENT_H
#define CONCATIODEVICECALLBACKEVENT_H

#include <QEvent>
#include <functional>

// This will be on the event loop to avoid possibly long operations on
// the hot path.
class ConcatIODeviceCallbackEvent : public QEvent
{
    std::function<void()> callback_;
public:
    static const QEvent::Type s_CustomType;
    ConcatIODeviceCallbackEvent(std::function<void()> callback);
    void call();
};

#endif // CONCATIODEVICECALLBACKEVENT_H
