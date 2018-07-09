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
    pb.mutable_text()->set_fontsize(fontSize());
}

ChannelType::Enum TextChannel::channelType() const
{
    return ChannelType::Text;
}

} // namespace channel
