#include "servicemain.h"

#include "servicebinder.h"

#include <batchservicelib/batchservicestatus.h>

#include <QAndroidService>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>

#include <memory>

namespace {

static std::weak_ptr<batchservicelib::BatchServiceImplBase> SERVICE_PTR;

// Called by Android thread.
void notificationCancelWorker(JNIEnv *env, jobject objectOrClass)
{
    Q_UNUSED(env);
    Q_UNUSED(objectOrClass);
    if (auto ptr = SERVICE_PTR.lock()) {
        QMetaObject::invokeMethod(ptr.get(), [ptr]() {
            ptr->requestCancelWorker();
        }, Qt::QueuedConnection);
    }
}

void registerNativeMethods()
{
    JNINativeMethod methods[]{
        {"notificationCancelWorker", "()V",
         reinterpret_cast<void*>(notificationCancelWorker)}};

    QAndroidJniObject javaClass("com/gmail/doctorfill456/docfill/DfService");
    QAndroidJniEnvironment env;
    jclass objectClass = env->GetObjectClass(javaClass.object<jobject>());
    env->RegisterNatives(objectClass,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(objectClass);
}

} // namespace anonymous

namespace androidlib {

int ServiceMain::serviceMain(batchservicelib::BatchServiceImplBase* service,
                             int &argc, char **argv)
{
    std::shared_ptr<batchservicelib::BatchServiceImplBase> serviceSp(service);

    SERVICE_PTR = serviceSp;
    registerNativeMethods();

    QObject::connect(serviceSp.get(), &batchservicelib::BatchServiceImplBase::parentStatusChanged,
                     [](const QVariant& status) {
        Q_ASSERT(status.canConvert<batchservicelib::BatchServiceStatus>());
        auto status2 = status.value<batchservicelib::BatchServiceStatus>();

        qInfo("Status update! isWorking = %d fileName = %s statusText = %s",
              status2.isWorking_, qPrintable(status2.fileName_),
              qPrintable(status2.statusText_));
    });

    QAndroidService app(argc, argv, [serviceSp](const QAndroidIntent&) -> QAndroidBinder* {
        return new ServiceBinder(serviceSp);
    });

    int result = app.exec();

    // TODO Tell the service that we're exiting, somehow.

    return result;
}

} // namespace androidlib
