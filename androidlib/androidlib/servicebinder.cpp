#include "servicebinder.h"

#include "jniutils.h"

#include <QAndroidParcel>
#include <QAndroidJniObject>
#include <QtAndroid>
#include <QCoreApplication>
#include <QDebug>
#include <QUrl>

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

    // SERVICEREQUESTS VERSION 03
    if (code == 1000) {
        QString retval;
        QUrl documentUrl = data.readVariant().toString();

        QMetaObject::invokeMethod(service_.get(), [&]() {
            retval = service_->requestExportJson(documentUrl);
        }, Qt::BlockingQueuedConnection);

        reply.writeVariant(retval);
    } else if (code == 1001) {
        QString retval;
        QUrl documentUrl = data.readVariant().toString();

        QMetaObject::invokeMethod(service_.get(), [&]() {
            retval = service_->requestExportPlayToFile(documentUrl);
        }, Qt::BlockingQueuedConnection);

        reply.writeVariant(retval);
    } else if (code == 1002) {
        QString retval;
        QUrl documentUrl = data.readVariant().toString();
        bool merge = data.readVariant().toBool();

        QMetaObject::invokeMethod(service_.get(), [&]() {
            retval = service_->requestExportHtml(documentUrl, merge);
        }, Qt::BlockingQueuedConnection);

        reply.writeVariant(retval);
    } else if (code == 1003) {
        QMetaObject::invokeMethod(service_.get(), [&]() {
            service_->requestCancelWorker();
        }, Qt::QueuedConnection);
    } else if (code == 1004) {
        QMetaObject::invokeMethod(service_.get(), [&]() {
            service_->requestClearStatus();
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
        "com/gmail/doctorfill456/docfill/DfService", "androidStartService",
        "(Landroid/content/Context;)V",
        QtAndroid::androidContext().object());
}

void ServiceBinder::androidStopService()
{
    QtAndroid::runOnAndroidThread([]() {
        QtAndroid::androidService().callMethod<void>("androidStopService");
    });
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

ServiceBinder::ServiceBinder(
    std::shared_ptr<batchservicelib::BatchServiceImplBase> service)
    : service_(service)
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
