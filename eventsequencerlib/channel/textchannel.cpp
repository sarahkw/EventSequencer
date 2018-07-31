#include "textchannel.h"

#include <eventsequencer.pb.h>

namespace channel {

QString TextChannel::content() const
{
    return content_;
}

void TextChannel::setContent(const QString &content)
{
    if (content_ != content) {
        content_ = content;
        emit contentChanged();
    }
}

TextChannel::TextChannel(int channelIndex, Document& d, QObject* parent)
    : ChannelBase(channelIndex, d, parent)
{

}

void TextChannel::toPb(pb::ChannelData &pb) const
{
    auto mut = pb.mutable_text();
    mut->set_content(content().toStdString());
}

void TextChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_text());
    setContent(QString::fromStdString(pb.text().content()));
}

ChannelType::Enum TextChannel::channelType() const
{
    return ChannelType::Text;
}

} // namespace channel
