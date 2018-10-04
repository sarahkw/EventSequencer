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

QString BatchServiceReplica::requestExportJson(QUrl documentUrl)
{
    return ipc_.requestExportJson(documentUrl);
}

QString BatchServiceReplica::requestExportPlayToFile(QUrl documentUrl)
{
    return ipc_.requestExportPlayToFile(documentUrl);
}

QString BatchServiceReplica::requestExportHtml(QUrl documentUrl)
{
    return ipc_.requestExportHtml(documentUrl);
}

void BatchServiceReplica::requestCancelWorker()
{
    return ipc_.requestCancelWorker();
}

void BatchServiceReplica::requestClearStatus()
{
    return ipc_.requestClearStatus();
}

#endif // Q_OS_ANDROID
