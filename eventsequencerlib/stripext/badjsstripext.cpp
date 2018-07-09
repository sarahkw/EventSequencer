#include "badjsstripext.h"

#include "../eventsequencer.pb.h"

namespace stripext {

QString BadJsStripExt::script() const
{
    return script_;
}

void BadJsStripExt::setScript(const QString &script)
{
    if (script_ != script) {
        script_ = script;
        emit scriptChanged();
    }
}

BadJsStripExt::BadJsStripExt(QObject *parent) : QObject(parent)
{

}

void BadJsStripExt::toPb(pb::Strip_BadJs &pb) const
{
    pb.set_script(script().toStdString());
}

void BadJsStripExt::fromPb(const pb::Strip_BadJs &pb)
{
    setScript(QString::fromStdString(pb.script()));
}

} // namespace stripext
