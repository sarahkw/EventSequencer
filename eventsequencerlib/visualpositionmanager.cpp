#include "visualpositionmanager.h"

VisualPositionManager::VisualPositionManager(QObject* parent) : QObject(parent)
{

}

void VisualPositionManager::setSpan(int channelIndexFirst, unsigned span)
{
    unsigned oldSpan = 0;

    auto oldSpanIter = spanMap_.find(channelIndexFirst);
    if (oldSpanIter != spanMap_.end()) {
        oldSpan = oldSpanIter->second;
        if (span == 0) {
            spanMap_.erase(oldSpanIter);
        } else {
            oldSpanIter->second = span;
        }
    } else {
        // New!
        spanMap_[channelIndexFirst] = span;
    }

    int diffSpan = static_cast<int>(span) - static_cast<int>(oldSpan);

    if (diffSpan != 0) {
        if (channelIndexFirst >= 0) {
            emit visualPositionChangedAfter(channelIndexFirst, diffSpan);
        } else {
            emit visualPositionChangedBefore(channelIndexFirst, diffSpan);
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
