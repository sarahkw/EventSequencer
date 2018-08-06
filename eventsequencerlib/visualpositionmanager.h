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

    void visualPositionChangedAfter(int channelIndexFirst, int delta);
    void visualPositionChangedBefore(int channelIndexFirst, int delta);

    // Destroy *before* reclaiming the space used. [from, to]
    void destroyChanIdx(ChannelIndex from, ChannelIndex to);

public slots:

};

#endif // VISUALPOSITIONMANAGER_H
