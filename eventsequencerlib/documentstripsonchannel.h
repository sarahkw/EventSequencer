#ifndef DOCUMENTSTRIPSONCHANNEL_H
#define DOCUMENTSTRIPSONCHANNEL_H

#include <QObject>
#include <set>
#include "channelindex.h"

class Strip;

class DocumentStripsOnChannel : public QObject
{
    Q_OBJECT

public:
    struct StripHolder {
        int startFrame;
        const Strip* strip;
    };
    struct Compare {
        bool operator()(const StripHolder& a, const StripHolder& b) const;
    };
    using StripSet = std::set<StripHolder, Compare>;
    using ChannelStripsMap = std::map<ChannelIndex, StripSet>;
private:
    ChannelStripsMap data_;

public:

    explicit DocumentStripsOnChannel(QObject *parent = nullptr);

    const StripSet* stripsForChannel(ChannelIndex channelIndex) const;

    std::pair<ChannelStripsMap::const_iterator,
              ChannelStripsMap::const_iterator>
    stripsBetweenChannels(ChannelIndex from, ChannelIndex toExclusive) const;

signals:

    void channelStripSetChanged(ChannelIndex channelIndex);
    void channelStripLocationChanged(ChannelIndex channelIndex, Strip* whichStrip);

public slots:
    void stripAfterPlaced(Strip* strip);
    void stripBeforeDelete(Strip* strip);
    void stripMoved(Strip* strip,
                    ChannelIndex previousChannelIndex,
                    int previousStartFrame,
                    int previousLength);
};

#endif // DOCUMENTSTRIPSONCHANNEL_H
