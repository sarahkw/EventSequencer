#include "channelfactory.h"

#include "badclockchannel.h"
#include "badjschannel.h"
#include "textchannel.h"
#include "audiochannel.h"
#include "labelchannel.h"
#include "collatechannel.h"
#include "playlistchannel.h"
#include "spanchannel.h"

#include <eventsequencer.pb.h>

#include <QDebug>

channel::ChannelBase *channel::ChannelFactory::Create(const pb::ChannelData &pb,
                                                      ChannelIndex channelIndex,
                                                      Document& d,
                                                      QObject *parent)
{
    ChannelBase* cb = nullptr;

    switch (pb.channel_case()) {
    case ::pb::ChannelData::kBadClock:
        cb = new channel::BadClockChannel(channelIndex, d, parent);
        break;
    case ::pb::ChannelData::kBadJs:
        cb = new channel::BadJsChannel(channelIndex, d, parent);
        break;
    case ::pb::ChannelData::kText:
        cb = new channel::TextChannel(channelIndex, d, parent);
        break;
    case ::pb::ChannelData::kAudio:
        cb = new channel::AudioChannel(channelIndex, d, parent);
        break;
    case ::pb::ChannelData::kLabel:
        cb = new channel::LabelChannel(channelIndex, d, parent);
        break;
    case ::pb::ChannelData::kCollate:
        cb = new channel::CollateChannel(channelIndex, d, parent);
        break;
    case ::pb::ChannelData::kPlaylist:
        cb = new channel::PlaylistChannel(channelIndex, d, parent);
        break;
    case ::pb::ChannelData::kSpan:
        cb = new channel::SpanChannel(channelIndex, d, parent);
        break;
    case ::pb::ChannelData::CHANNEL_NOT_SET:
        qWarning() << "Unknown channel! Loading file from newer version?";
        break;
    }

    if (cb != nullptr) {
        cb->fromPb(pb);
    }

    return cb;
}

channel::ChannelBase *channel::ChannelFactory::Create(channel::ChannelType::Enum type,
                                                      ChannelIndex channelIndex,
                                                      Document& d,
                                                      QObject *parent)
{
    switch (type) {
    case channel::ChannelType::BadClock:
        return new channel::BadClockChannel(channelIndex, d, parent);
    case channel::ChannelType::BadJs:
        return new channel::BadJsChannel(channelIndex, d, parent);
    case channel::ChannelType::Text:
        return new channel::TextChannel(channelIndex, d, parent);
    case channel::ChannelType::Audio:
        return new channel::AudioChannel(channelIndex, d, parent);
    case channel::ChannelType::Label:
        return new channel::LabelChannel(channelIndex, d, parent);
    case channel::ChannelType::Collate:
        return new channel::CollateChannel(channelIndex, d, parent);
    case channel::ChannelType::Playlist:
        return new channel::PlaylistChannel(channelIndex, d, parent);
    case channel::ChannelType::Span:
        return new channel::SpanChannel(channelIndex, d, parent);
    }

    Q_ASSERT(false);
}
