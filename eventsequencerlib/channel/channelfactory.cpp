#include "channelfactory.h"

#include "badclockchannel.h"
#include "badjschannel.h"
#include "textchannel.h"

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
    default:
    case ::pb::ChannelData::CHANNEL_NOT_SET:
        qWarning() << "Unknown channel! Loading file from newer version?";
        break;
    }

    if (cb != nullptr) {
        cb->fromPb(pb);
    }

    return cb;
}
