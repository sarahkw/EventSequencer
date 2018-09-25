#include "batchservicestatus.h"

QDataStream &operator<<(QDataStream &out, const BatchServiceStatus &val)
{
    out << val.isWorking_ << val.statusText_;
    return out;
}

QDataStream &operator>>(QDataStream &in, BatchServiceStatus &val)
{
    in >> val.isWorking_ >> val.statusText_;
    return in;
}
