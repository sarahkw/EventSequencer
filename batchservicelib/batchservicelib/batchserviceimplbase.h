#ifndef BATCHSERVICELIB_BATCHSERVICEIMPLBASE_H
#define BATCHSERVICELIB_BATCHSERVICEIMPLBASE_H

#include <QObject>
#include <QUrl>

namespace batchservicelib {

class BatchServiceImplBase : public QObject
{
    Q_OBJECT
public:
    explicit BatchServiceImplBase(QObject *parent = nullptr);

    // SERVICEREQUESTS VERSION 03
    virtual QString requestExportJson(QUrl documentUrl) = 0;
    virtual QString requestExportPlayToFile(QUrl documentUrl) = 0;
    virtual QString requestExportHtml(QUrl documentUrl, bool merge) = 0;
    virtual void requestCancelWorker() = 0;
    virtual void requestClearStatus() = 0;

signals:

    // This sucks, but moc doesn't support NOTIFY signals of parent class if
    // it has arguments.
    //
    // TODO Move the property to the -base level.
    void parentStatusChanged(const QVariant& status);

public slots:
};

} // namespace batchservicelib

#endif // BATCHSERVICELIB_BATCHSERVICEIMPLBASE_H
