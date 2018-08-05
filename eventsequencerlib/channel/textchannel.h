#ifndef TEXTCHANNEL_H
#define TEXTCHANNEL_H

#include "channeltype.h"
#include "channelbase.h"

#include <QObject>

namespace channel {

class TextChannel : public ChannelBase
{
    Q_OBJECT

    QString content_;

    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)

public:
    explicit TextChannel(ChannelIndex channelIndex, Document& d, QObject *parent = nullptr);

    void toPb(pb::ChannelData& pb) const override;
    void fromPb(const pb::ChannelData& pb) override;

    ChannelType::Enum channelType() const override;

    QString content() const;
    void setContent(const QString &content);

signals:

    void contentChanged();

public slots:
};

} // namespace channel

#endif // TEXTCHANNEL_H
