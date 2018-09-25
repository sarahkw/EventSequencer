#include "batchserviceimpl.h"

#include "batchservicestatus.h"

BatchServiceImpl::BatchServiceImpl()
{
    workSimulator_.setInterval(1000);
    QObject::connect(&workSimulator_, &QTimer::timeout, [this]() {
        workLeft_--;
        if (workLeft_ == 0) {
            workSimulator_.stop();
            emit androidStopService();
        }
        updateStatus();
    });
}

QVariant BatchServiceImpl::status() const
{
    return QVariant::fromValue(status_);
}

void BatchServiceImpl::updateStatus()
{
    status_.isWorking_ = workSimulator_.isActive();
    status_.statusText_ = QString("Working: %1").arg(workLeft_);
    emit statusChanged(status());
}

void BatchServiceImpl::requestStartWork()
{
    if (workSimulator_.isActive()) {
        qWarning("Work is already running!!");
        return;
    }

    workLeft_ = 30;
    emit statusChanged(workLeft_);
    emit androidStartService();
    workSimulator_.start();

    updateStatus();
}

void BatchServiceImpl::applicationExiting()
{

}
