#ifndef VISUALPOSITIONMANAGER_H
#define VISUALPOSITIONMANAGER_H

#include "channelindex.h"

#include <QObject>

#include <map>

class VisualPositionManager : public QObject
{
    Q_OBJECT

    // chanIdx -> span
    std::map<int, int> spanMap_;

public:
    VisualPositionManager(QObject* parent = nullptr);

    void setSpan(int channelIndexFirst, int span);
    void del(int channelIndexFirst);

    int chanIdxToVisualPosition(ChannelIndex chanIdx);
    bool chanIdxIsValid(ChannelIndex chanIdx);
    int visualPositionToChanIdx(int visualPosition);

    const std::map<int, int>& spanMap() const; // Needed to make placeholders

signals:

    void visualPositionChangedAfter(int channelIndexFirst, int delta);
    void visualPositionChangedBefore(int channelIndexFirst, int delta);

public slots:

};

#endif // VISUALPOSITIONMANAGER_H
