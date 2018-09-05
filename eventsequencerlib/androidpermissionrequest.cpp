#include "androidpermissionrequest.h"

#include <QtGlobal> // Q_OS_ANDROID
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#endif

AndroidPermissionRequest::AndroidPermissionRequest(QObject *parent) : QObject(parent)
{

}

void AndroidPermissionRequest::requestPermissions() const
{
#ifdef Q_OS_ANDROID
    QtAndroid::requestPermissionsSync(
        {"android.permission.WRITE_EXTERNAL_STORAGE",
         "android.permission.RECORD_AUDIO"});
#endif
}
