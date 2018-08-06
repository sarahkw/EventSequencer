#include "visualpositionmanager.h"

VisualPositionManager::VisualPositionManager(QObject* parent) : QObject(parent)
{

}

void VisualPositionManager::setSpan(int channelIndexFirst, unsigned span)
{
    if (span == 0) {
        spanMap_.erase(channelIndexFirst);
    } else {
        spanMap_[channelIndexFirst] = span;
    }

    if (span > 0) {
        if (channelIndexFirst >= 0) {
            emit visualPositionChangedAfter(channelIndexFirst, static_cast<int>(span));
        } else {
            emit visualPositionChangedBefore(channelIndexFirst, static_cast<int>(span));
        }
    }
}

void VisualPositionManager::del(int channelIndexFirst)
{
    setSpan(channelIndexFirst, 0);
}

int VisualPositionManager::chanIdxToVisualPosition(ChannelIndex chanIdx)
{
    return 0;
}

bool VisualPositionManager::chanIdxIsValid(ChannelIndex chanIdx)
{
    if (!chanIdx.hasSecond()) {
        return true;
    } else {
        auto it = spanMap_.find(chanIdx.first());
        if (it != spanMap_.end()) {
            return chanIdx.second() < it->second;
        } else {
            return false;
        }
    }
}

int VisualPositionManager::visualPositionToChanIdx(int visualPosition)
{
    return 0;
}

const std::map<int, unsigned> &VisualPositionManager::spanMap() const
{
    return spanMap_;
}
