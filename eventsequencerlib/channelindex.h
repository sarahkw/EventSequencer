#ifndef CHANNELINDEX_H
#define CHANNELINDEX_H

#include <QString>
#include <QMetaType>

namespace pb {
class ChannelIndex;
}

class ChannelIndex
{
    Q_GADGET

    int first_ = 0;       // The first one is a map.
    unsigned second_ = 0; // The second one is an indexed list.
    bool hasSecond_ = false;

    Q_PROPERTY(int first READ first CONSTANT)
    Q_PROPERTY(unsigned second READ second CONSTANT)
    Q_PROPERTY(bool hasSecond READ hasSecond CONSTANT)

public:
    ChannelIndex();
    static ChannelIndex make1(int first);
    static ChannelIndex make2(int first, unsigned second);
    static ChannelIndex makeFromPathString(QString pathString, bool* success);
    static ChannelIndex makeFromPb(const pb::ChannelIndex& pb);

    void toPb(pb::ChannelIndex& pb) const;

    int first() const;
    bool hasSecond() const;
    unsigned second() const;
    bool operator<(const ChannelIndex& o) const;
    bool operator==(const ChannelIndex& o) const;
    bool operator>=(const ChannelIndex& o) const;
    bool operator!=(const ChannelIndex& o) const;
    QString toDebugString() const;

    Q_INVOKABLE QString toPathString() const;
};

Q_DECLARE_METATYPE(ChannelIndex)

#endif // CHANNELINDEX_H
