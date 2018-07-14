#ifndef BADAUDIOCHANNEL_H
#define BADAUDIOCHANNEL_H

#include "channelbase.h"
#include "nodatapb.h"
#include <eventsequencer.pb.h>

#include <QObject>

namespace channel {

class BadAudioChannel : public ChannelBase,
        public NoDataPb<pb::ChannelData_BadAudio,
        &pb::ChannelData::mutable_badaudio,
        &pb::ChannelData::has_badaudio>
{
    Q_OBJECT

public:
    explicit BadAudioChannel(QObject *parent = nullptr);

    virtual ChannelType::Enum channelType() const override;

signals:

public slots:
};

} // namespace channel

#endif // BADAUDIOCHANNEL_H
