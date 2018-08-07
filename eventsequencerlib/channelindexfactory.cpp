#include "channelindexfactory.h"

ChannelIndexFactory::ChannelIndexFactory(QObject *parent) : QObject(parent)
{

}

QVariant ChannelIndexFactory::make1(int first)
{
    return QVariant::fromValue(ChannelIndex::make1(first));
}

QVariant ChannelIndexFactory::make2(int first, unsigned second)
{
    return QVariant::fromValue(ChannelIndex::make2(first, second));
}

QVariant ChannelIndexFactory::makeFromPathString(QString pathString)
{
    bool success;
    auto ci = ChannelIndex::makeFromPathString(pathString, &success);
    if (success) {
        return QVariant::fromValue(ci);
    } else {
        return QVariant();
    }
}
