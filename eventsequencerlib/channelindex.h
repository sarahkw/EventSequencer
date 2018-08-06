#ifndef CHANNELINDEX_H
#define CHANNELINDEX_H

#include <QString>
#include <QMetaType>

class ChannelIndex
{
    Q_GADGET

    int first_ = 0;       // The first one is a map.
    unsigned second_ = 0; // The second one is an indexed list.
    bool hasSecond_ = false;
public:
    ChannelIndex();
    static ChannelIndex make1(int first);
    static ChannelIndex make2(int first, unsigned second);
    int first() const;
    bool hasSecond() const;
    unsigned second() const;
    bool operator<(const ChannelIndex& o) const;
    bool operator==(const ChannelIndex& o) const;
    bool operator!=(const ChannelIndex& o) const;
    QString toDebugString() const;

    Q_INVOKABLE QString toPathString();
};

Q_DECLARE_METATYPE(ChannelIndex)

#endif // CHANNELINDEX_H
