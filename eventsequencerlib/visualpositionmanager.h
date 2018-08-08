#ifndef VISUALPOSITIONMANAGER_H
#define VISUALPOSITIONMANAGER_H

#include "channelindex.h"

#include <QObject>

#include <map>

class VisualPositionManager : public QObject
{
    Q_OBJECT

    // chanIdx -> span
    std::map<int, unsigned> spanMap_;

public:
    VisualPositionManager(QObject* parent = nullptr);

    void setSpan(int channelIndexFirst, unsigned span);
    void del(int channelIndexFirst);

    int chanIdxToVisualPosition(ChannelIndex chanIdx) const;
    bool chanIdxIsValid(ChannelIndex chanIdx) const;
    ChannelIndex visualPositionToChanIdx(int visualPosition) const;

    const std::map<int, unsigned>& spanMap() const; // Needed to make placeholders

signals:

    void visualPositionChangedAfter(ChannelIndex channelIndex, int delta);

    // Destroy *before* reclaiming the space used. [from, to)
    void destroyChanIdx(ChannelIndex from, ChannelIndex toExclusive);

public slots:

    void setSpan(ChannelIndex channelIndex, unsigned span);

};

#endif // VISUALPOSITIONMANAGER_H
