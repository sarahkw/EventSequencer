#include "textchannel.h"

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

TextChannel::TextChannel(QObject *parent) : QObject(parent)
{

}

ChannelType::Enum TextChannel::channelType() const
{
    return ChannelType::Text;
}

} // namespace channel
