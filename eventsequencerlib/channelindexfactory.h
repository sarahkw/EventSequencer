#ifndef CHANNELINDEXFACTORY_H
#define CHANNELINDEXFACTORY_H

#include <QObject>
#include <QVariant>
#include "channelindex.h"

class ChannelIndexFactory : public QObject
{
    Q_OBJECT
public:
    explicit ChannelIndexFactory(QObject *parent = nullptr);

    Q_INVOKABLE QVariant make1(int first) const;
    Q_INVOKABLE QVariant make2(int first, unsigned second) const;
    Q_INVOKABLE QVariant makeFromPathString(QString pathString) const;

signals:

public slots:
};

#endif // CHANNELINDEXFACTORY_H
