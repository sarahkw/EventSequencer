#include "batchserviceimpl.h"

BatchServiceImpl::BatchServiceImpl()
{
    workSimulator_.setInterval(1000);
    QObject::connect(&workSimulator_, &QTimer::timeout, [this]() {
        workLeft_--;
        emit statusChanged(workLeft_);
        if (workLeft_ == 0) {
            workSimulator_.stop();
            emit androidStopService();
        }
    });
}

QVariant BatchServiceImpl::status() const
{
    return status_;
}

void BatchServiceImpl::setStatus(const QVariant &status)
{
    if (status_ != status) {
        status_ = status;
        emit statusChanged(status);
    }
}

void BatchServiceImpl::requestStartWork()
{
    if (workSimulator_.isActive()) {
        qWarning("Work is already running!!");
        return;
    }

    workLeft_ = 60;
    emit statusChanged(workLeft_);
    emit androidStartService();
    workSimulator_.start();
}

void BatchServiceImpl::applicationExiting()
{

}
