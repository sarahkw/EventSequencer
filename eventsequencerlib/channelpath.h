#ifndef CHANNELPATH_H
#define CHANNELPATH_H

#include <QString>

class ChannelPath
{
    int first_ = 0;
    int second_ = 0;
    bool hasSecond_ = false;
    ChannelPath();
public:
    static ChannelPath make1(int first);
    int first() const;
    bool operator<(const ChannelPath& o) const;
    QString toDebugString() const;
};

#endif // CHANNELPATH_H
