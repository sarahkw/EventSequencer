#ifndef CONCATIODEVICENOTIFYEVENT_H
#define CONCATIODEVICENOTIFYEVENT_H

#include <QEvent>
#include <functional>

class ConcatIODeviceNotifyEvent : public QEvent
{
    std::function<void()> callback_;
public:
    static const QEvent::Type s_CustomType;
    ConcatIODeviceNotifyEvent(std::function<void()> callback);
    void call();
};

#endif // CONCATIODEVICENOTIFYEVENT_H
