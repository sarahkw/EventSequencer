#include "visualpositionmanager.h"

#include <QDebug>

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
            if (span < oldSpan) {
                emit destroyChanIdx(ChannelIndex::make2(channelIndexFirst, span),
                                    ChannelIndex::make2(channelIndexFirst, oldSpan));
            }
            emit visualPositionChangedAfter(channelIndexFirst, diffSpan);
        } else {
            if (span < oldSpan) {
                emit destroyChanIdx(ChannelIndex::make2(channelIndexFirst, oldSpan - 1),
                                    (span > 0) ? ChannelIndex::make2(channelIndexFirst, span - 1) :
                                                 ChannelIndex::make1(channelIndexFirst));
            }
            emit visualPositionChangedBefore(channelIndexFirst, diffSpan);
        }
    }
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

    const bool ispos = chanIdx.first() >= 0;

    if (ispos) {
        auto from = spanMap_.lower_bound(0);
        auto to = spanMap_.lower_bound(chanIdx.first());
        int delta = 0;
        for (auto iter = from; iter != to; ++iter) {
            delta += iter->second;
        }

        int ret = chanIdx.first() + delta;
        if (chanIdx.hasSecond()) {
            ret += 1 + chanIdx.second();
        }
        return ret;
    } else {
        auto from = std::make_reverse_iterator(spanMap_.lower_bound(0));
        auto to = std::make_reverse_iterator(spanMap_.upper_bound(chanIdx.first()));
        int delta = 0;
        for (auto iter = from; iter != to; ++iter) {
            delta += iter->second;
        }

        int ret = chanIdx.first() - delta;
        if (chanIdx.hasSecond()) {
            ret -= 1 + chanIdx.second();
        }
        return ret;
    }
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
    if (visualPosition >= 0) {
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
    } else {
        for (auto iter = std::make_reverse_iterator(spanMap_.lower_bound(0));
             iter != spanMap_.rend(); ++iter) {
            if (visualPosition < iter->first) {
                const bool inThisRange = visualPosition >= iter->first - iter->second;
                if (inThisRange) {
                    return ChannelIndex::make2(iter->first, -(visualPosition - (iter->first - 1)));
                } else {
                    visualPosition += iter->second;
                }
            } else {
                break;
            }
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
