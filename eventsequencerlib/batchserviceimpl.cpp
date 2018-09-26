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

QString BatchServiceImpl::requestExportHtml()
{
    if (workSimulator_.isActive()) {
        return "Work is already running!!";
    }

    workLeft_ = 30;
    emit statusChanged(workLeft_);
    emit androidStartService();
    workSimulator_.start();

    updateStatus();
    return {};
}

void BatchServiceImpl::applicationExiting()
{

}
