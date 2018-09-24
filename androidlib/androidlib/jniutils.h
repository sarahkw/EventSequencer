#ifndef ANDROIDLIB_JNIUTILS_H
#define ANDROIDLIB_JNIUTILS_H

#include <QAndroidBinder>

namespace androidlib {

class JniUtils
{
    JniUtils() = delete;
public:
    static bool binderTransact(
        QAndroidBinder& binder, int code, const QAndroidParcel& data,
        QAndroidParcel* reply = nullptr,
        QAndroidBinder::CallType flags = QAndroidBinder::CallType::Normal);
};

} // namespace androidlib

#endif // ANDROIDLIB_JNIUTILS_H
