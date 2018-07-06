#include "waitfor.h"

#include <QDebug>

QObject *WaitFor::result() const
{
    return result_;
}

void WaitFor::setResult(QObject *result)
{
    if (result_ != result) {
        result_ = result;
        emit resultChanged();
    }
}

void WaitFor::eraseUnregisterFn()
{
    unregisterFn_ = [](WaitFor*){};
}

WaitFor::WaitFor(UnregisterFn&& unregisterFn) : unregisterFn_(std::move(unregisterFn))
{

}

WaitFor::~WaitFor()
{
    unregisterFn_(this);
}
