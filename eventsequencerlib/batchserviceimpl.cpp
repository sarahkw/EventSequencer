#include "batchserviceimpl.h"

#include "batchservicestatus.h"

#include <QDebug>

namespace {

class ExportJsonWorkerThread : public BatchServiceImplThread {
protected:
    void run() override
    {
        for (int i = 5; i > 0; --i) {
            emit statusTextChanged(QString("Export JSON: %1").arg(i));
            QThread::sleep(1);
        }
    }
};

class ExportPlayToFileWorkerThread : public BatchServiceImplThread {
protected:
    void run() override
    {
        for (int i = 10; i > 0; --i) {
            emit statusTextChanged(QString("Export Play to File: %1").arg(i));
            QThread::sleep(1);
        }
    }
};

class ExportHtmlWorkerThread : public BatchServiceImplThread {
protected:
    void run() override
    {
        for (int i = 30; i > 0; --i) {
            emit statusTextChanged(QString("Export HTML: %1").arg(i));
            QThread::sleep(1);
        }
    }
};

} // namespace anonymous

BatchServiceImpl::BatchServiceImpl()
{
}

QVariant BatchServiceImpl::status() const
{
    return QVariant::fromValue(status_);
}

QString BatchServiceImpl::startRequestedExport(std::function<BatchServiceImplThread *()> creator)
{
    if (!!workerThread_) {
        return "Work is already running!!";
    }

    workerThread_ = decltype(workerThread_)(creator());

    QObject::connect(workerThread_.get(), &BatchServiceImplThread::finished,
                     this, &BatchServiceImpl::workerFinished,
                     Qt::QueuedConnection);
    QObject::connect(workerThread_.get(), &BatchServiceImplThread::statusTextChanged,
                     this, &BatchServiceImpl::workerStatusTextChanged,
                     Qt::QueuedConnection);

    status_.isWorking_ = true;
    status_.statusText_.clear();
    emit statusChanged(QVariant::fromValue(status_));
    emit androidStartService();

    workerThread_->start();

    return {};
}

QString BatchServiceImpl::requestExportJson(QUrl documentUrl)
{
    /*
    if (workSimulator_.isActive()) {
        return "Work is already running!!";
    }

    qInfo() << "requestExportJson" << documentUrl;

    workLeft_ = 30;
    emit statusChanged(workLeft_);
    emit androidStartService();
    workSimulator_.start();

    updateStatus();
    */
    return {};
}

QString BatchServiceImpl::requestExportPlayToFile(QUrl documentUrl)
{
    /*
    if (workSimulator_.isActive()) {
        return "Work is already running!!";
    }

    qInfo() << "requestExportPlayToFile" << documentUrl;

    workLeft_ = 30;
    emit statusChanged(workLeft_);
    emit androidStartService();
    workSimulator_.start();

    updateStatus();
    */
    return {};
}

QString BatchServiceImpl::requestExportHtml(QUrl documentUrl)
{
    return startRequestedExport([]() { return new ExportHtmlWorkerThread; });
}

void BatchServiceImpl::applicationExiting()
{

}

void BatchServiceImpl::workerFinished()
{
    // XXX I *think* but am not positive it's okay to delete the thread now,
    //     since this is a queued connection.
    workerThread_.reset();

    status_.isWorking_ = false;
    emit statusChanged(QVariant::fromValue(status_));
    emit androidStopService();
}

void BatchServiceImpl::workerStatusTextChanged(const QString &statusText)
{
    status_.statusText_ = statusText;
    emit statusChanged(QVariant::fromValue(status_));
}
