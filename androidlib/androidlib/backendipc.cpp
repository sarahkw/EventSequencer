#include "backendipc.h"

#include "jniutils.h"

#include <QtAndroid>
#include <QAndroidIntent>
#include <QAndroidParcel>
#include <QAndroidJniExceptionCleaner>
#include <QDebug>
#include <QCoreApplication>

namespace androidlib {

void BackendIpc::bindService()
{
    auto serviceIntent = QAndroidIntent(QtAndroid::androidActivity().object(),
                                        "com.gmail.doctorfill456.docfill.DfService");
    bool result = QtAndroid::bindService(serviceIntent,
                                         serviceConnection_,
                                         QtAndroid::BindFlag::AutoCreate);
    if (!result) {
        qWarning() << "BackendIpc unable to bind to service";
    }
}

void BackendIpc::unbindService()
{
    QAndroidJniExceptionCleaner cleaner;
    QtAndroid::androidContext().callMethod<void>(
        "unbindService", "(Landroid/content/ServiceConnection;)V",
        serviceConnection_.handle().object());
}

BackendIpc::BackendIpc(std::function<void(const QVariant&)>&& cb)
    : receiverBinder_(*this), serviceConnection_(this), serviceStateCallback_(std::move(cb))
{
    bindService();
}

BackendIpc::~BackendIpc()
{
    unbindService();
}

QString BackendIpc::requestExportJson(QUrl documentUrl)
{
    QAndroidParcel data;
    QAndroidParcel reply;
    data.writeVariant(documentUrl);
    JniUtils::binderTransact(rpcBinder_, 1000, data, &reply);
    return reply.readVariant().toString();
}

QString BackendIpc::requestExportPlayToFile(QUrl documentUrl)
{
    QAndroidParcel data;
    QAndroidParcel reply;
    data.writeVariant(documentUrl);
    JniUtils::binderTransact(rpcBinder_, 1001, data, &reply);
    return reply.readVariant().toString();
}

QString BackendIpc::requestExportHtml(QUrl documentUrl)
{
    QAndroidParcel data;
    QAndroidParcel reply;
    data.writeVariant(documentUrl);
    JniUtils::binderTransact(rpcBinder_, 1002, data, &reply);
    return reply.readVariant().toString();
}

bool BackendIpc::ReceiverBinder::onTransact(int code, const QAndroidParcel &data, const QAndroidParcel &reply, QAndroidBinder::CallType flags)
{
    // Called from Binder thread pool.

    if (code == 10) {
        QVariant x = data.readVariant();
        QMetaObject::invokeMethod(QCoreApplication::instance(),
                                      [x, this]() {
            parent_.serviceStateCallback_(x);
        });
    } else {
        return false;
    }
    return true;
}

void BackendIpc::ServiceConnection::onServiceConnected(const QString &name, const QAndroidBinder &serviceBinder)
{
    parent_->rpcBinder_ = serviceBinder;

    {
        // Subscribe to status.
        QAndroidParcel data;
        data.writeBinder(parent_->receiverBinder_);
        JniUtils::binderTransact(parent_->rpcBinder_, 10, data);
    }
}

void BackendIpc::ServiceConnection::onServiceDisconnected(const QString &name)
{
    qWarning() << "BackendIpc unexpected service disconnected";
}

} // namespace androidlib
