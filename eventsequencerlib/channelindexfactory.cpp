#include "channelindexfactory.h"

ChannelIndexFactory::ChannelIndexFactory(QObject *parent) : QObject(parent)
{

}

ChannelIndex ChannelIndexFactory::make1(int first)
{
    return ChannelIndex::make1(first);
}

ChannelIndex ChannelIndexFactory::make2(int first, unsigned second)
{
    return ChannelIndex::make2(first, second);
}
