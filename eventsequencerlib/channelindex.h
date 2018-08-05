#ifndef CHANNELINDEX_H
#define CHANNELINDEX_H

#include <QString>

class ChannelIndex
{
    int first_ = 0;
    int second_ = 0;
    bool hasSecond_ = false;
    ChannelIndex();
public:
    static ChannelIndex make1(int first);
    int first() const;
    bool operator<(const ChannelIndex& o) const;
    QString toDebugString() const;
};

#endif // CHANNELINDEX_H
