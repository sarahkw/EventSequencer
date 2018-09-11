#ifndef DESCRIBEDURATION_H
#define DESCRIBEDURATION_H

#include <QObject>

class DescribeDuration : public QObject
{
    Q_OBJECT
public:
    explicit DescribeDuration(QObject *parent = nullptr);

    Q_INVOKABLE static QString describeDuration(qint64 duration);

signals:

public slots:
};

#endif // DESCRIBEDURATION_H
