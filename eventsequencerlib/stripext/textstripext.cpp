#include "textstripext.h"

#include <eventsequencer.pb.h>

namespace stripext {

TextStripExt::TextStripExt(QObject *parent) : QObject(parent)
{

}

void TextStripExt::toPb(pb::Strip_Text &pb) const
{
    pb.set_content(content().toStdString());
}

void TextStripExt::fromPb(const pb::Strip_Text &pb)
{
    setContent(QString::fromStdString(pb.content()));
}

QString TextStripExt::content() const
{
    return content_;
}

void TextStripExt::setContent(const QString &content)
{
    if (content_ != content) {
        content_ = content;
        emit contentChanged();
    }
}

} // namespace stripext
