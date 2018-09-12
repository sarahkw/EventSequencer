#include "describeduration.h"

DescribeDuration::DescribeDuration(QObject *parent) : QObject(parent)
{

}

QString DescribeDuration::describeDuration(qint64 duration)
{
    static const char* LABELS[] = {"d", "h", "m", "s"};

    const bool isNeg = duration < 0;
    if (isNeg) {
        duration = -duration;
    }

    qint64 values[4]{};

    qint64& days = values[0];
    qint64& hours = values[1];
    qint64& minutes = values[2];
    qint64& seconds = values[3];

    seconds = duration;

    minutes = seconds / 60;
    seconds %= 60;

    hours = minutes / 60;
    minutes %= 60;

    days = hours / 24;
    hours %= 24;

    QString ret;

    size_t start = 4;
    for (size_t i = 0; i < 4; ++i) {
        if (values[i] != 0) {
            start = i;
            break;
        }
    }

    for (size_t i = start; i < 4; ++i) {
        if (values[i] > 0) {
            ret += QString("%1%2").arg(values[i]).arg(LABELS[i]);
        }
    }

    if (ret.isEmpty()) {
        ret = "0";
    }

    if (isNeg) {
        ret.prepend("-");
    }

    return ret;
}
