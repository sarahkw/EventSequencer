#include "jniutils.h"

#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QAndroidParcel>

namespace androidlib {

bool JniUtils::binderTransact(QAndroidBinder& binder, int code,
                              const QAndroidParcel& data, QAndroidParcel* reply,
                              QAndroidBinder::CallType flags)
{
    auto result = binder.handle().callMethod<jboolean>(
        "transact", "(ILandroid/os/Parcel;Landroid/os/Parcel;I)Z", jint(code),
        data.handle().object(), reply ? reply->handle().object() : nullptr,
        jint(flags));

    // I'm trying to be a bit more cautious than Qt. I don't know for sure that
    // Dalvik won't return a random boolean if an exception is thrown.

    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        result = false;
        // XXX Should probably have a special way to let caller know it was an
        //     exception instead of some other failure.
    }

    return result;
}

} // namespace androidlib
