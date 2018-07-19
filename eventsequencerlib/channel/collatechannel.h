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

    int channelFrom_ = 0;
    int channelTo_ = 0;

    Q_PROPERTY(int channelFrom READ channelFrom WRITE setChannelFrom NOTIFY channelFromChanged)
    Q_PROPERTY(int channelTo READ channelTo WRITE setChannelTo NOTIFY channelToChanged)

public:
    explicit CollateChannel(Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

    int channelFrom() const;
    void setChannelFrom(int channelFrom);

    int channelTo() const;
    void setChannelTo(int channelTo);

signals:

    void channelFromChanged();
    void channelToChanged();

private slots:

    void stripMoved(Strip* strip,
                    int previousChannel,
                    int previousStartFrame,
                    int previousLength);

public slots:
};

} // namespace channel

#endif // COLLATECHANNEL_H
