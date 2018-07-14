#include "labelstripext.h"

#include <eventsequencer.pb.h>

namespace stripext {

LabelStripExt::LabelStripExt(QObject *parent) : QObject(parent)
{

}

void LabelStripExt::toPb(pb::Strip_Label &pb) const
{
    pb.set_content(content().toStdString());
}

void LabelStripExt::fromPb(const pb::Strip_Label &pb)
{
    setContent(QString::fromStdString(pb.content()));
}

QString LabelStripExt::content() const
{
    return content_;
}

void LabelStripExt::setContent(const QString &content)
{
    if (content_ != content) {
        content_ = content;
        emit contentChanged();
    }
}

} // namespace stripext
