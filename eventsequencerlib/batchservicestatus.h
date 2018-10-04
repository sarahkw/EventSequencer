#ifndef BATCHSERVICESTATUS_H
#define BATCHSERVICESTATUS_H

#include <QMetaType>
#include <QString>
#include <QDataStream>

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

Q_DECLARE_METATYPE(BatchServiceStatus)

QDataStream& operator<<(QDataStream& out, const BatchServiceStatus& val);
QDataStream& operator>>(QDataStream& in, BatchServiceStatus& val);

#endif // BATCHSERVICESTATUS_H
