#include "batchservicestatus.h"

QDataStream &operator<<(QDataStream &out, const batchservicelib::BatchServiceStatus &val)
{
    out << val.isWorking_ << val.fileName_ << val.statusText_;
    return out;
}

QDataStream &operator>>(QDataStream &in, batchservicelib::BatchServiceStatus &val)
{
    in >> val.isWorking_ >> val.fileName_ >> val.statusText_;
    return in;
}
