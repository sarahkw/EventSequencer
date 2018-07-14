#ifndef LABELCHANNEL_H
#define LABELCHANNEL_H

#include "channelbase.h"

#include <QObject>

namespace channel {

class LabelChannel : public ChannelBase
{
    Q_OBJECT

public:
    explicit LabelChannel(QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

signals:

public slots:
};

} // namespace channel

#endif // LABELCHANNEL_H
