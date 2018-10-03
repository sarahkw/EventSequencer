#include "servicemain.h"

#include "servicebinder.h"

#include <QAndroidService>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>

#include <memory>

namespace {

void notificationCancelWorker(JNIEnv *env, jobject objectOrClass)
{
    qInfo("Cancel worker!");
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

int ServiceMain::serviceMain(QObject* service, int &argc, char **argv)
{
    std::shared_ptr<QObject> serviceSp(service);

    registerNativeMethods();

    QAndroidService app(argc, argv, [serviceSp](const QAndroidIntent&) -> QAndroidBinder* {
        return new ServiceBinder(serviceSp);
    });

    int result = app.exec();

    // TODO Tell the service that we're exiting, somehow.

    return result;
}

} // namespace androidlib
