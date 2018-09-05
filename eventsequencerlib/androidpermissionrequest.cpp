#include "androidpermissionrequest.h"

#include <QtGlobal> // Q_OS_ANDROID

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QHash>
#endif

AndroidPermissionRequest::AndroidPermissionRequest(QObject *parent) : QObject(parent)
{

}

void AndroidPermissionRequest::requestStorage() const
{
#ifdef Q_OS_ANDROID
    QtAndroid::requestPermissionsSync(
        {"android.permission.WRITE_EXTERNAL_STORAGE"});
#endif
}

bool AndroidPermissionRequest::requestRecordAudio() const
{
#ifdef Q_OS_ANDROID
    static const char RECORD_AUDIO[] = "android.permission.RECORD_AUDIO";

    auto result = QtAndroid::requestPermissionsSync({RECORD_AUDIO});
    auto iter = result.find(RECORD_AUDIO);
    if (iter != result.end()) {
        return *iter == QtAndroid::PermissionResult::Granted;
    } else {
        return false;
    }
#else
    return true;
#endif
}

//void AndroidPermissionRequest::requestPermissions() const
//{
//#ifdef Q_OS_ANDROID
//    QtAndroid::requestPermissionsSync(
//        {"android.permission.WRITE_EXTERNAL_STORAGE",
//         "android.permission.RECORD_AUDIO"});
//#endif
//}
