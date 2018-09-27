#ifndef BATCHSERVICEIMPL_H
#define BATCHSERVICEIMPL_H

#include "batchservicestatus.h"

#include <QObject>
#include <QThread>
#include <QVariant>
#include <QUrl>

#include <memory>

class BatchServiceImplThread : public QThread
{
    Q_OBJECT
signals:
    void statusTextChanged(const QString& statusText);
};

class BatchServiceImpl : public QObject
{
    Q_OBJECT

    std::unique_ptr<BatchServiceImplThread> workerThread_;

    BatchServiceStatus status_;
    Q_PROPERTY(QVariant status READ status NOTIFY statusChanged)

public:

    BatchServiceImpl();

    QVariant status() const;

private:
    QString startRequestedExport(std::function<BatchServiceImplThread*()> creator);
public:

    Q_INVOKABLE QString requestExportJson(QUrl documentUrl);
    Q_INVOKABLE QString requestExportPlayToFile(QUrl documentUrl);
    Q_INVOKABLE QString requestExportHtml(QUrl documentUrl);

signals:

    void statusChanged(const QVariant& status);

    void androidStartService();
    void androidStopService();

public slots:

    void applicationExiting();

private slots:

    void workerFinished();
    void workerStatusTextChanged(const QString& statusText);

};

#endif // BATCHSERVICEIMPL_H
