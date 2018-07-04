#include "badjs.h"

#include "../eventsequencer.pb.h"

namespace stripext {

QString BadJs::script() const
{
    return script_;
}

void BadJs::setScript(const QString &script)
{
    if (script_ != script) {
        script_ = script;
        emit scriptChanged();
    }
}

BadJs::BadJs(QObject *parent) : QObject(parent)
{

}

void BadJs::toPb(pb::Strip_BadJs &pb) const
{
    pb.set_script(script().toStdString());
}

void BadJs::fromPb(const pb::Strip_BadJs &pb)
{
    setScript(QString::fromStdString(pb.script()));
}

} // namespace stripext
