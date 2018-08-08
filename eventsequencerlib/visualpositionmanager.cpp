#include "visualpositionmanager.h"

#include <QDebug>

VisualPositionManager::VisualPositionManager(QObject* parent) : QObject(parent)
{

}

void VisualPositionManager::setSpan(int channelIndexFirst, unsigned span)
{
    if (channelIndexFirst < 0) {
        qWarning("Spanning is only supported for index >= 0!");
        return;
    }

    // We want to emit destoryChanIdx() before actually modifying the structure.
    // Because destroying the channel may need to know where its visual position
    // is. For example, to "unbind" itself to a visual position.
    std::function<void()> modifyStructure;

    std::function<void()> destroyChannels = [](){};

    int delta = 0;

    ChannelIndex affectedAfter;

    if (spanMap_.count(channelIndexFirst)) {
        auto oldSpan = spanMap_[channelIndexFirst];
        if (span == 0) {
            modifyStructure = [=]() { spanMap_.erase(channelIndexFirst); };
            affectedAfter = ChannelIndex::make1(channelIndexFirst);
            destroyChannels = [=]() {
                emit destroyChanIdx(ChannelIndex::make2(channelIndexFirst, span),
                                    ChannelIndex::make2(channelIndexFirst, oldSpan));
            };
            delta = -oldSpan;
        } else {
            modifyStructure = [=]() { spanMap_[channelIndexFirst] = span; };
            if (span > oldSpan) { // Increase
                affectedAfter = ChannelIndex::make2(channelIndexFirst, oldSpan - 1);
            } else if (span < oldSpan) { // Decrease
                affectedAfter = ChannelIndex::make2(channelIndexFirst, span - 1);
                destroyChannels = [=]() {
                    emit destroyChanIdx(ChannelIndex::make2(channelIndexFirst, span),
                                        ChannelIndex::make2(channelIndexFirst, oldSpan));
                };
            } else {
                return;                                          // EARLY RETURN
            }
            delta = span - oldSpan;
        }
    } else {
        modifyStructure = [=]() { spanMap_[channelIndexFirst] = span; };
        affectedAfter = ChannelIndex::make1(channelIndexFirst);
        delta = span;
    }

    destroyChannels();
    modifyStructure();
    emit visualPositionChangedAfter(affectedAfter, delta);
}

void VisualPositionManager::del(int channelIndexFirst)
{
    setSpan(channelIndexFirst, 0);
}

int VisualPositionManager::chanIdxToVisualPosition(ChannelIndex chanIdx) const
{
    // We'll get bad visual results but I don't think it's worth ASSERTing over.
    // XXX Maybe if we had a soft assert...
    if (!chanIdxIsValid(chanIdx)) {
        qWarning() << "Channel index not valid!" << chanIdx.toDebugString();
    }

    int delta = 0;
    if (chanIdx.first() >= 0) {
        auto from = spanMap_.lower_bound(0);
        auto to = spanMap_.lower_bound(chanIdx.first());
        for (auto iter = from; iter != to; ++iter) {
            delta += iter->second;
        }
    }

    int ret = chanIdx.first() + delta;
    if (chanIdx.hasSecond()) {
        ret += 1 + chanIdx.second();
    }
    return ret;
}

bool VisualPositionManager::chanIdxIsValid(ChannelIndex chanIdx) const
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

ChannelIndex VisualPositionManager::visualPositionToChanIdx(int visualPosition) const
{
    for (auto iter = spanMap_.lower_bound(0); iter != spanMap_.end(); ++iter) {
        if (visualPosition > iter->first) {
            const bool inThisRange = visualPosition <= iter->first + iter->second;
            if (inThisRange) {
                return ChannelIndex::make2(iter->first, visualPosition - (1 + iter->first));
            } else {
                visualPosition -= iter->second;
            }
        } else {
            break;
        }
    }

    return ChannelIndex::make1(visualPosition);
}

const std::map<int, unsigned> &VisualPositionManager::spanMap() const
{
    return spanMap_;
}

void VisualPositionManager::setSpan(ChannelIndex channelIndex, unsigned span)
{
    if (channelIndex.hasSecond()) {
        qWarning() << "Setting Span for subchannel not supported!";
        return;
    }
    setSpan(channelIndex.first(), span);
}
