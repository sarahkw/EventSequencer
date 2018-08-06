#include "visualpositionmanager.h"

VisualPositionManager::VisualPositionManager(QObject* parent) : QObject(parent)
{

}

void VisualPositionManager::setSpan(int channelIndexFirst, int span)
{
    if (span <= 0) {
        spanMap_.erase(channelIndexFirst);
    } else {
        spanMap_[channelIndexFirst] = span;
    }
}

void VisualPositionManager::del(int channelIndexFirst)
{
    setSpan(channelIndexFirst, 0);
}

int VisualPositionManager::chanIdxToVisualPosition(int chanIdx)
{
    return 0;
}

int VisualPositionManager::visualPositionToChanIdx(int visualPosition)
{
    return 0;
}

const std::map<int, int> &VisualPositionManager::spanMap() const
{
    return spanMap_;
}
