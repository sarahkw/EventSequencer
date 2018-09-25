#include "servicebinder.h"

#include "jniutils.h"

#include <QAndroidParcel>
#include <QAndroidJniObject>
#include <QtAndroid>
#include <QCoreApplication>
#include <QDebug>

namespace androidlib {

bool ServiceBinder::onTransact(int code, const QAndroidParcel& data,
                               const QAndroidParcel& reply,
                               QAndroidBinder::CallType flags)
{
    // ServiceBinder internal functionality.
    if (code == 10) {
        std::lock_guard<std::mutex> guard(serviceStateSubscribersMutex_);
        auto binder = data.readBinder();
        if (!serviceState_.isNull()) {
            QAndroidParcel data;
            data.writeVariant(serviceState_);
            JniUtils::binderTransact(binder, 10, data, nullptr,
                                     QAndroidBinder::CallType::OneWay);
        }
        serviceStateSubscribers_.push_back(binder);

        return true;
    }

    // Custom methods.
    auto service = service_;

    if (code == 1000) {
        // 2 invokeMethod in order to get a copy of the shared pointer onto the
        // queue.
        QMetaObject::invokeMethod(service.get(), [service]() {
            QMetaObject::invokeMethod(service.get(), "requestStartWork");
        }, Qt::QueuedConnection);
    } else {
        return false;
    }
    return true;
}

void ServiceBinder::statusChanged(const QVariant &status)
{
    this->broadcastServiceState(status);
}

void ServiceBinder::androidStartService()
{
    // XXX Run from Qt thread. I think Android is fine with this?
    QAndroidJniObject::callStaticMethod<void>(
        "com/gmail/doctorfill456/docfill/DfService", "startMyService",
        "(Landroid/content/Context;)V",
        QtAndroid::androidContext().object());
}

void ServiceBinder::androidStopService()
{
    // XXX Run from Qt thread. I think Android is fine with this?
    static const int STOP_FOREGROUND_REMOVE = 1;
    QtAndroid::androidService().callMethod<void>(
                "stopForeground", "(I)V", STOP_FOREGROUND_REMOVE);
    QtAndroid::androidService().callMethod<void>("stopSelf");
}

void ServiceBinder::broadcastServiceState(const QVariant &serviceState)
{
    std::lock_guard<std::mutex> guard(serviceStateSubscribersMutex_);
    serviceState_ = serviceState;

    QAndroidParcel data;
    data.writeVariant(serviceState_);

    std::vector<decltype(serviceStateSubscribers_)::iterator> toErase;

    for (auto iter = serviceStateSubscribers_.begin();
         iter != serviceStateSubscribers_.end();
         ++iter) {
        if (!JniUtils::binderTransact(*iter, 10, data, nullptr,
                                      QAndroidBinder::CallType::OneWay)) {
            toErase.push_back(iter);
        }
    }

    // XXX O(n^2)
    for (auto iter = toErase.rbegin();
         iter != toErase.rend();
         ++iter) {
        serviceStateSubscribers_.erase(*iter);
    }
}

ServiceBinder::ServiceBinder(std::shared_ptr<QObject> service) : service_(service)
{
    QObject::connect(service.get(),
                     SIGNAL(statusChanged(QVariant)), this,
                     SLOT(statusChanged(QVariant)),
                     Qt::DirectConnection);
    QObject::connect(service.get(), SIGNAL(androidStartService()), this,
                     SLOT(androidStartService()),
                     Qt::DirectConnection);
    QObject::connect(service.get(), SIGNAL(androidStopService()), this,
                     SLOT(androidStopService()),
                     Qt::DirectConnection);

    serviceState_ = service->property("status");
}

ServiceBinder::~ServiceBinder()
{
    service_->disconnect(this);
}

} // namespace androidlib
