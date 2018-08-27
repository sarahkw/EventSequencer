#ifndef DOCFILLCHANNEL_H
#define DOCFILLCHANNEL_H

#include "channelbase.h"

namespace channel {

class DocFillChannel : public ChannelBase
{
    Q_OBJECT

    ChannelIndex textChannel_;
    ChannelIndex resourceChannel_;

    Q_PROPERTY(ChannelIndex textChannel READ textChannel WRITE setTextChannel NOTIFY textChannelChanged)
    Q_PROPERTY(ChannelIndex resourceChannel READ resourceChannel WRITE setResourceChannel NOTIFY resourceChannelChanged)

    bool attemptExpansionOfResourceChannel_ = false;
    Q_PROPERTY(bool attemptExpansionOfResourceChannel
               READ attemptExpansionOfResourceChannel
               WRITE setAttemptExpansionOfResourceChannel
               NOTIFY attemptExpansionOfResourceChannelChanged)

public:
    explicit DocFillChannel(ChannelIndex channelIndex, Document& d, QObject *parent = nullptr);

    virtual void toPb(pb::ChannelData& pb) const override;
    virtual void fromPb(const pb::ChannelData& pb) override;

    virtual ChannelType::Enum channelType() const override;

    ChannelIndex textChannel() const;
    void setTextChannel(const ChannelIndex &textChannel);

    ChannelIndex resourceChannel() const;
    void setResourceChannel(const ChannelIndex &resourceChannel);

    bool attemptExpansionOfResourceChannel() const;
    void setAttemptExpansionOfResourceChannel(bool attemptExpansionOfResourceChannel);

signals:

    void textChannelChanged();
    void resourceChannelChanged();
    void attemptExpansionOfResourceChannelChanged();

public slots:
};

} // namespace channel


#endif // DOCFILLCHANNEL_H
