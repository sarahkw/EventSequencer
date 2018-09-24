#ifndef ANDROIDLIB_SERVICEBINDER_H
#define ANDROIDLIB_SERVICEBINDER_H

#include <QAndroidBinder>
#include <QVariant>

#include <memory>
#include <vector>
#include <mutex>

namespace androidlib {

class ServiceBinder : public QObject, public QAndroidBinder
{
    Q_OBJECT

    std::shared_ptr<QObject> service_;

    std::vector<QAndroidBinder> serviceStateSubscribers_;
    std::mutex serviceStateSubscribersMutex_;

    QVariant serviceState_;
    void broadcastServiceState(const QVariant &serviceState);

public:

    ServiceBinder(std::shared_ptr<QObject> service);
    ~ServiceBinder() override;

    bool onTransact(int code, const QAndroidParcel& data,
                    const QAndroidParcel& reply, CallType flags) override;

private slots:

    void statusChanged(const QVariant& status);
    void androidStartService();
    void androidStopService();
};

} // namespace androidlib

#endif // ANDROIDLIB_SERVICEBINDER_H
