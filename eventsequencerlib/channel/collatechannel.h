#ifndef COLLATECHANNEL_H
#define COLLATECHANNEL_H

#include "channelbase.h"

#include <QObject>

class Document;
class Strip;

namespace channel {

class CollateChannel : public ChannelBase
{
    Q_OBJECT

    Document& d_;

public:
    explicit CollateChannel(Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

signals:

private slots:

    void stripMoved(Strip* strip,
                    int previousChannel,
                    int previousStartFrame,
                    int previousLength);

public slots:
};

} // namespace channel

#endif // COLLATECHANNEL_H
