#ifndef BATCHSERVICEIMPL_H
#define BATCHSERVICEIMPL_H

#include <batchservicelib/batchservicestatus.h>

#include <QObject>
#include <QThread>
#include <QVariant>
#include <QUrl>

#include <memory>

class BatchServiceImplThread : public QThread
{
    Q_OBJECT

    QString fileName_;

protected:

    BatchServiceImplThread(QUrl initialUrl);

    void run() override final;

    struct FinalStatus {
        bool success;
        QString message;
    };

    virtual FinalStatus process() = 0;

    void setFileNameFromPath(QString filePath);
    void reportStatus(QString status);

signals:
    void statusTextChanged(const QString& fileName, const QString& statusText);
};

class BatchServiceImpl : public QObject
{
    Q_OBJECT

    std::unique_ptr<BatchServiceImplThread> workerThread_;

    batchservicelib::BatchServiceStatus status_;
    Q_PROPERTY(QVariant status READ status NOTIFY statusChanged)

public:

    BatchServiceImpl();
    ~BatchServiceImpl() override;

    QVariant status() const;

private:
    QString startRequestedExport(std::function<BatchServiceImplThread*()> creator);
public:

    // SERVICEREQUESTS VERSION 02
    Q_INVOKABLE QString requestExportJson(QUrl documentUrl);
    Q_INVOKABLE QString requestExportPlayToFile(QUrl documentUrl);
    Q_INVOKABLE QString requestExportHtml(QUrl documentUrl);
    Q_INVOKABLE void requestCancelWorker();
    Q_INVOKABLE void requestClearStatus();

signals:

    void statusChanged(const QVariant& status);

    void androidStartService();
    void androidStopService();

public slots:

    void applicationExiting();

private slots:

    void workerFinished();
    void workerStatusTextChanged(const QString& fileName, const QString& statusText);

};

#endif // BATCHSERVICEIMPL_H
