#ifndef CHANNELINDEXFACTORY_H
#define CHANNELINDEXFACTORY_H

#include <QObject>
#include "channelindex.h"

class ChannelIndexFactory : public QObject
{
    Q_OBJECT
public:
    explicit ChannelIndexFactory(QObject *parent = nullptr);

    Q_INVOKABLE ChannelIndex make1(int first);
    Q_INVOKABLE ChannelIndex make2(int first, unsigned second);

signals:

public slots:
};

#endif // CHANNELINDEXFACTORY_H
