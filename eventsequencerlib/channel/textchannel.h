#ifndef TEXTCHANNEL_H
#define TEXTCHANNEL_H

#include "channeltype.h"
#include "channelbase.h"

#include <QObject>

namespace channel {

class TextChannel : public ChannelBase
{
    Q_OBJECT

    Q_PROPERTY(channel::ChannelType::Enum channelType READ channelType CONSTANT)

    int fontSize_ = 10;

    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)

public:
    explicit TextChannel(QObject *parent = nullptr);

    void toPb(pb::ChannelData& pb) const override;

    ChannelType::Enum channelType() const;

    int fontSize() const;
    void setFontSize(int fontSize);

signals:

    void fontSizeChanged();

public slots:
};

} // namespace channel

#endif // TEXTCHANNEL_H
