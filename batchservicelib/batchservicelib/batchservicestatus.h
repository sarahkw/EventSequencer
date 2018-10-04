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
    QString fileName_;
    QString statusText_;
    Q_PROPERTY(bool isWorking MEMBER isWorking_ CONSTANT)
    Q_PROPERTY(QString fileName MEMBER fileName_ CONSTANT)
    Q_PROPERTY(QString statusText MEMBER statusText_ CONSTANT)
};

} // namespace batchservicelib

Q_DECLARE_METATYPE(batchservicelib::BatchServiceStatus)

QDataStream& operator<<(QDataStream& out, const batchservicelib::BatchServiceStatus& val);
QDataStream& operator>>(QDataStream& in, batchservicelib::BatchServiceStatus& val);

#endif // BATCHSERVICELIB_BATCHSERVICESTATUS_H
