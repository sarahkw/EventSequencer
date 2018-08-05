#ifndef CHANNELINDEX_H
#define CHANNELINDEX_H

#include <QString>
#include <QMetaType>

class ChannelIndex
{
    int first_ = 0;
    int second_ = 0;
    bool hasSecond_ = false;
public:
    ChannelIndex();
    static ChannelIndex make1(int first);
    int first() const;
    bool operator<(const ChannelIndex& o) const;
    bool operator==(const ChannelIndex& o) const;
    bool operator!=(const ChannelIndex& o) const;
    QString toDebugString() const;
};

Q_DECLARE_METATYPE(ChannelIndex)

#endif // CHANNELINDEX_H
