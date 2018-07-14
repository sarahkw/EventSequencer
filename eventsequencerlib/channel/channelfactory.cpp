#include "channelfactory.h"

#include "badclockchannel.h"
#include "badjschannel.h"
#include "textchannel.h"
#include "badaudiochannel.h"

#include <eventsequencer.pb.h>

#include <QDebug>

channel::ChannelBase *channel::ChannelFactory::Create(const pb::ChannelData &pb,
                                                      QObject *parent)
{
    ChannelBase* cb = nullptr;

    switch (pb.channel_case()) {
    case ::pb::ChannelData::kBadClock:
        cb = new channel::BadClockChannel(parent);
        break;
    case ::pb::ChannelData::kBadJs:
        cb = new channel::BadJsChannel(parent);
        break;
    case ::pb::ChannelData::kText:
        cb = new channel::TextChannel(parent);
        break;
    case ::pb::ChannelData::kBadAudio:
        cb = new channel::BadAudioChannel(parent);
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

channel::ChannelBase *channel::ChannelFactory::Create(channel::ChannelType::Enum type, QObject *parent)
{
    switch (type) {
    case channel::ChannelType::BadClock:
        return new channel::BadClockChannel(parent);
    case channel::ChannelType::BadJs:
        return new channel::BadJsChannel(parent);
    case channel::ChannelType::Text:
        return new channel::TextChannel(parent);
    case channel::ChannelType::Audio:
        return new channel::BadAudioChannel(parent);
    }

    Q_ASSERT(false);
}
