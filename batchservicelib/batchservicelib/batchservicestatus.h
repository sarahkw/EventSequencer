#ifndef BATCHSERVICELIB_BATCHSERVICESTATUS_H
#define BATCHSERVICELIB_BATCHSERVICESTATUS_H

#include <QMetaType>
#include <QString>
#include <QDataStream>

namespace batchservicelib {

struct BatchServiceStatus
{
    Q_GADGET
public:
    bool isWorking_ = false;

    // Marks this status update as the result of a job. Get the signal for the
    // status update, if isResult is set, then it can be posted as a
    // notification. Thus, there should be only ONE result update for each
    // job.
    bool isResult_ = false;

    QString fileName_;
    QString statusText_;
    Q_PROPERTY(bool isWorking MEMBER isWorking_ CONSTANT)
    Q_PROPERTY(bool isResult MEMBER isResult_ CONSTANT)
    Q_PROPERTY(QString fileName MEMBER fileName_ CONSTANT)
    Q_PROPERTY(QString statusText MEMBER statusText_ CONSTANT)
};

} // namespace batchservicelib

Q_DECLARE_METATYPE(batchservicelib::BatchServiceStatus)

QDataStream& operator<<(QDataStream& out, const batchservicelib::BatchServiceStatus& val);
QDataStream& operator>>(QDataStream& in, batchservicelib::BatchServiceStatus& val);

#endif // BATCHSERVICELIB_BATCHSERVICESTATUS_H
