#include "textchannel.h"

#include <eventsequencer.pb.h>

namespace channel {

int TextChannel::fontSize() const
{
    return fontSize_;
}

void TextChannel::setFontSize(int fontSize)
{
    if (fontSize_ != fontSize) {
        fontSize_ = fontSize;
        emit fontSizeChanged();
    }
}

TextChannel::TextChannel(QObject *parent) : ChannelBase(parent)
{

}

void TextChannel::toPb(pb::ChannelData &pb) const
{
    auto mut = pb.mutable_text();
    mut->set_fontsize(fontSize());
}

void TextChannel::fromPb(const pb::ChannelData &pb)
{
    Q_ASSERT(pb.has_text());
    setFontSize(pb.text().fontsize());
}

ChannelType::Enum TextChannel::channelType() const
{
    return ChannelType::Text;
}

} // namespace channel
