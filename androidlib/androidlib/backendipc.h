#ifndef ANDROIDLIB_BACKENDIPC_H
#define ANDROIDLIB_BACKENDIPC_H

#include <QAndroidServiceConnection>
#include <QAndroidBinder>
#include <QVariant>
#include <QUrl>
#include <functional>

namespace androidlib {

class BackendIpc
{
    QAndroidBinder rpcBinder_;

    struct ReceiverBinder : public QAndroidBinder {
        BackendIpc& parent_;
        bool onTransact(int code, const QAndroidParcel& data,
                        const QAndroidParcel& reply, CallType flags) override;
        ReceiverBinder(BackendIpc& parent) : parent_(parent) {}
    } receiverBinder_;

    struct ServiceConnection : public QAndroidServiceConnection {
        BackendIpc* parent_;
        ServiceConnection(BackendIpc* parent) : parent_(parent) {}

        // "Like many callbacks from the system, the methods on this
        // class are called from the main thread of your process." So,
        // called from Android thread.
        void onServiceConnected(const QString &name, const QAndroidBinder &serviceBinder) override;
        void onServiceDisconnected(const QString &name) override;
    } serviceConnection_;

    std::function<void(const QVariant&)> serviceStateCallback_;

private:

    void bindService();
    void unbindService();

public:
    BackendIpc(std::function<void(const QVariant&)>&& cb);
    ~BackendIpc();

    // SERVICEREQUESTS VERSION 04
    QString requestExportJson(QUrl documentUrl);
    QString requestExportPlayToFile(QUrl documentUrl);
    QString requestExportHtml(QUrl documentUrl, bool merge, int mp3Quality);
    void requestCancelWorker();
    void requestClearStatus();

};

} // namespace androidlib

#endif // ANDROIDLIB_BACKENDIPC_H
