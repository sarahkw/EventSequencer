#include "batchservicereplica.h"

#include <QtGlobal>
#ifdef Q_OS_ANDROID

BatchServiceReplica::BatchServiceReplica(QObject* parent)
    : QObject(parent), ipc_([this](const QVariant& v) {
          status_ = v;
          emit statusChanged(status_);
      })
{
}

QVariant BatchServiceReplica::status() const
{
    return status_;
}

QString BatchServiceReplica::requestExportHtml()
{
    return ipc_.requestExportHtml();
}

#endif // Q_OS_ANDROID
